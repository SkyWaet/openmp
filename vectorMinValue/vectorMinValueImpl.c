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

double measureFindMin(int (*method)(int *, int), int *array, int size)
{
    double start = omp_get_wtime();
    method(array, size);
    double end = omp_get_wtime();
    return (end - start) * 1000;
}

void doFindMinTestCycle(int matrixSize, FILE *file)
{
    Matrix *matrix = InitializeArrays(matrixSize);
    for (int i = 0; i < matrix->nRows; i++)
    {
        fprintf(file, "single;%d;%.20f\n", matrixSize,
                measureFindMin(FindMinSingleThread, matrix->data + i * matrix->nCols, matrix->nCols));
        fprintf(file, "critical_section;%d;%.20f\n", matrixSize,
                measureFindMin(FindMinWithForLoopParallelism, matrix->data + i * matrix->nCols, matrix->nCols));
        fprintf(file, "reduction;%d;%.20f\n", matrixSize,
                measureFindMin(FindMinWithReduction, matrix->data + i * matrix->nCols, matrix->nCols));
    }
    FreeMatrix(matrix);
}

int PerformFindMinComparison()
{
    FILE *f = fopen("../python_scripts/vectorMinValue/output.csv", "w+");
    fprintf(f, "method;array_size;elapsed_time\n");
    for (int i = 0; i < 30; i++)
    {
        doFindMinTestCycle(10000, f);
        doFindMinTestCycle(10000000, f);
        doFindMinTestCycle(100000000, f);
    }
    fclose(f);
    return 0;
}