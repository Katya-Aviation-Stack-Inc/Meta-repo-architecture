// Self-Adaptive Rotor Blades - Main Entry Point
// Placeholder for adaptive rotor control algorithms

#include "adaptive_rotor_system.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace adaptive_rotor;

int main() {
    std::cout << "=== Self-Adaptive Rotor Blades Demo ===" << std::endl;
    
    // Initialize adaptive rotor system (4 blades, 20 elements per blade)
    AdaptiveRotorSystem rotor_system(4, 20);
    
    if (!rotor_system.initialize()) {
        std::cerr << "Failed to initialize adaptive rotor system" << std::endl;
        return 1;
    }
    
    std::cout << "Starting rotor blade adaptation simulation..." << std::endl;
    
    // Simulation parameters
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> speed_dist(200.0, 400.0); // RPM
    std::uniform_real_distribution<> pitch_dist(0.0, 15.0); // degrees
    std::uniform_real_distribution<> vibration_freq_dist(10.0, 100.0); // Hz
    std::uniform_real_distribution<> vibration_amp_dist(0.1, 3.0); // g
    std::uniform_real_distribution<> airspeed_dist(0.0, 50.0); // m/s
    
    RotorState rotor_state{};
    std::vector<VibrationData> vibration_sensors;
    
    // Main simulation loop
    for (int iteration = 0; iteration < 500; ++iteration) {
        // Simulate rotor state
        rotor_state.rotor_speed = speed_dist(gen);
        rotor_state.collective_pitch = pitch_dist(gen);
        rotor_state.lateral_cyclic = (iteration % 100 - 50) * 0.1; // Sinusoidal
        rotor_state.longitudinal_cyclic = std::sin(iteration * 0.1) * 5.0;
        rotor_state.rotor_radius = 7.0; // meters
        rotor_state.num_blades = 4;
        rotor_state.air_density = 1.225; // kg/m³
        rotor_state.forward_speed = airspeed_dist(gen);
        rotor_state.vertical_speed = (iteration % 200 - 100) * 0.1;
        rotor_state.roll_angle = std::sin(iteration * 0.05) * 0.2; // radians
        rotor_state.pitch_angle = std::cos(iteration * 0.03) * 0.1;
        rotor_state.yaw_rate = std::sin(iteration * 0.02) * 0.1;
        
        // Generate vibration sensor data
        vibration_sensors.clear();
        int num_sensors = 16; // 4 sensors per blade
        
        for (int sensor = 0; sensor < num_sensors; ++sensor) {
            VibrationData vib_data;
            vib_data.frequency = vibration_freq_dist(gen);
            vib_data.amplitude = vibration_amp_dist(gen);
            vib_data.phase = (sensor * M_PI / 8.0);
            vib_data.blade_id = sensor / 4;
            vib_data.sensor_id = sensor % 4;
            
            // Add resonance at iteration 250
            if (iteration >= 250 && iteration <= 300) {
                if (vib_data.frequency > 45.0 && vib_data.frequency < 55.0) {
                    vib_data.amplitude *= 3.0; // Amplify resonance
                }
            }
            
            vibration_sensors.push_back(vib_data);
        }
        
        // Update rotor control
        auto servo_commands = rotor_system.update_rotor_control(rotor_state, vibration_sensors);
        
        // Simulate different adaptation modes
        if (iteration == 50) {
            std::cout << "\n*** Switching to Active Vibration Suppression ***" << std::endl;
            rotor_system.set_adaptation_mode(AdaptationMode::ACTIVE_VIBRATION_SUPPRESSION);
        }
        
        if (iteration == 150) {
            std::cout << "\n*** Switching to Performance Optimization ***" << std::endl;
            rotor_system.set_adaptation_mode(AdaptationMode::PERFORMANCE_OPTIMIZATION);
        }
        
        if (iteration == 250) {
            std::cout << "\n*** Resonance Condition Detected ***" << std::endl;
            rotor_system.set_adaptation_mode(AdaptationMode::RESONANCE_AVOIDANCE);
        }
        
        if (iteration == 350) {
            std::cout << "\n*** Emergency Adaptation Mode ***" << std::endl;
            rotor_system.set_adaptation_mode(AdaptationMode::EMERGENCY_ADAPTATION);
        }
        
        if (iteration == 400) {
            std::cout << "\n*** Returning to Normal Operation ***" << std::endl;
            rotor_system.set_adaptation_mode(AdaptationMode::ACTIVE_VIBRATION_SUPPRESSION);
        }
        
        // Display status every 50 iterations
        if (iteration % 50 == 0) {
            std::cout << "Iteration " << iteration << ":" << std::endl;
            std::cout << "  Rotor Speed: " << rotor_state.rotor_speed << " RPM" << std::endl;
            std::cout << "  System Health: " << (rotor_system.is_system_healthy() ? "OK" : "DEGRADED") << std::endl;
            std::cout << "  Efficiency: " << rotor_system.get_system_efficiency() << std::endl;
            std::cout << "  Servo Commands: " << servo_commands.size() << std::endl;
            
            auto vibrations = rotor_system.get_vibration_levels();
            if (!vibrations.empty()) {
                double max_vibration = 0.0;
                for (const auto& vib : vibrations) {
                    max_vibration = std::max(max_vibration, vib.amplitude);
                }
                std::cout << "  Max Vibration: " << max_vibration << "g" << std::endl;
            }
            std::cout << std::endl;
        }
        
        // Simulate real-time control loop (target: 1ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Run system diagnostics
    std::cout << "\n*** Running System Diagnostics ***" << std::endl;
    rotor_system.run_system_diagnostics();
    
    // Calibrate sensors
    std::cout << "\n*** Calibrating Sensors ***" << std::endl;
    rotor_system.calibrate_sensors();
    
    // Perform resonance scan
    std::cout << "\n*** Performing Resonance Scan ***" << std::endl;
    rotor_system.perform_resonance_scan();
    
    // Final system status
    std::cout << "\n=== Final System Status ===" << std::endl;
    std::cout << "System Health: " << (rotor_system.is_system_healthy() ? "OK" : "DEGRADED") << std::endl;
    std::cout << "Final Efficiency: " << rotor_system.get_system_efficiency() << std::endl;
    
    auto warnings = rotor_system.get_system_warnings();
    if (!warnings.empty()) {
        std::cout << "System Warnings (" << warnings.size() << "):" << std::endl;
        for (const auto& warning : warnings) {
            std::cout << "  " << warning << std::endl;
        }
    }
    
    std::cout << "\nSelf-Adaptive Rotor Blades Demo completed successfully." << std::endl;
    std::cout << "Demonstrated real-time blade adaptation, vibration suppression, and resonance avoidance." << std::endl;
    
    return 0;
}
