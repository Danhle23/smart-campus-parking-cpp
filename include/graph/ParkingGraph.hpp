#pragma once

#include "domain/Models.hpp"
#include "ds/HashTable.hpp"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace parking {

// Đồ thị danh sách kề phù hợp bãi xe thưa: bộ nhớ O(V + E).
class ParkingGraph {
public:
    ParkingGraph() = default;
    ParkingGraph(const ParkingGraph&) = delete;
    ParkingGraph& operator=(const ParkingGraph&) = delete;
    ParkingGraph(ParkingGraph&& other) noexcept
        : nodes_(std::move(other.nodes_)),
          adjacency_(std::move(other.adjacency_)),
          indexById_(std::move(other.indexById_)),
          edgeCount_(std::exchange(other.edgeCount_, 0)) {}

    ParkingGraph& operator=(ParkingGraph&& other) noexcept {
        if (this != &other) {
            nodes_ = std::move(other.nodes_);
            adjacency_ = std::move(other.adjacency_);
            indexById_ = std::move(other.indexById_);
            edgeCount_ = std::exchange(other.edgeCount_, 0);
        }
        return *this;
    }

    void addNode(ParkingNode node);
    void addEdge(const std::string& fromId,
                 const std::string& toId,
                 std::int64_t distanceMeters,
                 bool bidirectional = true,
                 bool enabled = true);

    ParkingNode* findNode(const std::string& id);
    const ParkingNode* findNode(const std::string& id) const;

    bool occupySlot(const std::string& slotId, const std::string& cardId);
    bool releaseSlot(const std::string& slotId, const std::string& expectedCardId = {});

    RouteResult shortestPathToNearestSlot(const std::string& startId,
                                          Zone zone,
                                          VehicleType vehicleType) const;
    RouteResult shortestPathToNode(const std::string& startId,
                                   const std::string& targetId) const;
    RouteResult bfsToNearestSlot(const std::string& startId,
                                 Zone zone,
                                 VehicleType vehicleType) const;

    [[nodiscard]] bool allEnabledEdgesHaveSameWeight() const;
    [[nodiscard]] std::size_t nodeCount() const noexcept { return nodes_.size(); }
    [[nodiscard]] std::size_t edgeCount() const noexcept { return edgeCount_; }
    [[nodiscard]] const std::vector<ParkingNode>& nodes() const noexcept { return nodes_; }
    [[nodiscard]] std::vector<ParkingNode>& nodes() noexcept { return nodes_; }

private:
    struct DijkstraData {
        std::vector<std::int64_t> distance;
        std::vector<std::size_t> parent;
    };

    [[nodiscard]] DijkstraData runDijkstra(std::size_t startIndex) const;
    [[nodiscard]] RouteResult buildRoute(std::size_t startIndex,
                                         std::size_t targetIndex,
                                         const DijkstraData& data,
                                         const std::string& algorithm) const;
    [[nodiscard]] bool slotMatches(const ParkingNode& node,
                                   Zone zone,
                                   VehicleType vehicleType) const;

    std::vector<ParkingNode> nodes_;
    std::vector<std::vector<Edge>> adjacency_;
    ds::HashTable<std::string, std::size_t, ds::StringHasher> indexById_;
    std::size_t edgeCount_{0};
};

}  // namespace parking
