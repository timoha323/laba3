#include "test.h"
#include "DataStructures/SparseVector.h"
#include "DataStructures/SparseMatrix.h"
#include "DataStructures/BTree.h"
#include "DataStructures/UnqPtr.h"
#include "DataStructures/HashTable.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <string>
#include <cstdlib>
#include <unordered_set>
#include <algorithm>
#include <random>

void run_tests() {
    std::cout << "Starting functional tests..." << std::endl;
    functional_tests();
    std::cout << "Functional tests completed." << std::endl;

    std::cout << "\nStarting performance tests..." << std::endl;
    performance_tests();
    std::cout << "Performance tests completed." << std::endl;
}

void functional_tests() {
    test_dictionary<HashTable<int, std::string>, int, std::string>("HashTable");

    test_dictionary<BTree<int, std::string>, int, std::string>("BTree");

    test_sparse_vector<HashTable<int, double>>("HashTable", true);
    test_sparse_vector<BTree<int, double>>("BTree", true);

    test_sparse_matrix<HashTable<IndexPair, double>>("HashTable", true);
    test_sparse_matrix<BTree<IndexPair, double>>("BTree", true);

    std::cout << "All functional tests completed successfully." << std::endl;
}

template <typename DictionaryType, typename KeyType, typename ValueType>
void test_dictionary(const std::string& dictionary_name) {
    std::cout << "Testing " << dictionary_name << "..." << std::endl;
    DictionaryType dictionary;

    dictionary.Add(1, "One");
    dictionary.Add(2, "Two");
    dictionary.Add(3, "Three");

    try {
        ValueType value = dictionary.Get(2);
        if (value != "Two") {
            std::cerr << "Error: expected 'Two', got '" << value << "'" << std::endl;
        } else {
            std::cout << "Get(2) succeeded, value: " << value << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception during Get: " << e.what() << std::endl;
    }

    dictionary.Update(2, "Second");
    try {
        ValueType value = dictionary.Get(2);
        if (value != "Second") {
            std::cerr << "Error in Update: expected 'Second', got '" << value << "'" << std::endl;
        } else {
            std::cout << "Update succeeded, new value of Get(2): " << value << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception during Update verification: " << e.what() << std::endl;
    }

    dictionary.Remove(3);
    if (dictionary.ContainsKey(3)) {
        std::cerr << "Error: Key 3 should have been removed." << std::endl;
    } else {
        std::cout << "Remove succeeded, key 3 is no longer in the " << dictionary_name << "." << std::endl;
    }

    std::cout << "Iterating over " << dictionary_name << ":" << std::endl;
    auto iterator = dictionary.GetIterator();
    while (iterator->MoveNext()) {
        std::cout << "Key: " << iterator->GetCurrentKey()
                  << ", Value: " << iterator->GetCurrentValue() << std::endl;
    }
}

template <typename DictionaryType>
void test_sparse_vector(const std::string& dictionary_name, bool extended) {
    std::cout << "Testing SparseVector with " << dictionary_name << "..." << std::endl;
    UnqPtr<IDictionary<int, double>> dictionary(new DictionaryType());
    SparseVector<double> vector(10, std::move(dictionary));

    vector.SetElement(0, 1.0);
    vector.SetElement(2, 3.5);
    vector.SetElement(4, -2.2);

    try {
        double value = vector.GetElement(2);
        if (value != 3.5) {
            std::cerr << "Error: expected 3.5, got " << value << std::endl;
        } else {
            std::cout << "GetElement(2) succeeded, value: " << value << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception during element retrieval: " << e.what() << std::endl;
    }

    vector.Map([](double x) { return x * 2; });

    try {
        double value = vector.GetElement(2);
        if (value != 7.0) {
            std::cerr << "Error in Map: expected 7.0, got " << value << std::endl;
        } else {
            std::cout << "Map succeeded, new value of GetElement(2): " << value << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception during Map verification: " << e.what() << std::endl;
    }

    double sum = vector.Reduce([](double acc, double x) { return acc + x; }, 0.0);
    double expected_sum = 2.0 + 0.0 + 7.0 + 0.0 + (-4.4);
    if (sum != expected_sum) {
        std::cerr << "Error in Reduce: expected " << expected_sum << ", got " << sum << std::endl;
    } else {
        std::cout << "Reduce succeeded, sum: " << sum << std::endl;
    }

    if (extended) {
        std::cout << "Extended Testing SparseVector with " << dictionary_name << "..." << std::endl;

        vector.SetElement(3, 3.3);
        vector.SetElement(5, 5.5);
        vector.SetElement(7, 7.7);
        vector.SetElement(9, 9.9);

        for (int i = 0; i < vector.GetLength(); ++i) {
            double value = vector.GetElement(i);
            std::cout << "Element at index " << i << ": " << value << std::endl;
        }

        vector.Map([](double x) { return x * -1; });

        try {
            double value = vector.GetElement(5);
            if (value != -5.5) {
                std::cerr << "Error in Map: expected -5.5, got " << value << std::endl;
            } else {
                std::cout << "Map succeeded, new value of GetElement(5): " << value << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception during Map verification: " << e.what() << std::endl;
        }

        double sum_extended = vector.Reduce([](double acc, double x) { return acc + x; }, 0.0);
        std::cout << "Reduce result (sum): " << sum_extended << std::endl;

        std::cout << "SparseVector elements after Map:" << std::endl;
        vector.ForEach([](int index, const double& value) {
            std::cout << "Index: " << index << ", Value: " << value << std::endl;
        });

        vector.RemoveElement(5);
        if (vector.GetElement(5) != 0.0) {
            std::cerr << "Error: Element at index 5 should have been removed." << std::endl;
        } else {
            std::cout << "RemoveElement succeeded, element at index 5 is now zero." << std::endl;
        }
    }
}

template <typename DictionaryType>
void test_sparse_matrix(const std::string& dictionary_name, bool extended) {
    std::cout << "Testing SparseMatrix with " << dictionary_name << "..." << std::endl;
    UnqPtr<IDictionary<IndexPair, double>> dictionary(new DictionaryType());
    SparseMatrix<double> matrix(4, 4, std::move(dictionary));

    matrix.SetElement(0, 0, 1.0);
    matrix.SetElement(1, 1, 2.0);
    matrix.SetElement(2, 2, 3.0);

    matrix.Map([](double x) { return x + 1; });

    try {
        double value = matrix.GetElement(1, 1);
        if (value != 3.0) {
            std::cerr << "Error in Map: expected 3.0, got " << value << std::endl;
        } else {
            std::cout << "Map succeeded, new value of GetElement(1,1): " << value << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception during Map verification: " << e.what() << std::endl;
    }

    double sum = matrix.Reduce([](double acc, double x) { return acc + x; }, 0.0);
    if (sum != 9.0) {
        std::cerr << "Error in Reduce: expected 9.0, got " << sum << std::endl;
    } else {
        std::cout << "Reduce succeeded, sum: " << sum << std::endl;
    }

    if (extended) {
        std::cout << "Extended Testing SparseMatrix with " << dictionary_name << "..." << std::endl;

        matrix.SetElement(3, 3, 4.0);
        matrix.SetElement(0, 3, 5.0);
        matrix.SetElement(3, 0, 6.0);

        for (int i = 0; i < matrix.GetRows(); ++i) {
            for (int j = 0; j < matrix.GetColumns(); ++j) {
                double value = matrix.GetElement(i, j);
                std::cout << "Element at (" << i << ", " << j << "): " << value << std::endl;
            }
        }

        matrix.Map([](double x) { return x + 10; });

        try {
            double value = matrix.GetElement(2, 2);
            if (value != 14.0) {
                std::cerr << "Error in Map: expected 14.0, got " << value << std::endl;
            } else {
                std::cout << "Map succeeded, new value of GetElement(2,2): " << value << std::endl;
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception during Map verification: " << e.what() << std::endl;
        }

        double sum_extended = matrix.Reduce([](double acc, double x) { return acc + x; }, 0.0);
        std::cout << "Reduce result (sum): " << sum_extended << std::endl;

        std::cout << "SparseMatrix elements after Map:" << std::endl;
        matrix.ForEach([](const IndexPair& index, const double& value) {
            std::cout << "Position: (" << index.row << ", " << index.column << "), Value: " << value << std::endl;
        });

        matrix.RemoveElement(1, 1);
        if (matrix.GetElement(1, 1) != 0.0) {
            std::cerr << "Error: Element at (1,1) should have been removed." << std::endl;
        } else {
            std::cout << "RemoveElement succeeded, element at (1,1) is now zero." << std::endl;
        }
    }
}

template<typename Func>
long long measure_time(Func func) {
    auto start = std::chrono::high_resolution_clock::now();
    func();
    auto finish = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
}

template<typename TDictionary>
void performance_test_vector(int size, const std::string& dict_name, std::ostream& log_stream) {
    UnqPtr<IDictionary<int, double>> dictionary(new TDictionary());
    SparseVector<double> vector(size, std::move(dictionary));

    long long num_elements = std::max(1LL, (long long)size / 10LL);
    std::unordered_set<int> indices;
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis(0, size - 1);

    long long insertion_time = measure_time([&]() {
        while (indices.size() < (size_t)num_elements) {
            indices.insert(dis(gen));
        }

        for (int idx : indices) {
            vector.SetElement(idx, static_cast<double>(std::rand()) / RAND_MAX);
        }
    });

    long long search_time = measure_time([&]() {
        for (int idx : indices) {
            vector.GetElement(idx);
        }
    });

    long long map_time = measure_time([&]() {
        vector.Map([](double x) { return x * 2; });
    });

    long long reduce_time = measure_time([&]() {
        vector.Reduce([](double acc, double x) { return acc + x; }, 0.0);
    });

    long long update_time = measure_time([&]() {
        for (int idx : indices) {
            vector.SetElement(idx, static_cast<double>(std::rand()) / RAND_MAX);
        }
    });

    long long iteration_time = measure_time([&]() {
        UnqPtr<IDictionaryIterator<int, double>> iterator = vector.GetIterator();
        while (iterator->MoveNext()) {
            volatile double val = iterator->GetCurrentValue();
            (void)val;
        }
    });

    log_stream << dict_name << ",Vector," << size << "," << num_elements << ","
               << insertion_time << "," << search_time << "," << map_time << ","
               << reduce_time << "," << update_time << "," << iteration_time << "\n";
}

template<typename TDictionary>
void performance_test_matrix(int size, const std::string& dict_name, std::ostream& log_stream) {
    int rows = std::max(1, size);
    int cols = std::max(1, size);
    UnqPtr<IDictionary<IndexPair, double>> dictionary(new TDictionary());
    SparseMatrix<double> matrix(rows, cols, std::move(dictionary));

    long long total_elements = (long long)rows * (long long)cols;
    long long num_elements = std::max(1LL, total_elements / 10LL);
    std::unordered_set<long long> index_set;
    std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dis_row(0, rows - 1);
    std::uniform_int_distribution<> dis_col(0, cols - 1);

    long long insertion_time = measure_time([&]() {
        while (index_set.size() < (size_t)num_elements) {
            int i = dis_row(gen);
            int j = dis_col(gen);
            index_set.insert((long long)i * (long long)cols + j);
        }

        for (long long key : index_set) {
            int i = (int)(key / cols);
            int j = (int)(key % cols);
            matrix.SetElement(i, j, static_cast<double>(std::rand()) / RAND_MAX);
        }
    });

    std::vector<IndexPair> indices;
    indices.reserve(index_set.size());
    for (long long key : index_set) {
        int i = (int)(key / cols);
        int j = (int)(key % cols);
        indices.emplace_back(i, j);
    }

    long long search_time = measure_time([&]() {
        for (const auto& idx : indices) {
            matrix.GetElement(idx.row, idx.column);
        }
    });

    long long map_time = measure_time([&]() {
        matrix.Map([](double x) { return x + 1; });
    });

    long long reduce_time = measure_time([&]() {
        matrix.Reduce([](double acc, double x) { return acc + x; }, 0.0);
    });

    long long update_time = measure_time([&]() {
        for (const auto& idx : indices) {
            matrix.SetElement(idx.row, idx.column, static_cast<double>(std::rand()) / RAND_MAX);
        }
    });

    long long iteration_time = measure_time([&]() {
        UnqPtr<IDictionaryIterator<IndexPair, double>> iterator = matrix.GetIterator();
        while (iterator->MoveNext()) {
            volatile double val = iterator->GetCurrentValue();
            (void)val;
        }
    });

    log_stream << dict_name << ",Matrix," << size << "," << num_elements << ","
               << insertion_time << "," << search_time << "," << map_time << ","
               << reduce_time << "," << update_time << "," << iteration_time << "\n";
}

std::vector<int> read_test_sizes(const std::string& filename) {
    std::vector<int> sizes;
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Failed to open configuration file: " << filename << std::endl;
        return sizes;
    }

    int size;
    while (file >> size) {
        sizes.push_back(size);
    }
    return sizes;
}

void performance_tests() {
    std::vector<int> sizes = read_test_sizes("config.txt");
    if (sizes.empty()) {
        std::cerr << "Cannot read sizes from config.txt" << std::endl;
        return;
    }

    std::ofstream log_file("performance_results.csv");
    if (!log_file.is_open()) {
        std::cerr << "Cannot open the file performance_results.csv for writing." << std::endl;
        return;
    }

    log_file << "Dictionary,Structure,Size,NumElements,InsertionTime(ms),SearchTime(ms),MapTime(ms),ReduceTime(ms),UpdateTime(ms),IterationTime(ms)\n";

    for (size_t i = 0; i < sizes.size(); ++i) {
        int size = sizes[i];
        std::cout << "\nTesting with data size: " << size << std::endl;

        if (i % 2 == 0) {
            performance_test_vector<HashTable<int, double>>(size, "HashTable", log_file);
            performance_test_vector<BTree<int, double>>(size, "BTree", log_file);
        } else {
            performance_test_matrix<HashTable<IndexPair, double>>(size, "HashTable", log_file);
            performance_test_matrix<BTree<IndexPair, double>>(size, "BTree", log_file);
        }
    }

    log_file.close();
    std::cout << "Performance tests completed. Results saved in performance_results.csv" << std::endl;
}
