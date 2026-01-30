cmake_minimum_required(VERSION 3.16)
project(SelfAdaptiveRotorBlades)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/adaptive_rotor_system.cpp
    src/adaptive_rotor_system_impl.cpp
)

# Create executable
add_executable(adaptive_rotor_demo ${SOURCES})

# Compiler flags for real-time performance
target_compile_options(adaptive_rotor_demo PRIVATE
    -Wall
    -Wextra
    -O3
    -march=native
    -ffast-math
)

# Link libraries
target_link_libraries(adaptive_rotor_demo pthread m)

# Installation
install(TARGETS adaptive_rotor_demo DESTINATION bin)

# Testing
enable_testing()
add_test(NAME adaptive_rotor_basic_test COMMAND adaptive_rotor_demo)

# Performance benchmark
add_executable(rotor_performance_test src/performance_test.cpp)
target_compile_options(rotor_performance_test PRIVATE -O3 -march=native)
target_link_libraries(rotor_performance_test pthread m)
