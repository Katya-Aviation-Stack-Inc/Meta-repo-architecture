#pragma once

#include "airmesh_protocol.h"
#include <map>
#include <set>

namespace aamn {

class AdvancedMeshNode : public MeshNetworkNode {
public:
    AdvancedMeshNode(uint32_t aircraft_id, std::unique_ptr<RFTransceiver> transceiver);
    
    // Enhanced routing capabilities
    bool enable_adaptive_routing();
    void disable_adaptive_routing();
    bool is_adaptive_routing_enabled() const;
    
    // Quality of Service features
    void set_packet_priority(uint8_t priority_level);
    uint8_t get_current_priority() const;
    
    // Network statistics
    struct NetworkStats {
        uint64_t packets_sent;
        uint64_t packets_received;
        uint64_t packets_dropped;
        double average_latency_ms;
        double packet_loss_rate;
        uint32_t connected_nodes;
    };
    
    NetworkStats get_network_stats() const;
    
    // Security features
    bool enable_encryption();
    void disable_encryption();
    bool is_encryption_enabled() const;
    
    // Frequency hopping for interference avoidance
    bool enable_frequency_hopping();
    void disable_frequency_hopping();
    bool is_frequency_hopping_enabled() const;
    
    // Network topology information
    std::map<uint32_t, std::vector<uint32_t>> get_network_topology() const;
    
    // Bandwidth management
    void set_bandwidth_limit(uint32_t bytes_per_second);
    uint32_t get_current_bandwidth_usage() const;
    
private:
    bool adaptive_routing_enabled_;
    uint8_t current_priority_;
    NetworkStats network_stats_;
    bool encryption_enabled_;
    bool frequency_hopping_enabled_;
    uint32_t bandwidth_limit_;
    uint32_t current_bandwidth_usage_;
    
    // Enhanced packet handling
    void handle_encrypted_packet(const MeshPacket& packet);
    MeshPacket encrypt_packet(const MeshPacket& packet);
    MeshPacket decrypt_packet(const MeshPacket& packet);
    
    // Routing algorithms
    uint32_t calculate_optimal_route(uint32_t destination_id);
    std::vector<uint32_t> find_shortest_path(uint32_t source, uint32_t destination);
    
    // Frequency management
    void hop_to_next_frequency();
    std::vector<double> available_frequencies_;
    size_t current_frequency_index_;
};

class MeshNetworkManager {
public:
    MeshNetworkManager();
    
    // Network formation and maintenance
    bool form_network(const std::vector<uint32_t>& node_ids);
    bool add_node(uint32_t node_id);
    bool remove_node(uint32_t node_id);
    
    // Network optimization
    void optimize_network_topology();
    void rebalance_traffic();
    
    // Network monitoring
    std::set<uint32_t> get_active_nodes() const;
    std::map<uint32_t, uint32_t> get_node_priorities() const;
    
    // Emergency network functions
    bool initiate_emergency_network_reset();
    bool isolate_faulty_nodes();
    
private:
    std::set<uint32_t> active_nodes_;
    std::map<uint32_t, uint32_t> node_priorities_;
    std::map<uint32_t, std::vector<uint32_t>> network_topology_;
    
    // Network optimization algorithms
    void calculate_optimal_tree();
    void distribute_routing_tables();
    
    // Fault detection
    std::set<uint32_t> detect_faulty_nodes();
};

// Utility functions for advanced mesh features
namespace mesh_utils {
    // Packet compression
    std::vector<uint8_t> compress_payload(const std::vector<uint8_t>& payload);
    std::vector<uint8_t> decompress_payload(const std::vector<uint8_t>& compressed);
    
    // Error correction
    std::vector<uint8_t> add_error_correction(const std::vector<uint8_t>& data);
    std::vector<uint8_t> correct_errors(const std::vector<uint8_t>& data);
    
    // Data serialization
    template<typename T>
    std::vector<uint8_t> serialize(const T& data) {
        std::vector<uint8_t> result(sizeof(T));
        std::memcpy(result.data(), &data, sizeof(T));
        return result;
    }
    
    template<typename T>
    T deserialize(const std::vector<uint8_t>& data) {
        T result;
        std::memcpy(&result, data.data(), sizeof(T));
        return result;
    }
}

} // namespace aamn