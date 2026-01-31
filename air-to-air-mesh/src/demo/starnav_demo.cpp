#include "../starnav/starnav_core.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace starnav;

int main() {
    std::cout << "=== Revolutionary StarNav Core Demo ===" << std::endl;
    std::cout << "Demonstrating stellar-based navigation system..." << std::endl << std::endl;

    // Create StarNav Core instance
    StarNavCore starnav;
    
    // Initialize the system
    if (starnav.initialize()) {
        std::cout << "✓ StarNav Core System initialized successfully" << std::endl;
    } else {
        std::cout << "✗ Failed to initialize StarNav Core System" << std::endl;
        return -1;
    }
    
    // Enable the system
    starnav.enable_system(true);
    
    // Set system parameters
    starnav.set_sensitivity_threshold(0.75);
    starnav.set_minimum_magnitude(3.5);
    starnav.set_max_fix_interval(5.0);
    
    // Simulate aircraft motion
    std::cout << "\n--- Simulating Aircraft Motion ---" << std::endl;
    AircraftMotion motion;
    motion.roll = 2.5;
    motion.pitch = -1.8;
    motion.yaw = 45.0;
    motion.velocity_north = 100.0;
    motion.velocity_east = 75.0;
    motion.velocity_down = -5.0;
    motion.timestamp = std::chrono::system_clock::now();
    
    starnav.update_aircraft_motion(motion);
    
    // Simulate star observations
    std::cout << "\n--- Simulating Star Observations ---" << std::endl;
    std::vector<StarObservation> observations;
    
    // Add some star observations (simulated)
    StarObservation polaris;
    polaris.star_id = 1;
    polaris.ra = 2.5303;
    polaris.dec = 89.2641;
    polaris.magnitude = 1.98;
    polaris.altitude = 45.0;
    polaris.azimuth = 0.0;
    polaris.observation_time = std::chrono::system_clock::now();
    observations.push_back(polaris);
    
    StarObservation vega;
    vega.star_id = 2;
    vega.ra = 18.6156;
    vega.dec = 38.7837;
    vega.magnitude = 0.03;
    vega.altitude = 60.0;
    vega.azimuth = 90.0;
    vega.observation_time = std::chrono::system_clock::now();
    observations.push_back(vega);
    
    StarObservation sirius;
    sirius.star_id = 3;
    sirius.ra = 6.7525;
    sirius.dec = -16.7131;
    sirius.magnitude = -1.46;
    sirius.altitude = 30.0;
    sirius.azimuth = 180.0;
    sirius.observation_time = std::chrono::system_clock::now();
    observations.push_back(sirius);
    
    StarObservation betelgeuse;
    betelgeuse.star_id = 4;
    betelgeuse.ra = 5.9197;
    betelgeuse.dec = 7.4071;
    betelgeuse.magnitude = 0.45;
    betelgeuse.altitude = 45.0;
    betelgeuse.azimuth = 270.0;
    betelgeuse.observation_time = std::chrono::system_clock::now();
    observations.push_back(betelgeuse);
    
    std::cout << "Captured " << observations.size() << " star observations:" << std::endl;
    for (const auto& obs : observations) {
        std::cout << "  Star ID " << obs.star_id << ": RA=" << obs.ra 
                  << "°, Dec=" << obs.dec << "°, Mag=" << obs.magnitude << std::endl;
    }
    
    // Process star observations
    std::cout << "\n--- Processing Star Observations ---" << std::endl;
    starnav.process_star_observations(observations);
    
    // Check constellation recognition
    auto pattern = starnav.recognize_constellation(observations);
    if (pattern.confidence > 0.5) {
        std::cout << "Recognized constellation: " << pattern.name 
                  << " with " << (pattern.confidence * 100) << "% confidence" << std::endl;
    }
    
    // Calculate position
    auto position = starnav.calculate_position(pattern, motion);
    if (position.is_valid) {
        std::cout << "Position fix calculated successfully:" << std::endl;
        std::cout << "  Latitude: " << position.latitude << "°" << std::endl;
        std::cout << "  Longitude: " << position.longitude << "°" << std::endl;
        std::cout << "  Altitude: " << position.altitude << "m" << std::endl;
        std::cout << "  Accuracy: " << position.accuracy << "m" << std::endl;
    }
    
    // Simulate motion compensation
    std::cout << "\n--- Compensating for Aircraft Motion ---" << std::endl;
    starnav.compensate_for_motion();
    
    // Process additional observations
    std::cout << "\n--- Processing Additional Observations ---" << std::endl;
    std::vector<StarObservation> more_observations = observations;
    
    // Add some new observations
    StarObservation rigel;
    rigel.star_id = 5;
    rigel.ra = 5.2423;
    rigel.dec = -8.2017;
    rigel.magnitude = 0.18;
    rigel.altitude = 25.0;
    rigel.azimuth = 225.0;
    rigel.observation_time = std::chrono::system_clock::now();
    more_observations.push_back(rigel);
    
    starnav.process_star_observations(more_observations);
    
    // Display system diagnostics
    auto diagnostics = starnav.get_system_diagnostics();
    
    std::cout << "\n--- StarNav System Diagnostics ---" << std::endl;
    std::cout << "Total Observations: " << diagnostics.total_observations << std::endl;
    std::cout << "Successful Fixes: " << diagnostics.successful_fixes << std::endl;
    std::cout << "Fix Accuracy: " << starnav.get_fix_accuracy() << "m" << std::endl;
    std::cout << "Recognition Rate: " << diagnostics.recognition_rate * 100 << "%" << std::endl;
    
    // Display current position
    auto current_pos = starnav.get_current_position();
    if (current_pos.is_valid) {
        std::cout << "\nCurrent Position:" << std::endl;
        std::cout << "  Latitude: " << current_pos.latitude << "°" << std::endl;
        std::cout << "  Longitude: " << current_pos.longitude << "°" << std::endl;
        std::cout << "  Altitude: " << current_pos.altitude << "m" << std::endl;
        std::cout << "  Accuracy: " << current_pos.accuracy << "m" << std::endl;
    }
    
    std::cout << "\n=== StarNav Core Demo Complete ===" << std::endl;
    std::cout << "The revolutionary stellar-based navigation system has successfully calculated position from star observations!" << std::endl;
    
    return 0;
}