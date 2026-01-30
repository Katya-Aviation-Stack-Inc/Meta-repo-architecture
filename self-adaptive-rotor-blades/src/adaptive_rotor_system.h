#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>
#include <array>
#include <complex>

namespace adaptive_rotor {

struct BladeElement {
    double radial_position;    // 0.0 to 1.0 (root to tip)
    double chord;             // meters
    double twist;             // degrees
    double pitch_angle;       // degrees
    double flap_angle;        // degrees
    double torsion;           // degrees
    double bend;              // meters
    double velocity;          // m/s
    double angle_of_attack;   // degrees
    double lift_coefficient;
    double drag_coefficient;
    double moment_coefficient;
};

struct RotorState {
    double rotor_speed;       // RPM
    double collective_pitch;  // degrees
    double lateral_cyclic;    // degrees
    double longitudinal_cyclic; // degrees
    double rotor_radius;      // meters
    int num_blades;           // count
    double air_density;       // kg/m³
    double forward_speed;     // m/s
    double vertical_speed;    // m/s
    double roll_angle;        // radians
    double pitch_angle;       // radians
    double yaw_rate;          // rad/s
};

struct ServoCommand {
    int blade_id;
    int element_id;
    double pitch_adjustment;  // degrees
    double flap_deflection;   // degrees
    double torsion_adjustment; // degrees
    std::chrono::system_clock::time_point timestamp;
};

struct VibrationData {
    double frequency;         // Hz
    double amplitude;         // g
    double phase;             // radians
    int blade_id;
    int sensor_id;
};

enum class AdaptationMode {
    PASSIVE,
    ACTIVE_VIBRATION_SUPPRESSION,
    PERFORMANCE_OPTIMIZATION,
    RESONANCE_AVOIDANCE,
    EMERGENCY_ADAPTATION
};

class DSPFilter {
public:
    DSPFilter(int filter_order, double sample_rate);
    ~DSPFilter() = default;
    
    std::vector<double> apply_filter(const std::vector<double>& input);
    void set_filter_coefficients(const std::vector<double>& coefficients);
    void design_bandstop(double center_freq, double bandwidth);
    void design_lowpass(double cutoff_freq);
    void design_highpass(double cutoff_freq);
    
    std::vector<double> get_frequency_response() const;
    double get_phase_delay() const { return phase_delay_; }

private:
    std::vector<double> apply_iir_filter(const std::vector<double>& input);
    std::vector<double> apply_fir_filter(const std::vector<double>& input);
    std::complex<double> evaluate_frequency_response(double frequency) const;
    
    int filter_order_;
    double sample_rate_;
    std::vector<double> coefficients_;
    std::vector<double> delay_line_;
    double phase_delay_;
    bool is_iir_;
};

class AdaptiveController {
public:
    AdaptiveController(int num_blades, int elements_per_blade);
    ~AdaptiveController() = default;
    
    bool initialize();
    void set_adaptation_mode(AdaptationMode mode);
    AdaptationMode get_adaptation_mode() const { return current_mode_; }
    
    std::vector<ServoCommand> calculate_adaptation(
        const RotorState& rotor_state,
        const std::vector<BladeElement>& blade_elements,
        const std::vector<VibrationData>& vibrations
    );
    
    void enable_resonance_detection(bool enable);
    bool is_resonance_detected() const { return resonance_detected_; }
    double get_resonance_frequency() const { return resonance_frequency_; }
    
    void set_performance_target(double target_efficiency);
    double get_current_efficiency() const { return current_efficiency_; }

private:
    void analyze_vibration_spectrum(const std::vector<VibrationData>& vibrations);
    void detect_resonance_conditions(const RotorState& state);
    void optimize_blade_loading(const RotorState& state, std::vector<BladeElement>& elements);
    void suppress_vibrations(const std::vector<VibrationData>& vibrations);
    void calculate_blade_forces(const RotorState& state, std::vector<BladeElement>& elements);
    
    std::vector<ServoCommand> generate_resonance_avoidance_commands();
    std::vector<ServoCommand> generate_emergency_adaptation_commands(const RotorState& state);
    
    int num_blades_;
    int elements_per_blade_;
    AdaptationMode current_mode_;
    bool resonance_detection_enabled_;
    bool resonance_detected_;
    double resonance_frequency_;
    double target_efficiency_;
    double current_efficiency_;
    
    std::unique_ptr<DSPFilter> vibration_filter_;
    std::vector<double> vibration_history_;
    std::vector<std::vector<double>> blade_load_history_;
};

class MicroServoArray {
public:
    MicroServoArray(int num_blades, int servos_per_blade);
    ~MicroServoArray() = default;
    
    bool initialize();
    bool execute_command(const ServoCommand& command);
    std::vector<ServoCommand> execute_batch_commands(const std::vector<ServoCommand>& commands);
    
    void set_max_deflection(double max_degrees);
    void set_max_rate(double max_degrees_per_sec);
    
    bool is_servo_healthy(int blade_id, int servo_id) const;
    std::vector<bool> get_system_health() const { return servo_health_; }
    
    double get_servo_position(int blade_id, int servo_id) const;
    std::vector<double> get_all_positions() const;

private:
    void validate_command(ServoCommand& command);
    void update_servo_dynamics(const ServoCommand& command);
    bool check_rate_limits(const ServoCommand& command);
    
    int num_blades_;
    int servos_per_blade_;
    double max_deflection_;
    double max_rate_;
    std::vector<std::vector<double>> servo_positions_;
    std::vector<std::vector<double>> servo_rates_;
    std::vector<bool> servo_health_;
    std::chrono::system_clock::time_point last_update_time_;
};

class RealTimeOptimizer {
public:
    RealTimeOptimizer();
    ~RealTimeOptimizer() = default;
    
    void set_optimization_target(const std::string& target);
    void set_constraints(const std::vector<std::pair<std::string, double>>& constraints);
    
    std::vector<ServoCommand> optimize_blade_configuration(
        const RotorState& current_state,
        const std::vector<BladeElement>& current_elements,
        const std::vector<VibrationData>& vibrations
    );
    
    double calculate_objective_function(
        const std::vector<BladeElement>& elements,
        const std::vector<VibrationData>& vibrations
    );
    
    bool is_optimization_converged() const { return optimization_converged_; }
    int get_optimization_iterations() const { return optimization_iterations_; }

private:
    std::vector<ServoCommand> gradient_descent_optimization(
        const RotorState& state,
        std::vector<BladeElement>& elements
    );
    
    std::vector<ServoCommand> genetic_algorithm_optimization(
        const RotorState& state,
        std::vector<BladeElement>& elements
    );
    
    double calculate_blade_efficiency(const std::vector<BladeElement>& elements);
    double calculate_vibration_level(const std::vector<VibrationData>& vibrations);
    double calculate_structural_load(const std::vector<BladeElement>& elements);
    
    std::string optimization_target_;
    std::vector<std::pair<std::string, double>> constraints_;
    bool optimization_converged_;
    int optimization_iterations_;
    double convergence_threshold_;
};

class AdaptiveRotorSystem {
public:
    AdaptiveRotorSystem(int num_blades = 4, int elements_per_blade = 20);
    ~AdaptiveRotorSystem() = default;
    
    bool initialize();
    void shutdown();
    
    // Main control loop - must run at high frequency (1kHz+)
    std::vector<ServoCommand> update_rotor_control(
        const RotorState& rotor_state,
        const std::vector<VibrationData>& vibration_sensors
    );
    
    // Configuration
    void set_adaptation_mode(AdaptationMode mode);
    void set_automatic_optimization(bool enable);
    void set_emergency_mode(bool emergency);
    
    // Monitoring
    bool is_system_healthy() const;
    double get_system_efficiency() const;
    std::vector<VibrationData> get_vibration_levels() const;
    std::vector<std::string> get_system_warnings() const;
    
    // Diagnostics
    void run_system_diagnostics();
    void calibrate_sensors();
    void perform_resonance_scan();

private:
    void update_blade_elements(const RotorState& state);
    void monitor_system_health();
    void log_performance_data();
    void handle_system_faults();
    
    int num_blades_;
    int elements_per_blade_;
    std::unique_ptr<AdaptiveController> controller_;
    std::unique_ptr<MicroServoArray> servo_array_;
    std::unique_ptr<RealTimeOptimizer> optimizer_;
    
    std::vector<BladeElement> blade_elements_;
    std::vector<VibrationData> current_vibrations_;
    std::vector<std::string> system_warnings_;
    
    bool automatic_optimization_enabled_;
    bool emergency_mode_;
    bool system_healthy_;
    double system_efficiency_;
    
    std::chrono::system_clock::time_point last_update_time_;
    std::chrono::microseconds target_update_period_{1000}; // 1ms target
};

} // namespace adaptive_rotor
