#!/bin/bash

echo "🔍 PORT 8081 DEBUG SCRIPT"

echo "1. Kill old firmware:"
sudo pkill -9 -f oht50_main
sleep 2

echo "2. Check ports free:"
netstat -tln | grep -E "(8080|8081)" || echo "Ports are free"

echo "3. Start firmware in background:"
cd /home/orangepi/Desktop/OHT_V2/firmware_new/build
sudo ./oht50_main --debug > debug_8081.log 2>&1 &
FIRMWARE_PID=$!
echo "Firmware PID: $FIRMWARE_PID"

echo "4. Wait for startup:"
sleep 8

echo "5. Check if ports are listening:"
netstat -tln | grep -E "(8080|8081)"

echo "6. Test port 8080 first (should work):"
timeout 3 curl -s http://localhost:8080/health | python3 -m json.tool || echo "Port 8080 failed"

echo "7. Test port 8081 with timeout:"
timeout 2 curl -s http://localhost:8081/health || echo "Port 8081 HANGS"

echo "8. Check WebSocket debug logs:"
grep "WS_DEBUG" debug_8081.log | tail -5

echo "9. Kill firmware:"
sudo kill -9 $FIRMWARE_PID

echo "🏁 Debug completed"
