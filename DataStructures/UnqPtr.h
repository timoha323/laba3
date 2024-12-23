#ifndef UNQPTR_H
#define UNQPTR_H
#include <cstddef>

template <typename T>
class UnqPtr {
private:
    T* ptr;

public:
    explicit UnqPtr(T* p = nullptr) : ptr(p) {}

    UnqPtr(UnqPtr<T>&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    UnqPtr<T>& operator=(UnqPtr<T>&& other) noexcept {
        if (this != &other) {
            delete ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    UnqPtr(const UnqPtr<T>&) = delete;
    UnqPtr<T>& operator=(const UnqPtr<T>&) = delete;

    ~UnqPtr() {
        delete ptr;
    }

    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }

    explicit operator bool() const {
        return ptr != nullptr;
    }

    bool operator!() const {
        return ptr == nullptr;
    }

    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    void reset(T* p = nullptr) {
        if (ptr != p) {
            delete ptr;
            ptr = p;
        }
    }

    T* get() const {
        return ptr;
    }
};

template <typename T>
class UnqPtr<T[]> {
private:
    T* ptr;

public:
    explicit UnqPtr(T* p = nullptr) : ptr(p) {}

    UnqPtr(UnqPtr<T[]>&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;
    }

    UnqPtr<T[]>& operator=(UnqPtr<T[]>&& other) noexcept {
        if (this != &other) {
            delete[] ptr;
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        return *this;
    }

    UnqPtr(const UnqPtr<T[]>&) = delete;
    UnqPtr<T[]>& operator=(const UnqPtr<T[]>&) = delete;

    ~UnqPtr() {
        delete[] ptr;
    }

    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }

    explicit operator bool() const {
        return ptr != nullptr;
    }

    bool operator!() const {
        return ptr == nullptr;
    }

    T& operator[](size_t index) const {
        return ptr[index];
    }

    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    void reset(T* p = nullptr) {
        if (ptr != p) {
            delete[] ptr;
            ptr = p;
        }
    }

    T* get() const {
        return ptr;
    }
};

#endif // UNQPTR_H
