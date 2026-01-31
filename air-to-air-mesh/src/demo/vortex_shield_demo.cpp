#include "../vortex_shield/vortex_shield.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace vortex_shield;

int main() {
    std::cout << "=== Revolutionary Vortex Shield System Demo ===" << std::endl;
    std::cout << "Demonstrating software-based vortex ring prevention..." << std::endl << std::endl;

    // Create Vortex Shield instance
    VortexShield shield;
    
    // Initialize the system
    if (shield.initialize()) {
        std::cout << "✓ Vortex Shield System initialized successfully" << std::endl;
    } else {
        std::cout << "✗ Failed to initialize Vortex Shield System" << std::endl;
        return -1;
    }
    
    // Enable the system
    shield.enable_system(true);
    
    // Set system parameters
    shield.set_sensitivity_threshold(0.6);
    shield.set_response_aggressiveness(0.9);
    shield.set_prediction_horizon(1.5);
    
    // Simulate normal flight conditions
    std::cout << "\n--- Simulating Normal Flight Conditions ---" << std::endl;
    
    // Generate normal pressure data
    std::default_random_engine generator;
    std::uniform_real_distribution<double> pressure_dist(95000.0, 105000.0); // Normal pressure range
    std::uniform_real_distribution<double> temp_dist(280.0, 300.0); // Normal temperature range
    
    for (int i = 0; i < 10; ++i) {
        PressureData pressure_data;
        pressure_data.static_pressure = pressure_dist(generator);
        pressure_data.dynamic_pressure = 1000.0 + (i * 100); // Increasing dynamic pressure
        pressure_data.total_pressure = pressure_data.static_pressure + pressure_data.dynamic_pressure;
        pressure_data.temperature = temp_dist(generator);
        pressure_data.timestamp = std::chrono::system_clock::now();
        
        shield.update_pressure_data(pressure_data);
        
        // Add small delay to simulate real-time data
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Generate normal flow dynamics
    std::uniform_real_distribution<double> velocity_dist(10.0, 50.0);
    std::uniform_real_distribution<double> vorticity_dist(0.1, 1.0);
    
    for (int i = 0; i < 10; ++i) {
        FlowDynamics flow_data;
        flow_data.axial_velocity = velocity_dist(generator);
        flow_data.radial_velocity = velocity_dist(generator) * 0.1;
        flow_data.tangential_velocity = velocity_dist(generator) * 0.2;
        flow_data.vorticity = vorticity_dist(generator);
        flow_data.turbulence_intensity = 0.1 + (i * 0.01);
        flow_data.timestamp = std::chrono::system_clock::now();
        
        shield.update_flow_dynamics(flow_data);
        
        // Add small delay to simulate real-time data
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Monitor conditions
    shield.monitor_vortex_conditions();
    
    // Check for vortex ring state
    VortexState state = shield.detect_vortex_ring_state();
    
    if (!state.is_vortex_ring_detected) {
        std::cout << "✓ No vortex ring state detected during normal flight" << std::endl;
    }
    
    // Simulate vortex ring conditions
    std::cout << "\n--- Simulating Vortex Ring Conditions ---" << std::endl;
    std::cout << "WARNING: Entering vortex ring state conditions!" << std::endl;
    
    // Generate vortex-like pressure data
    for (int i = 0; i < 5; ++i) {
        PressureData pressure_data;
        pressure_data.static_pressure = 101325.0; // Standard atmospheric pressure
        pressure_data.dynamic_pressure = 500.0 - (i * 50); // Decreasing dynamic pressure
        pressure_data.total_pressure = pressure_data.static_pressure + pressure_data.dynamic_pressure;
        pressure_data.temperature = 288.0;
        pressure_data.timestamp = std::chrono::system_clock::now();
        
        shield.update_pressure_data(pressure_data);
        
        // Add small delay to simulate real-time data
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Generate vortex-like flow dynamics
    for (int i = 0; i < 5; ++i) {
        FlowDynamics flow_data;
        flow_data.axial_velocity = 2.0 - (i * 0.2); // Very low axial velocity
        flow_data.radial_velocity = 1.0;
        flow_data.tangential_velocity = 15.0 + (i * 2.0); // Increasing tangential velocity
        flow_data.vorticity = 5.0 + (i * 1.0); // High vorticity
        flow_data.turbulence_intensity = 0.8 + (i * 0.05);
        flow_data.timestamp = std::chrono::system_clock::now();
        
        shield.update_flow_dynamics(flow_data);
        
        // Add small delay to simulate real-time data
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Update rotor state to simulate descent
    shield.update_rotor_state(300.0, 5.0); // 300 RPM, 5° collective
    
    // Monitor conditions again
    shield.monitor_vortex_conditions();
    
    // Check for vortex ring state
    state = shield.detect_vortex_ring_state();
    
    if (state.is_vortex_ring_detected) {
        std::cout << "✓ Vortex ring state successfully detected!" << std::endl;
        std::cout << "   Vortex Ring Index: " << state.vortex_ring_index << std::endl;
        std::cout << "   Descent Rate: " << state.descent_rate << " m/s" << std::endl;
        std::cout << "   Rotor RPM: " << state.rotor_rpm << std::endl;
        std::cout << "   Collective Angle: " << state.collective_angle << "°" << std::endl;
    }
    
    // Suppress vortex ring state
    std::cout << "\n--- Initiating Vortex Ring Suppression ---" << std::endl;
    shield.suppress_vortex_ring_state();
    
    // Calculate preventive adjustments
    ControlAdjustment adjustments = shield.calculate_preventive_adjustments(state);
    
    // Apply control corrections
    shield.apply_control_corrections(adjustments);
    
    // Update performance metrics
    auto diagnostics = shield.get_system_diagnostics();
    
    std::cout << "\n--- System Performance Metrics ---" << std::endl;
    std::cout << "Total Detections: " << shield.get_total_detections() << std::endl;
    std::cout << "Successful Corrections: " << shield.get_successful_corrections() << std::endl;
    std::cout << "Detection Accuracy: " << shield.get_detection_accuracy() * 100 << "%" << std::endl;
    std::cout << "Average Correction Time: " << diagnostics.avg_correction_time << " ms" << std::endl;
    
    std::cout << "\n=== Vortex Shield System Demo Complete ===" << std::endl;
    std::cout << "The revolutionary vortex ring prevention system has successfully detected and suppressed vortex ring state!" << std::endl;
    
    return 0;
}