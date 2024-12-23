#pragma once

#include "DataStructures/SparseVector.h"
#include "DataStructures/SparseMatrix.h"
#include "DataStructures/UnqPtr.h"
using Point = std::pair<double, double>;

void handleVectorOperations(UnqPtr<SparseVector<double>> &sparseVector);
void handleMatrixOperations(UnqPtr<SparseMatrix<double>> &sparseMatrix);
void updateVectorDisplay(const SparseVector<double> &sparseVector);
void updateMatrixDisplay(const SparseMatrix<double> &sparseMatrix);

void displayMenu();

void drawLine(std::vector<std::string>& canvas, int x1, int y1, int x2, int y2);
void plotGraph(const std::vector<Point>& points, int width = 80, int height = 20);