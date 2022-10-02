#include "vectorMinValue.h"
#include "limits.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../utils/utils.h"
#include "omp.h"

int FindMinSingleThread(int *vector, int size)
{
    int *end = vector + size;
    int min = INT_MAX;
    for (int *start = vector; start < end; start++)
    {
        if (min > *start)
        {
            min = *start;
        }
    }
    return min;
}

int FindMinWithForLoopParallelism(int *vector, int size)
{
    int *end = vector + size;
    int min = INT_MAX;
#pragma omp parallel for shared(vector, end, min) default(none)
    for (int *start = vector; start < end; start++)
    {
        if (min > *start)
        {
#pragma omp critical
            if (min > *start)
            {
                min = *start;
            }
        }
    }
    return min;
}

int FindMinWithReduction(int *vector, int size)
{
    int minValue;
    int start;
#pragma omp parallel for shared(vector, size) private(start) reduction(min \
                                                                       : minValue) default(none)
    for (start = 0; start < size; start++)
    {
        minValue = vector[start];
    }
    return minValue;
}

void fillWithRandomValues(int size, int *vector)
{
    int *end = vector + size;
    for (int *start = vector; start < end; start++)
    {
        *start = GetRandomInteger(0, size);
    }
}

Matrix *initializeArrays(int size)
{
    Matrix *matrix = InitMatrix(1, size);
    fillWithRandomValues(size, matrix->data);
    return matrix;
}

double measure(int (*method)(int *, int), int *array, int size)
{
    double start = omp_get_wtime();
    double end = omp_get_wtime();
    method(array, size);
    return (end - start) * 100000;
}

void doTestCycle(int matrixSize, FILE *file)
{
    Matrix *matrix = initializeArrays(matrixSize);
    for (int i = 0; i < matrix->nRows; i++)
    {
        fprintf(file, "single;%d;%.20f\n", matrixSize,
                measure(FindMinSingleThread, matrix->data + i * matrix->nCols, matrix->nCols));
        fprintf(file, "critical_section;%d;%.20f\n", matrixSize,
                measure(FindMinWithForLoopParallelism, matrix->data + i * matrix->nCols, matrix->nCols));
        fprintf(file, "reduction;%d;%.20f\n", matrixSize,
                measure(FindMinWithReduction, matrix->data + i * matrix->nCols, matrix->nCols));
    }
    FreeMatrix(matrix);
}

int PerformTaskOne()
{
    FILE *f = fopen("output.csv", "w+");
    fprintf(f, "method;array_size;elapsed_time\n");
    srand(time(NULL));
    for (int i = 0; i < 50; i++)
    {
        doTestCycle(10000, f);
        doTestCycle(10000000, f);
        doTestCycle(100000000, f);
    }
    fclose(f);
    return 0;
}