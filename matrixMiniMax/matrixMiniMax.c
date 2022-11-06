#include "matrixMiniMax.h"
#include "limits.h"
#include "malloc.h"
#include "omp.h"
#include "../utils/utils.h"

static int findMiniMaxSingleThread(Matrix *matrix)
{
    int maxVal = INT_MIN;

    for (int i = 0; i < matrix->nRows; i++)
    {
        int rowMin = GetMatrixElem(matrix, i, 0);
        for (int j = 1; j < matrix->nCols; j++)
        {
            int curr = GetMatrixElem(matrix, i, j);
            if (curr < rowMin)
            {
                rowMin = curr;
            }
        }
        if (rowMin > maxVal)
        {
            maxVal = rowMin;
        }
    }
    return maxVal;
}

static int findMiniMaxCriticalSection(Matrix *matrix)
{
    int maxVal = INT_MIN;
#pragma omp parallel for shared(matrix, maxVal)
    for (int i = 0; i < matrix->nRows; i++)
    {
        int rowMin = GetMatrixElem(matrix, i, 0);
        for (int j = 1; j < matrix->nCols; j++)
        {
            int curr = GetMatrixElem(matrix, i, j);
            if (curr < rowMin)
            {
                rowMin = curr;
            }
        }

        if (rowMin > maxVal)
        {
#pragma omp critical
            {
                if (rowMin > maxVal)
                {
                    maxVal = rowMin;
                }
            }
        }
    }
    return maxVal;
}

static int findMiniMaxReduction(Matrix *matrix)
{
    int maxVal = INT_MIN;
#pragma omp parallel for shared(matrix) reduction(max \
                                                  : maxVal)
    for (int i = 0; i < matrix->nRows; i++)
    {
        int rowMin = GetMatrixElem(matrix, i, 0);
        for (int j = 1; j < matrix->nCols; j++)
        {
            int curr = GetMatrixElem(matrix, i, j);
            if (curr < rowMin)
            {
                rowMin = curr;
            }
        }

        if (rowMin > maxVal)
        {
            maxVal = rowMin;
        }
    }

    return maxVal;
}

static double measure(int (*method)(Matrix *), Matrix *matrix)
{
    double start = omp_get_wtime();
    method(matrix);
    double end = omp_get_wtime();
    return (end - start) * 1000;
}

static void dotTestCycle(int nRows, int nCols, FILE *file)
{

    Matrix *matrix = InitMatrix(nRows, nCols);
    FillMatrixWithRandomValues(matrix);

    fprintf(file, "1;single;%d;%d;%.20f\n", matrix->nRows, matrix->nCols, measure(findMiniMaxSingleThread, matrix));
    const int maxNumThreads = omp_get_num_procs() * 4;
    for (int numThreads = 2; numThreads <= maxNumThreads; numThreads += 1)
    {
        omp_set_num_threads(numThreads);
        fprintf(file, "%d;critical_section;%d;%d;%.20f\n", numThreads,
                matrix->nRows, matrix->nCols, measure(findMiniMaxCriticalSection, matrix));
        fprintf(file, "%d;reduction;%d;%d;%.20f\n", numThreads,
                matrix->nRows, matrix->nCols, measure(findMiniMaxReduction, matrix));
    }
}

static void printTestResult()
{
    int nRows = 3;
    int nCols = 3;

    Matrix *matrix = InitMatrix(nRows, nCols);
    for (int i = 0; i < nRows * nCols; i++)
    {
        SetMatrixElem(matrix, i / nCols, i % nCols, i);
    }

    printf("%d\n", findMiniMaxSingleThread(matrix));
    printf("%d\n", findMiniMaxCriticalSection(matrix));
    printf("%d\n", findMiniMaxReduction(matrix));
}

int PerformMiniMaxSearchComparison()
{
    FILE *f = fopen("../python_scripts/matrixMiniMax/output.csv", "w+");
    fprintf(f, "num_threads;method;n_rows;n_cols;elapsed_time\n");

    for (int i = 0; i < 30; i++)
    {
        dotTestCycle(10, 10, f);
        dotTestCycle(100, 100, f);
        dotTestCycle(1000, 1000, f);
    }

    fclose(f);
}
