#ifndef GRAVITY_NAVIGATION_H
#define GRAVITY_NAVIGATION_H

#include <cstdint>
#include <vector>
#include <memory>
#include <chrono>

namespace gravity_nav {

    // Core data structures
    struct GravityMeasurement {
        double gravity_x;            // m/s²
        double gravity_y;           // m/s²
        double gravity_z;          // m/s²
        double magnitude;           // m/s²
        std::chrono::system_clock::time_point timestamp;
    };

    struct Position {
        double latitude;             // degrees
        double longitude;           // degrees
        double altitude;            // meters
        std::chrono::system_clock::time_point timestamp;
    };

    struct GravityAnomaly {
        double latitude;            // degrees
        double longitude;           // degrees
        double anomaly_value;       // mGal (10^-5 m/s²)
        double confidence;          // 0.0 to 1.0
        std::chrono::system_clock::time_point last_updated;
    };

    struct InertialData {
        double accel_x;              // m/s²
        double accel_y;              // m/s²
        double accel_z;              // m/s²
        double gyro_x;              // rad/s
        double gyro_y;              // rad/s
        double gyro_z;              // rad/s
        std::chrono::system_clock::time_point timestamp;
    };

    // Main Gravity Navigation class
    class GravityNavigation {
    private:
        // Core components
        std::unique_ptr<class GravitySensor> gravity_sensor_;
        std::unique_ptr<class AnomalyMapper> anomaly_mapper_;
        std::unique_ptr<class PositionEstimator> position_estimator_;
        std::unique_ptr<class FilterEngine> filter_engine_;
        
        // System state
        bool initialized_;
        bool active_;
        Position current_position_;
        GravityMeasurement current_gravity_;
        std::vector<GravityAnomaly> gravity_map_;
        
        // Performance metrics
        double position_accuracy_;   // meters
        double update_rate_;        // Hz
        std::vector<double> accuracy_history_;
        
    public:
        GravityNavigation();
        ~GravityNavigation();
        
        // Initialization
        bool initialize();
        void enable_system(bool enable);
        
        // Core functionality
        void update_gravity_measurement(const GravityMeasurement& measurement);
        Position calculate_position(const InertialData& inertial_data);
        
        // Gravity mapping
        void update_gravity_map(const GravityAnomaly& anomaly);
        std::vector<GravityAnomaly> get_local_gravity_map() const;
        
        // Calibration
        void calibrate_sensor();
        
        // Position correction
        void correct_position(const Position& reference_position);
        
        // Performance monitoring
        double get_position_accuracy() const;
        double get_update_rate() const;
        Position get_current_position() const;
        
    private:
        void update_performance_metrics();
        void handle_sensor_error();
    };

    // Gravity Sensor Interface
    class GravitySensor {
    private:
        bool calibrated_;
        double calibration_offset_x_;
        double calibration_offset_y_;
        double calibration_offset_z_;
        
    public:
        GravitySensor();
        ~GravitySensor();
        
        GravityMeasurement read_gravity();
        void calibrate();
        bool is_calibrated() const;
        
        // Sensor simulation for demonstration
        void simulate_gravity_field(double latitude, double longitude, double altitude);
    };

    // Anomaly Mapper
    class AnomalyMapper {
    private:
        std::vector<GravityAnomaly> local_map_;
        double map_resolution_;    // degrees
        double confidence_threshold_;
        
    public:
        AnomalyMapper();
        ~AnomalyMapper();
        
        void add_measurement(const GravityMeasurement& measurement, 
                          const Position& position);
        std::vector<GravityAnomaly> generate_local_map(const Position& current_position,
                                                       double radius_km);
        void update_anomaly(const GravityAnomaly& anomaly);
        GravityAnomaly find_matching_anomaly(const Position& position) const;
    };

    // Position Estimator
    class PositionEstimator {
    private:
        struct EstimationParameters {
            double gravity_sensitivity;     // m/s² per meter
            double integration_time;         // seconds
            double error_correction_factor;   // dimensionless
        };
        
        EstimationParameters params_;
        Position last_known_position_;
        std::unique_ptr<class IntegrationEngine> integrator_;
        
    public:
        PositionEstimator();
        ~PositionEstimator();
        
        Position estimate_position(const GravityMeasurement& gravity_measurement,
                                 const InertialData& inertial_data,
                                 const std::vector<GravityAnomaly>& gravity_map);
        
        void set_last_known_position(const Position& position);
        Position get_last_known_position() const;
        
        void set_estimation_parameters(const EstimationParameters& params);
        EstimationParameters get_estimation_parameters() const;
    };

    // Filter Engine
    class FilterEngine {
    private:
        struct FilterParameters {
            double kalman_gain;        // dimensionless
            double process_noise;       // m²/s⁴
            double measurement_noise;  // (m/s²)²
        };
        
        FilterParameters params_;
        std::vector<Position> position_history_;
        std::vector<GravityMeasurement> gravity_history_;
        
    public:
        FilterEngine();
        ~FilterEngine();
        
        Position filter_position(const Position& raw_position,
                               const GravityMeasurement& gravity_measurement);
        
        void update_filter_parameters(const FilterParameters& params);
        FilterParameters get_filter_parameters() const;
        
        void add_position_to_history(const Position& position);
        void add_gravity_to_history(const GravityMeasurement& measurement);
    };

} // namespace gravity_nav

#endif // GRAVITY_NAVIGATION_H