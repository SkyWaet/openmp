
#include "differentCycleModes.h"
#include "../utils/utils.h"
#include "limits.h"
#include "omp.h"
#include "math.h"

static int testIteration(const int number)
{
    if (number % 10 == 0)
    {
        return GetRandomInteger(-1000, 1000);
    }

    int sum = 0;
    for (int i = 0; i < number % 100; i++)
    {
        sum = (sum + i) % 100;
    }
    return sum;
}

static int plainForLoop(int numIterations)
{
    int sumMod = 0;
    for (int i = 0; i < numIterations; i++)
    {
        sumMod += testIteration(i) % 100;
    }
    return sumMod;
}

static int staticScheduledForLoop(int numIterations)
{
    int sumMod = 0;
#pragma omp parallel for shared(numIterations) schedule(static) reduction(+ \
                                                                          : sumMod)
    for (int i = 0; i < numIterations; i++)
    {
        sumMod += testIteration(i) % 100;
    }
    return sumMod;
}

static int dynamicScheduledForLoop(int numIterations)
{
    int sumMod = 0;
#pragma omp parallel for shared(numIterations) schedule(dynamic, 8) reduction(+ \
                                                                              : sumMod)

    for (int i = 0; i < numIterations; i++)
    {
        sumMod += testIteration(i) % 100;
    }
    return sumMod;
}

static int guidedScheduledForLoop(int numIterations)
{
    int sumMod = 0;
#pragma omp parallel for shared(numIterations) schedule(guided) reduction(+ \
                                                                          : sumMod)
    for (int i = 0; i < numIterations; i++)
    {
        sumMod += testIteration(i) % 100;
    }
    return sumMod;
}

static double measure(int (*method)(int), int numIterations)
{
    int res;
    double start = omp_get_wtime();
    res = method(numIterations);
    double end = omp_get_wtime();
    printf("%d", res);
    return (end - start) * 1000;
}

static void dotTestCycle(int numIterations, FILE *file)
{

    fprintf(file, "1;single;%d;%.20f\n", numIterations, measure(plainForLoop, numIterations));
    const int maxNumThreads = omp_get_num_procs() * 2;
    for (int numThreads = 2; numThreads <= maxNumThreads; numThreads += 1)
    {
        omp_set_num_threads(numThreads);
        fprintf(file, "%d;static;%d;%.20f\n", numThreads,
                numIterations, measure(staticScheduledForLoop, numIterations));
        fprintf(file, "%d;dynamic;%d;%.20f\n", numThreads,
                numIterations, measure(dynamicScheduledForLoop, numIterations));
        fprintf(file, "%d;guided;%d;%.20f\n", numThreads,
                numIterations, measure(guidedScheduledForLoop, numIterations));
    }
}

int PerformDifferentCycleModesComparison()
{
    FILE *f = fopen("../python_scripts/differentCycleModes/output.csv", "w+");
    fprintf(f, "num_threads;method;num_iterations;elapsed_time\n");

    for (int i = 0; i < 15; i++)
    {
        dotTestCycle(100, f);
        dotTestCycle(10000, f);
        dotTestCycle(1000000, f);
    }
    fclose(f);
    return 0;
}