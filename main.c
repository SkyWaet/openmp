#include "vectorMinValue/vectorMinValue.h"
#include "omp.h"
#include "stdio.h"

int main(int argc,
         char *argv[])
{
    switch (*argv[1])
    {
    case '1':
        return PerformTaskOne();
        break;
    
    default:
        break;
    }
    return 0;
}
