#ifndef DYNAMICARRAYSMART_H
#define DYNAMICARRAYSMART_H

#include "Sequence.h"
#include "UnqPtr.h"
#include <stdexcept>
#include <utility>

#define DYNAMICARRAY_EMPTY "DynamicArraySmart is empty"
#define DYNAMICARRAY_OUT_OF_RANGE "Index out of range"
#define DYNAMICARRAY_SUBSEQ_ERR "Invalid subsequence indices"

template <typename T>
class DynamicArraySmart : public Sequence<T> {
private:
    UnqPtr<T[]> data;
    int capacity;
    int length;

    void resize(int newCapacity) {
        UnqPtr<T[]> newData(new T[newCapacity]);
        for (int i = 0; i < length; ++i) {
            newData[i] = std::move(data[i]);
        }
        data = std::move(newData);
        capacity = newCapacity;
    }

public:
    DynamicArraySmart() : data(nullptr), capacity(0), length(0) {}

    explicit DynamicArraySmart(int initialCapacity)
            : data(new T[initialCapacity]), capacity(initialCapacity), length(0) {}

    ~DynamicArraySmart() override = default;

    DynamicArraySmart(const DynamicArraySmart&) = delete;
    DynamicArraySmart& operator=(const DynamicArraySmart&) = delete;

    DynamicArraySmart(DynamicArraySmart&& other) noexcept
            : data(std::move(other.data)),
              capacity(other.capacity),
              length(other.length) {
        other.capacity = 0;
        other.length = 0;
    }

    DynamicArraySmart& operator=(DynamicArraySmart&& other) noexcept {
        if (this != &other) {
            data = std::move(other.data);
            capacity = other.capacity;
            length = other.length;

            other.capacity = 0;
            other.length = 0;
        }
        return *this;
    }

    T& GetFirst() const override {
        if (length == 0)
            throw std::out_of_range(DYNAMICARRAY_EMPTY);
        return data[0];
    }

    T& GetLast() const override {
        if (length == 0)
            throw std::out_of_range(DYNAMICARRAY_EMPTY);
        return data[length - 1];
    }

    T& Get(int index) const override {
        if (index < 0 || index >= length)
            throw std::out_of_range(DYNAMICARRAY_OUT_OF_RANGE);
        return data[index];
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex >= length || startIndex > endIndex)
            throw std::out_of_range(DYNAMICARRAY_SUBSEQ_ERR);

        auto* subseq = new DynamicArraySmart<T>(endIndex - startIndex + 1);
        for (int i = startIndex; i <= endIndex; ++i) {
            subseq->Append(data[i]);
        }
        return subseq;
    }

    int GetLength() const override {
        return length;
    }

    void Append(const T& item) override {
        if (length == capacity) {
            int newCapacity = capacity == 0 ? 1 : capacity * 2;
            resize(newCapacity);
        }
        data[length++] = item;
    }

    void Prepend(const T& item) override {
        if (length == capacity) {
            int newCapacity = capacity == 0 ? 1 : capacity * 2;
            resize(newCapacity);
        }
        for (int i = length; i > 0; --i) {
            data[i] = std::move(data[i - 1]);
        }
        data[0] = item;
        ++length;
    }

    void InsertAt(const T& item, int index) override {
        if (index < 0 || index > length)
            throw std::out_of_range(DYNAMICARRAY_OUT_OF_RANGE);
        if (length == capacity) {
            int newCapacity = capacity == 0 ? 1 : capacity * 2;
            resize(newCapacity);
        }
        for (int i = length; i > index; --i) {
            data[i] = std::move(data[i - 1]);
        }
        data[index] = item;
        ++length;
    }

    void RemoveAt(int index) override {
        if (index < 0 || index >= length)
            throw std::out_of_range(DYNAMICARRAY_OUT_OF_RANGE);
        for (int i = index; i < length - 1; ++i) {
            data[i] = std::move(data[i + 1]);
        }
        --length;
    }

    Sequence<T>* Concat(Sequence<T>* list) const override {
        auto* newArray = new DynamicArraySmart<T>(length + list->GetLength());
        for (int i = 0; i < length; ++i) {
            newArray->Append(data[i]);
        }
        for (int i = 0; i < list->GetLength(); ++i) {
            newArray->Append(list->Get(i));
        }
        return newArray;
    }

    T& operator[](int index) {
        if (index < 0 || index >= length)
            throw std::out_of_range(DYNAMICARRAY_OUT_OF_RANGE);
        return data[index];
    }

    const T& operator[](int index) const {
        if (index < 0 || index >= length)
            throw std::out_of_range(DYNAMICARRAY_OUT_OF_RANGE);
        return data[index];
    }
};

#endif // DYNAMICARRAYSMART_H
