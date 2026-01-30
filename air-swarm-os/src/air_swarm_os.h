#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>
#include <array>
#include <string>
#include <unordered_map>
#include <functional>

namespace air_swarm_os {

struct AircraftPosition {
    double latitude;              // degrees
    double longitude;             // degrees
    double altitude;               // meters
    double heading;                // degrees (0-360)
    double airspeed;               // m/s
    double vertical_speed;         // m/s (positive = up)
    double track_angle;            // degrees
    std::chrono::system_clock::time_point timestamp;
    uint32_t aircraft_id;
    std::string flight_id;
    uint8_t aircraft_type;          // 0=jet, 1=helicopter, 2=prop, 3=UAV
    uint8_t priority_level;         // 0=low, 1=medium, 2=high, 3=emergency
};

struct AirspaceVolume {
    double min_lat, max_lat;      // degrees
    double min_lon, max_lon;      // degrees
    double min_alt, max_alt;      // meters
    std::vector<uint32_t> aircraft_ids;
    uint32_t controlling_node;     // aircraft_id of controlling node
    std::chrono::system_clock::time_point last_update;
    uint32_t volume_id;
    bool is_active;
};

struct ConflictResolution {
    uint32_t aircraft1_id;
    uint32_t aircraft2_id;
    double conflict_distance;      // meters
    double time_to_conflict;       // seconds
    std::string resolution_type;     // "vertical", "horizontal", "temporal"
    std::vector<double> new_altitudes; // Proposed altitudes
    std::vector<double> new_headings;   // Proposed headings
    std::chrono::system_clock::time_point resolution_time;
    uint32_t resolution_id;
    bool is_accepted;
};

struct SwarmMessage {
    uint32_t message_id;
    uint32_t sender_id;
    uint32_t target_id;             // 0 = broadcast
    uint8_t message_type;
    std::vector<uint8_t> payload;
    std::chrono::system_clock::time_point timestamp;
    uint64_t signature;             // Digital signature
    uint32_t nonce;                 // Anti-replay
};

struct BlockchainBlock {
    uint32_t block_id;
    uint32_t previous_hash;
    uint32_t merkle_root;           // Hash of transactions
    std::vector<SwarmMessage> transactions;
    std::chrono::system_clock::time_point timestamp;
    uint32_t validator_id;          // Aircraft that validated this block
    uint64_t proof_of_work;         // Consensus mechanism
};

enum class MessageType : uint8_t {
    POSITION_UPDATE = 0x01,
    INTENT_DECLARATION = 0x02,
    CONFLICT_DETECTED = 0x03,
    RESOLUTION_PROPOSAL = 0x04,
    RESOLUTION_ACCEPTED = 0x05,
    VOLUME_ASSIGNMENT = 0x06,
    EMERGENCY_BROADCAST = 0x07,
    HEARTBEAT = 0x08,
    COORDINATION_REQUEST = 0x09,
    ROUTE_CONFIRMATION = 0x0A
};

enum class SwarmRole : uint8_t {
    NODE = 0,
    COORDINATOR = 1,
    VALIDATOR = 2,
    EMERGENCY_CONTROLLER = 3
};

class CryptographicEngine {
public:
    CryptographicEngine();
    ~CryptographicEngine() = default;
    
    bool initialize(uint32_t aircraft_id);
    uint64_t sign_message(const SwarmMessage& message);
    bool verify_signature(const SwarmMessage& message);
    uint32_t calculate_hash(const void* data, size_t size);
    
    void set_private_key(const std::vector<uint8_t>& key);
    void set_public_key(const std::vector<uint8_t>& key);
    
    bool encrypt_message(SwarmMessage& message);
    bool decrypt_message(SwarmMessage& message);

private:
    uint32_t aircraft_id_;
    std::vector<uint8_t> private_key_;
    std::vector<uint8_t> public_key_;
};

class ConsensusMechanism {
public:
    ConsensusMechanism();
    ~ConsensusMechanism() = default;
    
    bool initialize(uint32_t num_nodes);
    bool add_block_candidate(const BlockchainBlock& block);
    bool validate_block(const BlockchainBlock& block);
    BlockchainBlock create_genesis_block();
    
    bool has_consensus();
    uint32_t get_consensus_round();
    void set_target_hash(uint32_t target_hash);

private:
    void calculate_difficulty();
    bool validate_proof_of_work(const BlockchainBlock& block);
    
    uint32_t num_nodes_;
    std::vector<BlockchainBlock> block_candidates_;
    uint32_t consensus_round_;
    uint32_t target_hash_;
    uint32_t difficulty_;
};

class SwarmNetwork {
public:
    SwarmNetwork();
    ~SwarmNetwork() = default;
    
    bool initialize(uint32_t aircraft_id, const std::string& flight_id);
    bool broadcast_message(const SwarmMessage& message);
    bool send_direct_message(uint32_t target_id, const SwarmMessage& message);
    std::vector<SwarmMessage> receive_messages();
    
    void add_peer(uint32_t peer_id, const std::string& address);
    void remove_peer(uint32_t peer_id);
    std::vector<uint32_t> get_connected_peers() const;
    
    bool is_connected() const;
    double get_network_health() const;

private:
    uint32_t aircraft_id_;
    std::string flight_id_;
    std::unordered_map<uint32_t, std::string> peers_;
    std::vector<SwarmMessage> message_queue_;
    bool connected_;
    double network_health_;
};

class AirspaceManager {
public:
    AirspaceManager();
    ~AirspaceManager() = default();
    
    bool initialize(double coverage_radius_km);
    void update_aircraft_position(const AircraftPosition& position);
    std::vector<AircraftPosition> get_nearby_aircraft(const AircraftPosition& position, double radius_km);
    
    std::vector<AirspaceVolume> get_volumes();
    bool assign_volume(uint32_t aircraft_id, AirspaceVolume& volume);
    void release_volume(uint32_t aircraft_id, uint32_t volume_id);
    
    std::vector<ConflictResolution> detect_conflicts();
    bool propose_resolution(const ConflictResolution& resolution);
    bool accept_resolution(uint32_t resolution_id);

private:
    void update_volumes();
    void optimize_volume_allocation();
    
    double coverage_radius_;
    std::vector<AirspaceVolume> volumes_;
    std::unordered_map<uint32_t, uint32_t> aircraft_to_volume_;
    std::vector<AircraftPosition> aircraft_positions_;
};

class ConflictResolver {
public:
    ConflictResolver();
    ~ConflictResolver() = default;
    
    bool initialize();
    std::vector<ConflictResolution> resolve_conflicts(
        const std::vector<AircraftPosition>& aircraft_positions
    );
    
    std::vector<ConflictResolution> propose_vertical_separation(
        const AircraftPosition& aircraft1, const AircraftPosition& aircraft2
    );
    std::vector<ConflictResolution> propose_horizontal_separation(
        const AircraftPosition& aircraft1, const AircraftPosition& aircraft2
    );
    std::vector<ConflictResolution> propose_temporal_separation(
        const AircraftPosition& aircraft1, const AircraftPosition& aircraft2
    );
    
    void set_separation_minima(double vertical_m, double horizontal_m, double temporal_s);
    void set_safety_factor(double factor);

private:
    double calculate_separation_distance(const AircraftPosition& pos1, const AircraftPosition& pos2);
    bool validate_resolution(const ConflictResolution& resolution);
    
    double vertical_minima_;
    double horizontal_minima_;
    double temporal_minima_;
    double safety_factor_;
};

class SwarmCoordinator {
public:
    SwarmCoordinator();
    ~SwarmCoordinator() = default;
    
    bool initialize(uint32_t aircraft_id);
    void set_role(SwarmRole role);
    
    void coordinate_swarm();
    bool validate_swarm_integrity();
    void handle_emergency_situation(uint32_t emergency_aircraft_id);
    
    std::vector<uint32_t> get_coordinated_aircraft() const;
    bool is_coordination_active() const;

private:
    void distribute_tasks();
    void monitor_swarm_health();
    void rebalance_load();
    
    uint32_t aircraft_id_;
    SwarmRole current_role_;
    std::vector<uint32_t> coordinated_aircraft_;
    bool coordination_active_;
    std::chrono::system_clock::time_point last_coordination_;
};

class AirSwarmOS {
public:
    AirSwarmOS();
    ~AirSwarmOS() = default;
    
    bool initialize(uint32_t aircraft_id, const std::string& flight_id, SwarmRole role);
    void shutdown();
    
    // Main swarm loop
    bool update_swarm_state(const AircraftPosition& current_position);
    
    // Network operations
    bool broadcast_position(const AircraftPosition& position);
    std::vector<AircraftPosition> get_swarm_positions();
    bool send_emergency_alert(const std::string& emergency_type);
    
    // Blockchain operations
    bool add_transaction(const SwarmMessage& message);
    bool validate_blockchain();
    BlockchainBlock get_latest_block();
    
    // Airspace management
    bool request_volume_assignment();
    bool release_current_volume();
    std::vector<ConflictResolution> get_pending_conflicts();
    
    // Configuration
    void set_swarm_parameters(double communication_range_km, double update_frequency_hz);
    void set_safety_parameters(double min_separation_m, double max_descent_rate);
    void enable_autonomous_mode(bool enable);
    
    // Monitoring
    bool is_swarm_healthy() const;
    double get_swarm_efficiency() const;
    std::vector<std::string> get_system_warnings() const;

private:
    void update_blockchain();
    void process_messages();
    void monitor_system_health();
    void handle_network_failures();
    void optimize_swarm_performance();
    
    void update_aircraft_position(const AircraftPosition& position);
    void check_for_conflicts();
    void resolve_detected_conflicts();
    
    uint32_t aircraft_id_;
    std::string flight_id_;
    SwarmRole current_role_;
    
    std::unique_ptr<CryptographicEngine> crypto_engine_;
    std::unique_ptr<ConsensusMechanism> consensus_;
    std::unique_ptr<SwarmNetwork> network_;
    std::unique_ptr<AirspaceManager> airspace_manager_;
    std::unique_ptr<ConflictResolver> conflict_resolver_;
    std::unique_ptr<SwarmCoordinator> coordinator_;
    
    BlockchainBlock latest_block_;
    std::vector<BlockchainBlock> blockchain_;
    AircraftPosition current_position_;
    
    bool autonomous_mode_enabled_;
    double communication_range_;
    double update_frequency_;
    double min_separation_;
    double max_descent_rate_;
    
    bool system_healthy_;
    double swarm_efficiency_;
    std::vector<std::string> system_warnings_;
    
    std::chrono::system_clock::time_point last_update_time_;
    std::chrono::microseconds target_update_period_{100000}; // 100ms target
};

} // namespace air_swarm_os
