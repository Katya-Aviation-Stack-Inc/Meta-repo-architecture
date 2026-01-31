#ifndef SELF_HEALING_BIOS_H
#define SELF_HEALING_BIOS_H

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>
#include <string>

namespace self_healing {

    // Core data structures
    struct MemorySegment {
        uint32_t address;
        uint32_t size;
        bool is_corrupted;
        uint32_t corruption_pattern;
        std::chrono::system_clock::time_point last_check;
    };

    struct ModuleStatus {
        std::string module_name;
        uint32_t module_id;
        bool is_functional;
        double health_score;  // 0.0 to 1.0
        std::chrono::system_clock::time_point last_update;
    };

    struct BusConfiguration {
        uint32_t bus_id;
        std::vector<uint32_t> connected_modules;
        bool is_active;
        double bandwidth_utilization;
        std::chrono::system_clock::time_point last_reconfiguration;
    };

    struct DataRoute {
        uint32_t source_module;
        uint32_t destination_module;
        std::vector<uint32_t> intermediate_nodes;
        bool is_active;
        double priority;
    };

    struct SystemDiagnostics {
        uint64_t total_memory;
        uint64_t corrupted_memory;
        uint32_t failed_modules;
        uint32_t active_modules;
        double overall_health;  // 0.0 to 1.0
        std::chrono::system_clock::time_point timestamp;
    };

    // Main Self-Healing BIOS class
    class SelfHealingBIOS {
    private:
        // Core components
        std::unique_ptr<class MemoryManager> memory_manager_;
        std::unique_ptr<class ModuleRecovery> module_recovery_;
        std::unique_ptr<class BusReconfigurator> bus_reconfigurator_;
        std::unique_ptr<class RouteManager> route_manager_;
        std::unique_ptr<class DiagnosticsEngine> diagnostics_engine_;
        
        // System state
        bool initialized_;
        bool active_;
        std::vector<MemorySegment> memory_segments_;
        std::vector<ModuleStatus> module_statuses_;
        std::vector<BusConfiguration> bus_configurations_;
        std::vector<DataRoute> data_routes_;
        
        // Performance metrics
        uint64_t total_recoveries_;
        uint64_t successful_recoveries_;
        double recovery_success_rate_;
        std::chrono::system_clock::time_point last_recovery_;
        
    public:
        SelfHealingBIOS();
        ~SelfHealingBIOS();
        
        // Initialization
        bool initialize();
        void enable_system(bool enable);
        
        // Core functionality
        void perform_diagnostics();
        void recover_corrupted_memory(const MemorySegment& segment);
        void recover_module(const ModuleStatus& module);
        void reconfigure_bus(const BusConfiguration& configuration);
        void reestablish_routes(const std::vector<DataRoute>& routes);
        
        // Memory management
        void register_memory_segment(const MemorySegment& segment);
        void check_memory_integrity();
        std::vector<MemorySegment> get_corrupted_segments() const;
        
        // Module management
        void register_module(const ModuleStatus& module);
        void update_module_status(uint32_t module_id, bool is_functional, double health_score);
        std::vector<ModuleStatus> get_failed_modules() const;
        
        // Bus management
        void register_bus(const BusConfiguration& configuration);
        void update_bus_status(uint32_t bus_id, bool is_active);
        std::vector<BusConfiguration> get_active_buses() const;
        
        // Route management
        void register_route(const DataRoute& route);
        void update_route_status(const DataRoute& route, bool is_active);
        std::vector<DataRoute> get_active_routes() const;
        
        // Performance monitoring
        SystemDiagnostics get_system_diagnostics() const;
        double get_recovery_success_rate() const;
        uint64_t get_total_recoveries() const;
        std::chrono::system_clock::time_point get_last_recovery_time() const;
        
    private:
        void update_performance_metrics();
        void log_recovery_event(const std::string& event_description);
        void handle_critical_failure();
    };

    // Memory Manager
    class MemoryManager {
    private:
        std::vector<MemorySegment> memory_segments_;
        uint32_t checksum_algorithm_;
        std::vector<uint8_t> backup_memory_;
        
    public:
        MemoryManager();
        ~MemoryManager();
        
        bool check_segment_integrity(const MemorySegment& segment);
        void repair_segment(MemorySegment& segment);
        void create_backup(const MemorySegment& segment);
        bool restore_from_backup(MemorySegment& segment);
        
        void set_checksum_algorithm(uint32_t algorithm);
        uint32_t get_checksum_algorithm() const;
        
        void add_memory_segment(const MemorySegment& segment);
        std::vector<MemorySegment> get_corrupted_segments() const;
    };

    // Module Recovery
    class ModuleRecovery {
    private:
        struct RecoveryParameters {
            double timeout_seconds;
            uint32_t max_recovery_attempts;
            bool enable_hardware_reset;
        };
        
        RecoveryParameters params_;
        std::vector<ModuleStatus> module_registry_;
        
    public:
        ModuleRecovery();
        ~ModuleRecovery();
        
        bool recover_module(ModuleStatus& module);
        void reset_module(uint32_t module_id);
        void reinitialize_module(uint32_t module_id);
        bool is_module_recoverable(uint32_t module_id) const;
        
        void set_recovery_parameters(const RecoveryParameters& params);
        RecoveryParameters get_recovery_parameters() const;
        
        void register_module(const ModuleStatus& module);
        std::vector<ModuleStatus> get_failed_modules() const;
    };

    // Bus Reconfigurator
    class BusReconfigurator {
    private:
        std::vector<BusConfiguration> bus_configurations_;
        double reconfiguration_timeout_;
        
    public:
        BusReconfigurator();
        ~BusReconfigurator();
        
        bool reconfigure_bus(BusConfiguration& configuration);
        void activate_bus(uint32_t bus_id);
        void deactivate_bus(uint32_t bus_id);
        bool is_bus_operational(uint32_t bus_id) const;
        
        void add_bus_configuration(const BusConfiguration& configuration);
        std::vector<BusConfiguration> get_active_buses() const;
        
        void set_reconfiguration_timeout(double timeout);
        double get_reconfiguration_timeout() const;
    };

    // Route Manager
    class RouteManager {
    private:
        std::vector<DataRoute> data_routes_;
        double route_optimization_threshold_;
        
    public:
        RouteManager();
        ~RouteManager();
        
        bool establish_route(DataRoute& route);
        void activate_route(uint32_t source, uint32_t destination);
        void deactivate_route(uint32_t source, uint32_t destination);
        std::vector<DataRoute> find_alternate_routes(uint32_t source, uint32_t destination) const;
        
        void add_route(const DataRoute& route);
        std::vector<DataRoute> get_active_routes() const;
        
        void set_optimization_threshold(double threshold);
        double get_optimization_threshold() const;
    };

    // Diagnostics Engine
    class DiagnosticsEngine {
    private:
        SystemDiagnostics current_diagnostics_;
        std::vector<SystemDiagnostics> diagnostics_history_;
        
    public:
        DiagnosticsEngine();
        ~DiagnosticsEngine();
        
        SystemDiagnostics perform_system_diagnostics();
        void update_diagnostics(const SystemDiagnostics& diagnostics);
        SystemDiagnostics get_current_diagnostics() const;
        
        void add_diagnostics_to_history(const SystemDiagnostics& diagnostics);
        std::vector<SystemDiagnostics> get_diagnostics_history() const;
        
        bool is_system_healthy() const;
        double get_system_health_score() const;
    };

} // namespace self_healing

#endif // SELF_HEALING_BIOS_H