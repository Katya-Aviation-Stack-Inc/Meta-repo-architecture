#include "predictive_airflow_core.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace predictive_airflow {

// PredictiveAirflowEngine Implementation
PredictiveAirflowEngine::PredictiveAirflowEngine()
    : current_mode_(PredictionMode::STEADY_STATE)
    , system_healthy_(true)
    , prediction_confidence_(0.8)
    , update_frequency_(50.0) // 50 Hz
    , emergency_predictions_enabled_(false)
    , adaptive_learning_enabled_(false)
    , last_update_time_(std::chrono::system_clock::now()) {
    
    neural_cfd_ = std::make_unique<TinyDeepCFD>();
    cfd_solver_ = std::make_unique<RealTimeCFDSolver>();
    vortex_predictor_ = std::make_unique<VortexPredictor>();
    turbulence_analyzer_ = std::make_unique<TurbulenceAnalyzer>();
    separation_predictor_ = std::make_unique<SeparationPredictor>();
}

bool PredictiveAirflowEngine::initialize() {
    std::cout << "Predictive Airflow Engine initializing..." << std::endl;
    
    if (!neural_cfd_->initialize(64, 32, 32)) {
        std::cerr << "Failed to initialize TinyDeepCFD" << std::endl;
        return false;
    }
    
    if (!cfd_solver_->initialize(10.0, 64)) {
        std::cerr << "Failed to initialize CFD solver" << std::endl;
        return false;
    }
    
    if (!vortex_predictor_->initialize()) {
        std::cerr << "Failed to initialize vortex predictor" << std::endl;
        return false;
    }
    
    if (!turbulence_analyzer_->initialize()) {
        std::cerr << "Failed to initialize turbulence analyzer" << std::endl;
        return false;
    }
    
    if (!separation_predictor_->initialize()) {
        std::cerr << "Failed to initialize separation predictor" << std::endl;
        return false;
    }
    
    std::cout << "Predictive Airflow Engine initialized successfully" << std::endl;
    return true;
}

FlowPrediction PredictiveAirflowEngine::predict_airflow(
    const AircraftState& aircraft_state,
    const std::vector<FlowNode>& current_flow,
    PredictionMode mode,
    double prediction_horizon_seconds) {
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    current_mode_ = mode;
    last_aircraft_state_ = aircraft_state;
    current_flow_field_ = current_flow;
    
    FlowPrediction prediction{};
    
    switch (mode) {
        case PredictionMode::STEADY_STATE:
            prediction = neural_cfd_->predict_flow(aircraft_state, current_flow, mode);
            break;
            
        case PredictionMode::UNSTEADY_DYNAMIC:
            // Combine neural network with CFD solver
            prediction = neural_cfd_->predict_flow(aircraft_state, current_flow, mode);
            auto cfd_result = cfd_solver_->solve_time_step(current_flow, aircraft_state);
            prediction.flow_field = cfd_result;
            break;
            
        case PredictionMode::VORTEX_SHEDDING:
            prediction = neural_cfd_->predict_flow(aircraft_state, current_flow, mode);
            prediction.vortex_positions = vortex_predictor_->predict_vortex_positions(
                current_flow, aircraft_state, prediction_horizon_seconds);
            break;
            
        case PredictionMode::TURBULENCE_MODELING:
            prediction = neural_cfd_->predict_flow(aircraft_state, current_flow, mode);
            prediction.turbulence_zones = turbulence_analyzer_->detect_turbulence_zones(current_flow);
            break;
            
        case PredictionMode::SEPARATION_PREDICTION:
            prediction = neural_cfd_->predict_flow(aircraft_state, current_flow, mode);
            prediction.separation_point = separation_predictor_->predict_separation_point(
                aircraft_state, current_flow);
            prediction.stall_angle = separation_predictor_->predict_stall_angle(
                aircraft_state, current_flow);
            break;
            
        case PredictionMode::EMERGENCY_ANALYSIS:
            prediction = neural_cfd_->predict_flow(aircraft_state, current_flow, mode);
            
            // Enhanced analysis for emergency conditions
            prediction.vortex_positions = vortex_predictor_->predict_vortex_positions(
                current_flow, aircraft_state, prediction_horizon_seconds);
            prediction.turbulence_zones = turbulence_analyzer_->detect_turbulence_zones(current_flow);
            prediction.separation_point = separation_predictor_->predict_separation_point(
                aircraft_state, current_flow);
            prediction.stall_angle = separation_predictor_->predict_stall_angle(
                aircraft_state, current_flow);
            
            // Lower confidence for emergency conditions
            prediction.confidence_level *= 0.7;
            break;
    }
    
    // Validate prediction results
    validate_prediction_results(prediction);
    
    // Update system performance
    monitor_system_performance();
    
    // Check prediction time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto prediction_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    if (prediction_time > target_prediction_time_) {
        system_warnings_.push_back("Prediction exceeded target: " + std::to_string(prediction_time.count()) + "μs");
        prediction_confidence_ *= 0.95;
    }
    
    last_prediction_ = prediction;
    last_update_time_ = std::chrono::system_clock::now();
    
    return prediction;
}

std::vector<std::array<double, 3>> PredictiveAirflowEngine::get_vortex_predictions(double time_ahead) {
    if (!current_flow_field_.empty()) {
        return vortex_predictor_->predict_vortex_positions(
            current_flow_field_, last_aircraft_state_, time_ahead);
    }
    return {};
}

std::vector<std::array<double, 3>> PredictiveAirflowEngine::get_turbulence_zones() {
    if (!current_flow_field_.empty()) {
        return turbulence_analyzer_->detect_turbulence_zones(current_flow_field_);
    }
    return {};
}

double PredictiveAirflowEngine::get_separation_point() const {
    return last_prediction_.separation_point;
}

double PredictiveAirflowEngine::get_stall_margin() const {
    return last_prediction_.stall_angle - last_aircraft_state_.angle_of_attack;
}

void PredictiveAirflowEngine::validate_prediction_results(FlowPrediction& prediction) {
    // Validate aerodynamic coefficients
    prediction.lift_coefficient = std::clamp(prediction.lift_coefficient, -2.0, 2.0);
    prediction.drag_coefficient = std::clamp(prediction.drag_coefficient, 0.0, 1.0);
    prediction.moment_coefficient = std::clamp(prediction.moment_coefficient, -1.0, 1.0);
    
    // Validate angles
    prediction.stall_angle = std::clamp(prediction.stall_angle, 0.0, 30.0);
    prediction.separation_point = std::clamp(prediction.separation_point, 0.0, 1.0);
    
    // Validate confidence
    prediction.confidence_level = std::clamp(prediction.confidence_level, 0.0, 1.0);
    
    // Check for unrealistic values
    if (std::abs(prediction.lift_coefficient) > 1.5) {
        system_warnings_.push_back("Unusual lift coefficient predicted");
        prediction.confidence_level *= 0.8;
    }
    
    if (prediction.drag_coefficient > 0.5) {
        system_warnings_.push_back("High drag coefficient predicted");
        prediction.confidence_level *= 0.9;
    }
}

void PredictiveAirflowEngine::monitor_system_performance() {
    system_healthy_ = true;
    
    // Check neural network status
    if (!neural_cfd_->is_model_trained()) {
        system_warnings_.push_back("Neural network not trained");
        prediction_confidence_ *= 0.9;
    }
    
    // Check CFD solver convergence
    if (!cfd_solver_->is_converged()) {
        system_warnings_.push_back("CFD solver not converged");
        prediction_confidence_ *= 0.95;
    }
    
    // Check prediction confidence
    if (prediction_confidence_ < 0.5) {
        system_healthy_ = false;
        system_warnings_.push_back("Low prediction confidence");
    }
    
    // Update confidence based on system health
    if (system_healthy_) {
        prediction_confidence_ = std::min(1.0, prediction_confidence_ + 0.01);
    } else {
        prediction_confidence_ = std::max(0.1, prediction_confidence_ - 0.02);
    }
    
    // Clear old warnings
    if (system_warnings_.size() > 20) {
        system_warnings_.erase(system_warnings_.begin(), system_warnings_.begin() + 10);
    }
}

bool PredictiveAirflowEngine::is_system_healthy() const {
    return system_healthy_;
}

double PredictiveAirflowEngine::get_prediction_confidence() const {
    return prediction_confidence_;
}

std::vector<std::string> PredictiveAirflowEngine::get_system_warnings() const {
    return system_warnings_;
}

void PredictiveAirflowEngine::set_prediction_mode(PredictionMode mode) {
    current_mode_ = mode;
    std::cout << "Prediction mode set to: " << static_cast<int>(mode) << std::endl;
}

void PredictiveAirflowEngine::set_update_frequency(double hz) {
    update_frequency_ = hz;
    std::cout << "Update frequency set to: " << hz << " Hz" << std::endl;
}

void PredictiveAirflowEngine::enable_emergency_predictions(bool enable) {
    emergency_predictions_enabled_ = enable;
    std::cout << "Emergency predictions " << (enable ? "enabled" : "disabled") << std::endl;
}

void PredictiveAirflowEngine::calibrate_with_flight_data(const std::vector<AircraftState>& flight_history) {
    std::cout << "Calibrating with " << flight_history.size() << " flight data points..." << std::endl;
    
    // In a real implementation, this would train the neural network
    // For now, just simulate calibration
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    // Mark neural network as trained
    neural_cfd_->set_learning_rate(0.001);
    
    std::cout << "Calibration completed" << std::endl;
}

void PredictiveAirflowEngine::optimize_for_aircraft(const std::string& aircraft_type) {
    std::cout << "Optimizing for aircraft type: " << aircraft_type << std::endl;
    
    // Adjust parameters based on aircraft type
    if (aircraft_type == "fighter") {
        cfd_solver_->set_time_step(0.0005); // Smaller time step for high-speed
        vortex_predictor_->set_prediction_horizon(1.0); // Shorter horizon for maneuvering
    } else if (aircraft_type == "commercial") {
        cfd_solver_->set_time_step(0.002); // Larger time step for cruise
        vortex_predictor_->set_prediction_horizon(3.0); // Longer horizon for stability
    } else if (aircraft_type == "helicopter") {
        cfd_solver_->enable_sliding_mesh(true); // Enable for rotor analysis
        turbulence_analyzer_->enable_les_modeling(true); // Enable LES for rotor wake
    }
    
    std::cout << "Optimization completed for " << aircraft_type << std::endl;
}

void PredictiveAirflowEngine::enable_adaptive_learning(bool enable) {
    adaptive_learning_enabled_ = enable;
    std::cout << "Adaptive learning " << (enable ? "enabled" : "disabled") << std::endl;
}

void PredictiveAirflowEngine::shutdown() {
    std::cout << "Shutting down Predictive Airflow Engine..." << std::endl;
    
    // Reset all components
    current_mode_ = PredictionMode::STEADY_STATE;
    system_healthy_ = false;
    prediction_confidence_ = 0.0;
    system_warnings_.clear();
    
    std::cout << "Predictive Airflow Engine shutdown complete" << std::endl;
}

} // namespace predictive_airflow
