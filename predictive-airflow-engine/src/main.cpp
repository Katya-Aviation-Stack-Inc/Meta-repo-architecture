#include "predictive_airflow_core.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

using namespace predictive_airflow;

int main() {
    std::cout << "=== Predictive Airflow Engine Demo ===" << std::endl;
    
    // Initialize Predictive Airflow Engine
    PredictiveAirflowEngine airflow_engine;
    
    if (!airflow_engine.initialize()) {
        std::cerr << "Failed to initialize Predictive Airflow Engine" << std::endl;
        return 1;
    }
    
    std::cout << "Starting predictive airflow simulation..." << std::endl;
    
    // Simulation parameters
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> airspeed_dist(50.0, 300.0); // m/s
    std::uniform_real_distribution<> aoa_dist(-10.0, 20.0); // degrees
    std::uniform_real_distribution<> altitude_dist(0.0, 15000.0); // meters
    std::uniform_real_distribution<> flow_dist(-20.0, 20.0); // m/s
    std::uniform_real_distribution<> pressure_dist(90000.0, 110000.0); // Pa
    
    AircraftState aircraft_state{};
    std::vector<FlowNode> current_flow_field;
    
    // Initialize flow field
    int grid_size = 1000;
    current_flow_field.resize(grid_size);
    
    for (int i = 0; i < grid_size; ++i) {
        current_flow_field[i].x = i * 0.01; // 1 cm spacing
        current_flow_field[i].y = 0.0;
        current_flow_field[i].z = (i % 10) * 0.1; // 10 cm vertical spacing
        current_flow_field[i].vx = 100.0; // Base flow velocity
        current_flow_field[i].vy = 0.0;
        current_flow_field[i].vz = 0.0;
        current_flow_field[i].pressure = 101325.0;
        current_flow_field[i].temperature = 288.15;
        current_flow_field[i].density = 1.225;
        current_flow_field[i].vorticity = 0.0;
        current_flow_field[i].turbulence_intensity = 0.05;
        current_flow_field[i].is_wake = false;
        current_flow_field[i].is_separated = false;
    }
    
    // Main simulation loop
    for (int iteration = 0; iteration < 500; ++iteration) {
        // Generate aircraft state
        aircraft_state.airspeed = airspeed_dist(gen);
        aircraft_state.angle_of_attack = aoa_dist(gen);
        aircraft_state.sideslip_angle = (iteration % 20 - 10) * 0.5; // ±5 degrees
        aircraft_state.altitude = altitude_dist(gen);
        aircraft_state.mach_number = aircraft_state.airspeed / 340.0; // Speed of sound
        aircraft_state.reynolds_number = aircraft_state.airspeed * 2.0 / KINEMATIC_VISCOSITY;
        aircraft_state.dynamic_pressure = 0.5 * AIR_DENSITY_SEA_LEVEL * aircraft_state.airspeed * aircraft_state.airspeed;
        aircraft_state.angular_velocity = {0.1, 0.05, 0.02};
        aircraft_state.acceleration = {2.0, 1.0, 0.5};
        
        // Update flow field with some variations
        for (auto& node : current_flow_field) {
            node.vx = aircraft_state.airspeed + flow_dist(gen) * 0.1;
            node.vy = flow_dist(gen) * 0.05;
            node.vz = flow_dist(gen) * 0.02;
            node.pressure = pressure_dist(gen);
            node.vorticity = std::abs(flow_dist(gen)) * 0.5;
            node.turbulence_intensity = 0.05 + std::abs(flow_dist(gen)) * 0.01;
            
            // Mark wake regions
            if (node.x > 0.5 && node.y < 0.2) {
                node.is_wake = true;
                node.turbulence_intensity *= 2.0;
            }
            
            // Mark separated flow regions
            if (aircraft_state.angle_of_attack > 15.0 && node.x > 0.3) {
                node.is_separated = true;
                node.vx *= -0.5; // Reverse flow
            }
        }
        
        // Predict airflow
        auto prediction = airflow_engine.predict_airflow(
            aircraft_state, current_flow_field, PredictionMode::UNSTEADY_DYNAMIC, 2.0
        );
        
        // Simulate different flight conditions
        if (iteration == 50) {
            std::cout << "\n*** High Angle of Attack Test ***" << std::endl;
            aircraft_state.angle_of_attack = 18.0; // Near stall
            airflow_engine.set_prediction_mode(PredictionMode::SEPARATION_PREDICTION);
        }
        
        if (iteration == 100) {
            std::cout << "\n*** Vortex Shedding Analysis ***" << std::endl;
            airflow_engine.set_prediction_mode(PredictionMode::VORTEX_SHEDDING);
        }
        
        if (iteration == 150) {
            std::cout << "\n*** Turbulence Encounter ***" << std::endl;
            airflow_engine.set_prediction_mode(PredictionMode::TURBULENCE_MODELING);
            
            // Add turbulence to flow field
            for (auto& node : current_flow_field) {
                node.turbulence_intensity = 0.3 + (iteration % 10) * 0.05;
                node.vx += (iteration % 5 - 2) * 5.0;
                node.vy += (iteration % 7 - 3) * 3.0;
            }
        }
        
        if (iteration == 200) {
            std::cout << "\n*** Emergency Stall Prediction ***" << std::endl;
            aircraft_state.angle_of_attack = 22.0; // Beyond stall
            airflow_engine.set_prediction_mode(PredictionMode::EMERGENCY_ANALYSIS);
            airflow_engine.enable_emergency_predictions(true);
        }
        
        if (iteration == 250) {
            std::cout << "\n*** High-Speed Flight ***" << std::endl;
            aircraft_state.airspeed = 250.0; // High speed
            aircraft_state.mach_number = 0.735; // Transonic
            airflow_engine.set_prediction_mode(PredictionMode::UNSTEADY_DYNAMIC);
        }
        
        if (iteration == 300) {
            std::cout << "\n*** Low-Speed Takeoff ***" << std::endl;
            aircraft_state.airspeed = 80.0; // Takeoff speed
            aircraft_state.angle_of_attack = 8.0;
            airflow_engine.set_prediction_mode(PredictionMode::STEADY_STATE);
        }
        
        if (iteration == 350) {
            std::cout << "\n*** Adaptive Learning Enabled ***" << std::endl;
            airflow_engine.enable_adaptive_learning(true);
        }
        
        if (iteration == 400) {
            std::cout << "\n*** Complex Maneuvering ***" << std::endl;
            aircraft_state.angular_velocity = {0.5, 0.3, 0.1}; // Aggressive maneuvering
            aircraft_state.sideslip_angle = 10.0;
            airflow_engine.set_prediction_mode(PredictionMode::UNSTEADY_DYNAMIC);
        }
        
        // Display status every 50 iterations
        if (iteration % 50 == 0) {
            std::cout << "Iteration " << iteration << ":" << std::endl;
            std::cout << "  Airspeed: " << aircraft_state.airspeed << " m/s" << std::endl;
            std::cout << "  Angle of Attack: " << aircraft_state.angle_of_attack << "°" << std::endl;
            std::cout << "  Mach Number: " << aircraft_state.mach_number << std::endl;
            std::cout << "  System Healthy: " << (airflow_engine.is_system_healthy() ? "Yes" : "No") << std::endl;
            std::cout << "  Prediction Confidence: " << airflow_engine.get_prediction_confidence() << std::endl;
            
            std::cout << "  Aerodynamic Coefficients:" << std::endl;
            std::cout << "    CL: " << prediction.lift_coefficient << std::endl;
            std::cout << "    CD: " << prediction.drag_coefficient << std::endl;
            std::cout << "    CM: " << prediction.moment_coefficient << std::endl;
            
            std::cout << "  Flow Predictions:" << std::endl;
            std::cout << "    Stall Angle: " << prediction.stall_angle << "°" << std::endl;
            std::cout << "    Separation Point: " << prediction.separation_point << std::endl;
            std::cout << "    Vortex Positions: " << prediction.vortex_positions.size() << std::endl;
            std::cout << "    Turbulence Zones: " << prediction.turbulence_zones.size() << std::endl;
            
            // Get additional predictions
            auto vortex_predictions = airflow_engine.get_vortex_predictions(1.0);
            auto turbulence_zones = airflow_engine.get_turbulence_zones();
            
            std::cout << "  Advanced Predictions:" << std::endl;
            std::cout << "    Future Vortices (1s): " << vortex_predictions.size() << std::endl;
            std::cout << "    Current Turbulence Zones: " << turbulence_zones.size() << std::endl;
            std::cout << "    Stall Margin: " << airflow_engine.get_stall_margin() << "°" << std::endl;
            std::cout << std::endl;
        }
        
        // Simulate real-time prediction
        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
    
    // Test emergency predictions
    std::cout << "\n*** Testing Emergency Stall Recovery ***" << std::endl;
    aircraft_state.angle_of_attack = 25.0; // Deep stall
    
    for (int i = 0; i < 10; ++i) {
        auto emergency_prediction = airflow_engine.predict_airflow(
            aircraft_state, current_flow_field, PredictionMode::EMERGENCY_ANALYSIS, 0.5
        );
        
        std::cout << "Emergency prediction " << i + 1 << ": CL=" << emergency_prediction.lift_coefficient 
                  << ", Confidence=" << emergency_prediction.confidence_level << std::endl;
        
        // Simulate recovery action
        aircraft_state.angle_of_attack -= 1.0;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    // Test vortex prediction accuracy
    std::cout << "\n*** Testing Vortex Prediction Accuracy ***" << std::endl;
    
    for (double time_ahead = 0.5; time_ahead <= 3.0; time_ahead += 0.5) {
        auto vortices = airflow_engine.get_vortex_predictions(time_ahead);
        std::cout << "Vortex prediction " << time_ahead << "s ahead: " << vortices.size() << " vortices" << std::endl;
    }
    
    // Final system status
    std::cout << "\n=== Final System Status ===" << std::endl;
    std::cout << "System Healthy: " << (airflow_engine.is_system_healthy() ? "Yes" : "No") << std::endl;
    std::cout << "Final Prediction Confidence: " << airflow_engine.get_prediction_confidence() << std::endl;
    
    auto warnings = airflow_engine.get_system_warnings();
    if (!warnings.empty()) {
        std::cout << "System Warnings (" << warnings.size() << "):" << std::endl;
        for (const auto& warning : warnings) {
            std::cout << "  " << warning << std::endl;
        }
    }
    
    std::cout << "\nPredictive Airflow Engine Demo completed successfully." << std::endl;
    std::cout << "Demonstrated real-time CFD analysis, vortex prediction, and stall detection." << std::endl;
    
    // Shutdown system
    airflow_engine.shutdown();
    
    return 0;
}
