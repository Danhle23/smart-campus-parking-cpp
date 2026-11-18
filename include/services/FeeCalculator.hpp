#pragma once

#include "domain/Models.hpp"

#include <cstdint>

namespace parking {

class FeeCalculator {
public:
    explicit FeeCalculator(FeePolicy policy = FeePolicy{}) : policy_(policy) {}

    [[nodiscard]] Money calculate(VehicleType vehicleType,
                                  Timestamp entryTime,
                                  Timestamp exitTime) const;
    [[nodiscard]] std::int64_t parkedMinutes(Timestamp entryTime,
                                             Timestamp exitTime) const;
    [[nodiscard]] const FeePolicy& policy() const noexcept { return policy_; }
    void setPolicy(FeePolicy policy) { policy_ = policy; }

private:
    FeePolicy policy_;
};

}  // namespace parking
