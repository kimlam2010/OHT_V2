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
