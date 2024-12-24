#ifndef BTREE_H
#define BTREE_H

#include "IDictionary.h"
#include "ShrdPtr.h"
#include "DynamicArraySmart.h"
#include "UnqPtr.h"
#include <iostream>
#include <stdexcept>

template<typename TKey, typename TElement>
class BTree : public IDictionary<TKey, TElement> {
public:
    BTree(int order = 3);

    virtual ~BTree();

    virtual size_t GetCount() const override;

    virtual size_t GetCapacity() const override;

    virtual TElement Get(const TKey &key) const override;

    virtual bool ContainsKey(const TKey &key) const override;

    virtual void Add(const TKey &key, const TElement &element) override;

    virtual void Remove(const TKey &key) override;

    virtual void Update(const TKey &key, const TElement &element) override;

    virtual UnqPtr<IDictionaryIterator<TKey, TElement>> GetIterator() const override;

private:
    struct Node {
        bool isLeaf;
        int numKeys;
        UnqPtr<TKey[]> keys;
        UnqPtr<TElement[]> values;
        UnqPtr<ShrdPtr<Node>[]> children;

        Node(bool leaf, int order);
    };

    ShrdPtr<Node> root;
    int order;
    size_t count;

    void SplitChild(ShrdPtr<Node> x, int i);

    void InsertNonFull(ShrdPtr<Node> x, const TKey &key, const TElement &value);

    TElement Search(ShrdPtr<Node> x, const TKey &key) const;

    bool Contains(ShrdPtr<Node> x, const TKey &key) const;

    void RemoveFromNode(ShrdPtr<Node> x, const TKey &key);

    void RemoveFromLeaf(ShrdPtr<Node> x, int idx);

    void RemoveFromNonLeaf(ShrdPtr<Node> x, int idx);

    TKey GetPredecessor(ShrdPtr<Node> x, int idx);

    TKey GetSuccessor(ShrdPtr<Node> x, int idx);

    void Fill(ShrdPtr<Node> x, int idx);

    void BorrowFromPrev(ShrdPtr<Node> x, int idx);

    void BorrowFromNext(ShrdPtr<Node> x, int idx);

    void Merge(ShrdPtr<Node> x, int idx);

    class BTreeIterator : public IDictionaryIterator<TKey, TElement> {
    public:
        BTreeIterator(const BTree *tree);

        virtual ~BTreeIterator() {}

        virtual bool MoveNext() override;

        virtual void Reset() override;

        virtual TKey GetCurrentKey() const override;

        virtual TElement GetCurrentValue() const override;

    private:
        const BTree *tree;
        struct StackNode {
            ShrdPtr<Node> node;
            int index;
        };
        DynamicArraySmart<StackNode> stack;
        TKey currentKey;
        TElement currentValue;
        bool hasCurrent;

        void PushLeftmost(ShrdPtr<Node> node);
    };

    friend class BTreeTest;

public:
    void PrintStructure(ShrdPtr<Node> node = ShrdPtr<Node>(), int depth = 0) const {
        auto currentNode = node ? node : root;
        if (!currentNode) return;

        for (int i = 0; i < depth; ++i) std::cout << "  ";
        std::cout << "[";
        for (int i = 0; i < currentNode->numKeys; ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << currentNode->keys[i];
        }
        std::cout << "]\n";

        if (!currentNode->isLeaf) {
            for (int i = 0; i <= currentNode->numKeys; ++i) {
                PrintStructure(currentNode->children[i], depth + 1);
            }
        }
    }
};

template<typename TKey, typename TElement>
BTree<TKey, TElement>::Node::Node(bool leaf, int order)
        : isLeaf(leaf), numKeys(0), keys(new TKey[2 * order - 1]), values(new TElement[2 * order - 1]),
          children(new ShrdPtr<Node>[2 * order]) {
}

template<typename TKey, typename TElement>
BTree<TKey, TElement>::BTree(int order)
        : root(new Node(true, order)), order(order), count(0) {
}

template<typename TKey, typename TElement>
BTree<TKey, TElement>::~BTree() {
}

template<typename TKey, typename TElement>
size_t BTree<TKey, TElement>::GetCount() const {
    return count;
}

template<typename TKey, typename TElement>
size_t BTree<TKey, TElement>::GetCapacity() const {
    return count;
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::Add(const TKey &key, const TElement &element) {
    if (ContainsKey(key)) {
        Update(key, element);
        return;
    }

    if (root->numKeys == 2 * order - 1) {
        ShrdPtr<Node> s(new Node(false, order));
        s->children[0] = root;
        SplitChild(s, 0);
        root = s;
    }

    InsertNonFull(root, key, element);
    ++count;
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::InsertNonFull(ShrdPtr<Node> x, const TKey &key, const TElement &value) {
    int i = x->numKeys - 1;

    if (x->isLeaf) {
        while (i >= 0 && key < x->keys[i]) {
            x->keys[i + 1] = x->keys[i];
            x->values[i + 1] = x->values[i];
            --i;
        }
        x->keys[i + 1] = key;
        x->values[i + 1] = value;
        ++x->numKeys;
    } else {
        while (i >= 0 && key < x->keys[i])
            --i;
        ++i;
        if (x->children[i]->numKeys == 2 * order - 1) {
            SplitChild(x, i);
            if (key > x->keys[i])
                ++i;
        }
        InsertNonFull(x->children[i], key, value);
    }
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::SplitChild(ShrdPtr<Node> x, int i) {
    ShrdPtr<Node> y = x->children[i];
    ShrdPtr<Node> z(new Node(y->isLeaf, order));
    z->numKeys = order - 1;

    for (int j = 0; j < order - 1; ++j) {
        z->keys[j] = y->keys[j + order];
        z->values[j] = y->values[j + order];
    }

    if (!y->isLeaf) {
        for (int j = 0; j < order; ++j)
            z->children[j] = y->children[j + order];
    }

    y->numKeys = order - 1;

    for (int j = x->numKeys; j >= i + 1; --j)
        x->children[j + 1] = x->children[j];
    x->children[i + 1] = z;

    for (int j = x->numKeys - 1; j >= i; --j) {
        x->keys[j + 1] = x->keys[j];
        x->values[j + 1] = x->values[j];
    }
    x->keys[i] = y->keys[order - 1];
    x->values[i] = y->values[order - 1];
    ++x->numKeys;
}

template<typename TKey, typename TElement>
TElement BTree<TKey, TElement>::Get(const TKey &key) const {
    return Search(root, key);
}

template<typename TKey, typename TElement>
TElement BTree<TKey, TElement>::Search(ShrdPtr<Node> x, const TKey &key) const {
    int i = 0;
    while (i < x->numKeys && key > x->keys[i])
        ++i;

    if (i < x->numKeys && key == x->keys[i])
        return x->values[i];

    if (x->isLeaf)
        throw std::runtime_error("Key not found.");
    else
        return Search(x->children[i], key);
}

template<typename TKey, typename TElement>
bool BTree<TKey, TElement>::ContainsKey(const TKey &key) const {
    try {
        Search(root, key);
        return true;
    }
    catch (const std::runtime_error &) {
        return false;
    }
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::Update(const TKey &key, const TElement &element) {
    ShrdPtr<Node> x = root;
    while (true) {
        int i = 0;
        while (i < x->numKeys && key > x->keys[i])
            ++i;

        if (i < x->numKeys && key == x->keys[i]) {
            x->values[i] = element;
            return;
        }

        if (x->isLeaf)
            throw std::runtime_error("Key not found.");

        x = x->children[i];
    }
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::Remove(const TKey &key) {
    if (!ContainsKey(key))
        throw std::runtime_error("Key not found.");

    RemoveFromNode(root, key);
    --count;

    if (root->numKeys == 0) {
        if (root->isLeaf) {
            root.reset(new Node(true, order));
        } else {
            root = root->children[0];
        }
    }
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::RemoveFromNode(ShrdPtr<Node> x, const TKey &key) {
    int idx = 0;
    while (idx < x->numKeys && x->keys[idx] < key)
        ++idx;

    if (idx < x->numKeys && x->keys[idx] == key) {
        if (x->isLeaf)
            RemoveFromLeaf(x, idx);
        else
            RemoveFromNonLeaf(x, idx);
    } else {
        if (x->isLeaf)
            throw std::runtime_error("Key not found.");

        bool flag = ((idx == x->numKeys));

        if (x->children[idx]->numKeys < order)
            Fill(x, idx);

        if (flag && idx > x->numKeys)
            RemoveFromNode(x->children[idx - 1], key);
        else
            RemoveFromNode(x->children[idx], key);
    }
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::RemoveFromLeaf(ShrdPtr<Node> x, int idx) {
    for (int i = idx + 1; i < x->numKeys; ++i) {
        x->keys[i - 1] = x->keys[i];
        x->values[i - 1] = x->values[i];
    }
    --x->numKeys;
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::RemoveFromNonLeaf(ShrdPtr<Node> x, int idx) {
    TKey k = x->keys[idx];

    if (x->children[idx]->numKeys >= order) {
        TKey predKey = GetPredecessor(x, idx);
        TElement predValue = x->values[idx];
        x->keys[idx] = predKey;
        x->values[idx] = predValue;
        RemoveFromNode(x->children[idx], predKey);
    } else if (x->children[idx + 1]->numKeys >= order) {
        TKey succKey = GetSuccessor(x, idx);
        TElement succValue = x->values[idx];
        x->keys[idx] = succKey;
        x->values[idx] = succValue;
        RemoveFromNode(x->children[idx + 1], succKey);
    } else {
        Merge(x, idx);
        RemoveFromNode(x->children[idx], k);
    }
}

template<typename TKey, typename TElement>
TKey BTree<TKey, TElement>::GetPredecessor(ShrdPtr<Node> x, int idx) {
    ShrdPtr<Node> cur = x->children[idx];
    while (!cur->isLeaf)
        cur = cur->children[cur->numKeys];
    return cur->keys[cur->numKeys - 1];
}

template<typename TKey, typename TElement>
TKey BTree<TKey, TElement>::GetSuccessor(ShrdPtr<Node> x, int idx) {
    ShrdPtr<Node> cur = x->children[idx + 1];
    while (!cur->isLeaf)
        cur = cur->children[0];
    return cur->keys[0];
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::Fill(ShrdPtr<Node> x, int idx) {
    if (idx != 0 && x->children[idx - 1]->numKeys >= order)
        BorrowFromPrev(x, idx);
    else if (idx != x->numKeys && x->children[idx + 1]->numKeys >= order)
        BorrowFromNext(x, idx);
    else {
        if (idx != x->numKeys)
            Merge(x, idx);
        else
            Merge(x, idx - 1);
    }
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::BorrowFromPrev(ShrdPtr<Node> x, int idx) {
    ShrdPtr<Node> child = x->children[idx];
    ShrdPtr<Node> sibling = x->children[idx - 1];

    for (int i = child->numKeys - 1; i >= 0; --i) {
        child->keys[i + 1] = child->keys[i];
        child->values[i + 1] = child->values[i];
    }

    if (!child->isLeaf) {
        for (int i = child->numKeys; i >= 0; --i)
            child->children[i + 1] = child->children[i];
    }

    child->keys[0] = x->keys[idx - 1];
    child->values[0] = x->values[idx - 1];

    if (!child->isLeaf)
        child->children[0] = sibling->children[sibling->numKeys];

    x->keys[idx - 1] = sibling->keys[sibling->numKeys - 1];
    x->values[idx - 1] = sibling->values[sibling->numKeys - 1];

    ++child->numKeys;
    --sibling->numKeys;
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::BorrowFromNext(ShrdPtr<Node> x, int idx) {
    ShrdPtr<Node> child = x->children[idx];
    ShrdPtr<Node> sibling = x->children[idx + 1];

    child->keys[child->numKeys] = x->keys[idx];
    child->values[child->numKeys] = x->values[idx];

    if (!child->isLeaf)
        child->children[child->numKeys + 1] = sibling->children[0];

    x->keys[idx] = sibling->keys[0];
    x->values[idx] = sibling->values[0];

    for (int i = 1; i < sibling->numKeys; ++i) {
        sibling->keys[i - 1] = sibling->keys[i];
        sibling->values[i - 1] = sibling->values[i];
    }

    if (!sibling->isLeaf) {
        for (int i = 1; i <= sibling->numKeys; ++i)
            sibling->children[i - 1] = sibling->children[i];
    }

    ++child->numKeys;
    --sibling->numKeys;
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::Merge(ShrdPtr<Node> x, int idx) {
    ShrdPtr<Node> child = x->children[idx];
    ShrdPtr<Node> sibling = x->children[idx + 1];

    child->keys[order - 1] = x->keys[idx];
    child->values[order - 1] = x->values[idx];

    for (int i = 0; i < sibling->numKeys; ++i) {
        child->keys[i + order] = sibling->keys[i];
        child->values[i + order] = sibling->values[i];
    }

    if (!child->isLeaf) {
        for (int i = 0; i <= sibling->numKeys; ++i)
            child->children[i + order] = sibling->children[i];
    }

    for (int i = idx + 1; i < x->numKeys; ++i) {
        x->keys[i - 1] = x->keys[i];
        x->values[i - 1] = x->values[i];
    }

    for (int i = idx + 2; i <= x->numKeys; ++i)
        x->children[i - 1] = x->children[i];

    child->numKeys += sibling->numKeys + 1;
    --x->numKeys;
    sibling.reset();
}

template<typename TKey, typename TElement>
BTree<TKey, TElement>::BTreeIterator::BTreeIterator(const BTree *tree)
        : tree(tree), hasCurrent(false) {
    Reset();
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::BTreeIterator::Reset() {
    stack = DynamicArraySmart<StackNode>();
    hasCurrent = false;
    if (tree->root) {
        PushLeftmost(tree->root);
    }
}

template<typename TKey, typename TElement>
void BTree<TKey, TElement>::BTreeIterator::PushLeftmost(ShrdPtr<Node> node) {
    while (node && node->numKeys > 0) {
        StackNode sn = {node, 0};
        stack.Append(sn);
        if (node->isLeaf)
            break;
        else
            node = node->children[0];
    }
}

template<typename TKey, typename TElement>
bool BTree<TKey, TElement>::BTreeIterator::MoveNext() {
    while (stack.GetLength() > 0) {
        StackNode &top = stack[stack.GetLength() - 1];

        if (top.index < top.node->numKeys) {
            if (top.node->numKeys == 0) {
                stack.RemoveAt(stack.GetLength() - 1);
                continue;
            }

            currentKey = top.node->keys[top.index];
            currentValue = top.node->values[top.index];
            hasCurrent = true;

            if (!top.node->isLeaf) {
                if (top.index + 1 <= top.node->numKeys) {
                    ShrdPtr<Node> child = top.node->children[top.index + 1];
                    ++top.index;
                    PushLeftmost(child);
                } else {
                    ++top.index;
                }
            } else {
                ++top.index;
            }

            return true;
        } else {
            stack.RemoveAt(stack.GetLength() - 1);
        }
    }

    hasCurrent = false;
    return false;
}


template<typename TKey, typename TElement>
TKey BTree<TKey, TElement>::BTreeIterator::GetCurrentKey() const {
    if (!hasCurrent)
        throw std::out_of_range("Iterator out of range");
    return currentKey;
}

template<typename TKey, typename TElement>
TElement BTree<TKey, TElement>::BTreeIterator::GetCurrentValue() const {
    if (!hasCurrent)
        throw std::out_of_range("Iterator out of range");
    return currentValue;
}


template<typename TKey, typename TElement>
UnqPtr<IDictionaryIterator<TKey, TElement>> BTree<TKey, TElement>::GetIterator() const {
    return UnqPtr<IDictionaryIterator<TKey, TElement>>(new BTreeIterator(this));
}

#endif // BTREE_H
