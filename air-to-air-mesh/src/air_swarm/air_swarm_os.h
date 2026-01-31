#ifndef AIR_SWARM_OS_H
#define AIR_SWARM_OS_H

#include <cstdint>
#include <vector>
#include <chrono>
#include <memory>
#include <string>
#include <map>
#include <set>
#include <queue>

namespace air_swarm {

    // Forward declarations
    class ConsensusEngine;
    class DecisionMaker;
    class TrafficManager;
    class ConflictResolver;

    // Data structures for swarm intelligence
    struct AircraftState {
        uint32_t aircraft_id;              // Unique aircraft identifier
        double latitude;                   // Latitude in degrees
        double longitude;                  // Longitude in degrees
        double altitude;                   // Altitude in meters
        double heading;                    // Heading in degrees
        double speed;                      // Speed in m/s
        double vertical_speed;             // Vertical speed in m/s
        std::string flight_phase;          // Current flight phase (takeoff, cruise, landing, etc.)
        std::chrono::system_clock::time_point timestamp;
    };

    struct FlightIntent {
        uint32_t aircraft_id;              // Aircraft identifier
        std::vector<std::pair<double, double>> waypoints; // Planned waypoints (lat, lon)
        double planned_altitude;           // Planned altitude in meters
        double planned_speed;              // Planned speed in m/s
        std::chrono::system_clock::time_point submission_time;
        std::chrono::system_clock::time_point expiration_time;
        bool is_priority;                 // Priority flight (emergency, etc.)
    };

    struct AirspaceState {
        std::map<uint32_t, AircraftState> aircraft_positions; // All aircraft positions
        std::map<uint32_t, FlightIntent> flight_intents;      // All flight intents
        std::set<std::pair<uint32_t, uint32_t>> conflicts;     // Detected conflicts
        std::chrono::system_clock::time_point last_update;
    };

    struct ConsensusDecision {
        uint32_t decision_id;               // Unique decision identifier
        std::string decision_type;        // Type of decision (route, altitude, speed, etc.)
        std::map<uint32_t, std::string> aircraft_actions; // Actions for each aircraft
        std::chrono::system_clock::time_point creation_time;
        std::chrono::system_clock::time_point execution_time;
        bool is_executed;                  // Whether decision has been executed
    };

    struct SwarmMetrics {
        uint64_t total_aircraft;           // Total aircraft in swarm
        uint64_t active_flights;            // Currently active flights
        uint64_t resolved_conflicts;         // Total conflicts resolved
        uint64_t consensus_decisions;        // Total consensus decisions made
        double average_decision_time;     // Average time to reach consensus (ms)
        double network_efficiency;         // Network efficiency percentage
        std::chrono::system_clock::time_point last_metrics_update;
    };

    // Main Air Swarm OS Class
    class AirSwarmOS {
    public:
        AirSwarmOS();
        ~AirSwarmOS();

        // System initialization
        bool initialize();
        void enable_system(bool enable);
        
        // Core functionality
        void update_aircraft_state(const AircraftState& state);
        void submit_flight_intent(const FlightIntent& intent);
        void process_swarm_intelligence();
        ConsensusDecision reach_consensus(const std::vector<FlightIntent>& intents);
        void execute_consensus_decision(const ConsensusDecision& decision);
        void resolve_conflicts();
        
        // Network management
        void add_aircraft_to_swarm(uint32_t aircraft_id);
        void remove_aircraft_from_swarm(uint32_t aircraft_id);
        std::vector<uint32_t> get_swarm_members() const;
        bool is_aircraft_in_swarm(uint32_t aircraft_id) const;
        
        // System status and diagnostics
        AirspaceState get_current_airspace_state() const;
        SwarmMetrics get_swarm_metrics() const;
        uint64_t get_total_aircraft() const;
        uint64_t get_active_flights() const;
        uint64_t get_resolved_conflicts() const;
        double get_network_efficiency() const;
        
        // Configuration methods
        void set_consensus_timeout(double timeout_seconds);
        void set_conflict_detection_range(double range_meters);
        void set_communication_range(double range_meters);
        
    private:
        // Private helper methods
        void update_airspace_model();
        void detect_conflicts();
        void broadcast_airspace_state();
        void update_performance_metrics();
        void log_swarm_event(const std::string& event_description);
        void handle_network_partition();
        
        // System state variables
        bool initialized_;
        bool active_;
        double consensus_timeout_;
        double conflict_detection_range_;
        double communication_range_;
        
        // Swarm state
        std::set<uint32_t> swarm_members_;
        AirspaceState airspace_state_;
        std::queue<ConsensusDecision> pending_decisions_;
        
        // Performance metrics
        uint64_t total_aircraft_;
        uint64_t active_flights_;
        uint64_t resolved_conflicts_;
        uint64_t consensus_decisions_;
        double network_efficiency_;
        std::chrono::system_clock::time_point last_metrics_update_;
        
        // Core components
        std::unique_ptr<ConsensusEngine> consensus_engine_;
        std::unique_ptr<DecisionMaker> decision_maker_;
        std::unique_ptr<TrafficManager> traffic_manager_;
        std::unique_ptr<ConflictResolver> conflict_resolver_;
    };

    // Consensus Engine Component
    class ConsensusEngine {
    public:
        ConsensusEngine();
        ~ConsensusEngine();
        
        ConsensusDecision reach_consensus(const std::vector<FlightIntent>& intents, double timeout);
        bool validate_consensus(const ConsensusDecision& decision);
        void set_consensus_parameters(double timeout, double confidence_threshold);
        double get_consensus_timeout() const;
        double get_confidence_threshold() const;
        
    private:
        double consensus_timeout_;
        double confidence_threshold_;
    };

    // Decision Maker Component
    class DecisionMaker {
    public:
        DecisionMaker();
        ~DecisionMaker();
        
        ConsensusDecision make_decision(const std::vector<FlightIntent>& intents);
        std::map<uint32_t, std::string> optimize_aircraft_actions(const std::vector<FlightIntent>& intents);
        void set_decision_parameters(double optimization_weight, double safety_margin);
        double get_optimization_weight() const;
        double get_safety_margin() const;
        
    private:
        double optimization_weight_;
        double safety_margin_;
    };

    // Traffic Manager Component
    class TrafficManager {
    public:
        TrafficManager();
        ~TrafficManager();
        
        void update_aircraft_state(const AircraftState& state);
        void submit_flight_intent(const FlightIntent& intent);
        std::vector<FlightIntent> get_active_intents() const;
        std::vector<AircraftState> get_aircraft_states() const;
        void set_traffic_parameters(double update_rate, double intent_timeout);
        double get_update_rate() const;
        double get_intent_timeout() const;
        
    private:
        std::map<uint32_t, AircraftState> aircraft_states_;
        std::map<uint32_t, FlightIntent> flight_intents_;
        double update_rate_;
        double intent_timeout_;
    };

    // Conflict Resolver Component
    class ConflictResolver {
    public:
        ConflictResolver();
        ~ConflictResolver();
        
        std::set<std::pair<uint32_t, uint32_t>> detect_conflicts(const AirspaceState& state, double detection_range);
        ConsensusDecision resolve_conflicts(const std::set<std::pair<uint32_t, uint32_t>>& conflicts, 
                                          const AirspaceState& state);
        void set_conflict_parameters(double detection_range, double resolution_aggressiveness);
        double get_detection_range() const;
        double get_resolution_aggressiveness() const;
        
    private:
        double detection_range_;
        double resolution_aggressiveness_;
    };

} // namespace air_swarm

#endif // AIR_SWARM_OS_H