#!/bin/bash
# RPLIDAR C1M1 Test Script - OHT-50
# Test với thông số kỹ thuật chính xác của C1M1

echo "=== RPLIDAR C1M1 Test - OHT-50 ==="
echo "Date: $(date)"
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

# RPLIDAR C1M1 Specifications
BAUD_RATE=460800
SAMPLE_RATE=5000
SCAN_FREQ_TYPICAL=10
ANGULAR_RESOLUTION=0.72
ACCURACY_MM=30
RESOLUTION_MM=15

# RPLIDAR Commands
CMD_GET_INFO="\xA5\x25"
CMD_GET_HEALTH="\xA5\x26"
CMD_START_SCAN="\xA5\x20"
CMD_STOP_SCAN="\xA5\x25"
CMD_RESET="\xA5\x40"

echo "🔧 RPLIDAR C1M1 Specifications:"
echo "  Baud Rate: $BAUD_RATE"
echo "  Sample Rate: ${SAMPLE_RATE}Hz"
echo "  Scan Frequency: ${SCAN_FREQ_TYPICAL}Hz (typical)"
echo "  Angular Resolution: ${ANGULAR_RESOLUTION}°"
echo "  Accuracy: ±${ACCURACY_MM}mm"
echo "  Resolution: ${RESOLUTION_MM}mm"
echo ""

echo "📡 Testing Communication..."

# Configure serial port với baud rate đúng
if stty -F /dev/ttyUSB0 $BAUD_RATE cs8 -cstopb -parenb 2>/dev/null; then
    echo -e "${GREEN}✓ Serial port configured at ${BAUD_RATE} baud${NC}"
else
    echo -e "${RED}✗ Failed to configure serial port${NC}"
    exit 1
fi

# Test 1: Get Device Info
echo ""
echo "Test 1: Get Device Info"
echo "Command: 0xA5 0x25"
echo -e "$CMD_GET_INFO" > /dev/ttyUSB0 2>/dev/null
sleep 0.1
if timeout 2s cat /dev/ttyUSB0 2>/dev/null | hexdump -C > /tmp/lidar_info.log 2>/dev/null; then
    if [ -s /tmp/lidar_info.log ]; then
        echo -e "${GREEN}✓ Device info response received${NC}"
        cat /tmp/lidar_info.log
    else
        echo -e "${RED}✗ No device info response${NC}"
    fi
else
    echo -e "${RED}✗ Device info timeout${NC}"
fi

# Test 2: Get Health Status
echo ""
echo "Test 2: Get Health Status"
echo "Command: 0xA5 0x26"
echo -e "$CMD_GET_HEALTH" > /dev/ttyUSB0 2>/dev/null
sleep 0.1
if timeout 2s cat /dev/ttyUSB0 2>/dev/null | hexdump -C > /tmp/lidar_health.log 2>/dev/null; then
    if [ -s /tmp/lidar_health.log ]; then
        echo -e "${GREEN}✓ Health status response received${NC}"
        cat /tmp/lidar_health.log
    else
        echo -e "${RED}✗ No health status response${NC}"
    fi
else
    echo -e "${RED}✗ Health status timeout${NC}"
fi

# Test 3: Reset Device
echo ""
echo "Test 3: Reset Device"
echo "Command: 0xA5 0x40"
echo -e "$CMD_RESET" > /dev/ttyUSB0 2>/dev/null
sleep 2  # Wait for reset
echo -e "${YELLOW}Device reset completed${NC}"

# Test 4: Get Device Info after reset
echo ""
echo "Test 4: Get Device Info (after reset)"
echo "Command: 0xA5 0x25"
echo -e "$CMD_GET_INFO" > /dev/ttyUSB0 2>/dev/null
sleep 0.1
if timeout 2s cat /dev/ttyUSB0 2>/dev/null | hexdump -C > /tmp/lidar_info_after_reset.log 2>/dev/null; then
    if [ -s /tmp/lidar_info_after_reset.log ]; then
        echo -e "${GREEN}✓ Device info response received after reset${NC}"
        cat /tmp/lidar_info_after_reset.log
    else
        echo -e "${RED}✗ No device info response after reset${NC}"
    fi
else
    echo -e "${RED}✗ Device info timeout after reset${NC}"
fi

# Test 5: Start Scan
echo ""
echo "Test 5: Start Scan"
echo "Command: 0xA5 0x20"
echo -e "$CMD_START_SCAN" > /dev/ttyUSB0 2>/dev/null
sleep 0.1
echo -e "${YELLOW}Scan started, monitoring for 5 seconds...${NC}"

# Monitor scan data for 5 seconds
timeout 5s cat /dev/ttyUSB0 2>/dev/null | hexdump -C > /tmp/lidar_scan.log 2>/dev/null &
SCAN_PID=$!
wait $SCAN_PID 2>/dev/null

if [ -s /tmp/lidar_scan.log ]; then
    echo -e "${GREEN}✓ Scan data received${NC}"
    echo "Sample scan data:"
    head -10 /tmp/lidar_scan.log
else
    echo -e "${RED}✗ No scan data received${NC}"
fi

# Stop scan
echo ""
echo "Test 6: Stop Scan"
echo "Command: 0xA5 0x25"
echo -e "$CMD_STOP_SCAN" > /dev/ttyUSB0 2>/dev/null
echo -e "${YELLOW}Scan stopped${NC}"

# Cleanup
rm -f /tmp/lidar_*.log

echo ""
echo "🎯 Test Summary:"
echo "RPLIDAR C1M1 communication test completed."
echo "Check results above for device responses."
