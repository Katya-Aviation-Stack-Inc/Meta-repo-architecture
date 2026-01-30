#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>
#include <array>
#include <complex>

namespace lgfn {

struct GravimetricReading {
    double gravity_magnitude;      // m/s²
    double gravity_gradient_x;    // Gal/m (1 Gal = 0.01 m/s²)
    double gravity_gradient_y;    // Gal/m
    double gravity_gradient_z;    // Gal/m
    double latitude;              // degrees
    double longitude;             // degrees
    double altitude;              // meters
    std::chrono::system_clock::time_point timestamp;
    double signal_quality;        // 0.0 to 1.0
    double temperature;           // Celsius
};

struct PositionEstimate {
    double latitude;              // degrees
    double longitude;             // degrees
    double altitude;              // meters
    double velocity_north;        // m/s
    double velocity_east;         // m/s
    double velocity_down;         // m/s
    double position_accuracy;     // meters
    double velocity_accuracy;     // m/s
    std::chrono::system_clock::time_point timestamp;
    uint8_t satellites_used;      // count
    bool gravity_lock;            // true if gravity field locked
};

struct GravityAnomaly {
    double anomaly_strength;      // mGal (milliGal)
    double spatial_frequency;     // 1/km
    double direction_x;           // unit vector
    double direction_y;           // unit vector
    double direction_z;           // unit vector
    double confidence;            // 0.0 to 1.0
    std::string anomaly_type;     // "mass_concentration", "void", "geological"
};

struct GravityMap {
    std::vector<std::vector<double>> gravity_field; // 2D gravity field
    double grid_spacing;          // meters
    double origin_latitude;       // degrees
    double origin_longitude;      // degrees
    int grid_size_x;              // cells
    int grid_size_y;              // cells
    std::chrono::system_clock::time_point creation_time;
    double map_accuracy;          // mGal
};

enum class NavigationMode {
    GPS_PRIMARY,
    GRAVITY_PRIMARY,
    HYBRID_MODE,
    DEAD_RECKONING,
    EMERGENCY_MODE
};

class CompactGravimeter {
public:
    CompactGravimeter(double sensitivity_mgal);
    ~CompactGravimeter() = default;
    
    bool initialize();
    bool calibrate();
    GravimetricReading take_reading();
    
    void set_sampling_rate(double hz);
    double get_sampling_rate() const { return sampling_rate_; }
    
    bool is_stable() const { return sensor_stable_; }
    double get_noise_level() const { return noise_level_; }
    
    void enable_temperature_compensation(bool enable);
    void set_filter_bandwidth(double hz);
    
    bool self_test();
    double get_sensor_temperature() const { return sensor_temperature_; }

private:
    void apply_temperature_correction();
    void apply_digital_filter();
    void calculate_signal_quality();
    void monitor_sensor_health();
    
    double sensitivity_;
    double sampling_rate_;
    double noise_level_;
    bool sensor_stable_;
    bool temperature_compensation_enabled_;
    double filter_bandwidth_;
    double sensor_temperature_;
    
    std::vector<GravimetricReading> reading_history_;
    std::chrono::system_clock::time_point last_reading_time_;
};

class DSPGravityProcessor {
public:
    DSPGravityProcessor();
    ~DSPGravityProcessor() = default;
    
    bool initialize(double sample_rate);
    std::vector<double> process_signal(const std::vector<GravimetricReading>& readings);
    std::vector<double> extract_gravity_gradients(const GravimetricReading& reading);
    
    void design_filter(double cutoff_freq, double filter_order);
    void enable_noise_reduction(bool enable);
    void set_correlation_window(double seconds);
    
    double calculate_spectral_density(const std::vector<double>& signal);
    std::vector<double> detect_anomalies(const std::vector<double>& gravity_signal);
    
    bool is_signal_locked() const { return signal_locked_; }
    double get_lock_confidence() const { return lock_confidence_; }

private:
    std::vector<double> apply_bandpass_filter(const std::vector<double>& signal);
    std::vector<double> correlate_with_model(const std::vector<double>& signal);
    std::vector<double> extract_fundamental_frequency(const std::vector<double>& signal);
    
    double sample_rate_;
    bool noise_reduction_enabled_;
    double correlation_window_;
    bool signal_locked_;
    double lock_confidence_;
    
    std::vector<double> filter_coefficients_;
    std::vector<std::complex<double>> fft_buffer_;
};

class GravityFieldMapper {
public:
    GravityFieldMapper();
    ~GravityFieldMapper() = default;
    
    bool initialize(double region_size_km);
    GravityMap create_local_map(const std::vector<GravimetricReading>& readings);
    void update_map(GravityMap& map, const GravimetricReading& new_reading);
    
    std::vector<GravityAnomaly> detect_anomalies(const GravityMap& map);
    bool compare_with_database(const GravityMap& map, const std::string& database_path);
    
    void set_resolution(double meters_per_cell);
    double get_map_accuracy() const { return current_map_accuracy_; }
    
    void export_map(const GravityMap& map, const std::string& filename);
    GravityMap import_map(const std::string& filename);

private:
    void interpolate_gravity_field(GravityMap& map);
    void smooth_gravity_field(GravityMap& map);
    void calculate_map_statistics(GravityMap& map);
    void validate_map_integrity(const GravityMap& map);
    
    double region_size_;
    double resolution_;
    double current_map_accuracy_;
    std::vector<GravityMap> map_database_;
};

class GravityNavigationCore {
public:
    GravityNavigationCore();
    ~GravityNavigationCore() = default;
    
    bool initialize();
    void shutdown();
    
    // Main navigation update
    PositionEstimate update_navigation(
        const std::vector<GravimetricReading>& gravity_readings,
        const PositionEstimate& gps_estimate,
        const PositionEstimate& ins_estimate
    );
    
    // Mode control
    void set_navigation_mode(NavigationMode mode);
    NavigationMode get_navigation_mode() const { return current_mode_; }
    
    // Gravity field operations
    bool build_local_gravity_map(int duration_seconds);
    bool lock_gravity_field();
    bool is_gravity_locked() const { return gravity_field_locked_; }
    
    // Position estimation
    PositionEstimate estimate_position_from_gravity(const GravimetricReading& reading);
    double calculate_position_confidence(const PositionEstimate& estimate);
    
    // System monitoring
    bool is_system_healthy() const;
    double get_navigation_accuracy() const;
    std::vector<std::string> get_system_warnings() const;

private:
    void fuse_sensor_data(const PositionEstimate& gps, const PositionEstimate& ins, PositionEstimate& gravity);
    void update_gravity_model(const std::vector<GravimetricReading>& readings);
    void validate_navigation_solution(PositionEstimate& estimate);
    void monitor_system_health();
    
    NavigationMode current_mode_;
    std::unique_ptr<CompactGravimeter> gravimeter_;
    std::unique_ptr<DSPGravityProcessor> dsp_processor_;
    std::unique_ptr<GravityFieldMapper> field_mapper_;
    
    GravityMap current_gravity_map_;
    PositionEstimate last_position_estimate_;
    std::vector<GravimetricReading> gravity_history_;
    
    bool gravity_field_locked_;
    bool system_healthy_;
    double navigation_accuracy_;
    std::vector<std::string> system_warnings_;
    
    std::chrono::system_clock::time_point last_update_time_;
    std::chrono::microseconds target_update_period_{100000}; // 100ms target
};

class LocalGravityFieldNavigation {
public:
    LocalGravityFieldNavigation();
    ~LocalGravityFieldNavigation() = default;
    
    bool initialize();
    void shutdown();
    
    // High-level interface
    void set_current_position(const PositionEstimate& position);
    PositionEstimate get_current_position() const;
    
    // Navigation modes
    void enable_gravity_primary_mode();
    void enable_hybrid_mode();
    void enable_emergency_mode();
    
    // Real-time navigation
    PositionEstimate process_navigation_update(
        const PositionEstimate& gps_position,
        const PositionEstimate& ins_position,
        double delta_time_seconds
    );
    
    // Gravity mapping
    bool start_mapping_session(int duration_minutes);
    bool is_mapping_active() const { return mapping_active_; }
    double get_mapping_progress() const { return mapping_progress_; }
    
    // System status
    bool is_navigation_available() const;
    double get_position_accuracy() const;
    bool has_gravity_lock() const;
    
    // Advanced features
    void set_aircraft_parameters(const std::string& aircraft_type, double cruise_speed);
    void configure_for_environment(const std::string& environment_type);
    void enable_anomaly_detection(bool enable);

private:
    void update_navigation_filter();
    void compensate_for_aircraft_dynamics();
    void log_navigation_data();
    void handle_sensor_failures();
    
    std::unique_ptr<GravityNavigationCore> navigation_core_;
    
    PositionEstimate current_position_;
    PositionEstimate gps_position_;
    PositionEstimate ins_position_;
    
    bool mapping_active_;
    double mapping_progress_;
    std::chrono::system_clock::time_point mapping_start_time_;
    
    std::string aircraft_type_;
    double cruise_speed_;
    std::string environment_type_;
    bool anomaly_detection_enabled_;
    
    std::chrono::system_clock::time_point initialization_time_;
    bool system_initialized_;
};

} // namespace lgfn
