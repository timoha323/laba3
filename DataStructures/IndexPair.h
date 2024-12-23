#ifndef INDEXPAIR_H
#define INDEXPAIR_H
#include <iostream>

struct IndexPair {
    int row;
    int column;

    IndexPair() : row(0), column(0) {};

    IndexPair(int r, int c) : row(r), column(c) {}

    bool operator==(const IndexPair& other) const {
        return row == other.row && column == other.column;
    }

    bool operator<(const IndexPair& other) const {
        if (row != other.row)
            return row < other.row;
        return column < other.column;
    }

    bool operator>(const IndexPair& other) const {
        if (row != other.row)
            return row > other.row;
        return column > other.column;
    }

};

inline std::ostream& operator<<(std::ostream& os, const IndexPair& ip) {
    os << "(" << ip.row << ", " << ip.column << ")";
    return os;
}

struct IndexPairHash {
    std::size_t operator()(const IndexPair& k) const {
        std::size_t row_hash = static_cast<std::size_t>(k.row) * 73856093;
        std::size_t col_hash = static_cast<std::size_t>(k.column) * 19349663;

        return row_hash ^ (col_hash * 2654435761);
    }
};


#endif // INDEXPAIR_H
