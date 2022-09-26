//
// Created by GSlepenkov on 26.09.2022.
//

#ifndef OPENMP_MATRIX_H
#define OPENMP_MATRIX_H

#endif //OPENMP_MATRIX_H

typedef struct {
    int *data;
    int nRows, nCols;
} Matrix;

Matrix *initMatrix(int nRows, int nCols);

int setMatrixElem(Matrix *matrix, int row, int col, int val);

int getMatrixElem(Matrix *matrix, int row, int col);

void freeMatrix(Matrix *matrix);


