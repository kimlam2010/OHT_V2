#!/bin/bash
# OHT-50 Production Upgrade Script
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
TIMESTAMP=$(date +%Y%m%d_%H%M%S)

echo -e "${BLUE}=== OHT-50 Production Upgrade ===${NC}"

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

# 1. Create backup directory
echo "1. Creating backup directory..."
mkdir -p "$BACKUP_DIR"
print_status "Created backup directory"

# 2. Stop services
echo "2. Stopping services..."
if systemctl is-active --quiet oht50.service; then
    systemctl stop oht50.service
    print_status "Stopped oht50.service"
else
    print_warning "oht50.service was not running"
fi

# 3. Backup current installation
echo "3. Creating backup..."
BACKUP_NAME="oht50_backup_$TIMESTAMP"
BACKUP_PATH="$BACKUP_DIR/$BACKUP_NAME"

# Backup configuration
if [ -d "$OHT_HOME/config" ]; then
    cp -r "$OHT_HOME/config" "$BACKUP_PATH.config"
    print_status "Backed up configuration"
fi

# Backup logs
if [ -d "$OHT_HOME/logs" ]; then
    cp -r "$OHT_HOME/logs" "$BACKUP_PATH.logs"
    print_status "Backed up logs"
fi

# Backup firmware
if [ -d "$OHT_HOME/firmware" ]; then
    cp -r "$OHT_HOME/firmware" "$BACKUP_PATH.firmware"
    print_status "Backed up firmware"
fi

# Create backup manifest
cat > "$BACKUP_PATH.manifest" << EOF
OHT-50 Backup Manifest
Created: $(date)
Version: $(cat $OHT_HOME/VERSION 2>/dev/null || echo "unknown")
Backup ID: $BACKUP_NAME
Contents:
- Configuration: $BACKUP_PATH.config
- Logs: $BACKUP_PATH.logs  
- Firmware: $BACKUP_PATH.firmware
EOF

print_status "Backup completed: $BACKUP_NAME"

# 4. Update code (if provided)
echo "4. Updating code..."
if [ -d "/tmp/OHT-50" ]; then
    # Remove old code (keep config and logs)
    rm -rf "$OHT_HOME/firmware" "$OHT_HOME/services" "$OHT_HOME/tools" "$OHT_HOME/deploy"
    
    # Copy new code
    cp -r /tmp/OHT-50/* "$OHT_HOME/"
    print_status "Updated code from /tmp/OHT-50"
else
    print_warning "No new code found in /tmp/OHT-50, skipping update"
fi

# 5. Update Python dependencies
echo "5. Updating Python dependencies..."
if [ -f "$OHT_HOME/backend/requirements.txt" ]; then
    "$OHT_HOME/venv/bin/pip" install -r "$OHT_HOME/backend/requirements.txt" --upgrade
    print_status "Updated backend dependencies (backend/)"
elif [ -f "$OHT_HOME/services/backend/requirements.txt" ]; then
    "$OHT_HOME/venv/bin/pip" install -r "$OHT_HOME/services/backend/requirements.txt" --upgrade
    print_status "Updated backend dependencies (services/backend)"
else
    print_warning "Backend requirements.txt not found"
fi

if [ -f "$OHT_HOME/services/center_client/requirements.txt" ]; then
    "$OHT_HOME/venv/bin/pip" install -r "$OHT_HOME/services/center_client/requirements.txt" --upgrade
    print_status "Updated center client dependencies"
fi

# 6. Rebuild firmware
echo "6. Rebuilding firmware..."
if [ -d "$OHT_HOME/firmware" ]; then
    cd "$OHT_HOME/firmware"
    make clean
    make all
    print_status "Rebuilt firmware"
    cd "$OHT_HOME"
else
    print_warning "Firmware directory not found"
fi

# 7. Ensure Node.js present and rebuild frontend
echo "7. Preparing frontend (Node server)..."
if ! command -v node >/dev/null 2>&1; then
  apt-get update
  apt-get install -y nodejs npm || true
fi
if [ -f "$OHT_HOME/frontend/package.json" ]; then
  cd "$OHT_HOME/frontend"
  npm ci || npm install
  npm run build
  chown -R "$OHT_USER:$OHT_GROUP" "$OHT_HOME/frontend"
  print_status "Frontend rebuilt"
else
  print_warning "frontend/package.json not found"
fi

# 8. Update systemd services (including frontend)
echo "8. Updating systemd services..."
if [ -f "$OHT_HOME/deploy/systemd/oht50.service" ]; then
    cp "$OHT_HOME/deploy/systemd/oht50.service" /etc/systemd/system/
    print_status "Updated firmware service"
else
    print_warning "Firmware service unit not found"
fi

if [ -f "$OHT_HOME/deploy/systemd/oht50-backend.service" ]; then
    cp "$OHT_HOME/deploy/systemd/oht50-backend.service" /etc/systemd/system/
    print_status "Updated backend service"
else
    print_warning "Backend service unit not found"
fi

systemctl daemon-reload

# 9. Set permissions
echo "9. Setting permissions..."
chown -R "$OHT_USER:$OHT_GROUP" "$OHT_HOME"
chmod +x "$OHT_HOME/tools/module_cli.py" 2>/dev/null || true
print_status "Set permissions"

# 10. Start services
echo "10. Starting services..."
for svc in oht50.service oht50-backend.service oht50-frontend.service; do
  if systemctl list-unit-files | grep -q "$svc"; then
      systemctl enable "$svc" || true
      systemctl restart "$svc"
      sleep 2
      if systemctl is-active --quiet "$svc"; then
          print_status "$svc started successfully"
      else
          print_error "$svc failed to start"
          systemctl status "$svc" --no-pager -l || true
      fi
  else
      print_warning "Service $svc not found"
  fi
done

# 11. Cleanup old backups (keep last 5)
echo "11. Cleaning up old backups..."
cd "$BACKUP_DIR"
ls -t | tail -n +6 | xargs -r rm -rf
print_status "Cleaned up old backups"

# 12. Final status
echo "12. Final status check..."
if systemctl is-active --quiet oht50.service; then
    print_status "OHT-50 service is running"
else
    print_warning "OHT-50 service is not running"
fi

echo -e "${GREEN}=== Upgrade Complete ===${NC}"
echo -e "Backup location: ${BLUE}$BACKUP_PATH${NC}"
echo -e "Service status: ${BLUE}systemctl status oht50.service${NC}"

# Show service status
echo -e "\n${BLUE}Service Status:${NC}"
systemctl status oht50.service --no-pager -l || true

echo -e "\n${GREEN}Upgrade completed successfully!${NC}"
