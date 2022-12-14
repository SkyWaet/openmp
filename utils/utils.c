//
// Created by GSlepenkov on 26.09.2022.
//

#include "utils.h"

int GetRandomInteger(int lower, int upper)
{
    return rand() % (upper - lower + 1) + lower;
}

void FillWithRandomValues(int size, int *vector)
{
    int *end = vector + size;
    for (int *start = vector; start < end; start++)
    {
        *start = GetRandomInteger(-size, size);
    }
}

Matrix *InitializeArrays(int size)
{
    Matrix *matrix = InitMatrix(1, size);
    FillWithRandomValues(size, matrix->data);
    return matrix;
}

void FillMatrixWithRandomValues(Matrix *matrix)
{
    for (int i = 0; i < matrix->nRows; i++)
    {
        for (int j = 0; j < matrix->nCols; j++)
        {
            SetMatrixElem(matrix, i, j, GetRandomInteger(-1000, 1000));
        }
    }
}

void FillLowerTriangularMatrixWithRandomValues(Matrix *matrix)
{
    for (int i = 0; i < matrix->nRows; i++)
    {
        for (int j = 0; j <= i; j++)
        {
            SetMatrixElem(matrix, i, j, GetRandomInteger(-1000, 1000));
        }
    }
}

void PrintArray(int size, int *vector)
{
    int *end = vector + size;
    for (int *start = vector; start < end; start++)
    {
        printf("%d ", *start);
    }
}

void PrintMatrix(Matrix *matrix)
{
    for (int i = 0; i < matrix->nRows; i++)
    {
        PrintArray(matrix->nCols, matrix->data + i * matrix->nCols);
        if (i < matrix->nRows - 1)
        {
            printf("\n");
        }
    }
    printf("\n");
};
