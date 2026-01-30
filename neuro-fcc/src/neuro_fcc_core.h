#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>
#include <array>

namespace neuro_fcc {

struct FlightControlSurface {
    double aileron_left;      // -1.0 to 1.0
    double aileron_right;     // -1.0 to 1.0
    double elevator;          // -1.0 to 1.0
    double rudder;            // -1.0 to 1.0
    double throttle;          // 0.0 to 1.0
    double flaps;             // 0.0 to 1.0
    double spoilers;          // 0.0 to 1.0
};

struct AircraftState {
    double roll;              // radians
    double pitch;             // radians
    double yaw;               // radians
    double roll_rate;         // rad/s
    double pitch_rate;        // rad/s
    double yaw_rate;          // rad/s
    double airspeed;          // m/s
    double altitude;          // meters
    double vertical_speed;    // m/s
    double bank_angle;        // radians
    double load_factor;       // g
};

struct PilotInput {
    double stick_x;           // -1.0 to 1.0
    double stick_y;           // -1.0 to 1.0
    double rudder_pedal;      // -1.0 to 1.0
    double throttle_lever;    // 0.0 to 1.0
    double flap_lever;        // 0.0 to 1.0
    std::chrono::system_clock::time_point timestamp;
};

struct TrajectoryCommand {
    double desired_roll;
    double desired_pitch;
    double desired_yaw;
    double desired_airspeed;
    double desired_altitude;
    double desired_vertical_speed;
    std::chrono::system_clock::time_point timestamp;
};

enum class FlightMode {
    MANUAL,
    ASSISTED,
    AUTOPILOT,
    EMERGENCY,
    NEURO_ASSIST
};

class NeuralNetwork {
public:
    NeuralNetwork(int input_size, int hidden_size, int output_size);
    ~NeuralNetwork() = default;
    
    std::vector<double> forward(const std::vector<double>& input);
    void train(const std::vector<double>& input, const std::vector<double>& target, double learning_rate = 0.01);
    void load_weights(const std::vector<std::vector<double>>& weights);
    std::vector<std::vector<double>> get_weights() const;
    
    void reset_state();
    bool is_trained() const { return trained_; }

private:
    std::vector<double> sigmoid(const std::vector<double>& x);
    std::vector<double> tanh_activation(const std::vector<double>& x);
    double sigmoid(double x);
    double tanh_func(double x);
    
    int input_size_;
    int hidden_size_;
    int output_size_;
    std::vector<std::vector<double>> weights_input_hidden_;
    std::vector<std::vector<double>> weights_hidden_output_;
    std::vector<double> hidden_bias_;
    std::vector<double> output_bias_;
    bool trained_;
};

class PilotBehaviorLearner {
public:
    PilotBehaviorLearner();
    ~PilotBehaviorLearner() = default;
    
    void record_pilot_input(const PilotInput& input, const AircraftState& state);
    void learn_from_session();
    std::vector<double> predict_control_input(const AircraftState& current_state);
    
    void set_pilot_aggression(double aggression_factor); // 0.0 to 1.0
    double get_pilot_aggression() const { return aggression_factor_; }
    
    void save_behavior_model(const std::string& filename);
    void load_behavior_model(const std::string& filename);
    
    int get_training_samples() const { return training_samples_; }
    bool is_model_ready() const { return training_samples_ > 1000; }

private:
    std::vector<PilotInput> input_history_;
    std::vector<AircraftState> state_history_;
    std::unique_ptr<NeuralNetwork> behavior_network_;
    double aggression_factor_;
    int training_samples_;
    bool model_trained_;
    
    std::vector<double> extract_features(const AircraftState& state);
    std::vector<double> normalize_input(const PilotInput& input);
};

class NeuroFCCCore {
public:
    NeuroFCCCore();
    ~NeuroFCCCore() = default;
    
    bool initialize();
    void set_flight_mode(FlightMode mode);
    FlightMode get_flight_mode() const { return current_mode_; }
    
    // Main control loop - must be called at high frequency (<2ms latency)
    FlightControlSurface process_control(
        const AircraftState& current_state,
        const PilotInput& pilot_input,
        const TrajectoryCommand& trajectory_command = {}
    );
    
    // Emergency handling
    void trigger_emergency_mode(const std::string& emergency_type);
    bool is_emergency_active() const { return emergency_active_; }
    void clear_emergency();
    
    // Learning and adaptation
    void enable_learning(bool enable);
    bool is_learning_enabled() const { return learning_enabled_; }
    void calibrate_pilot_behavior();
    
    // System health monitoring
    bool is_system_healthy() const;
    double get_system_confidence() const { return system_confidence_; }
    std::vector<std::string> get_system_warnings() const;
    
    // Accessors for testing
    PilotBehaviorLearner* get_behavior_learner() const { return behavior_learner_.get(); }

private:
    FlightControlSurface manual_control(const AircraftState& state, const PilotInput& input);
    FlightControlSurface assisted_control(const AircraftState& state, const PilotInput& input);
    FlightControlSurface autopilot_control(const AircraftState& state, const TrajectoryCommand& command);
    FlightControlSurface neuro_assisted_control(const AircraftState& state, const PilotInput& input);
    FlightControlSurface emergency_control(const AircraftState& state);
    
    void validate_control_surfaces(FlightControlSurface& surfaces);
    void update_system_health(const AircraftState& state, const FlightControlSurface& surfaces);
    void detect_control_anomalies(const FlightControlSurface& surfaces);
    
    FlightMode current_mode_;
    std::unique_ptr<PilotBehaviorLearner> behavior_learner_;
    bool learning_enabled_;
    bool emergency_active_;
    std::string emergency_type_;
    double system_confidence_;
    std::vector<std::string> system_warnings_;
    
    std::chrono::system_clock::time_point last_update_time_;
    std::chrono::microseconds target_latency_{2000}; // 2ms target
    
    // Control surface limits
    static constexpr double MAX_CONTROL_DEFLECTION = 1.0;
    static constexpr double MAX_RATE_LIMIT = 0.5; // rad/s
};

class FailureRecoveryController {
public:
    FailureRecoveryController(NeuroFCCCore& fcc_core);
    ~FailureRecoveryController() = default;
    
    void detect_failure(const AircraftState& state, const FlightControlSurface& commanded_surfaces);
    bool is_failure_detected() const { return failure_detected_; }
    void execute_recovery_procedure();
    
    void set_failure_tolerance(double tolerance) { failure_tolerance_ = tolerance; }
    double get_failure_tolerance() const { return failure_tolerance_; }

private:
    void analyze_control_surface_effectiveness(const AircraftState& state, const FlightControlSurface& surfaces);
    void compensate_for_failed_surfaces();
    void execute_emergency_landing_pattern();
    
    NeuroFCCCore& fcc_core_;
    bool failure_detected_;
    std::string failure_type_;
    double failure_tolerance_;
    std::chrono::system_clock::time_point failure_detection_time_;
    std::vector<bool> surface_health_status_;
};

} // namespace neuro_fcc
