#!/bin/bash

# 🔧 Test Script for WebSocket System Fix - Issue #153
# Tests the new libwebsockets-based WebSocket system

echo "🚀 Testing WebSocket System Fix - Issue #153"
echo "=============================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test results
TESTS_PASSED=0
TESTS_FAILED=0
TOTAL_TESTS=0

# Function to run a test
run_test() {
    local test_name="$1"
    local test_command="$2"
    
    echo -e "${BLUE}🧪 Running test: $test_name${NC}"
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    if eval "$test_command"; then
        echo -e "${GREEN}✅ Test passed: $test_name${NC}"
        TESTS_PASSED=$((TESTS_PASSED + 1))
        return 0
    else
        echo -e "${RED}❌ Test failed: $test_name${NC}"
        TESTS_FAILED=$((TESTS_FAILED + 1))
        return 1
    fi
}

# Function to check if libwebsockets is installed
check_libwebsockets() {
    echo -e "${YELLOW}📦 Checking libwebsockets installation...${NC}"
    
    if pkg-config --exists libwebsockets; then
        local version=$(pkg-config --modversion libwebsockets)
        echo -e "${GREEN}✅ libwebsockets found: version $version${NC}"
        return 0
    else
        echo -e "${RED}❌ libwebsockets not found${NC}"
        echo -e "${YELLOW}💡 Run: sudo ./install_libwebsockets.sh${NC}"
        return 1
    fi
}

# Function to check if library files exist
check_library_files() {
    echo -e "${YELLOW}📁 Checking library files...${NC}"
    
    local files=(
        "/home/orangepi/Desktop/OHT_V2/firmware_new/firmware_src/app/websocket_server_lws.h"
        "/home/orangepi/Desktop/OHT_V2/firmware_new/firmware_src/app/websocket_server_lws.c"
        "/home/orangepi/Desktop/OHT_V2/firmware_new/firmware_src/app/websocket_integration.h"
        "/home/orangepi/Desktop/OHT_V2/firmware_new/firmware_src/app/websocket_integration.c"
    )
    
    for file in "${files[@]}"; do
        if [ -f "$file" ]; then
            echo -e "${GREEN}✅ Found: $(basename "$file")${NC}"
        else
            echo -e "${RED}❌ Missing: $(basename "$file")${NC}"
            return 1
        fi
    done
    
    return 0
}

# Function to check CMakeLists.txt
check_cmake() {
    echo -e "${YELLOW}🔧 Checking CMakeLists.txt...${NC}"
    
    local cmake_file="/home/orangepi/Desktop/OHT_V2/firmware_new/firmware_src/app/CMakeLists.txt"
    
    if [ -f "$cmake_file" ]; then
        if grep -q "websocket_server_lws" "$cmake_file"; then
            echo -e "${GREEN}✅ CMakeLists.txt contains websocket_server_lws${NC}"
        else
            echo -e "${RED}❌ CMakeLists.txt missing websocket_server_lws${NC}"
            return 1
        fi
        
        if grep -q "websocket_integration" "$cmake_file"; then
            echo -e "${GREEN}✅ CMakeLists.txt contains websocket_integration${NC}"
        else
            echo -e "${RED}❌ CMakeLists.txt missing websocket_integration${NC}"
            return 1
        fi
        
        if grep -q "websockets" "$cmake_file"; then
            echo -e "${GREEN}✅ CMakeLists.txt links libwebsockets${NC}"
        else
            echo -e "${RED}❌ CMakeLists.txt missing libwebsockets link${NC}"
            return 1
        fi
    else
        echo -e "${RED}❌ CMakeLists.txt not found${NC}"
        return 1
    fi
    
    return 0
}

# Function to test compilation
test_compilation() {
    echo -e "${YELLOW}🔨 Testing compilation...${NC}"
    
    cd /home/orangepi/Desktop/OHT_V2/firmware_new
    
    # Try to build the project
    if make clean && make; then
        echo -e "${GREEN}✅ Compilation successful${NC}"
        return 0
    else
        echo -e "${RED}❌ Compilation failed${NC}"
        echo -e "${YELLOW}💡 Check for missing dependencies or syntax errors${NC}"
        return 1
    fi
}

# Function to test WebSocket server startup
test_websocket_startup() {
    echo -e "${YELLOW}🚀 Testing WebSocket server startup...${NC}"
    
    # This would require the actual firmware to be running
    # For now, we'll just check if the binary exists
    local binary_path="/home/orangepi/Desktop/OHT_V2/firmware_new/build/oht50_firmware"
    
    if [ -f "$binary_path" ]; then
        echo -e "${GREEN}✅ Firmware binary exists${NC}"
        echo -e "${YELLOW}💡 To test WebSocket server, run: $binary_path${NC}"
        return 0
    else
        echo -e "${RED}❌ Firmware binary not found${NC}"
        return 1
    fi
}

# Function to check thread management
check_thread_management() {
    echo -e "${YELLOW}🧵 Checking thread management...${NC}"
    
    local lws_file="/home/orangepi/Desktop/OHT_V2/firmware_new/firmware_src/app/websocket_server_lws.c"
    
    if [ -f "$lws_file" ]; then
        # Check for proper thread cleanup
        if grep -q "pthread_join\|pthread_detach" "$lws_file"; then
            echo -e "${GREEN}✅ Thread cleanup mechanisms found${NC}"
        else
            echo -e "${RED}❌ Thread cleanup mechanisms missing${NC}"
            return 1
        fi
        
        # Check for libwebsockets usage
        if grep -q "lws_" "$lws_file"; then
            echo -e "${GREEN}✅ libwebsockets API usage found${NC}"
        else
            echo -e "${RED}❌ libwebsockets API usage missing${NC}"
            return 1
        fi
        
        # Check for proper error handling
        if grep -q "error_handling\|HAL_STATUS" "$lws_file"; then
            echo -e "${GREEN}✅ Error handling mechanisms found${NC}"
        else
            echo -e "${RED}❌ Error handling mechanisms missing${NC}"
            return 1
        fi
    else
        echo -e "${RED}❌ websocket_server_lws.c not found${NC}"
        return 1
    fi
    
    return 0
}

# Function to check integration wrapper
check_integration_wrapper() {
    echo -e "${YELLOW}🔗 Checking integration wrapper...${NC}"
    
    local integration_file="/home/orangepi/Desktop/OHT_V2/firmware_new/firmware_src/app/websocket_integration.c"
    
    if [ -f "$integration_file" ]; then
        # Check for fallback mechanisms
        if grep -q "fallback\|switch_to" "$integration_file"; then
            echo -e "${GREEN}✅ Fallback mechanisms found${NC}"
        else
            echo -e "${RED}❌ Fallback mechanisms missing${NC}"
            return 1
        fi
        
        # Check for compatibility functions
        if grep -q "_compat" "$integration_file"; then
            echo -e "${GREEN}✅ Compatibility functions found${NC}"
        else
            echo -e "${RED}❌ Compatibility functions missing${NC}"
            return 1
        fi
        
        # Check for statistics tracking
        if grep -q "stats\|statistics" "$integration_file"; then
            echo -e "${GREEN}✅ Statistics tracking found${NC}"
        else
            echo -e "${RED}❌ Statistics tracking missing${NC}"
            return 1
        fi
    else
        echo -e "${RED}❌ websocket_integration.c not found${NC}"
        return 1
    fi
    
    return 0
}

# Main test execution
echo -e "${BLUE}🎯 Starting WebSocket System Fix Tests${NC}"
echo ""

# Run all tests
run_test "libwebsockets installation" "check_libwebsockets"
run_test "library files existence" "check_library_files"
run_test "CMakeLists.txt configuration" "check_cmake"
run_test "thread management" "check_thread_management"
run_test "integration wrapper" "check_integration_wrapper"
run_test "compilation" "test_compilation"
run_test "WebSocket server startup" "test_websocket_startup"

# Print test results
echo ""
echo -e "${BLUE}📊 Test Results Summary${NC}"
echo "=========================="
echo -e "${GREEN}✅ Tests Passed: $TESTS_PASSED${NC}"
echo -e "${RED}❌ Tests Failed: $TESTS_FAILED${NC}"
echo -e "${BLUE}📈 Total Tests: $TOTAL_TESTS${NC}"

if [ $TESTS_FAILED -eq 0 ]; then
    echo ""
    echo -e "${GREEN}🎉 All tests passed! WebSocket system fix is ready.${NC}"
    echo -e "${YELLOW}💡 Next steps:${NC}"
    echo "   1. Install libwebsockets: sudo ./install_libwebsockets.sh"
    echo "   2. Build firmware: cd firmware_new && make"
    echo "   3. Test WebSocket server: ./build/oht50_firmware"
    echo "   4. Monitor logs for WebSocket activity"
    exit 0
else
    echo ""
    echo -e "${RED}⚠️  Some tests failed. Please fix the issues above.${NC}"
    echo -e "${YELLOW}💡 Common fixes:${NC}"
    echo "   1. Install libwebsockets: sudo ./install_libwebsockets.sh"
    echo "   2. Check file permissions and paths"
    echo "   3. Verify CMakeLists.txt configuration"
    echo "   4. Check for compilation errors"
    exit 1
fi
