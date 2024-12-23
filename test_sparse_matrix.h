#ifndef TEST_SPARSE_MATRIX_H
#define TEST_SPARSE_MATRIX_H

#include "DataStructures/SparseMatrix.h"
#include "DataStructures/HashTable.h"
#include "DataStructures/BTree.h"
#include <iostream>
#include <chrono>

inline void test_sparse_matrix() {
    std::cout << "Testing SparseMatrix...\n";

    {
        std::cout << "  Using HashTable as storage...\n";
        UnqPtr<IDictionary<IndexPair, double>> dictionary(new HashTable<IndexPair, double>());
        SparseMatrix<double> matrix(3, 3, std::move(dictionary));

        matrix.SetElement(0, 0, 1.0);
        matrix.SetElement(1, 1, 2.0);
        matrix.SetElement(2, 2, 3.0);

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                std::cout << "    Element[" << i << "][" << j << "] = " << matrix.GetElement(i, j) << "\n";
            }
        }
    }

    {
        std::cout << "  Using BTree as storage...\n";
        UnqPtr<IDictionary<IndexPair, double>> dictionary(new BTree<IndexPair, double>());
        SparseMatrix<double> matrix(3, 3, std::move(dictionary));

        matrix.SetElement(0, 0, 1.0);
        matrix.SetElement(1, 1, 2.0);
        matrix.SetElement(2, 2, 3.0);

        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                std::cout << "    Element[" << i << "][" << j << "] = " << matrix.GetElement(i, j) << "\n";
            }
        }
    }

    std::cout << "SparseMatrix tests completed.\n";
}

inline std::pair<double, double> load_test_sparse_matrix(const int& size, const int& numElements) {
    if (size < 0) {
        std::cerr << "Error: size must be non-negative.\n";
        return {};
    } else if (numElements < 0) {
        std::cerr << "Error: numElements must be non-negative.\n";
        return{};
    }
    std::pair<double, double> hashTableBTreeTime;
    std::cout << "Load Testing SparseMatrix...\n";

    {
        std::cout << "  Using HashTable as storage...\n";
        UnqPtr<IDictionary<IndexPair, double>> dictionary(new HashTable<IndexPair, double>());
        SparseMatrix<double> matrix(size, size, std::move(dictionary));

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < numElements; ++i) {
            int row = i % size;
            int col = (i * 31) % size;
            matrix.SetElement(row, col, static_cast<double>(i));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        hashTableBTreeTime.first = duration.count();
        std::cout << "    Time taken: " << duration.count() << " seconds\n";
    }

    {
        std::cout << "  Using BTree as storage...\n";
        UnqPtr<IDictionary<IndexPair, double>> dictionary(new BTree<IndexPair, double>());
        SparseMatrix<double> matrix(size, size, std::move(dictionary));

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < numElements; ++i) {
            int row = i % size;
            int col = (i * 31) % size;
            matrix.SetElement(row, col, static_cast<double>(i));
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> duration = end - start;
        hashTableBTreeTime.second = duration.count();
        std::cout << "    Time taken: " << duration.count() << " seconds\n";
    }

    std::cout << "SparseMatrix load tests completed.\n";
    return hashTableBTreeTime;
}


#endif // TEST_SPARSE_MATRIX_H
