#pragma once

#include "domain/Models.hpp"
#include "graph/ParkingGraph.hpp"

#include <string>
#include <vector>

namespace parking {

class DataLoader {
public:
    static void loadParkingMap(ParkingGraph& graph,
                               const std::string& nodesFile,
                               const std::string& edgesFile);
    static std::vector<CameraReading> loadCameraReadings(const std::string& fileName);
    static FeePolicy loadFeePolicy(const std::string& fileName);

    static std::vector<std::string> parseCsvLine(const std::string& line);
};

}  // namespace parking
