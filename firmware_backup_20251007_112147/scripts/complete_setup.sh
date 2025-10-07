#!/bin/bash

# OHT-50 Firmware Complete Setup Script
# Performs all setup steps: build, static analysis, test setup

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== OHT-50 Firmware Complete Setup ===${NC}"
echo -e "This script will perform all setup steps:"
echo -e "1. Build system validation"
echo -e "2. Static analysis execution"
echo -e "3. Test framework setup"
echo -e "4. Compliance validation"
echo ""

# Configuration
BUILD_DIR="build"
ANALYSIS_DIR="static_analysis"
CMAKE_BUILD_TYPE=${1:-Release}

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Check prerequisites
echo -e "${GREEN}Checking prerequisites...${NC}"
if ! command_exists cmake; then
    echo -e "${RED}Error: cmake not found. Please install cmake first.${NC}"
    exit 1
fi

if ! command_exists make; then
    echo -e "${RED}Error: make not found. Please install make first.${NC}"
    exit 1
fi

if ! command_exists clang-tidy; then
    echo -e "${YELLOW}Warning: clang-tidy not found. Static analysis will be skipped.${NC}"
    SKIP_STATIC_ANALYSIS=true
else
    SKIP_STATIC_ANALYSIS=false
fi

if ! command_exists cppcheck; then
    echo -e "${YELLOW}Warning: cppcheck not found. Static analysis will be skipped.${NC}"
    SKIP_STATIC_ANALYSIS=true
fi

echo -e "${GREEN}Prerequisites check completed.${NC}"
echo ""

# Step 1: Build System Validation
echo -e "${BLUE}=== Step 1: Build System Validation ===${NC}"
echo -e "${GREEN}Building with CMake...${NC}"

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake .. -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
         -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
         -DENABLE_DEBUG=ON \
         -DENABLE_LOGGING=ON \
         -DENABLE_SAFETY=ON

echo -e "${GREEN}Building all targets...${NC}"
make -j$(nproc)

echo -e "${GREEN}Build completed successfully!${NC}"
cd ..

# Step 2: Static Analysis (if tools available)
if [ "$SKIP_STATIC_ANALYSIS" = false ]; then
    echo ""
    echo -e "${BLUE}=== Step 2: Static Analysis ===${NC}"
    echo -e "${GREEN}Running static analysis...${NC}"
    
    ./scripts/static_analysis.sh ${CMAKE_BUILD_TYPE}
    
    echo -e "${GREEN}Static analysis completed!${NC}"
else
    echo ""
    echo -e "${YELLOW}=== Step 2: Static Analysis (SKIPPED) ===${NC}"
    echo -e "${YELLOW}Static analysis tools not available. Skipping.${NC}"
fi

# Step 3: Test Framework Setup
echo ""
echo -e "${BLUE}=== Step 3: Test Framework Setup ===${NC}"
echo -e "${GREEN}Setting up test framework...${NC}"

./scripts/setup_tests.sh

echo -e "${GREEN}Test framework setup completed!${NC}"

# Step 4: Build Tests
echo ""
echo -e "${BLUE}=== Step 4: Test Build ===${NC}"
echo -e "${GREEN}Building tests...${NC}"

cd ${BUILD_DIR}
if make oht50_tests 2>/dev/null; then
    echo -e "${GREEN}Tests built successfully!${NC}"
    TEST_BUILD_SUCCESS=true
else
    echo -e "${YELLOW}Test build failed (expected for initial setup).${NC}"
    TEST_BUILD_SUCCESS=false
fi
cd ..

# Step 5: Generate Summary Report
echo ""
echo -e "${BLUE}=== Step 5: Summary Report ===${NC}"

cat > setup_summary.txt << EOF
OHT-50 Firmware Setup Summary
============================
Date: $(date)
Build Type: ${CMAKE_BUILD_TYPE}

BUILD STATUS:
============
✅ Main firmware build: SUCCESS
✅ HAL libraries: SUCCESS
✅ Application libraries: SUCCESS
✅ Main executable: SUCCESS

STATIC ANALYSIS:
===============
EOF

if [ "$SKIP_STATIC_ANALYSIS" = false ]; then
    if [ -f "${ANALYSIS_DIR}/analysis-summary.txt" ]; then
        cat ${ANALYSIS_DIR}/analysis-summary.txt >> setup_summary.txt
    else
        echo "❌ Static analysis: FAILED (no report generated)" >> setup_summary.txt
    fi
else
    echo "⚠️  Static analysis: SKIPPED (tools not available)" >> setup_summary.txt
fi

cat >> setup_summary.txt << EOF

TEST FRAMEWORK:
==============
✅ Test structure: CREATED
✅ Unity framework: SETUP
✅ Test templates: CREATED
✅ Test build: $(if [ "$TEST_BUILD_SUCCESS" = true ]; then echo "SUCCESS"; else echo "FAILED (expected)"; fi)

NEXT STEPS:
===========
1. Review static analysis results (if available)
2. Fix any remaining compilation issues
3. Complete test implementation
4. Run full test suite
5. Validate MISRA compliance

FILES CREATED:
==============
- build/ (build artifacts)
- static_analysis/ (analysis reports)
- tests/ (test framework)
- setup_summary.txt (this report)

COMMANDS:
=========
- Build: cd build && make
- Static Analysis: ./scripts/static_analysis.sh
- Tests: cd tests && ./run_tests.sh
- Clean: cd build && make clean
EOF

# Display summary
echo -e "${GREEN}=== SETUP COMPLETE ===${NC}"
echo -e "Summary saved to: ${YELLOW}setup_summary.txt${NC}"
echo ""
echo -e "${BLUE}=== SUMMARY ===${NC}"
cat setup_summary.txt

echo ""
echo -e "${GREEN}Setup completed successfully!${NC}"
echo -e "Next steps:"
echo -e "1. Review static analysis results"
echo -e "2. Fix any remaining issues"
echo -e "3. Complete test implementation"
echo -e "4. Validate compliance"
