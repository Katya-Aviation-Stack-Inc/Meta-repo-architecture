cmake_minimum_required(VERSION 3.16)
project(PredictiveAirflowEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/predictive_airflow_core.cpp
    src/predictive_airflow_engine.cpp
)

# Create executable
add_executable(predictive_airflow_demo ${SOURCES})

# Compiler flags for high-performance computing
target_compile_options(predictive_airflow_demo PRIVATE
    -Wall
    -Wextra
    -O3
    -march=native
    -ffast-math
    -fopenmp
)

# Link libraries
target_link_libraries(predictive_airflow_demo pthread m)

# Installation
install(TARGETS predictive_airflow_demo DESTINATION bin)

# Testing
enable_testing()
add_test(NAME predictive_airflow_basic_test COMMAND predictive_airflow_demo)

# CFD performance benchmark
add_executable(cfd_benchmark src/cfd_benchmark.cpp)
target_compile_options(cfd_benchmark PRIVATE -O3 -march=native -fopenmp)
target_link_libraries(cfd_benchmark pthread m)

# Neural network training test
add_executable(nn_training_test src/nn_training_test.cpp)
target_compile_options(nn_training_test PRIVATE -O3 -march=native)
target_link_libraries(nn_training_test pthread m)
