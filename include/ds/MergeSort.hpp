#pragma once

#include <cstddef>
#include <utility>
#include <vector>

namespace parking::ds {

template <typename T, typename Compare>
void mergeRanges(std::vector<T>& values,
                 std::vector<T>& buffer,
                 std::size_t left,
                 std::size_t middle,
                 std::size_t right,
                 Compare compare) {
    std::size_t i = left;
    std::size_t j = middle;
    std::size_t output = left;

    while (i < middle && j < right) {
        // Chọn phần tử bên trái khi hai phần tử tương đương để giữ tính ổn định.
        if (!compare(values[j], values[i])) {
            buffer[output++] = std::move(values[i++]);
        } else {
            buffer[output++] = std::move(values[j++]);
        }
    }
    while (i < middle) {
        buffer[output++] = std::move(values[i++]);
    }
    while (j < right) {
        buffer[output++] = std::move(values[j++]);
    }
    for (std::size_t index = left; index < right; ++index) {
        values[index] = std::move(buffer[index]);
    }
}

template <typename T, typename Compare>
void mergeSortRange(std::vector<T>& values,
                    std::vector<T>& buffer,
                    std::size_t left,
                    std::size_t right,
                    Compare compare) {
    if (right - left <= 1) {
        return;
    }
    const std::size_t middle = left + (right - left) / 2;
    mergeSortRange(values, buffer, left, middle, compare);
    mergeSortRange(values, buffer, middle, right, compare);
    mergeRanges(values, buffer, left, middle, right, compare);
}

template <typename T, typename Compare>
void mergeSort(std::vector<T>& values, Compare compare) {
    if (values.size() < 2) {
        return;
    }
    std::vector<T> buffer(values.size());
    mergeSortRange(values, buffer, 0, values.size(), compare);
}

}  // namespace parking::ds
