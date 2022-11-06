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

void doFindMinTestCycle(int matrixSize, FILE *file, int maxNumThreads)
{
    Matrix *matrix = InitializeArrays(matrixSize);
    fprintf(file, "%d;single;%d;%.20f\n", 1, matrixSize,
            measureFindMin(FindMinSingleThread, matrix->data, matrix->nCols));
    for (int numThreads = 2; numThreads <= maxNumThreads; numThreads += 1)
    {
        omp_set_num_threads(numThreads);
        fprintf(file, "%d;critical_section;%d;%.20f\n", numThreads, matrixSize,
                measureFindMin(FindMinWithForLoopParallelism, matrix->data, matrix->nCols));
        fprintf(file, "%d;reduction;%d;%.20f\n", numThreads, matrixSize,
                measureFindMin(FindMinWithReduction, matrix->data, matrix->nCols));
    }

    FreeMatrix(matrix);
}

int PerformFindMinComparison()
{
    FILE *f = fopen("../python_scripts/vectorMinValue/output.csv", "w+");
    fprintf(f, "num_threads;method;array_size;elapsed_time\n");
    const int maxNumThreads = omp_get_num_procs() * 4;
    for (int i = 0; i < 30; i++)
    {
        doFindMinTestCycle(100, f, maxNumThreads);
        doFindMinTestCycle(100000, f, maxNumThreads);
        doFindMinTestCycle(100000000, f, maxNumThreads);
    }

    fclose(f);
    return 0;
}