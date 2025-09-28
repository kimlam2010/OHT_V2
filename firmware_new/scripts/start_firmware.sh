#!/bin/bash

# OHT-50 Firmware Startup Script
# Fixes Issue #98: Firmware HTTP Server Not Working

echo "🚀 Starting OHT-50 Firmware Service..."

# Change to firmware directory
cd /home/orangepi/Desktop/OHT_V2/firmware_new/build

# Check if binary exists
if [ ! -f "oht50_main" ]; then
    echo "❌ ERROR: oht50_main binary not found!"
    echo "Please build firmware first:"
    echo "cd /home/orangepi/Desktop/OHT_V2/firmware_new"
    echo "mkdir -p build && cd build"
    echo "cmake .. -DCMAKE_BUILD_TYPE=Debug"
    echo "make -j\$(nproc)"
    exit 1
fi

# Kill any existing instances
echo "🔧 Stopping any existing firmware instances..."
pkill -f oht50_main
sleep 2

# Check if port 8081 is free
if netstat -tulpn | grep -q ":8081 "; then
    echo "⚠️  WARNING: Port 8081 is already in use"
    echo "Current processes using port 8081:"
    netstat -tulpn | grep ":8081 "
    echo "Killing processes on port 8081..."
    sudo fuser -k 8081/tcp
    sleep 2
fi

# Start firmware service
echo "🚀 Starting firmware service..."
echo "Command: ./oht50_main --debug"
echo "=================================="

# Start in background with output redirected to log
./oht50_main --debug > /tmp/oht50_firmware.log 2>&1 &
FIRMWARE_PID=$!

echo "📝 Firmware PID: $FIRMWARE_PID"
echo "📝 Log file: /tmp/oht50_firmware.log"

# Wait a moment for startup
sleep 3

# Check if process is running
if ps -p $FIRMWARE_PID > /dev/null; then
    echo "✅ Firmware service started successfully!"
else
    echo "❌ Firmware service failed to start!"
    echo "Check log file: /tmp/oht50_firmware.log"
    echo "Last 20 lines of log:"
    tail -20 /tmp/oht50_firmware.log
    exit 1
fi

# Check if port 8081 is listening
echo "🔍 Checking if port 8081 is listening..."
sleep 2

if netstat -tulpn | grep -q ":8081 "; then
    echo "✅ Port 8081 is listening!"
    netstat -tulpn | grep ":8081 "
else
    echo "❌ Port 8081 is not listening!"
    echo "Check log file: /tmp/oht50_firmware.log"
    exit 1
fi

# Test HTTP endpoints
echo "🧪 Testing HTTP endpoints..."

echo "Testing /health endpoint..."
HEALTH_RESPONSE=$(curl -s -w "%{http_code}" http://localhost:8081/health 2>/dev/null)
if [ "$HEALTH_RESPONSE" = "200" ]; then
    echo "✅ /health endpoint working!"
else
    echo "❌ /health endpoint failed (HTTP: $HEALTH_RESPONSE)"
fi

echo "Testing /api/v1/status endpoint..."
STATUS_RESPONSE=$(curl -s -w "%{http_code}" http://localhost:8081/api/v1/status 2>/dev/null)
if [ "$STATUS_RESPONSE" = "200" ]; then
    echo "✅ /api/v1/status endpoint working!"
else
    echo "❌ /api/v1/status endpoint failed (HTTP: $STATUS_RESPONSE)"
fi

echo "Testing /api/v1/robot/status endpoint..."
ROBOT_RESPONSE=$(curl -s -w "%{http_code}" http://localhost:8081/api/v1/robot/status 2>/dev/null)
if [ "$ROBOT_RESPONSE" = "200" ]; then
    echo "✅ /api/v1/robot/status endpoint working!"
else
    echo "❌ /api/v1/robot/status endpoint failed (HTTP: $ROBOT_RESPONSE)"
fi

echo ""
echo "🎉 Firmware service startup complete!"
echo "📊 Service Status:"
echo "   PID: $FIRMWARE_PID"
echo "   Port: 8081"
echo "   Log: /tmp/oht50_firmware.log"
echo ""
echo "🔧 Useful commands:"
echo "   Stop service: pkill -f oht50_main"
echo "   View logs: tail -f /tmp/oht50_firmware.log"
echo "   Test health: curl http://localhost:8081/health"
echo "   Test status: curl http://localhost:8081/api/v1/status"
