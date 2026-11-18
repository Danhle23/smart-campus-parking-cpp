#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <utility>

namespace parking::ds {

// Stack LIFO bằng DSLK đơn; mọi thao tác chỉ diễn ra ở đầu danh sách nên O(1).
template <typename T>
class LinkedStack {
private:
    struct Node {
        T value;
        std::unique_ptr<Node> next;

        Node(T newValue, std::unique_ptr<Node> nextNode)
            : value(std::move(newValue)), next(std::move(nextNode)) {}
    };

public:
    LinkedStack() = default;
    LinkedStack(const LinkedStack&) = delete;
    LinkedStack& operator=(const LinkedStack&) = delete;
    LinkedStack(LinkedStack&& other) noexcept
        : top_(std::move(other.top_)), size_(std::exchange(other.size_, 0)) {}

    LinkedStack& operator=(LinkedStack&& other) noexcept {
        if (this != &other) {
            top_ = std::move(other.top_);
            size_ = std::exchange(other.size_, 0);
        }
        return *this;
    }
    ~LinkedStack() = default;

    void push(T value) {
        top_ = std::make_unique<Node>(std::move(value), std::move(top_));
        ++size_;
    }

    std::optional<T> pop() {
        if (top_ == nullptr) {
            return std::nullopt;
        }

        std::unique_ptr<Node> removed = std::move(top_);
        top_ = std::move(removed->next);
        --size_;
        return std::move(removed->value);
    }

    T* top() noexcept { return top_ == nullptr ? nullptr : &top_->value; }
    const T* top() const noexcept { return top_ == nullptr ? nullptr : &top_->value; }

    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }

    void clear() noexcept {
        top_.reset();
        size_ = 0;
    }

private:
    std::unique_ptr<Node> top_;
    std::size_t size_{0};
};

}  // namespace parking::ds
