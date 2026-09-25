#pragma once
#include <vector>
#include <cstdint>
#include <functional>

namespace SharedUtils {

template <typename T>
class LuckyHashSet {
private:
    std::vector<T> items;
    std::vector<bool> initialized;

    uint32_t GetIndex(const T& item) const {
        return static_cast<uint32_t>(std::hash<T>{}(item) % items.size());
    }

public:
    explicit LuckyHashSet(int size) : items(size), initialized(size, false) {}

    LuckyHashSet(int size, const std::vector<T>& init_items) : LuckyHashSet(size) {
        Add(init_items);
    }

    int Size() const { return items.size(); }

    void Add(const T& item) {
        uint32_t idx = GetIndex(item);
        items[idx] = item;
        initialized[idx] = true;
    }

    void Add(const std::vector<T>& itemsList) {
        for (const T& item : itemsList) {
            Add(item);
        }
    }

    bool Contains(const T& item) const {
        // Checking against default not strictly required in C++ if we use `initialized` array
        uint32_t idx = GetIndex(item);
        if (initialized[idx] && items[idx] == item) {
            return true;
        }
        return false;
    }
};

}
