#include "vectorMinValue/vectorMinValue.h"
#include "dotProduct/dotProduct.h"
#include "integrals/integrals.h"
#include <stddef.h>
#include "stdlib.h"
#include <time.h>

int main(int argc,
         char *argv[])
{
    //return PerformIntegralComputationComparison();
    srand(time(NULL));
    switch (*argv[1])
    {
    case '1':
        return PerformFindMinComparison();
    case '2':
        return PerformDotProductComparison();
    case '3':
        return PerformIntegralComputationComparison();
    default:
        break;
    }
    return 0;
}
