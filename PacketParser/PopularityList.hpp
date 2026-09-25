#pragma once
#include <unordered_map>
#include <list>
#include <functional>
#include <algorithm>

namespace PacketParser {

template<typename TKey, typename TValue>
class PopularityList {
public:
    using ListCanExpand = std::function<bool(PopularityList<TKey, TValue>&)>;
    using PopularityLostEventHandler = std::function<void(const TKey&, const TValue&)>;
    using LastValueExpiredCheck = std::function<bool(const TValue&, const TValue&)>;

    PopularityLostEventHandler PopularityLost;

private:
    using KVPair = std::pair<TKey, TValue>;
    std::list<KVPair> m_linkedList;
    std::unordered_map<TKey, typename std::list<KVPair>::iterator> m_sortedList; // Note: historically sorted, now just a hash map in C#

    int m_minPoolSize;
    int m_maxPoolSize;
    int m_currentPoolSize;
    ListCanExpand m_listCanExpandDelegate;
    LastValueExpiredCheck m_lastValueExpiredCheck;

    void LastNodePopularityLost() {
        if (m_linkedList.empty()) return;
        auto lastNodeIt = std::prev(m_linkedList.end());
        TKey key = lastNodeIt->first;
        TValue value = lastNodeIt->second;

        m_sortedList.erase(key);
        m_linkedList.erase(lastNodeIt);

        if (PopularityLost) {
            PopularityLost(key, value);
        }
    }

public:
    PopularityList(int maxPoolSize) 
        : PopularityList(maxPoolSize, maxPoolSize, nullptr, nullptr) {}

    PopularityList(int minPoolSize, int maxPoolSize, ListCanExpand listCanExpandDelegate) 
        : PopularityList(minPoolSize, maxPoolSize, listCanExpandDelegate, nullptr) {}

    PopularityList(int minPoolSize, int maxPoolSize, ListCanExpand listCanExpandDelegate, LastValueExpiredCheck lastValueHasExpired)
        : m_minPoolSize(minPoolSize), m_maxPoolSize(maxPoolSize), m_currentPoolSize(minPoolSize),
          m_listCanExpandDelegate(listCanExpandDelegate), m_lastValueExpiredCheck(lastValueHasExpired) {}

    int Count() const { return m_sortedList.size(); }

    bool ContainsKey(const TKey& key) const {
        return m_sortedList.find(key) != m_sortedList.end();
    }

    void Remove(const TKey& key, bool triggerPopularityLost = false) {
        auto it = m_sortedList.find(key);
        if (it != m_sortedList.end()) {
            auto llNode = it->second;
            TValue val = llNode->second;
            m_linkedList.erase(llNode);
            m_sortedList.erase(it);

            if (triggerPopularityLost && PopularityLost) {
                PopularityLost(key, val);
            }
        }
    }

    void Add(const TKey& key, const TValue& value) {
        auto it = m_sortedList.find(key);
        if (it != m_sortedList.end()) {
            Remove(key);
        }

        m_linkedList.push_front({key, value});
        m_sortedList[key] = m_linkedList.begin();

        if (m_lastValueExpiredCheck) {
            while (!m_linkedList.empty() && m_lastValueExpiredCheck(m_linkedList.back().second, value)) {
                LastNodePopularityLost();
            }
        }

        while (static_cast<int>(m_sortedList.size()) > m_currentPoolSize) {
            if (m_currentPoolSize < m_maxPoolSize && m_listCanExpandDelegate && m_listCanExpandDelegate(*this)) {
                m_currentPoolSize = std::min(static_cast<int>(m_sortedList.size()), m_maxPoolSize);
            } else {
                LastNodePopularityLost();
            }
        }
    }

    bool TryGetValue(const TKey& key, TValue& value) {
        auto it = m_sortedList.find(key);
        if (it != m_sortedList.end()) {
            auto llNode = it->second;
            value = llNode->second;
            m_linkedList.splice(m_linkedList.begin(), m_linkedList, llNode);
            return true;
        }
        return false;
    }

    void Clear() {
        m_linkedList.clear();
        m_sortedList.clear();
    }

    // Iterator support
    auto begin() { return m_linkedList.begin(); }
    auto end() { return m_linkedList.end(); }
    auto begin() const { return m_linkedList.begin(); }
    auto end() const { return m_linkedList.end(); }
};

} // namespace PacketParser
