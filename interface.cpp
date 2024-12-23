#include "interface.h"

#include <iostream>
#include "DataStructures/SparseVector.h"
#include "DataStructures/SparseMatrix.h"
#include "DataStructures/HashTable.h"
#include "DataStructures/BTree.h"
#include "DataStructures/UnqPtr.h"

#include <iomanip>
#include <algorithm>
#include <cmath>
#include <string>

void displayMenu()
{
    int structureChoice = 0;
    int dictionaryChoice = 0;

    std::cout << "Select Structure:\n";
    std::cout << "1. Sparse Vector\n";
    std::cout << "2. Sparse Matrix\n";
    std::cin >> structureChoice;

    std::cout << "Select Dictionary:\n";
    std::cout << "1. HashTable\n";
    std::cout << "2. BTree\n";
    std::cin >> dictionaryChoice;

    if (structureChoice == 1) {
        int length = 30;
        UnqPtr<IDictionary<int, double>> dictionary;
        if (dictionaryChoice == 1) {
            dictionary = UnqPtr<IDictionary<int, double>>(new HashTable<int, double>());
        } else {
            dictionary = UnqPtr<IDictionary<int, double>>(new BTree<int, double>());
        }

        auto sparseVector = UnqPtr<SparseVector<double>>(new SparseVector<double>(length, std::move(dictionary)));
        handleVectorOperations(sparseVector);

    } else if (structureChoice == 2) {
        int rows = 5, cols = 5;
        UnqPtr<IDictionary<IndexPair, double>> dictionary;
        if (dictionaryChoice == 1) {
            dictionary = UnqPtr<IDictionary<IndexPair, double>>(new HashTable<IndexPair, double>());
        } else {
            dictionary = UnqPtr<IDictionary<IndexPair, double>>(new BTree<IndexPair, double>());
        }

        auto sparseMatrix = UnqPtr<SparseMatrix<double>>(new SparseMatrix<double>(rows, cols, std::move(dictionary)));
        handleMatrixOperations(sparseMatrix);
    }
}

void handleVectorOperations(UnqPtr<SparseVector<double>> &sparseVector) {
    while (true) {
        int choice;
        std::cout << "\nSparse Vector Operations:\n";
        std::cout << "1. Set Element\n";
        std::cout << "2. Get Element\n";
        std::cout << "3. Display Elements\n";
        std::cout << "4. Exit\n";
        std::cin >> choice;

        if (choice == 1) {
            int index;
            double value;
            std::cout << "Enter index (0-29): ";
            std::cin >> index;
            std::cout << "Enter value: ";
            std::cin >> value;

            try {
                sparseVector->SetElement(index, value);
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << "\n";
            }

        } else if (choice == 2) {
            int index;
            std::cout << "Enter index (0-29): ";
            std::cin >> index;

            try {
                double value = sparseVector->GetElement(index);
                std::cout << "Value at index " << index << ": " << value << "\n";
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << "\n";
            }

        } else if (choice == 3) {
            updateVectorDisplay(*sparseVector);
        } else if (choice == 4) {
            break;
        } else {
            std::cout << "Invalid choice. Try again.\n";
        }
    }
}

void handleMatrixOperations(UnqPtr<SparseMatrix<double>> &sparseMatrix) {
    while (true) {
        int choice;
        std::cout << "\nSparse Matrix Operations:\n";
        std::cout << "1. Set Element\n";
        std::cout << "2. Get Element\n";
        std::cout << "3. Display Elements\n";
        std::cout << "4. Exit\n";
        std::cin >> choice;

        if (choice == 1) {
            int row, col;
            double value;
            std::cout << "Enter row (0-4): ";
            std::cin >> row;
            std::cout << "Enter column (0-4): ";
            std::cin >> col;
            std::cout << "Enter value: ";
            std::cin >> value;

            try {
                sparseMatrix->SetElement(row, col, value);
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << "\n";
            }

        } else if (choice == 2) {
            int row, col;
            std::cout << "Enter row (0-4): ";
            std::cin >> row;
            std::cout << "Enter column (0-4): ";
            std::cin >> col;

            try {
                double value = sparseMatrix->GetElement(row, col);
                std::cout << "Value at (" << row << "," << col << "): " << value << "\n";
            } catch (const std::exception &e) {
                std::cerr << "Error: " << e.what() << "\n";
            }

        } else if (choice == 3) {
            updateMatrixDisplay(*sparseMatrix);
        } else if (choice == 4) {
            break;
        } else {
            std::cout << "Invalid choice. Try again.\n";
        }
    }
}

void updateVectorDisplay(const SparseVector<double> &sparseVector) {
    std::cout << "\nSparse Vector Elements:\n";
    for (int i = 0; i < sparseVector.GetLength(); ++i) {
        try {
            double value = sparseVector.GetElement(i);
            std::cout << "Index " << i << ": " << value << "\n";
        } catch (const std::exception &) {
            // Skip missing elements
        }
    }
}

void updateMatrixDisplay(const SparseMatrix<double> &sparseMatrix) {
    std::cout << "\nSparse Matrix Elements:\n";
    for (int i = 0; i < sparseMatrix.GetRows(); ++i) {
        for (int j = 0; j < sparseMatrix.GetColumns(); ++j) {
            try {
                double value = sparseMatrix.GetElement(i, j);
                std::cout << value << "\t";
            } catch (const std::exception &) {
                std::cout << "0\t";
            }
        }
        std::cout << "\n";
    }
}

void drawLine(std::vector<std::string>& canvas, int x1, int y1, int x2, int y2) {
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        if (x1 >= 0 && x1 < canvas[0].size() && y1 >= 0 && y1 < canvas.size() && canvas[y1][x1] != 'o') {
            canvas[y1][x1] = '*';
        }
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void plotGraph(const std::vector<Point>& points, int width, int height) {
    if (points.empty()) {
        std::cout << "No data for graphics.\n";
        return;
    }

    double x_min = points[0].first, x_max = points[0].first;
    double y_min = points[0].second, y_max = points[0].second;

    for (const auto& point : points) {
        x_min = std::min(x_min, point.first);
        x_max = std::max(x_max, point.first);
        y_min = std::min(y_min, point.second);
        y_max = std::max(y_max, point.second);
    }

    height += 2;
    width += 10;

    std::vector<std::string> canvas(height, std::string(width, ' '));

    std::vector<std::pair<int, int>> screenPoints;
    for (const auto& point : points) {
        int x = static_cast<int>((point.first - x_min) / (x_max - x_min) * (width - 11)) + 10;
        int y = static_cast<int>((point.second - y_min) / (y_max - y_min) * (height - 3));
        y = height - 3 - y;
        screenPoints.emplace_back(x, y);
        if (x >= 0 && x < width && y >= 0 && y < height) {
            canvas[y][x] = 'o';
        }
    }

    for (size_t i = 1; i < screenPoints.size(); ++i) {
        drawLine(canvas, screenPoints[i - 1].first, screenPoints[i - 1].second,
                 screenPoints[i].first, screenPoints[i].second);
    }

    int x_axis = height - 2;
    int y_axis = 10;

    for (int x = y_axis; x < width; ++x) {
        canvas[x_axis][x] = '-';
    }
    canvas[x_axis][y_axis] = '+';

    for (int y = 0; y < height - 2; ++y) {
        canvas[y][y_axis] = '|';
    }

    canvas[height - 1][y_axis] = '^';
    canvas[x_axis][width - 1] = '>';

    for (const auto& line : canvas) {
        std::cout << line << '\n';
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "X: time [" << x_min << ", " << x_max << "]\n";
    std::cout << "Y: algos [" << y_min << ", " << y_max << "]\n";
}