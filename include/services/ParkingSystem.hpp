#pragma once

#include "domain/Models.hpp"
#include "ds/BinaryMinHeap.hpp"
#include "ds/HashTable.hpp"
#include "ds/LinkedQueue.hpp"
#include "graph/ParkingGraph.hpp"
#include "services/FeeCalculator.hpp"

#include <optional>
#include <string>
#include <vector>

namespace parking {

// Facade nghiệp vụ. Mọi thay đổi trạng thái đi qua lớp này để Hash Table,
// đồ thị, Queue, Heap và Stack luôn nhất quán với nhau.
class ParkingSystem {
public:
    ParkingSystem();

    void loadMap(const std::string& nodesFile, const std::string& edgesFile);
    void loadFeePolicy(const std::string& feeFile);

    CheckInResult checkIn(VehicleRequest request);
    CheckOutResult checkOut(const std::string& cardId, Timestamp exitTime);
    RelocationResult confirmRelocation(const std::string& planId,
                                       Timestamp confirmedAt);

    std::vector<PlateMatch> fuzzySearch(const std::string& query,
                                        std::size_t limit = 3) const;
    std::vector<ParkingSession> activeSessions() const;
    std::optional<ParkingSession> findSession(const std::string& cardId) const;
    std::vector<HistoryEntry> historySortedByTime() const;
    std::vector<HistoryEntry> historySortedByFeeDescending() const;

    ParkingStatistics statistics() const;
    std::vector<std::string> validateInvariants() const;
    [[nodiscard]] std::size_t waitingCount() const noexcept;
    [[nodiscard]] const std::optional<RelocationPlan>& pendingRelocation() const noexcept {
        return pendingRelocation_;
    }

    const ParkingGraph& graph() const noexcept { return graph_; }
    const FeePolicy& feePolicy() const noexcept { return feeCalculator_.policy(); }

private:
    using SessionTable =
        ds::HashTable<std::string, ParkingSession, ds::StringHasher>;
    using PlateTable = ds::HashTable<std::string, std::string, ds::StringHasher>;
    using OverflowHeap =
        ds::BinaryMinHeap<OverflowEntry, OverflowEarlier>;

    ds::LinkedQueue<VehicleRequest>& waitingQueue(UserRole role, VehicleType type);
    const ds::LinkedQueue<VehicleRequest>& waitingQueue(UserRole role,
                                                         VehicleType type) const;
    ds::LinkedQueue<std::string>& holdingQueue(VehicleType type);
    const ds::LinkedQueue<std::string>& holdingQueue(VehicleType type) const;
    OverflowHeap& overflowHeap(VehicleType type);

    [[nodiscard]] bool isQueued(const std::string& cardId,
                                const std::string& plate) const;
    [[nodiscard]] bool hasLecturerWaiting(VehicleType type) const;
    [[nodiscard]] RouteResult routeForZone(const VehicleRequest& request,
                                           Zone zone) const;
    CheckInResult parkRequest(const VehicleRequest& request,
                              const RouteResult& route,
                              bool overflow);
    CheckInResult enqueueRequest(const VehicleRequest& request);
    std::optional<RelocationPlan> createRelocationPlan(const VehicleRequest& lecturer);
    std::optional<OverflowEntry> peekValidOverflow(VehicleType type,
                                                   const std::string& gateId);
    void buildBlockerOrders(const ParkingNode& targetSlot,
                            std::vector<BlockerMove>& removalOrder,
                            std::vector<BlockerMove>& restoreOrder) const;
    void removeFromHoldingQueue(VehicleType type, const std::string& cardId);

    bool rehomeHolding(VehicleType type,
                       Timestamp eventTime,
                       std::vector<AdmissionNotice>& admissions);
    bool admitWaiting(UserRole role,
                      VehicleType type,
                      Zone destinationZone,
                      Timestamp eventTime,
                      std::vector<AdmissionNotice>& admissions);
    void processReleasedCapacity(Zone zone,
                                 VehicleType type,
                                 Timestamp eventTime,
                                 std::vector<AdmissionNotice>& admissions);

    void appendHistory(Timestamp time,
                       std::string event,
                       const std::string& cardId,
                       const std::string& plate,
                       std::string fromSlot,
                       std::string toSlot,
                       Money fee = 0);

    ParkingGraph graph_;
    SessionTable sessionsByCard_;
    PlateTable cardByPlate_;

    ds::LinkedQueue<VehicleRequest> waitingStudentMotorcycles_;
    ds::LinkedQueue<VehicleRequest> waitingStudentCars_;
    ds::LinkedQueue<VehicleRequest> waitingLecturerMotorcycles_;
    ds::LinkedQueue<VehicleRequest> waitingLecturerCars_;
    ds::LinkedQueue<std::string> holdingMotorcycles_;
    ds::LinkedQueue<std::string> holdingCars_;

    OverflowHeap overflowMotorcycles_;
    OverflowHeap overflowCars_;
    std::optional<RelocationPlan> pendingRelocation_;

    FeeCalculator feeCalculator_;
    std::vector<HistoryEntry> history_;
    std::uint64_t nextSequence_{1};
    std::uint64_t nextHistoryId_{1};
    std::uint64_t nextPlanId_{1};
};

}  // namespace parking
