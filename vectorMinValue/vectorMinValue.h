//
// Created by GSlepenkov on 26.09.2022.
//

#ifndef OPENMP_VECTORMINVALUE_H
#define OPENMP_VECTORMINVALUE_H

#endif // OPENMP_VECTORMINVALUE_H

int FindMinSingleThread(int *vector, int size);

int FindMinWithForLoopParallelism(int *vector, int size);

int FindMinWithReduction(int *vector, int size);

int PerformFindMinComparison();