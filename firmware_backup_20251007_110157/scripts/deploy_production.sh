#!/bin/bash

# OHT-50 Production Deployment Script
# Version: 1.0.0
# Date: $(date)

set -e  # Exit on any error

# Configuration
PROJECT_NAME="OHT-50"
VERSION="1.0.0"
BUILD_DIR="build"
INSTALL_DIR="/opt/oht50"
BACKUP_DIR="/opt/oht50/backup"
LOG_FILE="/var/log/oht50_deployment.log"
SERVICE_NAME="oht50"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging function
log() {
    echo -e "${BLUE}[$(date '+%Y-%m-%d %H:%M:%S')]${NC} $1" | tee -a "$LOG_FILE"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1" | tee -a "$LOG_FILE"
    exit 1
}

success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1" | tee -a "$LOG_FILE"
}

warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1" | tee -a "$LOG_FILE"
}

# Check if running as root
check_root() {
    if [[ $EUID -ne 0 ]]; then
        error "This script must be run as root"
    fi
}

# Check system requirements
check_system_requirements() {
    log "Checking system requirements..."
    
    # Check OS
    if [[ ! -f /etc/os-release ]]; then
        error "Cannot determine OS version"
    fi
    
    # Check available memory
    local mem_total=$(grep MemTotal /proc/meminfo | awk '{print $2}')
    if [[ $mem_total -lt 1048576 ]]; then  # Less than 1GB
        warning "System has less than 1GB RAM: ${mem_total}KB"
    fi
    
    # Check available disk space
    local disk_free=$(df / | tail -1 | awk '{print $4}')
    if [[ $disk_free -lt 1048576 ]]; then  # Less than 1GB
        error "Insufficient disk space: ${disk_free}KB available"
    fi
    
    success "System requirements check passed"
}

# Create backup of existing installation
create_backup() {
    log "Creating backup of existing installation..."
    
    if [[ -d "$INSTALL_DIR" ]]; then
        local backup_name="oht50_backup_$(date +%Y%m%d_%H%M%S)"
        local backup_path="$BACKUP_DIR/$backup_name"
        
        mkdir -p "$BACKUP_DIR"
        cp -r "$INSTALL_DIR" "$backup_path"
        
        # Keep only last 5 backups
        ls -t "$BACKUP_DIR"/oht50_backup_* | tail -n +6 | xargs -r rm -rf
        
        success "Backup created: $backup_path"
    else
        log "No existing installation found, skipping backup"
    fi
}

# Build the project
build_project() {
    log "Building project..."
    
    if [[ ! -d "$BUILD_DIR" ]]; then
        error "Build directory not found: $BUILD_DIR"
    fi
    
    cd "$BUILD_DIR"
    
    # Clean and build
    make clean
    make -j$(nproc)
    
    # Run tests
    log "Running tests..."
    make test
    
    success "Build completed successfully"
}

# Install the application
install_application() {
    log "Installing application..."
    
    # Create installation directory
    mkdir -p "$INSTALL_DIR"
    mkdir -p "$INSTALL_DIR/bin"
    mkdir -p "$INSTALL_DIR/config"
    mkdir -p "$INSTALL_DIR/logs"
    mkdir -p "$INSTALL_DIR/data"
    
    # Copy executable
    cp "$BUILD_DIR/oht50_main" "$INSTALL_DIR/bin/"
    chmod +x "$INSTALL_DIR/bin/oht50_main"
    
    # Copy configuration files
    if [[ -d "$BUILD_DIR/config" ]]; then
        cp -r "$BUILD_DIR/config"/* "$INSTALL_DIR/config/"
    fi
    
    # Copy documentation
    if [[ -d "$BUILD_DIR/docs" ]]; then
        cp -r "$BUILD_DIR/docs" "$INSTALL_DIR/"
    fi
    
    success "Application installed to $INSTALL_DIR"
}

# Create systemd service
create_service() {
    log "Creating systemd service..."
    
    local service_file="/etc/systemd/system/${SERVICE_NAME}.service"
    
    cat > "$service_file" << EOF
[Unit]
Description=OHT-50 Master Module Service
After=network.target
Wants=network.target

[Service]
Type=simple
User=orangepi
Group=orangepi
WorkingDirectory=$INSTALL_DIR
ExecStart=$INSTALL_DIR/bin/oht50_main
Restart=always
RestartSec=5
StandardOutput=journal
StandardError=journal
SyslogIdentifier=oht50

# Security settings
NoNewPrivileges=true
PrivateTmp=true
ProtectSystem=strict
ProtectHome=true
ReadWritePaths=$INSTALL_DIR/logs $INSTALL_DIR/data

# Resource limits
LimitNOFILE=65536
LimitNPROC=4096

[Install]
WantedBy=multi-user.target
EOF
    
    # Reload systemd
    systemctl daemon-reload
    
    success "Systemd service created: $service_file"
}

# Configure logging
configure_logging() {
    log "Configuring logging..."
    
    # Create logrotate configuration
    local logrotate_file="/etc/logrotate.d/oht50"
    
    cat > "$logrotate_file" << EOF
$INSTALL_DIR/logs/*.log {
    daily
    missingok
    rotate 30
    compress
    delaycompress
    notifempty
    create 644 orangepi orangepi
    postrotate
        systemctl reload $SERVICE_NAME > /dev/null 2>&1 || true
    endscript
}
EOF
    
    success "Logging configured"
}

# Configure firewall
configure_firewall() {
    log "Configuring firewall..."
    
    # Check if ufw is available
    if command -v ufw >/dev/null 2>&1; then
        ufw allow 8080/tcp comment "OHT-50 API"
        ufw allow 8081/tcp comment "OHT-50 WebSocket"
        success "Firewall configured (ufw)"
    elif command -v firewall-cmd >/dev/null 2>&1; then
        firewall-cmd --permanent --add-port=8080/tcp
        firewall-cmd --permanent --add-port=8081/tcp
        firewall-cmd --reload
        success "Firewall configured (firewalld)"
    else
        warning "No supported firewall found, manual configuration required"
    fi
}

# Set up monitoring
setup_monitoring() {
    log "Setting up monitoring..."
    
    # Create monitoring script
    local monitor_script="$INSTALL_DIR/bin/monitor.sh"
    
    cat > "$monitor_script" << 'EOF'
#!/bin/bash

# OHT-50 Monitoring Script
SERVICE_NAME="oht50"
LOG_FILE="/var/log/oht50_monitor.log"

# Check if service is running
if ! systemctl is-active --quiet $SERVICE_NAME; then
    echo "$(date): Service $SERVICE_NAME is not running, attempting restart" >> "$LOG_FILE"
    systemctl restart $SERVICE_NAME
fi

# Check memory usage
MEMORY_USAGE=$(ps aux | grep oht50_main | grep -v grep | awk '{print $4}' | head -1)
if [[ -n "$MEMORY_USAGE" && $(echo "$MEMORY_USAGE > 80" | bc -l) -eq 1 ]]; then
    echo "$(date): High memory usage detected: ${MEMORY_USAGE}%" >> "$LOG_FILE"
fi

# Check disk space
DISK_USAGE=$(df /opt/oht50 | tail -1 | awk '{print $5}' | sed 's/%//')
if [[ $DISK_USAGE -gt 90 ]]; then
    echo "$(date): High disk usage detected: ${DISK_USAGE}%" >> "$LOG_FILE"
fi
EOF
    
    chmod +x "$monitor_script"
    
    # Add to crontab
    (crontab -l 2>/dev/null; echo "*/5 * * * * $monitor_script") | crontab -
    
    success "Monitoring configured"
}

# Start the service
start_service() {
    log "Starting service..."
    
    # Enable and start service
    systemctl enable "$SERVICE_NAME"
    systemctl start "$SERVICE_NAME"
    
    # Wait for service to start
    sleep 5
    
    # Check if service is running
    if systemctl is-active --quiet "$SERVICE_NAME"; then
        success "Service started successfully"
    else
        error "Failed to start service"
    fi
}

# Verify installation
verify_installation() {
    log "Verifying installation..."
    
    # Check if executable exists
    if [[ ! -f "$INSTALL_DIR/bin/oht50_main" ]]; then
        error "Executable not found"
    fi
    
    # Check if service is running
    if ! systemctl is-active --quiet "$SERVICE_NAME"; then
        error "Service is not running"
    fi
    
    # Check service status
    systemctl status "$SERVICE_NAME" --no-pager
    
    # Check logs
    if journalctl -u "$SERVICE_NAME" --no-pager -n 10 | grep -q "error\|Error\|ERROR"; then
        warning "Errors found in service logs"
    fi
    
    success "Installation verification completed"
}

# Display deployment summary
display_summary() {
    log "=== DEPLOYMENT SUMMARY ==="
    echo "Project: $PROJECT_NAME"
    echo "Version: $VERSION"
    echo "Installation Directory: $INSTALL_DIR"
    echo "Service Name: $SERVICE_NAME"
    echo "Log File: $LOG_FILE"
    echo ""
    echo "Useful commands:"
    echo "  Check service status: systemctl status $SERVICE_NAME"
    echo "  View logs: journalctl -u $SERVICE_NAME -f"
    echo "  Restart service: systemctl restart $SERVICE_NAME"
    echo "  Stop service: systemctl stop $SERVICE_NAME"
    echo ""
    success "Deployment completed successfully!"
}

# Main deployment function
main() {
    log "Starting OHT-50 production deployment..."
    
    check_root
    check_system_requirements
    create_backup
    build_project
    install_application
    create_service
    configure_logging
    configure_firewall
    setup_monitoring
    start_service
    verify_installation
    display_summary
}

# Run main function
main "$@"
