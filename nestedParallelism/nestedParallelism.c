#include "nestedParallelism.h"
#include "../utils/utils.h"
#include "limits.h"
#include "omp.h"
#include "math.h"

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

static int findMiniMaxReductionNested(Matrix *matrix)
{
    int maxVal = INT_MIN;
    int outerLoopNumThreads = omp_get_num_threads() / 2;
    int innerLoopNumThreads = omp_get_num_threads() - outerLoopNumThreads;
    if (innerLoopNumThreads <= 0)
    {
        innerLoopNumThreads = 1;
    }
#pragma omp parallel for shared(matrix) num_threads(outerLoopNumThreads) reduction(max \
                                                                                   : maxVal)
    for (int i = 0; i < matrix->nRows; i++)
    {
        int rowMin = GetMatrixElem(matrix, i, 0);
#pragma omp parallel for shared(matrix) num_threads(innerLoopNumThreads) reduction(min \
                                                                                   : rowMin)
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
    int value;
    double start = omp_get_wtime();
    value = method(matrix);
    double end = omp_get_wtime();
    printf("%d", value);
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
        fprintf(file, "%d;reduction;%d;%d;%.20f\n", numThreads,
                matrix->nRows, matrix->nCols, measure(findMiniMaxReduction, matrix));
        omp_set_nested(1);
        fprintf(file, "%d;nested;%d;%d;%.20f\n", numThreads,
                matrix->nRows, matrix->nCols, measure(findMiniMaxReductionNested, matrix));
        omp_set_nested(0);
    }
    FreeMatrix(matrix);
}

int performTest()
{
    Matrix *matrix = InitMatrix(100, 100);
    FillMatrixWithRandomValues(matrix);
    int single = findMiniMaxSingleThread(matrix);
    int reduction = findMiniMaxReduction(matrix);
    int nested = findMiniMaxReductionNested(matrix);

    printf("single = %d\n", single);
    printf("reduction = %d\n", reduction);
    printf("nested = %d\n", nested);

    printf("single - reduction = %d\n", abs(single - reduction));
    printf("single - nested = %d\n", abs(single - nested));

    FreeMatrix(matrix);
}

int PerformNestedParallelismComparison()
{
    FILE *f = fopen("../python_scripts/nestedParallelism/output.csv", "w+");
    fprintf(f, "num_threads;method;n_rows;n_cols;elapsed_time\n");

    for (int i = 0; i < 30; i++)
    {
        dotTestCycle(10, 10, f);
        dotTestCycle(100, 100, f);
        dotTestCycle(1000, 1000, f);
    }

    fclose(f);
    return 0;
}