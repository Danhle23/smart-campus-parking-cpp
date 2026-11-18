#include "graph/ParkingGraph.hpp"

#include "ds/BinaryMinHeap.hpp"
#include "ds/LinkedQueue.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace parking {
namespace {

constexpr std::size_t NO_PARENT = std::numeric_limits<std::size_t>::max();
constexpr std::int64_t INF = std::numeric_limits<std::int64_t>::max();

struct DijkstraState {
    std::int64_t distance{0};
    std::size_t nodeIndex{0};
    std::string nodeId;
};

struct DijkstraStateEarlier {
    bool operator()(const DijkstraState& left, const DijkstraState& right) const {
        if (left.distance != right.distance) {
            return left.distance < right.distance;
        }
        return left.nodeId < right.nodeId;
    }
};

bool accepts(VehicleType slotType, VehicleType requestedType) {
    return slotType == VehicleType::Any || slotType == requestedType;
}

}  // namespace

void ParkingGraph::addNode(ParkingNode node) {
    node.id = trim(node.id);
    if (node.id.empty()) {
        throw std::invalid_argument("Ma node khong duoc rong");
    }
    if (indexById_.contains(node.id)) {
        throw std::invalid_argument("Trung ma node: " + node.id);
    }
    if (node.type != NodeType::Slot) {
        node.zone = Zone::None;
        node.acceptedVehicle = VehicleType::Any;
        node.laneId.clear();
        node.depthFromExit = 0;
        node.occupiedCardId.clear();
    }
    if (node.type == NodeType::Slot && node.zone == Zone::None) {
        throw std::invalid_argument("O do " + node.id + " phai thuoc mot khu");
    }

    const std::size_t index = nodes_.size();
    indexById_.insert(node.id, index);
    nodes_.push_back(std::move(node));
    adjacency_.emplace_back();
}

void ParkingGraph::addEdge(const std::string& fromId,
                           const std::string& toId,
                           std::int64_t distanceMeters,
                           bool bidirectional,
                           bool enabled) {
    if (distanceMeters <= 0) {
        throw std::invalid_argument("Trong so canh phai la so duong");
    }
    const std::size_t* from = indexById_.find(trim(fromId));
    const std::size_t* to = indexById_.find(trim(toId));
    if (from == nullptr || to == nullptr) {
        throw std::invalid_argument("Canh tham chieu node khong ton tai: " + fromId + " -> " + toId);
    }

    adjacency_[*from].push_back(Edge{*to, distanceMeters, enabled});
    ++edgeCount_;
    if (bidirectional) {
        adjacency_[*to].push_back(Edge{*from, distanceMeters, enabled});
        ++edgeCount_;
    }
}

ParkingNode* ParkingGraph::findNode(const std::string& id) {
    const std::size_t* index = indexById_.find(trim(id));
    return index == nullptr ? nullptr : &nodes_[*index];
}

const ParkingNode* ParkingGraph::findNode(const std::string& id) const {
    const std::size_t* index = indexById_.find(trim(id));
    return index == nullptr ? nullptr : &nodes_[*index];
}

bool ParkingGraph::occupySlot(const std::string& slotId, const std::string& cardId) {
    ParkingNode* slot = findNode(slotId);
    if (slot == nullptr || !slot->isFreeSlot() || cardId.empty()) {
        return false;
    }
    slot->occupiedCardId = cardId;
    return true;
}

bool ParkingGraph::releaseSlot(const std::string& slotId, const std::string& expectedCardId) {
    ParkingNode* slot = findNode(slotId);
    if (slot == nullptr || slot->type != NodeType::Slot || slot->occupiedCardId.empty()) {
        return false;
    }
    if (!expectedCardId.empty() && slot->occupiedCardId != expectedCardId) {
        return false;
    }
    slot->occupiedCardId.clear();
    return true;
}

bool ParkingGraph::slotMatches(const ParkingNode& node,
                               Zone zone,
                               VehicleType vehicleType) const {
    return node.isFreeSlot() && node.zone == zone && accepts(node.acceptedVehicle, vehicleType);
}

ParkingGraph::DijkstraData ParkingGraph::runDijkstra(std::size_t startIndex) const {
    DijkstraData result;
    result.distance.assign(nodes_.size(), INF);
    result.parent.assign(nodes_.size(), NO_PARENT);

    ds::BinaryMinHeap<DijkstraState, DijkstraStateEarlier> frontier;
    result.distance[startIndex] = 0;
    frontier.push(DijkstraState{0, startIndex, nodes_[startIndex].id});

    while (!frontier.empty()) {
        const std::optional<DijkstraState> currentOptional = frontier.pop();
        if (!currentOptional.has_value()) {
            break;
        }
        const DijkstraState current = *currentOptional;
        if (current.distance != result.distance[current.nodeIndex]) {
            continue;  // Bản ghi cũ do không dùng decrease-key.
        }

        for (const Edge& edge : adjacency_[current.nodeIndex]) {
            if (!edge.enabled || !nodes_[edge.to].enabled) {
                continue;
            }
            if (current.distance > INF - edge.distanceMeters) {
                continue;  // Chặn tràn số khi cộng khoảng cách.
            }
            const std::int64_t candidate = current.distance + edge.distanceMeters;
            const bool strictlyBetter = candidate < result.distance[edge.to];
            const bool sameButBetterParent =
                candidate == result.distance[edge.to] &&
                (result.parent[edge.to] == NO_PARENT ||
                 nodes_[current.nodeIndex].id < nodes_[result.parent[edge.to]].id);

            if (strictlyBetter || sameButBetterParent) {
                result.distance[edge.to] = candidate;
                result.parent[edge.to] = current.nodeIndex;
                frontier.push(DijkstraState{candidate, edge.to, nodes_[edge.to].id});
            }
        }
    }
    return result;
}

RouteResult ParkingGraph::buildRoute(std::size_t startIndex,
                                     std::size_t targetIndex,
                                     const DijkstraData& data,
                                     const std::string& algorithm) const {
    RouteResult route;
    route.algorithm = algorithm;
    if (targetIndex >= nodes_.size() ||
        (targetIndex != startIndex && data.parent[targetIndex] == NO_PARENT)) {
        return route;
    }

    std::vector<std::string> reversed;
    std::size_t current = targetIndex;
    while (true) {
        reversed.push_back(nodes_[current].id);
        if (current == startIndex) {
            break;
        }
        current = data.parent[current];
        if (current == NO_PARENT) {
            return RouteResult{};
        }
    }
    std::reverse(reversed.begin(), reversed.end());

    route.found = true;
    route.totalDistance = data.distance[targetIndex];
    route.path = std::move(reversed);
    if (nodes_[targetIndex].type == NodeType::Slot) {
        route.destinationSlotId = nodes_[targetIndex].id;
    }
    return route;
}

RouteResult ParkingGraph::shortestPathToNearestSlot(const std::string& startId,
                                                    Zone zone,
                                                    VehicleType vehicleType) const {
    const std::size_t* start = indexById_.find(trim(startId));
    if (start == nullptr || !nodes_[*start].enabled) {
        return RouteResult{};
    }
    const DijkstraData data = runDijkstra(*start);

    std::size_t best = NO_PARENT;
    for (std::size_t index = 0; index < nodes_.size(); ++index) {
        const bool reachable = index == *start || data.parent[index] != NO_PARENT;
        if (!slotMatches(nodes_[index], zone, vehicleType) || !reachable) {
            continue;
        }
        if (best == NO_PARENT || data.distance[index] < data.distance[best] ||
            (data.distance[index] == data.distance[best] && nodes_[index].id < nodes_[best].id)) {
            best = index;
        }
    }
    return best == NO_PARENT
               ? RouteResult{}
               : buildRoute(*start, best, data, "DIJKSTRA");
}

RouteResult ParkingGraph::shortestPathToNode(const std::string& startId,
                                             const std::string& targetId) const {
    const std::size_t* start = indexById_.find(trim(startId));
    const std::size_t* target = indexById_.find(trim(targetId));
    if (start == nullptr || target == nullptr || !nodes_[*start].enabled ||
        !nodes_[*target].enabled) {
        return RouteResult{};
    }
    const DijkstraData data = runDijkstra(*start);
    return buildRoute(*start, *target, data, "DIJKSTRA");
}

RouteResult ParkingGraph::bfsToNearestSlot(const std::string& startId,
                                           Zone zone,
                                           VehicleType vehicleType) const {
    const std::size_t* start = indexById_.find(trim(startId));
    if (start == nullptr || !nodes_[*start].enabled) {
        return RouteResult{};
    }

    std::vector<int> depth(nodes_.size(), -1);
    std::vector<std::size_t> parent(nodes_.size(), NO_PARENT);
    std::vector<std::int64_t> chosenPathMeters(nodes_.size(), 0);
    ds::LinkedQueue<std::size_t> frontier;
    depth[*start] = 0;
    frontier.enqueue(*start);

    while (!frontier.empty()) {
        const std::optional<std::size_t> currentOptional = frontier.dequeue();
        if (!currentOptional.has_value()) {
            break;
        }
        const std::size_t current = *currentOptional;
        for (const Edge& edge : adjacency_[current]) {
            if (!edge.enabled || !nodes_[edge.to].enabled || depth[edge.to] != -1) {
                continue;
            }
            depth[edge.to] = depth[current] + 1;
            parent[edge.to] = current;
            chosenPathMeters[edge.to] =
                chosenPathMeters[current] > INF - edge.distanceMeters
                    ? INF
                    : chosenPathMeters[current] + edge.distanceMeters;
            frontier.enqueue(edge.to);
        }
    }

    std::size_t best = NO_PARENT;
    for (std::size_t index = 0; index < nodes_.size(); ++index) {
        if (depth[index] < 0 || !slotMatches(nodes_[index], zone, vehicleType)) {
            continue;
        }
        if (best == NO_PARENT || depth[index] < depth[best] ||
            (depth[index] == depth[best] && nodes_[index].id < nodes_[best].id)) {
            best = index;
        }
    }
    if (best == NO_PARENT) {
        return RouteResult{};
    }

    std::vector<std::string> reversed;
    for (std::size_t current = best;; current = parent[current]) {
        reversed.push_back(nodes_[current].id);
        if (current == *start) {
            break;
        }
    }
    std::reverse(reversed.begin(), reversed.end());
    return RouteResult{true,
                       chosenPathMeters[best],
                       std::move(reversed),
                       nodes_[best].id,
                       "BFS_SO_CANH"};
}

bool ParkingGraph::allEnabledEdgesHaveSameWeight() const {
    std::optional<std::int64_t> firstWeight;
    for (const auto& edges : adjacency_) {
        for (const Edge& edge : edges) {
            if (!edge.enabled) {
                continue;
            }
            if (!firstWeight.has_value()) {
                firstWeight = edge.distanceMeters;
            } else if (*firstWeight != edge.distanceMeters) {
                return false;
            }
        }
    }
    return true;
}

}  // namespace parking
