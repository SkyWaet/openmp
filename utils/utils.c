//
// Created by GSlepenkov on 26.09.2022.
//

#include <stdlib.h>
#include "utils.h"
#include "stdio.h"

int GetRandomInteger(int lower, int upper)
{
    return rand() % (upper - lower + 1) + lower;
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
};
