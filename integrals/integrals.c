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
        result += function(leftBorder + h / 2.0 + i * h);
    }
    return result * h;
}

static double integrateWithCriticalSection(double (*function)(double x), double leftBorder, double rightBorder, int numRects)
{
    double result = 0;
    double partialSum = 0;
    double h = (rightBorder - leftBorder) / numRects;

#pragma omp parallel shared(leftBorder, rightBorder, numRects, result, h, function) firstprivate(partialSum) default(none)
    {
        int numThreads = omp_get_num_threads();

        int threadNum = omp_get_thread_num();
        int rectsPerThread = numRects / numThreads;

        double threadLeftBorder = leftBorder + threadNum * rectsPerThread * h;
        if (threadNum == numThreads - 1)
        {
            rectsPerThread += numRects % numThreads;
        }

        for (int i = 0; i < rectsPerThread; i++)
        {
            partialSum += function(threadLeftBorder + h / 2.0 + i * h);
        }

#pragma omp critical
        {
            result += partialSum;
        }
    }
    return result * h;
}

static double integrateWithAtomic(double (*function)(double x), double leftBorder, double rightBorder, int numRects)
{
    double result = 0;
    double partialSum = 0;
    double h = (rightBorder - leftBorder) / numRects;

#pragma omp parallel shared(leftBorder, rightBorder, numRects, result, h, function) private(partialSum) default(none)
    {

        int numThreads = omp_get_num_threads();

        int threadNum = omp_get_thread_num();
        int rectsPerThread = numRects / numThreads;

        double threadLeftBorder = leftBorder + threadNum * rectsPerThread * h;
        if (threadNum == numThreads - 1)
        {
            rectsPerThread += numRects % numThreads;
        }
        for (int i = 0; i < rectsPerThread; i++)
        {
            partialSum += function(threadLeftBorder + h / 2.0 + i * h);
        }
#pragma omp atomic
        result += partialSum;
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
        result += function(leftBorder + h / 2.0 + i * h);
    }

    return result * h;
}

typedef struct MeasurmentResult
{
    double returnValue;
    double elapsedTime;
} MeasurmentResult;

static MeasurmentResult measure(double (*method)(double (*function)(double x), double, double, int),
                                double (*function)(double x), double leftBorder, double rightBorder, int numRects)
{
    double start = omp_get_wtime();
    double result = method(function, leftBorder, rightBorder, numRects);
    double end = omp_get_wtime();
    return (MeasurmentResult){.elapsedTime = (end - start) * 1000, .returnValue = result};
}

static void dotTestCycle(double (*function)(double x), double leftBorder, double rightBorder, int numRects, FILE *file, FILE *errPath)
{
    MeasurmentResult singleThreadResult = measure(integrateInSingleThread, function, leftBorder, rightBorder, numRects);
    fprintf(file, "1;single;%d;%.20f\n", numRects, singleThreadResult.elapsedTime);
    const int maxNumThreads = omp_get_num_procs() * 4;
    for (int numThreads = 2; numThreads <= maxNumThreads; numThreads += 1)
    {
        omp_set_num_threads(numThreads);

        MeasurmentResult critSecResult = measure(integrateWithCriticalSection, function, leftBorder, rightBorder, numRects);
        fprintf(file, "%d;critical_section;%d;%.20f\n", numThreads, numRects, critSecResult.elapsedTime);

        if (critSecResult.returnValue != singleThreadResult.returnValue)
        {
            fprintf(errPath, "threads = %d, numRects = %d, Expected criticalSection to return = %.15f, got = %.15f, diff = %.15f\n", numThreads, numRects, singleThreadResult.returnValue, critSecResult.returnValue,
                    singleThreadResult.returnValue - critSecResult.returnValue);
        }

        MeasurmentResult atomicResult = measure(integrateWithAtomic, function, leftBorder, rightBorder, numRects);
        fprintf(file, "%d;atomic;%d;%.20f\n", numThreads, numRects, atomicResult.elapsedTime);

        if (atomicResult.returnValue != singleThreadResult.returnValue)
        {
            fprintf(errPath, "threads = %d, Expected atomic to return = %.15f, got = %.15f, diff = %.15f\n", numThreads, singleThreadResult.returnValue, atomicResult.returnValue,
                    singleThreadResult.returnValue - atomicResult.returnValue);
        }

        MeasurmentResult reductionResult = measure(integrateWithReduction, function, leftBorder, rightBorder, numRects);
        fprintf(file, "%d;reduction;%d;%.20f\n", numThreads, numRects, reductionResult.elapsedTime);

        if (reductionResult.returnValue != singleThreadResult.returnValue)
        {
            fprintf(errPath, "threads = %d, numRects = %d, Expected reduction to return = %.15f, got = %.15f, diff = %.15f\n", numThreads, numRects, singleThreadResult.returnValue, reductionResult.returnValue,
                    singleThreadResult.returnValue - reductionResult.returnValue);
        }
    }
}

int PerformIntegralComputationComparison()
{
    FILE *f = fopen("../python_scripts/integrals/output.csv", "w+");
    FILE *errPath = fopen("errPath.txt", "w+");
    fprintf(f, "num_threads;method;num_rects;elapsed_time\n");
    for (int i = 0; i < 30; i++)
    {
        dotTestCycle(exp, 0, 100, 100, f, errPath);
        dotTestCycle(exp, 0, 100, 10000, f, errPath);
        dotTestCycle(exp, 0, 100, 1000000, f, errPath);
    }
    fclose(errPath);
    fclose(f);
    return 0;
}