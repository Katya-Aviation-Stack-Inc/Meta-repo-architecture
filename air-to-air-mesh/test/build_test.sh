#!/bin/bash

# Revolutionary build test script
# This script verifies that the revolutionary AAMN system builds correctly

echo "========================================"
echo "Revolutionary AAMN Build Test"
echo "========================================"

# Clean previous build
echo "Cleaning previous build..."
rm -rf build
echo "✓ Clean completed"

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Configure with CMake
echo "Configuring with CMake..."
if cmake .. -DCMAKE_BUILD_TYPE=Release; then
    echo "✓ CMake configuration successful"
else
    echo "✗ CMake configuration failed"
    exit 1
fi

# Build the revolutionary system
echo "Building the revolutionary system..."
if make -j$(nproc); then
    echo "✓ Build successful"
else
    echo "✗ Build failed"
    exit 1
fi

# Check if executable exists
if [ -f "airmesh_demo" ]; then
    echo "✓ Revolutionary executable created successfully"
    
    # Get file size
    size=$(du -h airmesh_demo | cut -f1)
    echo "  Executable size: $size"
    
    # Check dependencies
    echo "  Dependencies:"
    ldd airmesh_demo 2>/dev/null | grep -E "(libstdc|libgcc|libc)" | sed 's/^/    /' || echo "    None found"
    
    echo ""
    echo "========================================"
    echo "Revolutionary Build Test PASSED!"
    echo "========================================"
else
    echo "✗ Revolutionary executable not found"
    exit 1
fi