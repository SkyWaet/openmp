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
    int sum = 0;
    int i;
    int prod;
#pragma omp parallel for shared(a, b, sizeA, sum) private(i, prod)
    for (i = 0; i < sizeA; i++)
    {
        prod = a[i] * b[i];
#pragma omp critical
        {
            sum += prod;
        }
    }

    return sum;
}

int dotProductWithAtomic(int *a, int *b, int sizeA, int sizeB)
{
    if (sizeA != sizeB)
    {
        exit(-123);
    }
    int sum = 0;
    int i;
    int prod = 0;
#pragma omp parallel for shared(a, b, sizeA, sum) private(i, prod)
    for (i = 0; i < sizeA; i++)
    {
        prod = a[i] * b[i];
#pragma omp atomic
        sum += prod;
    }
    return sum;
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

    fprintf(file, "single;%d;%.20f\n", arraySize,
            measureDotProduct(dotProductSingleThread, firstArray, secondArray, arraySize, arraySize));
    fprintf(file, "critical_section;%d;%.20f\n", arraySize,
            measureDotProduct(dotProductWithCriticalSection, firstArray, secondArray, arraySize, arraySize));
    fprintf(file, "atomic;%d;%.20f\n", arraySize,
            measureDotProduct(dotProductWithAtomic, firstArray, secondArray, arraySize, arraySize));
    fprintf(file, "reduction;%d;%.20f\n", arraySize,
            measureDotProduct(dotProductWithReduction, firstArray, secondArray, arraySize, arraySize));

    free(firstArray);
    free(secondArray);
}

int PerformDotProductComparison()
{
    FILE *f = fopen("../python_scripts/dotProduct/output.csv", "w+");
    fprintf(f, "method;array_size;elapsed_time\n");
    for (int i = 0; i < 30; i++)
    {
        doDotProductTestCycle(100, f);
        doDotProductTestCycle(10000, f);
        doDotProductTestCycle(10000000, f);
    }
    fclose(f);
    return 0;
}