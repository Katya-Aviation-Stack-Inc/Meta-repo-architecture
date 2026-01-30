#include "lgfn_core.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <random>
#include <fstream>

namespace lgfn {

// Physical constants
constexpr double STANDARD_GRAVITY = 9.80665; // m/s²
constexpr double EARTH_RADIUS = 6371000.0; // meters
constexpr double GRAVITATIONAL_CONSTANT = 6.67430e-11; // m³/(kg·s²)

// CompactGravimeter Implementation
CompactGravimeter::CompactGravimeter(double sensitivity_mgal)
    : sensitivity_(sensitivity_mgal * 1e-5) // Convert mGal to m/s²
    , sampling_rate_(100.0) // 100 Hz default
    , noise_level_(1e-6) // 1 µg noise level
    , sensor_stable_(false)
    , temperature_compensation_enabled_(true)
    , filter_bandwidth_(10.0) // 10 Hz default
    , sensor_temperature_(25.0)
    , last_reading_time_(std::chrono::system_clock::now()) {
    
    reading_history_.reserve(1000);
}

bool CompactGravimeter::initialize() {
    std::cout << "Compact Gravimeter initializing with " << sensitivity_ * 1e6 << " µg sensitivity" << std::endl;
    
    // Simulate sensor warm-up
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    sensor_stable_ = true;
    noise_level_ = sensitivity_ * 0.1; // 10% of sensitivity as noise
    
    std::cout << "Compact Gravimeter initialized successfully" << std::endl;
    return true;
}

bool CompactGravimeter::calibrate() {
    std::cout << "Calibrating gravimeter..." << std::endl;
    
    // Simulate calibration process
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    
    // Reset reading history for calibration
    reading_history_.clear();
    
    std::cout << "Gravimeter calibration completed" << std::endl;
    return true;
}

GravimetricReading CompactGravimeter::take_reading() {
    GravimetricReading reading{};
    
    auto now = std::chrono::system_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_reading_time_);
    
    if (dt.count() > 0) {
        double dt_sec = dt.count() / 1000.0;
        
        // Simulate gravity reading with noise
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::normal_distribution<> noise(0.0, noise_level_);
        
        // Base gravity with altitude correction
        double altitude_correction = -2.0 * STANDARD_GRAVITY * 0.0 / EARTH_RADIUS; // Simplified
        reading.gravity_magnitude = STANDARD_GRAVITY + altitude_correction + noise(gen);
        
        // Simulate gravity gradients (typically 1-10 Eötvös = 1e-9 s⁻²)
        std::normal_distribution<> gradient_noise(0.0, 1e-9);
        reading.gravity_gradient_x = gradient_noise(gen) * 1e5; // Convert to Gal/m
        reading.gravity_gradient_y = gradient_noise(gen) * 1e5;
        reading.gravity_gradient_z = gradient_noise(gen) * 1e5;
        
        // Position (would come from other sensors in reality)
        reading.latitude = 0.0;
        reading.longitude = 0.0;
        reading.altitude = 1000.0;
        
        reading.timestamp = now;
        reading.temperature = sensor_temperature_;
        
        // Calculate signal quality
        calculate_signal_quality();
        reading.signal_quality = signal_locked_ ? 0.95 : 0.7;
        
        // Apply corrections
        if (temperature_compensation_enabled_) {
            apply_temperature_correction();
        }
        
        apply_digital_filter();
        
        // Store in history
        reading_history_.push_back(reading);
        if (reading_history_.size() > 1000) {
            reading_history_.erase(reading_history_.begin());
        }
        
        last_reading_time_ = now;
    }
    
    return reading;
}

void CompactGravimeter::apply_temperature_correction() {
    // Temperature coefficient for gravimeter (simplified)
    double temp_coefficient = -1e-7; // m/s² per degree Celsius
    double temp_deviation = sensor_temperature_ - 20.0; // Reference temperature 20°C
    
    // Apply temperature correction to last reading if available
    if (!reading_history_.empty()) {
        double correction = temp_coefficient * temp_deviation;
        reading_history_.back().gravity_magnitude -= correction;
    }
}

void CompactGravimeter::apply_digital_filter() {
    // Simple low-pass filter to reduce high-frequency noise
    if (reading_history_.size() >= 3) {
        auto& current = reading_history_.back();
        const auto& prev1 = reading_history_[reading_history_.size() - 2];
        const auto& prev2 = reading_history_[reading_history_.size() - 3];
        
        // 3-point moving average
        current.gravity_magnitude = (current.gravity_magnitude + prev1.gravity_magnitude + prev2.gravity_magnitude) / 3.0;
    }
}

void CompactGravimeter::calculate_signal_quality() {
    if (reading_history_.size() < 10) {
        signal_locked_ = false;
        return;
    }
    
    // Calculate signal variance
    double mean = 0.0;
    for (const auto& reading : reading_history_) {
        mean += reading.gravity_magnitude;
    }
    mean /= reading_history_.size();
    
    double variance = 0.0;
    for (const auto& reading : reading_history_) {
        variance += (reading.gravity_magnitude - mean) * (reading.gravity_magnitude - mean);
    }
    variance /= reading_history_.size();
    
    // Signal quality based on variance vs noise level
    double snr = noise_level_ * noise_level_ / variance;
    signal_locked_ = snr > 10.0; // SNR > 10 for lock
}

bool CompactGravimeter::self_test() {
    std::cout << "Running gravimeter self-test..." << std::endl;
    
    // Test sensor stability
    bool stability_test = sensor_stable_;
    
    // Test noise level
    bool noise_test = noise_level_ < sensitivity_ * 0.5;
    
    // Test temperature response
    double temp_test = std::abs(sensor_temperature_ - 25.0) < 10.0;
    
    bool test_passed = stability_test && noise_test && temp_test;
    
    std::cout << "Self-test " << (test_passed ? "PASSED" : "FAILED") << std::endl;
    return test_passed;
}

// DSPGravityProcessor Implementation
DSPGravityProcessor::DSPGravityProcessor()
    : sample_rate_(100.0)
    , noise_reduction_enabled_(true)
    , correlation_window_(10.0)
    , signal_locked_(false)
    , lock_confidence_(0.0) {
    
    fft_buffer_.resize(1024);
}

bool DSPGravityProcessor::initialize(double sample_rate) {
    sample_rate_ = sample_rate;
    
    // Design default filter
    design_filter(10.0, 4.0); // 10 Hz cutoff, 4th order
    
    std::cout << "DSP Gravity Processor initialized at " << sample_rate_ << " Hz" << std::endl;
    return true;
}

std::vector<double> DSPGravityProcessor::process_signal(const std::vector<GravimetricReading>& readings) {
    std::vector<double> gravity_signal;
    gravity_signal.reserve(readings.size());
    
    // Extract gravity magnitude from readings
    for (const auto& reading : readings) {
        gravity_signal.push_back(reading.gravity_magnitude);
    }
    
    // Apply noise reduction if enabled
    if (noise_reduction_enabled_) {
        gravity_signal = apply_bandpass_filter(gravity_signal);
    }
    
    // Detect anomalies
    auto anomalies = detect_anomalies(gravity_signal);
    
    // Update lock confidence
    if (anomalies.empty()) {
        lock_confidence_ = std::min(1.0, lock_confidence_ + 0.1);
        signal_locked_ = lock_confidence_ > 0.7;
    } else {
        lock_confidence_ = std::max(0.0, lock_confidence_ - 0.2);
        signal_locked_ = false;
    }
    
    return gravity_signal;
}

std::vector<double> DSPGravityProcessor::extract_gravity_gradients(const GravimetricReading& reading) {
    std::vector<double> gradients;
    gradients.push_back(reading.gravity_gradient_x);
    gradients.push_back(reading.gravity_gradient_y);
    gradients.push_back(reading.gravity_gradient_z);
    
    return gradients;
}

void DSPGravityProcessor::design_filter(double cutoff_freq, double filter_order) {
    // Simple Butterworth filter design (simplified)
    int n = static_cast<int>(filter_order);
    filter_coefficients_.resize(n + 1);
    
    double omega_c = 2.0 * M_PI * cutoff_freq / sample_rate_;
    
    for (int i = 0; i <= n; ++i) {
        filter_coefficients_[i] = 1.0 / (1.0 + std::pow(omega_c, n));
    }
    
    std::cout << "Filter designed: " << cutoff_freq << " Hz cutoff, " << filter_order << "th order" << std::endl;
}

std::vector<double> DSPGravityProcessor::apply_bandpass_filter(const std::vector<double>& signal) {
    std::vector<double> filtered_signal(signal.size());
    
    // Simple FIR filter implementation
    for (size_t i = filter_coefficients_.size() - 1; i < signal.size(); ++i) {
        double sum = 0.0;
        for (size_t j = 0; j < filter_coefficients_.size(); ++j) {
            sum += filter_coefficients_[j] * signal[i - j];
        }
        filtered_signal[i] = sum;
    }
    
    return filtered_signal;
}

std::vector<double> DSPGravityProcessor::detect_anomalies(const std::vector<double>& gravity_signal) {
    std::vector<double> anomalies;
    
    if (gravity_signal.size() < 10) return anomalies;
    
    // Calculate mean and standard deviation
    double mean = 0.0;
    for (double value : gravity_signal) {
        mean += value;
    }
    mean /= gravity_signal.size();
    
    double variance = 0.0;
    for (double value : gravity_signal) {
        variance += (value - mean) * (value - mean);
    }
    variance /= gravity_signal.size();
    double std_dev = std::sqrt(variance);
    
    // Detect anomalies (values > 3 sigma from mean)
    double threshold = 3.0 * std_dev;
    for (size_t i = 0; i < gravity_signal.size(); ++i) {
        if (std::abs(gravity_signal[i] - mean) > threshold) {
            anomalies.push_back(gravity_signal[i]);
        }
    }
    
    return anomalies;
}

// GravityFieldMapper Implementation
GravityFieldMapper::GravityFieldMapper()
    : region_size_(10.0) // 10 km default
    , resolution_(100.0) // 100 m default
    , current_map_accuracy_(1.0) // 1 mGal default {
    
    map_database_.reserve(10);
}

bool GravityFieldMapper::initialize(double region_size_km) {
    region_size_ = region_size_km;
    
    int grid_cells = static_cast<int>(region_size_ * 1000.0 / resolution_);
    
    std::cout << "Gravity Field Mapper initializing..." << std::endl;
    std::cout << "Region size: " << region_size_ << " km" << std::endl;
    std::cout << "Resolution: " << resolution_ << " m" << std::endl;
    std::cout << "Grid size: " << grid_cells << "x" << grid_cells << std::endl;
    
    return true;
}

GravityMap GravityFieldMapper::create_local_map(const std::vector<GravimetricReading>& readings) {
    GravityMap map{};
    
    // Calculate grid dimensions
    map.grid_spacing = resolution_;
    map.grid_size_x = static_cast<int>(region_size_ * 1000.0 / resolution_);
    map.grid_size_y = map.grid_size_x; // Square grid for simplicity
    
    // Initialize gravity field
    map.gravity_field.resize(map.grid_size_x, std::vector<double>(map.grid_size_y, STANDARD_GRAVITY));
    
    // Set origin (center of readings)
    if (!readings.empty()) {
        map.origin_latitude = readings[0].latitude;
        map.origin_longitude = readings[0].longitude;
    }
    
    map.creation_time = std::chrono::system_clock::now();
    map.map_accuracy = current_map_accuracy_;
    
    // Populate gravity field from readings
    for (const auto& reading : readings) {
        // Convert lat/lon to grid coordinates (simplified)
        int grid_x = static_cast<int>((reading.longitude - map.origin_longitude) * 111000.0 / resolution_);
        int grid_y = static_cast<int>((reading.latitude - map.origin_latitude) * 111000.0 / resolution_);
        
        if (grid_x >= 0 && grid_x < map.grid_size_x && grid_y >= 0 && grid_y < map.grid_size_y) {
            map.gravity_field[grid_x][grid_y] = reading.gravity_magnitude;
        }
    }
    
    // Interpolate and smooth the field
    interpolate_gravity_field(map);
    smooth_gravity_field(map);
    
    // Calculate statistics
    calculate_map_statistics(map);
    
    std::cout << "Local gravity map created: " << map.grid_size_x << "x" << map.grid_size_y << " grid" << std::endl;
    
    return map;
}

void GravityFieldMapper::interpolate_gravity_field(GravityMap& map) {
    // Simple bilinear interpolation for empty cells
    for (int i = 0; i < map.grid_size_x; ++i) {
        for (int j = 0; j < map.grid_size_y; ++j) {
            if (std::abs(map.gravity_field[i][j] - STANDARD_GRAVITY) < 1e-6) {
                // Find nearest non-empty cells
                double sum = 0.0;
                int count = 0;
                
                for (int di = -1; di <= 1; ++di) {
                    for (int dj = -1; dj <= 1; ++dj) {
                        int ni = i + di;
                        int nj = j + dj;
                        
                        if (ni >= 0 && ni < map.grid_size_x && nj >= 0 && nj < map.grid_size_y) {
                            if (std::abs(map.gravity_field[ni][nj] - STANDARD_GRAVITY) > 1e-6) {
                                sum += map.gravity_field[ni][nj];
                                count++;
                            }
                        }
                    }
                }
                
                if (count > 0) {
                    map.gravity_field[i][j] = sum / count;
                }
            }
        }
    }
}

void GravityFieldMapper::smooth_gravity_field(GravityMap& map) {
    // Apply Gaussian smoothing
    std::vector<std::vector<double>> smoothed_field = map.gravity_field;
    
    for (int i = 1; i < map.grid_size_x - 1; ++i) {
        for (int j = 1; j < map.grid_size_y - 1; ++j) {
            double sum = 0.0;
            double weights[] = {1, 2, 1, 2, 4, 2, 1, 2, 1}; // 3x3 Gaussian
            int weight_index = 0;
            
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    sum += map.gravity_field[i + di][j + dj] * weights[weight_index++];
                }
            }
            
            smoothed_field[i][j] = sum / 16.0; // Sum of weights
        }
    }
    
    map.gravity_field = smoothed_field;
}

void GravityFieldMapper::calculate_map_statistics(GravityMap& map) {
    double sum = 0.0;
    double sum_sq = 0.0;
    int count = 0;
    
    for (const auto& row : map.gravity_field) {
        for (double value : row) {
            sum += value;
            sum_sq += value * value;
            count++;
        }
    }
    
    double mean = sum / count;
    double variance = (sum_sq - sum * sum / count) / count;
    double std_dev = std::sqrt(variance);
    
    // Update map accuracy based on statistics
    map.map_accuracy = std_dev * 1000.0; // Convert to mGal
    
    std::cout << "Map statistics: mean = " << mean << " m/s², std_dev = " << std_dev * 1000.0 << " mGal" << std::endl;
}

} // namespace lgfn
