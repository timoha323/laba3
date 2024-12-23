// IDictionary.h
#ifndef IDICTIONARY_H
#define IDICTIONARY_H

#include <cstddef>
#include "IDictionaryIterator.h"
#include "UnqPtr.h"

template <typename TKey, typename TElement>
class IDictionary
{
public:
    virtual ~IDictionary() {}

    virtual size_t GetCount() const = 0;
    virtual size_t GetCapacity() const = 0;

    virtual TElement Get(const TKey& key) const = 0;
    virtual bool ContainsKey(const TKey& key) const = 0;
    virtual void Add(const TKey& key, const TElement& element) = 0;
    virtual void Remove(const TKey& key) = 0;
    virtual void Update(const TKey& key, const TElement& element) = 0;

    virtual UnqPtr<IDictionaryIterator<TKey, TElement>> GetIterator() const = 0;
};

#endif // IDICTIONARY_H
