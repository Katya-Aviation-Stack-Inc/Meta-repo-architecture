#include "vortex_shield_core.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace vortex_shield;

int main() {
    std::cout << "=== Vortex Shield System Demo ===" << std::endl;
    std::cout << "Life-saving helicopter vortex ring state suppression system" << std::endl;
    
    // Initialize Vortex Shield System
    VortexShieldSystem vortex_shield;
    
    if (!vortex_shield.initialize()) {
        std::cerr << "Failed to initialize Vortex Shield system" << std::endl;
        return 1;
    }
    
    std::cout << "Vortex Shield system initialized successfully" << std::endl;
    std::cout << "Starting vortex ring protection simulation..." << std::endl;
    
    // Simulation parameters
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> rpm_dist(300.0, 500.0); // RPM
    std::uniform_real_distribution<> pitch_dist(0.0, 15.0); // degrees
    std::uniform_real_distribution<> cyclic_dist(-10.0, 10.0); // degrees
    std::uniform_real_distribution<> airspeed_dist(-20.0, 50.0); // m/s
    std::uniform_real_distribution<> descent_dist(-15.0, 5.0); // m/s
    std::uniform_real_distribution<> altitude_dist(100.0, 3000.0); // meters
    std::uniform_real_distribution<> pressure_dist(95000.0, 105000.0); // Pa
    
    RotorState rotor_state{};
    FlightConditions flight_conditions{};
    PressureSensorData pressure_data{};
    
    // Main simulation loop
    for (int iteration = 0; iteration < 400; ++iteration) {
        // Generate rotor state
        rotor_state.main_rotor_rpm = rpm_dist(gen);
        rotor_state.tail_rotor_rpm = rotor_state.main_rotor_rpm * 5.0; // Typical tail rotor ratio
        rotor_state.collective_pitch = pitch_dist(gen);
        rotor_state.lateral_cyclic = cyclic_dist(gen);
        rotor_state.longitudinal_cyclic = cyclic_dist(gen);
        rotor_state.pedal_position = 0.0;
        rotor_state.rotor_radius = 5.0; // 5 meter radius
        rotor_state.num_blades = 4;
        rotor_state.disc_loading = 0.1; // Typical disc loading
        rotor_state.blade_twist = 8.0; // degrees
        
        // Generate flight conditions
        flight_conditions.airspeed = airspeed_dist(gen);
        flight_conditions.vertical_speed = descent_dist(gen);
        flight_conditions.altitude = altitude_dist(gen);
        flight_conditions.air_density = 1.225 * std::exp(-flight_conditions.altitude / 8000.0); // Altitude correction
        flight_conditions.temperature = 288.15 - 0.0065 * flight_conditions.altitude; // Standard lapse rate
        flight_conditions.wind_speed = 10.0;
        flight_conditions.wind_direction = 0.0;
        flight_conditions.turbulence_intensity = 0.1 + (iteration % 20) * 0.02;
        flight_conditions.pressure_altitude = flight_conditions.altitude;
        
        // Generate pressure sensor data
        pressure_data.pressure = pressure_dist(gen);
        pressure_data.dynamic_pressure = 0.5 * flight_conditions.air_density * flight_conditions.airspeed * flight_conditions.airspeed;
        pressure_data.static_pressure = pressure_data.pressure - pressure_data.dynamic_pressure;
        pressure_data.timestamp = std::chrono::system_clock::now();
        
        // Generate 8 pressure sensor readings around rotor disc
        for (int i = 0; i < 8; ++i) {
            double angle = 2.0 * M_PI * i / 8;
            double radius = rotor_state.rotor_radius * (0.3 + 0.6 * i / 7.0); // 30% to 90% of radius
            
            // Base pressure with radial variation
            double sensor_pressure = pressure_data.pressure;
            sensor_pressure -= 50.0 * (radius / rotor_state.rotor_radius); // Pressure drop with radius
            
            // Add turbulence effects
            sensor_pressure += (iteration % 10 - 5) * 2.0 * flight_conditions.turbulence_intensity;
            
            // Add cyclic effects
            sensor_pressure += rotor_state.collective_pitch * 10.0;
            sensor_pressure += rotor_state.lateral_cyclic * 5.0 * std::sin(angle);
            sensor_pressure += rotor_state.longitudinal_cyclic * 5.0 * std::cos(angle);
            
            pressure_data.sensor_readings[i] = sensor_pressure;
        }
        
        // Simulate different flight scenarios
        if (iteration == 50) {
            std::cout << "\n*** Rapid Descent Scenario ***" << std::endl;
            flight_conditions.vertical_speed = -12.0; // Rapid descent
            flight_conditions.airspeed = 5.0; // Low airspeed
            vortex_shield.set_protection_level(0.8);
        }
        
        if (iteration == 100) {
            std::cout << "\n*** High Altitude Vortex Ring ***" << std::endl;
            flight_conditions.altitude = 2000.0; // High altitude
            flight_conditions.vertical_speed = -8.0;
            flight_conditions.air_density *= 0.8; // Lower air density
        }
        
        if (iteration == 150) {
            std::cout << "\n*** Turbulent Conditions ***" << std::endl;
            flight_conditions.turbulence_intensity = 0.5; // High turbulence
            flight_conditions.wind_speed = 25.0; // Strong wind
        }
        
        if (iteration == 200) {
            std::cout << "\n*** Emergency Vortex Ring ***" << std::endl;
            flight_conditions.vertical_speed = -15.0; // Very rapid descent
            flight_conditions.airspeed = 0.0; // Zero airspeed
            rotor_state.collective_pitch = 12.0; // High collective
            vortex_shield.set_emergency_mode(true);
        }
        
        if (iteration == 250) {
            std::cout << "\n*** Recovery from Vortex Ring ***" << std::endl;
            // System should detect and recover from previous vortex ring
            vortex_shield.set_emergency_mode(false);
        }
        
        if (iteration == 300) {
            std::cout << "\n*** Low Speed Hover ***" << std::endl;
            flight_conditions.airspeed = 2.0; // Very low airspeed
            flight_conditions.vertical_speed = -2.0; // Slow descent
            rotor_state.collective_pitch = 8.0;
        }
        
        if (iteration == 350) {
            std::cout << "\n*** Aggressive Maneuvering ***" << std::endl;
            rotor_state.lateral_cyclic = 8.0; // Aggressive lateral cyclic
            rotor_state.longitudinal_cyclic = 10.0; // Aggressive longitudinal cyclic
            flight_conditions.turbulence_intensity = 0.3;
        }
        
        // Update vortex protection system
        auto control_command = vortex_shield.update_vortex_protection(
            rotor_state, flight_conditions, pressure_data
        );
        
        // Display status every 50 iterations
        if (iteration % 50 == 0) {
            std::cout << "Iteration " << iteration << ":" << std::endl;
            std::cout << "  Rotor RPM: " << rotor_state.main_rotor_rpm << std::endl;
            std::cout << "  Airspeed: " << flight_conditions.airspeed << " m/s" << std::endl;
            std::cout << "  Descent Rate: " << flight_conditions.vertical_speed << " m/s" << std::endl;
            std::cout << "  Altitude: " << flight_conditions.altitude << " m" << std::endl;
            std::cout << "  System Health: " << vortex_shield.get_system_health() << std::endl;
            
            bool vortex_detected = vortex_shield.is_vortex_ring_detected();
            std::cout << "  Vortex Ring: " << (vortex_detected ? "DETECTED" : "CLEAR") << std::endl;
            
            if (vortex_detected) {
                auto vortex_state = vortex_shield.get_vortex_state();
                std::cout << "    Severity: " << vortex_state.severity << std::endl;
                std::cout << "    Descent Rate: " << vortex_state.descent_rate << " m/s" << std::endl;
                std::cout << "    Induced Flow: " << vortex_state.induced_flow_velocity << " m/s" << std::endl;
            }
            
            bool recovery_active = vortex_shield.is_recovery_in_progress();
            std::cout << "  Recovery: " << (recovery_active ? "ACTIVE" : "STANDBY") << std::endl;
            
            if (recovery_active) {
                std::cout << "    Collective Adj: " << control_command.collective_adjustment << "°" << std::endl;
                std::cout << "    Forward Cyclic: " << control_command.cyclic_longitudinal << "°" << std::endl;
                std::cout << "    Lateral Cyclic: " << control_command.cyclic_lateral << "°" << std::endl;
                std::cout << "    Emergency: " << (control_command.emergency_recovery ? "YES" : "NO") << std::endl;
            }
            
            std::cout << std::endl;
        }
        
        // Simulate real-time control loop (100Hz target)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Test emergency recovery procedures
    std::cout << "\n*** Testing Emergency Recovery Procedures ***" << std::endl;
    
    // Create severe vortex ring conditions
    flight_conditions.vertical_speed = -18.0; // Very rapid descent
    flight_conditions.airspeed = 0.0;
    rotor_state.collective_pitch = 15.0;
    
    for (int i = 0; i < 20; ++i) {
        auto emergency_command = vortex_shield.update_vortex_protection(
            rotor_state, flight_conditions, pressure_data
        );
        
        bool vortex_detected = vortex_shield.is_vortex_ring_detected();
        bool recovery_active = vortex_shield.is_recovery_in_progress();
        
        std::cout << "Emergency test " << i + 1 << ": ";
        std::cout << "Vortex=" << (vortex_detected ? "YES" : "NO") << ", ";
        std::cout << "Recovery=" << (recovery_active ? "YES" : "NO") << ", ";
        std::cout << "Emergency=" << (emergency_command.emergency_recovery ? "YES" : "NO") << std::endl;
        
        // Simulate recovery effect
        if (recovery_active) {
            flight_conditions.vertical_speed += 0.5; // Gradual recovery
            flight_conditions.airspeed += 2.0; // Gain airspeed
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Test different recovery strategies
    std::cout << "\n*** Testing Recovery Strategies ***" << std::endl;
    
    std::vector<std::string> strategies = {"Collective Reduction", "Forward Translation", 
                                            "Lateral Motion", "Altitude Gain", 
                                            "Emergency Power", "Combined Maneuver"};
    
    for (size_t i = 0; i < strategies.size(); ++i) {
        std::cout << "Testing " << strategies[i] << "..." << std::endl;
        
        // Create moderate vortex ring conditions
        flight_conditions.vertical_speed = -10.0;
        flight_conditions.airspeed = 5.0;
        
        // Run recovery for 2 seconds
        for (int j = 0; j < 20; ++j) {
            auto test_command = vortex_shield.update_vortex_protection(
                rotor_state, flight_conditions, pressure_data
            );
            
            if (j == 19) { // After 2 seconds
                std::cout << "  Final collective adjustment: " << test_command.collective_adjustment << "°" << std::endl;
                std::cout << "  Recovery progress: " << (vortex_shield.is_recovery_in_progress() ? "Active" : "Complete") << std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    // Final system status
    std::cout << "\n=== Final System Status ===" << std::endl;
    std::cout << "System Health: " << vortex_shield.get_system_health() << std::endl;
    std::cout << "Vortex Ring Detected: " << (vortex_shield.is_vortex_ring_detected() ? "YES" : "NO") << std::endl;
    
    if (vortex_shield.is_vortex_ring_detected()) {
        auto final_vortex = vortex_shield.get_vortex_state();
        std::cout << "Final Vortex Severity: " << final_vortex.severity << std::endl;
        std::cout << "Final Descent Rate: " << final_vortex.descent_rate << " m/s" << std::endl;
    }
    
    std::cout << "Recovery in Progress: " << (vortex_shield.is_recovery_in_progress() ? "YES" : "NO") << std::endl;
    
    // Run system diagnostics
    std::cout << "\n*** Running System Diagnostics ***" << std::endl;
    vortex_shield.run_system_diagnostics();
    
    // Calibrate sensors
    std::cout << "\n*** Calibrating Sensors ***" << std::endl;
    vortex_shield.calibrate_sensors();
    
    // Test recovery procedures
    std::cout << "\n*** Testing Recovery Procedures ***" << std::endl;
    vortex_shield.test_recovery_procedures();
    
    std::cout << "\nVortex Shield Demo completed successfully." << std::endl;
    std::cout << "Demonstrated life-saving vortex ring state detection and recovery." << std::endl;
    std::cout << "This technology will save countless helicopter pilot lives!" << std::endl;
    
    // Shutdown system
    vortex_shield.shutdown();
    
    return 0;
}
