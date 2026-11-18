#include "domain/Models.hpp"
#include "domain/Types.hpp"
#include "io/DataLoader.hpp"
#include "services/ParkingSystem.hpp"

#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {

using parking::CheckInResult;
using parking::CheckInStatus;
using parking::ParkingSystem;
using parking::Timestamp;
using parking::UserRole;
using parking::VehicleRequest;
using parking::VehicleType;
using parking::Zone;

std::string joinPath(const std::string& directory, const std::string& file) {
    if (directory.empty() || directory.back() == '/') {
        return directory + file;
    }
    return directory + '/' + file;
}

void loadSystem(ParkingSystem& system, const std::string& dataDirectory) {
    system.loadMap(joinPath(dataDirectory, "nodes.csv"),
                   joinPath(dataDirectory, "edges.csv"));
    system.loadFeePolicy(joinPath(dataDirectory, "fees.cfg"));
}

std::string moneyText(parking::Money value) {
    std::string digits = std::to_string(value);
    for (int index = static_cast<int>(digits.size()) - 3; index > 0; index -= 3) {
        digits.insert(static_cast<std::size_t>(index), ".");
    }
    return digits + " VND";
}

void printRoute(const parking::RouteResult& route) {
    if (!route.found) {
        std::cout << "  Khong tim thay duong di.\n";
        return;
    }
    std::cout << "  " << route.algorithm << ": ";
    for (std::size_t index = 0; index < route.path.size(); ++index) {
        if (index != 0) {
            std::cout << " -> ";
        }
        std::cout << route.path[index];
    }
    std::cout << " (" << route.totalDistance << " m)\n";
}

void printCheckIn(const VehicleRequest& request, const CheckInResult& result) {
    std::cout << "[" << request.cardId << " / " << request.plate << "] "
              << parking::toString(result.status) << " - " << result.message << '\n';
    if (result.status == CheckInStatus::Parked) {
        std::cout << "  O do: " << result.slotId
                  << (result.overflow ? " (DO TRAN KHU GV)" : "") << '\n';
        printRoute(result.route);
    } else if (result.status == CheckInStatus::Queued) {
        std::cout << "  Vi tri Queue: " << result.queuePosition << '\n';
    } else if (result.relocation.has_value()) {
        const parking::RelocationPlan& plan = *result.relocation;
        std::cout << "  Plan: " << plan.planId << ", di doi "
                  << plan.targetStudentPlate << " tai " << plan.targetSlotId << '\n';
        std::cout << "  Thu tu dua xe chan ra (push Stack): ";
        for (const auto& blocker : plan.temporaryRemovalOrder) {
            std::cout << blocker.slotId << ' ';
        }
        std::cout << "\n  Thu tu hoan nguyen (pop Stack): ";
        for (const auto& blocker : plan.restoreOrder) {
            std::cout << blocker.slotId << ' ';
        }
        std::cout << '\n';
    }
}

VehicleRequest request(std::string card,
                       std::string plate,
                       UserRole role,
                       VehicleType type,
                       Timestamp time) {
    return VehicleRequest{std::move(card),
                          std::move(plate),
                          role,
                          type,
                          "GATE_A",
                          time,
                          0};
}

void requireHealthy(const ParkingSystem& system, const std::string& step) {
    const std::vector<std::string> errors = system.validateInvariants();
    if (!errors.empty()) {
        std::cerr << "Invariant loi sau " << step << ":\n";
        for (const std::string& error : errors) {
            std::cerr << "  - " << error << '\n';
        }
        throw std::runtime_error("Demo dung do invariant bi vi pham");
    }
}

int runDemo(const std::string& dataDirectory) {
    ParkingSystem system;
    loadSystem(system, dataDirectory);
    const Timestamp start = parking::parseDateTime("2026-07-16 08:00:00");

    std::cout << "\n========== SMART CAMPUS PARKING - DEMO NANG CAO ==========\n";
    std::cout << "\n1) BFS va Dijkstra tren do thi co trong so\n";
    const auto bfs = system.graph().bfsToNearestSlot(
        "GATE_A", Zone::Lecturer, VehicleType::Motorcycle);
    const auto dijkstra = system.graph().shortestPathToNearestSlot(
        "GATE_A", Zone::Lecturer, VehicleType::Motorcycle);
    printRoute(bfs);
    printRoute(dijkstra);
    std::cout << "  Ket luan: BFS it canh hon nhung Dijkstra cho quang duong ngan hon.\n";

    std::cout << "\n2) Lap day 2 khu va tao xe Sinh vien do tran\n";
    std::vector<VehicleRequest> arrivals = {
        request("GV001", "51-F1.888.01", UserRole::Lecturer, VehicleType::Motorcycle, start),
        request("GV002", "51-F1.888.02", UserRole::Lecturer, VehicleType::Motorcycle, start + 60),
        request("SV001", "59-Y2.222.01", UserRole::Student, VehicleType::Motorcycle, start + 120),
        request("SV002", "60-B3.333.02", UserRole::Student, VehicleType::Motorcycle, start + 180),
        request("SV003", "59-X1.123.55", UserRole::Student, VehicleType::Motorcycle, start + 240),
        request("SV-OVER", "59-X1.123.45", UserRole::Student, VehicleType::Motorcycle, start + 300),
        request("SV-OVER2", "59-X1.123.46", UserRole::Student, VehicleType::Motorcycle, start + 330),
        request("SV-WAIT", "59-X1.123.47", UserRole::Student, VehicleType::Motorcycle, start + 360)};
    for (VehicleRequest& arrival : arrivals) {
        const CheckInResult result = system.checkIn(arrival);
        printCheckIn(arrival, result);
    }
    const auto hashLookup = system.findSession("SV-OVER");
    if (hashLookup.has_value()) {
        std::cout << "  Hash lookup card=SV-OVER -> plate=" << hashLookup->plate
                  << ", slot=" << hashLookup->slotId << '\n';
    }
    requireHealthy(system, "check-in va Queue");

    std::cout << "\n3) Giang vien den: Min-Heap chon SV do tran som nhat, Stack lap ke hoach\n";
    VehicleRequest lecturer = request(
        "GV003", "51-F1.888.03", UserRole::Lecturer, VehicleType::Motorcycle, start + 420);
    const CheckInResult lecturerResult = system.checkIn(lecturer);
    printCheckIn(lecturer, lecturerResult);
    if (!lecturerResult.relocation.has_value()) {
        throw std::runtime_error("Demo ky vong co ke hoach di doi");
    }

    const parking::RelocationResult moved =
        system.confirmRelocation(lecturerResult.relocation->planId, start + 430);
    std::cout << "  Xac nhan " << lecturerResult.relocation->planId << ": "
              << moved.message << '\n';
    printRoute(moved.lecturerRoute);
    requireHealthy(system, "xac nhan di doi");

    std::cout << "\n4) Camera doc sai va Fuzzy Search top-3 (Levenshtein)\n";
    const std::vector<parking::CameraReading> readings =
        parking::DataLoader::loadCameraReadings(joinPath(dataDirectory, "camera_logs.csv"));
    const std::string noisyPlate = readings.empty() ? "59-X1.123.4S"
                                                     : readings.front().recognizedPlate;
    std::cout << "  Camera: " << noisyPlate
              << " -> chuan hoa: " << parking::normalizePlate(noisyPlate) << '\n';
    const std::vector<parking::PlateMatch> matches = system.fuzzySearch(noisyPlate, 3);
    for (std::size_t index = 0; index < matches.size(); ++index) {
        std::cout << "  " << index + 1 << ". " << matches[index].plate
                  << " | distance=" << matches[index].editDistance
                  << " | similarity=" << std::fixed << std::setprecision(1)
                  << matches[index].similarity * 100.0 << "%"
                  << " | slot=" << matches[index].slotId << '\n';
    }

    std::cout << "\n5) Check-out: khu tam duoc uu tien, sau do moi den Queue\n";
    const auto firstOut = system.checkOut("SV001", start + 120 + 61 * 60);
    std::cout << "  SV001: " << firstOut.message << ", " << firstOut.parkedMinutes
              << " phut, phi=" << moneyText(firstOut.fee) << '\n';
    for (const auto& admission : firstOut.automaticAdmissions) {
        std::cout << "  Tu dong cap " << admission.slotId << " cho " << admission.cardId
                  << (admission.fromHolding ? " (tu khu tam)" : " (tu Queue)") << '\n';
    }

    const auto secondOut = system.checkOut("SV002", start + 180 + 62 * 60);
    std::cout << "  SV002: " << secondOut.message << ", phi="
              << moneyText(secondOut.fee) << '\n';
    for (const auto& admission : secondOut.automaticAdmissions) {
        std::cout << "  Tu dong cap " << admission.slotId << " cho " << admission.cardId
                  << (admission.fromHolding ? " (tu khu tam)" : " (tu Queue)") << '\n';
    }
    requireHealthy(system, "check-out va tu dong phan bo");

    const parking::ParkingStatistics stats = system.statistics();
    std::cout << "\n6) Tong ket trang thai\n"
              << "  Slots: " << stats.occupiedSlots << '/' << stats.totalSlots << " dang dung\n"
              << "  Active sessions: " << stats.activeSessions << '\n'
              << "  Holding: " << stats.holdingVehicles << '\n'
              << "  Waiting: " << stats.waitingVehicles << '\n'
              << "  Invariant: PASS\n"
              << "============================================================\n";
    return EXIT_SUCCESS;
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string value;
    std::getline(std::cin, value);
    return value;
}

Timestamp readTime(const std::string& prompt) {
    const std::string value = readLine(prompt + " [bo trong = hien tai]: ");
    if (parking::trim(value).empty()) {
        return static_cast<Timestamp>(
            std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
    }
    return parking::parseDateTime(value);
}

void showStatus(const ParkingSystem& system) {
    const auto stats = system.statistics();
    std::cout << "Tong o: " << stats.totalSlots << ", dang dung: " << stats.occupiedSlots
              << ", active: " << stats.activeSessions << ", khu tam: "
              << stats.holdingVehicles << ", Queue: " << stats.waitingVehicles << "\n";
    for (const auto& session : system.activeSessions()) {
        std::cout << "  " << session.cardId << " | " << session.plate << " | "
                  << parking::toString(session.role) << " | "
                  << parking::toString(session.status) << " | "
                  << (session.slotId.empty() ? "KHU_TAM" : session.slotId)
                  << (session.overflow ? " | OVERFLOW" : "") << '\n';
    }
}

int runInteractive(const std::string& dataDirectory) {
    ParkingSystem system;
    loadSystem(system, dataDirectory);

    while (true) {
        std::cout << "\n========== SMART CAMPUS PARKING ==========\n"
                  << "1. Check-in\n2. Check-out\n3. Fuzzy Search bien so\n"
                  << "4. Doc log camera va tim top-3\n5. Xem trang thai\n"
                  << "6. Xac nhan di doi\n7. So sanh BFS/Dijkstra\n"
                  << "8. Xem lich su\n9. Kiem tra invariant\n0. Thoat\n";
        const std::string choice = readLine("Chon: ");
        try {
            if (choice == "0") {
                return EXIT_SUCCESS;
            }
            if (choice == "1") {
                VehicleRequest input;
                input.cardId = readLine("Ma the: ");
                input.plate = readLine("Bien so: ");
                input.role = readLine("Doi tuong (1=SV, 2=GV): ") == "2"
                                 ? UserRole::Lecturer
                                 : UserRole::Student;
                input.vehicleType = readLine("Xe (1=xe may, 2=o to): ") == "2"
                                        ? VehicleType::Car
                                        : VehicleType::Motorcycle;
                input.gateId = readLine("Cong [GATE_A]: ");
                if (parking::trim(input.gateId).empty()) {
                    input.gateId = "GATE_A";
                }
                input.requestedAt = readTime("Thoi gian vao YYYY-MM-DD HH:MM");
                printCheckIn(input, system.checkIn(input));
            } else if (choice == "2") {
                const std::string card = readLine("Ma the: ");
                const auto result = system.checkOut(
                    card, readTime("Thoi gian ra YYYY-MM-DD HH:MM"));
                std::cout << result.message;
                if (result.success) {
                    std::cout << ", phi=" << moneyText(result.fee);
                }
                std::cout << '\n';
                for (const auto& admission : result.automaticAdmissions) {
                    std::cout << "  Tu dong cap " << admission.slotId << " cho "
                              << admission.cardId
                              << (admission.fromHolding ? " (tu khu tam)" : " (tu Queue)")
                              << '\n';
                    printRoute(admission.route);
                }
            } else if (choice == "3") {
                const auto matches = system.fuzzySearch(readLine("Bien so truy van: "), 3);
                for (const auto& match : matches) {
                    std::cout << match.plate << " | distance=" << match.editDistance
                              << " | card=" << match.cardId << " | slot=" << match.slotId
                              << '\n';
                }
            } else if (choice == "4") {
                const auto readings = parking::DataLoader::loadCameraReadings(
                    joinPath(dataDirectory, "camera_logs.csv"));
                for (const auto& reading : readings) {
                    std::cout << parking::formatDateTime(reading.observedAt) << " | "
                              << reading.cameraId << " | " << reading.recognizedPlate << '\n';
                    for (const auto& match : system.fuzzySearch(reading.recognizedPlate, 3)) {
                        std::cout << "    -> " << match.plate
                                  << " (d=" << match.editDistance << ")\n";
                    }
                }
            } else if (choice == "5") {
                showStatus(system);
            } else if (choice == "6") {
                if (!system.pendingRelocation().has_value()) {
                    std::cout << "Khong co ke hoach dang cho.\n";
                } else {
                    const auto result =
                        system.confirmRelocation(
                            system.pendingRelocation()->planId,
                            readTime("Thoi gian xac nhan YYYY-MM-DD HH:MM"));
                    std::cout << result.message << '\n';
                    if (!result.temporaryRemovalOrder.empty() ||
                        !result.restoreOrder.empty()) {
                        std::cout << "  Plan hien tai - dua ra: ";
                        for (const auto& blocker : result.temporaryRemovalOrder) {
                            std::cout << blocker.slotId << ' ';
                        }
                        std::cout << "\n  Hoan nguyen: ";
                        for (const auto& blocker : result.restoreOrder) {
                            std::cout << blocker.slotId << ' ';
                        }
                        std::cout << '\n';
                    }
                    if (result.success) {
                        printRoute(result.lecturerRoute);
                    }
                }
            } else if (choice == "7") {
                const std::string gate = readLine("Cong [GATE_A]: ");
                const std::string actualGate = parking::trim(gate).empty() ? "GATE_A" : gate;
                const Zone zone = readLine("Khu (1=SV, 2=GV): ") == "2"
                                      ? Zone::Lecturer
                                      : Zone::Student;
                const VehicleType type = readLine("Xe (1=xe may, 2=o to): ") == "2"
                                             ? VehicleType::Car
                                             : VehicleType::Motorcycle;
                printRoute(system.graph().bfsToNearestSlot(actualGate, zone, type));
                printRoute(system.graph().shortestPathToNearestSlot(actualGate, zone, type));
            } else if (choice == "8") {
                for (const auto& event : system.historySortedByTime()) {
                    std::cout << event.id << " | " << parking::formatDateTime(event.time)
                              << " | " << event.event << " | " << event.cardId << " | "
                              << event.fromSlot << " -> " << event.toSlot
                              << " | " << moneyText(event.fee) << '\n';
                }
            } else if (choice == "9") {
                const auto errors = system.validateInvariants();
                if (errors.empty()) {
                    std::cout << "PASS - tat ca invariant deu dung.\n";
                } else {
                    for (const std::string& error : errors) {
                        std::cout << "FAIL: " << error << '\n';
                    }
                }
            } else {
                std::cout << "Lua chon khong hop le.\n";
            }
        } catch (const std::exception& error) {
            std::cout << "LOI: " << error.what() << '\n';
        }
    }
}

}  // namespace

int main(int argc, char* argv[]) {
    std::string dataDirectory = "data";
    bool demo = false;
    for (int index = 1; index < argc; ++index) {
        const std::string argument = argv[index];
        if (argument == "--demo") {
            demo = true;
        } else if (argument == "--data-dir" && index + 1 < argc) {
            dataDirectory = argv[++index];
        } else if (argument == "--help") {
            std::cout << "Usage: smart_parking [--demo] [--data-dir PATH]\n";
            return EXIT_SUCCESS;
        } else {
            std::cerr << "Tham so khong hop le: " << argument << '\n';
            return EXIT_FAILURE;
        }
    }

    try {
        return demo ? runDemo(dataDirectory) : runInteractive(dataDirectory);
    } catch (const std::exception& error) {
        std::cerr << "LOI KHOI DONG: " << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
