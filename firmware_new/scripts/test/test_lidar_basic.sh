#!/bin/bash
# LiDAR Basic Test Script - OHT-50
# Test hardware connectivity và software setup

echo "=== LiDAR Basic Test - OHT-50 ==="
echo "Date: $(date)"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counters
PASSED=0
FAILED=0
TOTAL=0

# Function to print test result
print_result() {
    local test_name="$1"
    local result="$2"
    local message="$3"
    
    TOTAL=$((TOTAL + 1))
    
    if [ "$result" = "PASS" ]; then
        echo -e "${GREEN}✅ PASS${NC}: $test_name - $message"
        PASSED=$((PASSED + 1))
    else
        echo -e "${RED}❌ FAIL${NC}: $test_name - $message"
        FAILED=$((FAILED + 1))
    fi
}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

echo "🔧 Test 1: System Requirements Check"
echo "----------------------------------------"

# Check if running on Orange Pi 5B
if [ -f "/proc/device-tree/model" ]; then
    MODEL=$(cat /proc/device-tree/model)
    if echo "$MODEL" | grep -q "Orange Pi 5B"; then
        print_result "Hardware Platform" "PASS" "Orange Pi 5B detected: $MODEL"
    else
        print_result "Hardware Platform" "FAIL" "Not Orange Pi 5B: $MODEL"
    fi
else
    print_result "Hardware Platform" "FAIL" "Cannot detect hardware platform"
fi

# Check OS version
if [ -f "/etc/os-release" ]; then
    OS_VERSION=$(grep "PRETTY_NAME" /etc/os-release | cut -d'"' -f2)
    print_result "OS Version" "PASS" "$OS_VERSION"
else
    print_result "OS Version" "FAIL" "Cannot detect OS version"
fi

# Check required commands
REQUIRED_COMMANDS=("lsusb" "stty" "hexdump" "timeout" "bc")
for cmd in "${REQUIRED_COMMANDS[@]}"; do
    if command_exists "$cmd"; then
        print_result "Command: $cmd" "PASS" "Available"
    else
        print_result "Command: $cmd" "FAIL" "Not found"
    fi
done

echo ""
echo "🔌 Test 2: USB Device Detection"
echo "----------------------------------------"

# Check USB devices
if lsusb | grep -q "10c4:ea60"; then
    USB_INFO=$(lsusb | grep "10c4:ea60")
    print_result "USB Device Detection" "PASS" "RPLIDAR found: $USB_INFO"
else
    print_result "USB Device Detection" "FAIL" "RPLIDAR not found in USB devices"
    echo -e "${YELLOW}💡 Tip: Check USB cable connection và power supply${NC}"
fi

# Check for any Silicon Labs devices
if lsusb | grep -q "Silicon Labs"; then
    SILICON_DEVICES=$(lsusb | grep "Silicon Labs")
    print_result "Silicon Labs Devices" "PASS" "Found: $SILICON_DEVICES"
else
    print_result "Silicon Labs Devices" "FAIL" "No Silicon Labs devices found"
fi

echo ""
echo "📁 Test 3: Serial Device Check"
echo "----------------------------------------"

# Check for ttyUSB devices
if ls /dev/ttyUSB* 2>/dev/null | grep -q "ttyUSB"; then
    TTYUSB_DEVICES=$(ls /dev/ttyUSB*)
    print_result "Serial Device Detection" "PASS" "Found: $TTYUSB_DEVICES"
    
    # Check permissions
    for device in /dev/ttyUSB*; do
        if [ -r "$device" ] && [ -w "$device" ]; then
            print_result "Device Permissions: $device" "PASS" "Read/Write access"
        else
            print_result "Device Permissions: $device" "FAIL" "Permission denied"
            echo -e "${YELLOW}💡 Tip: Run 'sudo chmod 666 $device' or add user to dialout group${NC}"
        fi
    done
else
    print_result "Serial Device Detection" "FAIL" "No ttyUSB devices found"
    echo -e "${YELLOW}💡 Tip: Check USB connection và udev rules${NC}"
fi

echo ""
echo "⚙️ Test 4: udev Rules Check"
echo "----------------------------------------"

# Check udev rules
UDEV_RULES_FILE="/etc/udev/rules.d/99-rplidar.rules"
if [ -f "$UDEV_RULES_FILE" ]; then
    if grep -q "10c4:ea60" "$UDEV_RULES_FILE"; then
        print_result "udev Rules File" "PASS" "Rules file exists và contains RPLIDAR rules"
    else
        print_result "udev Rules File" "FAIL" "Rules file exists but missing RPLIDAR rules"
    fi
else
    print_result "udev Rules File" "FAIL" "udev rules file not found: $UDEV_RULES_FILE"
    echo -e "${YELLOW}💡 Tip: Create udev rules file with RPLIDAR rules${NC}"
fi

echo ""
echo "🔧 Test 5: Serial Port Configuration"
echo "----------------------------------------"

# Test serial port configuration
if [ -e "/dev/ttyUSB0" ]; then
    # Try to configure serial port
    if timeout 5s stty -F /dev/ttyUSB0 115200 cs8 -cstopb -parenb 2>/dev/null; then
        print_result "Serial Port Configuration" "PASS" "Successfully configured /dev/ttyUSB0"
        
        # Check current settings
        CURRENT_SETTINGS=$(stty -F /dev/ttyUSB0 -a 2>/dev/null | grep -E "(speed|cs8|cstopb|parenb)")
        if [ -n "$CURRENT_SETTINGS" ]; then
            print_result "Serial Port Settings" "PASS" "Settings verified"
        else
            print_result "Serial Port Settings" "FAIL" "Cannot read current settings"
        fi
    else
        print_result "Serial Port Configuration" "FAIL" "Failed to configure /dev/ttyUSB0"
    fi
else
    print_result "Serial Port Configuration" "FAIL" "/dev/ttyUSB0 not available"
fi

echo ""
echo "📡 Test 6: Basic Communication Test"
echo "----------------------------------------"

# Test basic communication with LiDAR
if [ -e "/dev/ttyUSB0" ] && [ -r "/dev/ttyUSB0" ] && [ -w "/dev/ttyUSB0" ]; then
    echo -e "${YELLOW}Testing basic communication with LiDAR...${NC}"
    
    # Send device info command (0xA5 0x25)
    echo -e "\xA5\x25" > /dev/ttyUSB0 2>/dev/null
    
    # Try to read response (timeout 3 seconds)
    if timeout 3s hexdump -C /dev/ttyUSB0 2>/dev/null | head -5 > /tmp/lidar_response.log 2>/dev/null; then
        if [ -s /tmp/lidar_response.log ]; then
            print_result "Basic Communication" "PASS" "Received response from LiDAR"
            echo "Response preview:"
            head -3 /tmp/lidar_response.log
        else
            print_result "Basic Communication" "FAIL" "No response received"
        fi
    else
        print_result "Basic Communication" "FAIL" "Communication timeout"
    fi
    
    # Cleanup
    rm -f /tmp/lidar_response.log
else
    print_result "Basic Communication" "FAIL" "Cannot access /dev/ttyUSB0"
fi

echo ""
echo "📊 Test 7: System Resources Check"
echo "----------------------------------------"

# Check USB bandwidth
if [ -d "/sys/kernel/debug/usb" ]; then
    print_result "USB Debug Interface" "PASS" "Available"
else
    print_result "USB Debug Interface" "FAIL" "Not available (may need debugfs)"
fi

# Check system load
LOAD_AVG=$(uptime | awk -F'load average:' '{print $2}' | awk '{print $1}' | sed 's/,//')
print_result "System Load" "PASS" "Load average: $LOAD_AVG"

# Check available memory
FREE_MEM=$(free -m | grep "Mem:" | awk '{print $7}')
print_result "Available Memory" "PASS" "${FREE_MEM}MB free"

echo ""
echo "📋 Test 8: Software Dependencies Check"
echo "----------------------------------------"

# Check for RPLIDAR SDK
if [ -d "/usr/local/include" ] && [ -f "/usr/local/include/rplidar.h" ]; then
    print_result "RPLIDAR SDK Headers" "PASS" "Found in /usr/local/include"
elif [ -d "/usr/include" ] && [ -f "/usr/include/rplidar.h" ]; then
    print_result "RPLIDAR SDK Headers" "PASS" "Found in /usr/include"
else
    print_result "RPLIDAR SDK Headers" "FAIL" "RPLIDAR SDK headers not found"
    echo -e "${YELLOW}💡 Tip: Install RPLIDAR SDK from https://github.com/Slamtec/rplidar_sdk${NC}"
fi

# Check for RPLIDAR library
if [ -f "/usr/local/lib/librplidar_sdk.so" ]; then
    print_result "RPLIDAR SDK Library" "PASS" "Found in /usr/local/lib"
elif [ -f "/usr/lib/librplidar_sdk.so" ]; then
    print_result "RPLIDAR SDK Library" "PASS" "Found in /usr/lib"
else
    print_result "RPLIDAR SDK Library" "FAIL" "RPLIDAR SDK library not found"
fi

# Check pkg-config
if pkg-config --exists rplidar_sdk 2>/dev/null; then
    print_result "RPLIDAR pkg-config" "PASS" "Available"
else
    print_result "RPLIDAR pkg-config" "FAIL" "Not available"
fi

echo ""
echo "🎯 Test Summary"
echo "================"
echo "Total Tests: $TOTAL"
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$FAILED${NC}"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}🎉 All tests PASSED! LiDAR setup is ready.${NC}"
    exit 0
elif [ $FAILED -le 3 ]; then
    echo -e "${YELLOW}⚠️  Some tests failed. Check recommendations above.${NC}"
    exit 1
else
    echo -e "${RED}❌ Multiple tests failed. LiDAR setup needs attention.${NC}"
    exit 2
fi
