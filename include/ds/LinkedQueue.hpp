#pragma once

#include <cstddef>
#include <memory>
#include <optional>
#include <utility>

namespace parking::ds {

// Queue FIFO dựa trên DSLK đơn. Giữ cả head và tail giúp enqueue/dequeue O(1).
template <typename T>
class LinkedQueue {
private:
    struct Node {
        T value;
        std::unique_ptr<Node> next;

        explicit Node(T newValue) : value(std::move(newValue)), next(nullptr) {}
    };

public:
    LinkedQueue() = default;
    LinkedQueue(const LinkedQueue&) = delete;
    LinkedQueue& operator=(const LinkedQueue&) = delete;

    LinkedQueue(LinkedQueue&& other) noexcept
        : head_(std::move(other.head_)), tail_(other.tail_), size_(other.size_) {
        other.tail_ = nullptr;
        other.size_ = 0;
    }

    LinkedQueue& operator=(LinkedQueue&& other) noexcept {
        if (this != &other) {
            head_ = std::move(other.head_);
            tail_ = other.tail_;
            size_ = other.size_;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }

    ~LinkedQueue() = default;

    void enqueue(T value) {
        auto newNode = std::make_unique<Node>(std::move(value));
        Node* newTail = newNode.get();

        if (tail_ == nullptr) {
            head_ = std::move(newNode);
        } else {
            // Luôn nối từ rear để duy trì bất biến FIFO và enqueue O(1).
            tail_->next = std::move(newNode);
        }
        tail_ = newTail;
        ++size_;
    }

    std::optional<T> dequeue() {
        if (head_ == nullptr) {
            return std::nullopt;
        }

        std::unique_ptr<Node> removed = std::move(head_);
        head_ = std::move(removed->next);
        if (head_ == nullptr) {
            tail_ = nullptr;
        }
        --size_;
        return std::move(removed->value);
    }

    T* front() noexcept { return head_ == nullptr ? nullptr : &head_->value; }
    const T* front() const noexcept { return head_ == nullptr ? nullptr : &head_->value; }

    template <typename Function>
    void forEach(Function function) const {
        const Node* current = head_.get();
        while (current != nullptr) {
            function(current->value);
            current = current->next.get();
        }
    }

    template <typename Predicate>
    bool any(Predicate predicate) const {
        bool found = false;
        forEach([&](const T& value) {
            if (!found && predicate(value)) {
                found = true;
            }
        });
        return found;
    }

    // Xóa phần tử đầu tiên thỏa điều kiện mà không cấp phát lại node; dùng khi
    // một xe ở khu tạm check-out trước lúc được xếp lại vào ô Sinh viên.
    template <typename Predicate>
    bool removeFirst(Predicate predicate) {
        std::unique_ptr<Node>* link = &head_;
        Node* previous = nullptr;
        while (link->get() != nullptr) {
            Node* current = link->get();
            if (predicate(current->value)) {
                const bool removedTail = current == tail_;
                *link = std::move(current->next);
                if (removedTail) {
                    tail_ = previous;
                }
                --size_;
                return true;
            }
            previous = current;
            link = &(current->next);
        }
        return false;
    }

    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t size() const noexcept { return size_; }

    void clear() noexcept {
        head_.reset();
        tail_ = nullptr;
        size_ = 0;
    }

private:
    std::unique_ptr<Node> head_;
    Node* tail_{nullptr};
    std::size_t size_{0};
};

}  // namespace parking::ds
