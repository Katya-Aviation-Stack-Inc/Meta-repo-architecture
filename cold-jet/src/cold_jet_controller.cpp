#include "cold_jet_core.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace cold_jet {

// ColdJetController Implementation
ColdJetController::ColdJetController()
    : current_mode_(JetMode::IDLE)
    , system_healthy_(true)
    , system_efficiency_(0.8)
    , last_update_time_(std::chrono::system_clock::now()) {
    
    cooler_ = std::make_unique<CryogenicCooler>(20000.0); // 20kW max cooling
    flow_model_ = std::make_unique<FlowDynamicsModel>();
    vectoring_system_ = std::make_unique<ThrustVectoringSystem>();
}

bool ColdJetController::initialize() {
    std::cout << "Cold-Jet Controller initializing..." << std::endl;
    
    if (!cooler_->initialize()) {
        std::cerr << "Failed to initialize cryogenic cooler" << std::endl;
        return false;
    }
    
    if (!vectoring_system_->initialize()) {
        std::cerr << "Failed to initialize thrust vectoring system" << std::endl;
        return false;
    }
    
    flow_model_->initialize_grid(50, 50, 50);
    
    // Set default operational limits
    operational_limits_["max_thrust"] = 10000.0; // Newtons
    operational_limits_["max_cooling"] = 20000.0; // Watts
    operational_limits_["max_temperature"] = 373.15; // Kelvin
    
    // Set default performance targets
    performance_targets_["efficiency"] = 0.85;
    performance_targets_["response_time"] = 0.01; // seconds
    
    std::cout << "Cold-Jet Controller initialized successfully" << std::endl;
    return true;
}

ColdJetParameters ColdJetController::update_control(
    double thrust_command,
    double azimuth_command,
    double elevation_command,
    double altitude,
    double ambient_temperature) {
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Update flow model for current conditions
    flow_model_->set_altitude(altitude);
    flow_model_->set_ambient_conditions(ambient_temperature, 101325.0 * std::pow(1.0 - 0.0065 * altitude / 288.15, 5.256));
    
    // Calculate optimal parameters
    current_parameters_ = calculate_optimal_parameters(thrust_command, altitude, ambient_temperature);
    
    // Update cryogenic cooler
    double cooling_power = current_parameters_.cooling_power;
    cooler_->set_cooling_power(cooling_power);
    
    // Calculate thrust vector
    current_thrust_vector_ = vectoring_system_->calculate_thrust_vector(
        current_parameters_, azimuth_command, elevation_command
    );
    
    // Update system state
    update_system_state();
    monitor_performance();
    
    // Check update time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto update_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    if (update_time > target_update_period_) {
        system_warnings_.push_back("Control update exceeded target: " + std::to_string(update_time.count()) + "μs");
    }
    
    last_update_time_ = std::chrono::system_clock::now();
    return current_parameters_;
}

ColdJetParameters ColdJetController::calculate_optimal_parameters(
    double thrust_command,
    double altitude,
    double ambient_temp) {
    
    ColdJetParameters params{};
    
    // Calculate required mass flow rate for thrust
    double ambient_pressure = 101325.0 * std::pow(1.0 - 0.0065 * altitude / 288.15, 5.256);
    double air_density = ambient_pressure / (AIR_GAS_CONSTANT * ambient_temp);
    
    // Simplified thrust calculation: F = ṁ * V + (P_out - P_ambient) * A
    double nozzle_area = 0.01; // m² (10cm diameter nozzle)
    double required_velocity = thrust_command / (air_density * nozzle_area * 0.1); // Simplified
    
    params.mass_flow_rate = air_density * required_velocity * nozzle_area;
    params.nozzle_area = nozzle_area;
    
    // Calculate temperature requirements
    params.inlet_temperature = ambient_temp;
    double target_outlet_temp = 77.0; // Liquid nitrogen temperature
    
    // Adjust for altitude effects
    double altitude_factor = std::exp(-altitude / 8000.0); // Scale height ~8km
    target_outlet_temp = target_outlet_temp + (ambient_temp - target_outlet_temp) * (1.0 - altitude_factor);
    
    params.outlet_temperature = std::max(target_outlet_temp, 77.0);
    
    // Calculate pressure ratio
    params.pressure_ratio = 2.0 + altitude / 5000.0; // Increase with altitude
    
    // Calculate cooling power requirements
    double specific_heat = 1005.0; // J/(kg·K)
    params.cooling_power = params.mass_flow_rate * specific_heat * (params.inlet_temperature - params.outlet_temperature);
    
    // Calculate efficiency
    double ideal_efficiency = (params.inlet_temperature - params.outlet_temperature) / params.inlet_temperature;
    params.efficiency = ideal_efficiency * 0.6; // Real-world efficiency factor
    
    // Apply operational limits
    params.cooling_power = std::min(params.cooling_power, operational_limits_["max_cooling"]);
    params.mass_flow_rate = std::min(params.mass_flow_rate, 10.0); // Max 10 kg/s
    
    return params;
}

void ColdJetController::update_system_state() {
    // Update system efficiency
    double cooler_efficiency = cooler_->get_cooling_efficiency();
    double vectoring_efficiency = vectoring_system_->get_vectoring_efficiency();
    double flow_efficiency = flow_model_->calculate_cooling_effectiveness(current_parameters_);
    
    system_efficiency_ = cooler_efficiency * vectoring_efficiency * flow_efficiency;
    
    // Check system health
    system_healthy_ = true;
    
    if (!cooler_->is_cooling_stable()) {
        system_healthy_ = false;
        system_warnings_.push_back("Cryogenic cooler instability detected");
    }
    
    if (current_parameters_.efficiency < 0.3) {
        system_healthy_ = false;
        system_warnings_.push_back("Low system efficiency detected");
    }
    
    if (cooler_->is_emergency_active()) {
        system_warnings_.push_back("Emergency cooling active");
    }
    
    // Clear old warnings
    if (system_warnings_.size() > 20) {
        system_warnings_.erase(system_warnings_.begin(), system_warnings_.begin() + 10);
    }
}

void ColdJetController::monitor_performance() {
    // Check performance targets
    if (system_efficiency_ < performance_targets_["efficiency"]) {
        optimize_performance();
    }
    
    // Check for performance degradation
    static double last_efficiency = system_efficiency_;
    if (system_efficiency_ < last_efficiency * 0.9) {
        system_warnings_.push_back("Performance degradation detected");
    }
    last_efficiency = system_efficiency_;
}

void ColdJetController::optimize_performance() {
    // Simple optimization: adjust parameters to improve efficiency
    if (current_parameters_.efficiency < 0.5) {
        // Increase pressure ratio to improve thrust efficiency
        current_parameters_.pressure_ratio *= 1.1;
        current_parameters_.pressure_ratio = std::min(current_parameters_.pressure_ratio, 5.0);
    }
    
    if (cooler_->get_cooling_efficiency() < 0.7) {
        // Reduce cooling power to improve efficiency
        current_parameters_.cooling_power *= 0.9;
    }
}

bool ColdJetController::is_system_healthy() const {
    return system_healthy_;
}

double ColdJetController::get_system_efficiency() const {
    return system_efficiency_;
}

std::vector<std::string> ColdJetController::get_system_warnings() const {
    return system_warnings_;
}

void ColdJetController::set_jet_mode(JetMode mode) {
    current_mode_ = mode;
    
    switch (mode) {
        case JetMode::EMERGENCY_COOLING:
            cooler_->enable_emergency_cooling();
            break;
        case JetMode::PERFORMANCE_MODE:
            vectoring_system_->enable_micro_vectoring(true);
            break;
        default:
            cooler_->disable_emergency_cooling();
            vectoring_system_->enable_micro_vectoring(false);
            break;
    }
    
    std::cout << "Jet mode changed to: " << static_cast<int>(mode) << std::endl;
}

// ColdJetSoftwareCore Implementation
ColdJetSoftwareCore::ColdJetSoftwareCore()
    : thrust_command_(0.0)
    , azimuth_command_(0.0)
    , elevation_command_(0.0)
    , cooling_command_(0.0)
    , current_altitude_(0.0)
    , current_airspeed_(0.0)
    , ambient_temperature_(288.15)
    , ambient_pressure_(101325.0)
    , air_density_(1.225)
    , automatic_optimization_enabled_(false)
    , current_flight_phase_("ground")
    , aircraft_configuration_("generic")
    , system_initialized_(false) {
    
    controller_ = std::make_unique<ColdJetController>();
    initialization_time_ = std::chrono::system_clock::now();
}

bool ColdJetSoftwareCore::initialize() {
    std::cout << "Cold-Jet Software Core initializing..." << std::endl;
    
    if (!controller_->initialize()) {
        std::cerr << "Failed to initialize Cold-Jet controller" << std::endl;
        return false;
    }
    
    system_initialized_ = true;
    std::cout << "Cold-Jet Software Core initialized successfully" << std::endl;
    return true;
}

void ColdJetSoftwareCore::shutdown() {
    std::cout << "Shutting down Cold-Jet Software Core..." << std::endl;
    
    if (controller_) {
        controller_->set_jet_mode(JetMode::IDLE);
    }
    
    system_initialized_ = false;
    std::cout << "Cold-Jet Software Core shutdown complete" << std::endl;
}

ColdJetParameters ColdJetSoftwareCore::process_control_loop(
    double altitude,
    double airspeed,
    double ambient_temperature,
    double ambient_pressure) {
    
    if (!system_initialized_) {
        return ColdJetParameters{};
    }
    
    // Update environmental conditions
    current_altitude_ = altitude;
    current_airspeed_ = airspeed;
    ambient_temperature_ = ambient_temperature;
    ambient_pressure_ = ambient_pressure;
    
    // Calculate air density
    air_density_ = ambient_pressure / (AIR_GAS_CONSTANT * ambient_temperature);
    
    // Compensate for environmental conditions
    compensate_for_environmental_conditions();
    
    // Process control through controller
    auto parameters = controller_->update_control(
        thrust_command_,
        azimuth_command_,
        elevation_command_,
        altitude,
        ambient_temperature
    );
    
    // Update flight dynamics
    update_flight_dynamics();
    
    // Log performance data
    log_performance_data();
    
    return parameters;
}

void ColdJetSoftwareCore::compensate_for_environmental_conditions() {
    // Adjust commands based on environmental conditions
    
    // Altitude compensation
    double altitude_factor = std::exp(-current_altitude_ / 8000.0);
    thrust_command_ *= (1.0 + (1.0 - altitude_factor) * 0.5); // Increase thrust at altitude
    
    // Temperature compensation
    double temp_factor = ambient_temperature_ / 288.15; // Standard temperature
    cooling_command_ *= temp_factor; // Adjust cooling based on ambient temperature
    
    // Airspeed compensation
    if (current_airspeed_ > 50.0) {
        double ram_effect = current_airspeed_ * current_airspeed_ / 2000.0; // Ram rise effect
        thrust_command_ *= (1.0 - ram_effect * 0.1); // Reduce thrust due to ram effect
    }
}

void ColdJetSoftwareCore::update_flight_dynamics() {
    // Update flight dynamics based on current conditions
    
    // Calculate dynamic pressure
    double dynamic_pressure = 0.5 * air_density_ * current_airspeed_ * current_airspeed_;
    
    // Adjust for flight phase
    if (current_flight_phase_ == "takeoff") {
        thrust_command_ *= 1.2; // Boost for takeoff
    } else if (current_flight_phase_ == "cruise") {
        thrust_command_ *= 0.8; // Reduce for cruise efficiency
    } else if (current_flight_phase_ == "hover") {
        thrust_command_ = air_density_ * 9.80665 * 1000.0; // Hover thrust (simplified)
    }
}

void ColdJetSoftwareCore::log_performance_data() {
    // In a real implementation, this would log to a data file
    // For now, just track performance internally
}

bool ColdJetSoftwareCore::is_system_ready() const {
    return system_initialized_ && controller_->is_system_healthy();
}

double ColdJetSoftwareCore::get_current_thrust() const {
    return thrust_command_;
}

ThrustVector ColdJetSoftwareCore::get_thrust_vector() const {
    // This would return the actual thrust vector from the controller
    return ThrustVector{thrust_command_, azimuth_command_, elevation_command_, 2.0};
}

double ColdJetSoftwareCore::get_cooling_capacity() const {
    return cooling_command_;
}

void ColdJetSoftwareCore::enable_automatic_optimization(bool enable) {
    automatic_optimization_enabled_ = enable;
    std::cout << "Automatic optimization " << (enable ? "enabled" : "disabled") << std::endl;
}

void ColdJetSoftwareCore::set_flight_phase(const std::string& phase) {
    current_flight_phase_ = phase;
    std::cout << "Flight phase set to: " << phase << std::endl;
}

void ColdJetSoftwareCore::configure_for_aircraft(const std::string& aircraft_type) {
    aircraft_configuration_ = aircraft_type;
    std::cout << "Aircraft configuration set to: " << aircraft_type << std::endl;
}

} // namespace cold_jet
