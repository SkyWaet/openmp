#include "vectorMinValue/vectorMinValue.h"
#include "dotProduct/dotProduct.h"
#include <stddef.h>

int main(int argc,
         char *argv[])
{
    srand(time(NULL));
    switch (*argv[1])
    {
    case '1':
        return PerformFindMinComparison();
        break;
    case '2':
        return PerformDotProductComparison();
        break;
    default:
        break;
    }
    return 0;
}
