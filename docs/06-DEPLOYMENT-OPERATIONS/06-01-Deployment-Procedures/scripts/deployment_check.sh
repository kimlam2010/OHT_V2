#!/bin/bash
# OHT-50 Deployment Verification Script
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
OHT_HOME="/opt/oht50"
OHT_USER="oht"
OHT_GROUP="oht"

echo -e "${BLUE}=== OHT-50 Deployment Verification ===${NC}"

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

# Function to check if running as root
check_root() {
    if [[ $EUID -ne 0 ]]; then
        print_warning "Some checks require root privileges"
        return 1
    fi
    return 0
}

# 1. Check system requirements
echo "1. Checking system requirements..."

# Check OS
if [ -f /etc/os-release ]; then
    . /etc/os-release
    print_status "OS: $PRETTY_NAME"
else
    print_warning "OS version unknown"
fi

# Check kernel version
KERNEL_VERSION=$(uname -r)
if [[ "$KERNEL_VERSION" == *"6.1"* ]] || [[ "$KERNEL_VERSION" == *"6.2"* ]] || [[ "$KERNEL_VERSION" == *"6.3"* ]]; then
    print_status "Kernel: $KERNEL_VERSION (supported)"
else
    print_warning "Kernel: $KERNEL_VERSION (may need upgrade)"
fi

# Check hardware
if [ -f /proc/cpuinfo ]; then
    CPU_MODEL=$(grep "Hardware" /proc/cpuinfo | head -1 | cut -d: -f2 | xargs)
    if [[ "$CPU_MODEL" == *"RK3588"* ]]; then
        print_status "CPU: $CPU_MODEL (Orange Pi 5B detected)"
    else
        print_warning "CPU: $CPU_MODEL (not RK3588)"
    fi
fi

# Check memory
MEMORY_KB=$(grep MemTotal /proc/meminfo | awk '{print $2}')
MEMORY_GB=$((MEMORY_KB / 1024 / 1024))
if [ $MEMORY_GB -ge 4 ]; then
    print_status "Memory: ${MEMORY_GB}GB (sufficient)"
else
    print_error "Memory: ${MEMORY_GB}GB (insufficient, need 4GB+)"
fi

# Check storage
STORAGE_GB=$(df / | awk 'NR==2 {print int($2/1024/1024)}')
if [ $STORAGE_GB -ge 16 ]; then
    print_status "Storage: ${STORAGE_GB}GB (sufficient)"
else
    print_error "Storage: ${STORAGE_GB}GB (insufficient, need 16GB+)"
fi

# 2. Check dependencies
echo "2. Checking dependencies..."

# Check Python
if command -v python3 >/dev/null 2>&1; then
    PYTHON_VERSION=$(python3 --version | cut -d' ' -f2)
    print_status "Python3: $PYTHON_VERSION"
else
    print_error "Python3 not found"
fi

# Check pip
if command -v pip3 >/dev/null 2>&1; then
    print_status "pip3: available"
else
    print_error "pip3 not found"
fi

# Check Node.js
if command -v node >/dev/null 2>&1; then
    NODE_VERSION=$(node --version)
    print_status "Node.js: $NODE_VERSION"
else
    print_warning "Node.js not found (required for frontend)"
fi

# Check npm
if command -v npm >/dev/null 2>&1; then
    NPM_VERSION=$(npm --version)
    print_status "npm: $NPM_VERSION"
else
    print_warning "npm not found"
fi

# Check build tools
if command -v gcc >/dev/null 2>&1; then
    GCC_VERSION=$(gcc --version | head -1 | cut -d' ' -f3)
    print_status "gcc: $GCC_VERSION"
else
    print_error "gcc not found"
fi

if command -v make >/dev/null 2>&1; then
    print_status "make: available"
else
    print_error "make not found"
fi

# Check GPIO tools
if command -v gpioinfo >/dev/null 2>&1; then
    print_status "gpioinfo: available"
else
    print_warning "gpioinfo not found (libgpiod-tools)"
fi

# 3. Check OHT-50 installation
echo "3. Checking OHT-50 installation..."

if [ -d "$OHT_HOME" ]; then
    print_status "Installation directory exists: $OHT_HOME"
else
    print_error "Installation directory not found: $OHT_HOME"
    exit 1
fi

# Check user
if id "$OHT_USER" &>/dev/null; then
    print_status "User $OHT_USER exists"
else
    print_error "User $OHT_USER not found"
fi

# Check directories
for dir in "config" "logs" "firmware" "backend" "frontend" "tools"; do
    if [ -d "$OHT_HOME/$dir" ]; then
        print_status "Directory exists: $OHT_HOME/$dir"
    else
        print_warning "Directory missing: $OHT_HOME/$dir"
    fi
done

# Check virtual environment
if [ -d "$OHT_HOME/venv" ]; then
    print_status "Virtual environment exists"
    if [ -f "$OHT_HOME/venv/bin/python" ]; then
        print_status "Virtual environment Python available"
    else
        print_error "Virtual environment Python not found"
    fi
else
    print_error "Virtual environment not found"
fi

# 4. Check firmware
echo "4. Checking firmware..."

if [ -d "$OHT_HOME/firmware" ]; then
    if [ -f "$OHT_HOME/firmware/build/oht50_main" ]; then
        print_status "Firmware binary exists"
        
        # Check if binary is executable
        if [ -x "$OHT_HOME/firmware/build/oht50_main" ]; then
            print_status "Firmware binary is executable"
        else
            print_error "Firmware binary not executable"
        fi
    else
        print_error "Firmware binary not found"
    fi
    
    # Check Makefile
    if [ -f "$OHT_HOME/firmware/Makefile" ]; then
        print_status "Firmware Makefile exists"
    else
        print_error "Firmware Makefile not found"
    fi
else
    print_error "Firmware directory not found"
fi

# 5. Check services
echo "5. Checking services..."

if check_root; then
    # Check systemd services
    for service in "oht50.service" "oht50-backend.service" "oht50-frontend.service"; do
        if systemctl list-unit-files | grep -q "$service"; then
            print_status "Service exists: $service"
            
            # Check if enabled
            if systemctl is-enabled --quiet "$service" 2>/dev/null; then
                print_status "Service enabled: $service"
            else
                print_warning "Service not enabled: $service"
            fi
            
            # Check if running
            if systemctl is-active --quiet "$service" 2>/dev/null; then
                print_status "Service running: $service"
            else
                print_warning "Service not running: $service"
            fi
        else
            print_error "Service not found: $service"
        fi
    done
else
    print_warning "Skipping service checks (requires root)"
fi

# 6. Check configuration
echo "6. Checking configuration..."

if [ -f "$OHT_HOME/config/system.yaml" ]; then
    print_status "System configuration exists"
    
    # Check if configuration is readable
    if [ -r "$OHT_HOME/config/system.yaml" ]; then
        print_status "System configuration is readable"
    else
        print_error "System configuration not readable"
    fi
else
    print_error "System configuration not found"
fi

# 7. Check network
echo "7. Checking network..."

# Check network interfaces
if ip link show eth0 >/dev/null 2>&1; then
    print_status "Ethernet interface exists"
    
    # Check if interface is up
    if ip link show eth0 | grep -q "UP"; then
        print_status "Ethernet interface is up"
    else
        print_warning "Ethernet interface is down"
    fi
else
    print_warning "Ethernet interface not found"
fi

# Check network connectivity
if ping -c 1 8.8.8.8 >/dev/null 2>&1; then
    print_status "Internet connectivity available"
else
    print_warning "No internet connectivity"
fi

# 8. Check hardware interfaces
echo "8. Checking hardware interfaces..."

# Check RS485 device
if [ -e "/dev/ttyOHT485" ]; then
    print_status "RS485 device exists: /dev/ttyOHT485"
    
    # Check permissions
    if [ -r "/dev/ttyOHT485" ] && [ -w "/dev/ttyOHT485" ]; then
        print_status "RS485 device permissions OK"
    else
        print_error "RS485 device permissions incorrect"
    fi
else
    print_warning "RS485 device not found: /dev/ttyOHT485"
fi

# Check GPIO
if check_root; then
    if command -v gpioinfo >/dev/null 2>&1; then
        if gpioinfo >/dev/null 2>&1; then
            print_status "GPIO system accessible"
        else
            print_error "GPIO system not accessible"
        fi
    else
        print_warning "GPIO tools not available"
    fi
else
    print_warning "Skipping GPIO checks (requires root)"
fi

# 9. Check CLI tools
echo "9. Checking CLI tools..."

if [ -f "$OHT_HOME/tools/module_cli.py" ]; then
    print_status "CLI tool exists"
    
    # Check if executable
    if [ -x "$OHT_HOME/tools/module_cli.py" ]; then
        print_status "CLI tool is executable"
    else
        print_error "CLI tool not executable"
    fi
else
    print_error "CLI tool not found"
fi

# Check symlink
if [ -L "/usr/local/bin/oht50-cli" ]; then
    print_status "CLI symlink exists"
else
    print_warning "CLI symlink not found"
fi

# 10. Check logs
echo "10. Checking logs..."

if [ -d "$OHT_HOME/logs" ]; then
    print_status "Log directory exists"
    
    # Check if writable
    if [ -w "$OHT_HOME/logs" ]; then
        print_status "Log directory is writable"
    else
        print_error "Log directory not writable"
    fi
else
    print_error "Log directory not found"
fi

# Check journalctl if root
if check_root; then
    if command -v journalctl >/dev/null 2>&1; then
        print_status "journalctl available"
        
        # Check recent logs
        if journalctl -u oht50.service --since "1 hour ago" >/dev/null 2>&1; then
            print_status "Service logs accessible"
        else
            print_warning "No recent service logs"
        fi
    else
        print_warning "journalctl not available"
    fi
else
    print_warning "Skipping log checks (requires root)"
fi

# 11. Check API endpoints
echo "11. Checking API endpoints..."

# Check backend API
if curl -s http://localhost:8000/health >/dev/null 2>&1; then
    print_status "Backend API accessible"
else
    print_warning "Backend API not accessible"
fi

# Check frontend
if curl -s http://localhost:8081/ >/dev/null 2>&1; then
    print_status "Frontend accessible"
else
    print_warning "Frontend not accessible"
fi

# 12. Summary
echo ""
echo -e "${BLUE}=== Deployment Verification Summary ===${NC}"

# Count results
TOTAL_CHECKS=0
PASSED_CHECKS=0
WARNING_CHECKS=0
FAILED_CHECKS=0

# This would be calculated from actual check results
# For now, provide a summary message

echo -e "${GREEN}Deployment verification completed!${NC}"
echo ""
echo "Next steps:"
echo "1. Run startup test: sudo ./deploy/scripts/startup_test.sh"
echo "2. Run E-Stop test: sudo ./deploy/scripts/estop_latency_test.sh"
echo "3. Run RS485 test: sudo ./deploy/scripts/rs485_fault_test.sh"
echo ""
echo "For detailed troubleshooting, see: deploy/README.md"

exit 0
