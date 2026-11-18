#pragma once

#include "domain/Types.hpp"

#include <cstddef>
#include <optional>
#include <string>
#include <vector>

namespace parking {

struct ParkingNode {
    std::string id;
    std::string label;
    NodeType type{NodeType::Junction};
    Zone zone{Zone::None};
    VehicleType acceptedVehicle{VehicleType::Any};
    std::string laneId;
    int depthFromExit{0};
    bool enabled{true};
    std::string occupiedCardId;

    [[nodiscard]] bool isFreeSlot() const {
        return enabled && type == NodeType::Slot && occupiedCardId.empty();
    }
};

struct Edge {
    std::size_t to{0};
    std::int64_t distanceMeters{0};
    bool enabled{true};
};

struct RouteResult {
    bool found{false};
    std::int64_t totalDistance{0};
    std::vector<std::string> path;
    std::string destinationSlotId;
    std::string algorithm;
};

struct VehicleRequest {
    std::string cardId;
    std::string plate;
    UserRole role{UserRole::Student};
    VehicleType vehicleType{VehicleType::Motorcycle};
    std::string gateId;
    Timestamp requestedAt{0};
    std::uint64_t sequence{0};
};

struct ParkingSession {
    std::string cardId;
    std::string plate;
    UserRole role{UserRole::Student};
    VehicleType vehicleType{VehicleType::Motorcycle};
    std::string gateId;
    Timestamp entryTime{0};
    std::string slotId;
    SessionStatus status{SessionStatus::Parked};
    bool overflow{false};
    std::uint64_t sequence{0};
};

struct OverflowEntry {
    Timestamp parkedAt{0};
    std::uint64_t sequence{0};
    std::string cardId;
    std::string slotId;
    VehicleType vehicleType{VehicleType::Motorcycle};
};

struct OverflowEarlier {
    bool operator()(const OverflowEntry& left, const OverflowEntry& right) const {
        if (left.parkedAt != right.parkedAt) {
            return left.parkedAt < right.parkedAt;
        }
        if (left.sequence != right.sequence) {
            return left.sequence < right.sequence;
        }
        return left.cardId < right.cardId;
    }
};

struct BlockerMove {
    std::string cardId;
    std::string plate;
    std::string slotId;
    int depthFromExit{0};
};

struct RelocationPlan {
    std::string planId;
    VehicleRequest lecturerRequest;
    std::string targetStudentCardId;
    std::string targetStudentPlate;
    std::string targetSlotId;
    std::vector<BlockerMove> temporaryRemovalOrder;
    std::vector<BlockerMove> restoreOrder;
};

struct CheckInResult {
    CheckInStatus status{CheckInStatus::Rejected};
    std::string message;
    std::string slotId;
    bool overflow{false};
    std::size_t queuePosition{0};
    RouteResult route;
    std::optional<RelocationPlan> relocation;
};

struct AdmissionNotice {
    std::string cardId;
    std::string plate;
    std::string slotId;
    bool fromHolding{false};
    bool overflow{false};
    RouteResult route;
};

struct CheckOutResult {
    bool success{false};
    std::string message;
    Money fee{0};
    std::int64_t parkedMinutes{0};
    std::string releasedSlotId;
    std::vector<AdmissionNotice> automaticAdmissions;
};

struct RelocationResult {
    bool success{false};
    std::string message;
    std::string movedStudentCardId;
    std::string lecturerCardId;
    std::string lecturerSlotId;
    RouteResult lecturerRoute;
    std::vector<BlockerMove> temporaryRemovalOrder;
    std::vector<BlockerMove> restoreOrder;
};

struct PlateMatch {
    std::string plate;
    std::string cardId;
    std::string slotId;
    int editDistance{0};
    double similarity{0.0};
};

struct CameraReading {
    Timestamp observedAt{0};
    std::string cameraId;
    std::string recognizedPlate;
};

struct FeePolicy {
    Money motorcyclePerStartedHour{5000};
    Money carPerStartedHour{15000};
};

struct HistoryEntry {
    std::uint64_t id{0};
    Timestamp time{0};
    std::string event;
    std::string cardId;
    std::string plate;
    std::string fromSlot;
    std::string toSlot;
    Money fee{0};
};

struct ParkingStatistics {
    std::size_t totalSlots{0};
    std::size_t occupiedSlots{0};
    std::size_t studentSlots{0};
    std::size_t lecturerSlots{0};
    std::size_t activeSessions{0};
    std::size_t holdingVehicles{0};
    std::size_t waitingVehicles{0};
};

}  // namespace parking
