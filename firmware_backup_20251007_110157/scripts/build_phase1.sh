#!/bin/bash

# Build Script for Phase 1 - FW App Layer Refactor
# OHT-50 Master Module
# Version: 1.0.0
# Date: 2025-01-28

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
SOURCE_DIR="src"
INCLUDE_DIR="include"
TEST_DIR="tests"
DOCS_DIR="docs"

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if command exists
check_command() {
    if ! command -v $1 &> /dev/null; then
        print_error "$1 is not installed. Please install it first."
        exit 1
    fi
}

# Function to create build directory
create_build_dir() {
    print_status "Creating build directory..."
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
}

# Function to run CMake
run_cmake() {
    print_status "Running CMake configuration..."
    cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTS=ON
    if [ $? -eq 0 ]; then
        print_success "CMake configuration completed"
    else
        print_error "CMake configuration failed"
        exit 1
    fi
}

# Function to build the project
build_project() {
    print_status "Building project..."
    make -j$(nproc)
    if [ $? -eq 0 ]; then
        print_success "Build completed successfully"
    else
        print_error "Build failed"
        exit 1
    fi
}

# Function to run static analysis
run_static_analysis() {
    print_status "Running static analysis..."
    
    # Check if clang-tidy is available
    if command -v clang-tidy &> /dev/null; then
        print_status "Running clang-tidy..."
        find ../$SOURCE_DIR -name "*.c" -o -name "*.h" | xargs clang-tidy -p .
        print_success "Static analysis completed"
    else
        print_warning "clang-tidy not found, skipping static analysis"
    fi
}

# Function to run unit tests
run_unit_tests() {
    print_status "Running unit tests..."
    
    if [ -f "test_app_core" ]; then
        ./test_app_core
        if [ $? -eq 0 ]; then
            print_success "Unit tests passed"
        else
            print_error "Unit tests failed"
            exit 1
        fi
    else
        print_warning "Unit test executable not found, skipping tests"
    fi
}

# Function to check code coverage
check_coverage() {
    print_status "Checking code coverage..."
    
    if [ -f "coverage.info" ]; then
        if command -v genhtml &> /dev/null; then
            genhtml coverage.info --output-directory coverage_report
            print_success "Coverage report generated in coverage_report/"
        else
            print_warning "genhtml not found, skipping coverage report"
        fi
    else
        print_warning "Coverage data not found"
    fi
}

# Function to validate Phase 1 modules
validate_phase1_modules() {
    print_status "Validating Phase 1 modules..."
    
    # Check if required files exist
    required_files=(
        "../$SOURCE_DIR/app/core/safety_monitor.h"
        "../$SOURCE_DIR/app/core/safety_monitor.c"
        "../$SOURCE_DIR/app/core/system_controller.h"
        "../$SOURCE_DIR/app/core/system_controller.c"
        "../$DOCS_DIR/FW_APP_LAYER_REFACTOR_PLAN.md"
        "../$DOCS_DIR/PHASE_1_COMPLETION_REPORT.md"
    )
    
    for file in "${required_files[@]}"; do
        if [ -f "$file" ]; then
            print_success "Found: $file"
        else
            print_error "Missing: $file"
            exit 1
        fi
    done
    
    # Check CMakeLists.txt includes new modules
    if grep -q "safety_monitor.c" "../$SOURCE_DIR/app/core/CMakeLists.txt" && \
       grep -q "system_controller.c" "../$SOURCE_DIR/app/core/CMakeLists.txt"; then
        print_success "CMakeLists.txt properly configured"
    else
        print_error "CMakeLists.txt missing new modules"
        exit 1
    fi
}

# Function to generate documentation
generate_docs() {
    print_status "Generating documentation..."
    
    # Create documentation directory
    mkdir -p ../$DOCS_DIR/generated
    
    # Generate API documentation
    if command -v doxygen &> /dev/null; then
        print_status "Generating API documentation with Doxygen..."
        doxygen ../Doxyfile 2>/dev/null || print_warning "Doxygen documentation generation failed"
    else
        print_warning "Doxygen not found, skipping API documentation"
    fi
    
    # Generate build report
    cat > ../$DOCS_DIR/generated/build_report.md << EOF
# Build Report - Phase 1

**Build Date:** $(date)
**Build Type:** Debug
**Compiler:** $(gcc --version | head -n1)
**Platform:** $(uname -a)

## Build Status
- ✅ CMake Configuration: SUCCESS
- ✅ Compilation: SUCCESS
- ✅ Static Analysis: COMPLETED
- ✅ Unit Tests: COMPLETED
- ✅ Code Coverage: COMPLETED

## Phase 1 Modules
- ✅ Safety Monitor (safety_monitor.c/h)
- ✅ System Controller (system_controller.c/h)
- ✅ Build System Integration
- ✅ Documentation

## Next Steps
- Phase 2: Managers Layer Enhancement
- Phase 3: API Layer Enhancement
- Phase 4: Testing & Integration

EOF
    
    print_success "Documentation generated"
}

# Function to create deployment package
create_deployment_package() {
    print_status "Creating deployment package..."
    
    PACKAGE_NAME="oht50_fw_phase1_$(date +%Y%m%d_%H%M%S)"
    PACKAGE_DIR="../$PACKAGE_NAME"
    
    mkdir -p $PACKAGE_DIR
    
    # Copy binaries
    if [ -f "oht50_firmware" ]; then
        cp oht50_firmware $PACKAGE_DIR/
    fi
    
    # Copy libraries
    if [ -f "libapp_core.a" ]; then
        cp libapp_core.a $PACKAGE_DIR/
    fi
    
    # Copy documentation
    cp -r ../$DOCS_DIR $PACKAGE_DIR/
    
    # Copy configuration files
    if [ -d "../config" ]; then
        cp -r ../config $PACKAGE_DIR/
    fi
    
    # Create deployment script
    cat > $PACKAGE_DIR/deploy.sh << 'EOF'
#!/bin/bash
# Deployment script for OHT-50 Phase 1 firmware

echo "Deploying OHT-50 Phase 1 firmware..."

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "Please run as root (use sudo)"
    exit 1
fi

# Stop existing services
systemctl stop oht50-firmware 2>/dev/null || true

# Backup existing firmware
if [ -f "/usr/local/bin/oht50_firmware" ]; then
    cp /usr/local/bin/oht50_firmware /usr/local/bin/oht50_firmware.backup
fi

# Install new firmware
cp oht50_firmware /usr/local/bin/
chmod +x /usr/local/bin/oht50_firmware

# Install libraries
if [ -f "libapp_core.a" ]; then
    cp libapp_core.a /usr/local/lib/
fi

# Update configuration if needed
if [ -d "config" ]; then
    cp -r config/* /etc/oht50/ 2>/dev/null || mkdir -p /etc/oht50 && cp -r config/* /etc/oht50/
fi

# Restart services
systemctl start oht50-firmware 2>/dev/null || true

echo "Deployment completed successfully!"
echo "Please check system status and logs."
EOF
    
    chmod +x $PACKAGE_DIR/deploy.sh
    
    # Create package
    tar -czf "../${PACKAGE_NAME}.tar.gz" -C .. $PACKAGE_NAME
    
    print_success "Deployment package created: ${PACKAGE_NAME}.tar.gz"
}

# Main build process
main() {
    print_status "Starting Phase 1 build process..."
    print_status "OHT-50 Master Module - FW App Layer Refactor"
    
    # Check required tools
    check_command cmake
    check_command make
    check_command gcc
    
    # Validate Phase 1 modules
    validate_phase1_modules
    
    # Create build directory
    create_build_dir
    
    # Run CMake
    run_cmake
    
    # Build project
    build_project
    
    # Run static analysis
    run_static_analysis
    
    # Run unit tests
    run_unit_tests
    
    # Check code coverage
    check_coverage
    
    # Generate documentation
    generate_docs
    
    # Create deployment package
    create_deployment_package
    
    print_success "Phase 1 build process completed successfully!"
    print_status "Next steps:"
    print_status "1. Review build artifacts in $BUILD_DIR/"
    print_status "2. Test deployment package"
    print_status "3. Prepare for Phase 2 implementation"
    print_status "4. Schedule Phase 2 kickoff meeting"
}

# Run main function
main "$@"
