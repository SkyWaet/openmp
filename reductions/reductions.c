#include "./reductions.h"
#include "omp.h"
#include "malloc.h"
#include "../utils/utils.h"

static int arraySumReductionBuiltin(int *array, int length)
{
    int sum = 0;
    int i;
#pragma omp parallel for shared(array, length) private(i) reduction(+ \
                                                                    : sum)
    for (i = 0; i < length; i++)
    {
        sum += array[i];
    }
    return sum;
}

static int arraySumReductionCritical(int *array, int length)
{
    int total = 0;
    int i;
    int chunkSize;
    int start;
    int end;
#pragma omp parallel shared(array, length, chunkSize, total) private(i, start, end)
    {
        int sum = 0;
        chunkSize = length / omp_get_num_threads();
        start = omp_get_thread_num() * chunkSize;
        end = omp_get_thread_num() == omp_get_num_threads() - 1
                  ? length
                  : start + chunkSize;
        for (i = start; i < end; i++)
        {
            sum += array[i];
        }
#pragma omp critical
        {
            total += sum;
        }
    }
    return total;
}

static int arraySumReductionAtomics(int *array, int length)
{
    int total = 0;
    int i;
    int chunkSize;
    int start;
    int end;
#pragma omp parallel shared(array, length, chunkSize, total) private(i, start, end) default(none)
    {
        int sum = 0;
        chunkSize = length / omp_get_num_threads();
        start = omp_get_thread_num() * chunkSize;
        end = omp_get_thread_num() == omp_get_num_threads() - 1
                  ? length
                  : start + chunkSize;
        for (i = start; i < end; i++)
        {
            sum += array[i];
        }
#pragma omp atomic
        total += sum;
    }
    return total;
}

static int arraySumReductionLocks(int *array, int length)
{
    int total = 0;
    int i;
    int chunkSize;
    int start;
    int end;
    omp_lock_t lock;
    omp_init_lock(&lock);
#pragma omp parallel shared(array, length, chunkSize, total) private(i, start, end)
    {
        int sum = 0;
        chunkSize = length / omp_get_num_threads();
        start = omp_get_thread_num() * chunkSize;
        end = omp_get_thread_num() == omp_get_num_threads() - 1
                  ? length
                  : start + chunkSize;
        for (i = start; i < end; i++)
        {
            sum += array[i];
        }
        omp_set_lock(&lock);
        total += sum;
        omp_unset_lock(&lock);
    }
    return total;
    omp_destroy_lock(&lock);
}

static void performTest()
{
    int arrSize = 100;
    int *testArray = malloc(sizeof(int) * arrSize);
    FillWithRandomValues(arrSize, testArray);

    int reduction = arraySumReductionBuiltin(testArray, arrSize);
    int critical = arraySumReductionCritical(testArray, arrSize);
    int atomic = arraySumReductionAtomics(testArray, arrSize);
    int lock = arraySumReductionLocks(testArray, arrSize);

    printf("red = %d\n", reduction);
    printf("crit = %d\n", critical);
    printf("atomic = %d\n", atomic);
    printf("lock = %d\n", lock);

    printf("red - crit = %d\n", reduction - critical);
    printf("red - atomic = %d\n", reduction - atomic);
    printf("red - lock = %d\n", reduction - lock);
}

static double measure(int (*method)(int *, int), int *array, int length)
{
    double start = omp_get_wtime();
    method(array, length);
    double end = omp_get_wtime();
    return (end - start) * 1000;
}

static void doTestCycle(int length, FILE *file)
{
    int *testArray = malloc(sizeof(int) * length);
    FillWithRandomValues(length, testArray);
    const int maxThreads = omp_get_num_procs();

    for (int numThreads = 2; numThreads < maxThreads * 2; numThreads++)
    {
        omp_set_num_threads(numThreads);
        fprintf(file, "builtin;%d;%d;%0.15f\n", numThreads, length, measure(arraySumReductionBuiltin, testArray, length));
        fprintf(file, "critical;%d;%d;%0.15f\n", numThreads, length, measure(arraySumReductionCritical, testArray, length));
        fprintf(file, "atomics;%d;%d;%0.15f\n", numThreads, length, measure(arraySumReductionAtomics, testArray, length));
        fprintf(file, "locks;%d;%d;%0.15f\n", numThreads, length, measure(arraySumReductionLocks, testArray, length));
    }
    free(testArray);
}
int performReductionsComparison()
{
    FILE *file = fopen("../python_scripts/reductions/output.csv", "w+");
    fprintf(file, "method;num_threads;length;elapsed_time\n");

    for (int i = 0; i < 30; i++)
    {
        doTestCycle(100, file);
        doTestCycle(10000, file);
        doTestCycle(1000000, file);
    }
    fclose(file);
    return 0;
}