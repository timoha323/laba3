#include "test_sparse_vector.h"
#include "test_sparse_matrix.h"
#include "interface.h"
#include "DataStructures/BTree.h"
#include <iostream>
#include <vector>

void TestBTree() {
    BTree<int, std::string> tree;

    tree.Add(10, "Ten");
    tree.Add(20, "Twenty");
    tree.Add(5, "Five");
    tree.Add(6, "Six");
    tree.Add(12, "Twelve");
    tree.Add(30, "Thirty");
    tree.Add(7, "Seven");
    tree.Add(17, "Seventeen");

    std::cout << "B-Tree structure after additions:\n";
    tree.PrintStructure();

    std::cout << "\nSearch tests:\n";
    std::cout << "Search 6: " << (tree.ContainsKey(6) ? "Found" : "Not Found") << "\n";
    std::cout << "Search 15: " << (tree.ContainsKey(15) ? "Found" : "Not Found") << "\n";

    tree.Remove(6);
    std::cout << "\nB-Tree structure after removing 6:\n";
    tree.PrintStructure();
    try {
        tree.Remove(13);
        std::cout << "\nB-Tree structure after removing 13 (non-existent):\n";
    } catch (const std::exception& e) {
        std::cout << "EXPECTED EXCEPTION\n";
        std::cout << e.what() << "\n";
    }
    tree.PrintStructure();
}

int main() {
    TestBTree();

    const std::vector<Point> result_time_matrix_hash =
    {
        {load_test_sparse_matrix(10000000, 100000).first, 1},
        {load_test_sparse_matrix(1000000, 10000).first, 2},
        {load_test_sparse_matrix(100000, 1000).first, 3}
    };
    const std::vector<Point> result_time_matrix_b =
    {

        {load_test_sparse_matrix(10000000, 100000).second, 1},
        {load_test_sparse_matrix(1000000, 10000).second, 2},
        {load_test_sparse_matrix(100000, 1000).second, 3}
    };

    const std::vector<Point> result_time_matrix_compare =
    {
        {load_test_sparse_matrix(100000000, 10000).first, 1},
        {load_test_sparse_matrix(1000000, 10000).first, 2},
        {load_test_sparse_matrix(1000000, 1000).first, 3},
        {load_test_sparse_matrix(100000000, 10000).second, 1},
        {load_test_sparse_matrix(1000000, 10000).second, 2},
        {load_test_sparse_matrix(1000000, 1000).second, 3}
    };

    std::cout << "\n------------------------\n";
    const std::vector<Point> result_time_vector_hash =
    {
        {load_test_sparse_vector(1000000000, 100000).first, 1},
        {load_test_sparse_vector(10000000, 100000).first, 2},
        {load_test_sparse_vector(10000000, 10000).first, 3}
    };
    const std::vector<Point> result_time_vector_b =
    {
        {load_test_sparse_vector(1000000000, 100000).second, 1},
        {load_test_sparse_vector(100000000, 100000).second, 2},
        {load_test_sparse_vector(10000000, 10000).second, 3}
    };
    const std::vector<Point> result_time_vector_compare =
    {
        {load_test_sparse_vector(100000000, 10000).first, 1},
        {load_test_sparse_vector(1000000, 10000).first, 2},
        {load_test_sparse_vector(1000000, 1000).first, 3},
        {load_test_sparse_vector(100000000, 10000).second, 1},
        {load_test_sparse_vector(1000000, 10000).second, 2},
        {load_test_sparse_vector(1000000, 1000).second, 3}
    };
    std::cout << "\n------------------------\n";
    std::cout << "Result time vector\n\n\n";
    std::cout << "\nResult time HASH TABLE\n\n\n";
    plotGraph(result_time_vector_hash);
    std::cout << "\nResult time B_TREE\n\n\n";
    plotGraph(result_time_vector_b);
    std::cout << "\nResult time COMPARE\n\n\n";
    plotGraph(result_time_vector_compare);
    std::cout << "\n------------------------\n";
    std::cout << "Result time matrix\n\n\n";
    std::cout << "\nResult time HASH TABLE\n\n\n";
    plotGraph(result_time_matrix_hash);
    std::cout << "\nResult time B_TREE\n\n\n";
    plotGraph(result_time_matrix_b);
    std::cout << "\nResult time COMPARE\n\n\n";
    plotGraph(result_time_matrix_compare);
    std::cout << "\n------------------------\n";
    test_sparse_vector();
    test_sparse_matrix();
    displayMenu();
}
