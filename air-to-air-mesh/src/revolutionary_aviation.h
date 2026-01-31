#ifndef REVOLUTIONARY_AVIATION_H
#define REVOLUTIONARY_AVIATION_H

// Revolutionary Aviation Technologies Suite - Main Header
// This header provides access to all 10 groundbreaking aviation innovations

// Core Air-to-Air Mesh Network (AAMN) System
#include "core/airmesh_core.h"
#include "core/advanced_mesh_features.h"

// Neuro Flight Control Core (Neuro-FCC)
#include "neuro_fcc/neuro_fcc.h"

// Self-Adaptive Rotor Blades
#include "adaptive_rotor/adaptive_rotor.h"

// ColdJet Software Control Core
#include "coldjet/coldjet_control.h"

// Local Gravity Field Navigation (LGFN)
#include "gravity_nav/gravity_navigation.h"

// Predictive Airflow Engine
#include "airflow/predictive_airflow.h"

// Self-Healing Avionics BIOS
#include "self_healing/self_healing_bios.h"

// Vortex Shield System
#include "vortex_shield/vortex_shield.h"

// Air Swarm OS
#include "air_swarm/air_swarm_os.h"

// StarNav Core
#include "starnav/starnav_core.h"

// Component Headers for StarNav
#include "starnav/star_camera.h"
#include "starnav/constellation_recognizer.h"
#include "starnav/position_calculator.h"
#include "starnav/motion_compensator.h"

// Version Information
#define REVOLUTIONARY_AVIATION_VERSION_MAJOR 1
#define REVOLUTIONARY_AVIATION_VERSION_MINOR 0
#define REVOLUTIONARY_AVIATION_VERSION_PATCH 0
#define REVOLUTIONARY_AVIATION_VERSION_STRING "1.0.0"

// Feature Flags
#define FEATURE_AAMN_ENABLED 1
#define FEATURE_NEURO_FCC_ENABLED 1
#define FEATURE_ADAPTIVE_ROTOR_ENABLED 1
#define FEATURE_COLDJET_ENABLED 1
#define FEATURE_LGFN_ENABLED 1
#define FEATURE_PREDICTIVE_AIRFLOW_ENABLED 1
#define FEATURE_SELF_HEALING_ENABLED 1
#define FEATURE_VORTEX_SHIELD_ENABLED 1
#define FEATURE_AIR_SWARM_ENABLED 1
#define FEATURE_STARNAV_ENABLED 1

// Performance Constants
#define MIN_LATENCY_MICROSECONDS 1
#define MAX_BANDWIDTH_UTILIZATION 0.95
#define SECURITY_ENCRYPTION_LEVEL 256 // AES-256
#define RELIABILITY_TARGET 0.9999

// System Configuration
#ifdef __cplusplus
extern "C" {
#endif

// Initialization Functions
int initialize_revolutionary_aviation_system();
int configure_revolutionary_aviation_features();
int start_revolutionary_aviation_services();
int shutdown_revolutionary_aviation_system();

// System Status Functions
int get_revolutionary_aviation_status();
double get_revolutionary_aviation_performance();
const char* get_revolutionary_aviation_version();

// Feature Availability Functions
int is_aamn_available();
int is_neuro_fcc_available();
int is_adaptive_rotor_available();
int is_coldjet_available();
int is_lgfn_available();
int is_predictive_airflow_available();
int is_self_healing_available();
int is_vortex_shield_available();
int is_air_swarm_available();
int is_starnav_available();

#ifdef __cplusplus
}

// C++ API
namespace revolutionary_aviation {

    // System Management
    class SystemManager {
    public:
        static bool initialize();
        static bool configure();
        static bool start();
        static bool shutdown();
        static int get_status();
        static double get_performance();
        static const char* get_version();
    };

    // Feature Availability
    class FeatureChecker {
    public:
        static bool is_aamn_available();
        static bool is_neuro_fcc_available();
        static bool is_adaptive_rotor_available();
        static bool is_coldjet_available();
        static bool is_lgfn_available();
        static bool is_predictive_airflow_available();
        static bool is_self_healing_available();
        static bool is_vortex_shield_available();
        static bool is_air_swarm_available();
        static bool is_starnav_available();
    };

} // namespace revolutionary_aviation

#endif // __cplusplus

#endif // REVOLUTIONARY_AVIATION_H