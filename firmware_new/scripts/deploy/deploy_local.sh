#!/bin/bash

# OHT-50 Local Deployment Script
# Deploys the application to user's home directory without root privileges

set -e

# Configuration
APP_NAME="oht50"
INSTALL_DIR="$HOME/oht50"
BUILD_DIR="build"
PID_FILE="$INSTALL_DIR/oht50.pid"
LOG_FILE="$INSTALL_DIR/oht50.log"
STARTUP_SCRIPT="$INSTALL_DIR/start_oht50.sh"
MONITOR_SCRIPT="$INSTALL_DIR/monitor.sh"

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

success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running from correct directory
check_working_directory() {
    if [ ! -f "CMakeLists.txt" ]; then
        error "Please run this script from the firmware_new directory"
        exit 1
    fi
}

# Check system requirements
check_system_requirements() {
    log "Checking system requirements..."
    
    # Check memory
    MEMORY_KB=$(grep MemAvailable /proc/meminfo | awk '{print $2}')
    if [ "$MEMORY_KB" -lt 1000000 ]; then
        error "Insufficient memory: ${MEMORY_KB}KB available, need at least 1GB"
        exit 1
    fi
    success "Memory: ${MEMORY_KB}KB available"
    
    # Check disk space
    DISK_KB=$(df . | tail -1 | awk '{print $4}')
    if [ "$DISK_KB" -lt 10000000 ]; then
        error "Insufficient disk space: ${DISK_KB}KB available, need at least 10GB"
        exit 1
    fi
    success "Disk space: ${DISK_KB}KB available"
    
    success "System requirements check passed"
}

# Create backup of existing installation
create_backup() {
    if [ -d "$INSTALL_DIR" ]; then
        log "Creating backup of existing installation..."
        BACKUP_DIR="$INSTALL_DIR.backup.$(date +%Y%m%d_%H%M%S)"
        cp -r "$INSTALL_DIR" "$BACKUP_DIR"
        success "Backup created: $BACKUP_DIR"
    else
        log "No existing installation found, skipping backup"
    fi
}

# Build project
build_project() {
    log "Building project..."
    
    if [ ! -d "$BUILD_DIR" ]; then
        mkdir -p "$BUILD_DIR"
    fi
    
    cd "$BUILD_DIR"
    
    # Configure with CMake
    cmake .. -DCMAKE_BUILD_TYPE=Release
    
    # Build only the main application
    make oht50_main -j$(nproc)
    
    cd ..
    
    success "Project built successfully"
}

# Install application
install_application() {
    log "Installing application..."
    
    # Create installation directory
    mkdir -p "$INSTALL_DIR"
    
    # Copy executable
    cp "$BUILD_DIR/oht50_main" "$INSTALL_DIR/$APP_NAME"
    chmod +x "$INSTALL_DIR/$APP_NAME"
    
    # Create configuration directory
    mkdir -p "$INSTALL_DIR/config"
    
    # Create logs directory
    mkdir -p "$INSTALL_DIR/logs"
    
    success "Application installed to $INSTALL_DIR"
}

# Create startup script
create_startup_script() {
    log "Creating startup script..."
    
    cat > "$STARTUP_SCRIPT" << 'EOF'
#!/bin/bash

# OHT-50 Startup Script
# Starts the OHT-50 application in background

APP_NAME="oht50"
INSTALL_DIR="$(dirname "$0")"
PID_FILE="$INSTALL_DIR/oht50.pid"
LOG_FILE="$INSTALL_DIR/logs/oht50.log"

# Create logs directory if it doesn't exist
mkdir -p "$INSTALL_DIR/logs"

# Check if already running
if [ -f "$PID_FILE" ]; then
    PID=$(cat "$PID_FILE")
    if ps -p "$PID" > /dev/null 2>&1; then
        echo "OHT-50 is already running with PID $PID"
        exit 1
    else
        echo "Removing stale PID file"
        rm -f "$PID_FILE"
    fi
fi

# Start the application
echo "Starting OHT-50..."
cd "$INSTALL_DIR"
nohup ./$APP_NAME > "$LOG_FILE" 2>&1 &
PID=$!

# Save PID
echo $PID > "$PID_FILE"
echo "OHT-50 started with PID $PID"
echo "Log file: $LOG_FILE"
echo "PID file: $PID_FILE"
EOF

    chmod +x "$STARTUP_SCRIPT"
    success "Startup script created: $STARTUP_SCRIPT"
}

# Create monitoring script
create_monitoring_script() {
    log "Creating monitoring script..."
    
    cat > "$MONITOR_SCRIPT" << 'EOF'
#!/bin/bash

# OHT-50 Monitoring Script
# Monitors the OHT-50 application status

APP_NAME="oht50"
INSTALL_DIR="$(dirname "$0")"
PID_FILE="$INSTALL_DIR/oht50.pid"
LOG_FILE="$INSTALL_DIR/logs/oht50.log"

# Check if application is running
check_status() {
    if [ -f "$PID_FILE" ]; then
        PID=$(cat "$PID_FILE")
        if ps -p "$PID" > /dev/null 2>&1; then
            echo "OHT-50 is running with PID $PID"
            return 0
        else
            echo "OHT-50 is not running (stale PID file)"
            return 1
        fi
    else
        echo "OHT-50 is not running (no PID file)"
        return 1
    fi
}

# Restart application if not running
restart_if_needed() {
    if ! check_status; then
        echo "Restarting OHT-50..."
        "$INSTALL_DIR/start_oht50.sh"
        sleep 2
        check_status
    fi
}

# Show log tail
show_logs() {
    if [ -f "$LOG_FILE" ]; then
        echo "=== Recent logs ==="
        tail -20 "$LOG_FILE"
    else
        echo "No log file found"
    fi
}

# Main monitoring logic
case "$1" in
    status)
        check_status
        ;;
    restart)
        restart_if_needed
        ;;
    logs)
        show_logs
        ;;
    *)
        echo "Usage: $0 {status|restart|logs}"
        exit 1
        ;;
esac
EOF

    chmod +x "$MONITOR_SCRIPT"
    success "Monitoring script created: $MONITOR_SCRIPT"
}

# Test application
test_application() {
    log "Testing application..."
    
    # Check if executable exists
    if [ ! -f "$INSTALL_DIR/$APP_NAME" ]; then
        error "Application executable not found"
        exit 1
    fi
    
    # Test basic functionality
    if "$INSTALL_DIR/$APP_NAME" --help > /dev/null 2>&1; then
        success "Application test passed"
    else
        warning "Application test failed (this might be normal if --help is not supported)"
    fi
}

# Display summary
display_summary() {
    echo
    echo "=========================================="
    echo "           DEPLOYMENT SUMMARY"
    echo "=========================================="
    echo "Application: $APP_NAME"
    echo "Installation Directory: $INSTALL_DIR"
    echo "Executable: $INSTALL_DIR/$APP_NAME"
    echo "Startup Script: $STARTUP_SCRIPT"
    echo "Monitoring Script: $MONITOR_SCRIPT"
    echo "PID File: $PID_FILE"
    echo "Log File: $LOG_FILE"
    echo
    echo "To start the application:"
    echo "  $STARTUP_SCRIPT"
    echo
    echo "To check status:"
    echo "  $MONITOR_SCRIPT status"
    echo
    echo "To restart if needed:"
    echo "  $MONITOR_SCRIPT restart"
    echo
    echo "To view logs:"
    echo "  $MONITOR_SCRIPT logs"
    echo
    echo "To stop the application:"
    echo "  kill \$(cat $PID_FILE)"
    echo
    success "Deployment completed successfully!"
}

# Main deployment process
main() {
    echo "🚀 OHT-50 Local Deployment"
    echo "=========================="
    
    check_working_directory
    check_system_requirements
    create_backup
    build_project
    install_application
    create_startup_script
    create_monitoring_script
    test_application
    display_summary
}

# Run main function
main "$@"
