#ifndef COLDJET_CONTROL_H
#define COLDJET_CONTROL_H

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>

namespace coldjet {

    // Core data structures
    struct ThermalState {
        double temperature;         // Celsius
        double pressure;            // Pa
        double density;             // kg/m3
        double velocity;            // m/s
        double mass_flow_rate;       // kg/s
        std::chrono::system_clock::time_point timestamp;
    };

    struct ThrustVector {
        double x_component;         // Newtons
        double y_component;          // Newtons
        double z_component;          // Newtons
        std::chrono::system_clock::time_point timestamp;
    };

    struct ControlInput {
        double temperature_setpoint; // Celsius
        double pressure_setpoint;    // Pa
        double mass_flow_rate;       // kg/s
        double vectoring_angle_x;     // degrees
        double vectoring_angle_y;    // degrees
        double vectoring_angle_z;    // degrees
        std::chrono::system_clock::time_point timestamp;
    };

    struct EnvironmentalData {
        double ambient_temperature;  // Celsius
        double ambient_pressure;     // Pa
        double humidity;             // %
        double wind_speed;          // m/s
        double wind_direction;        // degrees
        std::chrono::system_clock::time_point timestamp;
    };

    // Main ColdJet Control class
    class ColdJetControl {
    private:
        // Core components
        std::unique_ptr<class ThermalDynamicsModel> thermal_model_;
        std::unique_ptr<class ThrustVectorController> vector_controller_;
        std::unique_ptr<class FlowOptimizer> flow_optimizer_;
        std::unique_ptr<class StabilityAnalyzer> stability_analyzer_;
        
        // System state
        bool initialized_;
        bool active_;
        ThermalState current_state_;
        ThrustVector current_thrust_;
        
        // Performance metrics
        double current_efficiency_;
        double current_thrust_;
        std::vector<double> temperature_history_;
        
    public:
        ColdJetControl();
        ~ColdJetControl();
        
        // Initialization
        bool initialize();
        void enable_system(bool enable);
        
        // Core functionality
        void update_thermal_state(const ThermalState& state);
        ThrustVector calculate_thrust_vector(const ControlInput& input,
                                           const EnvironmentalData& environment);
        
        // Temperature control
        void control_temperature(double setpoint);
        
        // Thrust vectoring
        void vector_thrust(const ThrustVector& desired_vector);
        
        // Flow optimization
        void optimize_flow(const EnvironmentalData& environment);
        
        // Stability analysis
        bool analyze_stability();
        
        // Performance monitoring
        double get_current_efficiency() const;
        double get_current_thrust() const;
        ThermalState get_current_state() const;
        
    private:
        void update_performance_metrics();
        void handle_temperature_violation(double temperature);
    };

    // Thermal Dynamics Model
    class ThermalDynamicsModel {
    private:
        struct ThermalParameters {
            double heat_capacity;       // J/(kg*K)
            double thermal_conductivity; // W/(m*K)
            double expansion_coefficient; // 1/K
            double reference_temperature; // Celsius
        };
        
        ThermalParameters params_;
        std::vector<ThermalState> state_history_;
        
    public:
        ThermalDynamicsModel();
        ~ThermalDynamicsModel();
        
        ThermalState predict_state(const ThermalState& current_state,
                                  const ControlInput& control_input,
                                  double time_step);
        
        void update_parameters(const ThermalParameters& params);
        ThermalParameters get_parameters() const;
    };

    // Thrust Vector Controller
    class ThrustVectorController {
    private:
        struct VectoringParameters {
            double max_deflection;     // degrees
            double response_time;      // seconds
            double precision;          // degrees
        };
        
        VectoringParameters params_;
        ThrustVector last_vector_;
        
    public:
        ThrustVectorController();
        ~ThrustVectorController();
        
        ThrustVector calculate_vector(const ThrustVector& desired_vector,
                                    const ThermalState& thermal_state);
        
        void update_parameters(const VectoringParameters& params);
        VectoringParameters get_parameters() const;
    };

    // Flow Optimizer
    class FlowOptimizer {
    private:
        struct OptimizationParameters {
            double target_efficiency;
            double max_temperature;
            double min_pressure;
            double max_flow_rate;
        };
        
        OptimizationParameters opt_params_;
        std::unique_ptr<class CFDModel> cfd_model_;
        
    public:
        FlowOptimizer();
        ~FlowOptimizer();
        
        void optimize_flow_parameters(const ThermalState& current_state,
                                    const EnvironmentalData& environment);
        
        void set_optimization_parameters(const OptimizationParameters& params);
        OptimizationParameters get_optimization_parameters() const;
    };

    // Stability Analyzer
    class StabilityAnalyzer {
    private:
        struct StabilityMetrics {
            double temperature_stability;
            double pressure_stability;
            double flow_stability;
            double overall_stability;
        };
        
        StabilityMetrics metrics_;
        std::vector<StabilityMetrics> history_;
        
    public:
        StabilityAnalyzer();
        ~StabilityAnalyzer();
        
        bool analyze_system_stability(const ThermalState& current_state,
                                     const ThrustVector& current_thrust);
        
        StabilityMetrics get_current_metrics() const;
        void update_metrics(const StabilityMetrics& metrics);
    };

} // namespace coldjet

#endif // COLDJET_CONTROL_H