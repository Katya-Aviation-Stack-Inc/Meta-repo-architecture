#ifndef NEURO_FCC_H
#define NEURO_FCC_H

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>
#include "../core/airmesh_core.h"

namespace neuro_fcc {

    // Core data structures
    struct AircraftState {
        double latitude;      // Degrees
        double longitude;     // Degrees
        double altitude;      // Meters
        double velocity_x;    // m/s
        double velocity_y;    // m/s
        double velocity_z;    // m/s
        double roll;          // radians
        double pitch;         // radians
        double yaw;           // radians
        double roll_rate;     // rad/s
        double pitch_rate;    // rad/s
        double yaw_rate;      // rad/s
        std::chrono::system_clock::time_point timestamp;
    };

    struct ControlInput {
        double elevator;       // -1.0 to 1.0
        double aileron;       // -1.0 to 1.0
        double rudder;        // -1.0 to 1.0
        double throttle;       // 0.0 to 1.0
        std::chrono::system_clock::time_point timestamp;
    };

    struct ControlOutput {
        double elevator_cmd;
        double aileron_cmd;
        double rudder_cmd;
        double throttle_cmd;
        std::chrono::system_clock::time_point timestamp;
    };

    struct EnvironmentalData {
        double wind_speed;        // m/s
        double wind_direction;    // degrees
        double temperature;        // Celsius
        double pressure;          // hPa
        double humidity;          // %
        std::chrono::system_clock::time_point timestamp;
    };

    struct Anomaly {
        enum class Type {
            CONTROL_DEVIATION,
            SENSOR_FAILURE,
            ACTUATOR_FAULT,
            ENVIRONMENTAL_DISTURBANCE
        };
        
        Type type;
        double severity;
        std::chrono::system_clock::time_point detection_time;
        std::string description;
    };

    struct CorrectionResponse {
        ControlOutput corrected_controls;
        bool requires_pilot_attention;
        std::string correction_description;
    };

    // Main Neuro-FCC class
    class NeuroFCC {
    private:
        // Core components
        std::unique_ptr<class PilotBehavioralModel> learning_module_;
        std::unique_ptr<class MotorReflexEngine> reflex_engine_;
        std::unique_ptr<class FaultCorrectionSystem> fault_correction_;
        std::unique_ptr<class TrajectoryReproductionModule> trajectory_module_;
        
        // System state
        bool initialized_;
        bool learning_enabled_;
        bool reflex_active_;
        double current_latency_;  // microseconds
        
        // Performance metrics
        std::vector<double> latency_history_;
        size_t processed_inputs_;
        
    public:
        NeuroFCC();
        ~NeuroFCC();
        
        // Initialization
        bool initialize();
        void enable_learning(bool enable);
        void enable_reflex(bool enable);
        
        // Core functionality
        ControlOutput process_control_request(const ControlInput& input, 
                                           const AircraftState& state,
                                           const EnvironmentalData& environment);
        
        // Learning interface
        void update_pilot_model(const ControlInput& input, 
                               const AircraftState& state);
        
        // Fault handling
        void handle_fault_condition(const Anomaly& fault);
        
        // Trajectory coordination
        void coordinate_trajectory(const aamn::ZeroLatencyAircraftPosition& planned_trajectory,
                                  const aamn::ZeroLatencyMeshNode& mesh_node);
        
        // Performance monitoring
        double get_current_latency() const;
        bool meets_latency_requirements() const;
        size_t get_processed_inputs() const;
        
    private:
        void update_latency_metrics(double latency);
        void handle_latency_violation(double latency);
    };

    // Pilot Behavioral Model
    class PilotBehavioralModel {
    private:
        struct ControlPattern {
            AircraftState state;
            ControlInput input;
            EnvironmentalData conditions;
            std::chrono::system_clock::time_point timestamp;
        };
        
        struct PilotProfile {
            double aggressiveness_factor;
            double precision_preference;
            double response_timing;
            std::vector<uint32_t> certified_aircraft;
            std::chrono::system_clock::time_point last_update;
        };
        
        std::unique_ptr<class NeuralNetwork> adaptation_network_;
        std::vector<ControlPattern> pattern_history_;
        PilotProfile pilot_profile_;
        
    public:
        PilotBehavioralModel();
        ~PilotBehavioralModel();
        
        void learn_control_pattern(const ControlInput& input, 
                                 const AircraftState& state,
                                 const EnvironmentalData& environment);
        
        ControlOutput predict_response(const AircraftState& state,
                                     const EnvironmentalData& environment);
        
        void adapt_to_conditions(const EnvironmentalData& environment);
        void update_profile(const ControlInput& input, const AircraftState& state);
    };

    // Motor Reflex Engine
    class MotorReflexEngine {
    private:
        static constexpr double MAX_LATENCY_MS = 2.0;
        std::chrono::high_resolution_clock::time_point last_response_;
        std::unique_ptr<class LowLevelController> hardware_interface_;
        std::vector<double> latency_history_;
        double average_latency_;
        
    public:
        MotorReflexEngine();
        ~MotorReflexEngine();
        
        ControlOutput execute_reflex(const ControlInput& request,
                                   const AircraftState& state);
        
        double get_current_latency() const;
        bool meets_latency_requirements() const;
    };

    // Fault Correction System
    class FaultCorrectionSystem {
    private:
        struct FaultDetector {
            std::string name;
            std::function<bool(const ControlInput&, const AircraftState&)> detection_function;
            double confidence_threshold;
        };
        
        struct CorrectionModel {
            Anomaly::Type fault_type;
            std::function<CorrectionResponse(const Anomaly&, 
                                           const ControlInput&, 
                                           const AircraftState&)> correction_function;
            double effectiveness_score;
        };
        
        std::vector<FaultDetector> detectors_;
        std::vector<CorrectionModel> correction_models_;
        std::unique_ptr<class RecoveryEngine> recovery_engine_;
        
    public:
        FaultCorrectionSystem();
        ~FaultCorrectionSystem();
        
        void detect_anomalies(const ControlInput& input, 
                            const AircraftState& state);
        
        CorrectionResponse apply_correction(const Anomaly& anomaly,
                                          const ControlInput& input,
                                          const AircraftState& state);
        
        void update_correction_models(const CorrectionResponse& result);
    };

    // Trajectory Reproduction Module
    class TrajectoryReproductionModule {
    private:
        struct TrajectoryPattern {
            std::vector<aamn::ZeroLatencyAircraftPosition> waypoints;
            EnvironmentalData conditions;
            std::vector<ControlInput> control_sequence;
            std::chrono::system_clock::time_point execution_time;
        };
        
        std::unique_ptr<class TinyAIModel> trajectory_model_;
        std::vector<TrajectoryPattern> learned_trajectories_;
        
    public:
        TrajectoryReproductionModule();
        ~TrajectoryReproductionModule();
        
        void learn_trajectory(const std::vector<aamn::ZeroLatencyAircraftPosition>& path,
                            const std::vector<EnvironmentalData>& conditions,
                            const std::vector<ControlInput>& controls);
        
        std::vector<ControlOutput> reproduce_trajectory(
            const std::vector<aamn::ZeroLatencyAircraftPosition>& trajectory_request);
        
        void coordinate_with_aamn(const aamn::ZeroLatencyMeshNode& mesh_node);
    };

} // namespace neuro_fcc

#endif // NEURO_FCC_H