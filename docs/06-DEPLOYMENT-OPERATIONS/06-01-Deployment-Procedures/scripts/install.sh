#!/bin/bash
# OHT-50 Production Installation Script
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
VENV_PATH="$OHT_HOME/venv"
CONFIG_DIR="$OHT_HOME/config"
LOG_DIR="$OHT_HOME/logs"

echo -e "${BLUE}=== OHT-50 Production Installation ===${NC}"

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

# 1. Create user and group
echo "1. Creating user and group..."
if ! id "$OHT_USER" &>/dev/null; then
    useradd -r -s /bin/bash -d "$OHT_HOME" -m "$OHT_USER"
    print_status "Created user $OHT_USER"
else
    print_warning "User $OHT_USER already exists"
fi

# 2. Create directories
echo "2. Creating directories..."
mkdir -p "$OHT_HOME" "$CONFIG_DIR" "$LOG_DIR"
chown -R "$OHT_USER:$OHT_GROUP" "$OHT_HOME"
print_status "Created directories"

# 3. Install system dependencies
echo "3. Installing system dependencies..."
apt-get update
apt-get install -y python3 python3-venv python3-pip git curl

# Install firmware build dependencies
apt-get install -y build-essential gcc make cmake

# Install RS485/GPIO dependencies (handle distro differences)
apt-get install -y libgpiod-dev || true
# Some distros provide 'gpiod' instead of 'libgpiod-tools'
if ! apt-get install -y gpiod; then
    apt-get install -y libgpiod-tools || true
fi

print_status "Installed system dependencies"

# 4. Clone/copy OHT-50 code
echo "4. Setting up OHT-50 code..."
if [ -d "/tmp/OHT-50" ]; then
    cp -r /tmp/OHT-50/* "$OHT_HOME/"
    print_status "Copied OHT-50 code from /tmp"
else
    # Fallback: copy from current directory if this looks like the repo
    if [ -d "firmware" ] || [ -d "services" ] || [ -d "tools" ]; then
        cp -r . "$OHT_HOME/"
        print_status "Copied OHT-50 code from current directory"
    else
        print_warning "OHT-50 code not found; please place repo at /tmp/OHT-50 or run installer from repo root"
    fi
fi

# 5. Set up Python virtual environment
echo "5. Setting up Python virtual environment..."
cd "$OHT_HOME"
python3 -m venv "$VENV_PATH"
print_status "Created virtual environment"

# 6. Install Python dependencies (canonical backend path)
echo "6. Installing Python dependencies..."
"$VENV_PATH/bin/pip" install --upgrade pip

# Prefer canonical backend path at repo root; fallback to legacy services/backend
if [ -f "backend/requirements.txt" ]; then
    "$VENV_PATH/bin/pip" install -r backend/requirements.txt
    print_status "Installed backend dependencies (backend/)"
elif [ -f "services/backend/requirements.txt" ]; then
    "$VENV_PATH/bin/pip" install -r services/backend/requirements.txt
    print_status "Installed backend dependencies (services/backend)"
else
    print_warning "Backend requirements.txt not found"
fi

# Center client dependencies (if present)
if [ -f "services/center_client/requirements.txt" ]; then
    "$VENV_PATH/bin/pip" install -r services/center_client/requirements.txt
    print_status "Installed center client dependencies"
fi

# 7. Build firmware
echo "7. Building firmware..."
if [ -d "firmware" ]; then
    cd firmware
    make clean
    make all
    print_status "Built firmware successfully"
    cd ..
else
    print_warning "Firmware directory not found"
fi

# 8. Create default configuration
echo "8. Creating default configuration..."
cat > "$CONFIG_DIR/system.yaml" << EOF
# OHT-50 System Configuration
# Generated: $(date)

device:
  id: "OHT50-001"
  name: "OHT-50 Master Module"
  location: "Factory Floor 1"

firmware:
  log_level: "INFO"
  scan_interval: 30
  heartbeat_interval: 30
  telemetry_interval: 60

modules:
  power:
    address: 0x02
    type: "power"
    name: "power_main"
  motor:
    address: 0x03
    type: "motor"
    name: "motor_main"
  io:
    address: 0x04
    type: "io"
    name: "io_main"
  dock:
    address: 0x05
    type: "dock"
    name: "dock_main"

safety:
  estop_timeout: 100
  max_voltage: 18.0
  max_current: 13.0
  max_temperature: 10.0

# Center configuration (for future use)
center:
  host: "localhost"
  port: 8080
  path: "/ws/oht50"
  enabled: false
  reconnect_initial: 5
  reconnect_max: 300
EOF

chown "$OHT_USER:$OHT_GROUP" "$CONFIG_DIR/system.yaml"
print_status "Created default configuration"

# 9. Install systemd services (firmware + backend)
echo "9. Installing systemd services..."
if [ -f "deploy/systemd/oht50.service" ]; then
    cp deploy/systemd/oht50.service /etc/systemd/system/
    print_status "Installed firmware service unit"
else
    print_warning "Firmware service unit not found"
fi

if [ -f "deploy/systemd/oht50-backend.service" ]; then
    cp deploy/systemd/oht50-backend.service /etc/systemd/system/
    print_status "Installed backend service unit"
else
    print_warning "Backend service unit not found"
fi

systemctl daemon-reload

# 10. Set permissions
echo "10. Setting permissions..."
chown -R "$OHT_USER:$OHT_GROUP" "$OHT_HOME"
chmod +x "$OHT_HOME/tools/module_cli.py"
print_status "Set permissions"

# 11. Install Node.js for frontend (if not present)
echo "11. Installing Node.js (for frontend service)..."
if ! command -v node >/dev/null 2>&1; then
  apt-get install -y nodejs npm || true
  # Some distros require NodeSource; skip for offline
fi

# 12. Build frontend (production)
echo "12. Building frontend (production)..."
if [ -f "$OHT_HOME/frontend/package.json" ]; then
  cd "$OHT_HOME/frontend"
  npm ci || npm install
  npm run build
  chown -R "$OHT_USER:$OHT_GROUP" "$OHT_HOME/frontend"
  print_status "Frontend built"
else
  print_warning "frontend/package.json not found"
fi

# 13. Install systemd services (frontend already copied with code)
echo "13. Installing/Enabling services..."
for svc in oht50.service oht50-backend.service oht50-frontend.service; do
  if [ -f "$OHT_HOME/deploy/systemd/$svc" ]; then
    cp "$OHT_HOME/deploy/systemd/$svc" /etc/systemd/system/
  fi
done
systemctl daemon-reload

for svc in oht50.service oht50-backend.service oht50-frontend.service; do
  if systemctl list-unit-files | grep -q "$svc"; then
      systemctl enable "$svc"
      systemctl start "$svc"
      print_status "Service $svc enabled and started"
  else
      print_warning "Service $svc not found, skipping"
  fi
done

# 14. Create CLI symlink
echo "14. Creating CLI symlink..."
ln -sf "$OHT_HOME/tools/module_cli.py" /usr/local/bin/oht50-cli
print_status "Created CLI symlink"

# 15. Final status check
echo "15. Final status check..."
if systemctl is-active --quiet oht50.service; then
    print_status "OHT-50 service is running"
else
    print_warning "OHT-50 service is not running"
fi

echo -e "${GREEN}=== Installation Complete ===${NC}"
echo -e "Installation directory: ${BLUE}$OHT_HOME${NC}"
echo -e "Configuration: ${BLUE}$CONFIG_DIR/system.yaml${NC}"
echo -e "Logs: ${BLUE}$LOG_DIR${NC}"
echo -e "CLI tool: ${BLUE}oht50-cli${NC}"
echo -e "Service status: ${BLUE}systemctl status oht50.service${NC}"

# Show service status
echo -e "\n${BLUE}Service Status:${NC}"
systemctl status oht50.service --no-pager -l || true

echo -e "\n${GREEN}Installation completed successfully!${NC}"
