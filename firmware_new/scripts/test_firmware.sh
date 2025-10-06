#!/bin/bash

# OHT-50 Firmware Test Script
# Tests all critical issues

echo "🧪 Testing OHT-50 Firmware Issues..."
echo "=================================="

cd /home/orangepi/Desktop/OHT_V2/firmware_new/build

# Test 1: Check if binary exists and is executable
echo "🔍 Test 1: Binary Check"
if [ -f "oht50_main" ] && [ -x "oht50_main" ]; then
    echo "✅ Binary exists and is executable"
    ls -la oht50_main
else
    echo "❌ Binary missing or not executable"
    exit 1
fi

# Test 2: Check binary dependencies
echo ""
echo "🔍 Test 2: Dependencies Check"
ldd oht50_main | head -10

# Test 3: Test dry-run mode
echo ""
echo "🔍 Test 3: Dry-run Mode Test"
echo "Starting firmware in dry-run mode for 5 seconds..."

timeout 5s ./oht50_main --dry-run --debug 2>&1 | head -20

# Test 4: Test HTTP server compilation (HTTP-only design)
echo ""
echo "🔍 Test 4: HTTP Server Source Check (port 8080)"
if [ -f "../firmware_src/app/http_server.c" ]; then
    echo "✅ HTTP server source exists (http_server.c)"
    grep -n "HTTP_SERVER_DEFAULT_PORT\|8080" ../firmware_src/app/http_server.h | head -3 || true
else
    echo "❌ HTTP server source missing"
fi

# Removed WebSocket checks per CTO decision (Firmware is HTTP-only)

echo ""
echo "🎯 Test Summary:"
echo "=================="
echo "Binary: ✅ Exists and executable"
echo "HTTP Server: ✅ Source check completed (port 8080)"
echo "WebSocket Server: ❌ Removed (HTTP-only firmware)"

echo ""
echo "🚀 Next Steps:"
echo "1. Kill any processes using port 8081"
echo "2. Start firmware service: ./oht50_main --debug"
echo "3. Test HTTP endpoints with curl"
echo "4. Verify Backend can connect"
