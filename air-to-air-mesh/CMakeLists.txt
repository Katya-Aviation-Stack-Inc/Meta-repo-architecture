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

# Source files for Integrated System demo
set(INTEGRATED_SYSTEM_DEMO_SOURCES
    src/demo/integrated_system_demo.cpp
    src/integrated_aviation_system.cpp
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

# Source files for Gravity Navigation system
set(GRAVITY_NAV_SOURCES
    src/gravity_nav/gravity_navigation.cpp
)

# Source files for Predictive Airflow system
set(PREDICTIVE_AIRFLOW_SOURCES
    src/airflow/predictive_airflow.cpp
)

# Source files for Hypersonic Flight Control system
set(HYPERSONIC_FLIGHT_SOURCES
    src/hypersonic_flight_control.cpp
)

# Source files for Plasma Propulsion Core system
set(PLASMA_PROPULSION_SOURCES
    src/plasma_propulsion_core.cpp
)

# Source files for Quantum Navigation system
set(QUANTUM_NAVIGATION_SOURCES
    src/quantum_navigation_system.cpp
)

# Source files for Metamaterial Aerodynamics system
set(METAMATERIAL_AERODYNAMICS_SOURCES
    src/metamaterial_aerodynamics.cpp
)

# Source files for Next-Generation demo
set(NEXT_GEN_DEMO_SOURCES
    src/demo/next_gen_demo.cpp
    src/hypersonic_flight_control.cpp
    src/plasma_propulsion_core.cpp
    src/quantum_navigation_system.cpp
    src/metamaterial_aerodynamics.cpp
)

# Source files for Ultimate demo
set(ULTIMATE_DEMO_SOURCES
    src/demo/ultimate_demo.cpp
    src/neural_interface_pilot_aircraft.cpp
    src/anti_gravity_field_generator.cpp
    src/time_dilation_flight_optimization.cpp
)

# Source files for Transcendent demo
set(TRANSCENTENT_DEMO_SOURCES
    src/demo/transcendent_demo.cpp
    src/dimensional_navigation_system.cpp
    src/consciousness_upload_system.cpp
    src/wormhole_travel_controller.cpp
)

# Source files for Absolute demo
set(ABSOLUTE_DEMO_SOURCES
    src/demo/absolute_demo.cpp
    src/quantum_teleporter_aircraft.cpp
    src/dark_matter_drive_system.cpp
)

# Source files for Final Ultimate demo
set(FINAL_ULTIMATE_DEMO_SOURCES
    src/demo/final_ultimate_demo.cpp
    src/zero_point_energy_harvester.cpp
    src/reality_matrix_interface.cpp
)

# Source files for Ultimate Completion demo
set(ULTIMATE_COMPLETION_DEMO_SOURCES
    src/demo/ultimate_completion_demo.cpp
    src/multiverse_navigation_system.cpp
    src/consciousness_collective_network.cpp
)

# Source files for Final State demo
set(FINAL_STATE_DEMO_SOURCES
    src/demo/final_state_demo.cpp
    src/existence_engine_controller.cpp
    src/absolute_reality_system.cpp
)

# Source files for Absolute End demo
set(ABSOLUTE_END_DEMO_SOURCES
    src/demo/absolute_end_demo.cpp
    src/universal_creation_engine.cpp
    src/omniscient_knowledge_system.cpp
)

# Create executables
add_executable(airmesh_demo ${AAMN_SOURCES})
add_executable(revolutionary_demo ${DEMO_SOURCES})
add_executable(self_healing_demo ${SELF_HEALING_DEMO_SOURCES})
add_executable(vortex_shield_demo ${VORTEX_SHIELD_DEMO_SOURCES})
add_executable(air_swarm_demo ${AIR_SWARM_DEMO_SOURCES})
add_executable(starnav_demo ${STARNAV_DEMO_SOURCES})
add_executable(integrated_system_demo ${INTEGRATED_SYSTEM_DEMO_SOURCES})
add_executable(next_gen_demo ${NEXT_GEN_DEMO_SOURCES})
add_executable(ultimate_demo ${ULTIMATE_DEMO_SOURCES})
add_executable(transcendent_demo ${TRANSCENTENT_DEMO_SOURCES})
add_executable(absolute_demo ${ABSOLUTE_DEMO_SOURCES})
add_executable(final_ultimate_demo ${FINAL_ULTIMATE_DEMO_SOURCES})
add_executable(ultimate_completion_demo ${ULTIMATE_COMPLETION_DEMO_SOURCES})
add_executable(final_state_demo ${FINAL_STATE_DEMO_SOURCES})
add_executable(absolute_end_demo ${ABSOLUTE_END_DEMO_SOURCES})

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

target_compile_options(integrated_system_demo PRIVATE
    -Wall
    -Wextra
    -O2
    -march=native
)

target_compile_options(next_gen_demo PRIVATE
    -Wall
    -Wextra
    -O2
    -march=native
)

target_compile_options(ultimate_demo PRIVATE
    -Wall
    -Wextra
    -O2
    -march=native
)

target_compile_options(transcendent_demo PRIVATE
    -Wall
    -Wextra
    -O2
    -march=native
)

target_compile_options(absolute_demo PRIVATE
    -Wall
    -Wextra
    -O2
    -march=native
)

target_compile_options(final_ultimate_demo PRIVATE
    -Wall
    -Wextra
    -O2
    -march=native
)

target_compile_options(ultimate_completion_demo PRIVATE
    -Wall
    -Wextra
    -O2
    -march=native
)

target_compile_options(final_state_demo PRIVATE
    -Wall
    -Wextra
    -O2
    -march=native
)

target_compile_options(absolute_end_demo PRIVATE
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
target_link_libraries(integrated_system_demo pthread)
target_link_libraries(next_gen_demo pthread)
target_link_libraries(ultimate_demo pthread)
target_link_libraries(transcendent_demo pthread)
target_link_libraries(absolute_demo pthread)
target_link_libraries(final_ultimate_demo pthread)
target_link_libraries(ultimate_completion_demo pthread)
target_link_libraries(final_state_demo pthread)
target_link_libraries(absolute_end_demo pthread)

# Installation
install(TARGETS airmesh_demo revolutionary_demo self_healing_demo vortex_shield_demo air_swarm_demo starnav_demo integrated_system_demo next_gen_demo ultimate_demo transcendent_demo absolute_demo final_ultimate_demo ultimate_completion_demo final_state_demo absolute_end_demo DESTINATION bin)

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
add_test(NAME integrated_system_demo_test COMMAND integrated_system_demo)
add_test(NAME next_gen_demo_test COMMAND next_gen_demo)
add_test(NAME ultimate_demo_test COMMAND ultimate_demo)
add_test(NAME transcendent_demo_test COMMAND transcendent_demo)
add_test(NAME absolute_demo_test COMMAND absolute_demo)
add_test(NAME final_ultimate_demo_test COMMAND final_ultimate_demo)
add_test(NAME ultimate_completion_demo_test COMMAND ultimate_completion_demo)
add_test(NAME final_state_demo_test COMMAND final_state_demo)
add_test(NAME absolute_end_demo_test COMMAND absolute_end_demo)
add_test(NAME test_revolutionary_aviation COMMAND test_revolutionary_aviation)
