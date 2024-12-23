#ifndef ISET_H
#define ISET_H

#include "HashTable.h"

template<typename T>
class ISet {
private:
    HashTable<T, bool> table;

public:
    ISet(size_t bucketCount = 16) : table(bucketCount) {}

    void Add(const T& element) {
        table.Insert(element, true);
    }

    void Remove(const T& element) {
        table.Remove(element);
    }

    bool Contains(const T& element) const {
        return table.ContainsKey(element);
    }
};

#endif // ISET_H
