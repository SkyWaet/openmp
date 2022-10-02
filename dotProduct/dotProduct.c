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

double measureDotProduct(int (*method)(int *, int *, int, int), int *vectorA, int *vectorB, int sizeA, int sizeB)
{
    double start = omp_get_wtime();
    method(vectorA, vectorB, sizeA, sizeB);
    double end = omp_get_wtime();
    return (end - start) * 100000;
}

void doDotProductTestCycle(int arraySize, FILE *file)
{
    int *firstArray = malloc(sizeof(int) * arraySize);
    int *secondArray = malloc(sizeof(int) * arraySize);

    fprintf(file, "single;%d;%.20f\n", arraySize,
            measureDotProduct(dotProductSingleThread, firstArray, secondArray, arraySize, arraySize));

    free(firstArray);
    free(secondArray);
}

int PerformDotProductComparison()
{

    return 0;
}