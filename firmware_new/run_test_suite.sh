#!/bin/bash

# OHT-50 Firmware Test Suite
# Phương án 3: Progressive Testing với Error Handling

set -e  # Dừng ngay nếu có lỗi

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to run test and update counters
run_test() {
    local test_name="$1"
    local test_desc="$2"
    local test_executable="$3"
    
    echo -e "${BLUE}🧪 Running: ${test_name}${NC}"
    echo -e "${YELLOW}📝 Testing: ${test_desc}${NC}"
    
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    if ./tests/${test_executable} 2>/dev/null || ./tests/unit/${test_executable} 2>/dev/null; then
        echo -e "${GREEN}✅ ${test_name}: PASSED${NC}"
        PASSED_TESTS=$((PASSED_TESTS + 1))
        return 0
    else
        echo -e "${RED}❌ ${test_name}: FAILED${NC}"
        FAILED_TESTS=$((FAILED_TESTS + 1))
        return 1
    fi
}

# Function to print phase header
print_phase_header() {
    local phase_num="$1"
    local phase_name="$2"
    local phase_desc="$3"
    
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}🔒 PHASE ${phase_num}: ${phase_name}${NC}"
    echo -e "${BLUE}📋 ${phase_desc}${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo ""
}

# Function to print summary
print_summary() {
    echo ""
    echo -e "${BLUE}========================================${NC}"
    echo -e "${BLUE}📊 TEST SUITE SUMMARY${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo -e "Total Tests: ${TOTAL_TESTS}"
    echo -e "${GREEN}Passed: ${PASSED_TESTS}${NC}"
    echo -e "${RED}Failed: ${FAILED_TESTS}${NC}"
    
    if [ $FAILED_TESTS -eq 0 ]; then
        echo -e "${GREEN}🎉 ALL TESTS PASSED!${NC}"
        echo -e "${GREEN}✅ Firmware ready for deployment${NC}"
        exit 0
    else
        echo -e "${RED}⚠️  ${FAILED_TESTS} test(s) failed${NC}"
        echo -e "${YELLOW}🔧 Please check failed tests above${NC}"
        exit 1
    fi
}

# Main test execution
main() {
    echo -e "${BLUE}🚀 OHT-50 FIRMWARE TEST SUITE${NC}"
    echo -e "${BLUE}========================================${NC}"
    echo -e "${YELLOW}📅 Started at: $(date)${NC}"
    echo -e "${YELLOW}🖥️  Platform: $(uname -s) $(uname -m)${NC}"
    echo ""

    # Change to build directory
    cd build
    
    # Phase 1: Core Safety (Critical First)
    print_phase_header "1" "CORE SAFETY TESTING" "Testing critical safety systems - E-Stop, Safety Zones, Interlocks"
    
    if run_test "Safety Basic" "E-Stop handling, System controller basic functions" "test_safety_basic_mock"; then
        echo -e "${GREEN}✅ Safety System: PASSED (Critical safety functions working)${NC}"
    else
        echo -e "${RED}❌ Safety System: FAILED - STOPPING TESTS${NC}"
        echo -e "${RED}🚨 Critical safety failure detected!${NC}"
        print_summary
        exit 1
    fi

    # Phase 2: Communication & Modules
    print_phase_header "2" "COMMUNICATION & MODULES" "Testing RS485 communication, module discovery, telemetry"
    
    run_test "Communication Manager" "RS485/Modbus protocol, timeout handling, error recovery" "test_communication_manager"
    run_test "Module Discovery" "Auto-discovery, health monitoring, module registry" "test_module_discovery"
    run_test "Telemetry Manager" "Real-time data collection, metrics, performance monitoring" "test_telemetry_manager"

    # Phase 3: System Integration
    print_phase_header "3" "SYSTEM INTEGRATION" "Testing API endpoints, WebSocket, system integration"
    
    run_test "Basic Integration" "API endpoints, HTTP server, WebSocket communication" "test_basic_integration"
    run_test "Module Discovery Unit" "Unit tests for module discovery functionality" "test_module_discovery_unit"

    # Phase 4: Performance & Stress
    print_phase_header "4" "PERFORMANCE & STRESS" "Testing system performance, load handling, fault injection"
    
    run_test "Performance Load" "CPU usage, memory usage, throughput testing" "test_performance_load"
    run_test "Stress Testing" "Fault injection, error recovery, system stability" "test_stress_testing"

    # Print final summary
    print_summary
}

# Handle interrupts gracefully
trap 'echo -e "\n${YELLOW}⚠️  Test interrupted by user${NC}"; print_summary; exit 1' INT TERM

# Run main function
main "$@"
