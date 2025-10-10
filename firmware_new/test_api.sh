#!/bin/bash

echo "=== Starting Firmware Test ==="

# Kill old processes
pkill -9 oht50_main 2>/dev/null
sleep 2

# Start firmware
cd /home/orangepi/Desktop/OHT_V2/firmware_new/build
./oht50_main > /tmp/oht50.log 2>&1 &
PID=$!
echo "Firmware started with PID: $PID"

# Wait for startup
echo "Waiting for firmware to initialize..."
sleep 10

# Test API endpoints
echo ""
echo "=== TEST 1: Health Check ==="
curl -s http://localhost:8080/health | head -5

echo ""
echo ""
echo "=== TEST 2: Get Power Module Registers ==="
curl -s http://localhost:8080/api/v1/modules/2/registers | head -80

echo ""
echo ""
echo "=== TEST 3: Get Single Register ==="
curl -s http://localhost:8080/api/v1/modules/2/registers/0x0000

echo ""
echo ""
echo "=== Check log for errors ==="
tail -30 /tmp/oht50.log | grep -E "(error|Error|ERROR|Register Metadata|✅)"

echo ""
echo "=== Test Complete ==="
echo "Firmware PID: $PID (use 'kill $PID' to stop)"

