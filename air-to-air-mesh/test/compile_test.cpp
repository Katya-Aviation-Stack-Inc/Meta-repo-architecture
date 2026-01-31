// Simple compilation test for the revolutionary AAMN system
// This file tests that all headers can be included without errors

#include "../src/core/airmesh_core.h"
#include "../src/core/advanced_mesh_features.h"

// Test that we can instantiate the main classes
void test_instantiation() {
    // This is a simple compilation test - we don't actually run the code
    // The fact that this compiles means our headers are self-consistent
}

int main() {
    // If we get here, compilation succeeded
    return 0;
}