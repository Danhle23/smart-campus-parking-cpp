#include "services/FeeCalculator.hpp"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace parking {

std::int64_t FeeCalculator::parkedMinutes(Timestamp entryTime, Timestamp exitTime) const {
    if (exitTime < entryTime) {
        throw std::invalid_argument("Thoi gian ra khong duoc som hon thoi gian vao");
    }
    if (entryTime < 0 &&
        exitTime > std::numeric_limits<Timestamp>::max() + entryTime) {
        throw std::overflow_error("Khoang thoi gian gui xe vuot mien 64-bit");
    }
    const Timestamp durationSeconds = exitTime - entryTime;
    return durationSeconds / 60 + (durationSeconds % 60 == 0 ? 0 : 1);
}

Money FeeCalculator::calculate(VehicleType vehicleType,
                               Timestamp entryTime,
                               Timestamp exitTime) const {
    const std::int64_t minutes = parkedMinutes(entryTime, exitTime);
    const std::int64_t roundedHours = minutes / 60 + (minutes % 60 == 0 ? 0 : 1);
    const std::int64_t startedHours = std::max<std::int64_t>(1, roundedHours);
    const Money hourlyFee = vehicleType == VehicleType::Car
                                ? policy_.carPerStartedHour
                                : policy_.motorcyclePerStartedHour;
    if (hourlyFee <= 0) {
        throw std::invalid_argument("Muc phi phai la so duong");
    }
    if (startedHours > std::numeric_limits<Money>::max() / hourlyFee) {
        throw std::overflow_error("Tong phi vuot mien 64-bit");
    }
    return hourlyFee * startedHours;
}

}  // namespace parking
