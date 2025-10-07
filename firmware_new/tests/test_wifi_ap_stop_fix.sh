#!/bin/bash
# Test script to verify WiFi AP Stop API fix
# Issue: #193, #194
# Date: 2025-10-07

set -e

echo "=========================================="
echo "🧪 WiFi AP Stop API Fix Verification Test"
echo "=========================================="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test configuration
FIRMWARE_URL="http://localhost:8081"
AP_START_API="${FIRMWARE_URL}/api/v1/network/ap/start"
AP_STOP_API="${FIRMWARE_URL}/api/v1/network/ap/stop"
AP_STATUS_API="${FIRMWARE_URL}/api/v1/network/ap/status"

# Test 1: Start WiFi AP
echo "📌 Test 1: Starting WiFi AP..."
START_RESPONSE=$(curl -s -X POST "${AP_START_API}" \
  -H "Content-Type: application/json" \
  -d '{
    "ssid": "OHT-50-Test",
    "password": "test12345",
    "security_type": "WPA2",
    "channel": 6,
    "ip_address": "192.168.4.1",
    "netmask": "255.255.255.0",
    "max_clients": 10
  }')

echo "Response: ${START_RESPONSE}"

SUCCESS=$(echo "${START_RESPONSE}" | jq -r '.success')
if [ "${SUCCESS}" != "true" ]; then
    echo -e "${RED}❌ Failed to start WiFi AP${NC}"
    exit 1
fi

echo -e "${GREEN}✅ WiFi AP started successfully${NC}"
echo ""

# Wait for AP to initialize
echo "⏳ Waiting 3 seconds for AP to initialize..."
sleep 3

# Test 2: Verify processes are running
echo "📌 Test 2: Verifying hostapd and dnsmasq are running..."

HOSTAPD_PID=$(pgrep -f hostapd || echo "")
DNSMASQ_PID=$(pgrep -f 'dnsmasq.*wlan0' || echo "")

if [ -z "${HOSTAPD_PID}" ]; then
    echo -e "${YELLOW}⚠️  Warning: hostapd process not found (may be mock)${NC}"
else
    echo -e "${GREEN}✅ hostapd running with PID: ${HOSTAPD_PID}${NC}"
fi

if [ -z "${DNSMASQ_PID}" ]; then
    echo -e "${YELLOW}⚠️  Warning: dnsmasq process not found (may be mock)${NC}"
else
    echo -e "${GREEN}✅ dnsmasq running with PID: ${DNSMASQ_PID}${NC}"
fi
echo ""

# Test 3: Stop WiFi AP
echo "📌 Test 3: Stopping WiFi AP..."
STOP_RESPONSE=$(curl -s -X POST "${AP_STOP_API}")

echo "Response: ${STOP_RESPONSE}"

SUCCESS=$(echo "${STOP_RESPONSE}" | jq -r '.success')
if [ "${SUCCESS}" != "true" ]; then
    echo -e "${RED}❌ Failed to stop WiFi AP${NC}"
    exit 1
fi

echo -e "${GREEN}✅ WiFi AP stop API returned success${NC}"
echo ""

# Wait for cleanup
echo "⏳ Waiting 2 seconds for cleanup..."
sleep 2

# Test 4: Verify processes are STOPPED (CRITICAL TEST!)
echo "📌 Test 4: 🔴 CRITICAL - Verifying processes are ACTUALLY STOPPED..."

HOSTAPD_AFTER=$(pgrep -f hostapd || echo "")
DNSMASQ_AFTER=$(pgrep -f 'dnsmasq.*wlan0' || echo "")

TEST_PASSED=true

if [ -n "${HOSTAPD_AFTER}" ]; then
    echo -e "${RED}❌ FAIL: hostapd process still running! PID: ${HOSTAPD_AFTER}${NC}"
    ps aux | grep hostapd | grep -v grep
    TEST_PASSED=false
else
    echo -e "${GREEN}✅ PASS: hostapd process stopped${NC}"
fi

if [ -n "${DNSMASQ_AFTER}" ]; then
    echo -e "${RED}❌ FAIL: dnsmasq process still running! PID: ${DNSMASQ_AFTER}${NC}"
    ps aux | grep dnsmasq | grep -v grep
    TEST_PASSED=false
else
    echo -e "${GREEN}✅ PASS: dnsmasq process stopped${NC}"
fi
echo ""

# Test 5: Verify PID files cleaned up
echo "📌 Test 5: Verifying PID files are cleaned up..."

PID_FILES_EXIST=false

if [ -f "/tmp/hostapd.pid" ]; then
    echo -e "${RED}❌ FAIL: /tmp/hostapd.pid still exists${NC}"
    PID_FILES_EXIST=true
else
    echo -e "${GREEN}✅ PASS: /tmp/hostapd.pid cleaned up${NC}"
fi

if [ -f "/tmp/dnsmasq.pid" ]; then
    echo -e "${RED}❌ FAIL: /tmp/dnsmasq.pid still exists${NC}"
    PID_FILES_EXIST=true
else
    echo -e "${GREEN}✅ PASS: /tmp/dnsmasq.pid cleaned up${NC}"
fi
echo ""

# Test 6: Verify API status reflects stopped state
echo "📌 Test 6: Verifying API status..."
STATUS_RESPONSE=$(curl -s -X GET "${AP_STATUS_API}")

echo "Response: ${STATUS_RESPONSE}"

AP_ENABLED=$(echo "${STATUS_RESPONSE}" | jq -r '.data.ap_enabled')
if [ "${AP_ENABLED}" != "false" ]; then
    echo -e "${RED}❌ FAIL: API shows ap_enabled=${AP_ENABLED}, expected false${NC}"
    TEST_PASSED=false
else
    echo -e "${GREEN}✅ PASS: API correctly shows ap_enabled=false${NC}"
fi
echo ""

# Final Result
echo "=========================================="
echo "📊 TEST SUMMARY"
echo "=========================================="

if [ "${TEST_PASSED}" = true ] && [ "${PID_FILES_EXIST}" = false ]; then
    echo -e "${GREEN}✅ ALL TESTS PASSED!${NC}"
    echo ""
    echo "✅ WiFi AP Stop API now correctly kills processes"
    echo "✅ Issue #193 and #194 are FIXED!"
    echo ""
    exit 0
else
    echo -e "${RED}❌ TESTS FAILED!${NC}"
    echo ""
    echo "❌ WiFi AP Stop API still has issues"
    echo "❌ Issue #193 and #194 are NOT fixed"
    echo ""
    
    echo "🔧 Manual cleanup (if needed):"
    echo "   sudo pkill hostapd"
    echo "   sudo pkill dnsmasq"
    echo "   rm -f /tmp/hostapd.pid /tmp/dnsmasq.pid"
    echo ""
    exit 1
fi

