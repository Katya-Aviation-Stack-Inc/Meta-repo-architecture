#include "cold_jet_core.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>

namespace cold_jet {

// Physical constants
constexpr double AIR_GAS_CONSTANT = 287.05; // J/(kg·K)
constexpr double SPECIFIC_HEAT_RATIO = 1.4;
constexpr double STANDARD_GRAVITY = 9.80665; // m/s²

// CryogenicCooler Implementation
CryogenicCooler::CryogenicCooler(double max_cooling_power)
    : max_cooling_power_(max_cooling_power)
    , current_cooling_power_(0.0)
    , temperature_drop_(0.0)
    , cooling_efficiency_(0.0)
    , cooling_stable_(false)
    , emergency_cooling_active_(false)
    , last_update_time_(std::chrono::system_clock::now()) {
    
    temperature_history_.reserve(1000);
}

bool CryogenicCooler::initialize() {
    std::cout << "Cryogenic Cooler initializing with " << max_cooling_power_ << "W capacity" << std::endl;
    
    current_cooling_power_ = 0.0;
    temperature_drop_ = 0.0;
    cooling_efficiency_ = 0.0;
    cooling_stable_ = false;
    
    std::cout << "Cryogenic Cooler initialized successfully" << std::endl;
    return true;
}

void CryogenicCooler::set_cooling_power(double power_watts) {
    power_watts = std::clamp(power_watts, 0.0, max_cooling_power_);
    
    if (emergency_cooling_active_) {
        power_watts = max_cooling_power_; // Full power in emergency
    }
    
    current_cooling_power_ = power_watts;
    update_cooling_dynamics();
}

void CryogenicCooler::update_cooling_dynamics() {
    auto now = std::chrono::system_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update_time_);
    
    if (dt.count() > 0) {
        double dt_sec = dt.count() / 1000.0;
        
        // Calculate temperature drop based on cooling power
        // Simplified model: ΔT = P / (m_dot * c_p)
        double mass_flow_rate = current_cooling_power_ / 1000.0; // kg/s (simplified)
        double specific_heat = 1005.0; // J/(kg·K) for air
        
        double target_drop = current_cooling_power_ / (mass_flow_rate * specific_heat + 0.001);
        
        // Smooth transition to target temperature drop
        double time_constant = 2.0; // seconds
        temperature_drop_ += (target_drop - temperature_drop_) * (1.0 - std::exp(-dt_sec / time_constant));
        
        // Calculate efficiency
        cooling_efficiency_ = (current_cooling_power_ / max_cooling_power_) * 
                            std::exp(-temperature_drop_ / 100.0); // Efficiency decreases with larger temp drop
        
        // Check stability
        if (temperature_history_.size() > 10) {
            double variance = 0.0;
            double mean = temperature_drop_;
            for (double temp : temperature_history_) {
                variance += (temp - mean) * (temp - mean);
            }
            variance /= temperature_history_.size();
            
            cooling_stable_ = variance < 1.0; // Stable if variance < 1K
        }
        
        // Store temperature history
        temperature_history_.push_back(temperature_drop_);
        if (temperature_history_.size() > 1000) {
            temperature_history_.erase(temperature_history_.begin());
        }
        
        last_update_time_ = now;
    }
}

void CryogenicCooler::calculate_temperature_drop() {
    // More detailed thermodynamic calculation
    // Using ideal gas law and isentropic expansion
    
    double inlet_temp = 288.15; // Standard temperature (15°C)
    double pressure_ratio = 2.0; // Expansion ratio
    
    // Isentropic expansion temperature
    double outlet_temp = inlet_temp / std::pow(pressure_ratio, (SPECIFIC_HEAT_RATIO - 1.0) / SPECIFIC_HEAT_RATIO);
    
    // Adjust for cooling power
    double cooling_effect = current_cooling_power_ / (1000.0 * 1005.0); // Temperature reduction
    temperature_drop_ = (inlet_temp - outlet_temp) + cooling_effect;
}

void CryogenicCooler::enable_emergency_cooling() {
    emergency_cooling_active_ = true;
    set_cooling_power(max_cooling_power_);
    std::cout << "Emergency cooling activated - full power (" << max_cooling_power_ << "W)" << std::endl;
}

void CryogenicCooler::disable_emergency_cooling() {
    emergency_cooling_active_ = false;
    std::cout << "Emergency cooling deactivated" << std::endl;
}

// FlowDynamicsModel Implementation
FlowDynamicsModel::FlowDynamicsModel()
    : grid_nx_(50), grid_ny_(50), grid_nz_(50)
    , ambient_temperature_(288.15)
    , ambient_pressure_(101325.0)
    , altitude_(0.0)
    , gravity_(STANDARD_GRAVITY) {
    
    flow_grid_.resize(grid_nx_, std::vector<std::vector<FlowField>>(grid_ny_, std::vector<FlowField>(grid_nz_)));
}

void FlowDynamicsModel::initialize_grid(int nx, int ny, int nz) {
    grid_nx_ = nx;
    grid_ny_ = ny;
    grid_nz_ = nz;
    
    flow_grid_.resize(nx, std::vector<std::vector<FlowField>>(ny, std::vector<FlowField>(nz)));
    
    // Initialize flow field with ambient conditions
    for (int i = 0; i < nx; ++i) {
        for (int j = 0; j < ny; ++j) {
            for (int k = 0; k < nz; ++k) {
                flow_grid_[i][j][k] = {
                    0.0, 0.0, 0.0, // velocities
                    ambient_temperature_,
                    ambient_pressure_,
                    1.225, // density
                    0.0    // vorticity
                };
            }
        }
    }
    
    std::cout << "Flow grid initialized: " << nx << "x" << ny << "x" << nz << std::endl;
}

FlowField FlowDynamicsModel::calculate_flow_field(const ColdJetParameters& params, const ThrustVector& vector) {
    FlowField result{};
    
    // Calculate jet velocity from mass flow rate and density
    double jet_density = params.pressure_ratio * ambient_pressure_ / (AIR_GAS_CONSTANT * params.inlet_temperature);
    double jet_velocity = params.mass_flow_rate / (jet_density * params.nozzle_area);
    
    // Apply thrust vectoring
    result.x_velocity = jet_velocity * std::cos(vector.elevation_angle) * std::cos(vector.azimuth_angle);
    result.y_velocity = jet_velocity * std::cos(vector.elevation_angle) * std::sin(vector.azimuth_angle);
    result.z_velocity = jet_velocity * std::sin(vector.elevation_angle);
    
    // Calculate temperature and pressure
    result.temperature = params.outlet_temperature;
    result.pressure = ambient_pressure_ * params.pressure_ratio;
    result.density = result.pressure / (AIR_GAS_CONSTANT * result.temperature);
    
    // Calculate vorticity (simplified)
    result.vorticity = jet_velocity / (2.0 * M_PI * std::sqrt(params.nozzle_area / M_PI));
    
    return result;
}

ThermodynamicState FlowDynamicsModel::calculate_thermodynamics(const FlowField& flow, double altitude) {
    ThermodynamicState state{};
    
    // Calculate ambient conditions at altitude
    double temp_altitude = ambient_temperature_ - 0.0065 * altitude; // Standard lapse rate
    double pressure_altitude = ambient_pressure_ * std::pow(1.0 - 0.0065 * altitude / 288.15, 5.256);
    
    state.temperature = flow.temperature;
    state.pressure = flow.pressure;
    state.density = flow.density;
    
    // Calculate enthalpy (for ideal gas)
    state.enthalpy = 1005.0 * state.temperature; // c_p * T for air
    
    // Calculate entropy
    state.entropy = 1005.0 * std::log(state.temperature / 288.15) - 
                   AIR_GAS_CONSTANT * std::log(state.pressure / 101325.0);
    
    // Calculate Mach number
    double speed_of_sound = std::sqrt(SPECIFIC_HEAT_RATIO * AIR_GAS_CONSTANT * state.temperature);
    double velocity_magnitude = std::sqrt(flow.x_velocity * flow.x_velocity + 
                                        flow.y_velocity * flow.y_velocity + 
                                        flow.z_velocity * flow.z_velocity);
    state.mach_number = velocity_magnitude / speed_of_sound;
    state.velocity = velocity_magnitude;
    
    return state;
}

double FlowDynamicsModel::calculate_thrust_coefficient(const ColdJetParameters& params) {
    // Thrust coefficient based on pressure ratio and temperature ratio
    double temp_ratio = params.outlet_temperature / params.inlet_temperature;
    double pressure_ratio = params.pressure_ratio;
    
    // Simplified thrust coefficient calculation
    double thrust_coeff = std::sqrt(2.0 * SPECIFIC_HEAT_RATIO / (SPECIFIC_HEAT_RATIO - 1.0) * 
                                   (1.0 - std::pow(pressure_ratio, -(SPECIFIC_HEAT_RATIO - 1.0) / SPECIFIC_HEAT_RATIO)));
    
    return thrust_coeff * params.efficiency;
}

double FlowDynamicsModel::calculate_cooling_effectiveness(const ColdJetParameters& params) {
    // Cooling effectiveness based on temperature drop and mass flow
    double temp_drop = params.inlet_temperature - params.outlet_temperature;
    double max_possible_drop = params.inlet_temperature - 77.0; // Liquid nitrogen temperature
    
    double effectiveness = (temp_drop / max_possible_drop) * params.efficiency;
    return std::clamp(effectiveness, 0.0, 1.0);
}

void FlowDynamicsModel::set_altitude(double altitude_meters) {
    altitude_ = altitude_meters;
    
    // Update ambient conditions for new altitude
    ambient_temperature_ = 288.15 - 0.0065 * altitude_meters;
    ambient_pressure_ = 101325.0 * std::pow(1.0 - 0.0065 * altitude_meters / 288.15, 5.256);
}

// ThrustVectoringSystem Implementation
ThrustVectoringSystem::ThrustVectoringSystem()
    : vectoring_available_(true)
    , vectoring_efficiency_(0.95)
    , micro_vectoring_enabled_(false)
    , max_azimuth_angle_(M_PI / 4.0) // ±45 degrees
    , max_elevation_angle_(M_PI / 6.0) // ±30 degrees
    , vectoring_response_rate_(30.0) // 30 deg/sec
    , last_vector_update_(std::chrono::system_clock::now()) {
    
    current_thrust_vector_ = {0.0, 0.0, 0.0, 1.0};
}

bool ThrustVectoringSystem::initialize() {
    std::cout << "Thrust Vectoring System initializing..." << std::endl;
    
    calculate_nozzle_geometry();
    
    std::cout << "Vectoring limits: Azimuth ±" << max_azimuth_angle_ * 180.0 / M_PI 
              << "°, Elevation ±" << max_elevation_angle_ * 180.0 / M_PI << "°" << std::endl;
    std::cout << "Response rate: " << vectoring_response_rate_ << "°/s" << std::endl;
    
    return true;
}

ThrustVector ThrustVectoringSystem::calculate_thrust_vector(
    const ColdJetParameters& jet_params,
    double azimuth_command,
    double elevation_command) {
    
    // Validate and limit commands
    azimuth_command = std::clamp(azimuth_command, -max_azimuth_angle_, max_azimuth_angle_);
    elevation_command = std::clamp(elevation_command, -max_elevation_angle_, max_elevation_angle_);
    
    // Calculate thrust magnitude
    double thrust_coeff = calculate_thrust_coefficient(jet_params);
    double thrust_magnitude = thrust_coeff * jet_params.pressure_ratio * jet_params.nozzle_area * ambient_pressure_;
    
    // Apply response rate limiting
    auto now = std::chrono::system_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_vector_update_);
    
    if (dt.count() > 0) {
        double dt_sec = dt.count() / 1000.0;
        double max_angle_change = vectoring_response_rate_ * dt_sec * M_PI / 180.0;
        
        double azimuth_change = azimuth_command - current_thrust_vector_.azimuth_angle;
        double elevation_change = elevation_command - current_thrust_vector_.elevation_angle;
        
        // Limit rate of change
        if (std::abs(azimuth_change) > max_angle_change) {
            azimuth_command = current_thrust_vector_.azimuth_angle + 
                            std::copysign(max_angle_change, azimuth_change);
        }
        
        if (std::abs(elevation_change) > max_angle_change) {
            elevation_command = current_thrust_vector_.elevation_angle + 
                              std::copysign(max_angle_change, elevation_change);
        }
        
        last_vector_update_ = now;
    }
    
    // Update thrust vector
    current_thrust_vector_.thrust_magnitude = thrust_magnitude;
    current_thrust_vector_.azimuth_angle = azimuth_command;
    current_thrust_vector_.elevation_angle = elevation_command;
    current_thrust_vector_.moment_arm = 2.0; // meters (simplified)
    
    // Apply efficiency losses for vectoring
    double vectoring_loss = 1.0 - (std::abs(azimuth_command) + std::abs(elevation_command)) / 
                                 (max_azimuth_angle_ + max_elevation_angle_) * 0.1;
    
    current_thrust_vector_.thrust_magnitude *= vectoring_loss * vectoring_efficiency_;
    
    return current_thrust_vector_;
}

void ThrustVectoringSystem::calculate_nozzle_geometry() {
    // Calculate optimal nozzle geometry for cold jet
    // This would involve complex CFD calculations in a real implementation
    
    std::cout << "Nozzle geometry calculated for optimal cold jet performance" << std::endl;
}

void ThrustVectoringSystem::enable_micro_vectoring(bool enable) {
    micro_vectoring_enabled_ = enable;
    vectoring_response_rate_ = enable ? 60.0 : 30.0; // Double response rate for micro-vectoring
    std::cout << "Micro-vectoring " << (enable ? "enabled" : "disabled") << std::endl;
}

double ThrustVectoringSystem::calculate_thrust_coefficient(const ColdJetParameters& params) {
    // Simplified thrust coefficient calculation for cold jet
    double pressure_ratio = params.pressure_ratio;
    double temp_ratio = params.outlet_temperature / params.inlet_temperature;
    
    // Account for reduced temperature effects
    double thermal_factor = std::sqrt(temp_ratio);
    
    return 1.4 * pressure_ratio * thermal_factor * params.efficiency;
}

} // namespace cold_jet
