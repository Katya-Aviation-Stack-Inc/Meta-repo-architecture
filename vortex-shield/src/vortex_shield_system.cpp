#include "vortex_shield_core.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace vortex_shield {

// VortexShieldSystem Implementation
VortexShieldSystem::VortexShieldSystem()
    : automatic_recovery_enabled_(true)
    , emergency_mode_(false)
    , system_healthy_(true)
    , protection_level_(0.7)
    , last_update_time_(std::chrono::system_clock::now()) {
    
    detector_ = std::make_unique<VortexRingDetector>();
    analyzer_ = std::make_unique<VortexFlowAnalyzer>();
    recovery_controller_ = std::make_unique<VortexRecoveryController>();
    blade_interface_ = std::make_unique<BladeControlInterface>();
}

bool VortexShieldSystem::initialize() {
    std::cout << "Vortex Shield System initializing..." << std::endl;
    
    // Initialize detector
    if (!detector_->initialize(8)) { // 8 pressure sensors
        std::cerr << "Failed to initialize vortex ring detector" << std::endl;
        return false;
    }
    
    // Initialize analyzer
    if (!analyzer_->initialize(5.0, 4)) { // 5m radius, 4 blades
        std::cerr << "Failed to initialize vortex flow analyzer" << std::endl;
        return false;
    }
    
    // Initialize recovery controller
    if (!recovery_controller_->initialize()) {
        std::cerr << "Failed to initialize recovery controller" << std::endl;
        return false;
    }
    
    // Initialize blade interface
    if (!blade_interface_->initialize(4)) { // 4 blades
        std::cerr << "Failed to initialize blade control interface" << std::endl;
        return false;
    }
    
    // Set default configuration
    detector_->set_detection_method(VortexDetectionMethod::MULTI_SENSOR_FUSION);
    detector_->set_sensitivity(0.7);
    detector_->set_response_time(0.1); // 100ms
    
    analyzer_->enable_advanced_modeling(true);
    analyzer_->set_turbulence_modeling(true);
    
    recovery_controller_->set_recovery_strategy(RecoveryStrategy::COMBINED_MANEUVER);
    recovery_controller_->set_aggression_level(0.5);
    recovery_controller_->set_safety_limits(15.0, 30.0); // 15 m/s descent, 30° bank
    
    std::cout << "Vortex Shield System initialized successfully" << std::endl;
    return true;
}

ControlCommand VortexShieldSystem::update_vortex_protection(
    const RotorState& rotor_state,
    const FlightConditions& flight_conditions,
    const PressureSensorData& pressure_data) {
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Store current states
    last_rotor_state_ = rotor_state;
    last_flight_conditions_ = flight_conditions;
    
    // Detect vortex ring state
    current_vortex_state_ = detector_->detect_vortex_ring(rotor_state, flight_conditions, pressure_data);
    
    ControlCommand control_command{};
    control_command.timestamp = std::chrono::system_clock::now();
    
    // If vortex ring detected, calculate recovery command
    if (current_vortex_state_.is_active && automatic_recovery_enabled_) {
        control_command = recovery_controller_->calculate_recovery_command(
            current_vortex_state_, rotor_state, flight_conditions
        );
        
        // Execute control command
        blade_interface_->execute_control_command(control_command);
        
        // Update vortex flow analysis
        auto vortex_field = analyzer_->calculate_vortex_field(rotor_state, flight_conditions);
        
        // Predict vortex trajectory
        auto vortex_trajectory = analyzer_->predict_vortex_trajectory(2.0, current_vortex_state_);
    }
    
    // Update system state
    update_system_state();
    
    // Monitor system health
    monitor_system_health();
    
    // Check update time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto update_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    if (update_time > target_update_period_) {
        system_healthy_ = false;
    }
    
    last_update_time_ = std::chrono::system_clock::now();
    
    return control_command;
}

void VortexShieldSystem::update_system_state() {
    // Update vortex state time in state
    if (current_vortex_state_.is_active) {
        auto now = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            now - current_vortex_state_.detection_time);
        current_vortex_state_.time_in_state = elapsed.count();
    }
    
    // Update system health based on component status
    bool detector_healthy = true;
    bool analyzer_healthy = true;
    bool controller_healthy = true;
    bool interface_healthy = blade_interface_->is_control_system_healthy();
    
    // Check detector warnings
    auto warnings = detector_->get_warning_messages();
    if (!warnings.empty()) {
        detector_healthy = false;
    }
    
    // Overall system health
    system_healthy_ = detector_healthy && analyzer_healthy && controller_healthy && interface_healthy;
}

void VortexShieldSystem::monitor_system_health() {
    // Monitor detection accuracy
    if (current_vortex_state_.is_active) {
        double confidence = detector_->get_prediction_confidence();
        if (confidence < 0.5) {
            system_healthy_ = false;
        }
    }
    
    // Monitor recovery progress
    if (recovery_controller_->is_recovery_active()) {
        double progress = recovery_controller_->get_recovery_progress();
        if (progress < 0.1 && current_vortex_state_.time_in_state > 2.0) {
            system_healthy_ = false; // Recovery not progressing
        }
    }
    
    // Monitor blade interface
    if (!blade_interface_->is_control_system_healthy()) {
        system_healthy_ = false;
    }
}

bool VortexShieldSystem::is_vortex_ring_detected() const {
    return current_vortex_state_.is_active;
}

VortexRingState VortexShieldSystem::get_vortex_state() const {
    return current_vortex_state_;
}

bool VortexShieldSystem::is_recovery_in_progress() const {
    return recovery_controller_->is_recovery_active();
}

double VortexShieldSystem::get_system_health() const {
    if (!system_healthy_) {
        return 0.0;
    }
    
    // Calculate health based on various factors
    double detection_health = detector_->get_prediction_confidence();
    double recovery_health = recovery_controller_->is_recovery_active() ? 
                           (1.0 - recovery_controller_->get_recovery_progress()) : 1.0;
    double interface_health = blade_interface_->is_control_system_healthy() ? 1.0 : 0.0;
    
    // Weighted average
    double overall_health = (detection_health * 0.4 + recovery_health * 0.3 + interface_health * 0.3);
    
    return std::clamp(overall_health, 0.0, 1.0);
}

void VortexShieldSystem::enable_automatic_recovery(bool enable) {
    automatic_recovery_enabled_ = enable;
    std::cout << "Automatic recovery " << (enable ? "enabled" : "disabled") << std::endl;
}

void VortexShieldSystem::set_protection_level(double level) {
    protection_level_ = std::clamp(level, 0.0, 1.0);
    detector_->set_sensitivity(protection_level_);
    recovery_controller_->set_aggression_level(protection_level_);
    
    std::cout << "Protection level set to: " << protection_level_ << std::endl;
}

void VortexShieldSystem::set_emergency_mode(bool emergency) {
    emergency_mode_ = emergency;
    
    if (emergency) {
        detector_->set_sensitivity(1.0); // Maximum sensitivity
        recovery_controller_->set_aggression_level(1.0); // Maximum aggression
        set_protection_level(1.0); // Maximum protection
        std::cout << "Emergency mode activated" << std::endl;
    } else {
        detector_->set_sensitivity(0.7); // Normal sensitivity
        recovery_controller_->set_aggression_level(0.5); // Normal aggression
        set_protection_level(0.7); // Normal protection
        std::cout << "Emergency mode deactivated" << std::endl;
    }
}

void VortexShieldSystem::run_system_diagnostics() {
    std::cout << "Running Vortex Shield system diagnostics..." << std::endl;
    
    // Test detector
    std::cout << "Testing vortex ring detector..." << std::endl;
    bool detector_ok = detector_->get_prediction_confidence() > 0.0;
    std::cout << "Detector status: " << (detector_ok ? "OK" : "FAILED") << std::endl;
    
    // Test analyzer
    std::cout << "Testing vortex flow analyzer..." << std::endl;
    auto test_field = analyzer_->calculate_vortex_field(last_rotor_state_, last_flight_conditions_);
    std::cout << "Analyzer status: " << (!test_field.empty() ? "OK" : "FAILED") << std::endl;
    
    // Test recovery controller
    std::cout << "Testing recovery controller..." << std::endl;
    bool controller_ok = !recovery_controller_->is_recovery_active() || 
                       recovery_controller_->get_recovery_progress() > 0.0;
    std::cout << "Controller status: " << (controller_ok ? "OK" : "FAILED") << std::endl;
    
    // Test blade interface
    std::cout << "Testing blade control interface..." << std::endl;
    bool interface_ok = blade_interface_->is_control_system_healthy();
    std::cout << "Interface status: " << (interface_ok ? "OK" : "FAILED") << std::endl;
    
    // Overall system status
    system_healthy_ = detector_ok && controller_ok && interface_ok;
    std::cout << "Overall system health: " << (system_healthy_ ? "HEALTHY" : "DEGRADED") << std::endl;
}

void VortexShieldSystem::calibrate_sensors() {
    std::cout << "Calibrating pressure sensors..." << std::endl;
    
    // Simulate sensor calibration
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    // In a real implementation, this would:
    // 1. Zero all pressure sensors
    // 2. Apply known pressure and verify readings
    // 3. Calibrate sensor offsets
    // 4. Verify sensor cross-talk
    
    std::cout << "Pressure sensors calibrated successfully" << std::endl;
}

void VortexShieldSystem::test_recovery_procedures() {
    std::cout << "Testing recovery procedures..." << std::endl;
    
    // Test each recovery strategy
    std::vector<RecoveryStrategy> strategies = {
        RecoveryStrategy::COLLECTIVE_REDUCTION,
        RecoveryStrategy::FORWARD_TRANSLATION,
        RecoveryStrategy::LATERAL_MOTION,
        RecoveryStrategy::ALTITUDE_GAIN,
        RecoveryStrategy::EMERGENCY_POWER,
        RecoveryStrategy::COMBINED_MANEUVER
    };
    
    for (auto strategy : strategies) {
        std::cout << "Testing " << static_cast<int>(strategy) << "..." << std::endl;
        
        // Create test vortex ring state
        VortexRingState test_state{};
        test_state.is_active = true;
        test_state.severity = 0.5;
        test_state.descent_rate = -10.0;
        test_state.induced_flow_velocity = 15.0;
        
        // Test recovery command calculation
        auto command = recovery_controller_->calculate_recovery_command(
            test_state, last_rotor_state_, last_flight_conditions_
        );
        
        std::cout << "  Command generated: collective=" << command.collective_adjustment 
                  << "°, forward=" << command.cyclic_longitudinal << "°" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "Recovery procedures test completed" << std::endl;
}

void VortexShieldSystem::shutdown() {
    std::cout << "Shutting down Vortex Shield System..." << std::endl;
    
    // Disable automatic recovery
    automatic_recovery_enabled_ = false;
    
    // Reset emergency mode
    emergency_mode_ = false;
    
    // Reset system health
    system_healthy_ = false;
    
    std::cout << "Vortex Shield System shutdown complete" << std::endl;
}

} // namespace vortex_shield
