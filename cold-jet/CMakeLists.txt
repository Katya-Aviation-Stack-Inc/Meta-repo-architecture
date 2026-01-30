cmake_minimum_required(VERSION 3.16)
project(ColdJet)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/cold_jet_core.cpp
    src/cold_jet_controller.cpp
)

# Create executable
add_executable(cold_jet_demo ${SOURCES})

# Compiler flags for scientific computing
target_compile_options(cold_jet_demo PRIVATE
    -Wall
    -Wextra
    -O3
    -march=native
    -ffast-math
)

# Link libraries
target_link_libraries(cold_jet_demo pthread m)

# Installation
install(TARGETS cold_jet_demo DESTINATION bin)

# Testing
enable_testing()
add_test(NAME cold_jet_basic_test COMMAND cold_jet_demo)

# Thermodynamic simulation test
add_executable(thermodynamic_test src/thermodynamic_test.cpp)
target_compile_options(thermodynamic_test PRIVATE -O3 -march=native)
target_link_libraries(thermodynamic_test pthread m)
