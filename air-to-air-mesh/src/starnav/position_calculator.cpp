#include "starnav_core.h"
#include <iostream>
#include <random>
#include <cmath>

namespace starnav {

    // PositionCalculator Implementation
    PositionCalculator::PositionCalculator()
        : min_stars_(4)
        , max_error_(100.0) {
    }

    PositionCalculator::~PositionCalculator() = default;

    PositionFix PositionCalculator::calculate_position(const ConstellationPattern& pattern, const AircraftMotion& motion) {
        // Calculate position based on constellation pattern and aircraft motion
        PositionFix fix;
        fix.fix_time = std::chrono::system_clock::now();
        fix.is_valid = false;
        
        // Check if we have enough stars for a position fix
        if (pattern.star_ids.size() < min_stars_) {
            std::cout << "Not enough stars for position fix: " << pattern.star_ids.size() 
                      << " < " << min_stars_ << std::endl;
            return fix;
        }
        
        // For demonstration, we'll generate a simulated position fix
        // In a real implementation, this would use celestial navigation algorithms
        
        std::default_random_engine generator;
        std::uniform_real_distribution<double> lat_dist(-90.0, 90.0);
        std::uniform_real_distribution<double> lon_dist(-180.0, 180.0);
        std::uniform_real_distribution<double> alt_dist(0.0, 15000.0);
        std::uniform_real_distribution<double> error_dist(10.0, 100.0);
        
        fix.latitude = lat_dist(generator);
        fix.longitude = lon_dist(generator);
        fix.altitude = alt_dist(generator);
        fix.accuracy = error_dist(generator);
        fix.is_valid = true;
        
        std::cout << "Position calculated from constellation '" << pattern.name 
                  << "' with " << pattern.star_ids.size() << " stars" << std::endl;
        
        return fix;
    }

    double PositionCalculator::calculate_accuracy(const PositionFix& fix) {
        // Calculate position accuracy
        // In a real implementation, this would be based on the geometry of the observed stars
        // and the precision of the measurements
        
        // For demonstration, we'll return the fix's accuracy value
        return fix.accuracy;
    }

    bool PositionCalculator::validate_fix(const PositionFix& fix) {
        // Validate position fix
        if (!fix.is_valid) {
            return false;
        }
        
        // Check if accuracy is within acceptable limits
        if (fix.accuracy > max_error_) {
            std::cout << "Position fix rejected due to high error: " << fix.accuracy << "m" << std::endl;
            return false;
        }
        
        // Check if coordinates are valid
        if (std::isnan(fix.latitude) || std::isnan(fix.longitude) || std::isnan(fix.altitude)) {
            std::cout << "Position fix rejected due to invalid coordinates" << std::endl;
            return false;
        }
        
        // Check if coordinates are within valid ranges
        if (fix.latitude < -90.0 || fix.latitude > 90.0) {
            std::cout << "Position fix rejected due to invalid latitude: " << fix.latitude << std::endl;
            return false;
        }
        
        if (fix.longitude < -180.0 || fix.longitude > 180.0) {
            std::cout << "Position fix rejected due to invalid longitude: " << fix.longitude << std::endl;
            return false;
        }
        
        if (fix.altitude < -500.0 || fix.altitude > 50000.0) {
            std::cout << "Position fix rejected due to invalid altitude: " << fix.altitude << std::endl;
            return false;
        }
        
        return true;
    }

    void PositionCalculator::set_calculation_parameters(double min_stars, double max_error) {
        min_stars_ = min_stars;
        max_error_ = max_error;
    }

    double PositionCalculator::get_min_stars() const {
        return min_stars_;
    }

    double PositionCalculator::get_max_error() const {
        return max_error_;
    }

} // namespace starnav