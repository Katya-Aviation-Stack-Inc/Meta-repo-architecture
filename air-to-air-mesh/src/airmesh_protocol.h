#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <chrono>

namespace aamn {

struct AircraftPosition {
    double latitude;
    double longitude;
    double altitude;
    double velocity_x;
    double velocity_y;
    double velocity_z;
    std::chrono::system_clock::time_point timestamp;
    uint32_t aircraft_id;
};

struct MeshPacket {
    uint32_t source_id;
    uint32_t destination_id;
    uint32_t sequence_number;
    uint8_t packet_type;
    uint8_t priority;  // 0-7 priority level for packet handling
    uint16_t ttl;     // Time to live in hops
    std::vector<uint8_t> payload;
    std::chrono::system_clock::time_point timestamp;
    std::vector<uint32_t> path_history;  // Track routing path
};

struct TrafficManagementZone {
    uint32_t zone_id;
    double min_latitude, max_latitude;
    double min_longitude, max_longitude;
    double min_altitude, max_altitude;
    uint32_t managing_aircraft;  // Aircraft ID responsible for zone
    std::chrono::system_clock::time_point valid_until;
    uint8_t congestion_level;  // 0-100 scale
};

struct WeatherUpdate {
    double latitude, longitude;
    double wind_speed;     // m/s
    double wind_direction; // degrees
    double temperature;    // Celsius
    double pressure;       // hPa
    double visibility;     // meters
    uint8_t precipitation; // 0-100 scale
    std::chrono::system_clock::time_point timestamp;
};

struct RouteSegment {
    double latitude;
    double longitude;
    double altitude;
    double speed_restriction;  // m/s, 0 = no restriction
    std::chrono::system_clock::time_point estimated_time;
};

enum class PacketType : uint8_t {
    POSITION_UPDATE = 0x01,
    VECTOR_MANEUVER = 0x02,
    COORDINATION_REQUEST = 0x03,
    COORDINATION_RESPONSE = 0x04,
    EMERGENCY_BROADCAST = 0x05,
    HEARTBEAT = 0x06
};

class RFTransceiver {
public:
    virtual ~RFTransceiver() = default;
    virtual bool initialize(double frequency_mhz) = 0;
    virtual bool transmit(const MeshPacket& packet) = 0;
    virtual std::vector<MeshPacket> receive() = 0;
    virtual double get_signal_strength() const = 0;
    virtual bool is_connected() const = 0;
};

class MeshNetworkNode {
public:
    MeshNetworkNode(uint32_t aircraft_id, std::unique_ptr<RFTransceiver> transceiver);
    ~MeshNetworkNode() = default;

    bool initialize();
    void update_position(const AircraftPosition& position);
    void broadcast_position();
    void process_incoming_packets();
    std::vector<AircraftPosition> get_nearby_aircraft() const;
    
    bool send_maneuver_coordination(uint32_t target_id, const std::vector<uint8_t>& maneuver_data);
    bool request_route_coordination(const std::vector<uint32_t>& affected_aircraft);
    
    void set_emergency_mode(bool emergency);
    bool is_emergency_active() const;
    
    // New enhanced capabilities
    bool broadcast_traffic_zone(const TrafficManagementZone& zone);
    bool share_route(const std::vector<RouteSegment>& route);
    bool send_weather_update(const WeatherUpdate& weather);
    bool request_priority_clearance(uint32_t priority_level);
    
    void set_frequency_band(double frequency_mhz);
    double get_current_frequency() const;
    
    std::vector<TrafficManagementZone> get_local_traffic_zones() const;
    std::vector<WeatherUpdate> get_local_weather() const;
    
    bool establish_swarm_network(const std::vector<uint32_t>& participant_ids);
    bool send_swarm_command(uint32_t command_type, const std::vector<uint8_t>& data);

private:
    void handle_position_update(const MeshPacket& packet);
    void handle_maneuver_coordination(const MeshPacket& packet);
    void handle_coordination_request(const MeshPacket& packet);
    void handle_emergency_broadcast(const MeshPacket& packet);
    void send_heartbeat();
    double calculate_distance(const AircraftPosition& pos1, const AircraftPosition& pos2);
    
    // New handlers for enhanced capabilities
    void handle_traffic_zone(const MeshPacket& packet);
    void handle_weather_update(const MeshPacket& packet);
    void handle_route_sharing(const MeshPacket& packet);
    void handle_swarm_coordination(const MeshPacket& packet);
    void handle_priority_request(const MeshPacket& packet);
    void handle_clearance_ack(const MeshPacket& packet);
    
    // Helper functions
    bool forward_packet(const MeshPacket& packet);
    MeshPacket create_mesh_packet(uint8_t packet_type, uint32_t destination_id,
                                 const std::vector<uint8_t>& payload);
    
    uint32_t aircraft_id_;
    std::unique_ptr<RFTransceiver> transceiver_;
    AircraftPosition current_position_;
    std::vector<AircraftPosition> nearby_aircraft_;
    std::vector<MeshPacket> packet_queue_;
    uint32_t sequence_number_;
    bool emergency_mode_;
    std::chrono::system_clock::time_point last_heartbeat_;
    
    // New data structures for enhanced capabilities
    std::vector<TrafficManagementZone> traffic_zones_;
    std::vector<WeatherUpdate> weather_updates_;
    std::vector<std::vector<RouteSegment>> shared_routes_;
    std::vector<uint32_t> swarm_participants_;
};

class DistributedAutopilot {
public:
    DistributedAutopilot(MeshNetworkNode& network_node);
    ~DistributedAutopilot() = default;
    
    bool enable_swarm_coordination();
    void disable_swarm_coordination();
    bool is_swarm_active() const;
    
    std::vector<uint32_t> get_conflicting_aircraft() const;
    bool negotiate_separation(uint32_t conflicting_aircraft_id);
    bool execute_collective_maneuver(const std::vector<uint32_t>& participants);

private:
    void analyze_traffic_conflicts();
    void propose_separation_solution(uint32_t conflict_id);
    void validate_maneuver_safety(const std::vector<uint32_t>& participants);
    
    MeshNetworkNode& network_node_;
    bool swarm_coordination_enabled_;
    std::vector<uint32_t> conflicting_aircraft_;
    std::chrono::system_clock::time_point last_conflict_analysis_;
};

} // namespace aamn
