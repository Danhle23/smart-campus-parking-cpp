#include "domain/Types.hpp"

#include <algorithm>
#include <cctype>
#include <ctime>
#include <iomanip>
#include <optional>
#include <sstream>
#include <stdexcept>

namespace parking {
namespace {

std::string canonical(const std::string& text) {
    return toUpperAscii(trim(text));
}

}  // namespace

std::string toString(UserRole value) {
    return value == UserRole::Lecturer ? "LECTURER" : "STUDENT";
}

std::string toString(VehicleType value) {
    switch (value) {
        case VehicleType::Motorcycle:
            return "MOTORCYCLE";
        case VehicleType::Car:
            return "CAR";
        case VehicleType::Any:
            return "ANY";
    }
    return "ANY";
}

std::string toString(Zone value) {
    switch (value) {
        case Zone::Student:
            return "STUDENT";
        case Zone::Lecturer:
            return "LECTURER";
        case Zone::None:
            return "NONE";
    }
    return "NONE";
}

std::string toString(NodeType value) {
    switch (value) {
        case NodeType::Gate:
            return "GATE";
        case NodeType::Junction:
            return "JUNCTION";
        case NodeType::Slot:
            return "SLOT";
    }
    return "JUNCTION";
}

std::string toString(SessionStatus value) {
    return value == SessionStatus::Holding ? "HOLDING" : "PARKED";
}

std::string toString(CheckInStatus value) {
    switch (value) {
        case CheckInStatus::Parked:
            return "PARKED";
        case CheckInStatus::Queued:
            return "QUEUED";
        case CheckInStatus::RelocationRequired:
            return "RELOCATION_REQUIRED";
        case CheckInStatus::Rejected:
            return "REJECTED";
    }
    return "REJECTED";
}

bool parseUserRole(const std::string& text, UserRole& output) {
    const std::string value = canonical(text);
    if (value == "STUDENT" || value == "SINHVIEN" || value == "SV") {
        output = UserRole::Student;
        return true;
    }
    if (value == "LECTURER" || value == "GIANGVIEN" || value == "GV") {
        output = UserRole::Lecturer;
        return true;
    }
    return false;
}

bool parseVehicleType(const std::string& text, VehicleType& output) {
    const std::string value = canonical(text);
    if (value == "MOTORCYCLE" || value == "MOTORBIKE" || value == "XE MAY") {
        output = VehicleType::Motorcycle;
        return true;
    }
    if (value == "CAR" || value == "OTO" || value == "O TO") {
        output = VehicleType::Car;
        return true;
    }
    if (value == "ANY" || value == "NONE" || value.empty()) {
        output = VehicleType::Any;
        return true;
    }
    return false;
}

bool parseZone(const std::string& text, Zone& output) {
    const std::string value = canonical(text);
    if (value == "STUDENT" || value == "SINHVIEN" || value == "SV") {
        output = Zone::Student;
        return true;
    }
    if (value == "LECTURER" || value == "GIANGVIEN" || value == "GV") {
        output = Zone::Lecturer;
        return true;
    }
    if (value == "NONE" || value.empty()) {
        output = Zone::None;
        return true;
    }
    return false;
}

bool parseNodeType(const std::string& text, NodeType& output) {
    const std::string value = canonical(text);
    if (value == "GATE") {
        output = NodeType::Gate;
        return true;
    }
    if (value == "JUNCTION") {
        output = NodeType::Junction;
        return true;
    }
    if (value == "SLOT") {
        output = NodeType::Slot;
        return true;
    }
    return false;
}

std::string trim(const std::string& text) {
    const auto first = std::find_if_not(text.begin(), text.end(), [](unsigned char value) {
        return std::isspace(value) != 0;
    });
    const auto last = std::find_if_not(text.rbegin(), text.rend(), [](unsigned char value) {
        return std::isspace(value) != 0;
    }).base();
    if (first >= last) {
        return {};
    }
    return std::string(first, last);
}

std::string toUpperAscii(const std::string& text) {
    std::string output = text;
    std::transform(output.begin(), output.end(), output.begin(), [](unsigned char value) {
        return static_cast<char>(std::toupper(value));
    });
    return output;
}

std::string normalizeCardId(const std::string& cardId) {
    std::string output;
    for (const char rawCharacter : toUpperAscii(trim(cardId))) {
        const auto character = static_cast<unsigned char>(rawCharacter);
        if (!std::isspace(character)) {
            output.push_back(static_cast<char>(character));
        }
    }
    return output;
}

std::string normalizePlate(const std::string& plate) {
    std::string output;
    for (const char rawCharacter : toUpperAscii(plate)) {
        const auto character = static_cast<unsigned char>(rawCharacter);
        if (std::isalnum(character)) {
            output.push_back(static_cast<char>(character));
        }
    }
    return output;
}

bool isValidNormalizedPlate(const std::string& plate) {
    if (plate.size() < 6 || plate.size() > 12) {
        return false;
    }
    return std::all_of(plate.begin(), plate.end(), [](unsigned char character) {
        return std::isalnum(character) != 0;
    });
}

Timestamp parseDateTime(const std::string& text) {
    const auto tryFormat = [&](const char* format) -> std::optional<Timestamp> {
        std::tm parsed{};
        std::istringstream input(text);
        input >> std::get_time(&parsed, format);
        if (input.fail()) {
            return std::nullopt;
        }
        input >> std::ws;
        if (!input.eof()) {
            return std::nullopt;  // Không chấp nhận ký tự thừa sau thời gian.
        }

        const int expectedYear = parsed.tm_year;
        const int expectedMonth = parsed.tm_mon;
        const int expectedDay = parsed.tm_mday;
        const int expectedHour = parsed.tm_hour;
        const int expectedMinute = parsed.tm_min;
        const int expectedSecond = parsed.tm_sec;
        parsed.tm_isdst = -1;
        const std::time_t converted = std::mktime(&parsed);
        if (converted == static_cast<std::time_t>(-1)) {
            return std::nullopt;
        }

        // mktime tự chuẩn hóa 31/02 thành tháng 03. So sánh round-trip để từ chối
        // ngày/giờ không tồn tại thay vì âm thầm tính phí sai.
        std::tm normalized{};
#if defined(_WIN32)
        localtime_s(&normalized, &converted);
#else
        localtime_r(&converted, &normalized);
#endif
        if (normalized.tm_year != expectedYear || normalized.tm_mon != expectedMonth ||
            normalized.tm_mday != expectedDay || normalized.tm_hour != expectedHour ||
            normalized.tm_min != expectedMinute || normalized.tm_sec != expectedSecond) {
            return std::nullopt;
        }
        return static_cast<Timestamp>(converted);
    };

    if (const std::optional<Timestamp> withSeconds =
            tryFormat("%Y-%m-%d %H:%M:%S");
        withSeconds.has_value()) {
        return *withSeconds;
    }
    if (const std::optional<Timestamp> withoutSeconds =
            tryFormat("%Y-%m-%d %H:%M");
        withoutSeconds.has_value()) {
        return *withoutSeconds;
    }
    throw std::invalid_argument("Thoi gian khong ton tai hoac sai dang YYYY-MM-DD HH:MM[:SS]");
}

std::string formatDateTime(Timestamp value) {
    const std::time_t raw = static_cast<std::time_t>(value);
    std::tm local{};
#if defined(_WIN32)
    localtime_s(&local, &raw);
#else
    localtime_r(&raw, &local);
#endif
    std::ostringstream output;
    output << std::put_time(&local, "%Y-%m-%d %H:%M:%S");
    return output.str();
}

}  // namespace parking
