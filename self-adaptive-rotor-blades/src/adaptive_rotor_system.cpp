#include "adaptive_rotor_system.h"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <complex>
#include <random>
#include <numeric>

namespace adaptive_rotor {

// DSP Filter Implementation
DSPFilter::DSPFilter(int filter_order, double sample_rate)
    : filter_order_(filter_order)
    , sample_rate_(sample_rate)
    , phase_delay_(0.0)
    , is_iir_(true) {
    
    delay_line_.resize(filter_order_, 0.0);
    coefficients_.resize(filter_order_ + 1, 0.0);
}

std::vector<double> DSPFilter::apply_filter(const std::vector<double>& input) {
    if (is_iir_) {
        return apply_iir_filter(input);
    } else {
        return apply_fir_filter(input);
    }
}

void DSPFilter::design_bandstop(double center_freq, double bandwidth) {
    // Design a simple bandstop filter for vibration suppression
    double omega_c = 2.0 * M_PI * center_freq / sample_rate_;
    double delta_omega = 2.0 * M_PI * bandwidth / sample_rate_;
    
    coefficients_.clear();
    coefficients_.resize(filter_order_ + 1, 0.0);
    
    // Simple bandstop design using window method
    for (int n = 0; n <= filter_order_; ++n) {
        if (n == filter_order_ / 2) {
            coefficients_[n] = 1.0 - (bandwidth / sample_rate_);
        } else {
            double arg = delta_omega * (n - filter_order_ / 2) / 2.0;
            if (std::abs(arg) > 1e-6) {
                coefficients_[n] = -std::sin(arg) / (M_PI * (n - filter_order_ / 2));
            }
        }
        
        // Apply Hamming window
        double window = 0.54 - 0.46 * std::cos(2.0 * M_PI * n / filter_order_);
        coefficients_[n] *= window;
    }
    
    is_iir_ = false; // Use FIR for bandstop
}

std::vector<double> DSPFilter::apply_fir_filter(const std::vector<double>& input) {
    std::vector<double> output(input.size(), 0.0);
    
    for (size_t i = 0; i < input.size(); ++i) {
        double sum = 0.0;
        for (int j = 0; j <= filter_order_; ++j) {
            if (i >= static_cast<size_t>(j)) {
                sum += coefficients_[j] * input[i - j];
            }
        }
        output[i] = sum;
    }
    
    return output;
}

std::vector<double> DSPFilter::apply_iir_filter(const std::vector<double>& input) {
    std::vector<double> output(input.size(), 0.0);
    
    for (size_t i = 0; i < input.size(); ++i) {
        double sum = coefficients_[0] * input[i];
        
        for (int j = 1; j <= filter_order_; ++j) {
            if (i >= static_cast<size_t>(j)) {
                sum += coefficients_[j] * input[i - j];
                sum -= coefficients_[j] * output[i - j];
            }
        }
        
        output[i] = sum;
    }
    
    return output;
}

// Adaptive Controller Implementation
AdaptiveController::AdaptiveController(int num_blades, int elements_per_blade)
    : num_blades_(num_blades)
    , elements_per_blade_(elements_per_blade)
    , current_mode_(AdaptationMode::PASSIVE)
    , resonance_detection_enabled_(true)
    , resonance_detected_(false)
    , resonance_frequency_(0.0)
    , target_efficiency_(0.85)
    , current_efficiency_(0.0) {
    
    vibration_filter_ = std::make_unique<DSPFilter>(8, 1000.0); // 8th order, 1kHz sample rate
    blade_load_history_.resize(num_blades_);
    for (auto& history : blade_load_history_) {
        history.resize(100, 0.0); // 100 samples history
    }
}

bool AdaptiveController::initialize() {
    std::cout << "Adaptive Controller initializing..." << std::endl;
    
    // Design vibration suppression filter
    vibration_filter_->design_bandstop(50.0, 10.0); // Suppress 50Hz vibration
    
    std::cout << "Adaptive Controller initialized successfully" << std::endl;
    return true;
}

void AdaptiveController::set_adaptation_mode(AdaptationMode mode) {
    current_mode_ = mode;
    std::cout << "Adaptation mode set to: " << static_cast<int>(mode) << std::endl;
}

std::vector<ServoCommand> AdaptiveController::calculate_adaptation(
    const RotorState& rotor_state,
    const std::vector<BladeElement>& blade_elements,
    const std::vector<VibrationData>& vibrations) {
    
    std::vector<ServoCommand> commands;
    
    switch (current_mode_) {
        case AdaptationMode::ACTIVE_VIBRATION_SUPPRESSION:
            commands = suppress_vibrations(vibrations);
            break;
        case AdaptationMode::PERFORMANCE_OPTIMIZATION:
            optimize_blade_loading(rotor_state, const_cast<std::vector<BladeElement>&>(blade_elements));
            break;
        case AdaptationMode::RESONANCE_AVOIDANCE:
            detect_resonance_conditions(rotor_state);
            if (resonance_detected_) {
                commands = generate_resonance_avoidance_commands();
            }
            break;
        case AdaptationMode::EMERGENCY_ADAPTATION:
            commands = generate_emergency_adaptation_commands(rotor_state);
            break;
        default:
            break;
    }
    
    return commands;
}

void AdaptiveController::analyze_vibration_spectrum(const std::vector<VibrationData>& vibrations) {
    // Simple frequency analysis
    std::map<double, double> frequency_spectrum;
    
    for (const auto& vib : vibrations) {
        frequency_spectrum[vib.frequency] += vib.amplitude;
    }
    
    // Find dominant frequencies
    double max_amplitude = 0.0;
    double dominant_frequency = 0.0;
    
    for (const auto& [freq, amp] : frequency_spectrum) {
        if (amp > max_amplitude) {
            max_amplitude = amp;
            dominant_frequency = freq;
        }
    }
    
    // Check for resonance conditions
    if (max_amplitude > 2.0) { // 2g threshold
        resonance_detected_ = true;
        resonance_frequency_ = dominant_frequency;
        std::cout << "Resonance detected at " << dominant_frequency << " Hz, amplitude: " << max_amplitude << "g" << std::endl;
    }
}

void AdaptiveController::detect_resonance_conditions(const RotorState& state) {
    // Calculate blade passing frequency
    double blade_passing_freq = (state.rotor_speed / 60.0) * state.num_blades;
    
    // Check for harmonics
    for (int harmonic = 1; harmonic <= 5; ++harmonic) {
        double harmonic_freq = blade_passing_freq * harmonic;
        
        // Simple resonance detection (would use FFT in real implementation)
        if (std::abs(harmonic_freq - resonance_frequency_) < 2.0) { // Within 2Hz
            resonance_detected_ = true;
            std::cout << "Harmonic resonance detected: " << harmonic_freq << " Hz" << std::endl;
        }
    }
}

std::vector<ServoCommand> AdaptiveController::suppress_vibrations(const std::vector<VibrationData>& vibrations) {
    std::vector<ServoCommand> commands;
    
    // Group vibrations by blade
    std::map<int, std::vector<VibrationData>> blade_vibrations;
    for (const auto& vib : vibrations) {
        blade_vibrations[vib.blade_id].push_back(vib);
    }
    
    for (const auto& [blade_id, blade_vibs] : blade_vibrations) {
        for (const auto& vib : blade_vibs) {
            if (vib.amplitude > 0.5) { // 0.5g threshold
                ServoCommand command;
                command.blade_id = blade_id;
                command.element_id = vib.sensor_id % elements_per_blade_;
                command.pitch_adjustment = -vib.amplitude * 0.1; // Proportional control
                command.flap_deflection = vib.amplitude * 0.05;
                command.torsion_adjustment = 0.0;
                command.timestamp = std::chrono::system_clock::now();
                
                commands.push_back(command);
            }
        }
    }
    
    return commands;
}

void AdaptiveController::optimize_blade_loading(const RotorState& state, std::vector<BladeElement>& elements) {
    // Optimize blade twist distribution for maximum efficiency
    double total_lift = 0.0;
    double total_drag = 0.0;
    
    for (auto& element : elements) {
        // Calculate optimal angle of attack
        double optimal_aoa = 5.0; // degrees (simplified)
        double aoa_error = optimal_aoa - element.angle_of_attack;
        
        // Adjust pitch to optimize
        element.pitch_angle += aoa_error * 0.1; // Small adjustment
        
        // Update aerodynamic coefficients (simplified)
        element.lift_coefficient = 2.0 * M_PI * element.angle_of_attack * M_PI / 180.0;
        element.drag_coefficient = 0.01 + 0.05 * std::pow(element.angle_of_attack * M_PI / 180.0, 2);
        
        total_lift += element.lift_coefficient * element.chord;
        total_drag += element.drag_coefficient * element.chord;
    }
    
    // Calculate efficiency
    current_efficiency_ = total_lift / (total_lift + total_drag);
    
    if (current_efficiency_ < target_efficiency_) {
        std::cout << "Blade efficiency: " << current_efficiency_ << " (target: " << target_efficiency_ << ")" << std::endl;
    }
}

std::vector<ServoCommand> AdaptiveController::generate_resonance_avoidance_commands() {
    std::vector<ServoCommand> commands;
    
    // Generate commands to shift resonance frequency
    for (int blade = 0; blade < num_blades_; ++blade) {
        for (int element = 0; element < elements_per_blade_; element += 5) { // Every 5th element
            ServoCommand command;
            command.blade_id = blade;
            command.element_id = element;
            command.pitch_adjustment = std::sin(blade * M_PI / 2.0) * 2.0; // 2 degree variation
            command.flap_deflection = 0.0;
            command.torsion_adjustment = 0.0;
            command.timestamp = std::chrono::system_clock::now();
            
            commands.push_back(command);
        }
    }
    
    std::cout << "Generated " << commands.size() << " resonance avoidance commands" << std::endl;
    return commands;
}

std::vector<ServoCommand> AdaptiveController::generate_emergency_adaptation_commands(const RotorState& state) {
    std::vector<ServoCommand> commands;
    
    // Emergency: flatten blade pitch to reduce loads
    for (int blade = 0; blade < num_blades_; ++blade) {
        for (int element = 0; element < elements_per_blade_; element += 3) { // Every 3rd element
            ServoCommand command;
            command.blade_id = blade;
            command.element_id = element;
            command.pitch_adjustment = -2.0; // Reduce pitch
            command.flap_deflection = 1.0; // Deploy flaps for drag
            command.torsion_adjustment = 0.0;
            command.timestamp = std::chrono::system_clock::now();
            
            commands.push_back(command);
        }
    }
    
    std::cout << "Emergency adaptation: " << commands.size() << " commands generated" << std::endl;
    return commands;
}

// MicroServoArray Implementation
MicroServoArray::MicroServoArray(int num_blades, int servos_per_blade)
    : num_blades_(num_blades)
    , servos_per_blade_(servos_per_blade)
    , max_deflection_(15.0) // 15 degrees
    , max_rate_(90.0) // 90 deg/sec
    , last_update_time_(std::chrono::system_clock::now()) {
    
    servo_positions_.resize(num_blades_, std::vector<double>(servos_per_blade_, 0.0));
    servo_rates_.resize(num_blades_, std::vector<double>(servos_per_blade_, 0.0));
    servo_health_.resize(num_blades_ * servos_per_blade_, true);
}

bool MicroServoArray::initialize() {
    std::cout << "MicroServoArray initializing with " << num_blades_ << " blades, " 
              << servos_per_blade_ << " servos per blade" << std::endl;
    
    // Initialize all servos to neutral position
    for (int blade = 0; blade < num_blades_; ++blade) {
        for (int servo = 0; servo < servos_per_blade_; ++servo) {
            servo_positions_[blade][servo] = 0.0;
            servo_rates_[blade][servo] = 0.0;
        }
    }
    
    std::cout << "MicroServoArray initialized successfully" << std::endl;
    return true;
}

bool MicroServoArray::execute_command(const ServoCommand& command) {
    if (command.blade_id >= num_blades_ || command.element_id >= servos_per_blade_) {
        return false;
    }
    
    int servo_index = command.blade_id * servos_per_blade_ + command.element_id;
    if (!servo_health_[servo_index]) {
        return false;
    }
    
    ServoCommand validated_command = command;
    validate_command(validated_command);
    
    if (!check_rate_limits(validated_command)) {
        return false;
    }
    
    // Update servo position
    servo_positions_[command.blade_id][command.element_id] += validated_command.pitch_adjustment;
    servo_positions_[command.blade_id][command.element_id] += validated_command.flap_deflection;
    servo_positions_[command.blade_id][command.element_id] += validated_command.torsion_adjustment;
    
    update_servo_dynamics(validated_command);
    
    return true;
}

std::vector<ServoCommand> MicroServoArray::execute_batch_commands(const std::vector<ServoCommand>& commands) {
    std::vector<ServoCommand> successful_commands;
    
    for (const auto& command : commands) {
        if (execute_command(command)) {
            successful_commands.push_back(command);
        }
    }
    
    return successful_commands;
}

void MicroServoArray::validate_command(ServoCommand& command) {
    // Limit deflection
    command.pitch_adjustment = std::clamp(command.pitch_adjustment, -max_deflection_, max_deflection_);
    command.flap_deflection = std::clamp(command.flap_deflection, -max_deflection_, max_deflection_);
    command.torsion_adjustment = std::clamp(command.torsion_adjustment, -max_deflection_, max_deflection_);
}

bool MicroServoArray::check_rate_limits(const ServoCommand& command) {
    auto now = std::chrono::system_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update_time_);
    
    if (dt.count() == 0) return false;
    
    double dt_sec = dt.count() / 1000.0;
    double max_change = max_rate_ * dt_sec;
    
    double total_change = std::abs(command.pitch_adjustment) + 
                         std::abs(command.flap_deflection) + 
                         std::abs(command.torsion_adjustment);
    
    return total_change <= max_change;
}

void MicroServoArray::update_servo_dynamics(const ServoCommand& command) {
    // Update servo rate (simplified)
    auto now = std::chrono::system_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update_time_);
    
    if (dt.count() > 0) {
        double dt_sec = dt.count() / 1000.0;
        double total_change = std::abs(command.pitch_adjustment) + 
                             std::abs(command.flap_deflection) + 
                             std::abs(command.torsion_adjustment);
        
        servo_rates_[command.blade_id][command.element_id] = total_change / dt_sec;
    }
    
    last_update_time_ = now;
}

} // namespace adaptive_rotor
