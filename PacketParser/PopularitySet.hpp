#ifndef POPULARITYSET_HPP
#define POPULARITYSET_HPP

#include "PopularityList.hpp"

template <typename T>
class PopularitySet {
public:
    PopularitySet(int maxPoolSize) : popularityList(maxPoolSize) {}

    int getCount() const { return popularityList.getCount(); }

    void add(const T& item) {
        popularityList.add(item, true);
    }

    void clear() {
        popularityList.clear();
    }

    bool contains(const T& item) const {
        return popularityList.containsKey(item);
    }

    QList<T> getEnumerator() const {
        return popularityList.getKeyEnumerator();
    }

    void remove(const T& item) {
        popularityList.remove(item);
    }

private:
    PopularityList<T, bool> popularityList;
};

#endif // POPULARITYSET_HPP
