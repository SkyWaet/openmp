cmake_minimum_required(VERSION 3.16)
project(openmp C)

set(CMAKE_C_STANDARD 11)

add_executable(openmp main.c vectorMinValue/vectorMinValue.h vectorMinValue/VectorMinValueImpl.c utils/utils.c utils/utils.h datatypes/matrix.c datatypes/matrix.h)
