#include "airmesh_protocol.h"
#include <algorithm>
#include <iostream>
#include <random>

namespace aamn {

MeshNetworkNode::MeshNetworkNode(uint32_t aircraft_id, std::unique_ptr<RFTransceiver> transceiver)
    : aircraft_id_(aircraft_id)
    , transceiver_(std::move(transceiver))
    , sequence_number_(0)
    , emergency_mode_(false)
    , last_heartbeat_(std::chrono::system_clock::now()) {
    
    current_position_ = AircraftPosition{
        0.0, 0.0, 0.0, 0.0, 0.0, 0.0,
        std::chrono::system_clock::now(), aircraft_id_
    };
}

bool MeshNetworkNode::initialize() {
    if (!transceiver_) {
        return false;
    }
    
    // Initialize RF transceiver on aviation frequency band (e.g., 978 MHz for ADS-B)
    bool success = transceiver_->initialize(978.0);
    if (success) {
        std::cout << "AAMN Node " << aircraft_id_ << " initialized successfully" << std::endl;
    }
    return success;
}

void MeshNetworkNode::update_position(const AircraftPosition& position) {
    current_position_ = position;
    current_position_.aircraft_id = aircraft_id_;
    current_position_.timestamp = std::chrono::system_clock::now();
}

void MeshNetworkNode::broadcast_position() {
    MeshPacket packet;
    packet.source_id = aircraft_id_;
    packet.destination_id = 0xFFFFFFFF; // Broadcast address
    packet.sequence_number = sequence_number_++;
    packet.packet_type = static_cast<uint8_t>(PacketType::POSITION_UPDATE);
    packet.timestamp = std::chrono::system_clock::now();
    
    // Serialize position data
    packet.payload.resize(sizeof(AircraftPosition));
    std::memcpy(packet.payload.data(), &current_position_, sizeof(AircraftPosition));
    
    if (transceiver_->transmit(packet)) {
        std::cout << "Position broadcast sent from aircraft " << aircraft_id_ << std::endl;
    }
}

void MeshNetworkNode::process_incoming_packets() {
    auto packets = transceiver_->receive();
    
    for (const auto& packet : packets) {
        if (packet.destination_id == aircraft_id_ || 
            packet.destination_id == 0xFFFFFFFF) { // Broadcast
            
            switch (static_cast<PacketType>(packet.packet_type)) {
                case PacketType::POSITION_UPDATE:
                    handle_position_update(packet);
                    break;
                case PacketType::VECTOR_MANEUVER:
                    handle_maneuver_coordination(packet);
                    break;
                case PacketType::COORDINATION_REQUEST:
                    handle_coordination_request(packet);
                    break;
                case PacketType::EMERGENCY_BROADCAST:
                    handle_emergency_broadcast(packet);
                    break;
                case PacketType::HEARTBEAT:
                    // Update last seen time for source aircraft
                    break;
                default:
                    break;
            }
        }
    }
    
    // Remove stale aircraft data (older than 30 seconds)
    auto now = std::chrono::system_clock::now();
    nearby_aircraft_.erase(
        std::remove_if(nearby_aircraft_.begin(), nearby_aircraft_.end(),
            [now](const AircraftPosition& pos) {
                return std::chrono::duration_cast<std::chrono::seconds>(now - pos.timestamp).count() > 30;
            }),
        nearby_aircraft_.end()
    );
    
    // Send periodic heartbeat
    if (std::chrono::duration_cast<std::chrono::seconds>(now - last_heartbeat_).count() > 5) {
        send_heartbeat();
        last_heartbeat_ = now;
    }
}

std::vector<AircraftPosition> MeshNetworkNode::get_nearby_aircraft() const {
    return nearby_aircraft_;
}

bool MeshNetworkNode::send_maneuver_coordination(uint32_t target_id, const std::vector<uint8_t>& maneuver_data) {
    MeshPacket packet;
    packet.source_id = aircraft_id_;
    packet.destination_id = target_id;
    packet.sequence_number = sequence_number_++;
    packet.packet_type = static_cast<uint8_t>(PacketType::VECTOR_MANEUVER);
    packet.timestamp = std::chrono::system_clock::now();
    packet.payload = maneuver_data;
    
    return transceiver_->transmit(packet);
}

bool MeshNetworkNode::request_route_coordination(const std::vector<uint32_t>& affected_aircraft) {
    bool success = true;
    
    for (uint32_t target_id : affected_aircraft) {
        MeshPacket packet;
        packet.source_id = aircraft_id_;
        packet.destination_id = target_id;
        packet.sequence_number = sequence_number_++;
        packet.packet_type = static_cast<uint8_t>(PacketType::COORDINATION_REQUEST);
        packet.timestamp = std::chrono::system_clock::now();
        
        // Serialize affected aircraft list
        packet.payload.resize(affected_aircraft.size() * sizeof(uint32_t));
        std::memcpy(packet.payload.data(), affected_aircraft.data(), packet.payload.size());
        
        success &= transceiver_->transmit(packet);
    }
    
    return success;
}

void MeshNetworkNode::set_emergency_mode(bool emergency) {
    emergency_mode_ = emergency;
    
    if (emergency) {
        // Broadcast emergency signal to all nearby aircraft
        MeshPacket packet;
        packet.source_id = aircraft_id_;
        packet.destination_id = 0xFFFFFFFF; // Broadcast
        packet.sequence_number = sequence_number_++;
        packet.packet_type = static_cast<uint8_t>(PacketType::EMERGENCY_BROADCAST);
        packet.timestamp = std::chrono::system_clock::now();
        
        // Include emergency reason and position
        std::string emergency_reason = "EMERGENCY";
        packet.payload.resize(emergency_reason.size() + sizeof(AircraftPosition));
        std::memcpy(packet.payload.data(), emergency_reason.c_str(), emergency_reason.size());
        std::memcpy(packet.payload.data() + emergency_reason.size(), &current_position_, sizeof(AircraftPosition));
        
        transceiver_->transmit(packet);
        
        std::cout << "Emergency broadcast sent from aircraft " << aircraft_id_ << std::endl;
    }
}

bool MeshNetworkNode::is_emergency_active() const {
    return emergency_mode_;
}

void MeshNetworkNode::handle_position_update(const MeshPacket& packet) {
    if (packet.payload.size() >= sizeof(AircraftPosition)) {
        AircraftPosition position;
        std::memcpy(&position, packet.payload.data(), sizeof(AircraftPosition));
        
        // Update or add aircraft position
        auto it = std::find_if(nearby_aircraft_.begin(), nearby_aircraft_.end(),
            [&position](const AircraftPosition& existing) {
                return existing.aircraft_id == position.aircraft_id;
            });
        
        if (it != nearby_aircraft_.end()) {
            *it = position; // Update existing
        } else {
            nearby_aircraft_.push_back(position); // Add new
        }
    }
}

void MeshNetworkNode::handle_maneuver_coordination(const MeshPacket& packet) {
    std::cout << "Received maneuver coordination from aircraft " << packet.source_id << std::endl;
    // TODO: Implement maneuver coordination logic
}

void MeshNetworkNode::handle_coordination_request(const MeshPacket& packet) {
    std::cout << "Received coordination request from aircraft " << packet.source_id << std::endl;
    
    // Send coordination response
    MeshPacket response;
    response.source_id = aircraft_id_;
    response.destination_id = packet.source_id;
    response.sequence_number = sequence_number_++;
    response.packet_type = static_cast<uint8_t>(PacketType::COORDINATION_RESPONSE);
    response.timestamp = std::chrono::system_clock::now();
    
    std::string response_msg = "COORDINATION_ACK";
    response.payload.resize(response_msg.size());
    std::memcpy(response.payload.data(), response_msg.c_str(), response_msg.size());
    
    transceiver_->transmit(response);
}

void MeshNetworkNode::handle_emergency_broadcast(const MeshPacket& packet) {
    std::cout << "EMERGENCY: Received emergency broadcast from aircraft " << packet.source_id << std::endl;
    
    // Immediately analyze for collision avoidance
    if (packet.payload.size() >= sizeof(AircraftPosition)) {
        AircraftPosition emergency_position;
        std::memcpy(&emergency_position, packet.payload.data(), packet.payload.size());
        
        // Calculate distance and potential collision course
        double distance = calculate_distance(current_position_, emergency_position);
        if (distance < 5000.0) { // Within 5km - immediate concern
            std::cout << "WARNING: Emergency aircraft within " << distance << " meters" << std::endl;
            // TODO: Implement emergency avoidance maneuver
        }
    }
}

void MeshNetworkNode::send_heartbeat() {
    MeshPacket packet;
    packet.source_id = aircraft_id_;
    packet.destination_id = 0xFFFFFFFF; // Broadcast
    packet.sequence_number = sequence_number_++;
    packet.packet_type = static_cast<uint8_t>(PacketType::HEARTBEAT);
    packet.timestamp = std::chrono::system_clock::now();
    
    transceiver_->transmit(packet);
}

double MeshNetworkNode::calculate_distance(const AircraftPosition& pos1, const AircraftPosition& pos2) {
    // Simplified distance calculation (should use proper aviation formulas)
    double dx = pos1.latitude - pos2.latitude;
    double dy = pos1.longitude - pos2.longitude;
    double dz = pos1.altitude - pos2.altitude;
    return std::sqrt(dx*dx + dy*dy + dz*dz) * 111000; // Rough conversion to meters
}

} // namespace aamn
