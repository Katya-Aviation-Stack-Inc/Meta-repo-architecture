#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>
#include <array>
#include <complex>

namespace vortex_shield {

struct RotorState {
    double main_rotor_rpm;        // Main rotor RPM
    double tail_rotor_rpm;         // Tail rotor RPM
    double collective_pitch;       // degrees
    double lateral_cyclic;         // degrees
    double longitudinal_cyclic;    // degrees
    double pedal_position;         // degrees
    double rotor_radius;           // meters
    int num_blades;                // count
    double disc_loading;           // ratio
    double blade_twist;            // degrees
};

struct FlightConditions {
    double airspeed;               // m/s
    double vertical_speed;         // m/s (positive = up, negative = down)
    double altitude;               // meters
    double air_density;            // kg/m³
    double temperature;            // Kelvin
    double wind_speed;             // m/s
    double wind_direction;          // degrees
    double turbulence_intensity;    // 0-1
    double pressure_altitude;       // meters
};

struct VortexRingState {
    bool is_active;                // Vortex ring state detected
    double severity;                // 0-1 (how severe)
    double descent_rate;            // m/s
    double induced_flow_velocity;   // m/s
    double vortex_core_radius;      // meters
    double circulation_strength;    // m²/s
    double time_in_state;           // seconds
    std::chrono::system_clock::time_point detection_time;
};

struct PressureSensorData {
    double pressure;                // Pascal
    double pressure_gradient;       // Pa/m
    double dynamic_pressure;        // Pascal
    double static_pressure;         // Pascal
    std::array<double, 8> sensor_readings; // 8 pressure sensors
    std::chrono::system_clock::time_point timestamp;
};

struct ControlCommand {
    double collective_adjustment;   // degrees
    double cyclic_lateral;          // degrees
    double cyclic_longitudinal;     // degrees
    double pedal_adjustment;        // degrees
    double thrust_vector_x;          // N
    double thrust_vector_y;          // N
    double thrust_vector_z;          // N
    bool emergency_recovery;         // true if emergency maneuver
    std::chrono::system_clock::time_point timestamp;
};

enum class VortexDetectionMethod {
    PRESSURE_ANALYSIS,
    FLOW_FIELD_MONITORING,
    INDUCED_FLOW_CALCULATION,
    DESCENT_RATE_ANALYSIS,
    VIBRATION_PATTERN,
    MULTI_SENSOR_FUSION
};

enum class RecoveryStrategy {
    COLLECTIVE_REDUCTION,
    FORWARD_TRANSLATION,
    LATERAL_MOTION,
    ALTITUDE_GAIN,
    EMERGENCY_POWER,
    COMBINED_MANEUVER
};

class VortexRingDetector {
public:
    VortexRingDetector();
    ~VortexRingDetector() = default;
    
    bool initialize(int num_pressure_sensors);
    VortexRingState detect_vortex_ring(
        const RotorState& rotor_state,
        const FlightConditions& flight_conditions,
        const PressureSensorData& pressure_data
    );
    
    void set_detection_method(VortexDetectionMethod method);
    void set_sensitivity(double sensitivity); // 0-1
    void set_response_time(double seconds);
    
    bool is_vortex_imminent() const;
    double get_prediction_confidence() const;
    std::vector<std::string> get_warning_messages() const;

private:
    bool detect_from_pressure(const PressureSensorData& data);
    bool detect_from_descent_rate(const FlightConditions& conditions);
    bool detect_from_induced_flow(const RotorState& rotor, const FlightConditions& flight);
    bool detect_from_vibration_pattern(const std::vector<double>& vibration_data);
    
    double calculate_induced_flow_velocity(const RotorState& rotor, const FlightConditions& flight);
    double calculate_descent_rate_threshold(const FlightConditions& conditions);
    bool analyze_pressure_distribution(const PressureSensorData& data);
    
    VortexDetectionMethod detection_method_;
    double sensitivity_;
    double response_time_;
    VortexRingState current_state_;
    std::vector<std::string> warning_messages_;
    double prediction_confidence_;
    
    std::vector<double> pressure_history_;
    std::vector<double> descent_rate_history_;
    std::chrono::system_clock::time_point last_analysis_time_;
};

class VortexFlowAnalyzer {
public:
    VortexFlowAnalyzer();
    ~VortexFlowAnalyzer() = default;
    
    bool initialize(double rotor_radius, int num_blades);
    std::vector<std::array<double, 3>> calculate_vortex_field(
        const RotorState& rotor_state,
        const FlightConditions& flight_conditions
    );
    
    double calculate_circulation_strength(const VortexRingState& vortex_state);
    double predict_vortex_growth_rate(const VortexRingState& current_state);
    std::vector<std::array<double, 3>> predict_vortex_trajectory(
        double time_ahead, const VortexRingState& current_state
    );
    
    void enable_advanced_modeling(bool enable);
    void set_turbulence_modeling(bool enable);

private:
    void calculate_vortex_core_positions(const RotorState& rotor);
    void compute_vortex_induced_velocities();
    void model_vortex_interaction();
    
    double rotor_radius_;
    int num_blades_;
    bool advanced_modeling_enabled_;
    bool turbulence_modeling_enabled_;
    
    std::vector<std::array<double, 3>> vortex_cores_;
    std::vector<double> vortex_strengths_;
    std::vector<std::array<double, 3>> induced_velocities_;
};

class VortexRecoveryController {
public:
    VortexRecoveryController();
    ~VortexRecoveryController() = default;
    
    bool initialize();
    ControlCommand calculate_recovery_command(
        const VortexRingState& vortex_state,
        const RotorState& current_rotor,
        const FlightConditions& flight_conditions
    );
    
    void set_recovery_strategy(RecoveryStrategy strategy);
    void set_aggression_level(double aggression); // 0-1
    void set_safety_limits(double max_descent_rate, double max_bank_angle);
    
    bool is_recovery_active() const;
    double get_recovery_progress() const;
    std::vector<std::string> get_recovery_status() const;

private:
    ControlCommand execute_collective_reduction(const VortexRingState& vortex);
    ControlCommand execute_forward_translation(const VortexRingState& vortex);
    ControlCommand execute_lateral_motion(const VortexRingState& vortex);
    ControlCommand execute_altitude_gain(const VortexRingState& vortex);
    ControlCommand execute_emergency_power(const VortexRingState& vortex);
    ControlCommand execute_combined_maneuver(const VortexRingState& vortex);
    
    RecoveryStrategy current_strategy_;
    double aggression_level_;
    double max_descent_rate_;
    double max_bank_angle_;
    
    bool recovery_active_;
    double recovery_progress_;
    std::vector<std::string> recovery_status_;
    std::chrono::system_clock::time_point recovery_start_time_;
};

class BladeControlInterface {
public:
    BladeControlInterface();
    ~BladeControlInterface() = default;
    
    bool initialize(int num_blades);
    bool execute_control_command(const ControlCommand& command);
    bool set_blade_pitch(int blade_id, double pitch_angle);
    bool set_collective_pitch(double collective);
    bool set_cyclic_inputs(double lateral, double longitudinal);
    
    std::vector<double> get_current_blade_pitches() const;
    double get_collective_pitch() const;
    bool is_control_system_healthy() const;

private:
    void validate_control_limits(ControlCommand& command);
    void apply_rate_limiting(ControlCommand& command);
    void monitor_actuator_response();
    
    int num_blades_;
    std::vector<double> blade_pitches_;
    double current_collective_;
    double current_lateral_cyclic_;
    double current_longitudinal_cyclic_;
    double current_pedal_;
    
    bool system_healthy_;
    std::vector<std::string> system_warnings_;
    std::chrono::system_clock::time_point last_command_time_;
};

class VortexShieldSystem {
public:
    VortexShieldSystem();
    ~VortexShieldSystem() = default;
    
    bool initialize();
    void shutdown();
    
    // Main control loop - must run at high frequency (100Hz+)
    ControlCommand update_vortex_protection(
        const RotorState& rotor_state,
        const FlightConditions& flight_conditions,
        const PressureSensorData& pressure_data
    );
    
    // Configuration
    void enable_automatic_recovery(bool enable);
    void set_protection_level(double level); // 0-1
    void set_emergency_mode(bool emergency);
    
    // Monitoring
    bool is_vortex_ring_detected() const;
    VortexRingState get_vortex_state() const;
    bool is_recovery_in_progress() const;
    double get_system_health() const;
    
    // Diagnostics
    void run_system_diagnostics();
    void calibrate_sensors();
    void test_recovery_procedures();

private:
    void update_system_state();
    void monitor_system_health();
    void log_performance_data();
    void handle_system_faults();
    
    std::unique_ptr<VortexRingDetector> detector_;
    std::unique_ptr<VortexFlowAnalyzer> analyzer_;
    std::unique_ptr<VortexRecoveryController> recovery_controller_;
    std::unique_ptr<BladeControlInterface> blade_interface_;
    
    VortexRingState current_vortex_state_;
    RotorState last_rotor_state_;
    FlightConditions last_flight_conditions_;
    
    bool automatic_recovery_enabled_;
    bool emergency_mode_;
    bool system_healthy_;
    double protection_level_;
    
    std::chrono::system_clock::time_point last_update_time_;
    std::chrono::microseconds target_update_period_{10000}; // 10ms target
};

} // namespace vortex_shield
