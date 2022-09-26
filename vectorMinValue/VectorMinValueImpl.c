//
// Created by GSlepenkov on 26.09.2022.
//

#include "vectorMinValue.h"
#include "limits.h"

int FindMinSingleThread(int *vector, int size) {
    int *end = vector + size;
    int max = INT_MIN;
    for (int *start = vector; start < end; start++) {
        if (max < *start) {
            max = *start;
        }
    }
    return max;
}