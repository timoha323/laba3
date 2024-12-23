#include "test_btree.h"
#include <cstdlib>
#include <sstream>
#include <queue>

BTreeTest::BTreeTest(int order)
        : btree(order)
{
}

void BTreeTest::AddElement(int key, const std::string& value) {
    try {
        btree.Add(key, value);
        std::cout << "Added key: " << key << ", value: " << value << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error adding key " << key << ": " << e.what() << std::endl;
    }
}

void BTreeTest::RemoveElement(int key) {
    try {
        btree.Remove(key);
        std::cout << "Removed key: " << key << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error removing key " << key << ": " << e.what() << std::endl;
    }
}

void BTreeTest::FillRandom(int numElements, int keyMin, int keyMax) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(keyMin, keyMax);
    std::uniform_int_distribution<> dis_val(1, 1000);

    for (int i = 0; i < numElements; ++i) {
        int key = dis(gen);
        std::string value = "Val" + std::to_string(dis_val(gen));
        try {
            btree.Add(key, value);
            std::cout << "Added key: " << key << ", value: " << value << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error adding key " << key << ": " << e.what() << std::endl;
        }
    }
}

void BTreeTest::Visualize(const std::string& filename) {
    std::ofstream dotFile("btree.dot");
    if (!dotFile.is_open()) {
        std::cerr << "Failed to open btree.dot for writing." << std::endl;
        return;
    }

    dotFile << "digraph BTree {\n";
    dotFile << "    node [shape=record];\n";

    int nodeCount = 0;
    int nullCount = 0;
    Traverse(btree.root.get(), nodeCount, dotFile, -1, -1, nullCount);

    dotFile << "}\n";
    dotFile.close();

    std::string command = "dot -Tpng btree.dot -o " + filename;
    int ret = system(command.c_str());
    if (ret != 0) {
        std::cerr << "Failed to generate PNG using Graphviz." << std::endl;
    }
    else {
        std::cout << "BTree visualization saved as " << filename << std::endl;
    }
}

void BTreeTest::Traverse(const BTree<int, std::string>::Node* node, int& nodeCount, std::ostream& out, int parent, int childIndex, int& nullCount) const {
    if (!node)
        return;

    int currentNodeId = nodeCount++;
    std::stringstream nodeLabel;
    nodeLabel << "<c0> |";

    for (int i = 0; i < node->numKeys; ++i) {
        if (i == node->numKeys -1){
            nodeLabel << "<k" << i + 1 << "> " << node->keys[i] << " | <c" << i + 1 << "> ";
        }
        else{
            nodeLabel << "<k" << i + 1 << "> " << node->keys[i] << " | <c" << i + 1 << "> |";
        }

    }


    out << "    node" << currentNodeId << " [label=\"{" << nodeLabel.str() << "}\"];\n";

    if (parent != -1) {
        out << "    node" << parent << ":c" << childIndex << " -> node" << currentNodeId << ":c0;\n";
    }

    if (!node->isLeaf) {
        for (int i = 0; i <= node->numKeys; ++i) {
            if (node->children[i]) {
                Traverse(node->children[i].get(), nodeCount, out, currentNodeId, i, nullCount);
            }
            else {
                int nullId = nullCount++;
                out << "    null" << nullId << " [shape=point];\n";
                out << "    node" << currentNodeId << ":c" << i << " -> null" << nullId << ";\n";
            }
        }
    }
}

void BTreeTest::DisplayTree() {
    auto iterator = btree.GetIterator();
    while (iterator->MoveNext()) {
        int key = iterator->GetCurrentKey();
        std::string value = iterator->GetCurrentValue();
        std::cout << "Key: " << key << ", Value: " << value << std::endl;
    }
}

bool BTreeTest::CheckBalance() {
    if (!btree.root) {
        std::cout << "The tree is empty." << std::endl;
        return true;
    }

    int leafDepth = -1;
    std::queue<std::pair<const BTree<int, std::string>::Node*, int>> q;
    q.push({ btree.root.get(), 0 });

    while (!q.empty()) {
        auto [current, depth] = q.front();
        q.pop();

        if (current->isLeaf) {
            if (leafDepth == -1) {
                leafDepth = depth;
            }
            else if (depth != leafDepth) {
                std::cout << "The tree is not balanced." << std::endl;
                return false;
            }
        }

        if (!current->isLeaf) {
            for (int i = 0; i <= current->numKeys; ++i) {
                if (current->children[i])
                    q.push({ current->children[i].get(), depth + 1 });
            }
        }
    }

    std::cout << "The tree is balanced." << std::endl;
    return true;
}

int BTreeTest::GetDepth(const BTree<int, std::string>::Node* node) const {
    if (node->isLeaf)
        return 0;
    return 1 + GetDepth(node->children[0].get());
}
