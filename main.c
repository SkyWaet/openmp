#include "vectorMinValue/vectorMinValue.h"
#include "dotProduct/dotProduct.h"
#include "integrals/integrals.h"
#include "matrixMiniMax/matrixMiniMax.h"
#include "matrixMiniMax/matrixMiniMaxForSpecialTypes.h"
#include "reductions/reductions.h"
#include <stddef.h>
#include "stdlib.h"
#include <time.h>

int main(int argc,
         char *argv[])
{
    srand(time(NULL));
    switch (*argv[1])
    {
    case '1':
        return PerformFindMinComparison();
    case '2':
        return PerformDotProductComparison();
    case '3':
        return PerformIntegralComputationComparison();
    case '4':
        return PerformMiniMaxSearchComparison();
    case '5':
        return PerformMiniMaxSearchForSpecTypesComparison();
    case '6':
        return performReductionsComparison();
    default:
        break;
    }
    return 0;
}
