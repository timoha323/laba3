#include "test_sparse_vector.h"
#include "test_sparse_matrix.h"
#include "interface.h"
#include "DataStructures/BTree.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <utility>


void save_data_to_file(const std::string& filename, const std::vector<Point>& data) {
    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    for (const auto& point : data) {
        outfile << point.first << " " << point.second << "\n";
    }
    outfile.close();
}

void generate_python_script() {
    std::ofstream pyfile("plot_graphs.py");
    if (!pyfile.is_open()) {
        throw std::runtime_error("Failed to create Python script.");
    }
    pyfile << "import matplotlib.pyplot as plt\n";
    pyfile << "import os\n\n";
    pyfile << "def plot_file(filename):\n";
    pyfile << "    times = []\n";
    pyfile << "    elements = []\n";
    pyfile << "    with open(filename, 'r') as f:\n";
    pyfile << "        for line in f:\n";
    pyfile << "            time, num_elements = map(float, line.split())\n";
    pyfile << "            times.append(time)\n";
    pyfile << "            elements.append(num_elements)\n";
    pyfile << "    plt.plot(elements, times, label=filename)\n\n";
    pyfile << "def main():\n";
    pyfile << "    files = [\"result_time_matrix_hash.txt\", \"result_time_matrix_b.txt\", \"result_time_vector_hash.txt\", \"result_time_vector_b.txt\"]\n";
    pyfile << "    for file in files:\n";
    pyfile << "        if os.path.exists(file):\n";
    pyfile << "            plot_file(file)\n";
    pyfile << "    plt.xlabel('Number of Elements')\n";
    pyfile << "    plt.ylabel('Time')\n";
    pyfile << "    plt.legend()\n";
    pyfile << "    plt.title('Performance Graphs')\n";
    pyfile << "    plt.show()\n\n";
    pyfile << "if __name__ == '__main__':\n";
    pyfile << "    main()\n";
    pyfile.close();
}

void TestBTree() {
    BTree<int, std::string> tree1;
    tree1.Add(10, "Ten");
    tree1.PrintStructure();
    tree1.Add(20, "Twenty");
    tree1.PrintStructure();
    tree1.Add(5, "Five");
    tree1.PrintStructure();
    tree1.Add(15, "Fifteen");
    tree1.PrintStructure();
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
        {load_test_sparse_matrix(10000000, 100000).first, 100000},
        {load_test_sparse_matrix(1000000, 10000).first, 10000},
        {load_test_sparse_matrix(100000, 1000).first, 1000}
    };
    const std::vector<Point> result_time_matrix_b =
    {
        {load_test_sparse_matrix(10000000, 100000).second, 100000},
        {load_test_sparse_matrix(1000000, 10000).second, 10000},
        {load_test_sparse_matrix(100000, 1000).second, 1000}
    };

    std::cout << "\n------------------------\n";
    const std::vector<Point> result_time_vector_hash =
    {
        {load_test_sparse_vector(1000000000, 100000).first, 100000},
        {load_test_sparse_vector(10000000, 100000).first, 100000},
        {load_test_sparse_vector(10000000, 10000).first, 10000}
    };
    const std::vector<Point> result_time_vector_b =
    {
        {load_test_sparse_vector(1000000000, 100000).second, 100000},
        {load_test_sparse_vector(100000000, 100000).second, 100000},
        {load_test_sparse_vector(10000000, 10000).second, 10000}
    };
    try {
        save_data_to_file("result_time_matrix_hash.txt", result_time_matrix_hash);
        save_data_to_file("result_time_matrix_b.txt", result_time_matrix_b);
        save_data_to_file("result_time_vector_hash.txt", result_time_vector_hash);
        save_data_to_file("result_time_vector_b.txt", result_time_vector_b);

        generate_python_script();

        system("python3 plot_graphs.py");
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    std::cout << "\n------------------------\n";
    std::cout << "Result time vector\n\n\n";
    std::cout << "\nResult time HASH TABLE\n\n\n";
    plotGraph(result_time_vector_hash);
    std::cout << "\nResult time B_TREE\n\n\n";
    plotGraph(result_time_vector_b);
    std::cout << "\n------------------------\n";
    std::cout << "Result time matrix\n\n\n";
    std::cout << "\nResult time HASH TABLE\n\n\n";
    plotGraph(result_time_matrix_hash);
    std::cout << "\nResult time B_TREE\n\n\n";
    plotGraph(result_time_matrix_b);
    std::cout << "\n------------------------\n";
    test_sparse_vector();
    test_sparse_matrix();
    displayMenu();
}
