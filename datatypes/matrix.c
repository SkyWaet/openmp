//
// Created by GSlepenkov on 26.09.2022.
//
#include "matrix.h"
#include <malloc.h>

Matrix *InitMatrix(int nRows, int nCols) {
    Matrix *matrix = malloc(sizeof(Matrix));
    matrix->data = malloc(nRows * nCols * sizeof(int));
    matrix->nRows = nRows;
    matrix->nCols = nCols;
    return matrix;
}

int SetMatrixElem(Matrix *matrix, int row, int col, int val) {
    int index = matrix->nCols * row + col;
    if (index < 0 || index >= matrix->nRows * matrix->nCols) {
        return -1;
    }
    matrix->data[index] = val;
    return 0;
}

int GetMatrixElem(Matrix *matrix, int row, int col) {
    int index = matrix->nCols * row + col;
    return *(matrix->data + index);
}

void FreeMatrix(Matrix *matrix) {
    free(matrix->data);
    matrix->data = NULL;
    matrix->nCols = 0;
    matrix->nRows = 0;
}


