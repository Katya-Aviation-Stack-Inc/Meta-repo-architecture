#include "advanced_mesh_features.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace aamn;

// Mock RF Transceiver for testing
class TestRFTransceiver : public RFTransceiver {
public:
    TestRFTransceiver() : frequency_(0.0), connected_(false) {}
    
    bool initialize(double frequency_mhz) override {
        frequency_ = frequency_mhz;
        connected_ = true;
        std::cout << "Test RF Transceiver initialized at " << frequency_mhz << " MHz" << std::endl;
        return true;
    }
    
    bool transmit(const MeshPacket& packet) override {
        if (!connected_) return false;
        
        // Simulate transmission delay
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        
        std::cout << "Transmitted packet: " << packet.sequence_number 
                  << " from " << packet.source_id 
                  << " to " << packet.destination_id << std::endl;
        return true;
    }
    
    std::vector<MeshPacket> receive() override {
        if (!connected_) return {};
        
        // Simulate receiving some packets
        std::vector<MeshPacket> packets;
        return packets;
    }
    
    double get_signal_strength() const override {
        return connected_ ? -50.0 : -100.0; // dBm
    }
    
    bool is_connected() const override {
        return connected_;
    }
    
    void set_frequency(double frequency_mhz) override {
        frequency_ = frequency_mhz;
        std::cout << "Test RF Transceiver frequency set to " << frequency_mhz << " MHz" << std::endl;
    }
    
    double get_frequency() const override {
        return frequency_;
    }

private:
    double frequency_;
    bool connected_;
};

int main() {
    std::cout << "=== Advanced Air-to-Air Mesh Network Demo ===" << std::endl;
    
    // Create test RF transceiver
    auto transceiver = std::make_unique<TestRFTransceiver>();
    
    // Initialize advanced mesh network node
    AdvancedMeshNode node(2001, std::move(transceiver));
    
    // Initialize the node
    if (!node.initialize()) {
        std::cerr << "Failed to initialize advanced mesh network node" << std::endl;
        return 1;
    }
    
    std::cout << "Advanced mesh node initialized successfully" << std::endl;
    
    // Test advanced features
    std::cout << "\n--- Testing Advanced Features ---" << std::endl;
    
    // Test adaptive routing
    node.enable_adaptive_routing();
    std::cout << "Adaptive routing enabled: " << (node.is_adaptive_routing_enabled() ? "Yes" : "No") << std::endl;
    
    // Test encryption
    node.enable_encryption();
    std::cout << "Encryption enabled: " << (node.is_encryption_enabled() ? "Yes" : "No") << std::endl;
    
    // Test frequency hopping
    node.enable_frequency_hopping();
    std::cout << "Frequency hopping enabled: " << (node.is_frequency_hopping_enabled() ? "Yes" : "No") << std::endl;
    
    // Test priority setting
    node.set_packet_priority(5);
    std::cout << "Current packet priority: " << static_cast<int>(node.get_current_priority()) << std::endl;
    
    // Test bandwidth management
    node.set_bandwidth_limit(1000000); // 1 Mbps
    std::cout << "Bandwidth limit set to: " << node.get_current_bandwidth_usage() << " bytes/sec" << std::endl;
    
    // Test network statistics
    auto stats = node.get_network_stats();
    std::cout << "Network statistics:" << std::endl;
    std::cout << "  Packets sent: " << stats.packets_sent << std::endl;
    std::cout << "  Packets received: " << stats.packets_received << std::endl;
    std::cout << "  Packets dropped: " << stats.packets_dropped << std::endl;
    std::cout << "  Average latency: " << stats.average_latency_ms << " ms" << std::endl;
    std::cout << "  Packet loss rate: " << stats.packet_loss_rate << std::endl;
    std::cout << "  Connected nodes: " << stats.connected_nodes << std::endl;
    
    // Test network manager
    std::cout << "\n--- Testing Network Manager ---" << std::endl;
    MeshNetworkManager manager;
    
    // Form a network with multiple nodes
    std::vector<uint32_t> node_ids = {2001, 2002, 2003, 2004, 2005};
    manager.form_network(node_ids);
    
    // Add an additional node
    manager.add_node(2006);
    
    // Get active nodes
    auto active_nodes = manager.get_active_nodes();
    std::cout << "Active nodes count: " << active_nodes.size() << std::endl;
    
    // Test network optimization
    manager.optimize_network_topology();
    manager.rebalance_traffic();
    
    // Test utility functions
    std::cout << "\n--- Testing Utility Functions ---" << std::endl;
    
    // Test data serialization
    AircraftPosition test_position;
    test_position.latitude = 40.7128;
    test_position.longitude = -74.0060;
    test_position.altitude = 1000.0;
    test_position.velocity_x = 100.0;
    test_position.velocity_y = 50.0;
    test_position.velocity_z = 0.0;
    test_position.heading = 90.0;
    test_position.bank_angle = 5.0;
    test_position.pitch_angle = 2.0;
    test_position.timestamp = std::chrono::system_clock::now();
    test_position.aircraft_id = 2001;
    test_position.aircraft_type = 1;
    test_position.priority_level = 75;
    test_position.is_autonomous = true;
    
    auto serialized = mesh_utils::serialize(test_position);
    std::cout << "Serialized position data size: " << serialized.size() << " bytes" << std::endl;
    
    auto deserialized = mesh_utils::deserialize<AircraftPosition>(serialized);
    std::cout << "Deserialized position: " << deserialized.latitude << ", " << deserialized.longitude << std::endl;
    
    // Test compression
    std::vector<uint8_t> test_data(1000, 42); // 1000 bytes of value 42
    auto compressed = mesh_utils::compress_payload(test_data);
    auto decompressed = mesh_utils::decompress_payload(compressed);
    std::cout << "Compression test: " << test_data.size() << " -> " << compressed.size() << " -> " << decompressed.size() << " bytes" << std::endl;
    
    // Test error correction
    auto with_correction = mesh_utils::add_error_correction(test_data);
    auto corrected = mesh_utils::correct_errors(with_correction);
    std::cout << "Error correction test: " << test_data.size() << " -> " << with_correction.size() << " -> " << corrected.size() << " bytes" << std::endl;
    
    std::cout << "\nAdvanced AAMN Demo completed successfully." << std::endl;
    return 0;
}