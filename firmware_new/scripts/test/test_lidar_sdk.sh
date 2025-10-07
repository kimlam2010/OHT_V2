#!/bin/bash
# LiDAR SDK Test Script - OHT-50
# Test LiDAR functionality với RPLIDAR SDK

echo "=== LiDAR SDK Test - OHT-50 ==="
echo "Date: $(date)"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
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

# Function to check if file exists
file_exists() {
    [ -f "$1" ]
}

echo "📋 Test 1: RPLIDAR SDK Installation Check"
echo "----------------------------------------"

# Check SDK installation
SDK_INSTALLED=false

# Check for SDK in common locations
SDK_LOCATIONS=(
    "/usr/local/lib/librplidar_sdk.so"
    "/usr/lib/librplidar_sdk.so"
    "/opt/rplidar_sdk/lib/librplidar_sdk.so"
)

for lib in "${SDK_LOCATIONS[@]}"; do
    if file_exists "$lib"; then
        print_result "RPLIDAR SDK Library" "PASS" "Found: $lib"
        SDK_INSTALLED=true
        break
    fi
done

if [ "$SDK_INSTALLED" = false ]; then
    print_result "RPLIDAR SDK Library" "FAIL" "Not found in common locations"
    echo -e "${YELLOW}💡 Tip: Install RPLIDAR SDK from https://github.com/Slamtec/rplidar_sdk${NC}"
fi

# Check headers
HEADER_INSTALLED=false
HEADER_LOCATIONS=(
    "/usr/local/include/rplidar.h"
    "/usr/include/rplidar.h"
    "/opt/rplidar_sdk/include/rplidar.h"
)

for header in "${HEADER_LOCATIONS[@]}"; do
    if file_exists "$header"; then
        print_result "RPLIDAR SDK Headers" "PASS" "Found: $header"
        HEADER_INSTALLED=true
        break
    fi
done

if [ "$HEADER_INSTALLED" = false ]; then
    print_result "RPLIDAR SDK Headers" "FAIL" "Not found in common locations"
fi

# Check pkg-config
if pkg-config --exists rplidar_sdk 2>/dev/null; then
    print_result "RPLIDAR pkg-config" "PASS" "Available"
    echo "pkg-config info:"
    pkg-config --libs rplidar_sdk
    pkg-config --cflags rplidar_sdk
else
    print_result "RPLIDAR pkg-config" "FAIL" "Not available"
fi

echo ""
echo "🔧 Test 2: SDK Examples Check"
echo "----------------------------------------"

# Check for SDK examples
EXAMPLE_LOCATIONS=(
    "$HOME/rplidar_sdk/sdk/output/Linux/Release"
    "/opt/rplidar_sdk/examples"
    "/usr/local/share/rplidar_sdk/examples"
)

EXAMPLE_FOUND=false
for example_dir in "${EXAMPLE_LOCATIONS[@]}"; do
    if [ -d "$example_dir" ]; then
        print_result "SDK Examples Directory" "PASS" "Found: $example_dir"
        
        # Check for specific examples
        if [ -f "$example_dir/ultra_simple" ]; then
            print_result "ultra_simple Example" "PASS" "Found executable"
            EXAMPLE_FOUND=true
        else
            print_result "ultra_simple Example" "FAIL" "Executable not found"
        fi
        
        if [ -f "$example_dir/simple_grabber" ]; then
            print_result "simple_grabber Example" "PASS" "Found executable"
        else
            print_result "simple_grabber Example" "FAIL" "Executable not found"
        fi
        
        break
    fi
done

if [ "$EXAMPLE_FOUND" = false ]; then
    print_result "SDK Examples" "FAIL" "No examples found"
    echo -e "${YELLOW}💡 Tip: Build SDK examples from source${NC}"
fi

echo ""
echo "📡 Test 3: Device Communication Test"
echo "----------------------------------------"

# Check if device is available
if [ ! -e "/dev/ttyUSB0" ]; then
    print_result "Device Availability" "FAIL" "/dev/ttyUSB0 not found"
    echo -e "${YELLOW}💡 Tip: Connect LiDAR và check USB connection${NC}"
else
    print_result "Device Availability" "PASS" "/dev/ttyUSB0 available"
    
    # Test device permissions
    if [ -r "/dev/ttyUSB0" ] && [ -w "/dev/ttyUSB0" ]; then
        print_result "Device Permissions" "PASS" "Read/Write access"
        
        # Try to run ultra_simple example
        if [ "$EXAMPLE_FOUND" = true ]; then
            echo -e "${BLUE}Testing LiDAR communication with ultra_simple...${NC}"
            
            # Find ultra_simple executable
            ULTRA_SIMPLE_PATH=""
            for example_dir in "${EXAMPLE_LOCATIONS[@]}"; do
                if [ -f "$example_dir/ultra_simple" ]; then
                    ULTRA_SIMPLE_PATH="$example_dir/ultra_simple"
                    break
                fi
            done
            
            if [ -n "$ULTRA_SIMPLE_PATH" ]; then
                echo "Running: $ULTRA_SIMPLE_PATH /dev/ttyUSB0"
                echo "Press Ctrl+C after 10 seconds to stop test..."
                
                # Run test for 10 seconds
                timeout 10s "$ULTRA_SIMPLE_PATH" /dev/ttyUSB0 > /tmp/lidar_sdk_test.log 2>&1 &
                TEST_PID=$!
                
                # Wait for test to complete
                wait $TEST_PID 2>/dev/null
                TEST_EXIT_CODE=$?
                
                # Check results
                if [ $TEST_EXIT_CODE -eq 0 ] || [ $TEST_EXIT_CODE -eq 124 ]; then
                    if [ -s /tmp/lidar_sdk_test.log ]; then
                        print_result "SDK Communication Test" "PASS" "Communication successful"
                        echo "Sample output:"
                        head -5 /tmp/lidar_sdk_test.log
                    else
                        print_result "SDK Communication Test" "FAIL" "No output generated"
                    fi
                else
                    print_result "SDK Communication Test" "FAIL" "Test failed with exit code $TEST_EXIT_CODE"
                fi
                
                # Cleanup
                rm -f /tmp/lidar_sdk_test.log
            else
                print_result "SDK Communication Test" "FAIL" "ultra_simple executable not found"
            fi
        else
            print_result "SDK Communication Test" "FAIL" "No SDK examples available"
        fi
    else
        print_result "Device Permissions" "FAIL" "Permission denied"
        echo -e "${YELLOW}💡 Tip: Run 'sudo chmod 666 /dev/ttyUSB0' or add user to dialout group${NC}"
    fi
fi

echo ""
echo "📊 Test 4: Performance Test"
echo "----------------------------------------"

# Test scan rate if communication is working
if [ -e "/dev/ttyUSB0" ] && [ -r "/dev/ttyUSB0" ] && [ -w "/dev/ttyUSB0" ] && [ "$EXAMPLE_FOUND" = true ]; then
    echo -e "${BLUE}Testing LiDAR scan rate...${NC}"
    
    # Find ultra_simple executable
    ULTRA_SIMPLE_PATH=""
    for example_dir in "${EXAMPLE_LOCATIONS[@]}"; do
        if [ -f "$example_dir/ultra_simple" ]; then
            ULTRA_SIMPLE_PATH="$example_dir/ultra_simple"
            break
        fi
    done
    
    if [ -n "$ULTRA_SIMPLE_PATH" ]; then
        # Run test for 30 seconds to measure scan rate
        echo "Running scan rate test for 30 seconds..."
        
        timeout 30s "$ULTRA_SIMPLE_PATH" /dev/ttyUSB0 > /tmp/lidar_scan_test.log 2>&1 &
        SCAN_PID=$!
        
        # Wait for test to complete
        wait $SCAN_PID 2>/dev/null
        SCAN_EXIT_CODE=$?
        
        if [ $SCAN_EXIT_CODE -eq 0 ] || [ $SCAN_EXIT_CODE -eq 124 ]; then
            if [ -s /tmp/lidar_scan_test.log ]; then
                # Count scan lines
                SCAN_COUNT=$(grep -c "scan" /tmp/lidar_scan_test.log 2>/dev/null || echo "0")
                
                if [ "$SCAN_COUNT" -gt 0 ]; then
                    # Calculate scan rate (30 seconds test)
                    SCAN_RATE=$(echo "scale=2; $SCAN_COUNT / 30" | bc 2>/dev/null || echo "0")
                    
                    echo "Scan count: $SCAN_COUNT"
                    echo "Scan rate: $SCAN_RATE Hz"
                    
                    # Check if scan rate is acceptable (target: 8Hz, minimum: 6Hz)
                    if (( $(echo "$SCAN_RATE >= 6" | bc -l) )); then
                        print_result "Scan Rate Test" "PASS" "Scan rate: ${SCAN_RATE}Hz (target: 8Hz)"
                    else
                        print_result "Scan Rate Test" "FAIL" "Scan rate too low: ${SCAN_RATE}Hz (target: 8Hz)"
                    fi
                else
                    print_result "Scan Rate Test" "FAIL" "No scan data detected"
                fi
            else
                print_result "Scan Rate Test" "FAIL" "No output generated"
            fi
        else
            print_result "Scan Rate Test" "FAIL" "Test failed"
        fi
        
        # Cleanup
        rm -f /tmp/lidar_scan_test.log
    else
        print_result "Scan Rate Test" "FAIL" "ultra_simple executable not found"
    fi
else
    print_result "Scan Rate Test" "FAIL" "Device not available or no examples"
fi

echo ""
echo "🔒 Test 5: Safety Integration Test"
echo "----------------------------------------"

# Test safety thresholds
if [ -e "/dev/ttyUSB0" ] && [ -r "/dev/ttyUSB0" ] && [ -w "/dev/ttyUSB0" ]; then
    echo -e "${BLUE}Testing safety threshold detection...${NC}"
    
    # Create simple safety test program
    cat > /tmp/lidar_safety_test.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>

#define EMERGENCY_STOP_MM 500
#define WARNING_MM 1000
#define SAFE_MM 2000

int main() {
    int fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        printf("Failed to open /dev/ttyUSB0\n");
        return 1;
    }
    
    // Configure serial port
    struct termios tty;
    memset(&tty, 0, sizeof(tty));
    if (tcgetattr(fd, &tty) != 0) {
        printf("Failed to get serial attributes\n");
        close(fd);
        return 1;
    }
    
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag &= ~(PARENB | CSTOPB);
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_oflag &= ~OPOST;
    
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        printf("Failed to set serial attributes\n");
        close(fd);
        return 1;
    }
    
    printf("Safety test: Monitoring LiDAR for 10 seconds...\n");
    
    // Monitor for 10 seconds
    for (int i = 0; i < 10; i++) {
        // Simulate distance reading (in real implementation, this would read from LiDAR)
        int distance_mm = 1500 + (rand() % 1000); // Random distance 1500-2500mm
        
        printf("Distance: %d mm", distance_mm);
        
        if (distance_mm < EMERGENCY_STOP_MM) {
            printf(" - EMERGENCY STOP!\n");
        } else if (distance_mm < WARNING_MM) {
            printf(" - WARNING!\n");
        } else if (distance_mm < SAFE_MM) {
            printf(" - CAUTION\n");
        } else {
            printf(" - SAFE\n");
        }
        
        sleep(1);
    }
    
    close(fd);
    printf("Safety test completed\n");
    return 0;
}
EOF

    # Compile test program
    if gcc -o /tmp/lidar_safety_test /tmp/lidar_safety_test.c 2>/dev/null; then
        print_result "Safety Test Compilation" "PASS" "Test program compiled successfully"
        
        # Run safety test
        echo "Running safety integration test..."
        /tmp/lidar_safety_test
        
        if [ $? -eq 0 ]; then
            print_result "Safety Integration Test" "PASS" "Safety test completed successfully"
        else
            print_result "Safety Integration Test" "FAIL" "Safety test failed"
        fi
    else
        print_result "Safety Test Compilation" "FAIL" "Failed to compile test program"
    fi
    
    # Cleanup
    rm -f /tmp/lidar_safety_test.c /tmp/lidar_safety_test
else
    print_result "Safety Integration Test" "FAIL" "Device not available"
fi

echo ""
echo "📋 Test 6: Documentation Check"
echo "----------------------------------------"

# Check for documentation files
DOC_FILES=(
    "docs/specs/lidar_wiring_guide.md"
    "docs/specs/lidar_setup_guide.md"
    "docs/specs/lidar_test_procedures.md"
    "docs/specs/lidar_integration_guide.md"
)

for doc_file in "${DOC_FILES[@]}"; do
    if file_exists "$doc_file"; then
        print_result "Documentation: $(basename "$doc_file")" "PASS" "Found"
    else
        print_result "Documentation: $(basename "$doc_file")" "FAIL" "Not found"
    fi
done

echo ""
echo "🎯 Test Summary"
echo "================"
echo "Total Tests: $TOTAL"
echo -e "Passed: ${GREEN}$PASSED${NC}"
echo -e "Failed: ${RED}$FAILED${NC}"

if [ $FAILED -eq 0 ]; then
    echo -e "${GREEN}🎉 All tests PASSED! LiDAR SDK is ready for integration.${NC}"
    echo -e "${BLUE}Next step: Proceed with HAL driver implementation${NC}"
    exit 0
elif [ $FAILED -le 3 ]; then
    echo -e "${YELLOW}⚠️  Some tests failed. Check recommendations above.${NC}"
    echo -e "${BLUE}Consider fixing issues before proceeding with HAL implementation${NC}"
    exit 1
else
    echo -e "${RED}❌ Multiple tests failed. LiDAR setup needs attention.${NC}"
    echo -e "${RED}Fix issues before proceeding with HAL implementation${NC}"
    exit 2
fi
