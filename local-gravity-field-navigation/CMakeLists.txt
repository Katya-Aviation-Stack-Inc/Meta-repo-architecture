cmake_minimum_required(VERSION 3.16)
project(LocalGravityFieldNavigation)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/lgfn_core.cpp
    src/lgfn_navigation.cpp
)

# Create executable
add_executable(lgfn_demo ${SOURCES})

# Compiler flags for scientific computing
target_compile_options(lgfn_demo PRIVATE
    -Wall
    -Wextra
    -O3
    -march=native
    -ffast-math
)

# Link libraries
target_link_libraries(lgfn_demo pthread m)

# Installation
install(TARGETS lgfn_demo DESTINATION bin)

# Testing
enable_testing()
add_test(NAME lgfn_basic_test COMMAND lgfn_demo)

# Gravimeter simulation test
add_executable(gravimeter_test src/gravimeter_test.cpp)
target_compile_options(gravimeter_test PRIVATE -O3 -march=native)
target_link_libraries(gravimeter_test pthread m)

# Gravity field mapping test
add_executable(gravity_mapping_test src/mapping_test.cpp)
target_compile_options(gravity_mapping_test PRIVATE -O3 -march=native)
target_link_libraries(gravity_mapping_test pthread m)
