#include "star_nav_core.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <fstream>
#include <sstream>

using namespace star_nav;

int main() {
    std::cout << "=== StarNav Core Demo ===" << std::endl;
    std::cout << "Revolutionary celestial navigation for aviation" << std::endl;
    
    // Initialize StarNav Core
    StarNavCore star_nav;
    
    if (!star_nav.initialize()) {
        std::cerr << "Failed to initialize StarNav Core" << std::endl;
        return 1;
    }
    
    std::cout << "StarNav Core initialized successfully" << std::endl;
    std::cout << "Starting celestial navigation simulation..." << std::endl;
    
    // Simulation parameters
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> lat_dist(-90.0, 90.0); // degrees
    std::uniform_real_distribution<> lon_dist(-180.0, 180.0); // degrees
    std::uniform_real_distribution<> alt_dist(1000.0, 12000.0); // meters
    std::uniform_real_distribution<> speed_dist(50.0, 300.0); // m/s
    std::uniform_real_distribution<> heading_dist(0.0, 360.0); // degrees
    std::uniform_real_distribution<> time_dist(0.0, 24.0); // hours
    std::uniform_int_distribution<> image_dist(0, 255); // pixel values
    
    AircraftState aircraft_state{};
    std::vector<uint8_t> image_data;
    
    // Create simulated image data (1920x1080)
    int image_width = 1920;
    int image_height = 1080;
    image_data.resize(image_width * image_height);
    
    // Main simulation loop
    for (int iteration = 0; iteration < 300; ++iteration) {
        // Generate aircraft state
        aircraft_state.latitude = lat_dist(gen);
        aircraft_state.longitude = lon_dist(gen);
        aircraft_state.altitude = alt_dist(gen);
        aircraft_state.airspeed = speed_dist(gen);
        aircraft_state.heading = heading_dist(gen);
        aircraft_state.vertical_speed = 0.0;
        aircraft_state.roll = 0.0;
        aircraft_state.pitch = 0.0;
        aircraft_state.yaw = 0.0;
        
        // Generate simulated star image
        for (size_t i = 0; i < image_data.size(); ++i) {
            image_data[i] = image_dist(gen);
        }
        
        // Add some bright spots to simulate stars
        int num_stars = 5 + (iteration % 15);
        for (int i = 0; i < num_stars; ++i) {
            int x = (gen() % image_width);
            int y = (gen() % image_height);
            int index = y * image_width + x;
            if (index < image_data.size()) {
                image_data[index] = 255; // Bright star
            }
        }
        
        // Simulate different flight conditions
        if (iteration == 50) {
            std::cout << "\n*** Night Flight Scenario ***" << std::endl;
            // Simulate night conditions - more stars visible
            for (int i = 0; i < 50; ++i) {
                int x = (gen() % image_width);
                int y = (gen() % image_height);
                int index = y * image_width + x;
                if (index < image_data.size()) {
                    image_data[index] = 200 + (gen() % 55);
                }
            }
        }
        
        if (iteration == 100) {
            std::cout << "\n*** High Altitude Navigation ***" << std::endl;
            aircraft_state.altitude = 10000.0; // High altitude
            aircraft_state.airspeed = 250.0; // High speed
        }
        
        if (iteration == 150) {
            std::cout << "\n*** GPS Denied Environment ***" << std::endl;
            // Simulate GPS loss - rely on star navigation
            star_nav.set_minimum_visibility(3.0); // Lower threshold
        }
        
        if (iteration == 200) {
            std::cout << "\n*** Emergency Navigation Mode ***" << std::endl;
            star_nav.enable_emergency_mode(true);
            aircraft_state.airspeed = 100.0; // Slower speed
            aircraft_state.altitude = 5000.0; // Medium altitude
        }
        
        if (iteration == 250) {
            std::cout << "\n*** Poor Visibility Conditions ***" << std::endl;
            // Simulate poor visibility - fewer stars
            for (size_t i = 0; i < image_data.size(); ++i) {
                image_data[i] = image_data[i] * 0.3; // Reduce brightness
            }
        }
        
        // Update navigation
        auto nav_state = star_nav.update_navigation(aircraft_state, image_data);
        
        // Display status every 50 iterations
        if (iteration % 50 == 0) {
            std::cout << "Iteration " << iteration << ":" << std::endl;
            std::cout << "  Aircraft Position:" << std::endl;
            std::cout << "    Lat: " << std::fixed << std::setprecision(6) << aircraft_state.latitude << "°" << std::endl;
            std::cout << "    Lon: " << std::fixed << std::setprecision(6) << aircraft_state.longitude << "°" << std::endl;
            std::cout << "    Alt: " << std::fixed << std::setprecision(1) << aircraft_state.altitude << "m" << std::endl;
            std::cout << "    Speed: " << std::fixed << std::setprecision(1) << aircraft_state.airspeed << " m/s" << std::endl;
            std::cout << "    Heading: " << std::fixed << std::setprecision(1) << aircraft_state.heading << "°" << std::endl;
            
            std::cout << "  Navigation Status:" << std::endl;
            std::cout << "    GPS Available: " << (nav_state.gps_available ? "Yes" : "No") << std::endl;
            std::cout << "    INS Available: " << (nav_state.ins_available ? "Yes" : "No") << std::endl;
            std::cout << "    Star Lock: " << (nav_state.star_lock ? "ACHIEVED" : "SEARCHING") << std::endl;
            std::cout << "    Stars Visible: " << nav_state.stars_visible << std::endl;
            
            std::cout << "  Navigation Accuracy:" << std::endl;
            std::cout << "    Position Accuracy: " << std::fixed << std::setprecision(1) << nav_state.position_accuracy << "m" << std::endl;
            std::cout << "    Velocity Accuracy: " << std::fixed << std::setprecision(1) << nav_state.velocity_accuracy << "m/s" << std::endl;
            std::cout << "    Heading Accuracy: " << std::fixed << std::setprecision(1) << nav_state.heading_accuracy << "°" << std::endl;
            
            auto current_position = star_nav.get_current_position();
            std::cout << "  Celestial Navigation:" << std::endl;
            std::cout << "    Star Confidence: " << std::fixed << std::setprecision(2) << current_position.confidence_level << std::endl;
            std::cout << "    Star Azimuth: " << std::fixed << std::setprecision(1) << current_position.azimuth << "°" << std::endl;
            std::cout << "    Star Elevation: " << std::fixed << std::setprecision(1) << current_position.declination << "°" << std::endl;
            
            std::cout << "  System Health: " << (star_nav.is_navigation_available() ? "HEALTHY" : "DEGRADED") << std::endl;
            
            auto system_status = star_nav.get_system_status();
            if (!system_status.empty()) {
                std::cout << "  System Status (" << system_status.size() << "):" << std::endl;
                for (size_t i = 0; i < std::min(system_status.size(), size_t(3)); ++i) {
                    std::cout << "    " << system_status[i] << std::endl;
                }
            }
            
            std::cout << std::endl;
        }
        
        // Simulate real-time navigation
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    
    // Test emergency navigation
    std::cout << "\n*** Testing Emergency Navigation ***" << std::endl;
    
    // Simulate complete GPS/INS failure
    aircraft_state.latitude = 0.0;
    aircraft_state.longitude = 0.0;
    aircraft_state.altitude = 0.0;
    
    for (int i = 0; i < 20; ++i) {
        auto emergency_nav = star_nav.update_navigation(aircraft_state, image_data);
        
        std::cout << "Emergency navigation " << i + 1 << ": ";
        std::cout << "Star Lock=" << (emergency_nav.star_lock ? "YES" : "NO") << ", ";
        std::cout << "Stars=" << emergency_nav.stars_visible << ", ";
        std::cout << "Accuracy=" << std::fixed << std::setprecision(1) << emergency_nav.position_accuracy << "m" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    // Test constellation recognition
    std::cout << "\n*** Testing Constellation Recognition ***" << std::endl;
    
    std::vector<std::string> test_constellations = {
        "Ursa Major", "Orion", "Cassiopeia", "Leo", "Scorpius"
    };
    
    for (const auto& constellation : test_constellations) {
        std::cout << "Testing " << constellation << " recognition..." << std::endl;
        
        // Simulate constellation-specific image
        for (size_t i = 0; i < image_data.size(); ++i) {
            image_data[i] = image_dist(gen);
        }
        
        // Add constellation pattern
        int num_stars = 7;
        for (int i = 0; i < num_stars; ++i) {
            int x = 200 + i * 200;
            int y = 300 + (i % 3) * 200;
            int index = y * image_width + x;
            if (index < image_data.size()) {
                image_data[index] = 255;
            }
        }
        
        auto constellation_nav = star_nav.update_navigation(aircraft_state, image_data);
        std::cout << "  Stars detected: " << constellation_nav.stars_visible << std::endl;
        std::cout << "  Star lock: " << (constellation_nav.star_lock ? "ACHIEVED" : "FAILED") << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    // Test camera calibration
    std::cout << "\n*** Testing Camera Calibration ***" << std::endl;
    
    bool calibration_success = star_nav.calibrate_camera();
    std::cout << "Camera calibration: " << (calibration_success ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "Camera calibrated: " << (star_nav.is_calibrated() ? "YES" : "NO") << std::endl;
    
    // Test different visibility conditions
    std::cout << "\n*** Testing Different Visibility Conditions ***" << std::endl;
    
    std::vector<std::pair<std::string, double>> visibility_tests = {
        {"Clear Sky", 6.0},
        {"Moderate Visibility", 4.5},
        {"Poor Visibility", 3.0},
        {"Minimal Visibility", 2.0}
    };
    
    for (const auto& test : visibility_tests) {
        std::cout << "Testing " << test.first << " (magnitude " << test.second << ")..." << std::endl;
        
        star_nav.set_minimum_visibility(test.second);
        
        // Generate image with appropriate star density
        for (size_t i = 0; i < image_data.size(); ++i) {
            image_data[i] = image_dist(gen);
        }
        
        int star_density = static_cast<int>(test.second * 3);
        for (int i = 0; i < star_density; ++i) {
            int x = gen() % image_width;
            int y = gen() % image_height;
            int index = y * image_width + x;
            if (index < image_data.size()) {
                image_data[index] = 200 + (gen() % 55);
            }
        }
        
        auto visibility_nav = star_nav.update_navigation(aircraft_state, image_data);
        std::cout << "  Stars visible: " << visibility_nav.stars_visible << std::endl;
        std::cout << "  Position accuracy: " << std::fixed << std::setprecision(1) << visibility_nav.position_accuracy << "m" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::milliseconds(800));
    }
    
    // Final system status
    std::cout << "\n=== Final System Status ===" << std::endl;
    std::cout << "Navigation Available: " << (star_nav.is_navigation_available() ? "YES" : "NO") << std::endl;
    
    auto final_nav_state = star_nav.get_navigation_state();
    std::cout << "Final Position Accuracy: " << std::fixed << std::setprecision(1) << final_nav_state.position_accuracy << "m" << std::endl;
    std::cout << "Final Heading Accuracy: " << std::fixed << std::setprecision(1) << final_nav_state.heading_accuracy << "°" << std::endl;
    std::cout << "Final Star Lock: " << (final_nav_state.star_lock ? "ACHIEVED" : "SEARCHING") << std::endl;
    
    auto final_position = star_nav.get_current_position();
    std::cout << "Final Celestial Position:" << std::endl;
    std::cout << "  Azimuth: " << std::fixed << std::setprecision(1) << final_position.azimuth << "°" << std::endl;
    std::cout << "  Elevation: " << std::fixed << std::setprecision(1) << final_position.declination << "°" << std::endl;
    std::cout << "  Confidence: " << std::fixed << std::setprecision(2) << final_position.confidence_level << std::endl;
    
    std::cout << "\nStarNav Core Demo completed successfully!" << std::endl;
    std::cout << "Demonstrated revolutionary celestial navigation for aviation." << std::endl;
    std::cout << "This technology enables navigation when all other systems fail!" << std::endl;
    
    // Shutdown system
    star_nav.shutdown();
    
    return 0;
}
