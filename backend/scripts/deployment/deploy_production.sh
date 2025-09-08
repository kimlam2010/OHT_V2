#!/bin/bash

# OHT-50 Backend Production Deployment Script
# This script deploys the OHT-50 Backend to production environment

set -e  # Exit on any error

# Configuration
BACKEND_DIR="/opt/oht50/backend"
SERVICE_NAME="oht50-backend"
SERVICE_USER="oht50"
SERVICE_GROUP="oht50"
LOG_DIR="/var/log/oht50"
DATA_DIR="/var/lib/oht50"
CONFIG_DIR="/etc/oht50"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging function
log() {
    echo -e "${BLUE}[$(date +'%Y-%m-%d %H:%M:%S')]${NC} $1"
}

error() {
    echo -e "${RED}[ERROR]${NC} $1" >&2
}

success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Check if running as root
check_root() {
    if [[ $EUID -eq 0 ]]; then
        error "This script should not be run as root for security reasons"
        exit 1
    fi
}

# Check system requirements
check_requirements() {
    log "Checking system requirements..."
    
    # Check Python version
    if ! command -v python3 &> /dev/null; then
        error "Python 3.11+ is required but not installed"
        exit 1
    fi
    
    PYTHON_VERSION=$(python3 -c 'import sys; print(".".join(map(str, sys.version_info[:2])))')
    if [[ $(echo "$PYTHON_VERSION < 3.11" | bc -l) -eq 1 ]]; then
        error "Python 3.11+ is required, found $PYTHON_VERSION"
        exit 1
    fi
    
    # Check required packages
    REQUIRED_PACKAGES=("docker" "docker-compose" "git" "curl" "jq")
    for package in "${REQUIRED_PACKAGES[@]}"; do
        if ! command -v $package &> /dev/null; then
            error "$package is required but not installed"
            exit 1
        fi
    done
    
    success "System requirements check passed"
}

# Create system user and directories
setup_system() {
    log "Setting up system user and directories..."
    
    # Create service user if it doesn't exist
    if ! id "$SERVICE_USER" &>/dev/null; then
        sudo useradd -r -s /bin/false -d "$BACKEND_DIR" "$SERVICE_USER"
        success "Created service user: $SERVICE_USER"
    else
        log "Service user already exists: $SERVICE_USER"
    fi
    
    # Create directories
    sudo mkdir -p "$BACKEND_DIR" "$LOG_DIR" "$DATA_DIR" "$CONFIG_DIR"
    sudo chown -R "$SERVICE_USER:$SERVICE_GROUP" "$BACKEND_DIR" "$LOG_DIR" "$DATA_DIR" "$CONFIG_DIR"
    sudo chmod 755 "$BACKEND_DIR" "$LOG_DIR" "$DATA_DIR" "$CONFIG_DIR"
    
    success "System directories created and configured"
}

# Clone or update repository
setup_repository() {
    log "Setting up repository..."
    
    if [ -d "$BACKEND_DIR/.git" ]; then
        log "Updating existing repository..."
        cd "$BACKEND_DIR"
        sudo -u "$SERVICE_USER" git fetch origin
        sudo -u "$SERVICE_USER" git reset --hard origin/main
    else
        log "Cloning repository..."
        sudo -u "$SERVICE_USER" git clone https://github.com/oht50/backend.git "$BACKEND_DIR"
    fi
    
    success "Repository setup completed"
}

# Install Python dependencies
install_dependencies() {
    log "Installing Python dependencies..."
    
    cd "$BACKEND_DIR"
    
    # Create virtual environment
    if [ ! -d "venv" ]; then
        sudo -u "$SERVICE_USER" python3 -m venv venv
    fi
    
    # Activate virtual environment and install dependencies
    sudo -u "$SERVICE_USER" bash -c "
        source venv/bin/activate
        pip install --upgrade pip
        pip install -r requirements.txt
    "
    
    success "Python dependencies installed"
}

# Setup configuration
setup_configuration() {
    log "Setting up configuration..."
    
    # Copy environment file
    if [ ! -f "$CONFIG_DIR/.env" ]; then
        sudo cp "$BACKEND_DIR/env.example" "$CONFIG_DIR/.env"
        sudo chown "$SERVICE_USER:$SERVICE_GROUP" "$CONFIG_DIR/.env"
        sudo chmod 600 "$CONFIG_DIR/.env"
        
        warning "Please edit $CONFIG_DIR/.env with your production settings"
    fi
    
    # Create database directory
    sudo mkdir -p "$DATA_DIR/database"
    sudo chown "$SERVICE_USER:$SERVICE_GROUP" "$DATA_DIR/database"
    
    # Create logs directory
    sudo mkdir -p "$LOG_DIR/backend"
    sudo chown "$SERVICE_USER:$SERVICE_GROUP" "$LOG_DIR/backend"
    
    success "Configuration setup completed"
}

# Run database migrations
run_migrations() {
    log "Running database migrations..."
    
    cd "$BACKEND_DIR"
    
    sudo -u "$SERVICE_USER" bash -c "
        source venv/bin/activate
        export DATABASE_URL=sqlite:///$DATA_DIR/database/oht50.db
        alembic upgrade head
    "
    
    success "Database migrations completed"
}

# Create systemd service
create_systemd_service() {
    log "Creating systemd service..."
    
    cat > /tmp/oht50-backend.service << EOF
[Unit]
Description=OHT-50 Backend Service
After=network.target
Wants=network.target

[Service]
Type=exec
User=$SERVICE_USER
Group=$SERVICE_GROUP
WorkingDirectory=$BACKEND_DIR
Environment=PATH=$BACKEND_DIR/venv/bin
Environment=DATABASE_URL=sqlite:///$DATA_DIR/database/oht50.db
Environment=LOG_LEVEL=INFO
Environment=LOG_FILE=$LOG_DIR/backend/oht50_backend.log
ExecStart=$BACKEND_DIR/venv/bin/uvicorn app.main:app --host 0.0.0.0 --port 8000
ExecReload=/bin/kill -HUP \$MAINPID
Restart=always
RestartSec=10
StandardOutput=journal
StandardError=journal
SyslogIdentifier=oht50-backend

[Install]
WantedBy=multi-user.target
EOF
    
    sudo mv /tmp/oht50-backend.service /etc/systemd/system/
    sudo systemctl daemon-reload
    sudo systemctl enable "$SERVICE_NAME"
    
    success "Systemd service created and enabled"
}

# Setup Docker services
setup_docker_services() {
    log "Setting up Docker services..."
    
    cd "$BACKEND_DIR"
    
    # Create production docker-compose file
    cat > docker-compose.prod.yml << EOF
version: '3.8'

services:
  backend:
    build: .
    container_name: oht50-backend
    ports:
      - "8000:8000"
    environment:
      - ENVIRONMENT=production
      - DATABASE_URL=sqlite:///data/database/oht50.db
      - JWT_SECRET=\${JWT_SECRET}
      - FIRMWARE_URL=\${FIRMWARE_URL}
      - LOG_LEVEL=INFO
    volumes:
      - $DATA_DIR:/app/data
      - $LOG_DIR:/app/logs
      - $CONFIG_DIR:/app/config
    restart: unless-stopped
    depends_on:
      - redis
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:8000/health"]
      interval: 30s
      timeout: 10s
      retries: 3
      start_period: 40s

  redis:
    image: redis:7-alpine
    container_name: oht50-redis
    ports:
      - "6379:6379"
    volumes:
      - redis_data:/data
    restart: unless-stopped
    command: redis-server --appendonly yes

  nginx:
    image: nginx:alpine
    container_name: oht50-nginx
    ports:
      - "80:80"
      - "443:443"
    volumes:
      - ./nginx.conf:/etc/nginx/nginx.conf
      - ./ssl:/etc/nginx/ssl
    restart: unless-stopped
    depends_on:
      - backend

volumes:
  redis_data:
EOF
    
    # Create nginx configuration
    cat > nginx.conf << EOF
events {
    worker_connections 1024;
}

http {
    upstream backend {
        server backend:8000;
    }
    
    server {
        listen 80;
        server_name _;
        
        location / {
            proxy_pass http://backend;
            proxy_set_header Host \$host;
            proxy_set_header X-Real-IP \$remote_addr;
            proxy_set_header X-Forwarded-For \$proxy_add_x_forwarded_for;
            proxy_set_header X-Forwarded-Proto \$scheme;
        }
        
        location /ws {
            proxy_pass http://backend;
            proxy_http_version 1.1;
            proxy_set_header Upgrade \$http_upgrade;
            proxy_set_header Connection "upgrade";
            proxy_set_header Host \$host;
            proxy_set_header X-Real-IP \$remote_addr;
            proxy_set_header X-Forwarded-For \$proxy_add_x_forwarded_for;
            proxy_set_header X-Forwarded-Proto \$scheme;
        }
    }
}
EOF
    
    success "Docker services configuration created"
}

# Build and start services
start_services() {
    log "Building and starting services..."
    
    cd "$BACKEND_DIR"
    
    # Build Docker images
    sudo -u "$SERVICE_USER" docker-compose -f docker-compose.prod.yml build
    
    # Start services
    sudo -u "$SERVICE_USER" docker-compose -f docker-compose.prod.yml up -d
    
    success "Services started successfully"
}

# Setup monitoring
setup_monitoring() {
    log "Setting up monitoring..."
    
    # Create monitoring script
    cat > /tmp/monitor.sh << 'EOF'
#!/bin/bash

# OHT-50 Backend Monitoring Script
SERVICE_NAME="oht50-backend"
LOG_FILE="/var/log/oht50/monitor.log"
ALERT_EMAIL="admin@oht50.com"

# Check service status
check_service() {
    if systemctl is-active --quiet "$SERVICE_NAME"; then
        echo "$(date): Service is running" >> "$LOG_FILE"
        return 0
    else
        echo "$(date): Service is not running" >> "$LOG_FILE"
        systemctl restart "$SERVICE_NAME"
        echo "Service restarted" | mail -s "OHT-50 Backend Restarted" "$ALERT_EMAIL"
        return 1
    fi
}

# Check health endpoint
check_health() {
    if curl -f -s http://localhost:8000/health > /dev/null; then
        echo "$(date): Health check passed" >> "$LOG_FILE"
        return 0
    else
        echo "$(date): Health check failed" >> "$LOG_FILE"
        echo "Health check failed" | mail -s "OHT-50 Backend Health Check Failed" "$ALERT_EMAIL"
        return 1
    fi
}

# Check disk space
check_disk_space() {
    DISK_USAGE=$(df / | awk 'NR==2 {print $5}' | sed 's/%//')
    if [ "$DISK_USAGE" -gt 90 ]; then
        echo "$(date): Disk usage is $DISK_USAGE%" >> "$LOG_FILE"
        echo "Disk usage is $DISK_USAGE%" | mail -s "OHT-50 Backend Disk Usage Alert" "$ALERT_EMAIL"
    fi
}

# Check memory usage
check_memory() {
    MEMORY_USAGE=$(free | awk 'NR==2{printf "%.0f", $3*100/$2}')
    if [ "$MEMORY_USAGE" -gt 90 ]; then
        echo "$(date): Memory usage is $MEMORY_USAGE%" >> "$LOG_FILE"
        echo "Memory usage is $MEMORY_USAGE%" | mail -s "OHT-50 Backend Memory Usage Alert" "$ALERT_EMAIL"
    fi
}

# Run all checks
check_service
check_health
check_disk_space
check_memory
EOF
    
    sudo mv /tmp/monitor.sh /usr/local/bin/oht50-monitor.sh
    sudo chmod +x /usr/local/bin/oht50-monitor.sh
    sudo chown "$SERVICE_USER:$SERVICE_GROUP" /usr/local/bin/oht50-monitor.sh
    
    # Create cron job for monitoring
    echo "*/5 * * * * /usr/local/bin/oht50-monitor.sh" | sudo crontab -u "$SERVICE_USER" -
    
    success "Monitoring setup completed"
}

# Setup backup
setup_backup() {
    log "Setting up backup..."
    
    # Create backup script
    cat > /tmp/backup.sh << 'EOF'
#!/bin/bash

# OHT-50 Backend Backup Script
BACKUP_DIR="/var/backups/oht50"
DATA_DIR="/var/lib/oht50"
LOG_DIR="/var/log/oht50"
DATE=$(date +%Y%m%d_%H%M%S)

# Create backup directory
mkdir -p "$BACKUP_DIR"

# Backup database
cp "$DATA_DIR/database/oht50.db" "$BACKUP_DIR/oht50_$DATE.db"

# Backup logs
tar -czf "$BACKUP_DIR/logs_$DATE.tar.gz" "$LOG_DIR"

# Backup configuration
tar -czf "$BACKUP_DIR/config_$DATE.tar.gz" "/etc/oht50"

# Keep only last 7 days of backups
find "$BACKUP_DIR" -name "*.db" -mtime +7 -delete
find "$BACKUP_DIR" -name "*.tar.gz" -mtime +7 -delete

echo "$(date): Backup completed" >> "$LOG_DIR/backup.log"
EOF
    
    sudo mv /tmp/backup.sh /usr/local/bin/oht50-backup.sh
    sudo chmod +x /usr/local/bin/oht50-backup.sh
    sudo chown "$SERVICE_USER:$SERVICE_GROUP" /usr/local/bin/oht50-backup.sh
    
    # Create cron job for backup
    echo "0 2 * * * /usr/local/bin/oht50-backup.sh" | sudo crontab -u "$SERVICE_USER" -
    
    success "Backup setup completed"
}

# Verify deployment
verify_deployment() {
    log "Verifying deployment..."
    
    # Wait for services to start
    sleep 30
    
    # Check service status
    if systemctl is-active --quiet "$SERVICE_NAME"; then
        success "Service is running"
    else
        error "Service is not running"
        exit 1
    fi
    
    # Check health endpoint
    if curl -f -s http://localhost:8000/health > /dev/null; then
        success "Health check passed"
    else
        error "Health check failed"
        exit 1
    fi
    
    # Check API endpoints
    if curl -f -s http://localhost:8000/docs > /dev/null; then
        success "API documentation accessible"
    else
        error "API documentation not accessible"
        exit 1
    fi
    
    success "Deployment verification completed"
}

# Main deployment function
main() {
    log "Starting OHT-50 Backend production deployment..."
    
    check_root
    check_requirements
    setup_system
    setup_repository
    install_dependencies
    setup_configuration
    run_migrations
    create_systemd_service
    setup_docker_services
    start_services
    setup_monitoring
    setup_backup
    verify_deployment
    
    success "OHT-50 Backend production deployment completed successfully!"
    
    log "Next steps:"
    log "1. Edit $CONFIG_DIR/.env with your production settings"
    log "2. Configure SSL certificates in $BACKEND_DIR/ssl/"
    log "3. Test the system thoroughly"
    log "4. Monitor logs in $LOG_DIR/backend/"
    log "5. Access the system at http://localhost:8000"
    
    log "Useful commands:"
    log "- Check service status: sudo systemctl status $SERVICE_NAME"
    log "- View logs: sudo journalctl -u $SERVICE_NAME -f"
    log "- Restart service: sudo systemctl restart $SERVICE_NAME"
    log "- Stop service: sudo systemctl stop $SERVICE_NAME"
}

# Run main function
main "$@"
