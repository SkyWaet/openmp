cmake_minimum_required(VERSION 3.16)
project(openmp C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-fopenmp")

add_executable(openmp main.c 
vectorMinValue/vectorMinValue.h vectorMinValue/vectorMinValueImpl.c 
utils/utils.c utils/utils.h 
datatypes/matrix.c datatypes/matrix.h 
dotProduct/dotProduct.c dotProduct/dotProduct.h 
integrals/integrals.c integrals/integrals.h
matrixMiniMax/matrixMiniMax.c matrixMiniMax/matrixMiniMax.h
matrixMiniMax/matrixMiniMaxForSpecialTypes.c matrixMiniMax/matrixMiniMaxForSpecialTypes.h
reductions/reductions.c reductions/reductions.h
nestedParallelism/nestedParallelism.c nestedParallelism/nestedParallelism.h 
differentCycleModes/differentCycleModes.c differentCycleModes/differentCycleModes.h
)

target_link_libraries(openmp m)