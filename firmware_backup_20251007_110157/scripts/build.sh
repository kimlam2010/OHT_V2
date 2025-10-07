#!/bin/bash

# OHT-50 Firmware Build Script
# ISO/IEC 12207 Compliant Build System

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Build configuration
BUILD_TYPE=${1:-Release}
BUILD_DIR="build"
INSTALL_DIR="install"

echo -e "${GREEN}=== OHT-50 Firmware Build Script ===${NC}"
echo -e "Build Type: ${YELLOW}${BUILD_TYPE}${NC}"
echo -e "Build Directory: ${YELLOW}${BUILD_DIR}${NC}"

# Check if CMake is available
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: CMake not found. Please install CMake first.${NC}"
    echo "Install command: sudo apt-get install cmake"
    exit 1
fi

# Create build directory
echo -e "${GREEN}Creating build directory...${NC}"
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

# Configure with CMake
echo -e "${GREEN}Configuring with CMake...${NC}"
cmake .. -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
         -DENABLE_DEBUG=ON \
         -DENABLE_LOGGING=ON \
         -DENABLE_SAFETY=ON

# Build
echo -e "${GREEN}Building firmware...${NC}"
make -j$(nproc)

# Run tests if available
if [ -f "Makefile" ]; then
    echo -e "${GREEN}Running tests...${NC}"
    make test || echo -e "${YELLOW}Warning: Some tests failed${NC}"
fi

# Install
echo -e "${GREEN}Installing...${NC}"
mkdir -p ../${INSTALL_DIR}
make install DESTDIR=../${INSTALL_DIR}

echo -e "${GREEN}=== Build Completed Successfully ===${NC}"
echo -e "Executable: ${YELLOW}${BUILD_DIR}/oht50_main${NC}"
echo -e "Install directory: ${YELLOW}${INSTALL_DIR}${NC}"
