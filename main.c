#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vectorMinValue/vectorMinValue.h"
#include "utils/utils.h"
#include <sys/time.h>
#include "omp.h"


void fillOrderedVectorAsc(int size, int *vector) {
    for (int i = 0; i < size; i++) {
        vector[i] = i;
    }
}

void fillOrderedVectorDesc(int size, int *vector) {
    for (int i = size - 1; i >= 0; i--) {
        vector[size - i - 1] = i;
    }
}

void fillWithRandomValues(int size, int *vector) {
    int *end = vector + size;
    for (int *start = vector; start < end; start++) {
        *start = GetRandomInteger(0, size);
    }
}

Matrix *initializeArrays(int size) {
    Matrix *matrix = initMatrix(3, size);
    fillOrderedVectorAsc(size, matrix->data);
    fillOrderedVectorDesc(size, matrix->data + size);
    fillWithRandomValues(size, matrix->data + 2 * size);
    return matrix;
}

double measure(int(*method)(int *, int), int *array, int size) {
    double start = omp_get_wtime();
    double end = omp_get_wtime();
    method(array, size);
    return end - start;
}

void doTestCycle(int matrixSize) {
    Matrix *matrix = initializeArrays(matrixSize);
    for (int i = 0; i < matrix->nRows; i++) {
        printf("Single thread. Array number=%d. Size=%d,  elapsed: %.12f seconds.\n", i, matrixSize,
               measure(FindMinSingleThread, matrix->data + i * matrix->nCols, matrix->nCols));
        printf("For loop parallelism, Array number=%d. Size=%d,  elapsed: %.12f seconds.\n", i, matrixSize,
               measure(FindMinWithForLoopParallelism, matrix->data + i * matrix->nCols, matrix->nCols));
    }
    freeMatrix(matrix);
}


int main() {
    srand(time(NULL));
    doTestCycle(10000);
    doTestCycle(10000000);
    doTestCycle(100000000);
    return 0;
}

