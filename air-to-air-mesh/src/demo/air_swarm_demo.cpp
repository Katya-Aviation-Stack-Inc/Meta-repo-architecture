#include "../air_swarm/air_swarm_os.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace air_swarm;

int main() {
    std::cout << "=== Revolutionary Air Swarm OS Demo ===" << std::endl;
    std::cout << "Demonstrating distributed air traffic management..." << std::endl << std::endl;

    // Create Air Swarm OS instance
    AirSwarmOS swarm_os;
    
    // Initialize the system
    if (swarm_os.initialize()) {
        std::cout << "✓ Air Swarm OS initialized successfully" << std::endl;
    } else {
        std::cout << "✗ Failed to initialize Air Swarm OS" << std::endl;
        return -1;
    }
    
    // Enable the system
    swarm_os.enable_system(true);
    
    // Set system parameters
    swarm_os.set_consensus_timeout(3.0);
    swarm_os.set_conflict_detection_range(1500.0);
    swarm_os.set_communication_range(40000.0);
    
    // Add aircraft to swarm
    std::cout << "\n--- Adding Aircraft to Swarm ---" << std::endl;
    for (uint32_t i = 1; i <= 5; ++i) {
        swarm_os.add_aircraft_to_swarm(i);
    }
    
    std::cout << "Total aircraft in swarm: " << swarm_os.get_total_aircraft() << std::endl;
    
    // Simulate aircraft states
    std::cout << "\n--- Simulating Aircraft States ---" << std::endl;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> lat_dist(40.0, 41.0);    // Latitude range
    std::uniform_real_distribution<double> lon_dist(-75.0, -74.0); // Longitude range
    std::uniform_real_distribution<double> alt_dist(1000.0, 3000.0); // Altitude range
    std::uniform_real_distribution<double> speed_dist(50.0, 150.0); // Speed range
    
    for (uint32_t i = 1; i <= 5; ++i) {
        AircraftState state;
        state.aircraft_id = i;
        state.latitude = lat_dist(generator);
        state.longitude = lon_dist(generator);
        state.altitude = alt_dist(generator);
        state.heading = (i * 72.0); // Evenly spaced headings
        state.speed = speed_dist(generator);
        state.vertical_speed = 0.0;
        state.flight_phase = "cruise";
        state.timestamp = std::chrono::system_clock::now();
        
        swarm_os.update_aircraft_state(state);
        
        // Add small delay to simulate real-time updates
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Submit flight intents
    std::cout << "\n--- Submitting Flight Intents ---" << std::endl;
    for (uint32_t i = 1; i <= 5; ++i) {
        FlightIntent intent;
        intent.aircraft_id = i;
        intent.waypoints.push_back({lat_dist(generator) + 0.1, lon_dist(generator) + 0.1});
        intent.waypoints.push_back({lat_dist(generator) + 0.2, lon_dist(generator) + 0.2});
        intent.planned_altitude = alt_dist(generator) + 500.0;
        intent.planned_speed = speed_dist(generator) + 20.0;
        intent.submission_time = std::chrono::system_clock::now();
        intent.expiration_time = intent.submission_time + std::chrono::minutes(30);
        intent.is_priority = (i == 1); // First aircraft is priority
        
        swarm_os.submit_flight_intent(intent);
        
        // Add small delay to simulate real-time submissions
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Process swarm intelligence
    std::cout << "\n--- Processing Swarm Intelligence ---" << std::endl;
    swarm_os.process_swarm_intelligence();
    
    // Check current airspace state
    auto airspace_state = swarm_os.get_current_airspace_state();
    std::cout << "Current airspace contains " << airspace_state.aircraft_positions.size() 
              << " aircraft and " << airspace_state.flight_intents.size() 
              << " flight intents" << std::endl;
    
    if (!airspace_state.conflicts.empty()) {
        std::cout << "Detected " << airspace_state.conflicts.size() << " conflicts" << std::endl;
    } else {
        std::cout << "No conflicts detected in airspace" << std::endl;
    }
    
    // Simulate conflicting aircraft
    std::cout << "\n--- Simulating Conflicting Aircraft ---" << std::endl;
    std::cout << "WARNING: Simulating aircraft conflict scenario!" << std::endl;
    
    // Update two aircraft to be very close to each other
    AircraftState conflicting_state1, conflicting_state2;
    conflicting_state1.aircraft_id = 1;
    conflicting_state1.latitude = 40.7128;
    conflicting_state1.longitude = -74.0060;
    conflicting_state1.altitude = 2000.0;
    conflicting_state1.heading = 90.0;
    conflicting_state1.speed = 100.0;
    conflicting_state1.vertical_speed = 0.0;
    conflicting_state1.flight_phase = "cruise";
    conflicting_state1.timestamp = std::chrono::system_clock::now();
    
    conflicting_state2.aircraft_id = 2;
    conflicting_state2.latitude = 40.7129; // Very close latitude
    conflicting_state2.longitude = -74.0061; // Very close longitude
    conflicting_state2.altitude = 2000.0; // Same altitude
    conflicting_state2.heading = 270.0;
    conflicting_state2.speed = 100.0;
    conflicting_state2.vertical_speed = 0.0;
    conflicting_state2.flight_phase = "cruise";
    conflicting_state2.timestamp = std::chrono::system_clock::now();
    
    swarm_os.update_aircraft_state(conflicting_state1);
    swarm_os.update_aircraft_state(conflicting_state2);
    
    // Process swarm intelligence again to detect conflicts
    std::cout << "\n--- Re-processing Swarm Intelligence ---" << std::endl;
    swarm_os.process_swarm_intelligence();
    
    // Check for conflicts
    airspace_state = swarm_os.get_current_airspace_state();
    if (!airspace_state.conflicts.empty()) {
        std::cout << "✓ Conflicts successfully detected: " << airspace_state.conflicts.size() << " conflicts" << std::endl;
        
        // Resolve conflicts
        std::cout << "\n--- Resolving Conflicts ---" << std::endl;
        swarm_os.resolve_conflicts();
        
        std::cout << "Conflicts resolved successfully" << std::endl;
    }
    
    // Reach consensus on flight intents
    std::cout << "\n--- Reaching Consensus ---" << std::endl;
    auto flight_intents = swarm_os.get_current_airspace_state().flight_intents;
    std::vector<FlightIntent> intent_vector;
    for (const auto& pair : flight_intents) {
        intent_vector.push_back(pair.second);
    }
    
    ConsensusDecision decision = swarm_os.reach_consensus(intent_vector);
    
    // Execute consensus decision
    std::cout << "\n--- Executing Consensus Decision ---" << std::endl;
    swarm_os.execute_consensus_decision(decision);
    
    // Display swarm metrics
    auto metrics = swarm_os.get_swarm_metrics();
    
    std::cout << "\n--- Swarm Performance Metrics ---" << std::endl;
    std::cout << "Total Aircraft: " << swarm_os.get_total_aircraft() << std::endl;
    std::cout << "Active Flights: " << swarm_os.get_active_flights() << std::endl;
    std::cout << "Resolved Conflicts: " << swarm_os.get_resolved_conflicts() << std::endl;
    std::cout << "Consensus Decisions: " << metrics.consensus_decisions << std::endl;
    std::cout << "Network Efficiency: " << swarm_os.get_network_efficiency() * 100 << "%" << std::endl;
    std::cout << "Average Decision Time: " << metrics.average_decision_time << " ms" << std::endl;
    
    std::cout << "\n=== Air Swarm OS Demo Complete ===" << std::endl;
    std::cout << "The revolutionary distributed air traffic management system has successfully coordinated aircraft and resolved conflicts!" << std::endl;
    
    return 0;
}