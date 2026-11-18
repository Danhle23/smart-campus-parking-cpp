#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

namespace parking::ds {

// Binary heap tổng quát. Compare(a,b)==true nghĩa là a có ưu tiên cao hơn b.
// Với std::less<T>, phần tử nhỏ nhất nằm ở root (min-heap).
template <typename T, typename Compare = std::less<T>>
class BinaryMinHeap {
public:
    explicit BinaryMinHeap(Compare compare = Compare{}) : compare_(std::move(compare)) {}

    void push(T value) {
        data_.push_back(std::move(value));
        siftUp(data_.size() - 1);
    }

    std::optional<T> pop() {
        if (data_.empty()) {
            return std::nullopt;
        }

        T root = std::move(data_.front());
        if (data_.size() == 1) {
            data_.pop_back();
            return root;
        }

        data_.front() = std::move(data_.back());
        data_.pop_back();
        siftDown(0);
        return root;
    }

    T* top() noexcept { return data_.empty() ? nullptr : &data_.front(); }
    const T* top() const noexcept { return data_.empty() ? nullptr : &data_.front(); }

    [[nodiscard]] bool empty() const noexcept { return data_.empty(); }
    [[nodiscard]] std::size_t size() const noexcept { return data_.size(); }
    void clear() noexcept { data_.clear(); }

private:
    void siftUp(std::size_t index) {
        while (index > 0) {
            const std::size_t parent = (index - 1) / 2;
            if (!compare_(data_[index], data_[parent])) {
                break;
            }
            std::swap(data_[index], data_[parent]);
            index = parent;
        }
    }

    void siftDown(std::size_t index) {
        while (true) {
            const std::size_t left = index * 2 + 1;
            const std::size_t right = left + 1;
            std::size_t best = index;

            if (left < data_.size() && compare_(data_[left], data_[best])) {
                best = left;
            }
            if (right < data_.size() && compare_(data_[right], data_[best])) {
                best = right;
            }
            if (best == index) {
                break;
            }

            std::swap(data_[index], data_[best]);
            index = best;
        }
    }

    std::vector<T> data_;
    Compare compare_;
};

}  // namespace parking::ds
