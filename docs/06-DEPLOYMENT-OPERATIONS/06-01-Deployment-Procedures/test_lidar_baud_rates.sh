#!/bin/bash
# Test LiDAR với different baud rates
# RPLIDAR A1M8 thường sử dụng 115200, nhưng có thể cần test các rates khác

echo "=== LiDAR Baud Rate Test ==="
echo "Date: $(date)"
echo ""

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Test baud rates
BAUD_RATES=(9600 19200 38400 57600 115200 230400 460800 921600)

# RPLIDAR commands
CMD_GET_INFO="\xA5\x25"
CMD_GET_HEALTH="\xA5\x26"
CMD_START_SCAN="\xA5\x20"
CMD_STOP_SCAN="\xA5\x25"

echo "Testing different baud rates..."
echo ""

for baud in "${BAUD_RATES[@]}"; do
    echo -e "${YELLOW}Testing baud rate: $baud${NC}"
    
    # Configure serial port
    if stty -F /dev/ttyUSB0 $baud cs8 -cstopb -parenb 2>/dev/null; then
        echo "  ✓ Port configured"
        
        # Test device info command
        echo -e "$CMD_GET_INFO" > /dev/ttyUSB0 2>/dev/null
        
        # Try to read response
        sleep 0.1
        if timeout 1s cat /dev/ttyUSB0 2>/dev/null | hexdump -C | head -3 > /tmp/lidar_test_$baud.log 2>/dev/null; then
            if [ -s /tmp/lidar_test_$baud.log ]; then
                echo -e "  ${GREEN}✓ Response received at $baud baud${NC}"
                echo "  Response:"
                cat /tmp/lidar_test_$baud.log
                echo ""
            else
                echo -e "  ${RED}✗ No response at $baud baud${NC}"
            fi
        else
            echo -e "  ${RED}✗ Timeout at $baud baud${NC}"
        fi
        
        # Cleanup
        rm -f /tmp/lidar_test_$baud.log
    else
        echo -e "  ${RED}✗ Failed to configure port at $baud baud${NC}"
    fi
    
    echo ""
done

echo "Baud rate test completed."
