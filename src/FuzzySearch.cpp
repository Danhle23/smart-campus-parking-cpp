#include "services/FuzzySearch.hpp"

#include "domain/Types.hpp"
#include "ds/MergeSort.hpp"

#include <algorithm>
#include <stdexcept>

namespace parking {

int FuzzySearch::levenshteinDistance(std::string_view left, std::string_view right) {
    // Chỉ giữ hai hàng của bảng quy hoạch động: bộ nhớ O(min(m,n)).
    if (left.size() > right.size()) {
        std::swap(left, right);
    }

    std::vector<int> previous(left.size() + 1);
    std::vector<int> current(left.size() + 1);
    for (std::size_t column = 0; column <= left.size(); ++column) {
        previous[column] = static_cast<int>(column);
    }

    for (std::size_t row = 1; row <= right.size(); ++row) {
        current[0] = static_cast<int>(row);
        for (std::size_t column = 1; column <= left.size(); ++column) {
            const int insertion = current[column - 1] + 1;
            const int deletion = previous[column] + 1;
            const int substitution = previous[column - 1] +
                                     (left[column - 1] == right[row - 1] ? 0 : 1);
            current[column] = std::min({insertion, deletion, substitution});
        }
        previous.swap(current);
    }
    return previous[left.size()];
}

std::vector<PlateMatch> FuzzySearch::topMatches(
    const std::string& rawQuery,
    const std::vector<ParkingSession>& sessions,
    std::size_t limit) {
    const std::string query = normalizePlate(rawQuery);
    if (query.empty()) {
        throw std::invalid_argument("Tu khoa bien so khong duoc rong");
    }

    std::vector<PlateMatch> matches;
    matches.reserve(sessions.size());
    for (const ParkingSession& session : sessions) {
        const std::string plate = normalizePlate(session.plate);
        const int distance = levenshteinDistance(query, plate);
        const std::size_t denominator = std::max(query.size(), plate.size());
        const double similarity = denominator == 0
                                      ? 1.0
                                      : 1.0 - static_cast<double>(distance) /
                                                  static_cast<double>(denominator);
        matches.push_back(PlateMatch{plate,
                                     session.cardId,
                                     session.status == SessionStatus::Holding
                                         ? "KHU_TAM_DI_DOI"
                                         : session.slotId,
                                     distance,
                                     similarity});
    }

    ds::mergeSort(matches, [](const PlateMatch& left, const PlateMatch& right) {
        if (left.editDistance != right.editDistance) {
            return left.editDistance < right.editDistance;
        }
        if (left.plate != right.plate) {
            return left.plate < right.plate;
        }
        return left.cardId < right.cardId;
    });

    if (matches.size() > limit) {
        matches.resize(limit);
    }
    return matches;
}

}  // namespace parking
