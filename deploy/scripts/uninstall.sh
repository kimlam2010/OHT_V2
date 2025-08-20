#!/bin/bash
# OHT-50 Production Uninstall Script
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
OHT_USER="oht"
OHT_GROUP="oht"
OHT_HOME="/opt/oht50"
BACKUP_DIR="/opt/oht50/backups"

echo -e "${BLUE}=== OHT-50 Production Uninstall ===${NC}"

# Check if running as root
if [[ $EUID -ne 0 ]]; then
   echo -e "${RED}Error: This script must be run as root${NC}"
   exit 1
fi

# Function to print status
print_status() {
    echo -e "${GREEN}[OK]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Confirmation prompt
echo -e "${YELLOW}WARNING: This will completely remove OHT-50 from the system!${NC}"
echo -e "This action will:"
echo -e "  - Stop and disable oht50.service"
echo -e "  - Remove user $OHT_USER"
echo -e "  - Delete $OHT_HOME"
echo -e "  - Remove CLI symlink"
echo -e "  - Preserve backups in $BACKUP_DIR"
echo -e ""
read -p "Are you sure you want to continue? (yes/no): " confirm

if [[ $confirm != "yes" ]]; then
    echo -e "${YELLOW}Uninstall cancelled.${NC}"
    exit 0
fi

# 1. Stop and disable services
echo "1. Stopping and disabling services..."
if systemctl is-active --quiet oht50.service; then
    systemctl stop oht50.service
    print_status "Stopped oht50.service"
else
    print_warning "oht50.service was not running"
fi

if systemctl is-enabled --quiet oht50.service; then
    systemctl disable oht50.service
    print_status "Disabled oht50.service"
else
    print_warning "oht50.service was not enabled"
fi

# 2. Remove systemd service file
echo "2. Removing systemd service..."
if [ -f "/etc/systemd/system/oht50.service" ]; then
    rm -f /etc/systemd/system/oht50.service
    systemctl daemon-reload
    print_status "Removed systemd service"
else
    print_warning "Systemd service file not found"
fi

# 3. Remove CLI symlink
echo "3. Removing CLI symlink..."
if [ -L "/usr/local/bin/oht50-cli" ]; then
    rm -f /usr/local/bin/oht50-cli
    print_status "Removed CLI symlink"
else
    print_warning "CLI symlink not found"
fi

# 4. Remove user and group
echo "4. Removing user and group..."
if id "$OHT_USER" &>/dev/null; then
    userdel -r "$OHT_USER" 2>/dev/null || userdel "$OHT_USER"
    print_status "Removed user $OHT_USER"
else
    print_warning "User $OHT_USER not found"
fi

# 5. Backup important data before deletion
echo "5. Creating final backup..."
if [ -d "$OHT_HOME" ]; then
    FINAL_BACKUP="$BACKUP_DIR/final_backup_$(date +%Y%m%d_%H%M%S)"
    mkdir -p "$FINAL_BACKUP"
    
    # Backup configuration
    if [ -d "$OHT_HOME/config" ]; then
        cp -r "$OHT_HOME/config" "$FINAL_BACKUP/"
        print_status "Backed up configuration"
    fi
    
    # Backup logs
    if [ -d "$OHT_HOME/logs" ]; then
        cp -r "$OHT_HOME/logs" "$FINAL_BACKUP/"
        print_status "Backed up logs"
    fi
    
    # Backup modules.yaml if exists
    if [ -f "$OHT_HOME/firmware/modules.yaml" ]; then
        cp "$OHT_HOME/firmware/modules.yaml" "$FINAL_BACKUP/"
        print_status "Backed up modules.yaml"
    fi
    
    # Create uninstall manifest
    cat > "$FINAL_BACKUP/uninstall_manifest.txt" << EOF
OHT-50 Uninstall Manifest
Uninstalled: $(date)
Backup location: $FINAL_BACKUP
Original installation: $OHT_HOME

Contents backed up:
- Configuration files
- Log files  
- Module registry (modules.yaml)

To restore:
1. Run install.sh
2. Copy files from $FINAL_BACKUP to $OHT_HOME
3. Restart services
EOF
    
    print_status "Final backup created: $FINAL_BACKUP"
fi

# 6. Remove installation directory
echo "6. Removing installation directory..."
if [ -d "$OHT_HOME" ]; then
    # Keep backup directory
    if [ -d "$BACKUP_DIR" ]; then
        mv "$BACKUP_DIR" /tmp/oht50_backups_$(date +%Y%m%d_%H%M%S)
        print_status "Moved backups to /tmp"
    fi
    
    rm -rf "$OHT_HOME"
    print_status "Removed installation directory"
else
    print_warning "Installation directory not found"
fi

# 7. Clean up any remaining files
echo "7. Cleaning up remaining files..."
# Remove any remaining symlinks
find /usr/local/bin -name "*oht50*" -type l -delete 2>/dev/null || true

# Remove any remaining service files
find /etc/systemd/system -name "*oht50*" -delete 2>/dev/null || true

print_status "Cleaned up remaining files"

# 8. Final status
echo "8. Final status..."
if [ -d "$OHT_HOME" ]; then
    print_error "Installation directory still exists"
else
    print_status "Installation directory removed"
fi

if systemctl list-unit-files | grep -q "oht50"; then
    print_error "OHT-50 services still exist"
else
    print_status "OHT-50 services removed"
fi

if id "$OHT_USER" &>/dev/null; then
    print_error "User $OHT_USER still exists"
else
    print_status "User $OHT_USER removed"
fi

echo -e "${GREEN}=== Uninstall Complete ===${NC}"
echo -e "OHT-50 has been completely removed from the system."
echo -e "Backups preserved in: ${BLUE}/tmp/oht50_backups_*${NC}"
echo -e ""
echo -e "${YELLOW}Note: System dependencies (python3, build-essential, etc.) were not removed.${NC}"
echo -e "To remove them manually, run:"
echo -e "  apt-get remove python3-venv python3-pip build-essential libgpiod-dev"
echo -e ""
echo -e "${GREEN}Uninstall completed successfully!${NC}"
