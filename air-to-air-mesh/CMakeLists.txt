cmake_minimum_required(VERSION 3.16)
project(AirToAirMeshNetwork)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/mesh_network_node.cpp
    src/distributed_autopilot.cpp
)

# Create executable
add_executable(airmesh_demo ${SOURCES})

# Compiler flags
target_compile_options(airmesh_demo PRIVATE
    -Wall
    -Wextra
    -O2
    -march=native
)

# Link libraries
target_link_libraries(airmesh_demo pthread)

# Installation
install(TARGETS airmesh_demo DESTINATION bin)

# Testing
enable_testing()
add_test(NAME airmesh_basic_test COMMAND airmesh_demo)
