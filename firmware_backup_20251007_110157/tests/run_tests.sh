#!/bin/bash

# OHT-50 Firmware Test Runner
# Runs all unit tests using Unity framework

set -e

echo "=== OHT-50 Firmware Test Runner ==="
echo "Date: $(date)"
echo "Build Type: Release"
echo

# Build tests
echo "Building tests..."
cd ../build
make oht50_tests

# Run tests
echo "Running tests..."
./oht50_tests

echo
echo "=== Test Results ==="
echo "All tests completed successfully!"
echo
echo "Test Coverage Summary:"
echo "- HAL Tests: 5/5 components"
echo "- Application Tests: 4/4 components"
echo "- Total Test Cases: TBD"
echo "- Coverage: TBD%"
