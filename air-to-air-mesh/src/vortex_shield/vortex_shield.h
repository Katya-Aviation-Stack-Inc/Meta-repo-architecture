#ifndef VORTEX_SHIELD_H
#define VORTEX_SHIELD_H

#include <cstdint>
#include <vector>
#include <chrono>
#include <memory>
#include <string>

namespace vortex_shield {

    // Forward declarations
    class PressureAnalyzer;
    class FlowPredictor;
    class ControlCorrector;
    class VibrationMonitor;

    // Data structures for vortex detection and control
    struct PressureData {
        double static_pressure;      // Static pressure in Pascals
        double dynamic_pressure;     // Dynamic pressure in Pascals
        double total_pressure;        // Total pressure in Pascals
        double temperature;           // Temperature in Kelvin
        std::chrono::system_clock::time_point timestamp;
    };

    struct FlowDynamics {
        double axial_velocity;       // Axial velocity in m/s
        double radial_velocity;      // Radial velocity in m/s
        double tangential_velocity;   // Tangential velocity in m/s
        double vorticity;             // Vorticity magnitude
        double turbulence_intensity;  // Turbulence intensity
        std::chrono::system_clock::time_point timestamp;
    };

    struct VortexState {
        bool is_vortex_ring_detected; // True if vortex ring state is detected
        double vortex_ring_index;      // Vortex ring detection index (0.0 to 1.0)
        double descent_rate;           // Descent rate in m/s
        double rotor_rpm;             // Rotor RPM
        double collective_angle;      // Collective pitch angle in degrees
        std::chrono::system_clock::time_point timestamp;
    };

    struct ControlAdjustment {
        double collective_correction;  // Collective pitch correction in degrees
        double cyclic_x_correction;     // Longitudinal cyclic correction in degrees
        double cyclic_y_correction;      // Lateral cyclic correction in degrees
        double throttle_correction;     // Throttle correction percentage
        std::chrono::system_clock::time_point timestamp;
    };

    struct VibrationData {
        double frequency;            // Vibration frequency in Hz
        double amplitude;            // Vibration amplitude in mm
        double phase;                // Vibration phase in radians
        std::string source_component; // Component causing vibration
        std::chrono::system_clock::time_point timestamp;
    };

    struct VortexShieldDiagnostics {
        uint64_t total_detections;        // Total vortex ring detections
        uint64_t successful_corrections;   // Successful corrections
        double avg_correction_time;       // Average correction time in milliseconds
        double detection_accuracy;        // Detection accuracy percentage
        std::chrono::system_clock::time_point last_detection;
        std::chrono::system_clock::time_point last_correction;
    };

    // Main Vortex Shield System Class
    class VortexShield {
    public:
        VortexShield();
        ~VortexShield();

        // System initialization
        bool initialize();
        void enable_system(bool enable);
        
        // Core functionality
        void monitor_vortex_conditions();
        VortexState detect_vortex_ring_state();
        ControlAdjustment calculate_preventive_adjustments(const VortexState& state);
        void apply_control_corrections(const ControlAdjustment& adjustments);
        void suppress_vortex_ring_state();
        
        // Data input methods
        void update_pressure_data(const PressureData& data);
        void update_flow_dynamics(const FlowDynamics& dynamics);
        void update_vibration_data(const VibrationData& vibration);
        void update_rotor_state(double rpm, double collective_angle);
        
        // System status and diagnostics
        VortexShieldDiagnostics get_system_diagnostics() const;
        double get_detection_accuracy() const;
        uint64_t get_total_detections() const;
        uint64_t get_successful_corrections() const;
        std::chrono::system_clock::time_point get_last_detection() const;
        std::chrono::system_clock::time_point get_last_correction() const;
        
        // Configuration methods
        void set_sensitivity_threshold(double threshold);
        void set_response_aggressiveness(double aggressiveness);
        void set_prediction_horizon(double horizon_seconds);
        
    private:
        // Private helper methods
        void analyze_pressure_patterns();
        void predict_flow_dynamics();
        void monitor_vibrations();
        void update_performance_metrics();
        void log_detection_event(const std::string& event_description);
        void handle_critical_vortex_state();
        
        // System state variables
        bool initialized_;
        bool active_;
        double sensitivity_threshold_;
        double response_aggressiveness_;
        double prediction_horizon_;
        
        // Performance metrics
        uint64_t total_detections_;
        uint64_t successful_corrections_;
        double detection_accuracy_;
        std::chrono::system_clock::time_point last_detection_;
        std::chrono::system_clock::time_point last_correction_;
        
        // Core components
        std::unique_ptr<PressureAnalyzer> pressure_analyzer_;
        std::unique_ptr<FlowPredictor> flow_predictor_;
        std::unique_ptr<ControlCorrector> control_corrector_;
        std::unique_ptr<VibrationMonitor> vibration_monitor_;
        
        // Data storage
        std::vector<PressureData> pressure_history_;
        std::vector<FlowDynamics> flow_history_;
        std::vector<VibrationData> vibration_history_;
        std::vector<VortexState> vortex_history_;
        std::vector<ControlAdjustment> adjustment_history_;
    };

    // Pressure Analysis Component
    class PressureAnalyzer {
    public:
        PressureAnalyzer();
        ~PressureAnalyzer();
        
        bool analyze_pressure_field(const std::vector<PressureData>& pressure_data);
        double calculate_vortex_index(const PressureData& data) const;
        bool detect_vortex_signatures(const std::vector<PressureData>& data) const;
        void set_analysis_parameters(double threshold, double window_size);
        double get_analysis_threshold() const;
        double get_window_size() const;
        
    private:
        double analysis_threshold_;
        double window_size_;
    };

    // Flow Prediction Component
    class FlowPredictor {
    public:
        FlowPredictor();
        ~FlowPredictor();
        
        FlowDynamics predict_flow_dynamics(const std::vector<FlowDynamics>& history, double prediction_horizon);
        bool predict_vortex_formation(const std::vector<FlowDynamics>& history);
        void set_prediction_parameters(double horizon, double confidence_threshold);
        double get_prediction_horizon() const;
        double get_confidence_threshold() const;
        
    private:
        double prediction_horizon_;
        double confidence_threshold_;
    };

    // Control Correction Component
    class ControlCorrector {
    public:
        ControlCorrector();
        ~ControlCorrector();
        
        ControlAdjustment calculate_corrections(const VortexState& state, double aggressiveness);
        bool apply_corrections(const ControlAdjustment& adjustments);
        void set_correction_parameters(double aggressiveness, double safety_margin);
        double get_aggressiveness() const;
        double get_safety_margin() const;
        
    private:
        double aggressiveness_;
        double safety_margin_;
    };

    // Vibration Monitoring Component
    class VibrationMonitor {
    public:
        VibrationMonitor();
        ~VibrationMonitor();
        
        bool monitor_vibrations(const std::vector<VibrationData>& vibration_data);
        VibrationData identify_vibration_source(const std::vector<VibrationData>& data);
        void suppress_vibrations(const VibrationData& source);
        void set_monitoring_parameters(double frequency_threshold, double amplitude_threshold);
        double get_frequency_threshold() const;
        double get_amplitude_threshold() const;
        
    private:
        double frequency_threshold_;
        double amplitude_threshold_;
    };

} // namespace vortex_shield

#endif // VORTEX_SHIELD_H