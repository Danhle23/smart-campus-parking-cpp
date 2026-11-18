#pragma once

#include "domain/Models.hpp"

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace parking {

class FuzzySearch {
public:
    static int levenshteinDistance(std::string_view left, std::string_view right);

    static std::vector<PlateMatch> topMatches(const std::string& rawQuery,
                                              const std::vector<ParkingSession>& sessions,
                                              std::size_t limit = 3);
};

}  // namespace parking
