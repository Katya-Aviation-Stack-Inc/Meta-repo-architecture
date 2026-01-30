#include "lgfn_core.h"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace lgfn {

// GravityNavigationCore Implementation
GravityNavigationCore::GravityNavigationCore()
    : current_mode_(NavigationMode::GPS_PRIMARY)
    , gravity_field_locked_(false)
    , system_healthy_(true)
    , navigation_accuracy_(10.0)
    , last_update_time_(std::chrono::system_clock::now()) {
    
    gravimeter_ = std::make_unique<CompactGravimeter>(0.1); // 0.1 mGal sensitivity
    dsp_processor_ = std::make_unique<DSPGravityProcessor>();
    field_mapper_ = std::make_unique<GravityFieldMapper>();
}

bool GravityNavigationCore::initialize() {
    std::cout << "Gravity Navigation Core initializing..." << std::endl;
    
    if (!gravimeter_->initialize()) {
        std::cerr << "Failed to initialize gravimeter" << std::endl;
        return false;
    }
    
    if (!dsp_processor_->initialize(gravimeter_->get_sampling_rate())) {
        std::cerr << "Failed to initialize DSP processor" << std::endl;
        return false;
    }
    
    if (!field_mapper_->initialize(10.0)) { // 10 km region
        std::cerr << "Failed to initialize gravity field mapper" << std::endl;
        return false;
    }
    
    std::cout << "Gravity Navigation Core initialized successfully" << std::endl;
    return true;
}

PositionEstimate GravityNavigationCore::update_navigation(
    const std::vector<GravimetricReading>& gravity_readings,
    const PositionEstimate& gps_estimate,
    const PositionEstimate& ins_estimate) {
    
    auto start_time = std::chrono::high_resolution_clock::now();
    
    PositionEstimate result = gps_estimate; // Default to GPS
    
    // Process gravity readings
    if (!gravity_readings.empty()) {
        auto processed_signal = dsp_processor_->process_signal(gravity_readings);
        update_gravity_model(gravity_readings);
        
        if (gravity_field_locked_) {
            auto gravity_position = estimate_position_from_gravity(gravity_readings.back());
            
            // Fuse sensor data based on mode
            switch (current_mode_) {
                case NavigationMode::GRAVITY_PRIMARY:
                    result = gravity_position;
                    break;
                case NavigationMode::HYBRID_MODE:
                    fuse_sensor_data(gps_estimate, ins_estimate, gravity_position);
                    result = last_position_estimate_;
                    break;
                case NavigationMode::EMERGENCY_MODE:
                    if (gravity_field_locked_) {
                        result = gravity_position;
                    } else {
                        result = ins_estimate; // Fall back to INS
                    }
                    break;
                default:
                    break;
            }
        }
    }
    
    // Validate navigation solution
    validate_navigation_solution(result);
    
    // Update system health
    monitor_system_health();
    
    // Check update time
    auto end_time = std::chrono::high_resolution_clock::now();
    auto update_time = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    
    if (update_time > target_update_period_) {
        system_warnings_.push_back("Navigation update exceeded target: " + std::to_string(update_time.count()) + "μs");
    }
    
    last_update_time_ = std::chrono::system_clock::now();
    last_position_estimate_ = result;
    
    return result;
}

void GravityNavigationCore::update_gravity_model(const std::vector<GravimetricReading>& readings) {
    // Add readings to history
    gravity_history_.insert(gravity_history_.end(), readings.begin(), readings.end());
    
    // Keep history manageable
    if (gravity_history_.size() > 10000) {
        gravity_history_.erase(gravity_history_.begin(), gravity_history_.begin() + 5000);
    }
    
    // Update gravity map if we have enough readings
    if (gravity_history_.size() > 100) {
        current_gravity_map_ = field_mapper_->create_local_map(gravity_history_);
        gravity_field_locked_ = dsp_processor_->is_signal_locked();
    }
}

PositionEstimate GravityNavigationCore::estimate_position_from_gravity(const GravimetricReading& reading) {
    PositionEstimate estimate{};
    
    if (!gravity_field_locked_ || current_gravity_map_.gravity_field.empty()) {
        return estimate;
    }
    
    // Find best match in gravity map
    double min_difference = std::numeric_limits<double>::max();
    int best_x = 0, best_y = 0;
    
    for (int i = 0; i < current_gravity_map_.grid_size_x; ++i) {
        for (int j = 0; j < current_gravity_map_.grid_size_y; ++j) {
            double difference = std::abs(current_gravity_map_.gravity_field[i][j] - reading.gravity_magnitude);
            
            if (difference < min_difference) {
                min_difference = difference;
                best_x = i;
                best_y = j;
            }
        }
    }
    
    // Convert grid coordinates to lat/lon
    estimate.latitude = current_gravity_map_.origin_latitude + 
                      (best_y * current_gravity_map_.grid_spacing) / 111000.0;
    estimate.longitude = current_gravity_map_.origin_longitude + 
                       (best_x * current_gravity_map_.grid_spacing) / 111000.0;
    estimate.altitude = reading.altitude;
    
    // Calculate accuracy based on match quality
    estimate.position_accuracy = min_difference * 1000000.0; // Convert to meters
    estimate.velocity_accuracy = 5.0; // Gravity doesn't provide velocity
    
    estimate.timestamp = reading.timestamp;
    estimate.satellites_used = 0;
    estimate.gravity_lock = true;
    
    return estimate;
}

void GravityNavigationCore::fuse_sensor_data(const PositionEstimate& gps, const PositionEstimate& ins, PositionEstimate& gravity) {
    // Kalman filter-like fusion (simplified)
    
    // Calculate weights based on accuracy
    double gps_weight = 1.0 / (gps.position_accuracy * gps.position_accuracy);
    double ins_weight = 1.0 / (ins.position_accuracy * ins.position_accuracy);
    double gravity_weight = 1.0 / (gravity.position_accuracy * gravity.position_accuracy);
    
    double total_weight = gps_weight + ins_weight + gravity_weight;
    
    // Weighted average for position
    last_position_estimate_.latitude = (gps.latitude * gps_weight + 
                                      ins.latitude * ins_weight + 
                                      gravity.latitude * gravity_weight) / total_weight;
    
    last_position_estimate_.longitude = (gps.longitude * gps_weight + 
                                       ins.longitude * ins_weight + 
                                       gravity.longitude * gravity_weight) / total_weight;
    
    last_position_estimate_.altitude = (gps.altitude * gps_weight + 
                                       ins.altitude * ins_weight + 
                                       gravity.altitude * gravity_weight) / total_weight;
    
    // Velocity from INS (gravity doesn't provide velocity)
    last_position_estimate_.velocity_north = ins.velocity_north;
    last_position_estimate_.velocity_east = ins.velocity_east;
    last_position_estimate_.velocity_down = ins.velocity_down;
    
    // Combined accuracy
    last_position_estimate_.position_accuracy = std::sqrt(1.0 / total_weight);
    last_position_estimate_.velocity_accuracy = ins.velocity_accuracy;
    
    last_position_estimate_.timestamp = std::chrono::system_clock::now();
    last_position_estimate_.satellites_used = gps.satellites_used;
    last_position_estimate_.gravity_lock = gravity.gravity_lock;
}

void GravityNavigationCore::validate_navigation_solution(PositionEstimate& estimate) {
    // Check for reasonable values
    if (std::abs(estimate.latitude) > 90.0) {
        estimate.latitude = std::clamp(estimate.latitude, -90.0, 90.0);
        system_warnings_.push_back("Latitude out of range, clamped");
    }
    
    if (std::abs(estimate.longitude) > 180.0) {
        estimate.longitude = std::clamp(estimate.longitude, -180.0, 180.0);
        system_warnings_.push_back("Longitude out of range, clamped");
    }
    
    if (estimate.altitude < -1000.0 || estimate.altitude > 50000.0) {
        estimate.altitude = std::clamp(estimate.altitude, 0.0, 20000.0);
        system_warnings_.push_back("Altitude out of range, clamped");
    }
    
    // Update navigation accuracy
    navigation_accuracy_ = estimate.position_accuracy;
}

bool GravityNavigationCore::build_local_gravity_map(int duration_seconds) {
    std::cout << "Building local gravity map for " << duration_seconds << " seconds..." << std::endl;
    
    auto start_time = std::chrono::system_clock::now();
    int readings_needed = duration_seconds * static_cast<int>(gravimeter_->get_sampling_rate());
    
    gravity_history_.clear();
    
    while (std::chrono::duration_cast<std::chrono::seconds>(
           std::chrono::system_clock::now() - start_time).count() < duration_seconds) {
        
        auto reading = gravimeter_->take_reading();
        gravity_history_.push_back(reading);
        
        if (gravity_history_.size() % 100 == 0) {
            std::cout << "Collected " << gravity_history_.size() << " readings..." << std::endl;
        }
        
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    // Create the map
    current_gravity_map_ = field_mapper_->create_local_map(gravity_history_);
    
    std::cout << "Gravity map building completed with " << gravity_history_.size() << " readings" << std::endl;
    return true;
}

bool GravityNavigationCore::lock_gravity_field() {
    if (current_gravity_map_.gravity_field.empty()) {
        std::cout << "No gravity map available for locking" << std::endl;
        return false;
    }
    
    // Process recent readings to establish lock
    std::vector<GravimetricReading> recent_readings(
        gravity_history_.end() - std::min(100, static_cast<int>(gravity_history_.size())),
        gravity_history_.end()
    );
    
    auto processed_signal = dsp_processor_->process_signal(recent_readings);
    
    gravity_field_locked_ = dsp_processor_->is_signal_locked();
    
    std::cout << "Gravity field lock " << (gravity_field_locked_ ? "ACHIEVED" : "FAILED") << std::endl;
    return gravity_field_locked_;
}

void GravityNavigationCore::monitor_system_health() {
    system_healthy_ = true;
    
    // Check gravimeter health
    if (!gravimeter_->is_stable()) {
        system_healthy_ = false;
        system_warnings_.push_back("Gravimeter instability detected");
    }
    
    // Check DSP processor lock
    if (!dsp_processor_->is_signal_locked() && current_mode_ != NavigationMode::GPS_PRIMARY) {
        system_warnings_.push_back("Gravity signal lock lost");
    }
    
    // Check navigation accuracy
    if (navigation_accuracy_ > 100.0) { // 100 meter threshold
        system_healthy_ = false;
        system_warnings_.push_back("Navigation accuracy degraded");
    }
    
    // Clear old warnings
    if (system_warnings_.size() > 20) {
        system_warnings_.erase(system_warnings_.begin(), system_warnings_.begin() + 10);
    }
}

// LocalGravityFieldNavigation Implementation
LocalGravityFieldNavigation::LocalGravityFieldNavigation()
    : mapping_active_(false)
    , mapping_progress_(0.0)
    , cruise_speed_(100.0) // m/s default
    , anomaly_detection_enabled_(false)
    , system_initialized_(false) {
    
    navigation_core_ = std::make_unique<GravityNavigationCore>();
}

bool LocalGravityFieldNavigation::initialize() {
    std::cout << "Local Gravity Field Navigation initializing..." << std::endl;
    
    if (!navigation_core_->initialize()) {
        std::cerr << "Failed to initialize navigation core" << std::endl;
        return false;
    }
    
    // Set default aircraft parameters
    aircraft_type_ = "generic";
    environment_type_ = "standard";
    
    system_initialized_ = true;
    initialization_time_ = std::chrono::system_clock::now();
    
    std::cout << "Local Gravity Field Navigation initialized successfully" << std::endl;
    return true;
}

void LocalGravityFieldNavigation::shutdown() {
    std::cout << "Shutting down Local Gravity Field Navigation..." << std::endl;
    
    if (navigation_core_) {
        navigation_core_->set_navigation_mode(NavigationMode::GPS_PRIMARY);
    }
    
    system_initialized_ = false;
    std::cout << "Local Gravity Field Navigation shutdown complete" << std::endl;
}

PositionEstimate LocalGravityFieldNavigation::process_navigation_update(
    const PositionEstimate& gps_position,
    const PositionEstimate& ins_position,
    double delta_time_seconds) {
    
    if (!system_initialized_) {
        return gps_position;
    }
    
    // Store sensor positions
    gps_position_ = gps_position;
    ins_position_ = ins_position;
    
    // Simulate gravity readings (would come from actual gravimeter)
    std::vector<GravimetricReading> gravity_readings;
    if (mapping_active_ || navigation_core_->get_navigation_mode() != NavigationMode::GPS_PRIMARY) {
        // Generate simulated gravity reading
        CompactGravimeter temp_gravimeter(0.1);
        temp_gravimeter.initialize();
        
        for (int i = 0; i < 10; ++i) {
            auto reading = temp_gravimeter.take_reading();
            reading.latitude = current_position_.latitude;
            reading.longitude = current_position_.longitude;
            reading.altitude = current_position_.altitude;
            gravity_readings.push_back(reading);
        }
    }
    
    // Process through navigation core
    current_position_ = navigation_core_->update_navigation(
        gravity_readings, gps_position, ins_position
    );
    
    // Update mapping progress
    if (mapping_active_) {
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
            std::chrono::system_clock::now() - mapping_start_time_);
        
        int total_seconds = 300; // 5 minutes default
        mapping_progress_ = std::min(1.0, static_cast<double>(elapsed.count()) / total_seconds);
        
        if (mapping_progress_ >= 1.0) {
            mapping_active_ = false;
            std::cout << "Gravity field mapping completed" << std::endl;
        }
    }
    
    // Compensate for aircraft dynamics
    compensate_for_aircraft_dynamics();
    
    return current_position_;
}

void LocalGravityFieldNavigation::compensate_for_aircraft_dynamics() {
    // Apply aircraft-specific corrections
    
    if (aircraft_type_ == "helicopter") {
        // Helicopter-specific vibrations and dynamics
        current_position_.position_accuracy *= 1.2; // Reduced accuracy due to vibrations
    } else if (aircraft_type_ == "jet") {
        // Jet aircraft dynamics
        double speed_factor = cruise_speed_ / 100.0;
        current_position_.position_accuracy *= (1.0 + speed_factor * 0.1);
    }
    
    // Environmental compensation
    if (environment_type_ == "urban") {
        current_position_.position_accuracy *= 1.5; // Urban canyon effects
    } else if (environment_type_ == "mountainous") {
        current_position_.position_accuracy *= 1.3; // Mountain terrain effects
    }
}

bool LocalGravityFieldNavigation::start_mapping_session(int duration_minutes) {
    if (mapping_active_) {
        std::cout << "Mapping session already active" << std::endl;
        return false;
    }
    
    std::cout << "Starting gravity field mapping session for " << duration_minutes << " minutes" << std::endl;
    
    mapping_active_ = true;
    mapping_progress_ = 0.0;
    mapping_start_time_ = std::chrono::system_clock::now();
    
    return true;
}

bool LocalGravityFieldNavigation::is_navigation_available() const {
    return system_initialized_ && navigation_core_->is_system_healthy();
}

double LocalGravityFieldNavigation::get_position_accuracy() const {
    return current_position_.position_accuracy;
}

bool LocalGravityFieldNavigation::has_gravity_lock() const {
    return navigation_core_->is_gravity_locked();
}

void LocalGravityFieldNavigation::enable_gravity_primary_mode() {
    navigation_core_->set_navigation_mode(NavigationMode::GRAVITY_PRIMARY);
    std::cout << "Gravity primary navigation mode enabled" << std::endl;
}

void LocalGravityFieldNavigation::enable_hybrid_mode() {
    navigation_core_->set_navigation_mode(NavigationMode::HYBRID_MODE);
    std::cout << "Hybrid navigation mode enabled" << std::endl;
}

void LocalGravityFieldNavigation::enable_emergency_mode() {
    navigation_core_->set_navigation_mode(NavigationMode::EMERGENCY_MODE);
    std::cout << "Emergency navigation mode enabled" << std::endl;
}

void LocalGravityFieldNavigation::set_aircraft_parameters(const std::string& aircraft_type, double cruise_speed) {
    aircraft_type_ = aircraft_type;
    cruise_speed_ = cruise_speed;
    std::cout << "Aircraft parameters set: " << aircraft_type << ", cruise speed " << cruise_speed << " m/s" << std::endl;
}

void LocalGravityFieldNavigation::configure_for_environment(const std::string& environment_type) {
    environment_type_ = environment_type;
    std::cout << "Environment configuration set to: " << environment_type << std::endl;
}

} // namespace lgfn
