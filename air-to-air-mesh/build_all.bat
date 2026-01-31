@echo off
REM Revolutionary Aviation Systems - Build Script for Windows
REM This script builds all revolutionary aviation systems on Windows

echo === Revolutionary Aviation Systems Build Script ===
echo Building all revolutionary aviation technologies...
echo.

REM Create build directory
echo Creating build directory...
mkdir build 2>nul
cd build

REM Configure with CMake
echo Configuring with CMake...
cmake .. -DCMAKE_BUILD_TYPE=Release

if %errorlevel% neq 0 (
    echo CMake configuration failed!
    exit /b 1
)

REM Build all targets
echo Building all revolutionary systems...
cmake --build . --config Release --parallel

if %errorlevel% neq 0 (
    echo Build failed!
    exit /b 1
)

echo.
echo === Build Summary ===
echo âœ“ Air-to-Air Mesh Network (AAMN)
echo âœ“ Neuro Flight Control Core (Neuro-FCC)
echo âœ“ Self-Adaptive Rotor Blades
echo âœ“ ColdJet Software Control Core
echo âœ“ Local Gravity Field Navigation (LGFN)
echo âœ“ Predictive Airflow Engine
echo âœ“ Self-Healing Avionics BIOS
echo âœ“ Vortex Shield System
echo âœ“ Air Swarm OS
echo âœ“ StarNav Core
echo.
echo All revolutionary aviation systems built successfully!
echo Executables are located in the build directory.
echo.
echo To run demonstrations:
echo   airmesh_demo.exe              - Air-to-Air Mesh Network
echo   revolutionary_demo.exe         - All systems integrated demo
echo   self_healing_demo.exe         - Self-Healing Avionics BIOS
echo   vortex_shield_demo.exe       - Vortex Shield System
echo   air_swarm_demo.exe            - Air Swarm OS
echo   starnav_demo.exe              - StarNav Core