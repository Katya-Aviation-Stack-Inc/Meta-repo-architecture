cmake_minimum_required(VERSION 3.16)
project(SelfHealingAvionicsBIOS)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Source files
set(SOURCES
    src/main.c
    src/self_healing_bios.c
)

# Create executable
add_executable(self_healing_bios_demo ${SOURCES})

# Compiler flags for embedded systems
target_compile_options(self_healing_bios_demo PRIVATE
    -Wall
    -Wextra
    -Werror
    -O2
    -ffreestanding
    -nostdlib
    -fno-builtin
    -fno-stack-protector
    -ffunction-sections
    -fdata-sections
)

# Linker flags
target_link_options(self_healing_bios_demo PRIVATE
    -Wl,--gc-sections
    -Wl,--print-gc-sections
)

# Installation
install(TARGETS self_healing_bios_demo DESTINATION bin)

# Testing
enable_testing()
add_test(NAME self_healing_bios_basic_test COMMAND self_healing_bios_demo)

# Memory stress test
add_executable(memory_stress_test src/memory_stress_test.c src/self_healing_bios.c)
target_compile_options(memory_stress_test PRIVATE -O2 -Wall -Wextra)
target_link_options(memory_stress_test PRIVATE -Wl,--gc-sections)

# Bus reliability test
add_executable(bus_reliability_test src/bus_reliability_test.c src/self_healing_bios.c)
target_compile_options(bus_reliability_test PRIVATE -O2 -Wall -Wextra)
target_link_options(bus_reliability_test PRIVATE -Wl,--gc-sections)

# Critical recovery test
add_executable(critical_recovery_test src/critical_recovery_test.c src/self_healing_bios.c)
target_compile_options(critical_recovery_test PRIVATE -O2 -Wall -Wextra)
target_link_options(critical_recovery_test PRIVATE -Wl,--gc-sections)
