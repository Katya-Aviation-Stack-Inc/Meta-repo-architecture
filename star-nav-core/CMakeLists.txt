cmake_minimum_required(VERSION 3.16)
project(StarNavCore)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.cpp
    src/star_nav_core.cpp
)

# Create executable
add_executable(starnav_demo ${SOURCES})

# Compiler flags for image processing
target_compile_options(starnav_demo PRIVATE
    -Wall
    -Wextra
    -O3
    -march=native
    -ffast-math
)

# Link libraries
target_link_libraries(starnav_demo pthread m)

# Installation
install(TARGETS starnav_demo DESTINATION bin)

# Testing
enable_testing()
add_test(NAME starnav_basic_test COMMAND starnav_demo)

# Star pattern recognition test
add_executable(star_recognition_test src/star_recognition_test.cpp src/star_nav_core.cpp)
target_compile_options(star_recognition_test PRIVATE -O3 -march=native)
target_link_libraries(star_recognition_test pthread m)

# Camera calibration test
add_executable(camera_calibration_test src/camera_calibration_test.cpp src/star_nav_core.cpp)
target_compile_options(camera_calibration_test PRIVATE -O3 -march=native)
target_link_libraries(camera_calibration_test pthread m)

# Constellation matching test
add_executable(constellation_test src/constellation_test.cpp src/star_nav_core.cpp)
target_compile_options(constellation_test PRIVATE -O3 -march=native)
target_link_libraries(constellation_test pthread m)

# Emergency navigation test
add_executable(emergency_navigation_test src/emergency_navigation_test.cpp src/star_nav_core.cpp)
target_compile_options(emergency_navigation_test PRIVATE -O3 -march=native)
target_link_libraries(emergency_navigation_test pthread m)
