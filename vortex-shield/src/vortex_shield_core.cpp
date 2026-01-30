#include "vortex_shield_core.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

namespace vortex_shield {

// Physical constants
constexpr double AIR_DENSITY_SEA_LEVEL = 1.225; // kg/m³
constexpr double GRAVITY = 9.80665; // m/s²

// VortexRingDetector Implementation
VortexRingDetector::VortexRingDetector()
    : detection_method_(VortexDetectionMethod::MULTI_SENSOR_FUSION)
    , sensitivity_(0.7)
    , response_time_(0.1) // 100ms
    , prediction_confidence_(0.0) {
    
    pressure_history_.reserve(100);
    descent_rate_history_.reserve(100);
}

bool VortexRingDetector::initialize(int num_pressure_sensors) {
    std::cout << "Vortex Ring Detector initializing with " << num_pressure_sensors << " sensors" << std::endl;
    
    pressure_history_.clear();
    descent_rate_history_.clear();
    
    std::cout << "Vortex Ring Detector initialized successfully" << std::endl;
    return true;
}

VortexRingState VortexRingDetector::detect_vortex_ring(
    const RotorState& rotor_state,
    const FlightConditions& flight_conditions,
    const PressureSensorData& pressure_data) {
    
    VortexRingState state{};
    state.detection_time = std::chrono::system_clock::now();
    
    // Initialize state
    state.is_active = false;
    state.severity = 0.0;
    state.descent_rate = flight_conditions.vertical_speed;
    state.induced_flow_velocity = calculate_induced_flow_velocity(rotor_state, flight_conditions);
    state.vortex_core_radius = rotor_state.rotor_radius * 0.3; // Approximate
    state.circulation_strength = 0.0;
    state.time_in_state = 0.0;
    
    // Detection based on selected method
    bool detected = false;
    
    switch (detection_method_) {
        case VortexDetectionMethod::PRESSURE_ANALYSIS:
            detected = detect_from_pressure(pressure_data);
            break;
        case VortexDetectionMethod::DESCENT_RATE_ANALYSIS:
            detected = detect_from_descent_rate(flight_conditions);
            break;
        case VortexDetectionMethod::INDUCED_FLOW_CALCULATION:
            detected = detect_from_induced_flow(rotor_state, flight_conditions);
            break;
        case VortexDetectionMethod::MULTI_SENSOR_FUSION:
            detected = detect_from_pressure(pressure_data) ||
                     detect_from_descent_rate(flight_conditions) ||
                     detect_from_induced_flow(rotor_state, flight_conditions);
            break;
        default:
            break;
    }
    
    if (detected) {
        state.is_active = true;
        
        // Calculate severity based on multiple factors
        double descent_factor = std::abs(state.descent_rate) / 10.0; // Normalize by 10 m/s
        double induced_flow_factor = state.induced_flow_velocity / 20.0; // Normalize by 20 m/s
        double descent_rate_threshold = calculate_descent_rate_threshold(flight_conditions);
        double threshold_factor = std::abs(state.descent_rate) / descent_rate_threshold;
        
        state.severity = std::clamp((descent_factor + induced_flow_factor + threshold_factor) / 3.0, 0.0, 1.0);
        
        // Calculate circulation strength (simplified)
        state.circulation_strength = state.induced_flow_velocity * 2.0 * M_PI * state.vortex_core_radius;
        
        warning_messages_.push_back("VORTEX RING STATE DETECTED");
        warning_messages_.push_back("Severity: " + std::to_string(state.severity));
        warning_messages_.push_back("Descent rate: " + std::to_string(state.descent_rate) + " m/s");
        
        prediction_confidence_ = 0.85; // High confidence when detected
    } else {
        // Check for imminent vortex ring
        double imminent_threshold = descent_rate_threshold * 0.8;
        if (std::abs(state.descent_rate) > imminent_threshold) {
            state.is_active = true;
            state.severity = 0.3; // Low severity for imminent
            warning_messages_.push_back("VORTEX RING IMMINENT");
            prediction_confidence_ = 0.6; // Moderate confidence
        }
    }
    
    current_state_ = state;
    last_analysis_time_ = std::chrono::system_clock::now();
    
    return state;
}

bool VortexRingDetector::detect_from_pressure(const PressureSensorData& data) {
    // Analyze pressure distribution around rotor
    bool pressure_anomaly = analyze_pressure_distribution(data);
    
    // Check for characteristic pressure pattern of vortex ring
    double pressure_variance = 0.0;
    double mean_pressure = 0.0;
    
    for (double pressure : data.sensor_readings) {
        mean_pressure += pressure;
    }
    mean_pressure /= data.sensor_readings.size();
    
    for (double pressure : data.sensor_readings) {
        pressure_variance += (pressure - mean_pressure) * (pressure - mean_pressure);
    }
    pressure_variance /= data.sensor_readings.size();
    
    // High pressure variance indicates vortex formation
    return pressure_anomaly && (pressure_variance > 100.0); // Threshold in Pa²
}

bool VortexRingDetector::detect_from_descent_rate(const FlightConditions& conditions) {
    // Check if descent rate exceeds critical threshold
    double threshold = calculate_descent_rate_threshold(conditions);
    
    // Add to history
    descent_rate_history_.push_back(conditions.vertical_speed);
    if (descent_rate_history_.size() > 100) {
        descent_rate_history_.erase(descent_rate_history_.begin());
    }
    
    return std::abs(conditions.vertical_speed) > threshold;
}

bool VortexRingDetector::detect_from_induced_flow(const RotorState& rotor, const FlightConditions& flight) {
    // Calculate induced flow velocity
    double induced_flow = calculate_induced_flow_velocity(rotor, flight);
    
    // Check if induced flow is insufficient (characteristic of vortex ring)
    double required_induced_flow = std::sqrt(2 * GRAVITY * flight.air_density / (M_PI * rotor.rotor_radius * rotor.rotor_radius));
    
    return induced_flow < required_induced_flow * 0.7; // 70% threshold
}

double VortexRingDetector::calculate_induced_flow_velocity(const RotorState& rotor, const FlightConditions& flight) {
    // Simplified induced flow calculation
    double disc_area = M_PI * rotor.rotor_radius * rotor.rotor_radius;
    double rotor_thrust = rotor.disc_loading * disc_area * flight.air_density * GRAVITY;
    
    // Induced velocity = sqrt(T / (2 * rho * A))
    double induced_velocity = std::sqrt(rotor_thrust / (2 * flight.air_density * disc_area));
    
    return induced_velocity;
}

double VortexRingDetector::calculate_descent_rate_threshold(const FlightConditions& conditions) {
    // Calculate critical descent rate based on flight conditions
    // Simplified model: V_crit = 2 * V_induced
    
    RotorState dummy_rotor{}; // Would need actual rotor state
    dummy_rotor.rotor_radius = 5.0; // Assume 5m radius
    dummy_rotor.disc_loading = 0.1; // Assume 0.1 disc loading
    
    double induced_flow = calculate_induced_flow_velocity(dummy_rotor, conditions);
    double critical_descent_rate = 2.0 * induced_flow;
    
    // Adjust for air density
    double density_factor = conditions.air_density / AIR_DENSITY_SEA_LEVEL;
    critical_descent_rate *= std::sqrt(density_factor);
    
    return critical_descent_rate;
}

bool VortexRingDetector::analyze_pressure_distribution(const PressureSensorData& data) {
    // Check for characteristic pressure pattern of vortex ring
    // In vortex ring state, pressure is lower in the core and higher in the periphery
    
    double center_pressure = data.sensor_readings[0]; // Assume center sensor
    double periphery_pressure = 0.0;
    
    for (size_t i = 1; i < data.sensor_readings.size(); ++i) {
        periphery_pressure += data.sensor_readings[i];
    }
    periphery_pressure /= (data.sensor_readings.size() - 1);
    
    // In vortex ring, center pressure < periphery pressure
    double pressure_difference = periphery_pressure - center_pressure;
    
    return pressure_difference > 50.0; // 50 Pa threshold
}

// VortexFlowAnalyzer Implementation
VortexFlowAnalyzer::VortexFlowAnalyzer()
    : rotor_radius_(5.0)
    , num_blades_(4)
    , advanced_modeling_enabled_(false)
    , turbulence_modeling_enabled_(false) {
    
    vortex_cores_.reserve(8);
    vortex_strengths_.reserve(8);
    induced_velocities_.reserve(100);
}

bool VortexFlowAnalyzer::initialize(double rotor_radius, int num_blades) {
    rotor_radius_ = rotor_radius;
    num_blades_ = num_blades;
    
    std::cout << "Vortex Flow Analyzer initializing..." << std::endl;
    std::cout << "Rotor radius: " << rotor_radius_ << "m, Blades: " << num_blades_ << std::endl;
    
    return true;
}

std::vector<std::array<double, 3>> VortexFlowAnalyzer::calculate_vortex_field(
    const RotorState& rotor_state,
    const FlightConditions& flight_conditions) {
    
    std::vector<std::array<double, 3>> vortex_field;
    
    // Calculate vortex core positions
    calculate_vortex_core_positions(rotor_state);
    
    // Compute induced velocities
    compute_vortex_induced_velocities();
    
    // Create vortex field grid (simplified)
    int grid_size = 20;
    for (int i = 0; i < grid_size; ++i) {
        for (int j = 0; j < grid_size; ++j) {
            for (int k = 0; k < 5; ++k) { // 5 vertical levels
                std::array<double, 3> point = {
                    (i - grid_size/2) * rotor_radius_ / grid_size * 2,
                    (j - grid_size/2) * rotor_radius_ / grid_size * 2,
                    k * rotor_radius_ / 4
                };
                
                // Calculate induced velocity at this point
                std::array<double, 3> induced_vel = {0.0, 0.0, 0.0};
                
                for (size_t v = 0; v < vortex_cores_.size(); ++v) {
                    double dx = point[0] - vortex_cores_[v][0];
                    double dy = point[1] - vortex_cores_[v][1];
                    double dz = point[2] - vortex_cores_[v][2];
                    double r = std::sqrt(dx*dx + dy*dy + dz*dz);
                    
                    if (r > 0.1) { // Avoid singularity
                        double theta = std::atan2(dy, dx);
                        double v_theta = vortex_strengths_[v] / (2 * M_PI * r);
                        
                        induced_vel[0] -= v_theta * std::sin(theta);
                        induced_vel[1] += v_theta * std::cos(theta);
                        induced_vel[2] += vortex_strengths_[v] * 0.1 / r; // Vertical component
                    }
                }
                
                vortex_field.push_back(induced_vel);
            }
        }
    }
    
    return vortex_field;
}

void VortexFlowAnalyzer::calculate_vortex_core_positions(const RotorState& rotor) {
    vortex_cores_.clear();
    
    // Simplified vortex core positions (would be more complex in reality)
    for (int i = 0; i < num_blades_; ++i) {
        double angle = 2.0 * M_PI * i / num_blades_;
        std::array<double, 3> core_pos = {
            rotor.rotor_radius * 0.7 * std::cos(angle),
            rotor.rotor_radius * 0.7 * std::sin(angle),
            -2.0 // Below rotor disc
        };
        vortex_cores_.push_back(core_pos);
        
        // Calculate vortex strength based on rotor conditions
        double blade_tip_speed = rotor.main_rotor_rpm * 2.0 * M_PI / 60.0 * rotor.rotor_radius;
        double circulation = blade_tip_speed * rotor.rotor_radius * 0.1; // Simplified
        vortex_strengths_.push_back(circulation);
    }
}

void VortexFlowAnalyzer::compute_vortex_induced_velocities() {
    induced_velocities_.clear();
    
    // Calculate induced velocities at various points
    for (const auto& core : vortex_cores_) {
        for (int i = 0; i < 20; ++i) {
            double r = 0.5 + i * 0.5; // Distance from core
            double v_theta = 10.0 / r; // Simplified induced velocity
            
            std::array<double, 3> vel = {v_theta, 0.0, 0.0};
            induced_velocities_.push_back(vel);
        }
    }
}

// VortexRecoveryController Implementation
VortexRecoveryController::VortexRecoveryController()
    : current_strategy_(RecoveryStrategy::COMBINED_MANEUVER)
    , aggression_level_(0.5)
    , max_descent_rate_(15.0) // m/s
    , max_bank_angle_(30.0) // degrees
    , recovery_active_(false)
    , recovery_progress_(0.0) {
    
    recovery_status_.reserve(10);
}

bool VortexRecoveryController::initialize() {
    std::cout << "Vortex Recovery Controller initializing..." << std::endl;
    std::cout << "Aggression level: " << aggression_level_ << std::endl;
    std::cout << "Max descent rate: " << max_descent_rate_ << " m/s" << std::endl;
    
    return true;
}

ControlCommand VortexRecoveryController::calculate_recovery_command(
    const VortexRingState& vortex_state,
    const RotorState& current_rotor,
    const FlightConditions& flight_conditions) {
    
    ControlCommand command{};
    command.timestamp = std::chrono::system_clock::now();
    
    if (!vortex_state.is_active) {
        recovery_active_ = false;
        recovery_progress_ = 0.0;
        return command;
    }
    
    recovery_active_ = true;
    recovery_start_time_ = std::chrono::system_clock::now();
    
    // Select recovery strategy based on severity
    if (vortex_state.severity > 0.7) {
        current_strategy_ = RecoveryStrategy::EMERGENCY_POWER;
    } else if (vortex_state.severity > 0.4) {
        current_strategy_ = RecoveryStrategy::COMBINED_MANEUVER;
    } else {
        current_strategy_ = RecoveryStrategy::FORWARD_TRANSLATION;
    }
    
    // Execute recovery strategy
    switch (current_strategy_) {
        case RecoveryStrategy::COLLECTIVE_REDUCTION:
            command = execute_collective_reduction(vortex_state);
            break;
        case RecoveryStrategy::FORWARD_TRANSLATION:
            command = execute_forward_translation(vortex_state);
            break;
        case RecoveryStrategy::LATERAL_MOTION:
            command = execute_lateral_motion(vortex_state);
            break;
        case RecoveryStrategy::ALTITUDE_GAIN:
            command = execute_altitude_gain(vortex_state);
            break;
        case RecoveryStrategy::EMERGENCY_POWER:
            command = execute_emergency_power(vortex_state);
            break;
        case RecoveryStrategy::COMBINED_MANEUVER:
            command = execute_combined_maneuver(vortex_state);
            break;
    }
    
    // Update recovery progress
    if (recovery_active_) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - recovery_start_time_);
        
        recovery_progress_ = std::min(1.0, elapsed.count() / 5000.0); // 5 second recovery
    }
    
    return command;
}

ControlCommand VortexRecoveryController::execute_combined_maneuver(const VortexRingState& vortex) {
    ControlCommand command{};
    
    // Combined maneuver: reduce collective + forward translation + lateral motion
    command.collective_adjustment = -3.0 * aggression_level_; // Reduce collective
    command.cyclic_lateral = 5.0 * aggression_level_; // Lateral cyclic
    command.cyclic_longitudinal = 8.0 * aggression_level_; // Forward cyclic
    command.pedal_adjustment = 2.0 * aggression_level_; // Anti-torque pedal
    
    // Calculate thrust vector
    command.thrust_vector_x = 100.0 * aggression_level_; // Forward thrust
    command.thrust_vector_y = 50.0 * aggression_level_;  // Lateral thrust
    command.thrust_vector_z = 200.0 * aggression_level_; // Upward thrust
    
    command.emergency_recovery = (vortex.severity > 0.6);
    
    recovery_status_.push_back("Executing combined recovery maneuver");
    recovery_status_.push_back("Collective: " + std::to_string(command.collective_adjustment) + "°");
    recovery_status_.push_back("Forward cyclic: " + std::to_string(command.cyclic_longitudinal) + "°");
    
    return command;
}

ControlCommand VortexRecoveryController::execute_forward_translation(const VortexRingState& vortex) {
    ControlCommand command{};
    
    // Forward translation to escape vortex ring
    command.collective_adjustment = -1.0 * aggression_level_; // Slight collective reduction
    command.cyclic_longitudinal = 10.0 * aggression_level_; // Strong forward cyclic
    command.cyclic_lateral = 0.0;
    command.pedal_adjustment = 0.0;
    
    command.thrust_vector_x = 150.0 * aggression_level_;
    command.thrust_vector_y = 0.0;
    command.thrust_vector_z = 100.0 * aggression_level_;
    
    command.emergency_recovery = false;
    
    recovery_status_.push_back("Executing forward translation");
    
    return command;
}

ControlCommand VortexRecoveryController::execute_emergency_power(const VortexRingState& vortex) {
    ControlCommand command{};
    
    // Emergency power: maximum collective + aggressive cyclic
    command.collective_adjustment = 5.0; // Maximum collective
    command.cyclic_longitudinal = 15.0 * aggression_level_; // Aggressive forward
    command.cyclic_lateral = 10.0 * aggression_level_; // Aggressive lateral
    command.pedal_adjustment = 5.0 * aggression_level_;
    
    command.thrust_vector_x = 200.0 * aggression_level_;
    command.thrust_vector_y = 150.0 * aggression_level_;
    command.thrust_vector_z = 300.0 * aggression_level_;
    
    command.emergency_recovery = true;
    
    recovery_status_.push_back("EMERGENCY POWER ACTIVATED");
    recovery_status_.push_back("Maximum collective and cyclic inputs");
    
    return command;
}

// BladeControlInterface Implementation
BladeControlInterface::BladeControlInterface()
    : num_blades_(4)
    , current_collective_(0.0)
    , current_lateral_cyclic_(0.0)
    , current_longitudinal_cyclic_(0.0)
    , current_pedal_(0.0)
    , system_healthy_(true) {
    
    blade_pitches_.resize(num_blades_, 0.0);
}

bool BladeControlInterface::initialize(int num_blades) {
    num_blades_ = num_blades;
    blade_pitches_.resize(num_blades_, 0.0);
    
    std::cout << "Blade Control Interface initializing with " << num_blades_ << " blades" << std::endl;
    
    return true;
}

bool BladeControlInterface::execute_control_command(const ControlCommand& command) {
    // Apply collective adjustment
    double new_collective = current_collective_ + command.collective_adjustment;
    new_collective = std::clamp(new_collective, -10.0, 20.0); // Limit range
    
    // Apply cyclic inputs
    double new_lateral = current_lateral_cyclic_ + command.cyclic_lateral;
    double new_longitudinal = current_longitudinal_cyclic_ + command.cyclic_longitudinal;
    double new_pedal = current_pedal_ + command.pedal_adjustment;
    
    // Limit cyclic inputs
    new_lateral = std::clamp(new_lateral, -15.0, 15.0);
    new_longitudinal = std::clamp(new_longitudinal, -15.0, 15.0);
    new_pedal = std::clamp(new_pedal, -25.0, 25.0);
    
    // Apply rate limiting
    double max_rate = 5.0; // degrees per 100ms
    auto now = std::chrono::system_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_command_time_);
    
    if (dt.count() > 0) {
        double dt_sec = dt.count() / 1000.0;
        double max_change = max_rate * dt_sec;
        
        new_collective = std::clamp(new_collective, 
                                  current_collective_ - max_change, 
                                  current_collective_ + max_change);
        new_lateral = std::clamp(new_lateral, 
                             current_lateral_cyclic_ - max_change, 
                             current_lateral_cyclic_ + max_change);
        new_longitudinal = std::clamp(new_longitudinal, 
                                 current_longitudinal_cyclic_ - max_change, 
                                 current_longitudinal_cyclic_ + max_change);
        new_pedal = std::clamp(new_pedal, 
                            current_pedal_ - max_change, 
                            current_pedal + max_change);
    }
    
    // Update blade pitches based on collective and cyclic inputs
    for (int i = 0; i < num_blades_; ++i) {
        double blade_angle = new_collective;
        
        // Add cyclic inputs based on blade position
        double blade_azimuth = 2.0 * M_PI * i / num_blades_;
        blade_angle += new_lateral * std::sin(blade_azimuth);
        blade_angle += new_longitudinal * std::cos(blade_azimuth);
        
        blade_pitches_[i] = std::clamp(blade_angle, 0.0, 30.0); // Limit blade pitch range
    }
    
    // Update current values
    current_collective_ = new_collective;
    current_lateral_cyclic_ = new_lateral;
    current_longitudinal_cyclic_ = new_longitudinal;
    current_pedal_ = new_pedal;
    
    last_command_time_ = now;
    
    return true;
}

} // namespace vortex_shield
