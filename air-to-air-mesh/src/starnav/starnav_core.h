#ifndef STARNAV_CORE_H
#define STARNAV_CORE_H

#include <cstdint>
#include <vector>
#include <chrono>
#include <memory>
#include <string>
#include <map>

namespace starnav {

    // Forward declarations
    class StarCamera;
    class ConstellationRecognizer;
    class PositionCalculator;
    class MotionCompensator;

    // Data structures for stellar navigation
    struct StarObservation {
        uint32_t star_id;                  // Unique star identifier
        double ra;                         // Right ascension in degrees
        double dec;                        // Declination in degrees
        double magnitude;                  // Apparent magnitude
        double altitude;                   // Altitude above horizon in degrees
        double azimuth;                    // Azimuth in degrees
        std::chrono::system_clock::time_point observation_time;
    };

    struct ConstellationPattern {
        std::string name;                  // Constellation name
        std::vector<uint32_t> star_ids;    // Star IDs in the constellation
        double confidence;                 // Pattern matching confidence (0.0 to 1.0)
    };

    struct AircraftMotion {
        double roll;                       // Roll angle in degrees
        double pitch;                      // Pitch angle in degrees
        double yaw;                        // Yaw angle in degrees
        double velocity_north;             // North velocity in m/s
        double velocity_east;               // East velocity in m/s
        double velocity_down;              // Down velocity in m/s
        std::chrono::system_clock::time_point timestamp;
    };

    struct PositionFix {
        double latitude;                   // Latitude in degrees
        double longitude;                  // Longitude in degrees
        double altitude;                   // Altitude in meters
        double accuracy;                   // Position accuracy in meters
        std::chrono::system_clock::time_point fix_time;
        bool is_valid;                     // Whether the fix is valid
    };

    struct StarCatalogEntry {
        uint32_t star_id;                  // Unique star identifier
        std::string name;                  // Star name
        double ra;                         // Right ascension in degrees
        double dec;                        // Declination in degrees
        double magnitude;                   // Apparent magnitude
        double spectral_type;              // Spectral type (numeric representation)
    };

    struct NavigationDiagnostics {
        uint64_t total_observations;        // Total star observations
        uint64_t successful_fixes;          // Successful position fixes
        double fix_accuracy;               // Average fix accuracy in meters
        double recognition_rate;           // Constellation recognition rate
        std::chrono::system_clock::time_point last_fix;
        std::chrono::system_clock::time_point last_observation;
    };

    // Main StarNav Core Class
    class StarNavCore {
    public:
        StarNavCore();
        ~StarNavCore();

        // System initialization
        bool initialize();
        void enable_system(bool enable);
        
        // Core functionality
        void process_star_observations(const std::vector<StarObservation>& observations);
        ConstellationPattern recognize_constellation(const std::vector<StarObservation>& observations);
        PositionFix calculate_position(const ConstellationPattern& pattern, const AircraftMotion& motion);
        void update_aircraft_motion(const AircraftMotion& motion);
        void compensate_for_motion();
        
        // System status and diagnostics
        NavigationDiagnostics get_system_diagnostics() const;
        PositionFix get_current_position() const;
        double get_fix_accuracy() const;
        uint64_t get_successful_fixes() const;
        std::chrono::system_clock::time_point get_last_fix() const;
        std::chrono::system_clock::time_point get_last_observation() const;
        
        // Configuration methods
        void set_sensitivity_threshold(double threshold);
        void set_minimum_magnitude(double magnitude);
        void set_max_fix_interval(double interval_seconds);
        
    private:
        // Private helper methods
        void update_star_catalog();
        void validate_position_fix(PositionFix& fix);
        void update_performance_metrics();
        void log_navigation_event(const std::string& event_description);
        void handle_insufficient_stars();
        
        // System state variables
        bool initialized_;
        bool active_;
        double sensitivity_threshold_;
        double minimum_magnitude_;
        double max_fix_interval_;
        
        // Navigation state
        PositionFix current_position_;
        AircraftMotion current_motion_;
        std::vector<StarObservation> recent_observations_;
        ConstellationPattern last_recognized_pattern_;
        
        // Performance metrics
        uint64_t total_observations_;
        uint64_t successful_fixes_;
        double fix_accuracy_;
        double recognition_rate_;
        std::chrono::system_clock::time_point last_fix_;
        std::chrono::system_clock::time_point last_observation_;
        
        // Core components
        std::unique_ptr<StarCamera> star_camera_;
        std::unique_ptr<ConstellationRecognizer> constellation_recognizer_;
        std::unique_ptr<PositionCalculator> position_calculator_;
        std::unique_ptr<MotionCompensator> motion_compensator_;
        
        // Star catalog
        std::map<uint32_t, StarCatalogEntry> star_catalog_;
    };

    // Star Camera Component
    class StarCamera {
    public:
        StarCamera();
        ~StarCamera();
        
        std::vector<StarObservation> capture_stars();
        bool calibrate_camera(double focal_length, double pixel_size);
        void set_exposure_parameters(double exposure_time, double gain);
        double get_exposure_time() const;
        double get_gain() const;
        
    private:
        double focal_length_;
        double pixel_size_;
        double exposure_time_;
        double gain_;
    };

    // Constellation Recognizer Component
    class ConstellationRecognizer {
    public:
        ConstellationRecognizer();
        ~ConstellationRecognizer();
        
        ConstellationPattern recognize_pattern(const std::vector<StarObservation>& observations);
        double calculate_pattern_confidence(const std::vector<StarObservation>& observations, 
                                          const std::vector<uint32_t>& pattern);
        void load_constellation_patterns();
        void set_recognition_parameters(double threshold, double tolerance);
        double get_recognition_threshold() const;
        double get_tolerance() const;
        
    private:
        double recognition_threshold_;
        double tolerance_;
        std::map<std::string, std::vector<uint32_t>> constellation_patterns_;
    };

    // Position Calculator Component
    class PositionCalculator {
    public:
        PositionCalculator();
        ~PositionCalculator();
        
        PositionFix calculate_position(const ConstellationPattern& pattern, const AircraftMotion& motion);
        double calculate_accuracy(const PositionFix& fix);
        bool validate_fix(const PositionFix& fix);
        void set_calculation_parameters(double min_stars, double max_error);
        double get_min_stars() const;
        double get_max_error() const;
        
    private:
        double min_stars_;
        double max_error_;
    };

    // Motion Compensator Component
    class MotionCompensator {
    public:
        MotionCompensator();
        ~MotionCompensator();
        
        std::vector<StarObservation> compensate_for_motion(const std::vector<StarObservation>& observations,
                                                       const AircraftMotion& motion);
        StarObservation compensate_single_observation(const StarObservation& observation,
                                                     const AircraftMotion& motion);
        void set_compensation_parameters(double prediction_horizon, double accuracy_weight);
        double get_prediction_horizon() const;
        double get_accuracy_weight() const;
        
    private:
        double prediction_horizon_;
        double accuracy_weight_;
    };

} // namespace starnav

#endif // STARNAV_CORE_H