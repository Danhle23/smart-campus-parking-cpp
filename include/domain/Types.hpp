#pragma once

#include <cstdint>
#include <string>

namespace parking {

using Timestamp = std::int64_t;  // Số giây tính từ Unix epoch.
using Money = std::int64_t;      // Đơn vị VND; tránh sai số số thực.

enum class UserRole { Student, Lecturer };
enum class VehicleType { Motorcycle, Car, Any };
enum class Zone { None, Student, Lecturer };
enum class NodeType { Gate, Junction, Slot };
enum class SessionStatus { Parked, Holding };
enum class CheckInStatus { Parked, Queued, RelocationRequired, Rejected };

std::string toString(UserRole value);
std::string toString(VehicleType value);
std::string toString(Zone value);
std::string toString(NodeType value);
std::string toString(SessionStatus value);
std::string toString(CheckInStatus value);

bool parseUserRole(const std::string& text, UserRole& output);
bool parseVehicleType(const std::string& text, VehicleType& output);
bool parseZone(const std::string& text, Zone& output);
bool parseNodeType(const std::string& text, NodeType& output);

std::string trim(const std::string& text);
std::string toUpperAscii(const std::string& text);
std::string normalizeCardId(const std::string& cardId);
std::string normalizePlate(const std::string& plate);
bool isValidNormalizedPlate(const std::string& plate);

Timestamp parseDateTime(const std::string& text);
std::string formatDateTime(Timestamp value);

}  // namespace parking
