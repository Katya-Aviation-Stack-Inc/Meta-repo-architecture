#include "neuro_fcc_core.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <fstream>

namespace neuro_fcc {

// Neural Network Implementation
NeuralNetwork::NeuralNetwork(int input_size, int hidden_size, int output_size)
    : input_size_(input_size)
    , hidden_size_(hidden_size)
    , output_size_(output_size)
    , trained_(false) {
    
    // Initialize weights with random values
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-0.5, 0.5);
    
    weights_input_hidden_.resize(hidden_size, std::vector<double>(input_size));
    weights_hidden_output_.resize(output_size, std::vector<double>(hidden_size));
    hidden_bias_.resize(hidden_size);
    output_bias_.resize(output_size);
    
    for (int i = 0; i < hidden_size; ++i) {
        for (int j = 0; j < input_size; ++j) {
            weights_input_hidden_[i][j] = dis(gen);
        }
        hidden_bias_[i] = dis(gen);
    }
    
    for (int i = 0; i < output_size; ++i) {
        for (int j = 0; j < hidden_size; ++j) {
            weights_hidden_output_[i][j] = dis(gen);
        }
        output_bias_[i] = dis(gen);
    }
}

std::vector<double> NeuralNetwork::forward(const std::vector<double>& input) {
    if (input.size() != input_size_) {
        return std::vector<double>(output_size_, 0.0);
    }
    
    // Hidden layer
    std::vector<double> hidden(hidden_size_);
    for (int i = 0; i < hidden_size_; ++i) {
        double sum = hidden_bias_[i];
        for (int j = 0; j < input_size_; ++j) {
            sum += weights_input_hidden_[i][j] * input[j];
        }
        hidden[i] = tanh_func(sum);
    }
    
    // Output layer
    std::vector<double> output(output_size_);
    for (int i = 0; i < output_size_; ++i) {
        double sum = output_bias_[i];
        for (int j = 0; j < hidden_size_; ++j) {
            sum += weights_hidden_output_[i][j] * hidden[j];
        }
        output[i] = sigmoid(sum);
    }
    
    return output;
}

void NeuralNetwork::train(const std::vector<double>& input, const std::vector<double>& target, double learning_rate) {
    if (input.size() != input_size_ || target.size() != output_size_) {
        return;
    }
    
    // Forward pass
    std::vector<double> hidden(hidden_size_);
    std::vector<double> output(output_size_);
    
    // Calculate hidden layer
    for (int i = 0; i < hidden_size_; ++i) {
        double sum = hidden_bias_[i];
        for (int j = 0; j < input_size_; ++j) {
            sum += weights_input_hidden_[i][j] * input[j];
        }
        hidden[i] = tanh_func(sum);
    }
    
    // Calculate output layer
    for (int i = 0; i < output_size_; ++i) {
        double sum = output_bias_[i];
        for (int j = 0; j < hidden_size_; ++j) {
            sum += weights_hidden_output_[i][j] * hidden[j];
        }
        output[i] = sigmoid(sum);
    }
    
    // Backpropagation (simplified)
    std::vector<double> output_error(output_size_);
    for (int i = 0; i < output_size_; ++i) {
        double error = target[i] - output[i];
        output_error[i] = error * output[i] * (1.0 - output[i]);
    }
    
    // Update hidden-output weights
    for (int i = 0; i < output_size_; ++i) {
        for (int j = 0; j < hidden_size_; ++j) {
            weights_hidden_output_[i][j] += learning_rate * output_error[i] * hidden[j];
        }
        output_bias_[i] += learning_rate * output_error[i];
    }
    
    trained_ = true;
}

std::vector<double> NeuralNetwork::sigmoid(const std::vector<double>& x) {
    std::vector<double> result(x.size());
    for (size_t i = 0; i < x.size(); ++i) {
        result[i] = sigmoid(x[i]);
    }
    return result;
}

std::vector<double> NeuralNetwork::tanh_activation(const std::vector<double>& x) {
    std::vector<double> result(x.size());
    for (size_t i = 0; i < x.size(); ++i) {
        result[i] = tanh_func(x[i]);
    }
    return result;
}

double NeuralNetwork::sigmoid(double x) {
    return 1.0 / (1.0 + std::exp(-x));
}

double NeuralNetwork::tanh_func(double x) {
    return std::tanh(x);
}

void NeuralNetwork::reset_state() {
    trained_ = false;
}

// Pilot Behavior Learner Implementation
PilotBehaviorLearner::PilotBehaviorLearner()
    : aggression_factor_(0.5)
    , training_samples_(0)
    , model_trained_(false) {
    
    behavior_network_ = std::make_unique<NeuralNetwork>(10, 20, 5); // state -> control mapping
}

void PilotBehaviorLearner::record_pilot_input(const PilotInput& input, const AircraftState& state) {
    input_history_.push_back(input);
    state_history_.push_back(state);
    training_samples_++;
    
    // Keep history manageable
    if (input_history_.size() > 10000) {
        input_history_.erase(input_history_.begin());
        state_history_.erase(state_history_.begin());
    }
    
    // Train network periodically
    if (training_samples_ % 100 == 0 && training_samples_ > 500) {
        learn_from_session();
    }
}

void PilotBehaviorLearner::learn_from_session() {
    if (input_history_.size() < 100) return;
    
    // Train on recent samples
    int start_idx = std::max(0, static_cast<int>(input_history_.size()) - 1000);
    
    for (int i = start_idx; i < input_history_.size(); ++i) {
        auto features = extract_features(state_history_[i]);
        auto target = normalize_input(input_history_[i]);
        
        behavior_network_->train(features, target, 0.01);
    }
    
    model_trained_ = true;
    std::cout << "Neuro-FCC: Trained on " << (input_history_.size() - start_idx) << " samples" << std::endl;
}

std::vector<double> PilotBehaviorLearner::predict_control_input(const AircraftState& current_state) {
    if (!model_trained_) {
        return std::vector<double>(5, 0.0); // Neutral input
    }
    
    auto features = extract_features(current_state);
    return behavior_network_->forward(features);
}

void PilotBehaviorLearner::set_pilot_aggression(double aggression_factor) {
    aggression_factor_ = std::clamp(aggression_factor, 0.0, 1.0);
}

std::vector<double> PilotBehaviorLearner::extract_features(const AircraftState& state) {
    std::vector<double> features(10);
    
    features[0] = state.roll;
    features[1] = state.pitch;
    features[2] = state.yaw;
    features[3] = state.roll_rate;
    features[4] = state.pitch_rate;
    features[5] = state.yaw_rate;
    features[6] = state.airspeed / 100.0; // Normalize
    features[7] = state.altitude / 10000.0; // Normalize
    features[8] = state.vertical_speed / 10.0; // Normalize
    features[9] = state.load_factor;
    
    return features;
}

std::vector<double> PilotBehaviorLearner::normalize_input(const PilotInput& input) {
    std::vector<double> normalized(5);
    
    normalized[0] = input.stick_x;
    normalized[1] = input.stick_y;
    normalized[2] = input.rudder_pedal;
    normalized[3] = input.throttle_lever;
    normalized[4] = input.flap_lever;
    
    return normalized;
}

// NeuroFCCCore Implementation
NeuroFCCCore::NeuroFCCCore()
    : current_mode_(FlightMode::MANUAL)
    , learning_enabled_(true)
    , emergency_active_(false)
    , system_confidence_(1.0)
    , last_update_time_(std::chrono::system_clock::now()) {
    
    behavior_learner_ = std::make_unique<PilotBehaviorLearner>();
}

bool NeuroFCCCore::initialize() {
    std::cout << "Neuro-FCC Core initializing..." << std::endl;
    
    // Initialize behavior learner
    behavior_learner_->set_pilot_aggression(0.5);
    
    // Clear any emergency state
    emergency_active_ = false;
    system_warnings_.clear();
    system_confidence_ = 1.0;
    
    std::cout << "Neuro-FCC Core initialized successfully" << std::endl;
    return true;
}

void NeuroFCCCore::set_flight_mode(FlightMode mode) {
    if (mode == FlightMode::EMERGENCY) {
        trigger_emergency_mode("MANUAL_EMERGENCY_ACTIVATION");
    } else {
        current_mode_ = mode;
        emergency_active_ = false;
        std::cout << "Flight mode changed to: " << static_cast<int>(mode) << std::endl;
    }
}

FlightControlSurface NeuroFCCCore::process_control(
    const AircraftState& current_state,
    const PilotInput& pilot_input,
    const TrajectoryCommand& trajectory_command) {
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    FlightControlSurface surfaces{};
    
    switch (current_mode_) {
        case FlightMode::MANUAL:
            surfaces = manual_control(current_state, pilot_input);
            break;
        case FlightMode::ASSISTED:
            surfaces = assisted_control(current_state, pilot_input);
            break;
        case FlightMode::AUTOPILOT:
            surfaces = autopilot_control(current_state, trajectory_command);
            break;
        case FlightMode::NEURO_ASSIST:
            surfaces = neuro_assisted_control(current_state, pilot_input);
            break;
        case FlightMode::EMERGENCY:
            surfaces = emergency_control(current_state);
            break;
    }
    
    // Validate control surfaces
    validate_control_surfaces(surfaces);
    
    // Update system health
    update_system_health(current_state, surfaces);
    
    // Record pilot behavior for learning
    if (learning_enabled_ && current_mode_ == FlightMode::MANUAL) {
        behavior_learner_->record_pilot_input(pilot_input, current_state);
    }
    
    // Check latency
    auto end_time = std::chrono::high_resolution_clock::now();
    auto latency = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    if (latency > target_latency_) {
        system_warnings_.push_back("High latency detected: " + std::to_string(latency.count()) + "μs");
        system_confidence_ *= 0.99;
    }
    
    last_update_time_ = std::chrono::system_clock::now();
    return surfaces;
}

FlightControlSurface NeuroFCCCore::manual_control(const AircraftState& state, const PilotInput& input) {
    FlightControlSurface surfaces{};
    
    // Direct pilot input mapping
    surfaces.aileron_left = input.stick_x;
    surfaces.aileron_right = -input.stick_x;
    surfaces.elevator = -input.stick_y;
    surfaces.rudder = input.rudder_pedal;
    surfaces.throttle = input.throttle_lever;
    surfaces.flaps = input.flap_lever;
    surfaces.spoilers = 0.0;
    
    return surfaces;
}

FlightControlSurface NeuroFCCCore::assisted_control(const AircraftState& state, const PilotInput& input) {
    FlightControlSurface surfaces = manual_control(state, input);
    
    // Add envelope protection
    if (state.load_factor > 2.5) {
        surfaces.elevator *= 0.5; // Reduce elevator authority
        system_warnings_.push_back("High G-load protection activated");
    }
    
    if (state.airspeed < 50.0) { // Low speed protection
        surfaces.flaps = std::max(surfaces.flaps, 0.3);
        system_warnings_.push_back("Low speed protection activated");
    }
    
    return surfaces;
}

FlightControlSurface NeuroFCCCore::neuro_assisted_control(const AircraftState& state, const PilotInput& input) {
    FlightControlSurface surfaces = manual_control(state, input);
    
    if (behavior_learner_->is_model_ready()) {
        auto predicted_input = behavior_learner_->predict_control_input(state);
        
        // Blend pilot input with learned behavior
        double blend_factor = 0.3; // 30% neural assistance
        
        surfaces.aileron_left = (1.0 - blend_factor) * surfaces.aileron_left + blend_factor * predicted_input[0];
        surfaces.aileron_right = (1.0 - blend_factor) * surfaces.aileron_right - blend_factor * predicted_input[0];
        surfaces.elevator = (1.0 - blend_factor) * surfaces.elevator - blend_factor * predicted_input[1];
        surfaces.rudder = (1.0 - blend_factor) * surfaces.rudder + blend_factor * predicted_input[2];
        surfaces.throttle = (1.0 - blend_factor) * surfaces.throttle + blend_factor * predicted_input[3];
    }
    
    return surfaces;
}

FlightControlSurface NeuroFCCCore::emergency_control(const AircraftState& state) {
    FlightControlSurface surfaces{};
    
    // Emergency recovery: wings level, gentle climb
    surfaces.aileron_left = -state.roll * 0.5; // Wings level
    surfaces.aileron_right = state.roll * 0.5;
    surfaces.elevator = 0.1; // Gentle climb
    surfaces.rudder = 0.0;
    surfaces.throttle = 0.7; // Moderate power
    surfaces.flaps = 0.2; // Some flaps for stability
    surfaces.spoilers = 0.0;
    
    system_warnings_.push_back("Emergency control mode active");
    
    return surfaces;
}

void NeuroFCCCore::validate_control_surfaces(FlightControlSurface& surfaces) {
    // Limit control surface deflections
    surfaces.aileron_left = std::clamp(surfaces.aileron_left, -MAX_CONTROL_DEFLECTION, MAX_CONTROL_DEFLECTION);
    surfaces.aileron_right = std::clamp(surfaces.aileron_right, -MAX_CONTROL_DEFLECTION, MAX_CONTROL_DEFLECTION);
    surfaces.elevator = std::clamp(surfaces.elevator, -MAX_CONTROL_DEFLECTION, MAX_CONTROL_DEFLECTION);
    surfaces.rudder = std::clamp(surfaces.rudder, -MAX_CONTROL_DEFLECTION, MAX_CONTROL_DEFLECTION);
    surfaces.throttle = std::clamp(surfaces.throttle, 0.0, 1.0);
    surfaces.flaps = std::clamp(surfaces.flaps, 0.0, 1.0);
    surfaces.spoilers = std::clamp(surfaces.spoilers, 0.0, 1.0);
}

void NeuroFCCCore::trigger_emergency_mode(const std::string& emergency_type) {
    emergency_active_ = true;
    emergency_type_ = emergency_type;
    current_mode_ = FlightMode::EMERGENCY;
    
    system_warnings_.push_back("EMERGENCY: " + emergency_type);
    system_confidence_ *= 0.5;
    
    std::cout << "Emergency mode triggered: " << emergency_type << std::endl;
}

void NeuroFCCCore::clear_emergency() {
    emergency_active_ = false;
    current_mode_ = FlightMode::ASSISTED;
    
    std::cout << "Emergency cleared, returning to assisted mode" << std::endl;
}

void NeuroFCCCore::enable_learning(bool enable) {
    learning_enabled_ = enable;
    std::cout << "Neural learning " << (enable ? "enabled" : "disabled") << std::endl;
}

bool NeuroFCCCore::is_system_healthy() const {
    return system_confidence_ > 0.7 && !emergency_active_;
}

std::vector<std::string> NeuroFCCCore::get_system_warnings() const {
    return system_warnings_;
}

void NeuroFCCCore::update_system_health(const AircraftState& state, const FlightControlSurface& surfaces) {
    // Detect anomalies
    detect_control_anomalies(surfaces);
    
    // Update confidence based on system performance
    if (system_warnings_.empty()) {
        system_confidence_ = std::min(1.0, system_confidence_ + 0.001);
    } else {
        system_confidence_ = std::max(0.1, system_confidence_ - 0.01);
    }
    
    // Clear old warnings
    if (system_warnings_.size() > 100) {
        system_warnings_.erase(system_warnings_.begin(), system_warnings_.begin() + 50);
    }
}

void NeuroFCCCore::calibrate_pilot_behavior() {
    if (behavior_learner_) {
        behavior_learner_->learn_from_session();
        std::cout << "Pilot behavior calibration completed" << std::endl;
    }
}

FlightControlSurface NeuroFCCCore::autopilot_control(const AircraftState& state, const TrajectoryCommand& command) {
    FlightControlSurface surfaces{};
    
    // Simple autopilot: wings level, maintain altitude
    surfaces.aileron_left = -state.roll * 0.5;
    surfaces.aileron_right = state.roll * 0.5;
    surfaces.elevator = (command.desired_altitude - state.altitude) / 1000.0 * 0.3;
    surfaces.rudder = -state.yaw_rate * 0.3;
    surfaces.throttle = (command.desired_airspeed - state.airspeed) / 50.0 * 0.5 + 0.5;
    surfaces.flaps = state.airspeed < 80.0 ? 0.3 : 0.0;
    surfaces.spoilers = 0.0;
    
    return surfaces;
}

void NeuroFCCCore::detect_control_anomalies(const FlightControlSurface& surfaces) {
    // Check for extreme control positions
    if (std::abs(surfaces.aileron_left) > 0.9 || std::abs(surfaces.aileron_right) > 0.9) {
        system_warnings_.push_back("Extreme aileron deflection detected");
    }
    
    if (std::abs(surfaces.elevator) > 0.9) {
        system_warnings_.push_back("Extreme elevator deflection detected");
    }
    
    // Check for asymmetric control (possible failure)
    double aileron_asymmetry = std::abs(surfaces.aileron_left + surfaces.aileron_right);
    if (aileron_asymmetry > 0.1) {
        system_warnings_.push_back("Aileron asymmetry detected");
    }
}

} // namespace neuro_fcc
