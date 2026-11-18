#include "io/DataLoader.hpp"

#include "domain/Types.hpp"

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <utility>

namespace parking {
namespace {

[[noreturn]] void lineError(const std::string& file,
                            std::size_t line,
                            const std::string& message) {
    throw std::runtime_error(file + ": dong " + std::to_string(line) + ": " + message);
}

bool parseBoolean(const std::string& text, bool& output) {
    const std::string value = toUpperAscii(trim(text));
    if (value == "1" || value == "TRUE" || value == "YES") {
        output = true;
        return true;
    }
    if (value == "0" || value == "FALSE" || value == "NO") {
        output = false;
        return true;
    }
    return false;
}

std::int64_t parsePositiveInteger(const std::string& text,
                                  const std::string& file,
                                  std::size_t line,
                                  const std::string& field) {
    try {
        std::size_t consumed = 0;
        const std::int64_t value = std::stoll(trim(text), &consumed);
        if (consumed != trim(text).size() || value <= 0) {
            lineError(file, line, field + " phai la so nguyen duong");
        }
        return value;
    } catch (const std::invalid_argument&) {
        lineError(file, line, field + " khong phai so nguyen");
    } catch (const std::out_of_range&) {
        lineError(file, line, field + " vuot mien so nguyen 64-bit");
    }
}

}  // namespace

std::vector<std::string> DataLoader::parseCsvLine(const std::string& line) {
    std::vector<std::string> fields;
    std::string current;
    bool insideQuotes = false;

    for (std::size_t index = 0; index < line.size(); ++index) {
        const char character = line[index];
        if (character == '"') {
            if (insideQuotes && index + 1 < line.size() && line[index + 1] == '"') {
                current.push_back('"');
                ++index;
            } else {
                insideQuotes = !insideQuotes;
            }
        } else if (character == ',' && !insideQuotes) {
            fields.push_back(trim(current));
            current.clear();
        } else if (character != '\r') {
            current.push_back(character);
        }
    }
    if (insideQuotes) {
        throw std::invalid_argument("Thieu dau nhay dong trong dong CSV");
    }
    fields.push_back(trim(current));
    return fields;
}

void DataLoader::loadParkingMap(ParkingGraph& graph,
                                const std::string& nodesFile,
                                const std::string& edgesFile) {
    ParkingGraph loaded;

    std::ifstream nodesInput(nodesFile);
    if (!nodesInput) {
        throw std::runtime_error("Khong mo duoc file node: " + nodesFile);
    }
    std::string line;
    std::size_t lineNumber = 0;
    bool skippedHeader = false;
    while (std::getline(nodesInput, line)) {
        ++lineNumber;
        if (trim(line).empty() || trim(line).front() == '#') {
            continue;
        }
        if (!skippedHeader) {
            skippedHeader = true;
            continue;
        }

        std::vector<std::string> fields;
        try {
            fields = parseCsvLine(line);
        } catch (const std::exception& error) {
            lineError(nodesFile, lineNumber, error.what());
        }
        if (fields.size() != 7) {
            lineError(nodesFile,
                      lineNumber,
                      "Can 7 cot: id,label,type,zone,vehicle_type,lane,depth");
        }

        ParkingNode node;
        node.id = fields[0];
        node.label = fields[1];
        if (!parseNodeType(fields[2], node.type)) {
            lineError(nodesFile, lineNumber, "type khong hop le: " + fields[2]);
        }
        if (!parseZone(fields[3], node.zone)) {
            lineError(nodesFile, lineNumber, "zone khong hop le: " + fields[3]);
        }
        if (!parseVehicleType(fields[4], node.acceptedVehicle)) {
            lineError(nodesFile, lineNumber, "vehicle_type khong hop le: " + fields[4]);
        }
        node.laneId = trim(fields[5]);
        if (!trim(fields[6]).empty()) {
            const std::string depthText = trim(fields[6]);
            std::size_t consumed = 0;
            try {
                node.depthFromExit = std::stoi(depthText, &consumed);
            } catch (const std::exception&) {
                lineError(nodesFile, lineNumber, "depth khong phai so nguyen");
            }
            if (consumed != depthText.size()) {
                lineError(nodesFile, lineNumber, "depth co ky tu thua");
            }
            if (node.depthFromExit < 0) {
                lineError(nodesFile, lineNumber, "depth khong duoc am");
            }
        }
        try {
            loaded.addNode(std::move(node));
        } catch (const std::exception& error) {
            lineError(nodesFile, lineNumber, error.what());
        }
    }
    if (!skippedHeader || loaded.nodeCount() == 0) {
        throw std::runtime_error("File node khong co du lieu: " + nodesFile);
    }

    std::ifstream edgesInput(edgesFile);
    if (!edgesInput) {
        throw std::runtime_error("Khong mo duoc file canh: " + edgesFile);
    }
    lineNumber = 0;
    skippedHeader = false;
    while (std::getline(edgesInput, line)) {
        ++lineNumber;
        if (trim(line).empty() || trim(line).front() == '#') {
            continue;
        }
        if (!skippedHeader) {
            skippedHeader = true;
            continue;
        }

        std::vector<std::string> fields;
        try {
            fields = parseCsvLine(line);
        } catch (const std::exception& error) {
            lineError(edgesFile, lineNumber, error.what());
        }
        if (fields.size() != 5) {
            lineError(edgesFile,
                      lineNumber,
                      "Can 5 cot: from,to,distance_m,bidirectional,enabled");
        }
        bool bidirectional = true;
        bool enabled = true;
        if (!parseBoolean(fields[3], bidirectional)) {
            lineError(edgesFile, lineNumber, "bidirectional khong hop le");
        }
        if (!parseBoolean(fields[4], enabled)) {
            lineError(edgesFile, lineNumber, "enabled khong hop le");
        }
        const std::int64_t distance =
            parsePositiveInteger(fields[2], edgesFile, lineNumber, "distance_m");
        try {
            loaded.addEdge(fields[0], fields[1], distance, bidirectional, enabled);
        } catch (const std::exception& error) {
            lineError(edgesFile, lineNumber, error.what());
        }
    }
    if (!skippedHeader || loaded.edgeCount() == 0) {
        throw std::runtime_error("File canh khong co du lieu: " + edgesFile);
    }

    // Chỉ thay trạng thái thật sau khi cả hai file hợp lệ (transactional load).
    graph = std::move(loaded);
}

std::vector<CameraReading> DataLoader::loadCameraReadings(const std::string& fileName) {
    std::ifstream input(fileName);
    if (!input) {
        throw std::runtime_error("Khong mo duoc file camera: " + fileName);
    }

    std::vector<CameraReading> readings;
    std::string line;
    std::size_t lineNumber = 0;
    bool skippedHeader = false;
    while (std::getline(input, line)) {
        ++lineNumber;
        if (trim(line).empty() || trim(line).front() == '#') {
            continue;
        }
        if (!skippedHeader) {
            skippedHeader = true;
            continue;
        }
        std::vector<std::string> fields;
        try {
            fields = parseCsvLine(line);
        } catch (const std::exception& error) {
            lineError(fileName, lineNumber, error.what());
        }
        if (fields.size() != 3) {
            lineError(fileName, lineNumber, "Can 3 cot: timestamp,camera_id,recognized_plate");
        }
        try {
            readings.push_back(CameraReading{parseDateTime(fields[0]), fields[1], fields[2]});
        } catch (const std::exception& error) {
            lineError(fileName, lineNumber, error.what());
        }
    }
    return readings;
}

FeePolicy DataLoader::loadFeePolicy(const std::string& fileName) {
    std::ifstream input(fileName);
    if (!input) {
        throw std::runtime_error("Khong mo duoc file cau hinh phi: " + fileName);
    }

    FeePolicy policy;
    bool foundMotorcycle = false;
    bool foundCar = false;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        const std::string clean = trim(line);
        if (clean.empty() || clean.front() == '#') {
            continue;
        }
        const std::size_t equals = clean.find('=');
        if (equals == std::string::npos) {
            lineError(fileName, lineNumber, "Cau hinh phai co dang key=value");
        }
        const std::string key = toUpperAscii(trim(clean.substr(0, equals)));
        const Money value = parsePositiveInteger(
            clean.substr(equals + 1), fileName, lineNumber, key);
        if (key == "MOTORCYCLE_PER_STARTED_HOUR") {
            policy.motorcyclePerStartedHour = value;
            foundMotorcycle = true;
        } else if (key == "CAR_PER_STARTED_HOUR") {
            policy.carPerStartedHour = value;
            foundCar = true;
        } else {
            lineError(fileName, lineNumber, "Khoa cau hinh khong ho tro: " + key);
        }
    }
    if (!foundMotorcycle || !foundCar) {
        throw std::runtime_error("File phi phai khai bao du phi xe may va o to");
    }
    return policy;
}

}  // namespace parking
