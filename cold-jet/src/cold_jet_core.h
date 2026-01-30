#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>
#include <array>

namespace cold_jet {

struct ThermodynamicState {
    double temperature;        // Kelvin
    double pressure;          // Pascal
    double density;           // kg/m³
    double enthalpy;          // J/kg
    double entropy;           // J/(kg·K)
    double mach_number;       // dimensionless
    double velocity;          // m/s
};

struct FlowField {
    double x_velocity;        // m/s
    double y_velocity;        // m/s
    double z_velocity;        // m/s
    double temperature;       // Kelvin
    double pressure;          // Pascal
    double density;           // kg/m³
    double vorticity;         // 1/s
};

struct ColdJetParameters {
    double inlet_temperature;  // Kelvin
    double outlet_temperature; // Kelvin
    double mass_flow_rate;     // kg/s
    double nozzle_area;        // m²
    double pressure_ratio;     // dimensionless
    double cooling_power;      // Watts
    double efficiency;         // 0.0 to 1.0
};

struct ThrustVector {
    double thrust_magnitude;   // Newtons
    double azimuth_angle;      // radians
    double elevation_angle;    // radians
    double moment_arm;         // meters
};

enum class JetMode {
    IDLE,
    COOLING_GENERATION,
    THRUST_PRODUCTION,
    VECTORING,
    EMERGENCY_COOLING,
    PERFORMANCE_MODE
};

class CryogenicCooler {
public:
    CryogenicCooler(double max_cooling_power);
    ~CryogenicCooler() = default;
    
    bool initialize();
    void set_cooling_power(double power_watts);
    double get_cooling_power() const { return current_cooling_power_; }
    double get_temperature_drop() const { return temperature_drop_; }
    
    bool is_cooling_stable() const { return cooling_stable_; }
    double get_cooling_efficiency() const { return cooling_efficiency_; }
    
    void enable_emergency_cooling();
    void disable_emergency_cooling();
    bool is_emergency_active() const { return emergency_cooling_active_; }

private:
    void update_cooling_dynamics();
    void calculate_temperature_drop();
    void monitor_system_health();
    
    double max_cooling_power_;
    double current_cooling_power_;
    double temperature_drop_;
    double cooling_efficiency_;
    bool cooling_stable_;
    bool emergency_cooling_active_;
    
    std::chrono::system_clock::time_point last_update_time_;
    std::vector<double> temperature_history_;
};

class FlowDynamicsModel {
public:
    FlowDynamicsModel();
    ~FlowDynamicsModel() = default;
    
    void initialize_grid(int nx, int ny, int nz);
    FlowField calculate_flow_field(const ColdJetParameters& params, const ThrustVector& vector);
    ThermodynamicState calculate_thermodynamics(const FlowField& flow, double altitude);
    
    void set_altitude(double altitude_meters);
    void set_ambient_conditions(double temperature, double pressure);
    
    double calculate_thrust_coefficient(const ColdJetParameters& params);
    double calculate_cooling_effectiveness(const ColdJetParameters& params);
    
    std::vector<FlowField> get_flow_history() const { return flow_history_; }

private:
    void solve_navier_stokes(const ColdJetParameters& params);
    void apply_boundary_conditions();
    void calculate_pressure_gradients();
    void update_velocity_field();
    
    int grid_nx_, grid_ny_, grid_nz_;
    std::vector<std::vector<std::vector<FlowField>>> flow_grid_;
    std::vector<FlowField> flow_history_;
    
    double ambient_temperature_;
    double ambient_pressure_;
    double altitude_;
    double gravity_;
};

class ThrustVectoringSystem {
public:
    ThrustVectoringSystem();
    ~ThrustVectoringSystem() = default;
    
    bool initialize();
    ThrustVector calculate_thrust_vector(
        const ColdJetParameters& jet_params,
        double azimuth_command,
        double elevation_command
    );
    
    void set_vectoring_limits(double max_azimuth, double max_elevation);
    void set_response_rate(double degrees_per_second);
    
    bool is_vectoring_available() const { return vectoring_available_; }
    double get_vectoring_efficiency() const { return vectoring_efficiency_; }
    
    void enable_micro_vectoring(bool enable);
    bool is_micro_vectoring_enabled() const { return micro_vectoring_enabled_; }

private:
    void calculate_nozzle_geometry();
    void update_vectoring_dynamics();
    void validate_vectoring_limits();
    
    bool vectoring_available_;
    double vectoring_efficiency_;
    bool micro_vectoring_enabled_;
    
    double max_azimuth_angle_;
    double max_elevation_angle_;
    double vectoring_response_rate_;
    
    ThrustVector current_thrust_vector_;
    std::chrono::system_clock::time_point last_vector_update_;
};

class ColdJetController {
public:
    ColdJetController();
    ~ColdJetController() = default;
    
    bool initialize();
    void shutdown();
    
    // Main control loop
    ColdJetParameters update_control(
        double thrust_command,
        double azimuth_command,
        double elevation_command,
        double altitude,
        double ambient_temperature
    );
    
    // Mode control
    void set_jet_mode(JetMode mode);
    JetMode get_jet_mode() const { return current_mode_; }
    
    // Configuration
    void set_performance_target(const std::string& target, double value);
    void set_operational_limits(double max_thrust, double max_cooling);
    
    // Monitoring
    bool is_system_healthy() const;
    double get_system_efficiency() const;
    std::vector<std::string> get_system_warnings() const;
    
    // Diagnostics
    void run_system_diagnostics();
    void calibrate_system();
    void test_emergency_procedures();

private:
    void update_system_state();
    void monitor_performance();
    void handle_system_faults();
    void optimize_performance();
    
    ColdJetParameters calculate_optimal_parameters(
        double thrust_command,
        double altitude,
        double ambient_temp
    );
    
    JetMode current_mode_;
    std::unique_ptr<CryogenicCooler> cooler_;
    std::unique_ptr<FlowDynamicsModel> flow_model_;
    std::unique_ptr<ThrustVectoringSystem> vectoring_system_;
    
    ColdJetParameters current_parameters_;
    ThrustVector current_thrust_vector_;
    
    bool system_healthy_;
    double system_efficiency_;
    std::vector<std::string> system_warnings_;
    
    std::map<std::string, double> performance_targets_;
    std::map<std::string, double> operational_limits_;
    
    std::chrono::system_clock::time_point last_update_time_;
    std::chrono::microseconds target_update_period_{5000}; // 5ms target
};

class ColdJetSoftwareCore {
public:
    ColdJetSoftwareCore();
    ~ColdJetSoftwareCore() = default;
    
    bool initialize();
    void shutdown();
    
    // High-level interface
    void set_thrust_command(double thrust_newtons);
    void set_direction_command(double azimuth_deg, double elevation_deg);
    void set_cooling_command(double cooling_watts);
    
    // Real-time control
    ColdJetParameters process_control_loop(
        double altitude,
        double airspeed,
        double ambient_temperature,
        double ambient_pressure
    );
    
    // System status
    bool is_system_ready() const;
    double get_current_thrust() const;
    ThrustVector get_thrust_vector() const;
    double get_cooling_capacity() const;
    
    // Advanced features
    void enable_automatic_optimization(bool enable);
    void set_flight_phase(const std::string& phase);
    void configure_for_aircraft(const std::string& aircraft_type);

private:
    void update_flight_dynamics();
    void calculate_air_density_model();
    void compensate_for_environmental_conditions();
    void log_performance_data();
    
    std::unique_ptr<ColdJetController> controller_;
    
    double thrust_command_;
    double azimuth_command_;
    double elevation_command_;
    double cooling_command_;
    
    double current_altitude_;
    double current_airspeed_;
    double ambient_temperature_;
    double ambient_pressure_;
    double air_density_;
    
    bool automatic_optimization_enabled_;
    std::string current_flight_phase_;
    std::string aircraft_configuration_;
    
    std::chrono::system_clock::time_point initialization_time_;
    bool system_initialized_;
};

} // namespace cold_jet
