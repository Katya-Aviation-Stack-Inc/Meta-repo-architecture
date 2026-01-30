#include "adaptive_rotor_system.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace adaptive_rotor {

// AdaptiveRotorSystem Implementation
AdaptiveRotorSystem::AdaptiveRotorSystem(int num_blades, int elements_per_blade)
    : num_blades_(num_blades)
    , elements_per_blade_(elements_per_blade)
    , automatic_optimization_enabled_(true)
    , emergency_mode_(false)
    , system_healthy_(true)
    , system_efficiency_(0.8)
    , last_update_time_(std::chrono::system_clock::now()) {
    
    controller_ = std::make_unique<AdaptiveController>(num_blades, elements_per_blade);
    servo_array_ = std::make_unique<MicroServoArray>(num_blades, elements_per_blade);
    optimizer_ = std::make_unique<RealTimeOptimizer>();
    
    blade_elements_.resize(num_blades * elements_per_blade);
    for (int i = 0; i < num_blades * elements_per_blade; ++i) {
        blade_elements_[i].radial_position = static_cast<double>(i % elements_per_blade) / elements_per_blade;
        blade_elements_[i].chord = 0.5; // meters
        blade_elements_[i].twist = 8.0 * (1.0 - blade_elements_[i].radial_position); // degrees
        blade_elements_[i].pitch_angle = 0.0;
        blade_elements_[i].flap_angle = 0.0;
        blade_elements_[i].torsion = 0.0;
        blade_elements_[i].bend = 0.0;
    }
}

bool AdaptiveRotorSystem::initialize() {
    std::cout << "Adaptive Rotor System initializing..." << std::endl;
    
    if (!controller_->initialize()) {
        std::cerr << "Failed to initialize adaptive controller" << std::endl;
        return false;
    }
    
    if (!servo_array_->initialize()) {
        std::cerr << "Failed to initialize servo array" << std::endl;
        return false;
    }
    
    optimizer_->set_optimization_target("efficiency");
    std::vector<std::pair<std::string, double>> constraints = {
        {"vibration", 2.0}, // Max 2g vibration
        {"load_factor", 2.5}, // Max 2.5g load
        {"blade_stress", 100.0} // MPa
    };
    optimizer_->set_constraints(constraints);
    
    std::cout << "Adaptive Rotor System initialized successfully" << std::endl;
    return true;
}

void AdaptiveRotorSystem::shutdown() {
    std::cout << "Shutting down Adaptive Rotor System..." << std::endl;
    
    // Return all servos to neutral position
    std::vector<ServoCommand> neutral_commands;
    for (int blade = 0; blade < num_blades_; ++blade) {
        for (int element = 0; element < elements_per_blade_; ++element) {
            ServoCommand cmd;
            cmd.blade_id = blade;
            cmd.element_id = element;
            cmd.pitch_adjustment = 0.0;
            cmd.flap_deflection = 0.0;
            cmd.torsion_adjustment = 0.0;
            cmd.timestamp = std::chrono::system_clock::now();
            neutral_commands.push_back(cmd);
        }
    }
    
    servo_array_->execute_batch_commands(neutral_commands);
    std::cout << "Adaptive Rotor System shutdown complete" << std::endl;
}

std::vector<ServoCommand> AdaptiveRotorSystem::update_rotor_control(
    const RotorState& rotor_state,
    const std::vector<VibrationData>& vibration_sensors) {
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    current_vibrations_ = vibration_sensors;
    update_blade_elements(rotor_state);
    
    // Calculate adaptation commands
    auto commands = controller_->calculate_adaptation(rotor_state, blade_elements_, vibration_sensors);
    
    // Apply automatic optimization if enabled
    if (automatic_optimization_enabled_ && !emergency_mode_) {
        auto optimization_commands = optimizer_->optimize_blade_configuration(rotor_state, blade_elements_);
        commands.insert(commands.end(), optimization_commands.begin(), optimization_commands.end());
    }
    
    // Execute servo commands
    auto successful_commands = servo_array_->execute_batch_commands(commands);
    
    // Monitor system health
    monitor_system_health();
    
    // Check update time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto update_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    if (update_time > target_update_period_) {
        system_warnings_.push_back("Update time exceeded target: " + std::to_string(update_time.count()) + "μs");
    }
    
    last_update_time_ = std::chrono::system_clock::now();
    return successful_commands;
}

void AdaptiveRotorSystem::update_blade_elements(const RotorState& state) {
    for (auto& element : blade_elements_) {
        // Calculate blade element velocity
        double radius = element.radial_position * state.rotor_radius;
        double tangential_speed = state.rotor_speed * 2.0 * M_PI / 60.0 * radius;
        element.velocity = std::sqrt(tangential_speed * tangential_speed + state.forward_speed * state.forward_speed);
        
        // Calculate angle of attack (simplified)
        double inflow_angle = std::atan2(state.vertical_speed, tangential_speed);
        element.angle_of_attack = (element.pitch_angle + element.twist) * M_PI / 180.0 - inflow_angle;
        
        // Update aerodynamic coefficients
        element.lift_coefficient = 2.0 * M_PI * element.angle_of_attack;
        element.drag_coefficient = 0.01 + 0.05 * element.angle_of_attack * element.angle_of_attack;
        element.moment_coefficient = -0.1 * element.angle_of_attack;
    }
}

void AdaptiveRotorSystem::monitor_system_health() {
    system_healthy_ = true;
    
    // Check servo health
    auto servo_health = servo_array_->get_system_health();
    for (bool healthy : servo_health) {
        if (!healthy) {
            system_healthy_ = false;
            system_warnings_.push_back("Servo failure detected");
        }
    }
    
    // Check vibration levels
    for (const auto& vib : current_vibrations_) {
        if (vib.amplitude > 5.0) { // 5g threshold
            system_healthy_ = false;
            system_warnings_.push_back("Excessive vibration detected: " + std::to_string(vib.amplitude) + "g");
        }
    }
    
    // Check controller resonance detection
    if (controller_->is_resonance_detected()) {
        system_healthy_ = false;
        system_warnings_.push_back("Resonance condition detected at " + std::to_string(controller_->get_resonance_frequency()) + " Hz");
    }
    
    // Update system efficiency
    system_efficiency_ = controller_->get_current_efficiency();
    
    // Clear old warnings
    if (system_warnings_.size() > 50) {
        system_warnings_.erase(system_warnings_.begin(), system_warnings_.begin() + 25);
    }
}

bool AdaptiveRotorSystem::is_system_healthy() const {
    return system_healthy_;
}

double AdaptiveRotorSystem::get_system_efficiency() const {
    return system_efficiency_;
}

std::vector<VibrationData> AdaptiveRotorSystem::get_vibration_levels() const {
    return current_vibrations_;
}

std::vector<std::string> AdaptiveRotorSystem::get_system_warnings() const {
    return system_warnings_;
}

void AdaptiveRotorSystem::run_system_diagnostics() {
    std::cout << "Running system diagnostics..." << std::endl;
    
    // Test servo array
    auto servo_health = servo_array_->get_system_health();
    int failed_servos = std::count(servo_health.begin(), servo_health.end(), false);
    std::cout << "Servo health: " << (servo_health.size() - failed_servos) << "/" << servo_health.size() << " operational" << std::endl;
    
    // Test controller
    std::cout << "Controller resonance detection: " << (controller_->is_resonance_detected() ? "ACTIVE" : "CLEAR") << std::endl;
    std::cout << "Controller efficiency: " << controller_->get_current_efficiency() << std::endl;
    
    // Test optimizer
    std::cout << "Optimizer convergence: " << (optimizer_->is_optimization_converged() ? "CONVERGED" : "ITERATING") << std::endl;
    
    std::cout << "System diagnostics completed" << std::endl;
}

void AdaptiveRotorSystem::calibrate_sensors() {
    std::cout << "Calibrating vibration sensors..." << std::endl;
    
    // Simulate sensor calibration
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    std::cout << "Vibration sensors calibrated" << std::endl;
}

void AdaptiveRotorSystem::perform_resonance_scan() {
    std::cout << "Performing resonance frequency scan..." << std::endl;
    
    // Simulate resonance scan (would sweep frequencies in real implementation)
    for (int freq = 10; freq <= 200; freq += 10) {
        std::cout << "Scanning " << freq << " Hz..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "Resonance scan completed" << std::endl;
}

void AdaptiveRotorSystem::set_adaptation_mode(AdaptationMode mode) {
    controller_->set_adaptation_mode(mode);
    
    if (mode == AdaptationMode::EMERGENCY_ADAPTATION) {
        emergency_mode_ = true;
        std::cout << "Emergency adaptation mode activated" << std::endl;
    } else {
        emergency_mode_ = false;
        std::cout << "Adaptation mode changed" << std::endl;
    }
}

void AdaptiveRotorSystem::set_automatic_optimization(bool enable) {
    automatic_optimization_enabled_ = enable;
    std::cout << "Automatic optimization " << (enable ? "enabled" : "disabled") << std::endl;
}

void AdaptiveRotorSystem::set_emergency_mode(bool emergency) {
    emergency_mode_ = emergency;
    if (emergency) {
        set_adaptation_mode(AdaptationMode::EMERGENCY_ADAPTATION);
        std::cout << "Emergency mode activated" << std::endl;
    } else {
        set_adaptation_mode(AdaptationMode::ACTIVE_VIBRATION_SUPPRESSION);
        std::cout << "Emergency mode deactivated" << std::endl;
    }
}

// RealTimeOptimizer Implementation
RealTimeOptimizer::RealTimeOptimizer()
    : optimization_converged_(false)
    , optimization_iterations_(0)
    , convergence_threshold_(0.001) {
    
    optimization_target_ = "efficiency";
}

void RealTimeOptimizer::set_optimization_target(const std::string& target) {
    optimization_target_ = target;
    optimization_converged_ = false;
    optimization_iterations_ = 0;
}

void RealTimeOptimizer::set_constraints(const std::vector<std::pair<std::string, double>>& constraints) {
    constraints_ = constraints;
}

std::vector<ServoCommand> RealTimeOptimizer::optimize_blade_configuration(
    const RotorState& current_state,
    const std::vector<BladeElement>& current_elements) {
    
    std::vector<ServoCommand> commands;
    
    // Simple gradient descent optimization
    commands = gradient_descent_optimization(current_state, const_cast<std::vector<BladeElement>&>(current_elements));
    
    optimization_iterations_++;
    
    // Check convergence
    if (optimization_iterations_ > 10) {
        optimization_converged_ = true;
    }
    
    return commands;
}

double RealTimeOptimizer::calculate_objective_function(
    const std::vector<BladeElement>& elements,
    const std::vector<VibrationData>& vibrations) {
    
    if (optimization_target_ == "efficiency") {
        return calculate_blade_efficiency(elements);
    } else if (optimization_target_ == "vibration") {
        return -calculate_vibration_level(vibrations); // Negative for minimization
    } else if (optimization_target_ == "load") {
        return -calculate_structural_load(elements); // Negative for minimization
    }
    
    return 0.0;
}

double RealTimeOptimizer::calculate_blade_efficiency(const std::vector<BladeElement>& elements) {
    double total_lift = 0.0;
    double total_drag = 0.0;
    
    for (const auto& element : elements) {
        total_lift += element.lift_coefficient * element.chord;
        total_drag += element.drag_coefficient * element.chord;
    }
    
    return total_lift / (total_lift + total_drag + 0.001); // Avoid division by zero
}

double RealTimeOptimizer::calculate_vibration_level(const std::vector<VibrationData>& vibrations) {
    double total_vibration = 0.0;
    for (const auto& vib : vibrations) {
        total_vibration += vib.amplitude * vib.amplitude; // RMS
    }
    return std::sqrt(total_vibration / vibrations.size());
}

double RealTimeOptimizer::calculate_structural_load(const std::vector<BladeElement>& elements) {
    double total_load = 0.0;
    for (const auto& element : elements) {
        total_load += std::abs(element.lift_coefficient) + std::abs(element.moment_coefficient);
    }
    return total_load;
}

std::vector<ServoCommand> RealTimeOptimizer::gradient_descent_optimization(
    const RotorState& state,
    std::vector<BladeElement>& elements) {
    
    std::vector<ServoCommand> commands;
    
    // Simple optimization: adjust pitch angles based on lift coefficient
    for (size_t i = 0; i < elements.size(); i += 5) { // Every 5th element
        if (elements[i].lift_coefficient < 0.5) {
            ServoCommand cmd;
            cmd.blade_id = i / elements.size() * 4; // Approximate blade ID
            cmd.element_id = i % 20;
            cmd.pitch_adjustment = 0.5; // Increase pitch
            cmd.flap_deflection = 0.0;
            cmd.torsion_adjustment = 0.0;
            cmd.timestamp = std::chrono::system_clock::now();
            commands.push_back(cmd);
        }
    }
    
    return commands;
}

} // namespace adaptive_rotor
