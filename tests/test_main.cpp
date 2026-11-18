#include "domain/Models.hpp"
#include "domain/Types.hpp"
#include "ds/BinaryMinHeap.hpp"
#include "ds/HashTable.hpp"
#include "ds/LinkedQueue.hpp"
#include "ds/LinkedStack.hpp"
#include "ds/MergeSort.hpp"
#include "graph/ParkingGraph.hpp"
#include "io/DataLoader.hpp"
#include "services/FeeCalculator.hpp"
#include "services/FuzzySearch.hpp"
#include "services/ParkingSystem.hpp"

#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace {

class TestFailure : public std::runtime_error {
public:
    explicit TestFailure(const std::string& message) : std::runtime_error(message) {}
};

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw TestFailure(message);
    }
}

template <typename Left, typename Right>
void expectEqual(const Left& left, const Right& right, const std::string& message) {
    if (!(left == right)) {
        throw TestFailure(message);
    }
}

template <typename Function>
void expectThrows(Function function, const std::string& message) {
    try {
        function();
    } catch (const std::exception&) {
        return;
    }
    throw TestFailure(message);
}

class TestRunner {
public:
    void run(const std::string& name, const std::function<void()>& test) {
        try {
            test();
            ++passed_;
            std::cout << "[PASS] " << name << '\n';
        } catch (const std::exception& error) {
            ++failed_;
            std::cout << "[FAIL] " << name << " -> " << error.what() << '\n';
        }
    }

    int finish() const {
        std::cout << "\nKet qua: " << passed_ << " PASS, " << failed_ << " FAIL\n";
        return failed_ == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    }

private:
    int passed_{0};
    int failed_{0};
};

std::string locateDataDirectory(const char* executable) {
    const std::filesystem::path executablePath = std::filesystem::absolute(executable);
    const std::vector<std::filesystem::path> candidates = {
        std::filesystem::current_path() / "data",
        std::filesystem::current_path().parent_path() / "data",
        executablePath.parent_path().parent_path() / "data"};
    for (const auto& candidate : candidates) {
        if (std::filesystem::exists(candidate / "nodes.csv")) {
            return candidate.string();
        }
    }
    throw std::runtime_error("Khong tim thay thu muc data cho test");
}

std::string pathJoin(const std::string& directory, const std::string& file) {
    return (std::filesystem::path(directory) / file).string();
}

void load(parking::ParkingSystem& system, const std::string& dataDirectory) {
    system.loadMap(pathJoin(dataDirectory, "nodes.csv"),
                   pathJoin(dataDirectory, "edges.csv"));
    system.loadFeePolicy(pathJoin(dataDirectory, "fees.cfg"));
}

parking::VehicleRequest makeRequest(std::string card,
                                    std::string plate,
                                    parking::UserRole role,
                                    parking::VehicleType type,
                                    parking::Timestamp time) {
    return parking::VehicleRequest{std::move(card),
                                   std::move(plate),
                                   role,
                                   type,
                                   "GATE_A",
                                   time,
                                   0};
}

struct MotorcycleScenario {
    parking::Timestamp start{0};
    parking::CheckInResult overflow;
};

MotorcycleScenario prepareRelocationScenario(parking::ParkingSystem& system,
                                             bool addWaitingStudent) {
    const parking::Timestamp start = parking::parseDateTime("2026-07-16 08:00:00");
    const std::vector<parking::VehicleRequest> firstArrivals = {
        makeRequest("GV001", "51F188801", parking::UserRole::Lecturer,
                    parking::VehicleType::Motorcycle, start),
        makeRequest("GV002", "51F188802", parking::UserRole::Lecturer,
                    parking::VehicleType::Motorcycle, start + 60),
        makeRequest("SV001", "59Y222201", parking::UserRole::Student,
                    parking::VehicleType::Motorcycle, start + 120),
        makeRequest("SV002", "60B333302", parking::UserRole::Student,
                    parking::VehicleType::Motorcycle, start + 180),
        makeRequest("SV003", "59X112355", parking::UserRole::Student,
                    parking::VehicleType::Motorcycle, start + 240)};
    for (const auto& arrival : firstArrivals) {
        const auto result = system.checkIn(arrival);
        expectEqual(result.status,
                    parking::CheckInStatus::Parked,
                    "Xe lap day phai check-in thanh cong");
    }

    const auto overflow = system.checkIn(makeRequest(
        "SVOVER", "59X112345", parking::UserRole::Student,
        parking::VehicleType::Motorcycle, start + 300));
    expectEqual(overflow.status,
                parking::CheckInStatus::Parked,
                "Sinh vien phai duoc do tran");
    expect(overflow.overflow, "Phai bat co overflow");
    expectEqual(overflow.slotId, std::string("LM03"), "Phai vao LM03");

    const auto secondOverflow = system.checkIn(makeRequest(
        "SVOVER2", "59X112346", parking::UserRole::Student,
        parking::VehicleType::Motorcycle, start + 330));
    expectEqual(secondOverflow.status,
                parking::CheckInStatus::Parked,
                "Sinh vien overflow thu hai phai duoc do");
    expect(secondOverflow.overflow, "Xe thu hai phai bat co overflow");
    expectEqual(secondOverflow.slotId, std::string("LM04"), "Phai vao LM04");

    if (addWaitingStudent) {
        const auto queued = system.checkIn(makeRequest(
            "SVWAIT", "59X112347", parking::UserRole::Student,
            parking::VehicleType::Motorcycle, start + 360));
        expectEqual(queued.status,
                    parking::CheckInStatus::Queued,
                    "Xe thu bay phai vao Queue");
    }
    return MotorcycleScenario{start, overflow};
}

void expectHealthy(const parking::ParkingSystem& system) {
    const auto errors = system.validateInvariants();
    if (!errors.empty()) {
        throw TestFailure("Invariant loi: " + errors.front());
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc < 1) {
        return EXIT_FAILURE;
    }
    const std::string dataDirectory = locateDataDirectory(argv[0]);
    TestRunner tests;

    tests.run("HashTable: collision, erase va rehash", [] {
        struct ConstantHasher {
            std::size_t operator()(int) const noexcept { return 0; }
        };
        parking::ds::HashTable<int, std::string, ConstantHasher> table(2);
        for (int value = 0; value < 100; ++value) {
            expect(table.insert(value, "V" + std::to_string(value)), "Insert that bai");
        }
        expect(table.bucketCount() > 2, "HashTable phai rehash");
        expectEqual(*table.find(50), std::string("V50"), "Find sau collision sai");
        expect(table.erase(50), "Erase chain giua that bai");
        expect(table.find(50) == nullptr, "Khoa da xoa van ton tai");
        expectEqual(*table.find(99), std::string("V99"), "Erase lam dut chain");
        expect(!table.insert(99, "duplicate"), "Cho phep duplicate key");

        auto moved = std::move(table);
        expect(table.empty(), "HashTable nguon sau move khong rong logic");
        expectEqual(*moved.find(99), std::string("V99"), "HashTable move mat du lieu");
        expect(table.insert(500, "reuse"), "HashTable sau move khong dung lai duoc");
        expectEqual(*table.find(500), std::string("reuse"), "Find tren nguon move sai");
    });

    tests.run("LinkedQueue: FIFO va vong doi rong-lai-rong", [] {
        parking::ds::LinkedQueue<int> queue;
        expect(!queue.dequeue().has_value(), "Dequeue rong phai nullopt");
        queue.enqueue(1);
        queue.enqueue(2);
        queue.enqueue(3);
        expectEqual(*queue.dequeue(), 1, "FIFO sai phan tu 1");
        expectEqual(*queue.dequeue(), 2, "FIFO sai phan tu 2");
        expectEqual(*queue.dequeue(), 3, "FIFO sai phan tu 3");
        expect(queue.empty(), "Queue phai rong");
        queue.enqueue(4);
        queue.enqueue(5);
        queue.enqueue(6);
        expect(queue.removeFirst([](int value) { return value == 5; }),
               "Khong xoa duoc node giua Queue");
        expectEqual(*queue.dequeue(), 4, "Remove lam sai front");
        expectEqual(*queue.dequeue(), 6, "Remove lam sai lien ket tail");
    });

    tests.run("LinkedStack: LIFO", [] {
        parking::ds::LinkedStack<std::string> stack;
        expect(!stack.pop().has_value(), "Pop rong phai nullopt");
        stack.push("A");
        stack.push("B");
        stack.push("C");
        expectEqual(*stack.pop(), std::string("C"), "LIFO sai C");
        expectEqual(*stack.pop(), std::string("B"), "LIFO sai B");
        expectEqual(*stack.pop(), std::string("A"), "LIFO sai A");
        stack.push("X");
        stack.push("Y");
        auto moved = std::move(stack);
        expect(stack.empty(), "Stack nguon sau move phai rong");
        expectEqual(*moved.pop(), std::string("Y"), "Stack move mat top");
        stack.push("REUSE");
        expectEqual(*stack.pop(), std::string("REUSE"), "Stack sau move khong dung lai duoc");
    });

    tests.run("BinaryMinHeap: thu tu uu tien", [] {
        parking::ds::BinaryMinHeap<int> heap;
        for (int value : {7, 2, 9, 1, 5, 1}) {
            heap.push(value);
        }
        const std::vector<int> expected = {1, 1, 2, 5, 7, 9};
        for (int value : expected) {
            expectEqual(*heap.pop(), value, "MinHeap pop sai thu tu");
        }
        expect(!heap.pop().has_value(), "Heap rong phai nullopt");
    });

    tests.run("MergeSort: on dinh khi khoa bang nhau", [] {
        struct Item {
            int key{0};
            int originalOrder{0};
        };
        std::vector<Item> values = {{2, 0}, {1, 1}, {2, 2}, {1, 3}};
        parking::ds::mergeSort(values, [](const Item& left, const Item& right) {
            return left.key < right.key;
        });
        expectEqual(values[0].originalOrder, 1, "MergeSort mat tinh on dinh 1");
        expectEqual(values[1].originalOrder, 3, "MergeSort mat tinh on dinh 2");
        expectEqual(values[2].originalOrder, 0, "MergeSort mat tinh on dinh 3");
        expectEqual(values[3].originalOrder, 2, "MergeSort mat tinh on dinh 4");
    });

    tests.run("Chuan hoa chuoi va Levenshtein", [] {
        expectEqual(parking::normalizePlate(" 59-X1.123.4s "),
                    std::string("59X11234S"),
                    "Chuan hoa bien so sai");
        expectEqual(parking::FuzzySearch::levenshteinDistance("59X11234S", "59X112345"),
                    1,
                    "Khoang cach thay the phai bang 1");
        expectEqual(parking::FuzzySearch::levenshteinDistance("", "ABC"),
                    3,
                    "Khoang cach chuoi rong sai");
    });

    tests.run("FeeCalculator: moc 59, 60, 61 phut", [] {
        const parking::FeeCalculator fees;
        expectEqual(fees.calculate(parking::VehicleType::Motorcycle, 0, 59 * 60),
                    static_cast<parking::Money>(5000),
                    "Phi 59 phut sai");
        expectEqual(fees.calculate(parking::VehicleType::Motorcycle, 0, 60 * 60),
                    static_cast<parking::Money>(5000),
                    "Phi 60 phut sai");
        expectEqual(fees.calculate(parking::VehicleType::Motorcycle, 0, 61 * 60),
                    static_cast<parking::Money>(10000),
                    "Phi 61 phut sai");
        expectThrows([&] { (void)fees.calculate(parking::VehicleType::Car, 10, 9); },
                     "Phai tu choi thoi gian am");
    });

    tests.run("DataLoader: CSV co quote va camera log", [&] {
        const auto fields = parking::DataLoader::parseCsvLine("A,\"B,C\",\"D\"\"E\"");
        expectEqual(fields.size(), static_cast<std::size_t>(3), "So cot CSV sai");
        expectEqual(fields[1], std::string("B,C"), "Cot co dau phay sai");
        expectEqual(fields[2], std::string("D\"E"), "Escape quote sai");
        const auto logs = parking::DataLoader::loadCameraReadings(
            pathJoin(dataDirectory, "camera_logs.csv"));
        expectEqual(logs.size(), static_cast<std::size_t>(4), "So log camera sai");
    });

    tests.run("Graph: Dijkstra dung met, BFS dung so canh", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const auto bfs = system.graph().bfsToNearestSlot(
            "GATE_A", parking::Zone::Lecturer, parking::VehicleType::Motorcycle);
        const auto dijkstra = system.graph().shortestPathToNearestSlot(
            "GATE_A", parking::Zone::Lecturer, parking::VehicleType::Motorcycle);
        expect(bfs.found && dijkstra.found, "Phai tim duoc duong");
        expectEqual(bfs.totalDistance, static_cast<std::int64_t>(20), "BFS demo sai");
        expectEqual(dijkstra.totalDistance, static_cast<std::int64_t>(10), "Dijkstra sai");
        expectEqual(dijkstra.destinationSlotId, std::string("LM01"), "O gan nhat sai");
    });

    tests.run("Graph: tie-break theo ma slot", [] {
        parking::ParkingGraph graph;
        parking::ParkingNode gate;
        gate.id = "G";
        gate.label = "Gate";
        gate.type = parking::NodeType::Gate;
        graph.addNode(std::move(gate));

        parking::ParkingNode slotB;
        slotB.id = "SLOT_B";
        slotB.label = "B";
        slotB.type = parking::NodeType::Slot;
        slotB.zone = parking::Zone::Student;
        slotB.acceptedVehicle = parking::VehicleType::Motorcycle;
        graph.addNode(std::move(slotB));

        parking::ParkingNode slotA;
        slotA.id = "SLOT_A";
        slotA.label = "A";
        slotA.type = parking::NodeType::Slot;
        slotA.zone = parking::Zone::Student;
        slotA.acceptedVehicle = parking::VehicleType::Motorcycle;
        graph.addNode(std::move(slotA));

        parking::ParkingNode slotMax;
        slotMax.id = "SLOT_MAX";
        slotMax.label = "Max";
        slotMax.type = parking::NodeType::Slot;
        slotMax.zone = parking::Zone::Student;
        slotMax.acceptedVehicle = parking::VehicleType::Motorcycle;
        graph.addNode(std::move(slotMax));
        graph.addEdge("G", "SLOT_B", 5);
        graph.addEdge("G", "SLOT_A", 5);
        graph.addEdge("G", "SLOT_MAX", std::numeric_limits<std::int64_t>::max());
        const auto route = graph.shortestPathToNearestSlot(
            "G", parking::Zone::Student, parking::VehicleType::Motorcycle);
        expectEqual(route.destinationSlotId, std::string("SLOT_A"), "Tie-break sai");
        const auto maxRoute = graph.shortestPathToNode("G", "SLOT_MAX");
        expect(maxRoute.found, "Canh int64 max bi coi la unreachable");
        expectEqual(maxRoute.totalDistance,
                    std::numeric_limits<std::int64_t>::max(),
                    "Khoang cach int64 max sai");
        expectThrows([&] { graph.addEdge("G", "SLOT_A", -1); },
                     "Phai tu choi canh am");
    });

    tests.run("DataLoader: loi file khong lam hong graph cu", [&] {
        parking::ParkingGraph graph;
        parking::DataLoader::loadParkingMap(graph,
                                            pathJoin(dataDirectory, "nodes.csv"),
                                            pathJoin(dataDirectory, "edges.csv"));
        const std::size_t oldNodes = graph.nodeCount();
        const std::filesystem::path badEdges =
            std::filesystem::temp_directory_path() / "smart_parking_bad_edges.csv";
        std::ofstream output(badEdges);
        output << "from,to,distance_m,bidirectional,enabled\n"
               << "GATE_A,J0,-2,true,true\n";
        output.close();
        expectThrows(
            [&] {
                parking::DataLoader::loadParkingMap(
                    graph, pathJoin(dataDirectory, "nodes.csv"), badEdges.string());
            },
            "File canh am phai bi tu choi");
        expectEqual(graph.nodeCount(), oldNodes, "Graph cu bi thay doi khi load loi");
    });

    tests.run("Check-in/check-out va chong trung", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const parking::Timestamp start = parking::parseDateTime("2026-07-16 08:00:00");
        const auto first = system.checkIn(makeRequest(
            "CARD01", "59X123456", parking::UserRole::Student,
            parking::VehicleType::Motorcycle, start));
        expectEqual(first.status, parking::CheckInStatus::Parked, "Check-in that bai");
        const auto duplicateCard = system.checkIn(makeRequest(
            "CARD01", "59X123457", parking::UserRole::Student,
            parking::VehicleType::Motorcycle, start + 1));
        expectEqual(duplicateCard.status,
                    parking::CheckInStatus::Rejected,
                    "Khong chan trung the");
        const auto duplicatePlate = system.checkIn(makeRequest(
            "CARD02", "59-X1.234.56", parking::UserRole::Student,
            parking::VehicleType::Motorcycle, start + 2));
        expectEqual(duplicatePlate.status,
                    parking::CheckInStatus::Rejected,
                    "Khong chan trung bien so");
        const auto out = system.checkOut("CARD01", start + 61 * 60);
        expect(out.success, "Check-out that bai");
        expectEqual(out.fee, static_cast<parking::Money>(10000), "Phi check-out sai");
        expectHealthy(system);
    });

    tests.run("Queue: ba xe cho duoc phuc vu FIFO", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const parking::Timestamp start = parking::parseDateTime("2026-07-16 08:00:00");
        for (int index = 1; index <= 3; ++index) {
            system.checkIn(makeRequest("GV" + std::to_string(index),
                                       "51F80000" + std::to_string(index),
                                       parking::UserRole::Lecturer,
                                       parking::VehicleType::Motorcycle,
                                       start + index));
            system.checkIn(makeRequest("SV" + std::to_string(index),
                                       "59X70000" + std::to_string(index),
                                       parking::UserRole::Student,
                                       parking::VehicleType::Motorcycle,
                                       start + 10 + index));
        }
        const auto fourthLecturer = system.checkIn(makeRequest(
            "GV4", "51F800004", parking::UserRole::Lecturer,
            parking::VehicleType::Motorcycle, start + 4));
        expectEqual(fourthLecturer.status,
                    parking::CheckInStatus::Parked,
                    "Chua lap day LM04");

        for (int index = 1; index <= 3; ++index) {
            const auto queued = system.checkIn(makeRequest(
                "WAIT" + std::to_string(index),
                "59X79999" + std::to_string(index),
                parking::UserRole::Student,
                parking::VehicleType::Motorcycle,
                start + 20 + index));
            expectEqual(queued.status,
                        parking::CheckInStatus::Queued,
                        "Xe phai vao Queue");
            expectEqual(queued.queuePosition,
                        static_cast<std::size_t>(index),
                        "Vi tri Queue sai");
        }
        for (int index = 1; index <= 3; ++index) {
            const auto out = system.checkOut("SV" + std::to_string(index),
                                             start + 3600 + index);
            expect(out.success, "Checkout tao slot that bai");
            expectEqual(out.automaticAdmissions.size(),
                        static_cast<std::size_t>(1),
                        "Khong tu dong nhan xe cho");
            expectEqual(out.automaticAdmissions[0].cardId,
                        "WAIT" + std::to_string(index),
                        "Queue khong FIFO");
        }
        expectHealthy(system);
    });

    tests.run("Relocation: Heap chon xe som nhat, Stack dung thu tu", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const MotorcycleScenario scenario = prepareRelocationScenario(system, false);
        const auto lecturer = system.checkIn(makeRequest(
            "GV003", "51F188803", parking::UserRole::Lecturer,
            parking::VehicleType::Motorcycle, scenario.start + 420));
        expectEqual(lecturer.status,
                    parking::CheckInStatus::RelocationRequired,
                    "Phai yeu cau di doi");
        expect(lecturer.relocation.has_value(), "Thieu relocation plan");
        const auto& plan = *lecturer.relocation;
        expectEqual(plan.targetStudentCardId, std::string("SVOVER"), "Heap chon sai xe");
        expectEqual(plan.temporaryRemovalOrder.size(),
                    static_cast<std::size_t>(2),
                    "Thieu xe chan");
        expectEqual(plan.temporaryRemovalOrder[0].slotId,
                    std::string("LM01"),
                    "Thu tu push sai");
        expectEqual(plan.temporaryRemovalOrder[1].slotId,
                    std::string("LM02"),
                    "Thu tu push sai");
        expectEqual(plan.restoreOrder[0].slotId,
                    std::string("LM02"),
                    "Thu tu pop sai");
        expectEqual(plan.restoreOrder[1].slotId,
                    std::string("LM01"),
                    "Thu tu pop sai");

        const parking::Timestamp confirmedAt = scenario.start + 2 * 60 * 60;
        const auto confirmed = system.confirmRelocation(plan.planId, confirmedAt);
        expect(confirmed.success, "Xac nhan di doi that bai");
        const auto movedStudent = system.findSession("SVOVER");
        expect(movedStudent.has_value(), "Mat session Sinh vien");
        expectEqual(movedStudent->status,
                    parking::SessionStatus::Holding,
                    "Sinh vien phai vao khu tam");
        const auto lecturerSession = system.findSession("GV003");
        expect(lecturerSession.has_value(), "Khong tao session Giang vien");
        expectEqual(lecturerSession->slotId, std::string("LM03"), "GV nhan sai o");
        expectEqual(system.findSession("GV001")->slotId,
                    std::string("LM01"),
                    "Xe chan LM01 khong duoc hoan nguyen");
        expectEqual(system.findSession("GV002")->slotId,
                    std::string("LM02"),
                    "Xe chan LM02 khong duoc hoan nguyen");
        const auto lecturerOut = system.checkOut("GV003", confirmedAt + 60);
        expectEqual(lecturerOut.fee,
                    static_cast<parking::Money>(5000),
                    "Khong duoc tinh phi thoi gian GV cho xac nhan");
        expectHealthy(system);

        // Hai component tách rời: Heap phải bỏ qua ứng viên sớm hơn nhưng
        // không reachable từ cổng của GV, rồi chọn xe sớm nhất trong component.
        const std::filesystem::path componentNodes =
            std::filesystem::temp_directory_path() / "smart_parking_component_nodes.csv";
        const std::filesystem::path componentEdges =
            std::filesystem::temp_directory_path() / "smart_parking_component_edges.csv";
        {
            std::ofstream nodes(componentNodes);
            nodes << "id,label,type,zone,vehicle_type,lane,depth\n"
                  << "GATE_A,Gate A,GATE,NONE,ANY,,0\n"
                  << "GATE_B,Gate B,GATE,NONE,ANY,,0\n"
                  << "SA,Student A,SLOT,STUDENT,MOTORCYCLE,A,1\n"
                  << "LA,Lecturer A,SLOT,LECTURER,MOTORCYCLE,A,2\n"
                  << "SB,Student B,SLOT,STUDENT,MOTORCYCLE,B,1\n"
                  << "LB,Lecturer B,SLOT,LECTURER,MOTORCYCLE,B,2\n";
        }
        {
            std::ofstream edges(componentEdges);
            edges << "from,to,distance_m,bidirectional,enabled\n"
                  << "GATE_A,SA,1,true,true\n"
                  << "GATE_A,LA,2,true,true\n"
                  << "GATE_B,SB,1,true,true\n"
                  << "GATE_B,LB,2,true,true\n";
        }

        parking::ParkingSystem components;
        components.loadMap(componentNodes.string(), componentEdges.string());
        components.loadFeePolicy(pathJoin(dataDirectory, "fees.cfg"));
        const parking::Timestamp base =
            parking::parseDateTime("2026-07-17 08:00:00");
        const auto atGate = [&](std::string card,
                                std::string plate,
                                parking::UserRole role,
                                parking::Timestamp time,
                                std::string gate) {
            auto input = makeRequest(std::move(card),
                                     std::move(plate),
                                     role,
                                     parking::VehicleType::Motorcycle,
                                     time);
            input.gateId = std::move(gate);
            return input;
        };
        expectEqual(components.checkIn(atGate("AFILL", "59A111111",
                                               parking::UserRole::Student,
                                               base, "GATE_A")).status,
                    parking::CheckInStatus::Parked,
                    "Khong lap day khu SV component A");
        expect(components.checkIn(atGate("OVER_A", "59A111112",
                                         parking::UserRole::Student,
                                         base + 10, "GATE_A")).overflow,
               "Khong tao overflow component A");
        expectEqual(components.checkIn(atGate("BFILL", "59B111111",
                                               parking::UserRole::Student,
                                               base + 20, "GATE_B")).status,
                    parking::CheckInStatus::Parked,
                    "Khong lap day khu SV component B");
        expect(components.checkIn(atGate("OVER_B", "59B111112",
                                         parking::UserRole::Student,
                                         base + 30, "GATE_B")).overflow,
               "Khong tao overflow component B");

        const auto lecturerB = components.checkIn(atGate(
            "GV_B", "51B111111", parking::UserRole::Lecturer, base + 40, "GATE_B"));
        expect(lecturerB.relocation.has_value(), "GV component B khong tao plan");
        expectEqual(lecturerB.relocation->targetStudentCardId,
                    std::string("OVER_B"),
                    "Heap chon xe som hon nhung khong reachable o component A");
        expect(components.confirmRelocation(lecturerB.relocation->planId,
                                            base + 50).success,
               "Plan reachable component B khong confirm duoc");
        const auto lecturerA = components.checkIn(atGate(
            "GV_A", "51A111111", parking::UserRole::Lecturer, base + 60, "GATE_A"));
        expect(lecturerA.relocation.has_value(),
               "Ung vien unreachable tam pop khong duoc chen lai Heap");
        expectEqual(lecturerA.relocation->targetStudentCardId,
                    std::string("OVER_A"),
                    "Heap mat ung vien component A sau khi chon component B");
        expectHealthy(components);
        (void)std::filesystem::remove(componentNodes);
        (void)std::filesystem::remove(componentEdges);
    });

    tests.run("Relocation: target checkout truoc confirm khong lam mat GV", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const MotorcycleScenario scenario = prepareRelocationScenario(system, false);
        const auto lecturer = system.checkIn(makeRequest(
            "GV003", "51F188803", parking::UserRole::Lecturer,
            parking::VehicleType::Motorcycle, scenario.start + 420));
        expect(lecturer.relocation.has_value(), "Phai co plan truoc checkout");
        const std::string oldPlan = lecturer.relocation->planId;

        const auto targetOut = system.checkOut("SVOVER", scenario.start + 430);
        expect(targetOut.success, "Target checkout that bai");
        expect(!system.pendingRelocation().has_value(), "Plan cu chua bi huy");
        expectEqual(targetOut.automaticAdmissions.size(),
                    static_cast<std::size_t>(1),
                    "GV dang cho khong duoc nhan o vua trong");
        expectEqual(targetOut.automaticAdmissions[0].cardId,
                    std::string("GV003"),
                    "Cap o sai xe sau khi huy plan");
        expect(system.findSession("GV003").has_value(), "Mat yeu cau GV");
        expect(!system.confirmRelocation(oldPlan, scenario.start + 431).success,
               "Plan cu van confirm duoc");
        expectHealthy(system);
    });

    tests.run("Relocation pending uu tien va Heap giu ung vien khi huy plan", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const MotorcycleScenario scenario = prepareRelocationScenario(system, true);

        const auto firstLecturer = system.checkIn(makeRequest(
            "GV003", "51F188803", parking::UserRole::Lecturer,
            parking::VehicleType::Motorcycle, scenario.start + 420));
        expectEqual(firstLecturer.status,
                    parking::CheckInStatus::RelocationRequired,
                    "GV dau phai tao pending plan");
        const auto laterLecturer = system.checkIn(makeRequest(
            "GV004", "51F188804", parking::UserRole::Lecturer,
            parking::VehicleType::Motorcycle, scenario.start + 421));
        expectEqual(laterLecturer.status,
                    parking::CheckInStatus::Queued,
                    "GV den sau phai vao Queue");

        const auto firstFree = system.checkOut("GV001", scenario.start + 430);
        expectEqual(firstFree.automaticAdmissions.size(),
                    static_cast<std::size_t>(1),
                    "Pending GV khong duoc nhan o moi trong");
        expectEqual(firstFree.automaticAdmissions[0].cardId,
                    std::string("GV003"),
                    "Pending GV bi xe den sau/SV vuot mat");
        expect(!system.pendingRelocation().has_value(), "Plan khong duoc huy sau cap truc tiep");
        expectEqual(system.waitingCount(),
                    static_cast<std::size_t>(2),
                    "Queue GV/SV bi thay doi sai");

        const auto newestLecturer = system.checkIn(makeRequest(
            "GV005", "51F188805", parking::UserRole::Lecturer,
            parking::VehicleType::Motorcycle, scenario.start + 435));
        expectEqual(newestLecturer.status,
                    parking::CheckInStatus::Queued,
                    "GV moi khong duoc tao plan vuot Queue GV cu");
        expectEqual(newestLecturer.queuePosition,
                    static_cast<std::size_t>(2),
                    "Sai vi tri FIFO trong Queue GV");

        const auto secondFree = system.checkOut("GV003", scenario.start + 440);
        expectEqual(secondFree.automaticAdmissions[0].cardId,
                    std::string("GV004"),
                    "Queue GV den sau khong duoc phuc vu dung luot");
        expectEqual(system.waitingCount(), static_cast<std::size_t>(2), "Mat Queue GV/SV");

        const auto thirdFree = system.checkOut("GV004", scenario.start + 445);
        expectEqual(thirdFree.automaticAdmissions[0].cardId,
                    std::string("GV005"),
                    "Queue GV khong duy tri FIFO sau hai lan giai phong");
        expectEqual(system.waitingCount(), static_cast<std::size_t>(1), "Mat Queue SV");

        const auto nextLecturer = system.checkIn(makeRequest(
            "GV006", "51F188806", parking::UserRole::Lecturer,
            parking::VehicleType::Motorcycle, scenario.start + 450));
        expect(nextLecturer.relocation.has_value(), "Khong tao duoc plan ke tiep");
        expectEqual(nextLecturer.relocation->targetStudentCardId,
                    std::string("SVOVER"),
                    "Huy plan lam mat ung vien som nhat trong Heap");
        expectHealthy(system);
    });

    tests.run("Khu GV: uu tien Queue Giang vien truoc Sinh vien", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const parking::Timestamp start = parking::parseDateTime("2026-07-16 08:00:00");
        for (int index = 1; index <= 4; ++index) {
            expectEqual(system.checkIn(makeRequest(
                            "GV" + std::to_string(index),
                            "51F70000" + std::to_string(index),
                            parking::UserRole::Lecturer,
                            parking::VehicleType::Motorcycle,
                            start + index)).status,
                        parking::CheckInStatus::Parked,
                        "Khong lap day khu GV");
        }
        for (int index = 1; index <= 3; ++index) {
            expectEqual(system.checkIn(makeRequest(
                            "SV" + std::to_string(index),
                            "59X60000" + std::to_string(index),
                            parking::UserRole::Student,
                            parking::VehicleType::Motorcycle,
                            start + 10 + index)).status,
                        parking::CheckInStatus::Parked,
                        "Khong lap day khu SV");
        }
        const auto gvWaiting = system.checkIn(makeRequest(
            "GVWAIT", "51F799991", parking::UserRole::Lecturer,
            parking::VehicleType::Motorcycle, start + 30));
        const auto svWaiting = system.checkIn(makeRequest(
            "SVWAIT", "59X699991", parking::UserRole::Student,
            parking::VehicleType::Motorcycle, start + 31));
        expectEqual(gvWaiting.status, parking::CheckInStatus::Queued, "GV phai cho");
        expectEqual(svWaiting.status, parking::CheckInStatus::Queued, "SV phai cho");

        const auto out = system.checkOut("GV1", start + 3600);
        expectEqual(out.automaticAdmissions[0].cardId,
                    std::string("GVWAIT"),
                    "Khong uu tien Queue GV");
        expectEqual(system.waitingCount(), static_cast<std::size_t>(1), "Mat Queue SV");
        expectHealthy(system);
    });

    tests.run("Khu tam uu tien truoc Queue", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const MotorcycleScenario scenario = prepareRelocationScenario(system, true);
        const auto lecturer = system.checkIn(makeRequest(
            "GV003", "51F188803", parking::UserRole::Lecturer,
            parking::VehicleType::Motorcycle, scenario.start + 420));
        expect(system.confirmRelocation(lecturer.relocation->planId,
                                        scenario.start + 430).success,
               "Di doi that bai");
        expectEqual(system.waitingCount(), static_cast<std::size_t>(1), "Sai Queue ban dau");

        const auto firstOut = system.checkOut("SV001", scenario.start + 4000);
        expectEqual(firstOut.automaticAdmissions[0].cardId,
                    std::string("SVOVER"),
                    "Khu tam khong duoc uu tien");
        expect(firstOut.automaticAdmissions[0].fromHolding, "Khong danh dau fromHolding");
        expectEqual(system.waitingCount(), static_cast<std::size_t>(1), "Queue bi lay qua som");

        const auto secondOut = system.checkOut("SV002", scenario.start + 4100);
        expectEqual(secondOut.automaticAdmissions[0].cardId,
                    std::string("SVWAIT"),
                    "Queue khong duoc phuc vu sau khu tam");
        expectEqual(system.waitingCount(), static_cast<std::size_t>(0), "Queue chua rong");
        expectHealthy(system);
    });

    tests.run("Fuzzy Search top-3 active vehicle", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const MotorcycleScenario scenario = prepareRelocationScenario(system, false);
        (void)scenario;
        const auto matches = system.fuzzySearch("59-X1.123.4S", 3);
        expectEqual(matches.size(), static_cast<std::size_t>(3), "Phai co top-3");
        expectEqual(matches[0].plate, std::string("59X112345"), "Fuzzy top-1 sai");
        expectEqual(matches[0].editDistance, 1, "Edit distance sai");
        expectEqual(matches[1].plate, std::string("59X112346"), "Fuzzy top-2 sai");
        expectEqual(matches[1].editDistance, 1, "Distance top-2 sai");
        expectEqual(matches[2].plate, std::string("59X112355"), "Fuzzy top-3 sai");
        expectEqual(matches[2].editDistance, 2, "Distance top-3 sai");
        expectEqual(system.fuzzySearch("59-X1.123.4S", 2).size(),
                    static_cast<std::size_t>(2),
                    "Khong ton trong limit");
        expectThrows([&] { system.fuzzySearch("---", 3); },
                     "Query rong sau chuan hoa phai loi");
    });

    tests.run("O to: overflow, relocation, rehome va tinh phi end-to-end", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const parking::Timestamp start = parking::parseDateTime("2026-07-16 08:00:00");
        expectEqual(system.checkIn(makeRequest(
                        "GVC1", "51A900001", parking::UserRole::Lecturer,
                        parking::VehicleType::Car, start)).status,
                    parking::CheckInStatus::Parked,
                    "O to GV khong vao LC01");
        expectEqual(system.checkIn(makeRequest(
                        "SVC1", "59A800001", parking::UserRole::Student,
                        parking::VehicleType::Car, start + 60)).status,
                    parking::CheckInStatus::Parked,
                    "O to SV1 khong vao SC01");
        expectEqual(system.checkIn(makeRequest(
                        "SVC2", "59A800002", parking::UserRole::Student,
                        parking::VehicleType::Car, start + 120)).status,
                    parking::CheckInStatus::Parked,
                    "O to SV2 khong vao SC02");
        const auto overflowCar = system.checkIn(makeRequest(
            "SVCOVER", "59A800003", parking::UserRole::Student,
            parking::VehicleType::Car, start + 180));
        expect(overflowCar.overflow && overflowCar.slotId == "LC02",
               "O to SV khong overflow vao LC02");

        const auto lecturerCar = system.checkIn(makeRequest(
            "GVC2", "51A900002", parking::UserRole::Lecturer,
            parking::VehicleType::Car, start + 240));
        expect(lecturerCar.relocation.has_value(), "O to GV khong tao relocation");
        expectEqual(lecturerCar.relocation->targetStudentCardId,
                    std::string("SVCOVER"),
                    "Heap o to chon sai target");
        expectEqual(lecturerCar.relocation->temporaryRemovalOrder[0].slotId,
                    std::string("LC01"),
                    "Stack o to thieu blocker LC01");
        expect(system.confirmRelocation(lecturerCar.relocation->planId,
                                        start + 300).success,
               "Confirm relocation o to that bai");

        const auto studentOut = system.checkOut("SVC1", start + 3000);
        expectEqual(studentOut.automaticAdmissions[0].cardId,
                    std::string("SVCOVER"),
                    "O to khu tam khong duoc rehome");
        const auto lecturerOut = system.checkOut("GVC2", start + 300 + 61 * 60);
        expectEqual(lecturerOut.fee,
                    static_cast<parking::Money>(30000),
                    "Phi o to 61 phut phai la 30.000 VND");
        expectHealthy(system);
    });

    tests.run("Queue tach theo loai xe, khong head-of-line blocking", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const parking::Timestamp start = parking::parseDateTime("2026-07-16 08:00:00");
        for (int index = 1; index <= 2; ++index) {
            system.checkIn(makeRequest("GVC" + std::to_string(index),
                                       "51A90000" + std::to_string(index),
                                       parking::UserRole::Lecturer,
                                       parking::VehicleType::Car,
                                       start + index));
            system.checkIn(makeRequest("SVC" + std::to_string(index),
                                       "59A80000" + std::to_string(index),
                                       parking::UserRole::Student,
                                       parking::VehicleType::Car,
                                       start + 10 + index));
        }
        const auto carWaiting = system.checkIn(makeRequest(
            "CARWAIT", "59A899999", parking::UserRole::Student,
            parking::VehicleType::Car, start + 20));
        expectEqual(carWaiting.status, parking::CheckInStatus::Queued, "O to phai cho");
        const auto motorcycle = system.checkIn(makeRequest(
            "MOTO", "59X123499", parking::UserRole::Student,
            parking::VehicleType::Motorcycle, start + 21));
        expectEqual(motorcycle.status,
                    parking::CheckInStatus::Parked,
                    "Queue o to da chan xe may");
        expectHealthy(system);
    });

    tests.run("History dung MergeSort va invariant sau chuoi thao tac", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const parking::Timestamp start = parking::parseDateTime("2026-07-16 08:00:00");
        system.checkIn(makeRequest("A", "59X111111", parking::UserRole::Student,
                                   parking::VehicleType::Motorcycle, start + 20));
        system.checkIn(makeRequest("B", "59X111112", parking::UserRole::Student,
                                   parking::VehicleType::Motorcycle, start + 10));
        system.checkOut("A", start + 3700);
        const auto history = system.historySortedByTime();
        for (std::size_t index = 1; index < history.size(); ++index) {
            expect(history[index - 1].time <= history[index].time,
                   "Lich su khong sap theo thoi gian");
        }
        const auto byFee = system.historySortedByFeeDescending();
        for (std::size_t index = 1; index < byFee.size(); ++index) {
            expect(byFee[index - 1].fee >= byFee[index].fee,
                   "Lich su khong sap phi giam dan");
        }
        expectHealthy(system);
    });

    tests.run("Input validation va graph unreachable", [&] {
        parking::ParkingSystem system;
        load(system, dataDirectory);
        const parking::Timestamp start = parking::parseDateTime("2026-07-16 08:00:00");
        const auto invalidPlate = system.checkIn(makeRequest(
            "CARD", "---", parking::UserRole::Student,
            parking::VehicleType::Motorcycle, start));
        expectEqual(invalidPlate.status,
                    parking::CheckInStatus::Rejected,
                    "Khong chan bien so rong");
        auto badGate = makeRequest("CARD2", "59X123456", parking::UserRole::Student,
                                   parking::VehicleType::Motorcycle, start);
        badGate.gateId = "NO_GATE";
        expectEqual(system.checkIn(badGate).status,
                    parking::CheckInStatus::Rejected,
                    "Khong chan gate sai");
        expect(!system.graph().shortestPathToNode("NO_GATE", "SM01").found,
               "Graph sai ma van tim duong");
        expectThrows([] { (void)parking::parseDateTime("2026-02-31 08:00"); },
                     "Chap nhan ngay 31/02");
        expectThrows([] { (void)parking::parseDateTime("2026-13-01 08:00"); },
                     "Chap nhan thang 13");
        expectThrows([] { (void)parking::parseDateTime("2026-07-16 08:00 abc"); },
                     "Chap nhan ky tu thua sau thoi gian");
    });

    tests.run("Stress 10000 khoa HashTable", [] {
        parking::ds::HashTable<std::string, int, parking::ds::StringHasher> table(5);
        for (int index = 0; index < 10000; ++index) {
            table.insert("CARD-" + std::to_string(index), index);
        }
        expectEqual(table.size(), static_cast<std::size_t>(10000), "Mat khoa khi stress");
        for (int index = 0; index < 10000; index += 137) {
            const int* value = table.find("CARD-" + std::to_string(index));
            expect(value != nullptr && *value == index, "Find stress sai");
        }
    });

    return tests.finish();
}
