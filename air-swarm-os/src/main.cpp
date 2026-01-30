#include "air_swarm_os.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <sstream>
#include <iomanip>

using namespace air_swarm_os;

int main() {
    std::cout << "=== Air Swarm OS Demo ===" << std::endl;
    std::cout << "Revolutionary distributed air traffic management system" << std::endl;
    
    // Initialize Air Swarm OS as Node 1
    AirSwarmOS swarm_os;
    
    if (!swarm_os.initialize(1001, "FLIGHT-001", SwarmRole::NODE)) {
        std::cerr << "Failed to initialize Air Swarm OS" << std::endl;
        return 1;
    }
    
    std::cout << "Air Swarm OS initialized successfully" << std::endl;
    std::cout << "Starting distributed air traffic simulation..." << std::endl;
    
    // Simulation parameters
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> lat_dist(-90.0, 90.0); // degrees
    std::uniform_real_distribution<> lon_dist(-180.0, 180.0); // degrees
    std::uniform_real_distribution<> alt_dist(1000.0, 10000.0); // meters
    std::uniform_real_distribution<> speed_dist(50.0, 300.0); // m/s
    std::uniform_real_distribution<> heading_dist(0.0, 360.0); // degrees
    std::uniform_real_distribution<> descent_dist(-10.0, 5.0); // m/s
    
    AircraftPosition current_position{};
    std::vector<AircraftPosition> swarm_positions;
    
    // Add more aircraft to the swarm
    std::vector<AirSwarmOS> additional_aircraft;
    for (int i = 2; i <= 10; ++i) {
        AirSwarmOS aircraft;
        aircraft.initialize(1000 + i, "FLIGHT-" + std::to_string(i), SwarmRole::NODE);
        additional_aircraft.push_back(aircraft);
    }
    
    // Main simulation loop
    for (int iteration = 0; iteration < 500; ++iteration) {
        // Generate current position
        current_position.latitude = lat_dist(gen);
        current_position.longitude = lon_dist(gen);
        current_position.altitude = alt_dist(gen);
        current_position.airspeed = speed_dist(gen);
        current_position.vertical_speed = descent_dist(gen);
        current_position.heading = heading_dist(gen);
        current_position.timestamp = std::chrono::system_clock::now();
        current_position.aircraft_id = 1001;
        current_position.flight_id = "FLIGHT-001";
        current_position.aircraft_type = 0; // Jet
        current_position.priority_level = 1; // Medium priority
        
        // Update swarm state
        bool swarm_healthy = swarm_os.update_swarm_state(current_position);
        
        // Broadcast position
        swarm_os.broadcast_position(current_position);
        
        // Get swarm positions
        swarm_positions = swarm_os.get_swarm_positions();
        
        // Add positions from other aircraft
        for (size_t i = 0; i < additional_aircraft.size(); ++i) {
            AircraftPosition other_position{};
            other_position.latitude = lat_dist(gen);
            other_position.longitude = lon_dist(gen);
            other_position.altitude = alt_dist(gen);
            other_position.airspeed = speed_dist(gen);
            other_position.vertical_speed = descent_dist(gen);
            other_position.heading = heading_dist(gen);
            other_position.timestamp = std::chrono::system_clock::now();
            other_position.aircraft_id = 1002 + i;
            other_position.flight_id = "FLIGHT-" + std::to_string(1002 + i);
            other_position.aircraft_type = i % 3; // Mix of aircraft types
            other_position.priority_level = (i % 3) == 0 ? 2 : 1; // Some high priority
            
            additional_aircraft[i].update_swarm_state(other_position);
            swarm_positions.push_back(other_position);
        }
        
        // Simulate different swarm scenarios
        if (iteration == 50) {
            std::cout << "\n*** High Density Airspace Scenario ***" << std::endl;
            
            // Add more aircraft to create high density
            for (int i = 11; i <= 20; ++i) {
                AirSwarmOS aircraft;
                aircraft.initialize(1000 + i, "FLIGHT-" + std::to_string(i), SwarmRole::NODE);
                additional_aircraft.push_back(aircraft);
            }
            
            swarm_os.set_safety_parameters(500.0, 8.0); // Tighter separation
        }
        
        if (iteration == 100) {
            std::cout << "\n*** Emergency Situation ***" << std::endl;
            
            // Simulate emergency aircraft
            AircraftPosition emergency_pos{};
            emergency_pos.latitude = lat_dist(gen);
            emergency_pos.longitude = lon_dist(gen);
            emergency_pos.altitude = 2000.0; // Low altitude
            emergency_pos.airspeed = 100.0;
            emergency_pos.vertical_speed = -15.0; // Rapid descent
            emergency_pos.heading = heading_dist(gen);
            emergency_pos.timestamp = std::chrono::system_clock::now();
            emergency_pos.aircraft_id = 9999;
            emergency_pos.flight_id = "EMERGENCY-001";
            emergency_pos.aircraft_type = 1; // Helicopter
            emergency_pos.priority_level = 3; // Emergency priority
            
            swarm_os.send_emergency_alert("low_altitude_rapid_descent");
            
            // Add emergency aircraft to swarm
            AirSwarmOS emergency_aircraft;
            emergency_aircraft.initialize(9999, "EMERGENCY-001", SwarmRole::EMERGENCY_CONTROLLER);
            emergency_aircraft.update_swarm_state(emergency_pos);
            swarm_positions.push_back(emergency_pos);
        }
        
        if (iteration == 150) {
            std::cout << "\n*** Coordinator Election ***" << std::endl;
            
            // Elect a coordinator from the swarm
            int coordinator_id = 1001 + (iteration % 5);
            additional_aircraft[coordinator_id - 1001].set_role(SwarmRole::COORDINATOR);
            
            swarm_os.set_role(SwarmRole::NODE);
        }
        
        if (iteration == 200) {
            std::cout << "\n*** Autonomous Mode Activation ***" << std::endl;
            swarm_os.enable_autonomous_mode(true);
            swarm_os.set_swarm_parameters(100.0, 10.0); // 100km range, 10Hz update
        }
        
        if (iteration == 250) {
            std::cout << "\n*** Volume Assignment Test ***" << std::endl;
            
            // Request volume assignment for all aircraft
            bool volume_assigned = swarm_os.request_volume_assignment();
            std::cout << "Volume assignment: " << (volume_assigned ? "SUCCESS" : "FAILED") << std::endl;
        }
        
        if (iteration == 300) {
            std::cout << "\n*** Blockchain Validation Test ***" << std::endl;
            
            bool blockchain_valid = swarm_os.validate_blockchain();
            std::cout << "Blockchain validation: " << (blockchain_valid ? "VALID" : "INVALID") << std::endl;
            
            auto latest_block = swarm_os.get_latest_block();
            std::cout << "Latest block ID: " << latest_block.block_id << std::endl;
            std::cout << "Transactions: " << latest_block.transactions.size() << std::endl;
        }
        
        if (iteration == 350) {
            std:: << "\n*** Conflict Detection and Resolution ***" << std::endl;
            
            auto conflicts = swarm_os.get_pending_conflicts();
            std::cout << "Conflicts detected: " << conflicts.size() << std::endl;
            
            for (const auto& conflict : conflicts) {
                std::cout << "  Aircraft " << conflict.aircraft1_id << " vs " << conflict.aircraft2_id;
                std::cout << " - Distance: " << conflict.conflict_distance << "m" << std::endl;
                std::cout << " - Time to conflict: " << conflict.resolution_time << "s" << std::endl;
                std::cout << " - Resolution: " << conflict.resolution_type << std::endl;
            }
        }
        
        if (iteration == 400) {
            std::cout << "\n*** Network Partition Test ***" << std::endl;
            
            // Simulate network partition by disconnecting some aircraft
            for (int i = 0; i < 5; ++i) {
                additional_aircraft[i].shutdown();
            }
            
            // Check swarm health
            bool swarm_healthy = swarm_os.is_swarm_healthy();
            std::cout << "Swarm health after partition: " << (swarm_healthy ? "HEALTHY" : "DEGRADED") << std::endl;
        }
        
        // Display status every 50 iterations
        if (iteration % 50 == 0) {
            std::cout << "Iteration " << iteration << ":" << std::endl;
            std::cout << "  Swarm Size: " << swarm_positions.size() << " aircraft" << std::endl;
            std::cout << "  System Health: " << swarm_os.is_swarm_healthy() << std::endl;
            std::cout << "  Swarm Efficiency: " << swarm_os.get_swarm_efficiency() << std::endl;
            std::cout << "  Autonomous Mode: " << (swarm_os.is_autonomous_mode_enabled() ? "ENABLED" : "DISABLED") << std::endl;
            
            std::cout << "  Current Position:" << std::endl;
            std::cout << "    Lat: " << std::fixed << std::setprecision(6) << current_position.latitude << "°" << std::endl;
            std::cout << "    Lon: " << std::fixed << std::setprecision(6) << current_position.longitude << "°" << std::theight << std::endl;
            std::cout << "    Alt: " << std::fixed << std::setprecision(1) << current_position.altitude << "m" << std::endl;
            std::cout << "    Speed: " << std::fixed << std::setprecision(1) << current_position.airspeed << " m/s" << std::endl;
            std::cout << "    Heading: " << std::fixed << std::setprecision(1) << current_position.heading << "°" << std::endl;
            
            std::cout << "  Nearby Aircraft: " << swarm_os.get_swarm_positions().size() << std::endl;
            
            auto nearby_aircraft = swarm_os.get_swarm_positions();
            if (!nearby_aircraft.empty()) {
                std::cout << "  Closest aircraft: " << std::fixed << std::setprecision(3) 
                          << nearby_aircraft[0].latitude << "°, "
                          << nearby_aircraft[0].longitude << "°" << std::endl;
                std::cout << "    Distance: " << std::fixed << std::setprecision(1) 
                          << std::sqrt(
                              std::pow(current_position.latitude - nearby_aircraft[0].latitude, 2) * 111000 +
                              std::pow(current_position.longitude - nearby_aircraft[0].longitude, 2) * 111000 * std::cos(current_position.latitude * M_PI / 180.0) +
                              std::pow(current_position.altitude - nearby_aircraft[0].altitude, 2)
                          ) / 1000.0 << " km" << std::endl;
            }
            
            auto warnings = swarm_os.get_system_warnings();
            if (!warnings.empty()) {
                std::cout << "  System Warnings (" << warnings.size() << "):" << std::endl;
                for (size_t i = 0; i < std::min(warnings.size(), size_t(3)); ++i) {
                    std::cout << "    " << warnings[i] << std::endl;
                }
            }
            
            std::cout << std::endl;
        }
        
        // Simulate real-time swarm operation
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Test emergency procedures
    std::cout << "\n*** Testing Emergency Procedures ***" << std::endl;
    
    // Simulate complete communication failure
    std::cout << "Simulating complete network failure..." << std::endl;
    for (auto& aircraft : additional_aircraft) {
        aircraft.shutdown();
    }
    swarm_os.shutdown();
    
    // Test recovery procedures
    std::cout << "Testing recovery from network partition..." << std::endl;
    
    // Reinitialize as coordinator
    AirSwarmOS recovery_swarm;
    recovery_swarm.initialize(1001, "FLIGHT-001", SwarmRole::COORDINATOR);
    
    // Reconnect other aircraft
    for (int i = 1; i <= 5; ++i) {
        additional_aircraft[i].initialize(1001 + i, "FLIGHT-" + std::to_string(1001 + i), SwarmRole::NODE);
        additional_aircraft[i].update_swarm_state(current_position);
    }
    
    // Check swarm recovery
    for (int i = 0; i < 10; ++i) {
        bool recovered = recovery_swarm.update_swarm_state(current_position);
        std::cout << "Recovery attempt " << i + 1 << ": " << (recovered ? "SUCCESS" : "FAILED") << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    // Final system status
    std::cout << "\n=== Final System Status ===" << std::endl;
    std::cout << "System Health: " << swarm_os.is_swarm_healthy() << std::endl;
    std::cout << "Swarm Efficiency: " << swarm_os.get_swarm_efficiency() << std::endl;
    std::cout << "Autonomous Mode: " << (swarm_os.is_autonomous_mode_enabled() ? "ENABLED" : "DISABLED") << std::endl;
    
    auto final_block = swarm_os.get_latest_block();
    std::cout << "Blockchain Length: " << final_block.block_id + 1 << " blocks" << std::endl;
    
    std::cout << "\nAir Swarm OS Demo completed successfully!" << std::endl;
    std::cout << "Demonstrated revolutionary distributed air traffic management." << std::endl;
    std::cout << "Each aircraft acts as a blockchain node for complete autonomy!" << std::endl;
    
    return 0;
}
