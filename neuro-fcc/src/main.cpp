#include "neuro_fcc_core.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace neuro_fcc;

int main() {
    std::cout << "=== Neuro-FCC (Flight Control Contour) Demo ===" << std::endl;
    
    // Initialize Neuro-FCC Core
    NeuroFCCCore fcc_core;
    
    if (!fcc_core.initialize()) {
        std::cerr << "Failed to initialize Neuro-FCC Core" << std::endl;
        return 1;
    }
    
    // Initialize failure recovery controller
    FailureRecoveryController recovery_controller(fcc_core);
    
    std::cout << "Starting flight control simulation..." << std::endl;
    
    // Simulation parameters
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> attitude_dist(-0.5, 0.5); // radians
    std::uniform_real_distribution<> rate_dist(-1.0, 1.0); // rad/s
    std::uniform_real_distribution<> airspeed_dist(50.0, 250.0); // m/s
    std::uniform_real_distribution<> altitude_dist(1000.0, 10000.0); // meters
    std::uniform_real_distribution<> input_dist(-1.0, 1.0);
    
    AircraftState current_state{};
    PilotInput pilot_input{};
    
    // Main simulation loop
    for (int iteration = 0; iteration < 1000; ++iteration) {
        // Simulate aircraft state
        current_state.roll = attitude_dist(gen);
        current_state.pitch = attitude_dist(gen) * 0.3; // Smaller pitch range
        current_state.yaw = attitude_dist(gen);
        current_state.roll_rate = rate_dist(gen);
        current_state.pitch_rate = rate_dist(gen) * 0.5;
        current_state.yaw_rate = rate_dist(gen) * 0.2;
        current_state.airspeed = airspeed_dist(gen);
        current_state.altitude = altitude_dist(gen);
        current_state.vertical_speed = rate_dist(gen) * 10.0; // m/s
        current_state.bank_angle = current_state.roll;
        current_state.load_factor = 1.0 + std::abs(current_state.roll) * 0.5;
        
        // Simulate pilot input
        pilot_input.stick_x = input_dist(gen);
        pilot_input.stick_y = input_dist(gen);
        pilot_input.rudder_pedal = input_dist(gen) * 0.5;
        pilot_input.throttle_lever = 0.5 + input_dist(gen) * 0.5;
        pilot_input.flap_lever = iteration < 200 ? 0.0 : (iteration < 400 ? 0.3 : 0.0);
        pilot_input.timestamp = std::chrono::system_clock::now();
        
        // Process control through Neuro-FCC
        auto control_surfaces = fcc_core.process_control(current_state, pilot_input);
        
        // Check for failures
        recovery_controller.detect_failure(current_state, control_surfaces);
        
        // Simulate different flight modes
        if (iteration == 100) {
            std::cout << "\n*** Switching to Neuro-Assisted Mode ***" << std::endl;
            fcc_core.set_flight_mode(FlightMode::NEURO_ASSIST);
        }
        
        if (iteration == 300) {
            std::cout << "\n*** Switching to Autopilot Mode ***" << std::endl;
            fcc_core.set_flight_mode(FlightMode::AUTOPILOT);
        }
        
        if (iteration == 500) {
            std::cout << "\n*** Emergency Simulation: High G-Load ***" << std::endl;
            current_state.load_factor = 3.5; // High G-load
            fcc_core.trigger_emergency_mode("HIGH_G_LOAD");
        }
        
        if (iteration == 550) {
            std::cout << "*** Emergency Cleared ***" << std::endl;
            fcc_core.clear_emergency();
        }
        
        if (iteration == 700) {
            std::cout << "\n*** Calibrating Pilot Behavior ***" << std::endl;
            fcc_core.calibrate_pilot_behavior();
        }
        
        // Display status every 100 iterations
        if (iteration % 100 == 0) {
            std::cout << "Iteration " << iteration << ":" << std::endl;
            std::cout << "  Mode: " << static_cast<int>(fcc_core.get_flight_mode()) << std::endl;
            std::cout << "  System Health: " << (fcc_core.is_system_healthy() ? "OK" : "DEGRADED") << std::endl;
            std::cout << "  Confidence: " << fcc_core.get_system_confidence() << std::endl;
            std::cout << "  Learning: " << (fcc_core.is_learning_enabled() ? "Enabled" : "Disabled") << std::endl;
            
            auto warnings = fcc_core.get_system_warnings();
            if (!warnings.empty()) {
                std::cout << "  Warnings: " << warnings.size() << std::endl;
                for (size_t i = 0; i < std::min(warnings.size(), size_t(3)); ++i) {
                    std::cout << "    " << warnings[warnings.size() - 1 - i] << std::endl;
                }
            }
            std::cout << std::endl;
        }
        
        // Simulate real-time control loop (target: <2ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Test failure recovery
    if (recovery_controller.is_failure_detected()) {
        std::cout << "\n*** Executing Recovery Procedure ***" << std::endl;
        recovery_controller.execute_recovery_procedure();
    }
    
    // Final system status
    std::cout << "\n=== Final System Status ===" << std::endl;
    std::cout << "System Health: " << (fcc_core.is_system_healthy() ? "OK" : "DEGRADED") << std::endl;
    std::cout << "Final Confidence: " << fcc_core.get_system_confidence() << std::endl;
    std::cout << "Emergency Active: " << (fcc_core.is_emergency_active() ? "Yes" : "No") << std::endl;
    
    auto final_warnings = fcc_core.get_system_warnings();
    if (!final_warnings.empty()) {
        std::cout << "Final Warnings (" << final_warnings.size() << "):" << std::endl;
        for (const auto& warning : final_warnings) {
            std::cout << "  " << warning << std::endl;
        }
    }
    
    std::cout << "\nNeuro-FCC Demo completed successfully." << std::endl;
    std::cout << "Neural control contour demonstrated biologically-inspired flight control." << std::endl;
    
    return 0;
}
