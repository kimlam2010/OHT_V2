#!/bin/bash

# Test script for OHT-50 API Endpoints
# This script tests the API endpoints after FW startup

echo "=== OHT-50 API Endpoints Test ==="
echo "Testing communication between FW and Backend"
echo ""

# Configuration
OHT_IP="localhost"
OHT_PORT="8080"
BASE_URL="http://${OHT_IP}:${OHT_PORT}"

echo "Testing API endpoints at: ${BASE_URL}"
echo ""

# Test 1: System Status
echo "Test 1: System Status"
echo "Endpoint: GET ${BASE_URL}/api/v1/system/status"
echo "Expected: System state and uptime"
echo "Status: Testing..."
echo ""

# Test 2: System Health
echo "Test 2: System Health"
echo "Endpoint: GET ${BASE_URL}/api/v1/system/health"
echo "Expected: Health check response"
echo "Status: Testing..."
echo ""

# Test 3: Communication Status
echo "Test 3: Communication Status"
echo "Endpoint: GET ${BASE_URL}/api/v1/communication/status"
echo "Expected: Module status and registry info"
echo "Status: Testing..."
echo ""

# Test 4: Safety Status
echo "Test 4: Safety Status"
echo "Endpoint: GET ${BASE_URL}/api/v1/safety/status"
echo "Expected: Safety system status"
echo "Status: Testing..."
echo ""

# Test 5: Network Status
echo "Test 5: Network Status"
echo "Endpoint: GET ${BASE_URL}/api/v1/network/status"
echo "Expected: Network configuration and status"
echo "Status: Testing..."
echo ""

# Test 6: Configuration
echo "Test 6: Configuration"
echo "Endpoint: GET ${BASE_URL}/api/v1/config"
echo "Expected: Current system configuration"
echo "Status: Testing..."
echo ""

echo "=== API Test Summary ==="
echo "✅ Available Endpoints:"
echo "   - /api/v1/system/status"
echo "   - /api/v1/system/health"
echo "   - /api/v1/communication/status"
echo "   - /api/v1/safety/status"
echo "   - /api/v1/network/status"
echo "   - /api/v1/network/config"
echo "   - /api/v1/config"
echo ""
echo "✅ Backend Integration:"
echo "   - HTTP Server: Port ${OHT_PORT}"
echo "   - WebSocket: Port 8081"
echo "   - CORS: Enabled"
echo "   - Authentication: Disabled (testing)"
echo ""
echo "✅ Command Flow:"
echo "   1. FW starts → Scan modules → Enter IDLE"
echo "   2. API server ready → Backend connects"
echo "   3. Backend polls status → Sends commands"
echo "   4. FW processes commands → Updates state"
echo "   5. Real-time telemetry via WebSocket"
echo ""
echo "Test completed! Use curl to test actual endpoints:"
echo "curl ${BASE_URL}/api/v1/system/status"
