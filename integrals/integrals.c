#include "integrals.h"
#include "stdio.h"
#include "malloc.h"
#include "omp.h"
#include "stdlib.h"
#include "math.h"

static double integrateInSingleThread(double (*function)(double x), double leftBorder, double rightBorder, int numRects)
{
    double result = 0;
    double h = (rightBorder - leftBorder) / numRects;
    for (int i = 0; i < numRects; i++)
    {
        result += function(leftBorder + h / 2 + i * h);
    }
    result *= h;
    return result;
}

static double integrateWithCriticalSection(double (*function)(double x), double leftBorder, double rightBorder, int numRects)
{
    double result = 0;
    double partialSum = 0;
    double h = (rightBorder - leftBorder) / numRects;

#pragma omp parallel shared(leftBorder, rightBorder, numRects, result, h, function) private(partialSum) default(none)
    {

        int threadNum = omp_get_thread_num();
        double rectsPerThread = numRects / omp_get_num_threads();
        if (threadNum == omp_get_num_threads() - 1)
        {
            rectsPerThread += numRects % omp_get_num_threads();
        }

        double threadLeftBorder = leftBorder + threadNum * h;

        for (int i = 0; i < rectsPerThread; i++)
        {
            partialSum += function(threadLeftBorder + h / 2 + i * h);
        }
#pragma omp critical
        {
            result += partialSum;
        }
    }
    return result * h;
}

static double integrateWithReduction(double (*function)(double x), double leftBorder, double rightBorder, int numRects)
{

    double result = 0;
    double h = (rightBorder - leftBorder) / numRects;
    int i = 0;
#pragma omp parallel for shared(leftBorder, h, numRects) private(i) reduction(+ \
                                                                              : result)
    for (i = 0; i < numRects; i++)
    {
        result += function(leftBorder + h / 2 + i * h);
    }

    return result * h;
}

static double measure(double (*method)(double (*function)(double x), double, double, int),
                      double (*function)(double x), double leftBorder, double rightBorder, int numRects)
{
    double start = omp_get_wtime();
    method(function, leftBorder, rightBorder, numRects);
    double end = omp_get_wtime();
    return (end - start) * 1000;
}

static void dotTestCycle(double (*function)(double x), double leftBorder, double rightBorder, int numRects, FILE *file)
{
    fprintf(file, "1;single;%d;%.20f\n", numRects, measure(integrateInSingleThread, function, leftBorder, rightBorder, numRects));
    const int maxNumThreads = omp_get_num_procs() * 4;
    for (int numThreads = 2; numThreads <= maxNumThreads; numThreads += 1)
    {
        omp_set_num_threads(numThreads);
        fprintf(file, "%d;critical_section;%d;%.20f\n", numThreads, numRects,
                measure(integrateWithCriticalSection, function, leftBorder, rightBorder, numRects));
        fprintf(file, "%d;reduction;%d;%.20f\n", numThreads, numRects,
                measure(integrateWithReduction, function, leftBorder, rightBorder, numRects));
    }
}

int PerformIntegralComputationComparison()
{
    FILE *f = fopen("../python_scripts/integrals/output.csv", "w+");
    fprintf(f, "num_threads;method;num_rects;elapsed_time\n");
    for (int i = 0; i < 30; i++)
    {

        dotTestCycle(exp, 0, 1, 100, f);
        dotTestCycle(exp, 0, 1, 10000, f);
        dotTestCycle(exp, 0, 1, 1000000, f);
    }

    fclose(f);
    return 0;
}