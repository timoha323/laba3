#ifndef TEST_SPARSE_VECTOR_H
#define TEST_SPARSE_VECTOR_H

#include "DataStructures/SparseVector.h"
#include "DataStructures/HashTable.h"
#include "DataStructures/BTree.h"
#include <iostream>
#include <chrono>

inline void test_sparse_vector() {
    std::cout << "Testing SparseVector...\n";

    {
        std::cout << "  Using HashTable as storage...\n";
        UnqPtr<IDictionary<int, double>> dictionary(new HashTable<int, double>());
        SparseVector<double> vector(5, std::move(dictionary));

        vector.SetElement(0, 1.0);
        vector.SetElement(2, 3.5);
        vector.SetElement(4, -2.2);

        for (int i = 0; i < 5; ++i) {
            std::cout << "    Element[" << i << "] = " << vector.GetElement(i) << "\n";
        }
    }

    {
        std::cout << "  Using BTree as storage...\n";
        UnqPtr<IDictionary<int, double>> dictionary(new BTree<int, double>());
        SparseVector<double> vector(5, std::move(dictionary));

        vector.SetElement(0, 1.0);
        vector.SetElement(2, 3.5);
        vector.SetElement(4, -2.2);

        for (int i = 0; i < 5; ++i) {
            std::cout << "    Element[" << i << "] = " << vector.GetElement(i) << "\n";
        }
    }

    std::cout << "SparseVector tests completed.\n";
}

inline std::pair<double, double> load_test_sparse_vector(const int& size, const int& numElements) {
    if (size < 0) {
        std::cerr << "Error: size must be non-negative.\n";
        return{};
    } else if (numElements < 0) {
        std::cerr << "Error: numElements must be non-negative.\n";
        return{};
    }
    std::pair<double, double> hashTableBTreeTime;

    std::cout << "Load Testing SparseVector...\n";

    {
        std::cout << "  Using HashTable as storage...\n";
        UnqPtr<IDictionary<int, double>> dictionary(new HashTable<int, double>());
        SparseVector<double> vector(size, std::move(dictionary));

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < numElements; ++i) {
            int index = (i * 31) % size; // Generate some pseudo-random indices
            vector.SetElement(index, static_cast<double>(i));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        hashTableBTreeTime.first = duration.count();
        std::cout << "    Time taken: " << duration.count() << " seconds\n";
    }

    {
        std::cout << "  Using BTree as storage...\n";
        UnqPtr<IDictionary<int, double>> dictionary(new BTree<int, double>());
        SparseVector<double> vector(size, std::move(dictionary));

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < numElements; ++i) {
            int index = (i * 31) % size;
            vector.SetElement(index, static_cast<double>(i));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        hashTableBTreeTime.second = duration.count();
        std::cout << "    Time taken: " << duration.count() << " seconds\n";
    }
    std::cout << "SparseVector load tests completed.\n";
    return hashTableBTreeTime;
}

#endif // TEST_SPARSE_VECTOR_H
