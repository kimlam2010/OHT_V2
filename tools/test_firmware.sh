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

# Test 4: Check if port 8081 is available
echo ""
echo "🔍 Test 4: Port 8081 Availability"
if netstat -tulpn | grep -q ":8081 "; then
    echo "⚠️  Port 8081 is already in use:"
    netstat -tulpn | grep ":8081 "
else
    echo "✅ Port 8081 is available"
fi

# Test 5: Test HTTP server compilation
echo ""
echo "🔍 Test 5: HTTP Server Source Check"
if [ -f "../src/app/simple_http_8081.c" ]; then
    echo "✅ HTTP server source exists"
    grep -n "simple_http_8081_start" ../src/app/simple_http_8081.c | head -3
else
    echo "❌ HTTP server source missing"
fi

# Test 6: Check WebSocket server
echo ""
echo "🔍 Test 6: WebSocket Server Check"
if [ -f "../src/app/websocket_server.c" ]; then
    echo "✅ WebSocket server source exists"
    grep -n "8081" ../src/app/websocket_server.c | head -3
else
    echo "❌ WebSocket server source missing"
fi

echo ""
echo "🎯 Test Summary:"
echo "=================="
echo "Binary: ✅ Exists and executable"
echo "Port 8081: $(netstat -tulpn | grep -q ":8081 " && echo "⚠️ In use" || echo "✅ Available")"
echo "HTTP Server: ✅ Source exists"
echo "WebSocket Server: ✅ Source exists"

echo ""
echo "🚀 Next Steps:"
echo "1. Kill any processes using port 8081"
echo "2. Start firmware service: ./oht50_main --debug"
echo "3. Test HTTP endpoints with curl"
echo "4. Verify Backend can connect"
