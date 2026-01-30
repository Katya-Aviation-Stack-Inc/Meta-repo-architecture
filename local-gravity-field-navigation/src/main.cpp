#include "lgfn_core.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace lgfn;

int main() {
    std::cout << "=== Local Gravity Field Navigation (LGFN) Demo ===" << std::endl;
    
    // Initialize Local Gravity Field Navigation system
    LocalGravityFieldNavigation lgfn_system;
    
    if (!lgfn_system.initialize()) {
        std::cerr << "Failed to initialize LGFN system" << std::endl;
        return 1;
    }
    
    std::cout << "Starting gravity field navigation simulation..." << std::endl;
    
    // Simulation parameters
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> lat_dist(-90.0, 90.0); // degrees
    std::uniform_real_distribution<> lon_dist(-180.0, 180.0); // degrees
    std::uniform_real_distribution<> alt_dist(0.0, 12000.0); // meters
    std::uniform_real_distribution<> vel_dist(-100.0, 100.0); // m/s
    std::uniform_real_distribution<> accuracy_dist(1.0, 20.0); // meters
    
    PositionEstimate gps_position{};
    PositionEstimate ins_position{};
    
    // Main simulation loop
    for (int iteration = 0; iteration < 400; ++iteration) {
        // Generate GPS position (with noise)
        gps_position.latitude = lat_dist(gen);
        gps_position.longitude = lon_dist(gen);
        gps_position.altitude = alt_dist(gen);
        gps_position.velocity_north = vel_dist(gen);
        gps_position.velocity_east = vel_dist(gen);
        gps_position.velocity_down = vel_dist(gen) * 0.1;
        gps_position.position_accuracy = accuracy_dist(gen);
        gps_position.velocity_accuracy = accuracy_dist(gen) * 0.1;
        gps_position.timestamp = std::chrono::system_clock::now();
        gps_position.satellites_used = 8 + (iteration % 5); // 8-12 satellites
        gps_position.gravity_lock = false;
        
        // Generate INS position (drifts over time)
        static double ins_drift = 0.0;
        ins_drift += 0.01; // 1% drift per iteration
        
        ins_position.latitude = gps_position.latitude + ins_drift * 0.001;
        ins_position.longitude = gps_position.longitude + ins_drift * 0.001;
        ins_position.altitude = gps_position.altitude + ins_drift * 10.0;
        ins_position.velocity_north = gps_position.velocity_north + ins_drift;
        ins_position.velocity_east = gps_position.velocity_east + ins_drift;
        ins_position.velocity_down = gps_position.velocity_down + ins_drift * 0.1;
        ins_position.position_accuracy = 5.0 + ins_drift * 10.0; // Degrading accuracy
        ins_position.velocity_accuracy = 1.0 + ins_drift;
        ins_position.timestamp = std::chrono::system_clock::now();
        ins_position.satellites_used = 0;
        ins_position.gravity_lock = false;
        
        // Process navigation update
        auto lgfn_position = lgfn_system.process_navigation_update(
            gps_position, ins_position, 0.1 // 100ms delta time
        );
        
        // Simulate different navigation scenarios
        if (iteration == 50) {
            std::cout << "\n*** GPS Jamming Scenario ***" << std::endl;
            gps_position.satellites_used = 0; // GPS lost
            lgfn_system.enable_gravity_primary_mode();
        }
        
        if (iteration == 100) {
            std::cout << "\n*** Starting Gravity Field Mapping ***" << std::endl;
            lgfn_system.start_mapping_session(5); // 5 minute mapping
        }
        
        if (iteration == 150) {
            std::cout << "\n*** GPS Recovery Test ***" << std::endl;
            gps_position.satellites_used = 10; // GPS recovered
            lgfn_system.enable_hybrid_mode();
        }
        
        if (iteration == 200) {
            std::cout << "\n*** High Altitude Navigation ***" << std::endl;
            gps_position.altitude = 10000.0; // 10 km altitude
            ins_position.altitude = 10000.0;
        }
        
        if (iteration == 250) {
            std::cout << "\n*** Emergency Navigation Mode ***" << std::endl;
            lgfn_system.enable_emergency_mode();
            gps_position.satellites_used = 0; // GPS lost again
        }
        
        if (iteration == 300) {
            std::cout << "\n*** Dense Urban Environment ***" << std::endl;
            lgfn_system.configure_for_environment("urban");
            gps_position.position_accuracy = 50.0; // Poor GPS accuracy
        }
        
        if (iteration == 350) {
            std::cout << "\n*** Returning to Normal Operation ***" << std::endl;
            lgfn_system.enable_hybrid_mode();
            gps_position.satellites_used = 12;
            gps_position.position_accuracy = 3.0;
        }
        
        // Display status every 50 iterations
        if (iteration % 50 == 0) {
            std::cout << "Iteration " << iteration << ":" << std::endl;
            std::cout << "  GPS Available: " << (gps_position.satellites_used > 0 ? "Yes" : "No") << std::endl;
            std::cout << "  GPS Accuracy: " << gps_position.position_accuracy << " m" << std::endl;
            std::cout << "  INS Accuracy: " << ins_position.position_accuracy << " m" << std::endl;
            std::cout << "  LGFN Available: " << (lgfn_system.is_navigation_available() ? "Yes" : "No") << std::endl;
            std::cout << "  LGFN Accuracy: " << lgfn_system.get_position_accuracy() << " m" << std::endl;
            std::cout << "  Gravity Lock: " << (lgfn_system.has_gravity_lock() ? "Yes" : "No") << std::endl;
            std::cout << "  Mapping Active: " << (lgfn_system.is_mapping_active() ? "Yes" : "No") << std::endl;
            
            if (lgfn_system.is_mapping_active()) {
                std::cout << "  Mapping Progress: " << lgfn_system.get_mapping_progress() * 100.0 << "%" << std::endl;
            }
            
            auto current_pos = lgfn_system.get_current_position();
            std::cout << "  Current Position:" << std::endl;
            std::cout << "    Lat: " << current_pos.latitude << "°" << std::endl;
            std::cout << "    Lon: " << current_pos.longitude << "°" << std::endl;
            std::cout << "    Alt: " << current_pos.altitude << " m" << std::endl;
            std::cout << "    Accuracy: " << current_pos.position_accuracy << " m" << std::endl;
            std::cout << std::endl;
        }
        
        // Simulate real-time navigation
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Test gravity field mapping completion
    if (lgfn_system.is_mapping_active()) {
        std::cout << "\n*** Waiting for Mapping Completion ***" << std::endl;
        while (lgfn_system.is_mapping_active()) {
            std::cout << "Mapping progress: " << lgfn_system.get_mapping_progress() * 100.0 << "%" << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        std::cout << "Gravity field mapping completed!" << std::endl;
    }
    
    // Test emergency procedures
    std::cout << "\n*** Testing Emergency Navigation ***" << std::endl;
    lgfn_system.enable_emergency_mode();
    
    // Simulate complete GPS loss
    gps_position.satellites_used = 0;
    gps_position.position_accuracy = 1000.0; // Very poor
    
    for (int i = 0; i < 20; ++i) {
        auto emergency_pos = lgfn_system.process_navigation_update(gps_position, ins_position, 0.1);
        std::cout << "Emergency navigation " << i + 1 << ": Accuracy " << emergency_pos.position_accuracy << " m" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    // Final system status
    std::cout << "\n=== Final System Status ===" << std::endl;
    std::cout << "Navigation Available: " << (lgfn_system.is_navigation_available() ? "Yes" : "No") << std::endl;
    std::cout << "Final Position Accuracy: " << lgfn_system.get_position_accuracy() << " m" << std::endl;
    std::cout << "Gravity Lock Status: " << (lgfn_system.has_gravity_lock() ? "LOCKED" : "SEARCHING") << std::endl;
    
    auto final_position = lgfn_system.get_current_position();
    std::cout << "Final Position:" << std::endl;
    std::cout << "  Latitude: " << final_position.latitude << "°" << std::endl;
    std::cout << "  Longitude: " << final_position.longitude << "°" << std::endl;
    std::cout << "  Altitude: " << final_position.altitude << " m" << std::endl;
    std::cout << "  Gravity Lock: " << (final_position.gravity_lock ? "Yes" : "No") << std::endl;
    
    std::cout << "\nLocal Gravity Field Navigation Demo completed successfully." << std::endl;
    std::cout << "Demonstrated GPS-independent navigation using local gravity field variations." << std::endl;
    
    // Shutdown system
    lgfn_system.shutdown();
    
    return 0;
}
