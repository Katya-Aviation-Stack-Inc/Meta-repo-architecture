cmake_minimum_required(VERSION 3.16)
project(VortexShield)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/vortex_shield_core.cpp
    src/vortex_shield_system.cpp
)

# Create executable
add_executable(vortex_shield_demo ${SOURCES})

# Compiler flags for real-time systems
target_compile_options(vortex_shield_demo PRIVATE
    -Wall
    -Wextra
    -O3
    -march=native
    -ffast-math
)

# Link libraries
target_link_libraries(vortex_shield_demo pthread m)

# Installation
install(TARGETS vortex_shield_demo DESTINATION bin)

# Testing
enable_testing()
add_test(NAME vortex_shield_basic_test COMMAND vortex_shield_demo)

# Vortex detection accuracy test
add_executable(vortex_detection_test src/vortex_detection_test.cpp src/vortex_shield_core.cpp src/vortex_shield_system.cpp)
target_compile_options(vortex_detection_test PRIVATE -O3 -march=native)
target_link_libraries(vortex_detection_test pthread m)

# Recovery strategy test
add_executable(recovery_strategy_test src/recovery_strategy_test.cpp src/vortex_shield_core.cpp src/vortex_shield_system.cpp)
target_compile_options(recovery_strategy_test PRIVATE -O3 -march=native)
target_link_libraries(recovery_strategy_test pthread m)

# Real-time performance test
add_executable(realtime_performance_test src/realtime_test.cpp src/vortex_shield_core.cpp src/vortex_shield_system.cpp)
target_compile_options(realtime_performance_test PRIVATE -O3 -march=native)
target_link_libraries(realtime_performance_test pthread m)
