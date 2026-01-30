#include "airmesh_protocol.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace aamn;

int main() {
    std::cout << "=== Air-to-Air Mesh Network (AAMN) Demo ===" << std::endl;
    
    // Create mock RF transceiver
    auto transceiver = std::make_unique<MockRFTransceiver>();
    
    // Initialize mesh network node for aircraft ID 1001
    MeshNetworkNode node(1001, std::move(transceiver));
    
    if (!node.initialize()) {
        std::cerr << "Failed to initialize mesh network node" << std::endl;
        return 1;
    }
    
    // Initialize distributed autopilot
    DistributedAutopilot autopilot(node);
    
    // Enable swarm coordination
    autopilot.enable_swarm_coordination();
    
    // Simulate aircraft movement and network operations
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> lat_dist(-90.0, 90.0);
    std::uniform_real_distribution<> lon_dist(-180.0, 180.0);
    std::uniform_real_distribution<> alt_dist(1000.0, 40000.0); // feet
    std::uniform_real_distribution<> vel_dist(-200.0, 200.0); // knots
    
    std::cout << "Starting simulation..." << std::endl;
    
    for (int i = 0; i < 100; ++i) {
        // Update aircraft position
        AircraftPosition position;
        position.latitude = lat_dist(gen);
        position.longitude = lon_dist(gen);
        position.altitude = alt_dist(gen);
        position.velocity_x = vel_dist(gen);
        position.velocity_y = vel_dist(gen);
        position.velocity_z = vel_dist(gen) * 0.1; // Vertical velocity is smaller
        position.timestamp = std::chrono::system_clock::now();
        
        node.update_position(position);
        
        // Broadcast position every 5 iterations
        if (i % 5 == 0) {
            node.broadcast_position();
        }
        
        // Process incoming packets
        node.process_incoming_packets();
        
        // Analyze conflicts every 10 iterations
        if (i % 10 == 0) {
            autopilot.analyze_traffic_conflicts();
            
            auto conflicts = autopilot.get_conflicting_aircraft();
            for (uint32_t conflict_id : conflicts) {
                autopilot.negotiate_separation(conflict_id);
            }
        }
        
        // Simulate emergency at iteration 50
        if (i == 50) {
            std::cout << "\n*** EMERGENCY SIMULATION ***" << std::endl;
            node.set_emergency_mode(true);
            
            // Simulate emergency resolution after 5 iterations
            std::this_thread::sleep_for(std::chrono::seconds(1));
            node.set_emergency_mode(false);
            std::cout << "*** Emergency resolved ***" << std::endl;
        }
        
        // Display status every 20 iterations
        if (i % 20 == 0) {
            auto nearby = node.get_nearby_aircraft();
            std::cout << "Iteration " << i << ": ";
            std::cout << nearby.size() << " nearby aircraft, ";
            std::cout << "Swarm active: " << (autopilot.is_swarm_active() ? "Yes" : "No");
            std::cout << std::endl;
        }
        
        // Simulate real-time operation
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    std::cout << "\nSimulation completed." << std::endl;
    
    // Test collective maneuver
    std::vector<uint32_t> participants = {1002, 1003, 1004};
    autopilot.execute_collective_maneuver(participants);
    
    // Disable swarm coordination
    autopilot.disable_swarm_coordination();
    
    std::cout << "AAMN Demo finished successfully." << std::endl;
    return 0;
}
