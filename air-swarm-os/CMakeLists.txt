cmake_minimum_required(VERSION 3.16)
project(AirSwarmOS)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/air_swarm_os.cpp
    src/air_swarm_os_core.cpp
)

# Create executable
add_executable(air_swarm_os_demo ${SOURCES})

# Compiler flags for distributed systems
target_compile_options(air_swarm_os_demo PRIVATE
    -Wall
    -Wextra
    -O3
    -march=native
    -ffast-math
)

# Link libraries
target_link_libraries(air_swarm_os_demo pthread m)

# Installation
install(TARGETS air_swarm_os_demo DESTINATION bin)

# Testing
enable_testing()
add_test(NAME air_swarm_os_basic_test COMMAND air_swarm_os_demo)

# Network simulation test
add_executable(network_simulation_test src/network_simulation_test.cpp src/air_swarm_os_core.cpp src/air_swarm_os.cpp)
target_compile_options(network_simulation_test PRIVATE -O3 -march=native)
target_link_libraries(network_simulation_test pthread m)

# Blockchain performance test
add_executable(blockchain_test src/blockchain_test.cpp src/air_swarm_os_core.cpp src/air_swarm_os.cpp)
target_compile_options(blockchain_test PRIVATE -O3 -march=native)
target_link_libraries(blockchain_test pthread m)

# Consensus mechanism test
add_executable(consensus_test src/consensus_test.cpp src/air_swarm_os_core.cpp src/air_swarm_os.cpp)
target_compile_options(consensus_test PRIVATE -O3 -march=native)
target_link_libraries(consensus_test pthread m)

# Conflict resolution test
add_executable(conflict_resolution_test src/conflict_resolution_test.cpp src/air_swarm_os_core.cpp src/air_swarm_os.cpp)
target_compile_options(conflict_resolution_test PRIVATE -O3 -march=native)
target_link_libraries(conflict_resolution_test pthread m)
