// Cold-Jet Software Control Core - Main Entry Point
// Placeholder for super-cooled air thrust control

#include "cold_jet_core.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace cold_jet;

int main() {
    std::cout << "=== Cold-Jet Software Control Core Demo ===" << std::endl;
    
    // Initialize Cold-Jet Software Core
    ColdJetSoftwareCore cold_jet_core;
    
    if (!cold_jet_core.initialize()) {
        std::cerr << "Failed to initialize Cold-Jet Core" << std::endl;
        return 1;
    }
    
    std::cout << "Starting Cold-Jet propulsion simulation..." << std::endl;
    
    // Simulation parameters
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> thrust_dist(1000.0, 5000.0); // Newtons
    std::uniform_real_distribution<> azimuth_dist(-30.0, 30.0); // degrees
    std::uniform_real_distribution<> elevation_dist(-15.0, 15.0); // degrees
    std::uniform_real_distribution<> altitude_dist(0.0, 10000.0); // meters
    std::uniform_real_distribution<> airspeed_dist(0.0, 200.0); // m/s
    std::uniform_real_distribution<> temp_dist(-50.0, 40.0); // Celsius
    
    // Main simulation loop
    for (int iteration = 0; iteration < 300; ++iteration) {
        // Generate flight conditions
        double altitude = altitude_dist(gen);
        double airspeed = airspeed_dist(gen);
        double ambient_temp_celsius = temp_dist(gen);
        double ambient_temp_kelvin = ambient_temp_celsius + 273.15;
        double ambient_pressure = 101325.0 * std::pow(1.0 - 0.0065 * altitude / 288.15, 5.256);
        
        // Generate control commands
        double thrust_command = thrust_dist(gen);
        double azimuth_command = azimuth_dist(gen) * M_PI / 180.0;
        double elevation_command = elevation_dist(gen) * M_PI / 180.0;
        double cooling_command = 1000.0 + iteration * 10.0; // Increasing cooling demand
        
        // Set commands to Cold-Jet core
        cold_jet_core.set_thrust_command(thrust_command);
        cold_jet_core.set_direction_command(azimuth_command * 180.0 / M_PI, elevation_command * 180.0 / M_PI);
        cold_jet_core.set_cooling_command(cooling_command);
        
        // Process control loop
        auto jet_parameters = cold_jet_core.process_control_loop(
            altitude, airspeed, ambient_temp_kelvin, ambient_pressure
        );
        
        // Simulate different flight phases
        if (iteration == 50) {
            std::cout << "\n*** Takeoff Phase ***" << std::endl;
            cold_jet_core.set_flight_phase("takeoff");
            cold_jet_core.set_thrust_command(8000.0); // High thrust
        }
        
        if (iteration == 100) {
            std::cout << "\n*** Cruise Phase ***" << std::endl;
            cold_jet_core.set_flight_phase("cruise");
            cold_jet_core.set_thrust_command(3000.0); // Moderate thrust
        }
        
        if (iteration == 150) {
            std::cout << "\n*** High Altitude Operation ***" << std::endl;
            cold_jet_core.set_flight_phase("high_altitude");
            cold_jet_core.set_cooling_command(5000.0); // High cooling demand
        }
        
        if (iteration == 200) {
            std::cout << "\n*** Emergency Cooling Mode ***" << std::endl;
            cold_jet_core.set_flight_phase("emergency");
            cold_jet_core.set_cooling_command(15000.0); // Maximum cooling
        }
        
        if (iteration == 250) {
            std::cout << "\n*** Precision Vectoring Test ***" << std::endl;
            cold_jet_core.set_flight_phase("hover");
            cold_jet_core.set_direction_command(10.0, 5.0); // Precise vectoring
        }
        
        // Display status every 50 iterations
        if (iteration % 50 == 0) {
            std::cout << "Iteration " << iteration << ":" << std::endl;
            std::cout << "  Altitude: " << altitude << " m" << std::endl;
            std::cout << "  Airspeed: " << airspeed << " m/s" << std::endl;
            std::cout << "  Ambient Temp: " << ambient_temp_celsius << "°C" << std::endl;
            std::cout << "  System Ready: " << (cold_jet_core.is_system_ready() ? "Yes" : "No") << std::endl;
            std::cout << "  Current Thrust: " << cold_jet_core.get_current_thrust() << " N" << std::endl;
            std::cout << "  Cooling Capacity: " << cold_jet_core.get_cooling_capacity() << " W" << std::endl;
            
            auto thrust_vector = cold_jet_core.get_thrust_vector();
            std::cout << "  Thrust Vector: " << thrust_vector.thrust_magnitude << " N, "
                      << "Az: " << thrust_vector.azimuth_angle * 180.0 / M_PI << "°, "
                      << "El: " << thrust_vector.elevation_angle * 180.0 / M_PI << "°" << std::endl;
            
            std::cout << "  Jet Parameters:" << std::endl;
            std::cout << "    Inlet Temp: " << jet_parameters.inlet_temperature << " K" << std::endl;
            std::cout << "    Outlet Temp: " << jet_parameters.outlet_temperature << " K" << std::endl;
            std::cout << "    Mass Flow: " << jet_parameters.mass_flow_rate << " kg/s" << std::endl;
            std::cout << "    Pressure Ratio: " << jet_parameters.pressure_ratio << std::endl;
            std::cout << "    Efficiency: " << jet_parameters.efficiency * 100.0 << "%" << std::endl;
            std::cout << std::endl;
        }
        
        // Simulate real-time control loop
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    
    // Test emergency procedures
    std::cout << "\n*** Testing Emergency Procedures ***" << std::endl;
    cold_jet_core.set_flight_phase("emergency");
    cold_jet_core.set_cooling_command(20000.0); // Emergency cooling
    
    for (int i = 0; i < 10; ++i) {
        auto params = cold_jet_core.process_control_loop(5000.0, 100.0, 268.15, 54000.0);
        std::cout << "Emergency test " << i + 1 << ": Cooling power " << params.cooling_power << " W" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Test automatic optimization
    std::cout << "\n*** Enabling Automatic Optimization ***" << std::endl;
    cold_jet_core.enable_automatic_optimization(true);
    
    for (int i = 0; i < 20; ++i) {
        cold_jet_core.set_thrust_command(4000.0 + i * 100.0);
        auto params = cold_jet_core.process_control_loop(8000.0, 150.0, 253.15, 37000.0);
        std::cout << "Optimization " << i + 1 << ": Efficiency " << params.efficiency * 100.0 << "%" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    
    // Final system status
    std::cout << "\n=== Final System Status ===" << std::endl;
    std::cout << "System Ready: " << (cold_jet_core.is_system_ready() ? "Yes" : "No") << std::endl;
    std::cout << "Final Thrust: " << cold_jet_core.get_current_thrust() << " N" << std::endl;
    std::cout << "Final Cooling Capacity: " << cold_jet_core.get_cooling_capacity() << " W" << std::endl;
    
    auto final_vector = cold_jet_core.get_thrust_vector();
    std::cout << "Final Thrust Vector: " << final_vector.thrust_magnitude << " N" << std::endl;
    
    std::cout << "\nCold-Jet Demo completed successfully." << std::endl;
    std::cout << "Demonstrated super-cooled air thrust generation, vectoring, and thermal management." << std::endl;
    
    // Shutdown system
    cold_jet_core.shutdown();
    
    return 0;
}
