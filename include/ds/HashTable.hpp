#pragma once

#include <cstddef>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace parking::ds {

// Hash table tự cài bằng separate chaining. Mỗi bucket là một DSLK đơn;
// unique_ptr giữ quyền sở hữu node để bảng không rò rỉ bộ nhớ khi có exception.
template <typename Key, typename Value, typename Hasher = std::hash<Key>>
class HashTable {
private:
    struct Node {
        Key key;
        Value value;
        std::unique_ptr<Node> next;

        Node(const Key& newKey, Value newValue)
            : key(newKey), value(std::move(newValue)), next(nullptr) {}
    };

public:
    explicit HashTable(std::size_t bucketCount = 17, Hasher hasher = Hasher{})
        : buckets_(bucketCount == 0 ? 17 : bucketCount), hasher_(std::move(hasher)) {}

    HashTable(const HashTable&) = delete;
    HashTable& operator=(const HashTable&) = delete;
    HashTable(HashTable&& other) noexcept(std::is_nothrow_move_constructible<Hasher>::value)
        : buckets_(std::move(other.buckets_)),
          size_(std::exchange(other.size_, 0)),
          hasher_(std::move(other.hasher_)),
          maxLoadFactor_(other.maxLoadFactor_) {}

    HashTable& operator=(HashTable&& other) noexcept(
        std::is_nothrow_move_assignable<Hasher>::value) {
        if (this != &other) {
            buckets_ = std::move(other.buckets_);
            size_ = std::exchange(other.size_, 0);
            hasher_ = std::move(other.hasher_);
            maxLoadFactor_ = other.maxLoadFactor_;
        }
        return *this;
    }
    ~HashTable() = default;

    bool insert(const Key& key, Value value) {
        if (contains(key)) {
            return false;
        }

        // std::vector hợp lệ nhưng có thể rỗng sau move. Khởi tạo lười giúp
        // đối tượng nguồn sau move vẫn dùng lại được thay vì hash modulo 0.
        if (buckets_.empty()) {
            buckets_.resize(17);
        }

        if (static_cast<double>(size_ + 1) / static_cast<double>(buckets_.size()) >
            maxLoadFactor_) {
            rehash(buckets_.size() * 2 + 1);
        }

        const std::size_t index = bucketIndex(key, buckets_.size());
        auto newNode = std::make_unique<Node>(key, std::move(value));
        newNode->next = std::move(buckets_[index]);
        buckets_[index] = std::move(newNode);
        ++size_;
        return true;
    }

    bool insertOrAssign(const Key& key, Value value) {
        if (Value* current = find(key); current != nullptr) {
            *current = std::move(value);
            return false;
        }
        insert(key, std::move(value));
        return true;
    }

    Value* find(const Key& key) {
        if (buckets_.empty()) {
            return nullptr;
        }
        Node* current = buckets_[bucketIndex(key, buckets_.size())].get();
        while (current != nullptr) {
            if (current->key == key) {
                return &current->value;
            }
            current = current->next.get();
        }
        return nullptr;
    }

    const Value* find(const Key& key) const {
        if (buckets_.empty()) {
            return nullptr;
        }
        const Node* current = buckets_[bucketIndex(key, buckets_.size())].get();
        while (current != nullptr) {
            if (current->key == key) {
                return &current->value;
            }
            current = current->next.get();
        }
        return nullptr;
    }

    bool contains(const Key& key) const { return find(key) != nullptr; }

    bool erase(const Key& key) {
        if (buckets_.empty()) {
            return false;
        }
        const std::size_t index = bucketIndex(key, buckets_.size());
        std::unique_ptr<Node>* link = &buckets_[index];

        while (link->get() != nullptr) {
            if ((*link)->key == key) {
                // Nối node trước trực tiếp với node sau rồi để unique_ptr hủy node xóa.
                *link = std::move((*link)->next);
                --size_;
                return true;
            }
            link = &((*link)->next);
        }
        return false;
    }

    void clear() noexcept {
        for (auto& bucket : buckets_) {
            bucket.reset();
        }
        size_ = 0;
    }

    template <typename Function>
    void forEach(Function function) {
        for (auto& bucket : buckets_) {
            Node* current = bucket.get();
            while (current != nullptr) {
                function(current->key, current->value);
                current = current->next.get();
            }
        }
    }

    template <typename Function>
    void forEach(Function function) const {
        for (const auto& bucket : buckets_) {
            const Node* current = bucket.get();
            while (current != nullptr) {
                function(current->key, current->value);
                current = current->next.get();
            }
        }
    }

    [[nodiscard]] std::size_t size() const noexcept { return size_; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
    [[nodiscard]] std::size_t bucketCount() const noexcept { return buckets_.size(); }
    [[nodiscard]] double loadFactor() const noexcept {
        if (buckets_.empty()) {
            return 0.0;
        }
        return static_cast<double>(size_) / static_cast<double>(buckets_.size());
    }

private:
    [[nodiscard]] std::size_t bucketIndex(const Key& key, std::size_t count) const {
        return hasher_(key) % count;
    }

    void rehash(std::size_t newBucketCount) {
        if (newBucketCount == 0) {
            throw std::invalid_argument("So bucket cua HashTable phai lon hon 0");
        }

        std::vector<std::unique_ptr<Node>> newBuckets(newBucketCount);
        for (auto& oldBucket : buckets_) {
            std::unique_ptr<Node> current = std::move(oldBucket);
            while (current != nullptr) {
                std::unique_ptr<Node> next = std::move(current->next);
                const std::size_t index = bucketIndex(current->key, newBucketCount);
                current->next = std::move(newBuckets[index]);
                newBuckets[index] = std::move(current);
                current = std::move(next);
            }
        }
        buckets_ = std::move(newBuckets);
    }

    std::vector<std::unique_ptr<Node>> buckets_;
    std::size_t size_{0};
    Hasher hasher_;
    double maxLoadFactor_{0.75};
};

// FNV-1a 64-bit: đơn giản, ổn định giữa các lần chạy và dễ trình bày khi bảo vệ.
struct StringHasher {
    std::size_t operator()(const std::string& text) const noexcept {
        std::size_t hash = static_cast<std::size_t>(14695981039346656037ULL);
        for (const char rawCharacter : text) {
            const auto character = static_cast<unsigned char>(rawCharacter);
            hash ^= static_cast<std::size_t>(character);
            hash *= static_cast<std::size_t>(1099511628211ULL);
        }
        return hash;
    }
};

}  // namespace parking::ds
