// SparseMatrix.h
#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H

#include "IDictionary.h"
#include "IndexPair.h"
#include "ShrdPtr.h"
#include "DynamicArraySmart.h"
#include "KeyValue.h"
#include <vector>

template<typename TElement>
class SparseMatrix {
public:
    SparseMatrix(int rows, int columns, UnqPtr<IDictionary<IndexPair, TElement>> dictionary)
            : rows(rows), columns(columns), elements(std::move(dictionary)) {}

    ~SparseMatrix(){}

    int GetRows() const
    {
        return rows;
    }

    int GetColumns() const
    {
        return columns;
    }

    TElement GetElement(int row, int column) const
    {
        if (row < 0 || row >= rows || column < 0 || column >= columns)
        {
            throw std::out_of_range("Row or column index is out of bounds.");
        }

        IndexPair key(row, column);
        if (elements->ContainsKey(key))
        {
            return elements->Get(key);
        }
        else
        {
            return TElement();
        }
    }

    void SetElement(int row, int column, const TElement& value)
    {
        if (row < 0 || row >= rows || column < 0 || column >= columns)
        {
            throw std::out_of_range("Row or column index is out of bounds.");
        }

        IndexPair key(row, column);
        if (value != TElement())
        {
            if (elements->ContainsKey(key))
            {
                elements->Update(key, value);
            }
            else
            {
                elements->Add(key, value);
            }
        }
        else
        {
            RemoveElement(key.row, key.column);
        }
    }

    void RemoveElement(int row, int column) {
        if (row < 0 || row >= rows || column < 0 || column >= columns) {
            throw std::out_of_range("Row or column index is out of bounds.");
        }

        IndexPair key(row, column);
        if (elements->ContainsKey(key)) {
            elements->Remove(key);
        }
    }

    void ForEach(void (*func)(const IndexPair &, const TElement &)) const {
        auto iterator = elements->GetIterator();

        while (iterator->MoveNext()) {
            IndexPair key = iterator->GetCurrentKey();
            TElement value = iterator->GetCurrentValue();
            func(key, value);
        }
    }



    void Map(TElement (*func)(TElement))
    {
        DynamicArraySmart<KeyValue<IndexPair, TElement>> updates;
        auto iterator = elements->GetIterator();
        while (iterator->MoveNext())
        {
            IndexPair key = iterator->GetCurrentKey();
            TElement value = iterator->GetCurrentValue();
            TElement newValue = func(value);
            updates.Append(KeyValue<IndexPair, TElement>(key, newValue));
        }
        for (int i = 0; i < updates.GetLength(); ++i)
        {
            const KeyValue<IndexPair, TElement>& kv = updates.Get(i);
            elements->Update(kv.key, kv.value);
        }
    }


    TElement Reduce(TElement (*func)(TElement, TElement), TElement initial) const
    {
        TElement result = initial;
        auto iterator = elements->GetIterator();
        while (iterator->MoveNext())
        {
            TElement value = iterator->GetCurrentValue();
            result = func(result, value);
        }
        return result;
    }

    UnqPtr<IDictionaryIterator<IndexPair, TElement>> GetIterator() const
    {
        return elements->GetIterator();
    }

    const IDictionary<IndexPair, TElement>& GetElements() const {
        return *elements;
    }

private:
    int rows;
    int columns;
    UnqPtr<IDictionary<IndexPair, TElement>> elements;
};

#endif // SPARSEMATRIX_H
