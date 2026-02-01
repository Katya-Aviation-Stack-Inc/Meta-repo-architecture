#ifndef HYPERSONIC_FLIGHT_CONTROL_H
#define HYPERSONIC_FLIGHT_CONTROL_H

#include <vector>
#include <memory>
#include <chrono>
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>

namespace revolutionary_aviation {

    // Hypersonic flight regimes
    enum class HypersonicRegime {
        SUBSONIC,        // < Mach 0.8
        TRANSONIC,      // Mach 0.8-1.2
        SUPERSONIC,     // Mach 1.2-5.0
        HYPERSONIC,     // Mach 5.0-25.0
        REENTRY,        // Mach 25.0+
        ORBITAL         // Orbital velocities
    };

    // Plasma state management
    enum class PlasmaState {
        NONE,           // No plasma formation
        IONIZATION,     // Air ionization begins
        PLASMA_FORMATION, // Full plasma layer
        COMMUNICATION_BLACKOUT, // Radio blackout
        THERMAL_PROTECTION   // Plasma for thermal protection
    };

    // Hypersonic flight parameters
    struct HypersonicParameters {
        double mach_number = 0.0;
        double altitude_km = 0.0;
        double dynamic_pressure_pa = 0.0;
        double stagnation_temperature_k = 0.0;
        double heat_flux_w_m2 = 0.0;
        PlasmaState plasma_state = PlasmaState::NONE;
        double ionization_degree = 0.0;
        double electron_density_m3 = 0.0;
    };

    // Control surface configuration for hypersonic flight
    struct HypersonicControlConfig {
        // Traditional control surfaces
        double elevator_deflection = 0.0;
        double aileron_deflection = 0.0;
        double rudder_deflection = 0.0;
        
        // Hypersonic-specific controls
        double body_flap_deflection = 0.0;
        double speed_brake_position = 0.0;
        double reaction_control_thrust = 0.0;
        
        // Plasma controls
        double plasma_density_modifier = 0.0;
        double electromagnetic_field_strength = 0.0;
        
        // Thermal management
        double active_cooling_flow = 0.0;
        double heat_shield_position = 0.0;
    };

    // Hypersonic flight control system
    class HypersonicFlightControl {
    public:
        HypersonicFlightControl();
        ~HypersonicFlightControl();

        // System initialization
        bool initialize();
        bool shutdown();
        bool is_initialized() const;

        // Flight regime management
        bool transition_to_regime(HypersonicRegime target_regime);
        HypersonicRegime get_current_regime() const;
        bool predict_regime_transition(double time_horizon_seconds);

        // Real-time hypersonic control
        bool execute_hypersonic_maneuver(const std::string& maneuver_type);
        bool maintain_hypersonic_stability();
        bool control_plasma_interaction();
        bool manage_thermal_protection();

        // Advanced control algorithms
        bool enable_adaptive_control();
        bool enable_predictive_control();
        bool enable_neural_network_control();
        bool enable_robust_control();

        // Plasma management
        bool predict_plasma_formation();
        bool control_plasma_density();
        bool mitigate_communication_blackout();
        bool utilize_plasma_for_propulsion();

        // Thermal management
        bool calculate_heat_load_distribution();
        bool activate_active_cooling();
        bool manage_thermal_stress();
        bool optimize_heat_shield_configuration();

        // Performance optimization
        bool optimize_hypersonic_trajectory();
        bool minimize_heat_load();
        bool maximize_range_efficiency();
        bool optimize_fuel_consumption();

        // Safety and emergency systems
        bool detect_hypersonic_emergencies();
        bool execute_emergency_procedures();
        bool ensure_structural_integrity();
        bool manage_aerodynamic_forces();

        // Data acquisition and monitoring
        HypersonicParameters get_current_parameters() const;
        std::vector<double> get_control_surface_positions() const;
        std::vector<double> get_plasma_characteristics() const;
        std::vector<double> get_thermal_distribution() const;

    private:
        struct HypersonicControlImpl;
        std::unique_ptr<HypersonicControlImpl> impl_;
        
        std::atomic<bool> initialized_;
        std::atomic<HypersonicRegime> current_regime_;
        
        // Threading
        std::thread control_thread_;
        std::thread monitoring_thread_;
        mutable std::mutex control_mutex_;
        std::condition_variable control_cv_;
        
        // Internal methods
        void control_loop();
        void monitoring_loop();
        bool calculate_hypersonic_dynamics();
        bool compute_plasma_interactions();
        bool manage_thermal_systems();
    };

    // Plasma interaction manager
    class PlasmaInteractionManager {
    public:
        PlasmaInteractionManager();
        ~PlasmaInteractionManager();

        // Plasma physics simulation
        bool initialize_plasma_physics();
        bool calculate_plasma_parameters(const HypersonicParameters& params);
        bool predict_plasma_behavior(double time_horizon_seconds);

        // Communication management
        bool detect_communication_blackout();
        bool predict_blackout_duration();
        bool mitigate_blackout_effects();
        bool establish_alternative_communication();

        // Plasma utilization
        bool utilize_plasma_for_flow_control();
        bool generate_plasma_thrusters();
        bool create_plasma_shield();
        bool manipulate_plasma_density();

        // Electromagnetic effects
        bool calculate_electromagnetic_interference();
        bool manage_magnetohydrodynamics();
        bool control_plasma_conductivity();
        bool optimize_electromagnetic_fields();

    private:
        struct PlasmaManagerImpl;
        std::unique_ptr<PlasmaManagerImpl> plasma_impl_;
    };

    // Thermal protection system
    class ThermalProtectionSystem {
    public:
        ThermalProtectionSystem();
        ~ThermalProtectionSystem();

        // Heat management
        bool initialize_thermal_system();
        bool calculate_heat_flux_distribution();
        bool manage_active_cooling_systems();
        bool control_heat_shield_configuration();

        // Material protection
        bool monitor_material_integrity();
        bool predict_material_degradation();
        bool manage_thermal_expansion();
        bool optimize_coolant_distribution();

        // Emergency thermal protection
        bool activate_emergency_cooling();
        bool deploy_emergency_heat_shields();
        bool manage_thermal_emergencies();
        bool ensure_crew_survival_temperature();

    private:
        struct ThermalProtectionImpl;
        std::unique_ptr<ThermalProtectionImpl> thermal_impl_;
    };

    // Hypersonic trajectory optimizer
    class HypersonicTrajectoryOptimizer {
    public:
        HypersonicTrajectoryOptimizer();
        ~HypersonicTrajectoryOptimizer();

        // Trajectory planning
        bool optimize_ascend_trajectory();
        bool optimize_cruise_trajectory();
        bool optimize_descend_trajectory();
        bool optimize_orbital_insertion();

        // Performance optimization
        bool minimize_total_heat_load();
        bool maximize_range_and_endurange();
        bool optimize_fuel_efficiency();
        bool minimize_flight_time();

        // Constraint management
        bool respect_thermal_constraints();
        bool respect_structural_limits();
        bool respect_communication_windows();
        bool respect_safety_corridors();

        // Real-time optimization
        bool enable_real_time_optimization();
        bool adapt_to_changing_conditions();
        bool reoptimize_during_flight();
        bool handle_trajectory_disruptions();

    private:
        struct TrajectoryOptimizerImpl;
        std::unique_ptr<TrajectoryOptimizerImpl> optimizer_impl_;
    };

    // Hypersonic safety monitor
    class HypersonicSafetyMonitor {
    public:
        HypersonicSafetyMonitor();
        ~HypersonicSafetyMonitor();

        // Safety monitoring
        bool initialize_safety_systems();
        bool monitor_flight_envelope();
        bool detect_structural_stress();
        bool monitor_thermal_limits();

        // Emergency detection
        bool detect_flight_instabilities();
        bool detect_thermal_emergencies();
        bool detect_communication_failures();
        bool detect_system_degradation();

        // Safety actions
        bool execute_safety_procedures();
        bool activate_safety_systems();
        bool ensure_safe_flight_conditions();
        bool manage_emergency_landings();

    private:
        struct SafetyMonitorImpl;
        std::unique_ptr<SafetyMonitorImpl> safety_impl_;
    };

} // namespace revolutionary_aviation

#endif // HYPERSONIC_FLIGHT_CONTROL_H
