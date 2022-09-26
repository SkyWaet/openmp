#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "vectorMinValue/vectorMinValue.h"
#include "utils/utils.h"
#include <sys/time.h>


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

double getElapsedTime(struct timeval begin, struct timeval end) {
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    return seconds + microseconds * 1e-6;
}

void doTestCycle(int matrixSize) {
    Matrix *matrix = initializeArrays(matrixSize);
    struct timeval begin, end;
    int result;
    for (int i = 0; i < matrix->nRows; i++) {
        gettimeofday(&begin, 0);
        result = FindMinSingleThread(matrix->data + i * matrix->nCols, matrix->nCols);
        gettimeofday(&end, 0);
        printf("row=%d, result=%d, elapsed: %.12f seconds.\n", i, result, getElapsedTime(begin, end));
    }
    freeMatrix(matrix);
}

int main() {
    srand(time(NULL));
    doTestCycle(100);
    doTestCycle(10000);
    doTestCycle(1000000);
    return 0;
}

