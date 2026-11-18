#include "services/ParkingSystem.hpp"

#include "domain/Types.hpp"
#include "ds/LinkedStack.hpp"
#include "ds/MergeSort.hpp"
#include "io/DataLoader.hpp"
#include "services/FuzzySearch.hpp"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <type_traits>
#include <utility>

namespace parking {

ParkingSystem::ParkingSystem()
    : sessionsByCard_(23), cardByPlate_(23), feeCalculator_(FeePolicy{}) {}

void ParkingSystem::loadMap(const std::string& nodesFile, const std::string& edgesFile) {
    if (!sessionsByCard_.empty() || waitingCount() != 0 || pendingRelocation_.has_value()) {
        throw std::logic_error("Khong duoc thay ban do khi he thong dang co xe");
    }
    DataLoader::loadParkingMap(graph_, nodesFile, edgesFile);
}

void ParkingSystem::loadFeePolicy(const std::string& feeFile) {
    feeCalculator_.setPolicy(DataLoader::loadFeePolicy(feeFile));
}

ds::LinkedQueue<VehicleRequest>& ParkingSystem::waitingQueue(UserRole role,
                                                              VehicleType type) {
    if (role == UserRole::Lecturer) {
        return type == VehicleType::Car ? waitingLecturerCars_
                                        : waitingLecturerMotorcycles_;
    }
    return type == VehicleType::Car ? waitingStudentCars_ : waitingStudentMotorcycles_;
}

const ds::LinkedQueue<VehicleRequest>& ParkingSystem::waitingQueue(
    UserRole role,
    VehicleType type) const {
    if (role == UserRole::Lecturer) {
        return type == VehicleType::Car ? waitingLecturerCars_
                                        : waitingLecturerMotorcycles_;
    }
    return type == VehicleType::Car ? waitingStudentCars_ : waitingStudentMotorcycles_;
}

ds::LinkedQueue<std::string>& ParkingSystem::holdingQueue(VehicleType type) {
    return type == VehicleType::Car ? holdingCars_ : holdingMotorcycles_;
}

const ds::LinkedQueue<std::string>& ParkingSystem::holdingQueue(VehicleType type) const {
    return type == VehicleType::Car ? holdingCars_ : holdingMotorcycles_;
}

ParkingSystem::OverflowHeap& ParkingSystem::overflowHeap(VehicleType type) {
    return type == VehicleType::Car ? overflowCars_ : overflowMotorcycles_;
}

bool ParkingSystem::isQueued(const std::string& cardId, const std::string& plate) const {
    const auto matches = [&](const VehicleRequest& request) {
        return request.cardId == cardId || request.plate == plate;
    };
    if (waitingStudentMotorcycles_.any(matches) || waitingStudentCars_.any(matches) ||
        waitingLecturerMotorcycles_.any(matches) || waitingLecturerCars_.any(matches)) {
        return true;
    }
    return pendingRelocation_.has_value() &&
           (pendingRelocation_->lecturerRequest.cardId == cardId ||
            pendingRelocation_->lecturerRequest.plate == plate);
}

bool ParkingSystem::hasLecturerWaiting(VehicleType type) const {
    const bool hasPendingSameType =
        pendingRelocation_.has_value() &&
        pendingRelocation_->lecturerRequest.vehicleType == type;
    return hasPendingSameType || !waitingQueue(UserRole::Lecturer, type).empty();
}

RouteResult ParkingSystem::routeForZone(const VehicleRequest& request, Zone zone) const {
    return graph_.shortestPathToNearestSlot(request.gateId, zone, request.vehicleType);
}

CheckInResult ParkingSystem::parkRequest(const VehicleRequest& request,
                                         const RouteResult& route,
                                         bool overflow) {
    CheckInResult result;
    result.route = route;
    result.slotId = route.destinationSlotId;
    result.overflow = overflow;
    if (!route.found || route.destinationSlotId.empty()) {
        result.message = "Khong co duong den o do phu hop";
        return result;
    }
    if (!graph_.occupySlot(route.destinationSlotId, request.cardId)) {
        result.message = "O do vua duoc cap cho xe khac";
        return result;
    }

    ParkingSession session;
    session.cardId = request.cardId;
    session.plate = request.plate;
    session.role = request.role;
    session.vehicleType = request.vehicleType;
    session.gateId = request.gateId;
    session.entryTime = request.requestedAt;
    session.slotId = route.destinationSlotId;
    session.status = SessionStatus::Parked;
    session.overflow = overflow;
    session.sequence = request.sequence;

    if (!sessionsByCard_.insert(session.cardId, session)) {
        graph_.releaseSlot(route.destinationSlotId, request.cardId);
        result.message = "Ma the da ton tai trong Hash Table";
        return result;
    }
    if (!cardByPlate_.insert(session.plate, session.cardId)) {
        sessionsByCard_.erase(session.cardId);
        graph_.releaseSlot(route.destinationSlotId, request.cardId);
        result.message = "Bien so da ton tai trong Hash Table";
        return result;
    }

    if (overflow) {
        overflowHeap(request.vehicleType)
            .push(OverflowEntry{session.entryTime,
                                session.sequence,
                                session.cardId,
                                session.slotId,
                                session.vehicleType});
    }

    result.status = CheckInStatus::Parked;
    result.message = overflow
                         ? "Da cap o tam trong khu Giang vien"
                         : "Check-in thanh cong";
    appendHistory(request.requestedAt,
                  overflow ? "CHECK_IN_OVERFLOW" : "CHECK_IN",
                  request.cardId,
                  request.plate,
                  {},
                  route.destinationSlotId);
    return result;
}

CheckInResult ParkingSystem::enqueueRequest(const VehicleRequest& request) {
    ds::LinkedQueue<VehicleRequest>& queue = waitingQueue(request.role, request.vehicleType);
    queue.enqueue(request);

    CheckInResult result;
    result.status = CheckInStatus::Queued;
    result.queuePosition = queue.size();
    result.message = "Bai het cho phu hop; xe da vao Queue FIFO";
    appendHistory(request.requestedAt,
                  "WAITING_QUEUE",
                  request.cardId,
                  request.plate,
                  {},
                  {});
    return result;
}

CheckInResult ParkingSystem::checkIn(VehicleRequest request) {
    CheckInResult rejected;
    request.cardId = normalizeCardId(request.cardId);
    request.plate = normalizePlate(request.plate);
    request.gateId = trim(request.gateId);

    if (request.cardId.empty()) {
        rejected.message = "Ma the khong duoc rong";
        return rejected;
    }
    if (!isValidNormalizedPlate(request.plate)) {
        rejected.message = "Bien so sau chuan hoa phai gom 6-12 ky tu chu/so";
        return rejected;
    }
    if (request.vehicleType == VehicleType::Any) {
        rejected.message = "Phai chon XE MAY hoac O TO";
        return rejected;
    }
    const ParkingNode* gate = graph_.findNode(request.gateId);
    if (gate == nullptr || gate->type != NodeType::Gate || !gate->enabled) {
        rejected.message = "Cong vao khong ton tai hoac bi khoa";
        return rejected;
    }
    if (sessionsByCard_.contains(request.cardId)) {
        rejected.message = "Ma the dang co xe trong bai";
        return rejected;
    }
    if (cardByPlate_.contains(request.plate)) {
        rejected.message = "Bien so da co trong bai";
        return rejected;
    }
    if (isQueued(request.cardId, request.plate)) {
        rejected.message = "Xe da nam trong Queue hoac ke hoach di doi";
        return rejected;
    }

    request.sequence = nextSequence_++;
    if (request.role == UserRole::Student) {
        RouteResult route = routeForZone(request, Zone::Student);
        if (route.found) {
            return parkRequest(request, route, false);
        }

        // Không lấy chỗ khu GV nếu đang có GV cùng loại xe chờ trước cổng.
        if (!hasLecturerWaiting(request.vehicleType)) {
            route = routeForZone(request, Zone::Lecturer);
            if (route.found) {
                return parkRequest(request, route, true);
            }
        }
        return enqueueRequest(request);
    }

    RouteResult route = routeForZone(request, Zone::Lecturer);
    if (route.found) {
        return parkRequest(request, route, false);
    }

    // Giữ FIFO giữa các Giảng viên: người đến sau không được tạo plan và vượt
    // một yêu cầu cùng loại xe đã nằm trong Queue từ trước.
    if (!waitingQueue(UserRole::Lecturer, request.vehicleType).empty()) {
        return enqueueRequest(request);
    }

    if (!pendingRelocation_.has_value()) {
        std::optional<RelocationPlan> plan = createRelocationPlan(request);
        if (plan.has_value()) {
            pendingRelocation_ = plan;
            CheckInResult result;
            result.status = CheckInStatus::RelocationRequired;
            result.message = "Can di doi xe Sinh vien do tran som nhat";
            result.relocation = plan;
            appendHistory(request.requestedAt,
                          "RELOCATION_PLAN_CREATED",
                          request.cardId,
                          request.plate,
                          {},
                          plan->targetSlotId);
            return result;
        }
    }
    return enqueueRequest(request);
}

std::optional<OverflowEntry> ParkingSystem::peekValidOverflow(
    VehicleType type,
    const std::string& gateId) {
    OverflowHeap& heap = overflowHeap(type);
    std::vector<OverflowEntry> unreachableEntries;
    std::optional<OverflowEntry> candidate;
    while (!heap.empty()) {
        const OverflowEntry* entry = heap.top();
        if (entry == nullptr) {
            break;
        }
        const ParkingSession* session = sessionsByCard_.find(entry->cardId);
        const ParkingNode* slot = graph_.findNode(entry->slotId);
        if (session != nullptr && slot != nullptr && session->status == SessionStatus::Parked &&
            session->overflow && session->vehicleType == type &&
            session->slotId == entry->slotId && slot->zone == Zone::Lecturer &&
            slot->occupiedCardId == entry->cardId) {
            if (graph_.shortestPathToNode(gateId, entry->slotId).found) {
                // Không pop ứng viên được chọn. Nếu kế hoạch bị hủy, entry vẫn
                // ở Heap; nếu di dời thành công, nó tự thành stale.
                candidate = *entry;
                break;
            }

            // Root hợp lệ nhưng ở component khác. Tạm lấy ra để xét ứng viên
            // kế tiếp theo đúng thứ tự Heap, rồi chèn lại trước khi trả kết quả.
            std::optional<OverflowEntry> unreachable = heap.pop();
            if (unreachable.has_value()) {
                unreachableEntries.push_back(std::move(*unreachable));
            }
            continue;
        }
        (void)heap.pop();  // Chỉ loại entry cũ sau checkout/di dời.
    }
    for (OverflowEntry& entry : unreachableEntries) {
        heap.push(std::move(entry));
    }
    return candidate;
}

void ParkingSystem::buildBlockerOrders(const ParkingNode& targetSlot,
                                       std::vector<BlockerMove>& removalOrder,
                                       std::vector<BlockerMove>& restoreOrder) const {
    removalOrder.clear();
    restoreOrder.clear();
    if (!targetSlot.laneId.empty()) {
        for (const ParkingNode& node : graph_.nodes()) {
            if (node.type != NodeType::Slot || node.occupiedCardId.empty() ||
                node.laneId != targetSlot.laneId ||
                node.depthFromExit >= targetSlot.depthFromExit) {
                continue;
            }
            const ParkingSession* blocker = sessionsByCard_.find(node.occupiedCardId);
            removalOrder.push_back(BlockerMove{node.occupiedCardId,
                                               blocker == nullptr ? std::string{}
                                                                  : blocker->plate,
                                               node.id,
                                               node.depthFromExit});
        }
    }
    ds::mergeSort(removalOrder, [](const BlockerMove& left, const BlockerMove& right) {
        if (left.depthFromExit != right.depthFromExit) {
            return left.depthFromExit < right.depthFromExit;
        }
        return left.slotId < right.slotId;
    });

    // Xe chắn được đưa ra từ gần cửa đến sâu hơn và push vào Stack.
    // Sau khi lấy xe mục tiêu, pop tạo đúng thứ tự hoàn nguyên sâu -> gần.
    ds::LinkedStack<BlockerMove> temporaryStack;
    for (const BlockerMove& blocker : removalOrder) {
        temporaryStack.push(blocker);
    }
    while (!temporaryStack.empty()) {
        std::optional<BlockerMove> blocker = temporaryStack.pop();
        if (blocker.has_value()) {
            restoreOrder.push_back(std::move(*blocker));
        }
    }
}

std::optional<RelocationPlan> ParkingSystem::createRelocationPlan(
    const VehicleRequest& lecturer) {
    const std::optional<OverflowEntry> candidate =
        peekValidOverflow(lecturer.vehicleType, lecturer.gateId);
    if (!candidate.has_value()) {
        return std::nullopt;
    }

    const ParkingSession* targetSession = sessionsByCard_.find(candidate->cardId);
    const ParkingNode* targetSlot = graph_.findNode(candidate->slotId);
    if (targetSession == nullptr || targetSlot == nullptr) {
        return std::nullopt;
    }

    std::vector<BlockerMove> blockers;
    std::vector<BlockerMove> restoreOrder;
    buildBlockerOrders(*targetSlot, blockers, restoreOrder);

    std::ostringstream planId;
    planId << "MOVE-" << std::setw(4) << std::setfill('0') << nextPlanId_++;
    return RelocationPlan{planId.str(),
                          lecturer,
                          candidate->cardId,
                          targetSession->plate,
                          candidate->slotId,
                          std::move(blockers),
                          std::move(restoreOrder)};
}

RelocationResult ParkingSystem::confirmRelocation(const std::string& planId,
                                                  Timestamp confirmedAt) {
    RelocationResult result;
    if (!pendingRelocation_.has_value() ||
        toUpperAscii(trim(planId)) != toUpperAscii(pendingRelocation_->planId)) {
        result.message = "Khong tim thay ke hoach di doi dang cho";
        return result;
    }

    const RelocationPlan plan = *pendingRelocation_;
    const VehicleRequest lecturer = plan.lecturerRequest;
    if (confirmedAt < lecturer.requestedAt) {
        result.message = "Thoi gian xac nhan khong duoc truoc luc Giang vien den";
        return result;
    }
    if (sessionsByCard_.contains(lecturer.cardId) || cardByPlate_.contains(lecturer.plate)) {
        result.message = "The hoac bien so cua Giang vien da hoat dong";
        return result;
    }

    // Trong thời gian người dùng xem kế hoạch, có thể một ô GV khác vừa trống.
    // Khi đó cấp trực tiếp, tuyệt đối không di dời Sinh viên không cần thiết.
    VehicleRequest admittedLecturer = lecturer;
    admittedLecturer.requestedAt = confirmedAt;
    const RouteResult freeRoute = routeForZone(admittedLecturer, Zone::Lecturer);
    if (freeRoute.found) {
        const CheckInResult parked = parkRequest(admittedLecturer, freeRoute, false);
        if (parked.status == CheckInStatus::Parked) {
            pendingRelocation_.reset();
            appendHistory(confirmedAt,
                          "RELOCATION_CANCELLED_FREE_SLOT",
                          admittedLecturer.cardId,
                          admittedLecturer.plate,
                          {},
                          parked.slotId);
            result.success = true;
            result.message = "Co o Giang vien trong; da cap truc tiep, khong di doi Sinh vien";
            result.lecturerCardId = admittedLecturer.cardId;
            result.lecturerSlotId = parked.slotId;
            result.lecturerRoute = parked.route;
            return result;
        }
    }

    const auto rebuildPlanOrQueue = [&](const std::string& reason) {
        pendingRelocation_.reset();
        std::optional<RelocationPlan> replacement = createRelocationPlan(lecturer);
        if (replacement.has_value()) {
            pendingRelocation_ = replacement;
            appendHistory(confirmedAt,
                          "RELOCATION_PLAN_REFRESHED",
                          lecturer.cardId,
                          lecturer.plate,
                          {},
                          replacement->targetSlotId);
            result.message = reason + "; da tao lai plan, hay kiem tra va xac nhan lai";
            result.temporaryRemovalOrder = replacement->temporaryRemovalOrder;
            result.restoreOrder = replacement->restoreOrder;
        } else {
            (void)enqueueRequest(lecturer);
            result.message = reason + "; yeu cau Giang vien da vao Queue";
        }
    };

    ParkingSession* target = sessionsByCard_.find(plan.targetStudentCardId);
    ParkingNode* slot = graph_.findNode(plan.targetSlotId);
    if (target == nullptr || slot == nullptr || target->status != SessionStatus::Parked ||
        !target->overflow || target->slotId != plan.targetSlotId ||
        slot->occupiedCardId != target->cardId) {
        // Không làm mất yêu cầu GV: thử candidate reachable kế tiếp rồi mới Queue.
        rebuildPlanOrQueue("Xe muc tieu da thay doi");
        return result;
    }

    RouteResult route = graph_.shortestPathToNode(lecturer.gateId, plan.targetSlotId);
    if (!route.found) {
        rebuildPlanOrQueue("Duong den xe muc tieu khong con hieu luc");
        return result;
    }

    std::vector<BlockerMove> currentRemovalOrder;
    std::vector<BlockerMove> currentRestoreOrder;
    buildBlockerOrders(*slot, currentRemovalOrder, currentRestoreOrder);
    const auto sameOrder = [](const std::vector<BlockerMove>& left,
                              const std::vector<BlockerMove>& right) {
        if (left.size() != right.size()) {
            return false;
        }
        for (std::size_t index = 0; index < left.size(); ++index) {
            if (left[index].cardId != right[index].cardId ||
                left[index].plate != right[index].plate ||
                left[index].slotId != right[index].slotId ||
                left[index].depthFromExit != right[index].depthFromExit) {
                return false;
            }
        }
        return true;
    };
    if (!sameOrder(currentRemovalOrder, plan.temporaryRemovalOrder) ||
        !sameOrder(currentRestoreOrder, plan.restoreOrder)) {
        pendingRelocation_->temporaryRemovalOrder = currentRemovalOrder;
        pendingRelocation_->restoreOrder = currentRestoreOrder;
        result.message = "Danh sach xe chan da thay doi; plan da lam moi, hay xac nhan lai";
        result.temporaryRemovalOrder = std::move(currentRemovalOrder);
        result.restoreOrder = std::move(currentRestoreOrder);
        return result;
    }

    const ParkingSession originalTarget = *target;
    ParkingSession lecturerSession;
    lecturerSession.cardId = lecturer.cardId;
    lecturerSession.plate = lecturer.plate;
    lecturerSession.role = lecturer.role;
    lecturerSession.vehicleType = lecturer.vehicleType;
    lecturerSession.gateId = lecturer.gateId;
    lecturerSession.entryTime = confirmedAt;  // Không tính phí thời gian chờ xác nhận.
    lecturerSession.slotId = plan.targetSlotId;
    lecturerSession.status = SessionStatus::Parked;
    lecturerSession.overflow = false;
    lecturerSession.sequence = lecturer.sequence;

    // Chuẩn bị mọi cấp phát của audit log trước khi bắt đầu transaction.
    static_assert(std::is_nothrow_move_constructible<HistoryEntry>::value,
                  "HistoryEntry move phai noexcept de commit audit an toan");
    HistoryEntry movedHistory{nextHistoryId_,
                              confirmedAt,
                              "STUDENT_MOVED_TO_HOLDING",
                              originalTarget.cardId,
                              originalTarget.plate,
                              plan.targetSlotId,
                              "KHU_TAM_DI_DOI",
                              0};
    HistoryEntry lecturerHistory{nextHistoryId_ + 1,
                                 confirmedAt,
                                 "LECTURER_CHECK_IN_AFTER_RELOCATION",
                                 lecturer.cardId,
                                 lecturer.plate,
                                 {},
                                 plan.targetSlotId,
                                 0};
    history_.reserve(history_.size() + 2);

    if (!graph_.releaseSlot(plan.targetSlotId, target->cardId)) {
        result.message = "Khong the giai phong o cua xe Sinh vien";
        return result;
    }
    std::string savedTargetSlot;
    savedTargetSlot.swap(target->slotId);
    target->status = SessionStatus::Holding;
    target->overflow = false;

    bool occupiedByLecturer = false;
    bool insertedSession = false;
    bool insertedPlate = false;
    bool transactionReady = false;
    try {
        occupiedByLecturer = graph_.occupySlot(plan.targetSlotId, lecturer.cardId);
        if (occupiedByLecturer) {
            insertedSession = sessionsByCard_.insert(lecturer.cardId, lecturerSession);
        }
        if (insertedSession) {
            insertedPlate = cardByPlate_.insert(lecturer.plate, lecturer.cardId);
        }
        if (insertedPlate) {
            // enqueue có strong guarantee: node chỉ được nối sau khi cấp phát xong.
            holdingQueue(originalTarget.vehicleType).enqueue(originalTarget.cardId);
            transactionReady = true;
        }
    } catch (...) {
        transactionReady = false;
    }

    if (!transactionReady) {
        if (insertedPlate) {
            cardByPlate_.erase(lecturer.plate);
        }
        if (insertedSession) {
            sessionsByCard_.erase(lecturer.cardId);
        }
        if (occupiedByLecturer) {
            graph_.releaseSlot(plan.targetSlotId, lecturer.cardId);
        }
        target->status = originalTarget.status;
        target->overflow = originalTarget.overflow;
        target->slotId.swap(savedTargetSlot);
        const bool restored = graph_.occupySlot(plan.targetSlotId, originalTarget.cardId);
        result.message = restored
                             ? "Loi giao dich; da rollback toan bo di doi"
                             : "LOI NGHIEM TRONG: rollback o do that bai";
        return result;
    }

    // Từ đây chỉ còn các thao tác noexcept: transaction đã commit hoàn toàn.
    pendingRelocation_.reset();
    history_.push_back(std::move(movedHistory));
    history_.push_back(std::move(lecturerHistory));
    nextHistoryId_ += 2;

    result.success = true;
    result.message = "Da di doi xe Sinh vien va cap o cho Giang vien";
    result.movedStudentCardId = originalTarget.cardId;
    result.lecturerCardId = lecturer.cardId;
    result.lecturerSlotId = plan.targetSlotId;
    result.lecturerRoute = std::move(route);
    result.temporaryRemovalOrder = std::move(currentRemovalOrder);
    result.restoreOrder = std::move(currentRestoreOrder);
    return result;
}

bool ParkingSystem::rehomeHolding(VehicleType type,
                                  Timestamp eventTime,
                                  std::vector<AdmissionNotice>& admissions) {
    ds::LinkedQueue<std::string>& queue = holdingQueue(type);
    const std::size_t attempts = queue.size();
    for (std::size_t index = 0; index < attempts; ++index) {
        const std::optional<std::string> cardOptional = queue.dequeue();
        if (!cardOptional.has_value()) {
            return false;
        }
        ParkingSession* session = sessionsByCard_.find(*cardOptional);
        if (session == nullptr || session->status != SessionStatus::Holding ||
            session->vehicleType != type) {
            continue;  // Bỏ node chờ cũ sau checkout/rollback.
        }

        VehicleRequest request{session->cardId,
                               session->plate,
                               session->role,
                               session->vehicleType,
                               session->gateId,
                               eventTime,
                               session->sequence};
        RouteResult route = routeForZone(request, Zone::Student);
        if (!route.found) {
            queue.enqueue(session->cardId);
            return false;  // Giữ FIFO tuyệt đối trong Queue khu tạm.
        }
        if (!graph_.occupySlot(route.destinationSlotId, session->cardId)) {
            queue.enqueue(session->cardId);
            return false;
        }

        session->slotId = route.destinationSlotId;
        session->status = SessionStatus::Parked;
        session->overflow = false;
        admissions.push_back(AdmissionNotice{session->cardId,
                                             session->plate,
                                             session->slotId,
                                             true,
                                             false,
                                             route});
        appendHistory(eventTime,
                      "HOLDING_TO_STUDENT_SLOT",
                      session->cardId,
                      session->plate,
                      "KHU_TAM_DI_DOI",
                      session->slotId);
        return true;
    }
    return false;
}

void ParkingSystem::removeFromHoldingQueue(VehicleType type,
                                           const std::string& cardId) {
    (void)holdingQueue(type).removeFirst(
        [&](const std::string& queuedCard) { return queuedCard == cardId; });
}

bool ParkingSystem::admitWaiting(UserRole role,
                                 VehicleType type,
                                 Zone destinationZone,
                                 Timestamp eventTime,
                                 std::vector<AdmissionNotice>& admissions) {
    ds::LinkedQueue<VehicleRequest>& queue = waitingQueue(role, type);
    const std::size_t attempts = queue.size();
    for (std::size_t index = 0; index < attempts; ++index) {
        std::optional<VehicleRequest> requestOptional = queue.dequeue();
        if (!requestOptional.has_value()) {
            return false;
        }
        VehicleRequest request = std::move(*requestOptional);
        if (sessionsByCard_.contains(request.cardId) || cardByPlate_.contains(request.plate)) {
            continue;  // Bản ghi Queue cũ, bỏ an toàn.
        }

        request.requestedAt = eventTime;  // Không tính phí thời gian chờ ngoài cổng.
        RouteResult route = routeForZone(request, destinationZone);
        if (!route.found) {
            queue.enqueue(std::move(request));
            return false;  // Không cho xe sau vượt phần tử đầu Queue FIFO.
        }
        const bool overflow = role == UserRole::Student && destinationZone == Zone::Lecturer;
        CheckInResult parked = parkRequest(request, route, overflow);
        if (parked.status != CheckInStatus::Parked) {
            queue.enqueue(std::move(request));
            return false;
        }
        admissions.push_back(AdmissionNotice{request.cardId,
                                             request.plate,
                                             parked.slotId,
                                             false,
                                             overflow,
                                             parked.route});
        return true;
    }
    return false;
}

void ParkingSystem::processReleasedCapacity(Zone zone,
                                            VehicleType type,
                                            Timestamp eventTime,
                                            std::vector<AdmissionNotice>& admissions) {
    if (zone == Zone::Student) {
        if (rehomeHolding(type, eventTime, admissions)) {
            return;
        }
        admitWaiting(UserRole::Student, type, Zone::Student, eventTime, admissions);
        return;
    }

    if (zone == Zone::Lecturer) {
        // Yêu cầu đang chờ xác nhận di dời được tạo trước các xe nằm sau trong
        // Queue, nên phải có ưu tiên cao nhất khi bất kỳ ô GV cùng loại vừa trống.
        if (pendingRelocation_.has_value() &&
            pendingRelocation_->lecturerRequest.vehicleType == type) {
            VehicleRequest pendingLecturer = pendingRelocation_->lecturerRequest;
            pendingLecturer.requestedAt = eventTime;
            const RouteResult route = routeForZone(pendingLecturer, Zone::Lecturer);
            if (route.found) {
                const CheckInResult parked = parkRequest(pendingLecturer, route, false);
                if (parked.status == CheckInStatus::Parked) {
                    pendingRelocation_.reset();
                    admissions.push_back(AdmissionNotice{pendingLecturer.cardId,
                                                         pendingLecturer.plate,
                                                         parked.slotId,
                                                         false,
                                                         false,
                                                         parked.route});
                    appendHistory(eventTime,
                                  "RELOCATION_CANCELLED_FREE_SLOT",
                                  pendingLecturer.cardId,
                                  pendingLecturer.plate,
                                  {},
                                  parked.slotId);
                    return;
                }
            }
        }

        if (admitWaiting(UserRole::Lecturer, type, Zone::Lecturer, eventTime, admissions)) {
            return;
        }
        if (!hasLecturerWaiting(type)) {
            admitWaiting(UserRole::Student, type, Zone::Lecturer, eventTime, admissions);
        }
    }
}

CheckOutResult ParkingSystem::checkOut(const std::string& rawCardId, Timestamp exitTime) {
    CheckOutResult result;
    const std::string cardId = normalizeCardId(rawCardId);
    ParkingSession* stored = sessionsByCard_.find(cardId);
    if (stored == nullptr) {
        result.message = "Khong tim thay ma the dang hoat dong";
        return result;
    }

    ParkingSession session = *stored;
    try {
        result.parkedMinutes = feeCalculator_.parkedMinutes(session.entryTime, exitTime);
        result.fee = feeCalculator_.calculate(session.vehicleType, session.entryTime, exitTime);
    } catch (const std::exception& error) {
        result.message = error.what();
        return result;
    }

    Zone releasedZone = Zone::None;
    VehicleType releasedType = session.vehicleType;
    if (session.status == SessionStatus::Parked) {
        const ParkingNode* slot = graph_.findNode(session.slotId);
        if (slot == nullptr || slot->occupiedCardId != session.cardId) {
            result.message = "Bat nhat quan: session khong khop trang thai o do";
            return result;
        }
        releasedZone = slot->zone;
        releasedType = slot->acceptedVehicle == VehicleType::Any
                           ? session.vehicleType
                           : slot->acceptedVehicle;
        if (!graph_.releaseSlot(session.slotId, session.cardId)) {
            result.message = "Khong the giai phong o do";
            return result;
        }
        result.releasedSlotId = session.slotId;
    } else if (session.status == SessionStatus::Holding) {
        // Không để node mồ côi trong Queue khu tạm sau khi xe rời bãi.
        removeFromHoldingQueue(session.vehicleType, session.cardId);
    }

    cardByPlate_.erase(session.plate);
    sessionsByCard_.erase(session.cardId);
    appendHistory(exitTime,
                  "CHECK_OUT",
                  session.cardId,
                  session.plate,
                  session.slotId,
                  {},
                  result.fee);

    if (releasedZone != Zone::None) {
        processReleasedCapacity(
            releasedZone, releasedType, exitTime, result.automaticAdmissions);
    }

    result.success = true;
    result.message = "Check-out thanh cong";
    return result;
}

std::vector<PlateMatch> ParkingSystem::fuzzySearch(const std::string& query,
                                                   std::size_t limit) const {
    return FuzzySearch::topMatches(query, activeSessions(), limit);
}

std::vector<ParkingSession> ParkingSystem::activeSessions() const {
    std::vector<ParkingSession> sessions;
    sessions.reserve(sessionsByCard_.size());
    sessionsByCard_.forEach([&](const std::string&, const ParkingSession& session) {
        sessions.push_back(session);
    });
    ds::mergeSort(sessions, [](const ParkingSession& left, const ParkingSession& right) {
        return left.cardId < right.cardId;
    });
    return sessions;
}

std::optional<ParkingSession> ParkingSystem::findSession(const std::string& cardId) const {
    const ParkingSession* session = sessionsByCard_.find(normalizeCardId(cardId));
    return session == nullptr ? std::nullopt : std::optional<ParkingSession>(*session);
}

std::vector<HistoryEntry> ParkingSystem::historySortedByTime() const {
    std::vector<HistoryEntry> output = history_;
    ds::mergeSort(output, [](const HistoryEntry& left, const HistoryEntry& right) {
        if (left.time != right.time) {
            return left.time < right.time;
        }
        return left.id < right.id;
    });
    return output;
}

std::vector<HistoryEntry> ParkingSystem::historySortedByFeeDescending() const {
    std::vector<HistoryEntry> output = history_;
    ds::mergeSort(output, [](const HistoryEntry& left, const HistoryEntry& right) {
        if (left.fee != right.fee) {
            return left.fee > right.fee;
        }
        return left.id < right.id;
    });
    return output;
}

ParkingStatistics ParkingSystem::statistics() const {
    ParkingStatistics result;
    for (const ParkingNode& node : graph_.nodes()) {
        if (node.type != NodeType::Slot) {
            continue;
        }
        ++result.totalSlots;
        if (!node.occupiedCardId.empty()) {
            ++result.occupiedSlots;
        }
        if (node.zone == Zone::Student) {
            ++result.studentSlots;
        } else if (node.zone == Zone::Lecturer) {
            ++result.lecturerSlots;
        }
    }
    result.activeSessions = sessionsByCard_.size();
    sessionsByCard_.forEach([&](const std::string&, const ParkingSession& session) {
        if (session.status == SessionStatus::Holding) {
            ++result.holdingVehicles;
        }
    });
    result.waitingVehicles = waitingCount();
    return result;
}

std::size_t ParkingSystem::waitingCount() const noexcept {
    return waitingStudentMotorcycles_.size() + waitingStudentCars_.size() +
           waitingLecturerMotorcycles_.size() + waitingLecturerCars_.size();
}

std::vector<std::string> ParkingSystem::validateInvariants() const {
    std::vector<std::string> errors;
    std::size_t occupiedSlots = 0;
    std::size_t holdingSessions = 0;
    const auto slotAccepts = [](const ParkingNode& slot, VehicleType type) {
        return slot.acceptedVehicle == VehicleType::Any || slot.acceptedVehicle == type;
    };

    for (const ParkingNode& node : graph_.nodes()) {
        if (node.type != NodeType::Slot || node.occupiedCardId.empty()) {
            continue;
        }
        ++occupiedSlots;
        const ParkingSession* session = sessionsByCard_.find(node.occupiedCardId);
        if (session == nullptr) {
            errors.push_back("O " + node.id + " co the " + node.occupiedCardId +
                             " nhung khong co session");
        } else {
            if (session->status != SessionStatus::Parked || session->slotId != node.id) {
                errors.push_back("O " + node.id + " khong tro nguoc dung session");
            }
            if (!node.enabled) {
                errors.push_back("O dang co xe nhung bi disable: " + node.id);
            }
            if (!slotAccepts(node, session->vehicleType)) {
                errors.push_back("O khong tuong thich loai xe: " + node.id);
            }
        }
    }

    sessionsByCard_.forEach([&](const std::string& cardId, const ParkingSession& session) {
        if (cardId != session.cardId) {
            errors.push_back("Khoa Hash khong khop session: " + cardId);
        }
        const std::string* indexedCard = cardByPlate_.find(session.plate);
        if (indexedCard == nullptr || *indexedCard != cardId) {
            errors.push_back("Chi muc plate->card sai cho " + cardId);
        }
        if (session.status == SessionStatus::Holding) {
            ++holdingSessions;
            if (!session.slotId.empty() || session.overflow) {
                errors.push_back("Xe o khu tam van gan slot/overflow: " + cardId);
            }
            return;
        }

        const ParkingNode* slot = graph_.findNode(session.slotId);
        if (slot == nullptr || slot->occupiedCardId != cardId) {
            errors.push_back("Session " + cardId + " khong co o do tuong ung");
            return;
        }
        if (!slot->enabled || !slotAccepts(*slot, session.vehicleType)) {
            errors.push_back("Session nam trong o disable/sai loai xe: " + cardId);
        }
        if (session.role == UserRole::Lecturer && slot->zone != Zone::Lecturer) {
            errors.push_back("Giang vien bi xep sai khu: " + cardId);
        }
        if (session.role == UserRole::Lecturer && session.overflow) {
            errors.push_back("Giang vien khong duoc mang co overflow: " + cardId);
        }
        if (session.role == UserRole::Student) {
            const bool shouldOverflow = slot->zone == Zone::Lecturer;
            if (session.overflow != shouldOverflow) {
                errors.push_back("Co overflow sai cho Sinh vien: " + cardId);
            }
        }
    });

    cardByPlate_.forEach([&](const std::string& plate, const std::string& cardId) {
        const ParkingSession* session = sessionsByCard_.find(cardId);
        if (session == nullptr || session->plate != plate) {
            errors.push_back("Chi muc cardByPlate co ban ghi mo coi: " + plate);
        }
    });

    if (sessionsByCard_.size() != cardByPlate_.size()) {
        errors.push_back("Hai Hash Table khac so luong ban ghi");
    }
    if (sessionsByCard_.size() != occupiedSlots + holdingSessions) {
        errors.push_back("So session khong bang occupied + holding");
    }

    std::vector<std::string> waitingCards;
    std::vector<std::string> waitingPlates;
    const auto inspectQueue = [&](const ds::LinkedQueue<VehicleRequest>& queue,
                                  UserRole expectedRole,
                                  VehicleType expectedType) {
        queue.forEach([&](const VehicleRequest& request) {
            if (request.role != expectedRole || request.vehicleType != expectedType) {
                errors.push_back("Xe nam sai Queue vai tro/loai: " + request.cardId);
            }
            if (sessionsByCard_.contains(request.cardId) || cardByPlate_.contains(request.plate)) {
                errors.push_back("Xe vua cho vua dang trong bai: " + request.cardId);
            }
            if (std::find(waitingCards.begin(), waitingCards.end(), request.cardId) !=
                waitingCards.end()) {
                errors.push_back("Trung ma the trong Queue: " + request.cardId);
            }
            if (std::find(waitingPlates.begin(), waitingPlates.end(), request.plate) !=
                waitingPlates.end()) {
                errors.push_back("Trung bien so trong Queue: " + request.plate);
            }
            waitingCards.push_back(request.cardId);
            waitingPlates.push_back(request.plate);
        });
    };
    inspectQueue(waitingStudentMotorcycles_, UserRole::Student, VehicleType::Motorcycle);
    inspectQueue(waitingStudentCars_, UserRole::Student, VehicleType::Car);
    inspectQueue(waitingLecturerMotorcycles_, UserRole::Lecturer, VehicleType::Motorcycle);
    inspectQueue(waitingLecturerCars_, UserRole::Lecturer, VehicleType::Car);

    std::vector<std::string> holdingCards;
    const auto inspectHolding = [&](const ds::LinkedQueue<std::string>& queue,
                                    VehicleType expectedType) {
        queue.forEach([&](const std::string& cardId) {
            if (std::find(holdingCards.begin(), holdingCards.end(), cardId) !=
                holdingCards.end()) {
                errors.push_back("Trung ma the trong Queue khu tam: " + cardId);
            }
            holdingCards.push_back(cardId);
            const ParkingSession* session = sessionsByCard_.find(cardId);
            if (session == nullptr || session->status != SessionStatus::Holding) {
                errors.push_back("Node khu tam khong co Holding session: " + cardId);
            } else if (session->vehicleType != expectedType) {
                errors.push_back("Xe nam sai Queue khu tam: " + cardId);
            }
        });
    };
    inspectHolding(holdingMotorcycles_, VehicleType::Motorcycle);
    inspectHolding(holdingCars_, VehicleType::Car);

    sessionsByCard_.forEach([&](const std::string& cardId, const ParkingSession& session) {
        if (session.status == SessionStatus::Holding &&
            std::count(holdingCards.begin(), holdingCards.end(), cardId) != 1) {
            errors.push_back("Holding session khong co dung mot node Queue: " + cardId);
        }
    });

    if (pendingRelocation_.has_value()) {
        const RelocationPlan& plan = *pendingRelocation_;
        const VehicleRequest& request = pendingRelocation_->lecturerRequest;
        if (request.role != UserRole::Lecturer || request.vehicleType == VehicleType::Any) {
            errors.push_back("Pending plan khong chua yeu cau Giang vien hop le");
        }
        if (sessionsByCard_.contains(request.cardId) || cardByPlate_.contains(request.plate)) {
            errors.push_back("Giang vien trong ke hoach di doi da co session");
        }
        if (std::find(waitingCards.begin(), waitingCards.end(), request.cardId) !=
                waitingCards.end() ||
            std::find(waitingPlates.begin(), waitingPlates.end(), request.plate) !=
                waitingPlates.end()) {
            errors.push_back("Giang vien pending dong thoi nam trong Queue");
        }
        const ParkingSession* target = sessionsByCard_.find(plan.targetStudentCardId);
        const ParkingNode* targetSlot = graph_.findNode(plan.targetSlotId);
        if (target == nullptr || targetSlot == nullptr ||
            target->status != SessionStatus::Parked || !target->overflow ||
            target->role != UserRole::Student ||
            target->vehicleType != request.vehicleType ||
            target->slotId != plan.targetSlotId ||
            targetSlot->zone != Zone::Lecturer || !targetSlot->enabled ||
            targetSlot->occupiedCardId != plan.targetStudentCardId) {
            errors.push_back("Xe muc tieu cua pending plan khong con hop le");
        }
    }
    return errors;
}

void ParkingSystem::appendHistory(Timestamp time,
                                  std::string event,
                                  const std::string& cardId,
                                  const std::string& plate,
                                  std::string fromSlot,
                                  std::string toSlot,
                                  Money fee) {
    history_.push_back(HistoryEntry{nextHistoryId_++,
                                    time,
                                    std::move(event),
                                    cardId,
                                    plate,
                                    std::move(fromSlot),
                                    std::move(toSlot),
                                    fee});
}

}  // namespace parking
