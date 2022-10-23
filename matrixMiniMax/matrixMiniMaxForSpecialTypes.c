#include "./matrixMiniMaxForSpecialTypes.h"
#include "limits.h"
#include "omp.h"
#include "stdlib.h"
#include "../utils/utils.h"
#include "stdio.h"

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

static int findMiniMaxReduction(Matrix *matrix)
{
    int maxVal = INT_MIN;
#pragma omp parallel for shared(matrix) reduction(max \
                                                  : maxVal) schedule(runtime)
    for (int i = 0; i < matrix->nRows; i++)
    {
        int rowMin = GetMatrixElem(matrix, i, 0);
        for (int j = 1; j <= i; j++)
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

static void dotTestCycleForTriangularMatrix(int nRows, int nCols, FILE *file)
{

    Matrix *matrix = InitMatrix(nRows, nCols);
    FillLowerTriangularMatrixWithRandomValues(matrix);
    fprintf(file, "single;1;%0.15f\n", measure(findMiniMaxSingleThread, matrix));
    for (int i = 2; i < omp_get_max_threads() * 4; i++)
    {
        omp_set_schedule(0x1, 2);
        fprintf(file, "static;%d;%0.15f\n", i, measure(findMiniMaxReduction, matrix));
        omp_set_schedule(0x2, 2);
        fprintf(file, "dynamic;%d;%0.15f\n", i, measure(findMiniMaxReduction, matrix));
        omp_set_schedule(0x3, 2);
        fprintf(file, "guided;%d;%0.15f\n", i, measure(findMiniMaxReduction, matrix));
    }
    FreeMatrix(matrix);
}

int PerformMiniMaxSearchForSpecTypesComparison()
{
    FILE *f = fopen("../python_scripts/matrixMiniMaxForSpecTypes/output.csv", "w+");
    fprintf(f, "method;num_threads;elapsed_time\n");
    for (int i = 0; i < 30; i++)
    {
        dotTestCycleForTriangularMatrix(100, 100, f);
    }
    fclose(f);
    return 0;
}