#include "starnav_core.h"
#include <iostream>
#include <random>
#include <cmath>

namespace starnav {

    // MotionCompensator Implementation
    MotionCompensator::MotionCompensator()
        : prediction_horizon_(0.1)
        , accuracy_weight_(0.5) {
    }

    MotionCompensator::~MotionCompensator() = default;

    std::vector<StarObservation> MotionCompensator::compensate_for_motion(const std::vector<StarObservation>& observations,
                                                                         const AircraftMotion& motion) {
        // Compensate star observations for aircraft motion
        std::vector<StarObservation> compensated;
        compensated.reserve(observations.size());
        
        for (const auto& obs : observations) {
            StarObservation comp_obs = compensate_single_observation(obs, motion);
            compensated.push_back(comp_obs);
        }
        
        std::cout << "Compensated " << observations.size() << " star observations for aircraft motion" << std::endl;
        return compensated;
    }

    StarObservation MotionCompensator::compensate_single_observation(const StarObservation& observation,
                                                                   const AircraftMotion& motion) {
        // Compensate a single star observation for aircraft motion
        StarObservation compensated = observation;
        
        // For demonstration, we'll apply a simple compensation model
        // In a real implementation, this would use more sophisticated motion compensation algorithms
        
        // Apply roll compensation
        compensated.altitude += motion.roll * 0.1; // Simple proportional compensation
        
        // Apply pitch compensation
        compensated.azimuth += motion.pitch * 0.1; // Simple proportional compensation
        
        // Apply yaw compensation
        compensated.azimuth += motion.yaw * 0.05; // Simple proportional compensation
        
        // Apply velocity compensation (simplified)
        double time_factor = prediction_horizon_;
        compensated.altitude += motion.velocity_down * time_factor * 0.01;
        compensated.azimuth += (motion.velocity_north * cos(compensated.azimuth * M_PI / 180.0) +
                              motion.velocity_east * sin(compensated.azimuth * M_PI / 180.0)) * time_factor * 0.001;
        
        // Ensure values stay within valid ranges
        compensated.altitude = std::max(0.0, std::min(90.0, compensated.altitude));
        compensated.azimuth = fmod(compensated.azimuth + 360.0, 360.0);
        
        return compensated;
    }

    void MotionCompensator::set_compensation_parameters(double prediction_horizon, double accuracy_weight) {
        prediction_horizon_ = prediction_horizon;
        accuracy_weight_ = accuracy_weight;
    }

    double MotionCompensator::get_prediction_horizon() const {
        return prediction_horizon_;
    }

    double MotionCompensator::get_accuracy_weight() const {
        return accuracy_weight_;
    }

} // namespace starnav