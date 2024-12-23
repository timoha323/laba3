#ifndef HASHTABLE_H
#define HASHTABLE_H

#include "IDictionary.h"
#include "DynamicArraySmart.h"
#include "LinkedListSmart.h"
#include "ShrdPtr.h"
#include "UnqPtr.h"
#include "IndexPair.h"
#include <functional>
#include <stdexcept>
#include <type_traits>

template<typename TKey, typename TElement>
class HashTable : public IDictionary<TKey, TElement> {
public:
    HashTable(size_t initialCapacity = 16);

    virtual ~HashTable();

    virtual size_t GetCount() const override;

    virtual size_t GetCapacity() const override;

    virtual TElement Get(const TKey &key) const override;

    virtual bool ContainsKey(const TKey &key) const override;

    virtual void Add(const TKey &key, const TElement &element) override;

    virtual void Remove(const TKey &key) override;

    virtual void Update(const TKey &key, const TElement &element) override;

    virtual UnqPtr<IDictionaryIterator<TKey, TElement>> GetIterator() const override;

private:
    struct KeyValuePair {
        TKey key;
        TElement value;

        KeyValuePair(const TKey &k, const TElement &v) : key(k), value(v) {}
    };

    UnqPtr<DynamicArraySmart<LinkedListSmart<KeyValuePair>>> table;
    size_t count;
    size_t capacity;

    size_t HashFunction(const TKey &key) const;

    void Rehash();

    class HashTableIterator : public IDictionaryIterator<TKey, TElement> {
    public:
        HashTableIterator(const HashTable *hashTable);

        virtual ~HashTableIterator() {}

        virtual bool MoveNext() override;

        virtual void Reset() override;

        virtual TKey GetCurrentKey() const override;

        virtual TElement GetCurrentValue() const override;

    private:
        const HashTable *hashTable;
        size_t bucketIndex;
        int listIndex;
    };
};

template<typename TKey, typename TElement>
HashTable<TKey, TElement>::HashTable(size_t initialCapacity)
        : table(new DynamicArraySmart<LinkedListSmart<KeyValuePair>>(static_cast<int>(initialCapacity))), count(0),
          capacity(initialCapacity) {
    for (size_t i = 0; i < capacity; ++i) {
        table->Append(LinkedListSmart<KeyValuePair>());
    }
}

template<typename TKey, typename TElement>
HashTable<TKey, TElement>::~HashTable() {

}

template<typename TKey, typename TElement>
size_t HashTable<TKey, TElement>::GetCount() const {
    return count;
}

template<typename TKey, typename TElement>
size_t HashTable<TKey, TElement>::GetCapacity() const {
    return capacity;
}

template<typename TKey, typename TElement>
size_t HashTable<TKey, TElement>::HashFunction(const TKey &key) const {
    if constexpr (std::is_same<TKey, IndexPair>::value) {
        return IndexPairHash()(key);
    } else {
        return std::hash<TKey>()(key);
    }
}

template<typename TKey, typename TElement>
void HashTable<TKey, TElement>::Add(const TKey &key, const TElement &element) {
    size_t index = HashFunction(key) % capacity;
    LinkedListSmart<KeyValuePair> &chain = table->Get(static_cast<int>(index));

    auto iterator = chain.begin();
    for (; iterator != chain.end(); ++iterator) {
        if ((*iterator).key == key) {
            (*iterator).value = element;
            return;
        }
    }

    chain.Append(KeyValuePair(key, element));
    ++count;

    if (static_cast<double>(count) / capacity > 0.75) {
        Rehash();
    }
}

template<typename TKey, typename TElement>
void HashTable<TKey, TElement>::Remove(const TKey &key) {
    size_t index = HashFunction(key) % capacity;
    LinkedListSmart<KeyValuePair> &chain = table->Get(static_cast<int>(index));

    auto iterator = chain.begin();
    int i = 0;
    for (; iterator != chain.end(); ++iterator) {
        ++i;
        if ((*iterator).key == key) {
            chain.RemoveAt(i);
            --count;
            return;
        }
    }

    throw std::runtime_error("Key not found.");
}

template<typename TKey, typename TElement>
void HashTable<TKey, TElement>::Update(const TKey &key, const TElement &element) {
    size_t index = HashFunction(key) % capacity;
    LinkedListSmart<KeyValuePair> &chain = table->Get(static_cast<int>(index));

    for (int i = 0; i < chain.GetLength(); ++i) {
        if (chain.Get(i).key == key) {
            chain.Get(i).value = element;
            return;
        }
    }

    throw std::runtime_error("Key not found.");
}

template<typename TKey, typename TElement>
bool HashTable<TKey, TElement>::ContainsKey(const TKey &key) const {
    size_t index = HashFunction(key) % capacity;
    const LinkedListSmart<KeyValuePair> &chain = table->Get(static_cast<int>(index));

    for (int i = 0; i < chain.GetLength(); ++i) {
        if (chain.Get(i).key == key) {
            return true;
        }
    }

    return false;
}

template<typename TKey, typename TElement>
TElement HashTable<TKey, TElement>::Get(const TKey &key) const {
    size_t index = HashFunction(key) % capacity;
    const LinkedListSmart<KeyValuePair> &chain = table->Get(static_cast<int>(index));

    for (int i = 0; i < chain.GetLength(); ++i) {
        if (chain.Get(i).key == key) {
            return chain.Get(i).value;
        }
    }

    throw std::runtime_error("Key not found.");
}

template<typename TKey, typename TElement>
void HashTable<TKey, TElement>::Rehash() {
    size_t newCapacity = capacity * 2;
    UnqPtr<DynamicArraySmart<LinkedListSmart<KeyValuePair>>> newTable(
            new DynamicArraySmart<LinkedListSmart<KeyValuePair>>(static_cast<int>(newCapacity)));

    for (size_t i = 0; i < newCapacity; ++i) {
        newTable->Append(LinkedListSmart<KeyValuePair>());
    }

    for (size_t i = 0; i < capacity; ++i) {
        LinkedListSmart<KeyValuePair> &chain = table->Get(static_cast<int>(i));
        for (int j = 0; j < chain.GetLength(); ++j) {
            const KeyValuePair &kvp = chain.Get(j);
            size_t index = HashFunction(kvp.key) % newCapacity;
            newTable->Get(static_cast<int>(index)).Append(kvp);
        }
    }

    table = std::move(newTable);
    capacity = newCapacity;
}

template<typename TKey, typename TElement>
HashTable<TKey, TElement>::HashTableIterator::HashTableIterator(const HashTable *hashTable)
        : hashTable(hashTable), bucketIndex(0), listIndex(-1) {
}

template<typename TKey, typename TElement>
bool HashTable<TKey, TElement>::HashTableIterator::MoveNext() {
    ++listIndex;

    while (bucketIndex < hashTable->capacity) {
        LinkedListSmart<KeyValuePair> &chain = hashTable->table->Get(static_cast<int>(bucketIndex));
        if (listIndex < chain.GetLength()) {
            return true;
        } else {
            ++bucketIndex;
            listIndex = 0;
        }
    }

    return false;
}

template<typename TKey, typename TElement>
void HashTable<TKey, TElement>::HashTableIterator::Reset() {
    bucketIndex = 0;
    listIndex = -1;
}

template<typename TKey, typename TElement>
TKey HashTable<TKey, TElement>::HashTableIterator::GetCurrentKey() const {
    if (bucketIndex >= hashTable->capacity)
        throw std::out_of_range("Iterator out of range");

    const LinkedListSmart<KeyValuePair> &chain = hashTable->table->Get(static_cast<int>(bucketIndex));
    return chain.Get(listIndex).key;
}

template<typename TKey, typename TElement>
TElement HashTable<TKey, TElement>::HashTableIterator::GetCurrentValue() const {
    if (bucketIndex >= hashTable->capacity)
        throw std::out_of_range("Iterator out of range");

    const LinkedListSmart<KeyValuePair> &chain = hashTable->table->Get(static_cast<int>(bucketIndex));
    return chain.Get(listIndex).value;
}

template<typename TKey, typename TElement>
UnqPtr<IDictionaryIterator<TKey, TElement>> HashTable<TKey, TElement>::GetIterator() const {
    return UnqPtr<IDictionaryIterator<TKey, TElement>>(new HashTableIterator(this));
}

#endif // HASHTABLE_H
