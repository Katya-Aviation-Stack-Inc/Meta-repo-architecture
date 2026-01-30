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
    std::vector<uint8_t> payload;
    std::chrono::system_clock::time_point timestamp;
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

private:
    void handle_position_update(const MeshPacket& packet);
    void handle_maneuver_coordination(const MeshPacket& packet);
    void handle_coordination_request(const MeshPacket& packet);
    void handle_emergency_broadcast(const MeshPacket& packet);
    void send_heartbeat();
    double calculate_distance(const AircraftPosition& pos1, const AircraftPosition& pos2);
    
    uint32_t aircraft_id_;
    std::unique_ptr<RFTransceiver> transceiver_;
    AircraftPosition current_position_;
    std::vector<AircraftPosition> nearby_aircraft_;
    std::vector<MeshPacket> packet_queue_;
    uint32_t sequence_number_;
    bool emergency_mode_;
    std::chrono::system_clock::time_point last_heartbeat_;
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
