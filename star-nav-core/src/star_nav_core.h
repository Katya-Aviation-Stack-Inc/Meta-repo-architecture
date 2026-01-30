#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>
#include <array>
#include <complex>
#include <string>

#ifdef __cplusplus
extern "C" {
#endif

namespace star_nav {

struct StarPosition {
    double right_ascension;        // degrees
    declination;              // degrees
    double azimuth;               // degrees
    double rotation;              // degrees
    double roll;                  // degrees
    double pitch;                  // degrees
    double yaw;                   // degrees
    std::chrono::system_clock::time_point timestamp;
    uint32_t star_id;
    double confidence_level;          // 0.0 to 1.0
};

struct StarMeasurement {
    double magnitude;              // magnitude
    double azimuth;                 // degrees
    elevation;               // degrees
    double signal_to_noise;         // SNR
    std::vector<double> spectrum;       // Power spectrum
    std::chrono::system_clock::time_point timestamp;
    uint32_t sensor_id;
};

struct ConstellationData {
    std::string name;              // Constellation name
    std::vector<uint32_t> star_ids;     // Star IDs in constellation
    std::vector<std::array<double, 3>> star_positions; // RA/Dec/Alt
    std::vector<double> magnitudes;        // Star magnitudes
    std::vector<double> right_ascensions;   // Right ascensions
    std::vector<double> declinations;       // Declinations
    double total_magnitude;           // Total magnitude
    std::chrono::system_clock::time_point catalog_epoch;
};

struct NavigationState {
    bool gps_available;             // GPS is available
    bool ins_available;             // INS is available
    bool star_lock;                 // Star lock achieved
    double position_accuracy;          // meters
    double velocity_accuracy;          // m/s
    double heading_accuracy;          // degrees
    std::chrono::system_clock::time_point last_update;
    uint32_t satellites_visible;       // GPS satellites
    uint32_t stars_visible;          // Stars detected
    double position_error;             // meters
    double velocity_error;             // m/s
};

struct CameraParameters {
    double fov_h;                  // Horizontal field of view (degrees)
    double fov_v;                  // Vertical field of view (degrees)
    double image_width;              // pixels
    double image_height;             // pixels
    double pixel_size;               // arcseconds per pixel
    double exposure_time;            // seconds
    double gain;                   // dB
    double noise_level;              // electrons
    std::chrono::system_clock::time_point calibration_time;
};

enum class StarDatabase {
public:
    StarDatabase();
    ~StarDatabase() = default;
    
    bool initialize(const std::string& catalog_path);
    bool load_catalog(const std::string& catalog_path);
    
    std::vector<ConstellationData> get_constellations() const;
    ConstellationData get_constellation(const std::string& name) const;
    bool is_constellation_visible(const std::string& name, double min_altitude) const;
    
    std::vector<uint32_t> find_stars_in_region(
        double min_altitude, double max_altitude,
        double min_azimuth, double max_azimuth
    );
    
    bool update_star_position(uint32_t star_id, const StarPosition& position);
    StarPosition get_star_position(uint32_t star_id) const;
    
    void set_location_estimate(const AircraftState& state);
    void set_camera_parameters(const CameraParameters& params);
    
    bool calibrate_camera();
    bool is_calibrated() const;

private:
    void load_star_catalog(const std::string& catalog_path);
    void calculate_precession();
    void calculate_nutation();
    
    std::vector<ConstellationData> constellations_;
    std::unordered_map<std::string, ConstellationData> constellation_map_;
    double local_julian_date_;
    double local_sidereal_time_;
    double local_greenwich_mean_sidereal_;
    
    CameraParameters camera_params_;
    bool camera_calibrated_;
    NavigationState nav_state_;
};

enum class StarDetector {
public:
    StarDetector();
    ~StarDetector() = default;
    
    bool initialize(const CameraParameters& params, const StarDatabase& database);
    std::vector<StarMeasurement> detect_stars(const std::vector<uint8_t>& image_data);
    
    std::vector<StarPosition> calculate_star_positions(const std::vector<StarMeasurement>& measurements);
    std::vector<uint32_t> identify_constellations(const std::vector<StarPosition>& positions);
    
    void set_detection_sensitivity(double sensitivity); // 0-1
    void set_minimum_magnitude(double magnitude); // 0.0
    void set_confidence_threshold(double threshold); // 0.0-1.0
    
    bool is_star_lock_achieved() const;
    double get_lock_confidence() const;
    std::vector<std::string> get_detected_constellations() const;

private:
    void process_image_data(const std::vector<uint8_t>& image_data);
    void apply_noise_reduction(std::vector<double>& spectrum);
    void calculate_centroid();
    void track_stars(const std::vector<StarPosition>& positions);
    
    CameraParameters camera_params_;
    StarDatabase* database_;
    double detection_sensitivity_;
    double minimum_magnitude_;
    double confidence_threshold_;
    
    std::vector<std::vector<double>> image_history_;
    std::vector<StarPosition> tracked_stars_;
    bool star_lock_achieved_;
    double lock_confidence_;
};

enum class AttitudeEstimator {
public:
    AttitudeEstimator();
    ~AttitudeEstimator() = default;
    
    bool initialize();
    double estimate_altitude(const StarPosition& position, const NavigationState& nav_state);
    double estimate_heading(const StarPosition& position, const NavigationState& nav_state);
    double estimate_ground_track(const std::vector<StarPosition>& positions);
    
    void set_sensor_fusion_weights(double gps_weight, double ins_weight, double star_weight);
    void set_filter_coefficients(double alpha, double beta, double gamma);
    
    double get_altitude_accuracy() const;
    double heading_accuracy() const;
    double position_accuracy() const;

private:
    void kalman_filter_update(const StarPosition& measurement, const NavigationState& nav_state);
    void complementary_filter_update(const StarPosition& measurement, const NavigationState& nav_state);
    void sensor_fusion_update(const NavigationState& nav_state);
    
    double gps_weight_;
    double ins_weight_;
    double star_weight_;
    double alpha_, beta_, gamma_;
    
    std::vector<StarPosition> position_history_;
    double altitude_estimate_;
    double heading_estimate_;
    double ground_track_estimate_;
    
    double altitude_variance_;
    double heading_variance_;
    double position_variance_;
};

class StarNavCore {
public:
    StarNavCore();
    ~StarNavCore() = default;
    
    bool initialize();
    void shutdown();
    
    // Main navigation loop
    NavigationState update_navigation(
        const AircraftState& aircraft_state,
        const std::vector<uint8_t>& image_data
    );
    
    // Camera control
    void set_camera_parameters(const CameraParameters& params);
    bool calibrate_camera();
    bool is_calibrated() const;
    
    // Navigation output
    NavigationState get_navigation_state() const;
    StarPosition get_current_position() const;
    double get_position_accuracy() const;
    
    // Configuration
    void set_minimum_visibility(double min_magnitude);
    void set_confidence_threshold(double threshold);
    void enable_emergency_mode(bool enable);
    
    // Monitoring
    bool is_navigation_available() const;
    std::vector<std::string> get_system_status() const;

private:
    void update_navigation_state(const AircraftState& aircraft_state);
    void validate_navigation_solution();
    void log_performance_data();
    void handle_sensor_failures();
    
    std::unique_ptr<StarDatabase> star_database_;
    std::star_database_ = std::make_unique<StarDatabase>();
    std::unique_ptr<StarDetector> star_detector_;
    std::unique_ptr<AttitudeEstimator> altitude_estimator_;
    
    NavigationState current_nav_state_;
    StarPosition current_position_;
    CameraParameters camera_params_;
    
    bool emergency_mode_;
    double minimum_visibility_;
    double confidence_threshold_;
    bool system_healthy_;
    std::vector<std::string> system_status_;
    
    std::chrono::system_clock::time_point last_update_time_;
    std::chrono::microseconds target_update_period_{50000}; // 500ms target
};

} // namespace star_nav
