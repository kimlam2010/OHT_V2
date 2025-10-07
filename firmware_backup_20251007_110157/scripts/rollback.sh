#!/bin/bash

# OHT-50 Firmware Rollback Script
# ISO/IEC 12207 Compliant Rollback
# Version: 1.0.0

set -e  # Exit on any error

# Configuration
FIRMWARE_NAME="oht50_main"
BACKUP_DIR="/opt/oht50/backup"
INSTALL_DIR="/opt/oht50/firmware"
LOG_FILE="/var/log/oht50_rollback.log"

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

# List available backups
list_backups() {
    log "Available backups:"
    
    if [[ ! -d "$BACKUP_DIR" ]]; then
        error_exit "Backup directory not found: $BACKUP_DIR"
    fi
    
    BACKUPS=($(ls -t "$BACKUP_DIR"/${FIRMWARE_NAME}_* 2>/dev/null || true))
    
    if [[ ${#BACKUPS[@]} -eq 0 ]]; then
        error_exit "No backups found in $BACKUP_DIR"
    fi
    
    echo -e "${YELLOW}Available backups:${NC}"
    for i in "${!BACKUPS[@]}"; do
        BACKUP_FILE=$(basename "${BACKUPS[$i]}")
        BACKUP_DATE=$(echo "$BACKUP_FILE" | sed 's/.*_\([0-9]\{8\}_[0-9]\{6\}\)/\1/')
        echo "  $((i+1)). $BACKUP_FILE ($BACKUP_DATE)"
    done
    
    return 0
}

# Select backup to restore
select_backup() {
    BACKUPS=($(ls -t "$BACKUP_DIR"/${FIRMWARE_NAME}_* 2>/dev/null))
    
    if [[ ${#BACKUPS[@]} -eq 0 ]]; then
        error_exit "No backups found"
    fi
    
    if [[ $# -eq 1 ]]; then
        # Backup specified as argument
        SELECTED_INDEX=$1
        if [[ $SELECTED_INDEX -lt 1 || $SELECTED_INDEX -gt ${#BACKUPS[@]} ]]; then
            error_exit "Invalid backup selection: $SELECTED_INDEX"
        fi
    else
        # Interactive selection
        echo -e "${YELLOW}Select backup to restore (1-${#BACKUPS[@]}):${NC}"
        read -r SELECTED_INDEX
        
        if [[ ! $SELECTED_INDEX =~ ^[0-9]+$ ]] || [[ $SELECTED_INDEX -lt 1 || $SELECTED_INDEX -gt ${#BACKUPS[@]} ]]; then
            error_exit "Invalid selection: $SELECTED_INDEX"
        fi
    fi
    
    SELECTED_BACKUP="${BACKUPS[$((SELECTED_INDEX-1))]}"
    log "Selected backup: $SELECTED_BACKUP"
    
    echo -e "${YELLOW}Selected: $(basename "$SELECTED_BACKUP")${NC}"
}

# Stop running firmware
stop_firmware() {
    log "Stopping running firmware..."
    
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

# Restore backup
restore_backup() {
    log "Restoring backup: $SELECTED_BACKUP"
    
    # Verify backup file exists and is readable
    if [[ ! -f "$SELECTED_BACKUP" ]]; then
        error_exit "Backup file not found: $SELECTED_BACKUP"
    fi
    
    if [[ ! -r "$SELECTED_BACKUP" ]]; then
        error_exit "Backup file not readable: $SELECTED_BACKUP"
    fi
    
    # Create backup of current firmware before restoring
    if [[ -f "$INSTALL_DIR/$FIRMWARE_NAME" ]]; then
        CURRENT_BACKUP="$BACKUP_DIR/${FIRMWARE_NAME}_current_$(date +%Y%m%d_%H%M%S)"
        cp "$INSTALL_DIR/$FIRMWARE_NAME" "$CURRENT_BACKUP"
        log "Current firmware backed up to: $CURRENT_BACKUP"
    fi
    
    # Restore the selected backup
    cp "$SELECTED_BACKUP" "$INSTALL_DIR/$FIRMWARE_NAME"
    chmod +x "$INSTALL_DIR/$FIRMWARE_NAME"
    
    log "Backup restored successfully"
}

# Verify restoration
verify_restoration() {
    log "Verifying restoration..."
    
    # Check if file exists and is executable
    if [[ ! -f "$INSTALL_DIR/$FIRMWARE_NAME" ]]; then
        error_exit "Restored firmware not found"
    fi
    
    if [[ ! -x "$INSTALL_DIR/$FIRMWARE_NAME" ]]; then
        error_exit "Restored firmware is not executable"
    fi
    
    # Check file size (should be reasonable)
    FILE_SIZE=$(stat -c%s "$INSTALL_DIR/$FIRMWARE_NAME")
    if [[ $FILE_SIZE -lt 10000 ]]; then
        error_exit "Restored firmware seems too small: ${FILE_SIZE} bytes"
    fi
    
    # Verify file integrity (basic check)
    BACKUP_SIZE=$(stat -c%s "$SELECTED_BACKUP")
    if [[ $FILE_SIZE -ne $BACKUP_SIZE ]]; then
        error_exit "File size mismatch: backup=${BACKUP_SIZE}, restored=${FILE_SIZE}"
    fi
    
    log "Restoration verification passed"
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

# Main rollback function
main() {
    echo -e "${GREEN}=== OHT-50 Firmware Rollback ===${NC}"
    log "Starting rollback process"
    
    check_root
    list_backups
    select_backup "$@"
    stop_firmware
    restore_backup
    verify_restoration
    start_firmware
    health_check
    
    echo -e "${GREEN}Rollback completed successfully!${NC}"
    log "Rollback completed successfully"
}

# Show usage
usage() {
    echo "Usage: $0 [backup_number]"
    echo "  backup_number: Optional backup number to restore (1, 2, 3, etc.)"
    echo "  If not specified, will show interactive selection"
    echo ""
    echo "Examples:"
    echo "  $0          # Interactive selection"
    echo "  $0 1        # Restore most recent backup"
    echo "  $0 2        # Restore second most recent backup"
}

# Check for help
if [[ "$1" == "-h" || "$1" == "--help" ]]; then
    usage
    exit 0
fi

# Run main function
main "$@"
