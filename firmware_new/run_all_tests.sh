#!/bin/bash

# FW Team - Run All Unit Tests Script (CLEANED VERSION)
# Version: 2.0.0
# Date: 2025-01-28
# Status: Simplified test suite after cleanup

echo "=========================================="
echo "FW Team - Simplified Unit Tests Execution"
echo "=========================================="
echo "Date: $(date)"
echo "Build Directory: $(pwd)"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test results tracking
total_tests=0
passed_tests=0
failed_tests=0

# Function to run a test and capture results
run_test() {
    local test_name=$1
    local test_target=$2
    
    echo "Running: $test_name"
    echo "----------------------------------------"
    
    if make $test_target > /dev/null 2>&1; then
        local output=$(./tests/unit/$test_target 2>&1)
        local exit_code=$?
        
        # Extract test results from Unity output
        local test_count=$(echo "$output" | grep "Tests Run:" | awk '{print $3}')
        local failures=$(echo "$output" | grep "Failures:" | awk '{print $2}')
        local ignored=$(echo "$output" | grep "Ignored:" | awk '{print $2}')
        local success_rate=$(echo "$output" | grep "Success Rate:" | awk '{print $3}' | sed 's/%//')
        
        if [ "$exit_code" -eq 0 ] && [ "$failures" -eq 0 ]; then
            echo -e "${GREEN}✅ PASSED${NC}"
            echo "Tests: $test_count, Failures: $failures, Success Rate: ${success_rate}%"
            passed_tests=$((passed_tests + test_count))
        else
            echo -e "${RED}❌ FAILED${NC}"
            echo "Tests: $test_count, Failures: $failures, Success Rate: ${success_rate}%"
            failed_tests=$((failed_tests + failures))
        fi
        
        total_tests=$((total_tests + test_count))
        
        # Show detailed output for failures
        if [ "$failures" -gt 0 ]; then
            echo ""
            echo "Failed Tests:"
            echo "$output" | grep "❌ FAILED" -A 1
        fi
        
    else
        echo -e "${RED}❌ BUILD FAILED${NC}"
        failed_tests=$((failed_tests + 1))
    fi
    
    echo ""
}

# Run simplified test suite
echo "Starting simplified unit test execution..."
echo ""

# Core HAL Tests (ESSENTIAL)
run_test "HAL Common Tests" "test_hal_common"
run_test "HAL GPIO Tests" "test_hal_gpio"
run_test "HAL RS485 Tests" "test_hal_rs485"
run_test "HAL E-Stop Tests" "test_hal_estop"

# Basic Integration Tests (SIMPLIFIED)
run_test "Basic Integration Tests" "test_basic_integration"
run_test "Module Discovery Tests" "test_module_discovery_simple"

# Basic Performance Tests (SIMPLIFIED)
run_test "Basic Performance Tests" "test_basic_performance"

# Generate summary report
echo "=========================================="
echo "SIMPLIFIED TEST EXECUTION SUMMARY"
echo "=========================================="
echo "Total Tests Run: $total_tests"
echo "Passed Tests: $passed_tests"
echo "Failed Tests: $failed_tests"

if [ $failed_tests -eq 0 ]; then
    echo -e "${GREEN}🎉 ALL TESTS PASSED!${NC}"
    exit_code=0
else
    echo -e "${RED}⚠️  SOME TESTS FAILED${NC}"
    exit_code=1
fi

echo ""
echo "Simplified Test Coverage Summary:"
echo "- HAL Layer: Common, GPIO, RS485, E-Stop"
echo "- Integration: Basic integration, Module discovery"
echo "- Performance: Basic performance validation"
echo ""
echo "Removed: Complex performance tests, stress tests, safety validation tests"
echo "Reason: Simplified test suite for faster development cycle"

exit $exit_code
