#include "dotProduct.h"
#include "stdio.h"
#include "malloc.h"
#include "omp.h"
#include "stdlib.h"

int dotProductSingleThread(int *a, int *b, int sizeA, int sizeB)
{
    if (sizeA != sizeB)
    {
        exit(-123);
    }
    int sum = 0;
    for (int i = 0; i < sizeA; i++)
    {
        sum += a[i] * b[i];
    }
    return sum;
}

int dotProductWithCriticalSection(int *a, int *b, int sizeA, int sizeB)
{
    if (sizeA != sizeB)
    {
        exit(-123);
    }
    int total = 0;
    int i;
    int chunkSize;
    int sum;
    int start;
    int end;
#pragma omp parallel shared(a, b, sizeA, chunkSize, total) private(i, sum, start, end)
    {
        chunkSize = sizeA / omp_get_num_threads();
        start = omp_get_thread_num() * chunkSize;
        end = omp_get_thread_num() == omp_get_num_threads() - 1
                  ? sizeA
                  : start + chunkSize;
        for (i = start; i < end; i++)
        {
            sum += a[i] * b[i];
        }
#pragma omp critical
        {
            total += sum;
        }
    }

    return total;
}

int dotProductWithAtomic(int *a, int *b, int sizeA, int sizeB)
{
    if (sizeA != sizeB)
    {
        exit(-123);
    }
    int total = 0;
    int i;
    int chunkSize;
    int sum;
    int start;
    int end;
#pragma omp parallel shared(a, b, sizeA, chunkSize, total) private(i, sum, start, end)
    {
        chunkSize = sizeA / omp_get_num_threads();
        start = omp_get_thread_num() * chunkSize;
        end = omp_get_thread_num() == omp_get_num_threads() - 1
                  ? sizeA
                  : start + chunkSize;
        for (i = start; i < end; i++)
        {
            sum += a[i] * b[i];
        }
#pragma omp atomic
        total += sum;
    }

    return total;
}

int dotProductWithReduction(int *a, int *b, int sizeA, int sizeB)
{
    if (sizeA != sizeB)
    {
        exit(-123);
    }
    int sum = 0;
    int i;
#pragma omp parallel for shared(a, b, sizeA) reduction(+ \
                                                       : sum) default(none)

    for (i = 0; i < sizeA; i++)
    {
        sum += a[i] * b[i];
    }
    return sum;
}

double measureDotProduct(int (*method)(int *, int *, int, int), int *vectorA, int *vectorB, int sizeA, int sizeB)
{
    double start = omp_get_wtime();
    method(vectorA, vectorB, sizeA, sizeB);
    double end = omp_get_wtime();
    return (end - start) * 1000;
}

void doDotProductTestCycle(int arraySize, FILE *file)
{
    int *firstArray = malloc(sizeof(int) * arraySize);
    int *secondArray = malloc(sizeof(int) * arraySize);
    FillWithRandomValues(arraySize, firstArray);
    FillWithRandomValues(arraySize, secondArray);

    fprintf(file, "1;single;%d;%.20f\n", arraySize,
            measureDotProduct(dotProductSingleThread, firstArray, secondArray, arraySize, arraySize));
    const int maxNumThreads = omp_get_num_procs() * 4;
    for (int numThreads = 2; numThreads <= maxNumThreads; numThreads += 1)
    {
        omp_set_num_threads(numThreads);
        fprintf(file, "%d;critical_section;%d;%.20f\n", numThreads, arraySize,
                measureDotProduct(dotProductWithCriticalSection, firstArray, secondArray, arraySize, arraySize));
        fprintf(file, "%d;atomic;%d;%.20f\n", numThreads, arraySize,
                measureDotProduct(dotProductWithAtomic, firstArray, secondArray, arraySize, arraySize));
        fprintf(file, "%d;reduction;%d;%.20f\n", numThreads, arraySize,
                measureDotProduct(dotProductWithReduction, firstArray, secondArray, arraySize, arraySize));
    }

    free(firstArray);
    free(secondArray);
}

int PerformDotProductComparison()
{
    FILE *f = fopen("../python_scripts/dotProduct/output.csv", "w+");
    fprintf(f, "num_threads;method;array_size;elapsed_time\n");
    for (int i = 0; i < 30; i++)
    {
        doDotProductTestCycle(100, f);
        doDotProductTestCycle(100000, f);
        doDotProductTestCycle(100000000, f);
    }
    fclose(f);
    return 0;
}