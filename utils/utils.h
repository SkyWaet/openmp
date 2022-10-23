//
// Created by GSlepenkov on 26.09.2022.
//

#include "../datatypes/matrix.h"
#include "stdlib.h"
#include "stdio.h"

#ifndef OPENMP_UTILS_H
#define OPENMP_UTILS_H

#endif // OPENMP_UTILS_H

int GetRandomInteger(int lower, int upper);

void FillWithRandomValues(int size, int *vector);

Matrix *InitializeArrays(int size);

void FillMatrixWithRandomValues(Matrix *matrix);

void FillLowerTriangularMatrixWithRandomValues(Matrix *matrix);

void PrintArray(int size, int *vector);

void PrintMatrix(Matrix *matrix);
