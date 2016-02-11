cmake_minimum_required(VERSION 3.0)
project(pararellQuicksort)

find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
SET(CMAKE_C_COMPILER mpicc)

set(SOURCE_FILES main.c worker.c)
add_executable(pararellQuicksort main.c)
add_executable(worker worker.c)