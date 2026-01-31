cmake_minimum_required(VERSION 3.16)
project(RevolutionaryAviationSystems)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(src)

# Compiler definitions
add_definitions(-DADVANCED_MESH_FEATURES -DREVOLUTIONARY_SYSTEMS)

# Source files for core AAMN system
set(AAMN_SOURCES
    src/core/airmesh_core.cpp
    src/core/advanced_mesh_features.cpp
    src/main.cpp
)

# Source files for revolutionary aviation system
set(REVOLUTIONARY_SOURCES
    src/revolutionary_aviation.cpp
)

# Source files for Neuro-FCC system
set(NEURO_FCC_SOURCES
    src/neuro_fcc/neuro_fcc.cpp
)

# Source files for Adaptive Rotor system
set(ADAPTIVE_ROTOR_SOURCES
    src/adaptive_rotor/adaptive_rotor.cpp
)

# Source files for ColdJet system
set(COLDJET_SOURCES
    src/coldjet/coldjet_control.cpp
)

# Source files for Self-Healing BIOS system
set(SELF_HEALING_SOURCES
    src/self_healing/self_healing_bios.cpp
)

# Source files for Vortex Shield system
set(VORTEX_SHIELD_SOURCES
    src/vortex_shield/vortex_shield.cpp
)

# Source files for Air Swarm OS system
set(AIR_SWARM_SOURCES
    src/air_swarm/air_swarm_os.cpp
)

# Source files for StarNav Core system
set(STARNAV_SOURCES
    src/starnav/starnav_core.cpp
    src/starnav/star_camera.cpp
    src/starnav/constellation_recognizer.cpp
    src/starnav/position_calculator.cpp
    src/starnav/motion_compensator.cpp
)

# Source files for revolutionary demo
set(DEMO_SOURCES
    src/demo/revolutionary_demo.cpp
    src/core/airmesh_core.cpp
    src/core/advanced_mesh_features.cpp
    src/neuro_fcc/neuro_fcc.cpp
    src/adaptive_rotor/adaptive_rotor.cpp
    src/coldjet/coldjet_control.cpp
    src/self_healing/self_healing_bios.cpp
    src/vortex_shield/vortex_shield.cpp
    src/air_swarm/air_swarm_os.cpp
    src/starnav/starnav_core.cpp
    src/starnav/star_camera.cpp
    src/starnav/constellation_recognizer.cpp
    src/starnav/position_calculator.cpp
    src/starnav/motion_compensator.cpp
    src/revolutionary_aviation.cpp
)

# Source files for Self-Healing demo
set(SELF_HEALING_DEMO_SOURCES
    src/demo/self_healing_demo.cpp
    src/self_healing/self_healing_bios.cpp
)

# Source files for Vortex Shield demo
set(VORTEX_SHIELD_DEMO_SOURCES
    src/demo/vortex_shield_demo.cpp
    src/vortex_shield/vortex_shield.cpp
)

# Source files for Air Swarm demo
set(AIR_SWARM_DEMO_SOURCES
    src/demo/air_swarm_demo.cpp
    src/air_swarm/air_swarm_os.cpp
)

# Source files for StarNav demo
set(STARNAV_DEMO_SOURCES
    src/demo/starnav_demo.cpp
    src/starnav/starnav_core.cpp
    src/starnav/star_camera.cpp
    src/starnav/constellation_recognizer.cpp
    src/starnav/position_calculator.cpp
    src/starnav/motion_compensator.cpp
)

# Create executables
add_executable(airmesh_demo ${AAMN_SOURCES})
add_executable(revolutionary_demo ${DEMO_SOURCES})
add_executable(self_healing_demo ${SELF_HEALING_DEMO_SOURCES})
add_executable(vortex_shield_demo ${VORTEX_SHIELD_DEMO_SOURCES})
add_executable(air_swarm_demo ${AIR_SWARM_DEMO_SOURCES})
add_executable(starnav_demo ${STARNAV_DEMO_SOURCES})

# Compiler flags
target_compile_options(airmesh_demo PRIVATE
    -Wall
    -Wextra
    -O2
    -march=native
)

target_compile_options(revolutionary_demo PRIVATE
    -Wall
    -Wextra
    -O2
    -march=native
)

# Link libraries
target_link_libraries(airmesh_demo pthread)
target_link_libraries(revolutionary_demo pthread)
target_link_libraries(self_healing_demo pthread)
target_link_libraries(vortex_shield_demo pthread)
target_link_libraries(air_swarm_demo pthread)
target_link_libraries(starnav_demo pthread)

# Installation
install(TARGETS airmesh_demo revolutionary_demo self_healing_demo vortex_shield_demo air_swarm_demo starnav_demo DESTINATION bin)

# Test executable
add_executable(test_revolutionary_aviation tests/test_revolutionary_aviation.cpp)
target_link_libraries(test_revolutionary_aviation pthread)

# Testing
enable_testing()
add_test(NAME airmesh_basic_test COMMAND airmesh_demo)
add_test(NAME revolutionary_demo_test COMMAND revolutionary_demo)
add_test(NAME self_healing_demo_test COMMAND self_healing_demo)
add_test(NAME vortex_shield_demo_test COMMAND vortex_shield_demo)
add_test(NAME air_swarm_demo_test COMMAND air_swarm_demo)
add_test(NAME starnav_demo_test COMMAND starnav_demo)
add_test(NAME test_revolutionary_aviation COMMAND test_revolutionary_aviation)
