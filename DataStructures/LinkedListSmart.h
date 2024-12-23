#ifndef LINKEDLISTSMART_H
#define LINKEDLISTSMART_H

#include "Sequence.h"
#include <memory>
#include <stdexcept>

#define LINKEDLIST_EMPTY "LinkedListSmart is empty"
#define LINKEDLIST_OUT_OF_RANGE "Index out of range"
#define LINKEDLIST_SUBSEQ_ERR "Invalid subsequence indices"

template <typename T>
class LinkedListSmart : public Sequence<T> {
private:
    struct Node {
        T data;
        std::shared_ptr<Node> next;

        explicit Node(const T& item) : data(item), next(nullptr) {}
    };

    std::shared_ptr<Node> head;
    size_t length;

public:
    LinkedListSmart() : head(nullptr), length(0) {}
    ~LinkedListSmart() override = default;

    T& GetFirst() const override {
        if (!head)
            throw std::out_of_range(LINKEDLIST_EMPTY);
        return head->data;
    }

    T& GetLast() const override {
        if (!head)
            throw std::out_of_range(LINKEDLIST_EMPTY);
        auto current = head;
        while (current->next) {
            current = current->next;
        }
        return current->data;
    }

    T& Get(int index) const override {
        if (index < 0 || static_cast<size_t>(index) >= length)
            throw std::out_of_range(LINKEDLIST_OUT_OF_RANGE);
        auto current = head;
        for (int i = 0; i < index; ++i) {
            current = current->next;
        }
        return current->data;
    }

    Sequence<T>* GetSubsequence(int startIndex, int endIndex) const override {
        if (startIndex < 0 || endIndex >= static_cast<int>(length) || startIndex > endIndex)
            throw std::out_of_range(LINKEDLIST_SUBSEQ_ERR);

        auto subseq = new LinkedListSmart<T>();
        auto current = head;
        for (int i = 0; i <= endIndex; ++i) {
            if (i >= startIndex) {
                subseq->Append(current->data);
            }
            current = current->next;
        }
        return subseq;
    }

    int GetLength() const override {
        return static_cast<int>(length);
    }

    void Append(const T& item) override {
        auto newNode = std::make_shared<Node>(item);
        if (!head) {
            head = newNode;
        } else {
            auto current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
        ++length;
    }

    void Prepend(const T& item) override {
        auto newNode = std::make_shared<Node>(item);
        newNode->next = head;
        head = newNode;
        ++length;
    }

    void InsertAt(const T& item, int index) override {
        if (index < 0 || index > static_cast<int>(length))
            throw std::out_of_range(LINKEDLIST_OUT_OF_RANGE);
        if (index == 0) {
            Prepend(item);
        } else {
            auto current = head;
            for (int i = 0; i < index - 1; ++i) {
                current = current->next;
            }
            auto newNode = std::make_shared<Node>(item);
            newNode->next = current->next;
            current->next = newNode;
            ++length;
        }
    }

    void RemoveAt(int index) override {
        if (index < 0 || index >= static_cast<int>(length))
            throw std::out_of_range(LINKEDLIST_OUT_OF_RANGE);

        if (index == 0) {
            if (head) {
                head = head->next;
                --length;
            }
        } else {
            auto current = head;
            for (int i = 0; i < index - 1; ++i) {
                current = current->next;
            }

            if (current->next) {
                current->next = current->next->next;
                --length;
            }
        }
    }

    Sequence<T>* Concat(Sequence<T>* list) const override {
        auto newList = new LinkedListSmart<T>();
        auto current = head;
        while (current) {
            newList->Append(current->data);
            current = current->next;
        }
        for (int i = 0; i < list->GetLength(); ++i) {
            newList->Append(list->Get(i));
        }
        return newList;
    }
};

#endif // LINKEDLISTSMART_H
