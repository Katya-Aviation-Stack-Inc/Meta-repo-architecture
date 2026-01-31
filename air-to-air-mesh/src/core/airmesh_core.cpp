#include "airmesh_core.h"
#include <algorithm>
#include <iostream>
#include <cstring>
#include <cmath>

namespace aamn {

ZeroLatencyMeshNode::ZeroLatencyMeshNode(uint32_t aircraft_id, 
                                         std::unique_ptr<ZeroLatencyRFTransceiver> transceiver)
    : aircraft_id_(aircraft_id)
    , transceiver_(std::move(transceiver))
    , sequence_number_(0)
    , emergency_mode_(false)
    , last_heartbeat_(std::chrono::system_clock::now()) {
    
    // Initialize current position with default values
    current_position_ = ZeroLatencyAircraftPosition{
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0,           // Position and velocity
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0,           // Orientation and rates
        0.0, 0.0, 0.0,                          // Predictive position
        aircraft_id_, 1, 50, false, false,       // ID, type, priority, flags
        {}, {}, 0.0,                            // Swarm data
        std::chrono::system_clock::now(), 
        std::chrono::system_clock::now() + std::chrono::milliseconds(100)  // Prediction timestamp
    };
    
    // Initialize network performance metrics
    network_performance_ = NetworkPerformance{0, 0, 0, 0.0, 0.0, 0, 0.0};
}

bool ZeroLatencyMeshNode::initialize() {
    if (!transceiver_) {
        return false;
    }
    
    // Initialize RF transceiver on aviation frequency band (e.g., 978 MHz for ADS-B)
    bool success = transceiver_->initialize(978.0);
    if (success) {
        std::cout << "Revolutionary AAMN Node " << aircraft_id_ << " initialized successfully" << std::endl;
        
        // Enable zero-latency mode for revolutionary performance
        transceiver_->enable_zero_latency_mode();
        
        // Enable encryption for secure communication
        transceiver_->enable_encryption();
    }
    return success;
}

void ZeroLatencyMeshNode::update_position(const ZeroLatencyAircraftPosition& position) {
    current_position_ = position;
    current_position_.aircraft_id = aircraft_id_;
    current_position_.timestamp = std::chrono::system_clock::now();
    
    // Calculate predictive position (100ms into future)
    current_position_.prediction_timestamp = current_position_.timestamp + std::chrono::milliseconds(100);
    current_position_.predicted_latitude = current_position_.latitude + 
        (current_position_.velocity_x * 0.1 / 111000.0);  // Rough conversion
    current_position_.predicted_longitude = current_position_.longitude + 
        (current_position_.velocity_y * 0.1 / 111000.0);
    current_position_.predicted_altitude = current_position_.altitude + 
        (current_position_.velocity_z * 0.1);
}

void ZeroLatencyMeshNode::broadcast_position() {
    // Broadcast current position
    ZeroLatencyMeshPacket packet = create_mesh_packet(
        static_cast<uint8_t>(ZeroLatencyPacketType::POSITION_UPDATE),
        0xFFFFFFFF,  // Broadcast
        std::vector<uint8_t>()
    );
    
    // Serialize position data
    packet.payload.resize(sizeof(ZeroLatencyAircraftPosition));
    std::memcpy(packet.payload.data(), &current_position_, sizeof(ZeroLatencyAircraftPosition));
    
    if (transceiver_->transmit(packet)) {
        network_performance_.packets_sent++;
        std::cout << "Revolutionary position broadcast sent from aircraft " << aircraft_id_ << std::endl;
    } else {
        network_performance_.packets_dropped++;
    }
    
    // Also broadcast predictive position for zero-latency coordination
    ZeroLatencyMeshPacket pred_packet = create_mesh_packet(
        static_cast<uint8_t>(ZeroLatencyPacketType::PREDICTIVE_POSITION),
        0xFFFFFFFF,  // Broadcast
        std::vector<uint8_t>()
    );
    
    // Serialize predictive position data
    pred_packet.payload.resize(sizeof(ZeroLatencyAircraftPosition));
    std::memcpy(pred_packet.payload.data(), &current_position_, sizeof(ZeroLatencyAircraftPosition));
    
    if (transceiver_->transmit(pred_packet)) {
        network_performance_.packets_sent++;
        std::cout << "Revolutionary predictive position broadcast sent from aircraft " << aircraft_id_ << std::endl;
    } else {
        network_performance_.packets_dropped++;
    }
}

void ZeroLatencyMeshNode::process_incoming_packets() {
    auto packets = transceiver_->receive();
    network_performance_.packets_received += packets.size();
    
    for (const auto& packet : packets) {
        // Check if packet is for this aircraft or broadcast
        if (packet.destination_id == aircraft_id_ || 
            packet.destination_id == 0xFFFFFFFF) {
            
            // Handle different packet types
            switch (static_cast<ZeroLatencyPacketType>(packet.packet_type)) {
                case ZeroLatencyPacketType::POSITION_UPDATE:
                    handle_position_update(packet);
                    break;
                case ZeroLatencyPacketType::PREDICTIVE_POSITION:
                    handle_predictive_position(packet);
                    break;
                case ZeroLatencyPacketType::MANEUVER_INTENT:
                    handle_maneuver_intent(packet);
                    break;
                case ZeroLatencyPacketType::COORDINATION_REQUEST:
                    handle_coordination_request(packet);
                    break;
                case ZeroLatencyPacketType::EMERGENCY_BROADCAST:
                    handle_emergency_broadcast(packet);
                    break;
                case ZeroLatencyPacketType::SWARM_SYNCHRONIZATION:
                    handle_swarm_synchronization(packet);
                    break;
                case ZeroLatencyPacketType::TRAFFIC_MANAGEMENT_ZONE:
                    handle_traffic_zone(packet);
                    break;
                case ZeroLatencyPacketType::WEATHER_UPDATE:
                    handle_weather_update(packet);
                    break;
                case ZeroLatencyPacketType::ROUTE_SHARING:
                    handle_route_sharing(packet);
                    break;
                case ZeroLatencyPacketType::SWARM_COMMAND:
                    handle_swarm_command(packet);
                    break;
                case ZeroLatencyPacketType::PRIORITY_REQUEST:
                    handle_priority_request(packet);
                    break;
                case ZeroLatencyPacketType::CLEARANCE_ACK:
                    handle_clearance_ack(packet);
                    break;
                case ZeroLatencyPacketType::HEARTBEAT:
                    // Update last seen time for source aircraft
                    break;
                default:
                    // Forward unknown packets if TTL allows
                    if (packet.ttl > 1) {
                        forward_packet(packet);
                    }
                    break;
            }
        } else {
            // Forward packets not destined for this node
            if (packet.ttl > 1) {
                forward_packet(packet);
            }
        }
    }
    
    // Remove stale aircraft data (older than 30 seconds)
    auto now = std::chrono::system_clock::now();
    nearby_aircraft_.erase(
        std::remove_if(nearby_aircraft_.begin(), nearby_aircraft_.end(),
            [now](const ZeroLatencyAircraftPosition& pos) {
                return std::chrono::duration_cast<std::chrono::seconds>(
                    now - pos.timestamp).count() > 30;
            }),
        nearby_aircraft_.end()
    );
    
    // Send periodic heartbeat
    if (std::chrono::duration_cast<std::chrono::seconds>(now - last_heartbeat_).count() > 5) {
        ZeroLatencyMeshPacket packet = create_mesh_packet(
            static_cast<uint8_t>(ZeroLatencyPacketType::HEARTBEAT),
            0xFFFFFFFF,  // Broadcast
            std::vector<uint8_t>()
        );
        transceiver_->transmit(packet);
        last_heartbeat_ = now;
    }
}

std::vector<ZeroLatencyAircraftPosition> ZeroLatencyMeshNode::get_nearby_aircraft() const {
    return nearby_aircraft_;
}

bool ZeroLatencyMeshNode::send_maneuver_intent(uint32_t target_id, const std::vector<double>& intent_vector) {
    if (intent_vector.size() != 3) {
        return false;  // Intent vector must have 3 components (x, y, z)
    }
    
    ZeroLatencyMeshPacket packet = create_mesh_packet(
        static_cast<uint8_t>(ZeroLatencyPacketType::MANEUVER_INTENT),
        target_id,
        std::vector<uint8_t>()
    );
    
    // Serialize intent vector
    packet.payload.resize(intent_vector.size() * sizeof(double));
    std::memcpy(packet.payload.data(), intent_vector.data(), packet.payload.size());
    
    bool success = transceiver_->transmit(packet);
    if (success) {
        network_performance_.packets_sent++;
    } else {
        network_performance_.packets_dropped++;
    }
    
    return success;
}

bool ZeroLatencyMeshNode::request_route_coordination(const std::vector<uint32_t>& affected_aircraft) {
    bool success = true;
    
    for (uint32_t target_id : affected_aircraft) {
        ZeroLatencyMeshPacket packet = create_mesh_packet(
            static_cast<uint8_t>(ZeroLatencyPacketType::COORDINATION_REQUEST),
            target_id,
            std::vector<uint8_t>()
        );
        
        // Serialize affected aircraft list
        packet.payload.resize(affected_aircraft.size() * sizeof(uint32_t));
        std::memcpy(packet.payload.data(), affected_aircraft.data(), packet.payload.size());
        
        bool result = transceiver_->transmit(packet);
        if (result) {
            network_performance_.packets_sent++;
        } else {
            network_performance_.packets_dropped++;
        }
        success &= result;
    }
    
    return success;
}

void ZeroLatencyMeshNode::set_emergency_mode(bool emergency) {
    emergency_mode_ = emergency;
    
    if (emergency) {
        // Broadcast emergency signal to all nearby aircraft
        ZeroLatencyMeshPacket packet = create_mesh_packet(
            static_cast<uint8_t>(ZeroLatencyPacketType::EMERGENCY_BROADCAST),
            0xFFFFFFFF,  // Broadcast
            std::vector<uint8_t>()
        );
        
        // Include emergency reason and position
        std::string emergency_reason = "EMERGENCY";
        packet.payload.resize(emergency_reason.size() + sizeof(ZeroLatencyAircraftPosition));
        std::memcpy(packet.payload.data(), emergency_reason.c_str(), emergency_reason.size());
        std::memcpy(packet.payload.data() + emergency_reason.size(), 
                    &current_position_, sizeof(ZeroLatencyAircraftPosition));
        
        if (transceiver_->transmit(packet)) {
            network_performance_.packets_sent++;
            std::cout << "Revolutionary emergency broadcast sent from aircraft " << aircraft_id_ << std::endl;
        } else {
            network_performance_.packets_dropped++;
        }
    }
}

bool ZeroLatencyMeshNode::is_emergency_active() const {
    return emergency_mode_;
}

bool ZeroLatencyMeshNode::broadcast_traffic_zone(const ZeroLatencyTrafficZone& zone) {
    ZeroLatencyMeshPacket packet = create_mesh_packet(
        static_cast<uint8_t>(ZeroLatencyPacketType::TRAFFIC_MANAGEMENT_ZONE),
        0xFFFFFFFF,  // Broadcast
        std::vector<uint8_t>()
    );
    
    // Serialize zone data
    packet.payload.resize(sizeof(ZeroLatencyTrafficZone));
    std::memcpy(packet.payload.data(), &zone, sizeof(ZeroLatencyTrafficZone));
    
    bool success = transceiver_->transmit(packet);
    if (success) {
        network_performance_.packets_sent++;
    } else {
        network_performance_.packets_dropped++;
    }
    
    return success;
}

bool ZeroLatencyMeshNode::share_route(const std::vector<ZeroLatencyRouteSegment>& route) {
    ZeroLatencyMeshPacket packet = create_mesh_packet(
        static_cast<uint8_t>(ZeroLatencyPacketType::ROUTE_SHARING),
        0xFFFFFFFF,  // Broadcast to all
        std::vector<uint8_t>()
    );
    
    // Serialize route data
    size_t route_size = route.size() * sizeof(ZeroLatencyRouteSegment);
    packet.payload.resize(sizeof(size_t) + route_size);
    
    // First store the number of segments
    size_t route_count = route.size();
    std::memcpy(packet.payload.data(), &route_count, sizeof(size_t));
    
    // Then store the route segments
    std::memcpy(packet.payload.data() + sizeof(size_t), route.data(), route_size);
    
    bool success = transceiver_->transmit(packet);
    if (success) {
        network_performance_.packets_sent++;
    } else {
        network_performance_.packets_dropped++;
    }
    
    return success;
}

bool ZeroLatencyMeshNode::send_weather_update(const ZeroLatencyWeather& weather) {
    ZeroLatencyMeshPacket packet = create_mesh_packet(
        static_cast<uint8_t>(ZeroLatencyPacketType::WEATHER_UPDATE),
        0xFFFFFFFF,  // Broadcast
        std::vector<uint8_t>()
    );
    
    // Serialize weather data
    packet.payload.resize(sizeof(ZeroLatencyWeather));
    std::memcpy(packet.payload.data(), &weather, sizeof(ZeroLatencyWeather));
    
    bool success = transceiver_->transmit(packet);
    if (success) {
        network_performance_.packets_sent++;
    } else {
        network_performance_.packets_dropped++;
    }
    
    return success;
}

bool ZeroLatencyMeshNode::request_priority_clearance(uint32_t priority_level) {
    ZeroLatencyMeshPacket packet = create_mesh_packet(
        static_cast<uint8_t>(ZeroLatencyPacketType::PRIORITY_REQUEST),
        0xFFFFFFFF,  // Broadcast to all
        std::vector<uint8_t>()
    );
    
    // Serialize priority level
    packet.payload.resize(sizeof(uint32_t));
    std::memcpy(packet.payload.data(), &priority_level, sizeof(uint32_t));
    
    bool success = transceiver_->transmit(packet);
    if (success) {
        network_performance_.packets_sent++;
    } else {
        network_performance_.packets_dropped++;
    }
    
    return success;
}

void ZeroLatencyMeshNode::set_frequency_band(double frequency_mhz) {
    transceiver_->set_frequency(frequency_mhz);
}

double ZeroLatencyMeshNode::get_current_frequency() const {
    return transceiver_->get_frequency();
}

bool ZeroLatencyMeshNode::enable_zero_latency_mode() {
    return transceiver_->enable_zero_latency_mode();
}

bool ZeroLatencyMeshNode::enable_encryption() {
    return transceiver_->enable_encryption();
}

std::vector<ZeroLatencyTrafficZone> ZeroLatencyMeshNode::get_local_traffic_zones() const {
    return traffic_zones_;
}

std::vector<ZeroLatencyWeather> ZeroLatencyMeshNode::get_local_weather() const {
    return weather_updates_;
}

std::vector<std::vector<ZeroLatencyRouteSegment>> ZeroLatencyMeshNode::get_shared_routes() const {
    return shared_routes_;
}

bool ZeroLatencyMeshNode::establish_swarm_network(const std::vector<uint32_t>& participant_ids) {
    swarm_participants_ = participant_ids;
    
    // Send swarm coordination packet to all participants
    ZeroLatencyMeshPacket packet = create_mesh_packet(
        static_cast<uint8_t>(ZeroLatencyPacketType::SWARM_SYNCHRONIZATION),
        0xFFFFFFFF,  // Broadcast to all
        std::vector<uint8_t>()
    );
    
    // Serialize participant list
    size_t participant_count = participant_ids.size();
    packet.payload.resize(sizeof(size_t) + participant_count * sizeof(uint32_t));
    
    // Store participant count
    std::memcpy(packet.payload.data(), &participant_count, sizeof(size_t));
    
    // Store participant IDs
    std::memcpy(packet.payload.data() + sizeof(size_t), 
                participant_ids.data(), 
                participant_count * sizeof(uint32_t));
    
    bool success = transceiver_->transmit(packet);
    if (success) {
        network_performance_.packets_sent++;
    } else {
        network_performance_.packets_dropped++;
    }
    
    return success;
}

bool ZeroLatencyMeshNode::send_swarm_command(uint32_t command_type, const std::vector<uint8_t>& data) {
    if (swarm_participants_.empty()) {
        return false;
    }
    
    ZeroLatencyMeshPacket packet = create_mesh_packet(
        static_cast<uint8_t>(ZeroLatencyPacketType::SWARM_COMMAND),
        0xFFFFFFFF,  // Broadcast to swarm participants
        std::vector<uint8_t>()
    );
    
    // Serialize command with data
    size_t header_size = 2 * sizeof(uint32_t); // command_type + data_size
    packet.payload.resize(header_size + data.size());
    
    // Store command type and data size
    std::memcpy(packet.payload.data(), &command_type, sizeof(uint32_t));
    size_t data_size = data.size();
    std::memcpy(packet.payload.data() + sizeof(uint32_t), &data_size, sizeof(size_t));
    
    // Store command data
    if (!data.empty()) {
        std::memcpy(packet.payload.data() + header_size, data.data(), data.size());
    }
    
    bool success = transceiver_->transmit(packet);
    if (success) {
        network_performance_.packets_sent++;
    } else {
        network_performance_.packets_dropped++;
    }
    
    return success;
}

ZeroLatencyMeshNode::NetworkPerformance ZeroLatencyMeshNode::get_network_performance() const {
    return network_performance_;
}

// Packet handlers
void ZeroLatencyMeshNode::handle_position_update(const ZeroLatencyMeshPacket& packet) {
    if (packet.payload.size() >= sizeof(ZeroLatencyAircraftPosition)) {
        ZeroLatencyAircraftPosition position;
        std::memcpy(&position, packet.payload.data(), sizeof(ZeroLatencyAircraftPosition));
        
        // Update or add aircraft position
        auto it = std::find_if(nearby_aircraft_.begin(), nearby_aircraft_.end(),
            [&position](const ZeroLatencyAircraftPosition& existing) {
                return existing.aircraft_id == position.aircraft_id;
            });
        
        if (it != nearby_aircraft_.end()) {
            *it = position; // Update existing
        } else {
            nearby_aircraft_.push_back(position); // Add new
        }
        
        std::cout << "Received revolutionary position update from aircraft " << position.aircraft_id << std::endl;
    }
}

void ZeroLatencyMeshNode::handle_predictive_position(const ZeroLatencyMeshPacket& packet) {
    if (packet.payload.size() >= sizeof(ZeroLatencyAircraftPosition)) {
        ZeroLatencyAircraftPosition position;
        std::memcpy(&position, packet.payload.data(), sizeof(ZeroLatencyAircraftPosition));
        
        // Process predictive position for zero-latency coordination
        std::cout << "Received revolutionary predictive position from aircraft " << position.aircraft_id << std::endl;
        
        // TODO: Implement predictive conflict detection and avoidance
    }
}

void ZeroLatencyMeshNode::handle_maneuver_intent(const ZeroLatencyMeshPacket& packet) {
    std::cout << "Received maneuver intent from aircraft " << packet.source_id << std::endl;
    // TODO: Implement maneuver coordination logic
}

void ZeroLatencyMeshNode::handle_coordination_request(const ZeroLatencyMeshPacket& packet) {
    std::cout << "Received coordination request from aircraft " << packet.source_id << std::endl;
    
    // Send coordination response
    ZeroLatencyMeshPacket response = create_mesh_packet(
        static_cast<uint8_t>(ZeroLatencyPacketType::COORDINATION_RESPONSE),
        packet.source_id,
        std::vector<uint8_t>()
    );
    
    std::string response_msg = "COORDINATION_ACK";
    response.payload.resize(response_msg.size());
    std::memcpy(response.payload.data(), response_msg.c_str(), response_msg.size());
    
    bool success = transceiver_->transmit(response);
    if (success) {
        network_performance_.packets_sent++;
    } else {
        network_performance_.packets_dropped++;
    }
}

void ZeroLatencyMeshNode::handle_emergency_broadcast(const ZeroLatencyMeshPacket& packet) {
    std::cout << "REVOLUTIONARY EMERGENCY: Received emergency broadcast from aircraft " << packet.source_id << std::endl;
    
    // Immediately analyze for collision avoidance
    if (packet.payload.size() >= sizeof(ZeroLatencyAircraftPosition)) {
        ZeroLatencyAircraftPosition emergency_position;
        std::memcpy(&emergency_position, packet.payload.data(), packet.payload.size());
        
        // Calculate distance and potential collision course
        double distance = calculate_distance(current_position_, emergency_position);
        if (distance < 5000.0) { // Within 5km - immediate concern
            std::cout << "WARNING: Emergency aircraft within " << distance << " meters" << std::endl;
            // TODO: Implement emergency avoidance maneuver
        }
    }
}

void ZeroLatencyMeshNode::handle_swarm_synchronization(const ZeroLatencyMeshPacket& packet) {
    std::cout << "Received swarm synchronization from aircraft " << packet.source_id << std::endl;
    // TODO: Implement swarm synchronization logic
}

void ZeroLatencyMeshNode::handle_traffic_zone(const ZeroLatencyMeshPacket& packet) {
    if (packet.payload.size() >= sizeof(ZeroLatencyTrafficZone)) {
        ZeroLatencyTrafficZone zone;
        std::memcpy(&zone, packet.payload.data(), sizeof(ZeroLatencyTrafficZone));
        
        // Add or update zone
        auto it = std::find_if(traffic_zones_.begin(), traffic_zones_.end(),
            [&zone](const ZeroLatencyTrafficZone& existing) {
                return existing.zone_id == zone.zone_id;
            });
        
        if (it != traffic_zones_.end()) {
            *it = zone; // Update existing
        } else {
            traffic_zones_.push_back(zone); // Add new
        }
        
        std::cout << "Received revolutionary traffic management zone " << zone.zone_id << std::endl;
    }
}

void ZeroLatencyMeshNode::handle_weather_update(const ZeroLatencyMeshPacket& packet) {
    if (packet.payload.size() >= sizeof(ZeroLatencyWeather)) {
        ZeroLatencyWeather weather;
        std::memcpy(&weather, packet.payload.data(), sizeof(ZeroLatencyWeather));
        weather_updates_.push_back(weather);
        std::cout << "Received revolutionary weather update for location" << std::endl;
    }
}

void ZeroLatencyMeshNode::handle_route_sharing(const ZeroLatencyMeshPacket& packet) {
    if (packet.payload.size() >= sizeof(size_t)) {
        size_t route_count;
        std::memcpy(&route_count, packet.payload.data(), sizeof(size_t));
        
        if (packet.payload.size() >= sizeof(size_t) + route_count * sizeof(ZeroLatencyRouteSegment)) {
            std::vector<ZeroLatencyRouteSegment> route(route_count);
            std::memcpy(route.data(), 
                       packet.payload.data() + sizeof(size_t), 
                       route_count * sizeof(ZeroLatencyRouteSegment));
            
            shared_routes_.push_back(route);
            std::cout << "Received shared route with " << route_count << " segments" << std::endl;
        }
    }
}

void ZeroLatencyMeshNode::handle_swarm_command(const ZeroLatencyMeshPacket& packet) {
    std::cout << "Received swarm command from aircraft " << packet.source_id << std::endl;
    // TODO: Implement swarm command handling
}

void ZeroLatencyMeshNode::handle_priority_request(const ZeroLatencyMeshPacket& packet) {
    if (packet.payload.size() >= sizeof(uint32_t)) {
        uint32_t priority_level;
        std::memcpy(&priority_level, packet.payload.data(), sizeof(uint32_t));
        std::cout << "Received priority request (level " << priority_level << ") from aircraft " 
                  << packet.source_id << std::endl;
        // TODO: Implement priority handling logic
    }
}

void ZeroLatencyMeshNode::handle_clearance_ack(const ZeroLatencyMeshPacket& packet) {
    std::cout << "Received clearance acknowledgment from aircraft " << packet.source_id << std::endl;
    // TODO: Implement clearance acknowledgment handling
}

bool ZeroLatencyMeshNode::forward_packet(const ZeroLatencyMeshPacket& packet) {
    // Decrement TTL and forward
    ZeroLatencyMeshPacket forwarded_packet = packet;
    if (forwarded_packet.ttl > 0) {
        forwarded_packet.ttl--;
        forwarded_packet.path_history.push_back(aircraft_id_);
        bool success = transceiver_->transmit(forwarded_packet);
        if (success) {
            network_performance_.packets_sent++;
        } else {
            network_performance_.packets_dropped++;
        }
        return success;
    }
    return false;
}

ZeroLatencyMeshPacket ZeroLatencyMeshNode::create_mesh_packet(uint8_t packet_type, uint32_t destination_id, 
                                                               const std::vector<uint8_t>& payload) {
    ZeroLatencyMeshPacket packet;
    packet.source_id = aircraft_id_;
    packet.destination_id = destination_id;
    packet.sequence_number = sequence_number_++;
    packet.packet_type = packet_type;
    packet.priority = 0;  // Default priority
    packet.ttl = 10;       // Default time to live
    packet.payload = payload;
    packet.timestamp = std::chrono::system_clock::now();
    packet.expiry_time = packet.timestamp + std::chrono::seconds(10); // Expire in 10 seconds
    return packet;
}

double ZeroLatencyMeshNode::calculate_distance(const ZeroLatencyAircraftPosition& pos1, 
                                               const ZeroLatencyAircraftPosition& pos2) {
    // Simplified distance calculation (should use proper aviation formulas)
    double dx = pos1.latitude - pos2.latitude;
    double dy = pos1.longitude - pos2.longitude;
    double dz = pos1.altitude - pos2.altitude;
    return std::sqrt(dx*dx + dy*dy + dz*dz) * 111000; // Rough conversion to meters
}

double ZeroLatencyMeshNode::calculate_predictive_conflict_probability(
    const ZeroLatencyAircraftPosition& pos1, 
    const ZeroLatencyAircraftPosition& pos2) {
    // Calculate probability of conflict based on current and predicted positions
    double current_distance = calculate_distance(pos1, pos2);
    double predicted_distance = calculate_distance(
        ZeroLatencyAircraftPosition{pos1.predicted_latitude, pos1.predicted_longitude, pos1.predicted_altitude},
        ZeroLatencyAircraftPosition{pos2.predicted_latitude, pos2.predicted_longitude, pos2.predicted_altitude}
    );
    
    // Simple conflict probability model
    double conflict_threshold = 1000.0; // 1km minimum safe distance
    if (predicted_distance < conflict_threshold) {
        return 1.0 - (predicted_distance / conflict_threshold);
    }
    return 0.0;
}

} // namespace aamn