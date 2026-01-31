#ifndef ADAPTIVE_ROTOR_H
#define ADAPTIVE_ROTOR_H

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>

namespace adaptive_rotor {

    // Core data structures
    struct RotorBladeState {
        double angle_of_attack;     // degrees
        double chord_length;       // meters
        double twist_angle;         // degrees
        double bending_moment;      // Nm
        double torsional_moment;    // Nm
        double vibration_frequency; // Hz
        double temperature;         // Celsius
        std::chrono::system_clock::time_point timestamp;
    };

    struct BladeControlCommand {
        double pitch_adjustment;    // degrees
        double chord_adjustment;     // meters
        double twist_adjustment;    // degrees
        double flap_deflection;      // degrees
        double tab_deflection;      // degrees
        std::chrono::system_clock::time_point timestamp;
    };

    struct VibrationData {
        double frequency;           // Hz
        double amplitude;            // meters
        double phase;               // radians
        std::chrono::system_clock::time_point timestamp;
    };

    struct AeroData {
        double airspeed;            // m/s
        double air_density;         // kg/m3
        double pressure;            // Pa
        double temperature;          // Celsius
        std::chrono::system_clock::time_point timestamp;
    };

    // Main Adaptive Rotor class
    class AdaptiveRotor {
    private:
        // Core components
        std::unique_ptr<class BladeControlSystem> blade_control_;
        std::unique_ptr<class VibrationSuppressionSystem> vibration_system_;
        std::unique_ptr<class AeroOptimizer> aero_optimizer_;
        std::unique_ptr<class ResonanceCanceller> resonance_canceller_;
        
        // System state
        bool initialized_;
        bool adaptive_control_active_;
        size_t blade_count_;
        std::vector<RotorBladeState> blade_states_;
        
        // Performance metrics
        std::vector<double> vibration_levels_;
        double current_efficiency_;
        
    public:
        AdaptiveRotor(size_t blade_count);
        ~AdaptiveRotor();
        
        // Initialization
        bool initialize();
        void enable_adaptive_control(bool enable);
        
        // Core functionality
        void update_blade_states(const std::vector<RotorBladeState>& states);
        std::vector<BladeControlCommand> calculate_control_commands(
            const std::vector<RotorBladeState>& states,
            const AeroData& aero_data);
        
        // Vibration suppression
        void suppress_vibrations(const std::vector<VibrationData>& vibration_data);
        
        // Aero optimization
        void optimize_aerodynamics(const AeroData& aero_data);
        
        // Resonance cancellation
        void cancel_resonance(const std::vector<VibrationData>& vibration_data);
        
        // Performance monitoring
        double get_current_efficiency() const;
        std::vector<double> get_vibration_levels() const;
        std::vector<RotorBladeState> get_blade_states() const;
        
    private:
        void update_performance_metrics(const std::vector<VibrationData>& vibration_data);
    };

    // Blade Control System
    class BladeControlSystem {
    private:
        struct ControlAlgorithm {
            double kp;  // Proportional gain
            double ki;  // Integral gain
            double kd;  // Derivative gain
            double max_adjustment;  // Maximum adjustment per cycle
        };
        
        std::vector<ControlAlgorithm> control_algorithms_;
        std::vector<BladeControlCommand> last_commands_;
        
    public:
        BladeControlSystem(size_t blade_count);
        ~BladeControlSystem();
        
        std::vector<BladeControlCommand> calculate_commands(
            const std::vector<RotorBladeState>& current_states,
            const std::vector<RotorBladeState>& target_states);
        
        void update_control_algorithm(size_t blade_index, const ControlAlgorithm& algorithm);
        ControlAlgorithm get_control_algorithm(size_t blade_index) const;
    };

    // Vibration Suppression System
    class VibrationSuppressionSystem {
    private:
        struct FilterParameters {
            double frequency;  // Hz
            double bandwidth; // Hz
            double gain;       // Dimensionless
        };
        
        std::vector<FilterParameters> active_filters_;
        std::unique_ptr<class DigitalFilter> digital_filter_;
        
    public:
        VibrationSuppressionSystem();
        ~VibrationSuppressionSystem();
        
        void suppress_vibrations(const std::vector<VibrationData>& vibration_data,
                               std::vector<BladeControlCommand>& commands);
        
        void update_filter_parameters(const FilterParameters& params);
        std::vector<FilterParameters> get_active_filters() const;
    };

    // Aero Optimizer
    class AeroOptimizer {
    private:
        struct OptimizationParameters {
            double target_lift_coefficient;
            double target_drag_coefficient;
            double max_angle_of_attack;
            double min_angle_of_attack;
        };
        
        OptimizationParameters opt_params_;
        std::unique_ptr<class CFDModel> cfd_model_;
        
    public:
        AeroOptimizer();
        ~AeroOptimizer();
        
        std::vector<BladeControlCommand> optimize_aerodynamics(
            const std::vector<RotorBladeState>& blade_states,
            const AeroData& aero_data);
        
        void set_optimization_parameters(const OptimizationParameters& params);
        OptimizationParameters get_optimization_parameters() const;
    };

    // Resonance Canceller
    class ResonanceCanceller {
    private:
        struct ResonanceMode {
            double frequency;  // Hz
            double damping;    // Dimensionless
            double amplitude; // meters
        };
        
        std::vector<ResonanceMode> detected_modes_;
        std::unique_ptr<class AdaptiveFilter> adaptive_filter_;
        
    public:
        ResonanceCanceller();
        ~ResonanceCanceller();
        
        void cancel_resonance(const std::vector<VibrationData>& vibration_data,
                             std::vector<BladeControlCommand>& commands);
        
        void detect_resonance_modes(const std::vector<VibrationData>& vibration_data);
        std::vector<ResonanceMode> get_detected_modes() const;
    };

} // namespace adaptive_rotor

#endif // ADAPTIVE_ROTOR_H