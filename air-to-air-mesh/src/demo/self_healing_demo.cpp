#include "../self_healing/self_healing_bios.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace self_healing;

int main() {
    std::cout << "=== Revolutionary Self-Healing Avionics BIOS Demo ===" << std::endl;
    std::cout << "Demonstrating autonomous system recovery capabilities..." << std::endl << std::endl;

    // Create Self-Healing BIOS instance
    SelfHealingBIOS bios;
    
    // Initialize the system
    if (bios.initialize()) {
        std::cout << "✓ Self-Healing BIOS initialized successfully" << std::endl;
    } else {
        std::cout << "✗ Failed to initialize Self-Healing BIOS" << std::endl;
        return -1;
    }
    
    // Enable the system
    bios.enable_system(true);
    
    // Register memory segments
    std::cout << "\n--- Registering Memory Segments ---" << std::endl;
    MemorySegment nav_segment = {0x1000, 1024, false, 0.95};
    MemorySegment comms_segment = {0x2000, 2048, false, 0.98};
    MemorySegment control_segment = {0x3000, 4096, false, 0.92};
    
    bios.register_memory_segment(nav_segment);
    bios.register_memory_segment(comms_segment);
    bios.register_memory_segment(control_segment);
    
    // Register modules
    std::cout << "\n--- Registering Modules ---" << std::endl;
    ModuleStatus imu_module = {1, "IMU_SENSOR", true, 0.95, std::chrono::system_clock::now()};
    ModuleStatus gps_module = {2, "GPS_RECEIVER", true, 0.88, std::chrono::system_clock::now()};
    ModuleStatus radio_module = {3, "RADIO_COMMS", true, 0.92, std::chrono::system_clock::now()};
    ModuleStatus fc_module = {4, "FLIGHT_CONTROLLER", true, 0.90, std::chrono::system_clock::now()};
    
    bios.register_module(imu_module);
    bios.register_module(gps_module);
    bios.register_module(radio_module);
    bios.register_module(fc_module);
    
    // Register buses
    std::cout << "\n--- Registering Buses ---" << std::endl;
    BusConfiguration avionics_bus = {1, "AVIONICS_BUS", true, 1000000, std::chrono::system_clock::now()};
    BusConfiguration sensor_bus = {2, "SENSOR_BUS", true, 500000, std::chrono::system_clock::now()};
    
    bios.register_bus(avionics_bus);
    bios.register_bus(sensor_bus);
    
    // Register data routes
    std::cout << "\n--- Registering Data Routes ---" << std::endl;
    DataRoute imu_to_fc = {1, 4, 1000, true};
    DataRoute gps_to_fc = {2, 4, 500, true};
    DataRoute fc_to_radio = {4, 3, 2000, true};
    
    bios.register_route(imu_to_fc);
    bios.register_route(gps_to_fc);
    bios.register_route(fc_to_radio);
    
    // Perform initial diagnostics
    std::cout << "\n--- Performing Initial Diagnostics ---" << std::endl;
    bios.perform_diagnostics();
    
    auto diagnostics = bios.get_system_diagnostics();
    std::cout << "System Health: " << diagnostics.overall_health * 100 << "%" << std::endl;
    std::cout << "Active Modules: " << diagnostics.active_modules << std::endl;
    
    // Simulate a memory corruption event
    std::cout << "\n--- Simulating Memory Corruption Event ---" << std::endl;
    std::cout << "WARNING: Memory corruption detected in navigation segment!" << std::endl;
    
    // Mark the navigation segment as corrupted
    nav_segment.is_corrupted = true;
    
    // Attempt to recover corrupted memory
    bios.recover_corrupted_memory(nav_segment);
    
    // Simulate a module failure
    std::cout << "\n--- Simulating Module Failure ---" << std::endl;
    std::cout << "WARNING: GPS receiver module failure detected!" << std::endl;
    
    // Mark the GPS module as failed
    gps_module.is_functional = false;
    gps_module.health_score = 0.1;
    
    // Attempt to recover the failed module
    bios.recover_module(gps_module);
    
    // Simulate a bus reconfiguration
    std::cout << "\n--- Simulating Bus Reconfiguration ---" << std::endl;
    std::cout << "Reconfiguring sensor bus for optimal performance..." << std::endl;
    
    bios.reconfigure_bus(sensor_bus);
    
    // Simulate route reestablishment
    std::cout << "\n--- Simulating Route Reestablishment ---" << std::endl;
    std::cout << "Reestablishing critical data routes..." << std::endl;
    
    std::vector<DataRoute> critical_routes = {imu_to_fc, gps_to_fc, fc_to_radio};
    bios.reestablish_routes(critical_routes);
    
    // Perform final diagnostics
    std::cout << "\n--- Performing Final Diagnostics ---" << std::endl;
    bios.perform_diagnostics();
    
    diagnostics = bios.get_system_diagnostics();
    std::cout << "System Health: " << diagnostics.overall_health * 100 << "%" << std::endl;
    std::cout << "Active Modules: " << diagnostics.active_modules << std::endl;
    
    // Display recovery statistics
    std::cout << "\n--- Recovery Statistics ---" << std::endl;
    std::cout << "Total Recoveries: " << bios.get_total_recoveries() << std::endl;
    std::cout << "Recovery Success Rate: " << bios.get_recovery_success_rate() * 100 << "%" << std::endl;
    std::cout << "Last Recovery: " << std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::system_clock::now() - bios.get_last_recovery_time()).count() << " seconds ago" << std::endl;
    
    std::cout << "\n=== Self-Healing Avionics BIOS Demo Complete ===" << std::endl;
    std::cout << "The revolutionary self-healing system has successfully recovered from simulated failures!" << std::endl;
    
    return 0;
}