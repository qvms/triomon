#pragma once
#include <map>
#include <list>
#include <mutex>
#include <vector>

namespace PacketParser {

template <typename Key, typename Value>
class PopularityList {
private:
    std::map<Key, Value> m_map;
    std::mutex m_mutex;

public:
    PopularityList(size_t capacity = 1024) {}

    bool TryGetValue(const Key& key, Value& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto it = m_map.find(key);
        if (it != m_map.end()) {
            value = it->second;
            return true;
        }
        return false;
    }

    void Add(const Key& key, const Value& value) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_map[key] = value;
    }

    void Remove(const Key& key) {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_map.erase(key);
    }
};

} // namespace PacketParser
