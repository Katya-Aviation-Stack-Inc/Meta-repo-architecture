#!/bin/bash

# Revolutionary Aviation Systems - Build Script
# This script builds all revolutionary aviation systems

echo "=== Revolutionary Aviation Systems Build Script ==="
echo "Building all revolutionary aviation technologies..."
echo ""

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build all targets
echo "Building all revolutionary systems..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "=== Build Summary ==="
echo "✓ Air-to-Air Mesh Network (AAMN)"
echo "✓ Neuro Flight Control Core (Neuro-FCC)"
echo "✓ Self-Adaptive Rotor Blades"
echo "✓ ColdJet Software Control Core"
echo "✓ Local Gravity Field Navigation (LGFN)"
echo "✓ Predictive Airflow Engine"
echo "✓ Self-Healing Avionics BIOS"
echo "✓ Vortex Shield System"
echo "✓ Air Swarm OS"
echo "✓ StarNav Core"
echo ""
echo "All revolutionary aviation systems built successfully!"
echo "Executables are located in the build directory."
echo ""
echo "To run demonstrations:"
echo "  ./airmesh_demo              # Air-to-Air Mesh Network"
echo "  ./revolutionary_demo         # All systems integrated demo"
echo "  ./self_healing_demo         # Self-Healing Avionics BIOS"
echo "  ./vortex_shield_demo       # Vortex Shield System"
echo "  ./air_swarm_demo            # Air Swarm OS"
echo "  ./starnav_demo              # StarNav Core"