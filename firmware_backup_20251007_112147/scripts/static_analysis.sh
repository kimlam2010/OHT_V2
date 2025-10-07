#!/bin/bash

# OHT-50 Firmware Static Analysis Script
# MISRA C:2012 Compliance Check

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
ANALYSIS_DIR="static_analysis"
CMAKE_BUILD_TYPE=${1:-Release}

echo -e "${BLUE}=== OHT-50 Firmware Static Analysis ===${NC}"
echo -e "Build Type: ${YELLOW}${CMAKE_BUILD_TYPE}${NC}"
echo -e "Analysis Directory: ${YELLOW}${ANALYSIS_DIR}${NC}"

# Check if tools are available
if ! command -v clang-tidy &> /dev/null; then
    echo -e "${RED}Error: clang-tidy not found. Please install clang-tidy first.${NC}"
    exit 1
fi

if ! command -v cppcheck &> /dev/null; then
    echo -e "${RED}Error: cppcheck not found. Please install cppcheck first.${NC}"
    exit 1
fi

# Create analysis directory
mkdir -p ${ANALYSIS_DIR}

# Build with CMake to generate compile_commands.json
echo -e "${GREEN}Building with CMake to generate compile_commands.json...${NC}"
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake .. -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
         -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
         -DENABLE_DEBUG=ON \
         -DENABLE_LOGGING=ON \
         -DENABLE_SAFETY=ON

# Run clang-tidy
echo -e "${GREEN}Running clang-tidy...${NC}"
clang-tidy \
    -p . \
    --config-file=../.clang-tidy \
    --header-filter='.*' \
    --quiet \
    ../src/**/*.c \
    ../src/**/*.h \
    ../include/*.h \
    > ../${ANALYSIS_DIR}/clang-tidy-report.txt 2>&1 || true

# Run cppcheck
echo -e "${GREEN}Running cppcheck...${NC}"
cppcheck \
    --enable=all \
    --std=c11 \
    --language=c \
    --output-file=../${ANALYSIS_DIR}/cppcheck-report.txt \
    --xml --xml-version=2 \
    --suppress=missingIncludeSystem \
    --suppress=unusedFunction \
    --suppress=unmatchedSuppression \
    ../src/ ../include/ 2>&1 || true

# Generate summary report
echo -e "${GREEN}Generating analysis summary...${NC}"
cd ..

cat > ${ANALYSIS_DIR}/analysis-summary.txt << EOF
OHT-50 Firmware Static Analysis Summary
=======================================
Date: $(date)
Build Type: ${CMAKE_BUILD_TYPE}

CLANG-TIDY RESULTS:
==================
EOF

if [ -s ${ANALYSIS_DIR}/clang-tidy-report.txt ]; then
    echo "Issues found:" >> ${ANALYSIS_DIR}/analysis-summary.txt
    grep -c "warning\|error" ${ANALYSIS_DIR}/clang-tidy-report.txt >> ${ANALYSIS_DIR}/analysis-summary.txt || echo "0" >> ${ANALYSIS_DIR}/analysis-summary.txt
else
    echo "No issues found" >> ${ANALYSIS_DIR}/analysis-summary.txt
fi

cat >> ${ANALYSIS_DIR}/analysis-summary.txt << EOF

CPPCHECK RESULTS:
================
EOF

if [ -s ${ANALYSIS_DIR}/cppcheck-report.txt ]; then
    echo "Issues found:" >> ${ANALYSIS_DIR}/analysis-summary.txt
    grep -c "error\|warning" ${ANALYSIS_DIR}/cppcheck-report.txt >> ${ANALYSIS_DIR}/analysis-summary.txt || echo "0" >> ${ANALYSIS_DIR}/analysis-summary.txt
else
    echo "No issues found" >> ${ANALYSIS_DIR}/analysis-summary.txt
fi

# Display summary
echo -e "${GREEN}=== Analysis Complete ===${NC}"
echo -e "Reports saved in: ${YELLOW}${ANALYSIS_DIR}/${NC}"
echo -e "Summary: ${YELLOW}${ANALYSIS_DIR}/analysis-summary.txt${NC}"
echo -e "Clang-tidy: ${YELLOW}${ANALYSIS_DIR}/clang-tidy-report.txt${NC}"
echo -e "Cppcheck: ${YELLOW}${ANALYSIS_DIR}/cppcheck-report.txt${NC}"

# Show summary
echo -e "\n${BLUE}=== ANALYSIS SUMMARY ===${NC}"
cat ${ANALYSIS_DIR}/analysis-summary.txt

echo -e "\n${GREEN}Static analysis completed successfully!${NC}"
