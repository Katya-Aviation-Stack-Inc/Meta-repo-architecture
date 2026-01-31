#!/bin/bash

# Revolutionary Aviation Systems build script
# This script builds all revolutionary aviation technologies

echo "========================================"
echo "Building Revolutionary Aviation Systems"
echo "========================================"

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the revolutionary systems
echo "Building the revolutionary systems..."
make -j$(nproc)

# Check if build was successful
if [ $? -eq 0 ]; then
    echo "✓ Revolutionary aviation systems built successfully!"
    
    # Display build artifacts
    echo ""
    echo "Build Artifacts:"
    ls -la airmesh_demo 2>/dev/null && echo "  - airmesh_demo (AAMN demonstration)" || echo "  - airmesh_demo (not found)"
    ls -la revolutionary_demo 2>/dev/null && echo "  - revolutionary_demo (full technology demonstration)" || echo "  - revolutionary_demo (not found)"
    
    echo ""
    echo "To run the AAMN demonstration:"
    echo "  cd build && ./airmesh_demo"
    echo ""
    echo "To run the full revolutionary demonstration:"
    echo "  cd build && ./revolutionary_demo"
    
    echo ""
    echo "========================================"
    echo "Revolutionary Build Complete!"
    echo "========================================"
else
    echo "✗ Failed to build revolutionary aviation systems!"
    exit 1
fi