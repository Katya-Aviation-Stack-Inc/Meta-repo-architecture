#include "air_swarm_os.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace air_swarm_os {

// AirSwarmOS Implementation
AirSwarmOS::AirSwarmOS()
    : aircraft_id_(0)
    , current_role_(SwarmRole::NODE)
    , autonomous_mode_enabled_(false)
    , communication_range_(50.0)
    , update_frequency_(10.0)
    , min_separation_(1000.0)
    , max_descent_rate_(15.0)
    , system_healthy_(true)
    , swarm_efficiency_(0.8)
    , last_update_time_(std::chrono::system_clock::now()) {
    
    crypto_engine_ = std::make_unique<CryptographicEngine>();
    consensus_ = std::make_unique<ConsensusMechanism>();
    network_ = std::make_unique<SwarmNetwork>();
    airspace_manager_ = std::make_unique<AirspaceManager>();
    conflict_resolver_ = std::make_unique<ConflictResolver>();
    coordinator_ = std::make_unique<SwarmCoordinator>();
}

bool AirSwarmOS::initialize(uint32_t aircraft_id, const std::string& flight_id, SwarmRole role) {
    aircraft_id_ = aircraft_id;
    flight_id_ = flight_id;
    current_role_ = role;
    
    std::cout << "Air Swarm OS initializing..." << std:: std::endl;
    std::cout << "Aircraft ID: " << aircraft_id_ << std::endl;
    std::cout << "Flight ID: " << flight_id_ << std::endl;
    std::cout << "Role: " << static_cast<int>(role) << std::endl;
    
    // Initialize cryptographic engine
    if (!crypto_engine_->initialize(aircraft_id_)) {
        std::cerr << "Failed to initialize cryptographic engine" << std::endl;
        return false;
    }
    
    // Initialize consensus mechanism
    if (!consensus_->initialize(1)) { // Start with 1 node
        std::cerr << "Failed to initialize consensus mechanism" << std::endl;
        return false;
    }
    
    // Initialize network
    if (!network_->initialize(aircraft_id_, flight_id_)) {
        std::cerr << "Failed to initialize swarm network" << std::endl;
        return false;
    }
    
    // Initialize airspace manager
    if (!airspace_manager_->initialize(50.0)) { // 50km coverage
        std::cerr << "Failed to initialize airspace manager" << std::endl;
        return false;
    }
    
    // Initialize conflict resolver
    if (!conflict_resolver_->initialize()) {
        std::cerr << "Failed to initialize conflict resolver" << std::endl;
        return false;
    }
    
    // Initialize coordinator
    if (!coordinator_->initialize(aircraft_id_)) {
        std::cerr << "Failed to initialize swarm coordinator" << std::endl;
        return false;
    }
    
    // Create genesis block
    latest_block_ = consensus_->create_genesis_block();
    blockchain_.push_back(latest_block_);
    
    // Set default parameters
    set_swarm_parameters(100.0, 10.0); // 100km range, 10Hz update
    set_safety_parameters(1000.0, 8.0); // 1km separation, 8m/s max descent
    
    std::cout << "Air Swarm OS initialized successfully" << std::endl;
    return true;
}

void AirSwarmOS::shutdown() {
    std::cout << "Shutting down Air Swarm OS..." << std::endl;
    
    autonomous_mode_enabled_ = false;
    system_healthy_ = false;
    
    network_->remove_peer(aircraft_id_);
    
    std::cout << "Air Swarm OS shutdown complete" << std::endl;
}

bool AirSwarmOS::update_swarm_state(const AircraftPosition& current_position) {
    auto start_time = std::chrono::high_resolution_clock::now();
    
    // Update current position
    update_aircraft_position(current_position);
    
    // Process incoming messages
    process_messages();
    
    // Update blockchain
    update_blockchain();
    
    // Check for conflicts
    check_for_conflicts();
    
    // Resolve detected conflicts
    resolve_detected_conflicts();
    
    // Monitor system health
    monitor_system_health();
    
    // Check update time
    auto end_time = std::chrono::high_resolution::clock::now();
    auto update_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    if (update_time > target_update_period_) {
        system_healthy_ = false;
    }
    
    last_update_time_ = std::chrono::system_clock::now();
    
    return system_healthy_;
}

bool AirSwarmOS::broadcast_position(const AircraftPosition& position) {
    SwarmMessage message{};
    message.message_id = 0; // Will be set by network
    message.sender_id = aircraft_id_;
    message.target_id = 0; // Broadcast
    message.message_type = static_cast<uint8_t>(MessageType::POSITION_UPDATE);
    message.timestamp = std::chrono::system_clock::now();
    message.nonce = static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Serialize position data
    message.payload.resize(sizeof(AircraftPosition));
    std::memcpy(message.payload.data(), &position, sizeof(AircraftPosition));
    
    // Sign message
    message.signature = crypto_engine_->sign_message(message);
    
    // Broadcast to network
    return network_->broadcast_message(message);
}

std::vector<AircraftPosition> AirSwarmOS::get_swarm_positions() {
    return airspace_manager_->get_nearby_aircraft(current_position_, communication_range_);
}

bool AirSwarmOS::send_emergency_alert(const std::string& emergency_type) {
    SwarmMessage message{};
    message.message_id = 0;
    message.sender_id = aircraft_id_;
    message.target_id = 0; // Broadcast
    message.message_type = static_cast<uint8_t>(MessageType::EMERGENCY_BROADCAST);
    message.timestamp = std::chrono::system_clock::now();
    message.nonce = static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count());
    
    // Serialize emergency type
    message.payload.resize(emergency_type.length());
    std::memcpy(message.payload.data(), emergency_type.c_str(), emergency_type.length());
    
    // Sign message
    message.signature = crypto_engine_->sign_message(message);
    
    return network_->broadcast_message(message);
}

bool AirSwarmOS::add_transaction(const SwarmMessage& message) {
    // Add transaction to current block
    if (blockchain_.empty()) {
        return false;
    }
    
    BlockchainBlock& latest_block = blockchain_.back();
    latest_block.transactions.push_back(message);
    
    // Update merkle root
    uint32_t merkle_root = 0;
    for (const auto& transaction : latest_block.transactions) {
        merkle_root ^= crypto_engine_->calculate_hash(&transaction, sizeof(transaction));
    }
    latest_block.merkle_root = merkle_root;
    
    return true;
}

bool AirSwarmOS::validate_blockchain() {
    if (blockchain_.empty()) {
        return false;
    }
    
    // Validate all blocks in blockchain
    for (const auto& block : blockchain_) {
        if (!consensus_->validate_block(block)) {
            return false;
        }
    }
    
    return true;
}

BlockchainBlock AirSwarmOS::get_latest_block() {
    if (blockchain_.empty()) {
        return BlockchainBlock{};
    }
    
    return blockchain_.back();
}

bool AirSwarmOS::request_volume_assignment() {
    // Request volume assignment from airspace manager
    AirspaceVolume volume{};
    
    if (airspace_manager_->get_volumes().empty()) {
        return false;
    }
    
    // Find available volume
    for (auto& vol : airspace_manager_->get_volumes()) {
        if (vol.is_active && vol.aircraft_ids.empty()) {
            volume = vol;
            break;
        }
    }
    
    if (volume.volume_id == 0) {
        return false; // No available volumes
    }
    
    // Assign volume to this aircraft
    return airspace_manager_->assign_volume(aircraft_id_, volume);
}

bool AirSwarmOS::release_current_volume() {
    auto it = aircraft_to_volume_map_.find(aircraft_id_);
    if (it != aircraft_to_volume_map_.end()) {
        uint32_t volume_id = it->second;
        airspace_manager_->release_volume(aircraft_id_, volume_id);
        return true;
    }
    
    return false;
}

std::vector<ConflictResolution> AirSwarmOS::get_pending_conflicts() {
    return conflict_resolver_->resolve_conflicts(airspace_manager_->get_nearby_aircraft(current_position_));
}

void AirSwarmOS::update_blockchain() {
    // Check if we have enough transactions for a new block
    if (!blockchain_.empty() && blockchain_.back().transactions.size() >= 10) {
        // Create new block
        BlockchainBlock new_block{};
        new_block.block_id = blockchain_.back().block_id + 1;
        new_block.previous_hash = blockchain_.back().merkle_root;
        new_block.timestamp = std::chrono::system_clock::now();
        new_block.validator_id = aircraft_id_;
        
        // Move transactions from old block to new block
        new_block.transactions = blockchain_.back().transactions;
        blockchain_.back().transactions.clear();
        
        // Calculate new merkle root
        uint32_t merkle_root = 0;
        for (const auto& transaction : new_block.transactions) {
            merkle_root ^= crypto_engine_->calculate_hash(&transaction, sizeof(transaction));
        }
        new_block.merkle_root = merkle_root;
        
        // Add block to blockchain
        blockchain_.push_back(new_block);
        
        // Add block candidate to consensus
        consensus_->add_block_candidate(new_block);
        
        // Check for consensus
        if (consensus_->has_consensus()) {
            // Block is accepted, remove from candidates
            blockchain_.pop_back();
            blockchain_.push_back(new_block);
        }
    }
}

void AirSwarmOS::process_messages() {
    auto messages = network_->receive_messages();
    
    for (const auto& message : messages) {
        // Verify message signature
        if (crypto_engine_->verify_signature(message)) {
            // Process message based on type
            switch (static_cast<MessageType>(message.message_type)) {
                case MessageType::POSITION_UPDATE:
                    // Update aircraft position in airspace manager
                    {
                        AircraftPosition position{};
                        std::memcpy(&position, message.payload.data(), std::min(message.payload.size(), sizeof(AircraftPosition)));
                        airspace_manager_->update_aircraft_position(position);
                    }
                    break;
                    
                case MessageType::INTENT_DECLARATION:
                    // Handle intent declaration
                    std::cout << "Received intent declaration from aircraft " << message.sender_id << std::endl;
                    break;
                    
                case MessageType::CONFLICT_DETECTED:
                    // Handle conflict detection
                    std::cout << "Conflict detected by aircraft " << message.sender_id << std::endl;
                    break;
                    
                case MessageType::RESOLUTION_PROPOSAL:
                    // Handle resolution proposal
                    std::cout << "Resolution proposal from aircraft " << message.sender_id << std::endl;
                    break;
                    
                case MessageType::EMERGENCY_BROADCAST:
                    // Handle emergency broadcast
                    std::cout << "Emergency broadcast from aircraft " << message.sender_id << std::endl;
                    break;
                    
                case MessageType::HEARTBEAT:
                    // Handle heartbeat
                    break;
                    
                default:
                    break;
            }
        }
    }
}

void AirSwarmOS::update_aircraft_position(const AircraftPosition& position) {
    current_position_ = position;
    airspace_manager_->update_aircraft_position(position);
}

void AirSwarmOS::check_for_conflicts() {
    auto conflicts = airspace_manager_->detect_conflicts();
    
    for (const auto& conflict : conflicts) {
        // Add conflict to blockchain
        SwarmMessage message{};
        message.message_id = 0;
        message.sender_id = aircraft_id_;
        message.target_id = 0; // Broadcast
        message.message_type = static_cast<uint8_t>(MessageType::CONFLICT_DETECTED);
        message.timestamp = std::chrono::system_clock::now();
        message.nonce = static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count());
        
        // Serialize conflict data
        message.payload.resize(sizeof(ConflictResolution));
        std::memcpy(message.payload.data(), &conflict, sizeof(ConflictResolution));
        
        // Sign message
        message.signature = crypto_engine_->sign_message(message);
        
        add_transaction(message);
    }
}

void AirSwarmOS::resolve_detected_conflicts() {
    auto conflicts = get_pending_conflicts();
    
    for (auto& conflict : conflicts) {
        // Accept resolution if it meets safety criteria
        if (conflict.conflict_distance > min_separation_ / safety_factor_) {
            conflict.is_accepted = true;
            
            // Apply resolution to aircraft positions
            if (!conflict.new_altitudes.empty()) {
                // Update aircraft positions (simplified)
                for (size_t i = 0; i < conflict.new_altitudes.size(); ++i) {
                    if (conflict.aircraft1_id == current_position_.aircraft_id && i < conflict.new_altitudes.size()) {
                        current_position_.altitude = conflict.new_altitudes[i];
                    }
                }
            }
            
            // Add resolution to blockchain
            SwarmMessage message{};
            message.message_id = 0;
            message.sender_id = aircraft_id_;
            message.target_id = 0; // Broadcast
            message.message_type = static_cast<uint8_t>(MessageType::RESOLUTION_ACCEPTED);
            message.timestamp = std::chrono::system_clock::now();
            message.nonce = static_cast<uint32_t>(std::chrono::system_clock::now().time_since_epoch().count());
            
            message.payload.resize(sizeof(ConflictResolution));
            std::memcpy(message.payload.data(), &conflict, sizeof(ConflictResolution));
            
            message.signature = crypto_engine_->sign_message(message);
            add_transaction(message);
        }
    }
}

void AirSwarmOS::monitor_system_health() {
    system_healthy_ = network_->is_connected() && 
                     crypto_engine_ && 
                     consensus_ && 
                     airspace_manager_ && 
                     conflict_resolver_ &&
                     coordinator_;
    
    // Calculate swarm efficiency based on message processing
    double message_throughput = network_->get_network_health() * 100.0; // Messages per second
    swarm_efficiency_ = std::min(1.0, message_throughput / (update_frequency_ * 10.0));
    
    // Check for system warnings
    system_warnings_.clear();
    
    // Add warnings based on system state
    if (!system_healthy_) {
        system_warnings_.push_back("System health degraded");
    }
    
    if (!network_->is_connected()) {
        system_warnings_.push_back("Network disconnected");
    }
    
    if (swarm_positions_.size() < 2) {
        system_warnings_.push_back("Low swarm size");
    }
    
    if (get_pending_conflicts().size() > 5) {
        system_warnings_.push_back("High conflict rate");
    }
    
    if (swarm_efficiency_ < 0.5) {
        system_warnings_.push_back("Low swarm efficiency");
    }
}

void AirSwarmOS::handle_network_failures() {
    if (!network_->is_connected()) {
        // Attempt to reconnect
        network_->initialize(aircraft_id_, flight_id_);
    }
}

void AirSwarmOS::optimize_swarm_performance() {
    // Optimize update frequency based on swarm size and network health
    double optimal_frequency = std::max(1.0, std::min(20.0, swarm_positions_.size() / 5.0));
    
    if (std::abs(update_frequency_ - optimal_frequency) > 1.0) {
        update_frequency_ = optimal_frequency;
        std::cout << "Optimized update frequency to " << update_frequency_ << " Hz" << std::endl;
    }
}

void AirSwarmOS::set_swarm_parameters(double communication_range_km, double update_frequency_hz) {
    communication_range_ = communication_range_km;
    update_frequency_ = update_frequency_hz;
    
    std::cout << "Swarm parameters updated:" << std::endl;
    std::cout << "Communication range: " << communication_range_ << " km" << std::endl;
    std::cout << "Update frequency: " << update_frequency_ << " Hz" << std::endl;
}

void AirSwarmOS::set_safety_parameters(double min_separation_m, double max_descent_rate) {
    min_separation_ = min_separation_m;
    max_descent_rate_ = max_descent_rate;
    
    conflict_resolver_->set_separation_minima(min_separation_m, 1000.0, 60.0);
    conflict_resolver_->set_safety_factor(1.5);
    
    std::cout << "Safety parameters updated:" << std::endl;
    std::cout << "Min separation: " << min_separation_ << " m" << std::endl;
    std::cout << "Max descent rate: " << max_descent_rate_ << " m/s" << std::endl;
}

void AirSwarmOS::enable_autonomous_mode(bool enable) {
    autonomous_mode_enabled_ = enable;
    
    if (enable) {
        coordinator_->set_role(SwarmRole::COORDINATOR);
    } else {
        coordinator_->set_role(SwarmRole::NODE);
    }
    
    std::cout << "Autonomous mode " << (enable ? "ENABLED" : "DISABLED") << std::endl;
}

bool AirSwarmOS::is_swarm_healthy() const {
    return system_healthy_;
}

double AirSwarmOS::get_swarm_efficiency() const {
    return swarm_efficiency_;
}

bool AirSwarmOS::is_autonomous_mode_enabled() const {
    return autonomous_mode_enabled_;
}

std::vector<std::string> AirSwarmOS::get_system_warnings() const {
    return system_warnings_;
}

} // namespace air_swarm_os
