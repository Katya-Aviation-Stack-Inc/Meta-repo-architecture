cmake_minimum_required(VERSION 3.16)
project(NeuroFCC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/neuro_fcc_core.cpp
    src/failure_recovery.cpp
)

# Create executable
add_executable(neuro_fcc_demo ${SOURCES})

# Compiler flags for real-time performance
target_compile_options(neuro_fcc_demo PRIVATE
    -Wall
    -Wextra
    -O3
    -march=native
    -ffast-math
)

# Link libraries
target_link_libraries(neuro_fcc_demo pthread m)

# Installation
install(TARGETS neuro_fcc_demo DESTINATION bin)

# Testing
enable_testing()
add_test(NAME neuro_fcc_basic_test COMMAND neuro_fcc_demo)

# Performance testing
add_executable(neuro_fcc_latency_test src/latency_test.cpp)
target_compile_options(neuro_fcc_latency_test PRIVATE -O3 -march=native)
target_link_libraries(neuro_fcc_latency_test pthread m)
