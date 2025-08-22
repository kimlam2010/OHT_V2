#!/bin/bash
# LiDAR Complete Test Suite - OHT-50
# Run all LiDAR tests in sequence

echo "=== LiDAR Complete Test Suite - OHT-50 ==="
echo "Date: $(date)"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test results
BASIC_TEST_RESULT=0
SDK_TEST_RESULT=0

echo "🚀 Starting LiDAR test suite..."
echo ""

# Test 1: Basic hardware và software test
echo -e "${BLUE}📋 Test 1: Basic Hardware & Software Test${NC}"
echo "================================================"
if [ -f "tools/test_lidar_basic.sh" ]; then
    chmod +x tools/test_lidar_basic.sh
    ./tools/test_lidar_basic.sh
    BASIC_TEST_RESULT=$?
else
    echo -e "${RED}❌ Basic test script not found: tools/test_lidar_basic.sh${NC}"
    BASIC_TEST_RESULT=2
fi

echo ""
echo ""

# Test 2: SDK functionality test
echo -e "${BLUE}📋 Test 2: SDK Functionality Test${NC}"
echo "=========================================="
if [ -f "tools/test_lidar_sdk.sh" ]; then
    chmod +x tools/test_lidar_sdk.sh
    ./tools/test_lidar_sdk.sh
    SDK_TEST_RESULT=$?
else
    echo -e "${RED}❌ SDK test script not found: tools/test_lidar_sdk.sh${NC}"
    SDK_TEST_RESULT=2
fi

echo ""
echo ""

# Overall test summary
echo "🎯 Overall Test Summary"
echo "======================"

echo -e "Basic Test: ${BASIC_TEST_RESULT}"
echo -e "SDK Test: ${SDK_TEST_RESULT}"

if [ $BASIC_TEST_RESULT -eq 0 ] && [ $SDK_TEST_RESULT -eq 0 ]; then
    echo -e "${GREEN}🎉 All tests PASSED! LiDAR is ready for HAL implementation.${NC}"
    echo ""
    echo -e "${BLUE}Next steps:${NC}"
    echo "1. Proceed with HAL driver implementation"
    echo "2. Implement safety integration"
    echo "3. Implement control system integration"
    echo "4. Implement telemetry integration"
    exit 0
elif [ $BASIC_TEST_RESULT -eq 0 ] && [ $SDK_TEST_RESULT -ne 0 ]; then
    echo -e "${YELLOW}⚠️  Basic tests passed, but SDK tests failed.${NC}"
    echo -e "${YELLOW}Consider installing RPLIDAR SDK before proceeding.${NC}"
    exit 1
elif [ $BASIC_TEST_RESULT -ne 0 ] && [ $SDK_TEST_RESULT -eq 0 ]; then
    echo -e "${YELLOW}⚠️  SDK tests passed, but basic tests failed.${NC}"
    echo -e "${YELLOW}Check hardware connection và system setup.${NC}"
    exit 1
else
    echo -e "${RED}❌ Multiple tests failed. LiDAR setup needs attention.${NC}"
    echo -e "${RED}Fix issues before proceeding with HAL implementation.${NC}"
    exit 2
fi
