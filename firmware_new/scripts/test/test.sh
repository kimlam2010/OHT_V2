#!/bin/bash
# OHT-50 Firmware Test Script
# ISO/IEC/IEEE 29119 Compliant

set -e

BUILD_DIR="build"

echo "Running OHT-50 Firmware tests..."

# Build if not already built
if [ ! -d "$BUILD_DIR" ]; then
    echo "Building firmware first..."
    ./scripts/build.sh
fi

# Run tests
cd $BUILD_DIR
ctest --output-on-failure --verbose

echo "Tests completed!"
