#!/bin/bash

# OHT-50 Firmware Deployment Script
# ISO/IEC 12207 Compliant Deployment
# Version: 1.0.0

set -e  # Exit on any error

# Configuration
FIRMWARE_NAME="oht50_main"
BACKUP_DIR="/opt/oht50/backup"
INSTALL_DIR="/opt/oht50/firmware"
LOG_FILE="/var/log/oht50_deploy.log"
VERSION_FILE="/opt/oht50/version.txt"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Logging function
log() {
    echo "[$(date '+%Y-%m-%d %H:%M:%S')] $1" | tee -a "$LOG_FILE"
}

# Error handling
error_exit() {
    log "ERROR: $1"
    echo -e "${RED}ERROR: $1${NC}"
    exit 1
}

# Check if running as root
check_root() {
    if [[ $EUID -ne 0 ]]; then
        error_exit "This script must be run as root"
    fi
}

# Check system requirements
check_system() {
    log "Checking system requirements..."
    
    # Check if backup directory exists
    if [[ ! -d "$BACKUP_DIR" ]]; then
        log "Creating backup directory: $BACKUP_DIR"
        mkdir -p "$BACKUP_DIR"
    fi
    
    # Check if install directory exists
    if [[ ! -d "$INSTALL_DIR" ]]; then
        log "Creating install directory: $INSTALL_DIR"
        mkdir -p "$INSTALL_DIR"
    fi
    
    # Check available disk space (minimum 100MB)
    AVAILABLE_SPACE=$(df "$INSTALL_DIR" | awk 'NR==2 {print $4}')
    if [[ $AVAILABLE_SPACE -lt 102400 ]]; then
        error_exit "Insufficient disk space. Need at least 100MB, available: ${AVAILABLE_SPACE}KB"
    fi
    
    log "System requirements check passed"
}

# Create backup of current firmware
create_backup() {
    log "Creating backup of current firmware..."
    
    if [[ -f "$INSTALL_DIR/$FIRMWARE_NAME" ]]; then
        BACKUP_FILE="$BACKUP_DIR/${FIRMWARE_NAME}_$(date +%Y%m%d_%H%M%S)"
        cp "$INSTALL_DIR/$FIRMWARE_NAME" "$BACKUP_FILE"
        log "Backup created: $BACKUP_FILE"
    else
        log "No existing firmware found, skipping backup"
    fi
}

# Stop running firmware
stop_firmware() {
    log "Stopping running firmware..."
    
    # Try to stop gracefully first
    if pgrep -f "$FIRMWARE_NAME" > /dev/null; then
        log "Sending SIGTERM to running firmware..."
        pkill -TERM -f "$FIRMWARE_NAME"
        
        # Wait up to 10 seconds for graceful shutdown
        for i in {1..10}; do
            if ! pgrep -f "$FIRMWARE_NAME" > /dev/null; then
                log "Firmware stopped gracefully"
                return 0
            fi
            sleep 1
        done
        
        # Force kill if still running
        log "Force killing firmware..."
        pkill -KILL -f "$FIRMWARE_NAME"
        sleep 2
    else
        log "No running firmware found"
    fi
}

# Deploy new firmware
deploy_firmware() {
    log "Deploying new firmware..."
    
    # Copy new firmware
    if [[ ! -f "build/$FIRMWARE_NAME" ]]; then
        error_exit "Build file not found: build/$FIRMWARE_NAME"
    fi
    
    cp "build/$FIRMWARE_NAME" "$INSTALL_DIR/$FIRMWARE_NAME"
    chmod +x "$INSTALL_DIR/$FIRMWARE_NAME"
    
    # Update version file
    echo "$(date '+%Y-%m-%d %H:%M:%S') - Version: $(git describe --tags --always 2>/dev/null || echo 'unknown')" > "$VERSION_FILE"
    
    log "Firmware deployed successfully"
}

# Verify deployment
verify_deployment() {
    log "Verifying deployment..."
    
    # Check if file exists and is executable
    if [[ ! -f "$INSTALL_DIR/$FIRMWARE_NAME" ]]; then
        error_exit "Deployed firmware not found"
    fi
    
    if [[ ! -x "$INSTALL_DIR/$FIRMWARE_NAME" ]]; then
        error_exit "Deployed firmware is not executable"
    fi
    
    # Check file size (should be reasonable)
    FILE_SIZE=$(stat -c%s "$INSTALL_DIR/$FIRMWARE_NAME")
    if [[ $FILE_SIZE -lt 10000 ]]; then
        error_exit "Deployed firmware seems too small: ${FILE_SIZE} bytes"
    fi
    
    log "Deployment verification passed"
}

# Start firmware
start_firmware() {
    log "Starting firmware..."
    
    cd "$INSTALL_DIR"
    nohup "./$FIRMWARE_NAME" > /var/log/oht50_firmware.log 2>&1 &
    FIRMWARE_PID=$!
    
    # Wait a moment for startup
    sleep 3
    
    # Check if process is running
    if kill -0 "$FIRMWARE_PID" 2>/dev/null; then
        log "Firmware started successfully (PID: $FIRMWARE_PID)"
        echo "$FIRMWARE_PID" > /var/run/oht50_firmware.pid
    else
        error_exit "Failed to start firmware"
    fi
}

# Health check
health_check() {
    log "Performing health check..."
    
    # Wait for firmware to stabilize
    sleep 5
    
    # Check if process is still running
    if ! pgrep -f "$FIRMWARE_NAME" > /dev/null; then
        error_exit "Firmware process not running after startup"
    fi
    
    # Check log for errors (basic check)
    if tail -n 20 /var/log/oht50_firmware.log | grep -i "error\|fatal\|crash" > /dev/null; then
        log "WARNING: Errors detected in firmware log"
    fi
    
    log "Health check passed"
}

# Main deployment function
main() {
    echo -e "${GREEN}=== OHT-50 Firmware Deployment ===${NC}"
    log "Starting deployment process"
    
    check_root
    check_system
    create_backup
    stop_firmware
    deploy_firmware
    verify_deployment
    start_firmware
    health_check
    
    echo -e "${GREEN}Deployment completed successfully!${NC}"
    log "Deployment completed successfully"
}

# Run main function
main "$@"
