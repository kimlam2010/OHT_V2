#!/bin/bash

# Network Management Integration Test Script - OHT-50
# Version: 1.0.0
# Date: 2025-01-28
# Author: Firmware Team - OHT-50

# Test Configuration
FIRMWARE_URL="http://localhost:8080"
OPERATOR_TOKEN="oht50_operator_token_2025"
ADMIN_TOKEN="oht50_admin_token_2025"
TEST_SSID="OHT-50-Test"
TEST_PASSWORD="test_password_123"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test Statistics
TESTS_RUN=0
TESTS_PASSED=0
TESTS_FAILED=0

# Test Helper Functions
test_start() {
    TESTS_RUN=$((TESTS_RUN + 1))
    echo -e "${BLUE}[$TESTS_RUN] $1...${NC}"
}

test_pass() {
    TESTS_PASSED=$((TESTS_PASSED + 1))
    echo -e "${GREEN}✓ PASS${NC}"
}

test_fail() {
    TESTS_FAILED=$((TESTS_FAILED + 1))
    echo -e "${RED}✗ FAIL ($1)${NC}"
}

test_result() {
    local response="$1"
    local expected_status="$2"
    local test_name="$3"
    
    if [[ "$response" == *"\"success\": true"* ]] && [[ "$response" == *"\"status\": $expected_status"* ]]; then
        test_pass
        return 0
    else
        test_fail "Expected success=true, status=$expected_status"
        return 1
    fi
}

# Network API Test Functions
test_network_status() {
    test_start "Network Status API"
    
    response=$(curl -s -w "%{http_code}" "$FIRMWARE_URL/api/v1/network/status")
    http_code="${response: -3}"
    body="${response%???}"
    
    if [[ "$http_code" == "200" ]] && [[ "$body" == *"\"success\": true"* ]]; then
        echo "    Response: $body"
        test_pass
    else
        test_fail "HTTP $http_code: $body"
    fi
}

test_wifi_scan() {
    test_start "WiFi Scan API"
    
    response=$(curl -s -w "%{http_code}" "$FIRMWARE_URL/api/v1/network/wifi/scan")
    http_code="${response: -3}"
    body="${response%???}"
    
    if [[ "$http_code" == "200" ]] && [[ "$body" == *"\"success\": true"* ]]; then
        echo "    Response: $body"
        test_pass
    else
        test_fail "HTTP $http_code: $body"
    fi
}

test_wifi_connect() {
    test_start "WiFi Connect API"
    
    response=$(curl -s -w "%{http_code}" \
        -H "Authorization: Bearer $OPERATOR_TOKEN" \
        -H "Content-Type: application/json" \
        -X POST "$FIRMWARE_URL/api/v1/network/wifi/connect" \
        -d "{\"ssid\": \"$TEST_SSID\", \"password\": \"$TEST_PASSWORD\", \"security_type\": \"WPA2\"}")
    
    http_code="${response: -3}"
    body="${response%???}"
    
    if [[ "$http_code" == "200" ]] && [[ "$body" == *"\"success\": true"* ]]; then
        echo "    Response: $body"
        test_pass
    else
        test_fail "HTTP $http_code: $body"
    fi
}

test_wifi_disconnect() {
    test_start "WiFi Disconnect API"
    
    response=$(curl -s -w "%{http_code}" \
        -H "Authorization: Bearer $OPERATOR_TOKEN" \
        -X POST "$FIRMWARE_URL/api/v1/network/wifi/disconnect")
    
    http_code="${response: -3}"
    body="${response%???}"
    
    if [[ "$http_code" == "200" ]] && [[ "$body" == *"\"success\": true"* ]]; then
        echo "    Response: $body"
        test_pass
    else
        test_fail "HTTP $http_code: $body"
    fi
}

test_network_performance() {
    test_start "Network Performance API"
    
    response=$(curl -s -w "%{http_code}" "$FIRMWARE_URL/api/v1/network/performance")
    http_code="${response: -3}"
    body="${response%???}"
    
    if [[ "$http_code" == "200" ]] && [[ "$body" == *"\"success\": true"* ]]; then
        echo "    Response: $body"
        test_pass
    else
        test_fail "HTTP $http_code: $body"
    fi
}

test_network_health() {
    test_start "Network Health API"
    
    response=$(curl -s -w "%{http_code}" "$FIRMWARE_URL/api/v1/network/health")
    http_code="${response: -3}"
    body="${response%???}"
    
    if [[ "$http_code" == "200" ]] && [[ "$body" == *"\"success\": true"* ]]; then
        echo "    Response: $body"
        test_pass
    else
        test_fail "HTTP $http_code: $body"
    fi
}

test_roaming_enable() {
    test_start "Roaming Enable API"
    
    response=$(curl -s -w "%{http_code}" \
        -H "Authorization: Bearer $ADMIN_TOKEN" \
        -H "Content-Type: application/json" \
        -X POST "$FIRMWARE_URL/api/v1/network/roaming/enable" \
        -d "{\"enabled\": true, \"threshold\": -70}")
    
    http_code="${response: -3}"
    body="${response%???}"
    
    if [[ "$http_code" == "200" ]] && [[ "$body" == *"\"success\": true"* ]]; then
        echo "    Response: $body"
        test_pass
    else
        test_fail "HTTP $http_code: $body"
    fi
}

test_mobile_app_enable() {
    test_start "Mobile App Enable API"
    
    response=$(curl -s -w "%{http_code}" \
        -H "Authorization: Bearer $ADMIN_TOKEN" \
        -H "Content-Type: application/json" \
        -X POST "$FIRMWARE_URL/api/v1/network/mobile/enable" \
        -d "{\"enabled\": true, \"port\": 8080}")
    
    http_code="${response: -3}"
    body="${response%???}"
    
    if [[ "$http_code" == "200" ]] && [[ "$body" == *"\"success\": true"* ]]; then
        echo "    Response: $body"
        test_pass
    else
        test_fail "HTTP $http_code: $body"
    fi
}

test_network_config() {
    test_start "Network Config API"
    
    # Test GET config
    response=$(curl -s -w "%{http_code}" \
        -H "Authorization: Bearer $OPERATOR_TOKEN" \
        "$FIRMWARE_URL/api/v1/network/config")
    
    http_code="${response: -3}"
    body="${response%???}"
    
    if [[ "$http_code" == "200" ]] && [[ "$body" == *"\"success\": true"* ]]; then
        echo "    GET Config: $body"
        
        # Test POST config
        response=$(curl -s -w "%{http_code}" \
            -H "Authorization: Bearer $ADMIN_TOKEN" \
            -H "Content-Type: application/json" \
            -X POST "$FIRMWARE_URL/api/v1/network/config" \
            -d "{\"wifi_enabled\": true, \"wifi_ssid\": \"$TEST_SSID\", \"wifi_password\": \"$TEST_PASSWORD\", \"roaming_enabled\": true}")
        
        http_code="${response: -3}"
        body="${response%???}"
        
        if [[ "$http_code" == "200" ]] && [[ "$body" == *"\"success\": true"* ]]; then
            echo "    POST Config: $body"
            test_pass
        else
            test_fail "POST Config failed: HTTP $http_code: $body"
        fi
    else
        test_fail "GET Config failed: HTTP $http_code: $body"
    fi
}

test_network_statistics() {
    test_start "Network Statistics API"
    
    response=$(curl -s -w "%{http_code}" \
        -H "Authorization: Bearer $OPERATOR_TOKEN" \
        "$FIRMWARE_URL/api/v1/network/statistics")
    
    http_code="${response: -3}"
    body="${response%???}"
    
    if [[ "$http_code" == "200" ]] && [[ "$body" == *"\"success\": true"* ]]; then
        echo "    Response: $body"
        test_pass
    else
        test_fail "HTTP $http_code: $body"
    fi
}

test_statistics_reset() {
    test_start "Statistics Reset API"
    
    response=$(curl -s -w "%{http_code}" \
        -H "Authorization: Bearer $ADMIN_TOKEN" \
        -X POST "$FIRMWARE_URL/api/v1/network/statistics/reset")
    
    http_code="${response: -3}"
    body="${response%???}"
    
    if [[ "$http_code" == "200" ]] && [[ "$body" == *"\"success\": true"* ]]; then
        echo "    Response: $body"
        test_pass
    else
        test_fail "HTTP $http_code: $body"
    fi
}

# Authentication Tests
test_authentication() {
    test_start "Authentication Tests"
    
    # Test unauthorized access
    response=$(curl -s -w "%{http_code}" \
        -H "Authorization: Bearer invalid_token" \
        "$FIRMWARE_URL/api/v1/network/config")
    
    http_code="${response: -3}"
    
    if [[ "$http_code" == "401" ]]; then
        echo "    Unauthorized access properly rejected"
        
        # Test valid authentication
        response=$(curl -s -w "%{http_code}" \
            -H "Authorization: Bearer $OPERATOR_TOKEN" \
            "$FIRMWARE_URL/api/v1/network/config")
        
        http_code="${response: -3}"
        
        if [[ "$http_code" == "200" ]]; then
            echo "    Valid authentication accepted"
            test_pass
        else
            test_fail "Valid token rejected: HTTP $http_code"
        fi
    else
        test_fail "Unauthorized access not properly rejected: HTTP $http_code"
    fi
}

# Performance Tests
test_performance() {
    test_start "Performance Tests"
    
    # Test response times for critical endpoints
    echo "    Testing response times..."
    
    # Network Status (should be < 100ms)
    start_time=$(date +%s%3N)
    response=$(curl -s "$FIRMWARE_URL/api/v1/network/status")
    end_time=$(date +%s%3N)
    response_time=$((end_time - start_time))
    
    echo "    Network Status: ${response_time}ms"
    
    if [[ $response_time -lt 100 ]]; then
        echo "    ✓ Network Status response time OK"
        
        # WiFi Scan (should be < 10 seconds)
        start_time=$(date +%s%3N)
        response=$(curl -s "$FIRMWARE_URL/api/v1/network/wifi/scan")
        end_time=$(date +%s%3N)
        response_time=$((end_time - start_time))
        
        echo "    WiFi Scan: ${response_time}ms"
        
        if [[ $response_time -lt 10000 ]]; then
            echo "    ✓ WiFi Scan response time OK"
            test_pass
        else
            test_fail "WiFi Scan too slow: ${response_time}ms"
        fi
    else
        test_fail "Network Status too slow: ${response_time}ms"
    fi
}

# Error Handling Tests
test_error_handling() {
    test_start "Error Handling Tests"
    
    # Test invalid endpoint
    response=$(curl -s -w "%{http_code}" "$FIRMWARE_URL/api/v1/network/invalid")
    http_code="${response: -3}"
    
    if [[ "$http_code" == "404" ]]; then
        echo "    Invalid endpoint properly returns 404"
        
        # Test invalid JSON
        response=$(curl -s -w "%{http_code}" \
            -H "Authorization: Bearer $OPERATOR_TOKEN" \
            -H "Content-Type: application/json" \
            -X POST "$FIRMWARE_URL/api/v1/network/wifi/connect" \
            -d "invalid json")
        
        http_code="${response: -3}"
        
        if [[ "$http_code" == "400" ]]; then
            echo "    Invalid JSON properly returns 400"
            test_pass
        else
            test_fail "Invalid JSON not properly handled: HTTP $http_code"
        fi
    else
        test_fail "Invalid endpoint not properly handled: HTTP $http_code"
    fi
}

# Main Test Runner
main() {
    echo -e "${YELLOW}=== OHT-50 Network Management Integration Tests ===${NC}"
    echo "Firmware URL: $FIRMWARE_URL"
    echo "Operator Token: $OPERATOR_TOKEN"
    echo "Admin Token: $ADMIN_TOKEN"
    echo ""
    
    # Check if firmware is running
    echo -e "${BLUE}Checking firmware connectivity...${NC}"
    if curl -s --connect-timeout 5 "$FIRMWARE_URL/api/v1/network/status" > /dev/null; then
        echo -e "${GREEN}✓ Firmware is running and accessible${NC}"
    else
        echo -e "${RED}✗ Firmware is not accessible at $FIRMWARE_URL${NC}"
        echo "Please ensure firmware is running before running tests."
        exit 1
    fi
    echo ""
    
    # Run API Tests
    echo -e "${YELLOW}--- Network API Tests ---${NC}"
    test_network_status
    test_wifi_scan
    test_wifi_connect
    test_wifi_disconnect
    test_network_performance
    test_network_health
    test_roaming_enable
    test_mobile_app_enable
    test_network_config
    test_network_statistics
    test_statistics_reset
    
    # Run Authentication Tests
    echo -e "\n${YELLOW}--- Authentication Tests ---${NC}"
    test_authentication
    
    # Run Performance Tests
    echo -e "\n${YELLOW}--- Performance Tests ---${NC}"
    test_performance
    
    # Run Error Handling Tests
    echo -e "\n${YELLOW}--- Error Handling Tests ---${NC}"
    test_error_handling
    
    # Print Results
    echo -e "\n${YELLOW}=== Test Results ===${NC}"
    echo "Tests Run: $TESTS_RUN"
    echo -e "Tests Passed: ${GREEN}$TESTS_PASSED${NC}"
    echo -e "Tests Failed: ${RED}$TESTS_FAILED${NC}"
    
    success_rate=$((TESTS_PASSED * 100 / TESTS_RUN))
    echo "Success Rate: $success_rate%"
    
    if [[ $TESTS_FAILED -eq 0 ]]; then
        echo -e "\n${GREEN}🎉 All tests passed! Network Management is ready for deployment.${NC}"
        exit 0
    else
        echo -e "\n${RED}❌ $TESTS_FAILED test(s) failed. Please fix issues before deployment.${NC}"
        exit 1
    fi
}

# Run main function
main "$@"
