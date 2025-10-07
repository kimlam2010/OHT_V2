#!/bin/bash

# OHT-50 Firmware Release Package Creator
# ISO/IEC 12207 Compliant Release
# Version: 1.0.0

set -e  # Exit on any error

# Configuration
PROJECT_NAME="OHT-50_Firmware"
VERSION=$(git describe --tags --always 2>/dev/null || echo "v1.0.0-$(date +%Y%m%d)")
BUILD_DIR="build"
RELEASE_DIR="release"
PACKAGE_NAME="${PROJECT_NAME}_${VERSION}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging function
log() {
    echo -e "${BLUE}[$(date '+%Y-%m-%d %H:%M:%S')]${NC} $1"
}

# Error handling
error_exit() {
    log "${RED}ERROR: $1${NC}"
    exit 1
}

# Check prerequisites
check_prerequisites() {
    log "Checking prerequisites..."
    
    # Check if build directory exists
    if [[ ! -d "$BUILD_DIR" ]]; then
        error_exit "Build directory not found: $BUILD_DIR"
    fi
    
    # Check if main executable exists
    if [[ ! -f "$BUILD_DIR/oht50_main" ]]; then
        error_exit "Main executable not found: $BUILD_DIR/oht50_main"
    fi
    
    # Check if scripts directory exists
    if [[ ! -d "scripts" ]]; then
        error_exit "Scripts directory not found: scripts"
    fi
    
    log "Prerequisites check passed"
}

# Clean previous release
clean_previous_release() {
    log "Cleaning previous release..."
    
    if [[ -d "$RELEASE_DIR" ]]; then
        rm -rf "$RELEASE_DIR"
        log "Removed previous release directory"
    fi
    
    mkdir -p "$RELEASE_DIR"
    log "Created fresh release directory"
}

# Build firmware
build_firmware() {
    log "Building firmware..."
    
    cd "$BUILD_DIR"
    
    # Clean and rebuild
    make clean
    cmake ..
    make oht50_main -j$(nproc)
    
    cd ..
    
    log "Firmware build completed"
}

# Create release structure
create_release_structure() {
    log "Creating release structure..."
    
    # Create main release directory
    mkdir -p "$RELEASE_DIR/$PACKAGE_NAME"
    
    # Create subdirectories
    mkdir -p "$RELEASE_DIR/$PACKAGE_NAME/bin"
    mkdir -p "$RELEASE_DIR/$PACKAGE_NAME/scripts"
    mkdir -p "$RELEASE_DIR/$PACKAGE_NAME/config"
    mkdir -p "$RELEASE_DIR/$PACKAGE_NAME/docs"
    mkdir -p "$RELEASE_DIR/$PACKAGE_NAME/tests"
    
    log "Release structure created"
}

# Copy binaries
copy_binaries() {
    log "Copying binaries..."
    
    # Copy main executable
    cp "$BUILD_DIR/oht50_main" "$RELEASE_DIR/$PACKAGE_NAME/bin/"
    chmod +x "$RELEASE_DIR/$PACKAGE_NAME/bin/oht50_main"
    
    # Copy test executables if they exist
    if [[ -f "$BUILD_DIR/tests/unit/test_hal_common" ]]; then
        cp "$BUILD_DIR/tests/unit/test_hal_common" "$RELEASE_DIR/$PACKAGE_NAME/tests/"
        chmod +x "$RELEASE_DIR/$PACKAGE_NAME/tests/test_hal_common"
    fi
    
    log "Binaries copied"
}

# Copy scripts
copy_scripts() {
    log "Copying scripts..."
    
    # Copy deployment scripts
    cp scripts/deploy.sh "$RELEASE_DIR/$PACKAGE_NAME/scripts/"
    cp scripts/rollback.sh "$RELEASE_DIR/$PACKAGE_NAME/scripts/"
    chmod +x "$RELEASE_DIR/$PACKAGE_NAME/scripts/"*.sh
    
    # Copy build script
    cp scripts/build.sh "$RELEASE_DIR/$PACKAGE_NAME/scripts/" 2>/dev/null || true
    
    log "Scripts copied"
}

# Copy configuration files
copy_config() {
    log "Copying configuration files..."
    
    # Copy config files if they exist
    if [[ -d "config" ]]; then
        cp -r config/* "$RELEASE_DIR/$PACKAGE_NAME/config/" 2>/dev/null || true
    fi
    
    # Create default config if none exists
    if [[ ! -f "$RELEASE_DIR/$PACKAGE_NAME/config/oht50_config.json" ]]; then
        cat > "$RELEASE_DIR/$PACKAGE_NAME/config/oht50_config.json" << 'EOF'
{
    "system": {
        "debug_mode": false,
        "log_level": "INFO",
        "update_interval_ms": 50
    },
    "safety": {
        "estop_pin": 18,
        "estop_timeout_ms": 100,
        "debounce_time_ms": 10
    },
    "communication": {
        "rs485_device": "/dev/ttyOHT485",
        "rs485_baud_rate": 115200,
        "modbus_timeout_ms": 1000
    },
    "modules": {
        "power_module_address": 2,
        "motor_module_address": 3,
        "dock_module_address": 4
    }
}
EOF
    fi
    
    log "Configuration files copied"
}

# Copy documentation
copy_documentation() {
    log "Copying documentation..."
    
    # Copy README
    if [[ -f "README.md" ]]; then
        cp README.md "$RELEASE_DIR/$PACKAGE_NAME/"
    fi
    
    # Copy docs directory
    if [[ -d "docs" ]]; then
        cp -r docs "$RELEASE_DIR/$PACKAGE_NAME/"
    fi
    
    # Create release notes
    cat > "$RELEASE_DIR/$PACKAGE_NAME/RELEASE_NOTES.md" << EOF
# OHT-50 Firmware Release Notes

**Version:** $VERSION  
**Date:** $(date '+%Y-%m-%d %H:%M:%S')  
**Build:** $(git rev-parse HEAD 2>/dev/null || echo "unknown")

## Changes in this Release

### New Features
- ISO/IEC 12207 compliant firmware structure
- CMake build system
- MISRA C:2012 compliance
- Unity test framework
- Automated deployment scripts

### Improvements
- Modular HAL layer
- Improved error handling
- Better logging system
- Enhanced safety mechanisms

### Bug Fixes
- Fixed include path issues
- Resolved compilation warnings
- Improved build reliability

## Installation

1. Extract the release package
2. Run the deployment script as root:
   \`\`\`bash
   sudo ./scripts/deploy.sh
   \`\`\`

## Rollback

If issues occur, use the rollback script:
\`\`\`bash
sudo ./scripts/rollback.sh
\`\`\`

## Testing

Run unit tests:
\`\`\`bash
./tests/test_hal_common
\`\`\`

## Support

For issues or questions, contact the FW Team.
EOF
    
    log "Documentation copied"
}

# Create installation script
create_install_script() {
    log "Creating installation script..."
    
    cat > "$RELEASE_DIR/$PACKAGE_NAME/install.sh" << 'EOF'
#!/bin/bash

# OHT-50 Firmware Installation Script
# Version: 1.0.0

set -e

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${GREEN}=== OHT-50 Firmware Installation ===${NC}"

# Check if running as root
if [[ $EUID -ne 0 ]]; then
    echo -e "${RED}This script must be run as root${NC}"
    exit 1
fi

# Create installation directories
echo "Creating installation directories..."
mkdir -p /opt/oht50/firmware
mkdir -p /opt/oht50/backup
mkdir -p /var/log

# Copy firmware
echo "Installing firmware..."
cp bin/oht50_main /opt/oht50/firmware/
chmod +x /opt/oht50/firmware/oht50_main

# Copy scripts
echo "Installing scripts..."
cp scripts/deploy.sh /opt/oht50/
cp scripts/rollback.sh /opt/oht50/
chmod +x /opt/oht50/*.sh

# Copy configuration
echo "Installing configuration..."
if [[ -d config ]]; then
    cp -r config/* /opt/oht50/
fi

# Create systemd service (optional)
echo "Creating systemd service..."
cat > /etc/systemd/system/oht50-firmware.service << 'SERVICE_EOF'
[Unit]
Description=OHT-50 Firmware
After=network.target

[Service]
Type=simple
User=root
WorkingDirectory=/opt/oht50/firmware
ExecStart=/opt/oht50/firmware/oht50_main
Restart=always
RestartSec=5
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
SERVICE_EOF

# Reload systemd
systemctl daemon-reload

echo -e "${GREEN}Installation completed successfully!${NC}"
echo ""
echo "To start the firmware:"
echo "  sudo systemctl start oht50-firmware"
echo ""
echo "To enable auto-start:"
echo "  sudo systemctl enable oht50-firmware"
echo ""
echo "To check status:"
echo "  sudo systemctl status oht50-firmware"
EOF
    
    chmod +x "$RELEASE_DIR/$PACKAGE_NAME/install.sh"
    
    log "Installation script created"
}

# Create package checksum
create_checksum() {
    log "Creating package checksum..."
    
    cd "$RELEASE_DIR/$PACKAGE_NAME"
    
    # Create SHA256 checksum
    find . -type f -exec sha256sum {} \; > SHA256SUMS
    
    cd ../..
    
    log "Package checksum created"
}

# Create compressed package
create_package() {
    log "Creating compressed package..."
    
    cd "$RELEASE_DIR"
    
    # Create tar.gz package
    tar -czf "${PACKAGE_NAME}.tar.gz" "$PACKAGE_NAME"
    
    # Create zip package (optional - skip if zip not available)
    if command -v zip >/dev/null 2>&1; then
        zip -r "${PACKAGE_NAME}.zip" "$PACKAGE_NAME"
        log "Created both tar.gz and zip packages"
    else
        log "Zip command not available, created tar.gz package only"
    fi
    
    cd ..
    
    log "Compressed packages created"
}

# Generate package info
generate_package_info() {
    log "Generating package information..."
    
    PACKAGE_SIZE=$(du -sh "$RELEASE_DIR/$PACKAGE_NAME" | cut -f1)
    TAR_SIZE=$(du -sh "$RELEASE_DIR/${PACKAGE_NAME}.tar.gz" | cut -f1)
    ZIP_SIZE="N/A"
    if [[ -f "$RELEASE_DIR/${PACKAGE_NAME}.zip" ]]; then
        ZIP_SIZE=$(du -sh "$RELEASE_DIR/${PACKAGE_NAME}.zip" | cut -f1)
    fi
    
    cat > "$RELEASE_DIR/PACKAGE_INFO.txt" << EOF
OHT-50 Firmware Release Package Information
==========================================

Package Name: $PACKAGE_NAME
Version: $VERSION
Build Date: $(date '+%Y-%m-%d %H:%M:%S')
Git Commit: $(git rev-parse HEAD 2>/dev/null || echo "unknown")

Package Sizes:
- Uncompressed: $PACKAGE_SIZE
- Tar.gz: $TAR_SIZE
- Zip: $ZIP_SIZE

Contents:
- bin/oht50_main (Main executable)
- scripts/ (Deployment and rollback scripts)
- config/ (Configuration files)
- docs/ (Documentation)
- tests/ (Unit tests)
- install.sh (Installation script)

Installation:
1. Extract the package
2. Run: sudo ./install.sh
3. Start: sudo systemctl start oht50-firmware

For more information, see RELEASE_NOTES.md
EOF
    
    log "Package information generated"
}

# Main function
main() {
    echo -e "${GREEN}=== OHT-50 Firmware Release Package Creator ===${NC}"
    log "Starting release package creation for version: $VERSION"
    
    check_prerequisites
    clean_previous_release
    build_firmware
    create_release_structure
    copy_binaries
    copy_scripts
    copy_config
    copy_documentation
    create_install_script
    create_checksum
    create_package
    generate_package_info
    
    echo -e "${GREEN}Release package created successfully!${NC}"
    echo ""
    echo -e "${YELLOW}Package location:${NC}"
    echo "  Directory: $RELEASE_DIR/$PACKAGE_NAME"
    echo "  Tar.gz: $RELEASE_DIR/${PACKAGE_NAME}.tar.gz"
    if [[ -f "$RELEASE_DIR/${PACKAGE_NAME}.zip" ]]; then
        echo "  Zip: $RELEASE_DIR/${PACKAGE_NAME}.zip"
    fi
    echo ""
    echo -e "${YELLOW}Package size:${NC}"
    echo "  $(du -sh "$RELEASE_DIR/$PACKAGE_NAME" | cut -f1) (uncompressed)"
    echo "  $(du -sh "$RELEASE_DIR/${PACKAGE_NAME}.tar.gz" | cut -f1) (compressed)"
    echo ""
    echo -e "${GREEN}Ready for deployment!${NC}"
}

# Run main function
main "$@"
