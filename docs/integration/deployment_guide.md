# 🚀 FW-BE Integration Deployment Guide

## 📋 **Deployment Overview**
**Version:** 1.0.0  
**Date:** 2025-01-27  
**Team:** Integration  
**Task:** INT-02 (Documentation & Deployment)  

---

## 🎯 **Prerequisites**

### **Hardware Requirements**
- **Orange Pi 5B (RK3588)** với firmware đã deploy
- **Network connectivity** giữa FW và BE servers
- **Minimum 2GB RAM** cho backend server
- **10GB storage** cho logs và data

### **Software Requirements**
- **Python 3.8+** cho backend
- **Node.js 16+** (optional cho frontend)
- **Docker** (optional cho containerized deployment)
- **Git** cho version control

---

## 📦 **Installation Steps**

### **1. Firmware Deployment**

```bash
# SSH vào Orange Pi 5B
ssh orangepi@192.168.1.100

# Clone repository
git clone https://github.com/your-org/OHT-50.git
cd OHT-50

# Build firmware
cd firmware
make all

# Deploy firmware
sudo ./deploy.sh

# Verify deployment
sudo systemctl status oht50
```

### **2. Backend Deployment**

```bash
# Clone backend repository
git clone https://github.com/your-org/oht50-backend.git
cd oht50-backend

# Create virtual environment
python3 -m venv venv
source venv/bin/activate

# Install dependencies
pip install -r requirements.txt

# Install FW integration libraries
pip install aiohttp websockets
```

### **3. Configuration Setup**

```bash
# Create configuration file
cp config/config.example.py config/config.py

# Edit configuration
nano config/config.py
```

**Configuration Example:**
```python
# FW Integration Configuration
FW_HOST = '192.168.1.100'  # Orange Pi 5B IP
FW_HTTP_PORT = 8080
FW_WS_PORT = 8081
FW_TIMEOUT = 30
FW_AUTH_TOKEN = 'your-auth-token'
FW_SECRET_KEY = 'your-secret-key'

# Backend Configuration
BACKEND_HOST = '0.0.0.0'
BACKEND_PORT = 5000
DEBUG = False
LOG_LEVEL = 'INFO'
```

---

## 🔧 **Configuration Details**

### **FW Client Configuration**
```python
from backend.lib.fw_client import FWConfig

fw_config = FWConfig(
    host="192.168.1.100",
    http_port=8080,
    ws_port=8081,
    timeout=30,
    auth_token="your-auth-token"
)
```

### **Backend App Configuration**
```python
from flask import Flask
from backend.routes.fw_integration import init_fw_integration

app = Flask(__name__)

# Configure FW integration
app.config['FW_HOST'] = '192.168.1.100'
app.config['FW_HTTP_PORT'] = 8080
app.config['FW_WS_PORT'] = 8081
app.config['FW_TIMEOUT'] = 30
app.config['FW_AUTH_TOKEN'] = 'your-auth-token'
app.config['FW_SECRET_KEY'] = 'your-secret-key'

# Initialize FW integration
init_fw_integration(app)
```

---

## 🚀 **Deployment Methods**

### **Method 1: Direct Deployment**

```bash
# Start backend server
python app.py

# Or with gunicorn
gunicorn -w 4 -b 0.0.0.0:5000 app:app
```

### **Method 2: Docker Deployment**

```dockerfile
# Dockerfile
FROM python:3.9-slim

WORKDIR /app
COPY requirements.txt .
RUN pip install -r requirements.txt

COPY . .
EXPOSE 5000

CMD ["gunicorn", "-w", "4", "-b", "0.0.0.0:5000", "app:app"]
```

```bash
# Build and run Docker container
docker build -t oht50-backend .
docker run -d -p 5000:5000 --name oht50-backend oht50-backend
```

### **Method 3: Systemd Service**

```bash
# Create systemd service file
sudo nano /etc/systemd/system/oht50-backend.service
```

```ini
[Unit]
Description=OHT-50 Backend Service
After=network.target

[Service]
Type=simple
User=www-data
WorkingDirectory=/opt/oht50-backend
Environment=PATH=/opt/oht50-backend/venv/bin
ExecStart=/opt/oht50-backend/venv/bin/gunicorn -w 4 -b 0.0.0.0:5000 app:app
Restart=always
RestartSec=10

[Install]
WantedBy=multi-user.target
```

```bash
# Enable and start service
sudo systemctl daemon-reload
sudo systemctl enable oht50-backend
sudo systemctl start oht50-backend
sudo systemctl status oht50-backend
```

---

## 🧪 **Testing Deployment**

### **1. Connection Test**

```bash
# Test FW connection
curl -X POST http://localhost:5000/api/v1/fw/connect

# Expected response:
{
  "success": true,
  "message": "Connected to FW successfully",
  "status": "connected"
}
```

### **2. System Status Test**

```bash
# Get system status
curl -X GET http://localhost:5000/api/v1/fw/system/status

# Expected response:
{
  "success": true,
  "data": {
    "system_name": "OHT-50 Master Module",
    "version": "1.0.0",
    "status": "running",
    "uptime_ms": 1234567890,
    "active_modules": 3,
    "estop_active": false,
    "safety_ok": true
  },
  "timestamp": "2025-01-27T10:30:00"
}
```

### **3. Module Command Test**

```bash
# Send module command
curl -X POST http://localhost:5000/api/v1/fw/modules/1/command \
  -H "Content-Type: application/json" \
  -d '{"command": "get_status", "parameters": {}}'

# Expected response:
{
  "success": true,
  "data": {
    "status": "idle",
    "position": 0.0,
    "velocity": 0.0
  },
  "timestamp": "2025-01-27T10:30:00"
}
```

---

## 📊 **Monitoring & Logging**

### **Log Configuration**

```python
import logging

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('/var/log/oht50-backend/app.log'),
        logging.StreamHandler()
    ]
)
```

### **Health Check Endpoint**

```python
@app.route('/health')
def health_check():
    return jsonify({
        'status': 'healthy',
        'timestamp': datetime.now().isoformat(),
        'fw_connected': fw_client.is_connected() if fw_client else False
    })
```

### **Monitoring Commands**

```bash
# Check service status
sudo systemctl status oht50-backend

# View logs
sudo journalctl -u oht50-backend -f

# Check FW connection
curl -X GET http://localhost:5000/api/v1/fw/status

# Health check
curl -X GET http://localhost:5000/health
```

---

## 🔒 **Security Configuration**

### **Authentication Setup**

```python
# Generate secure tokens
import secrets

auth_token = secrets.token_urlsafe(32)
secret_key = secrets.token_urlsafe(32)

print(f"Auth Token: {auth_token}")
print(f"Secret Key: {secret_key}")
```

### **SSL/TLS Configuration**

```python
# For production, use SSL
if not app.debug:
    app.config['FW_ENABLE_SSL'] = True
    app.config['FW_VERIFY_SSL'] = True
```

### **Firewall Configuration**

```bash
# Allow only necessary ports
sudo ufw allow 5000/tcp  # Backend API
sudo ufw allow 8080/tcp  # FW HTTP API
sudo ufw allow 8081/tcp  # FW WebSocket
sudo ufw enable
```

---

## 🔄 **Update & Maintenance**

### **Update Process**

```bash
# Stop service
sudo systemctl stop oht50-backend

# Backup current version
cp -r /opt/oht50-backend /opt/oht50-backend.backup

# Update code
cd /opt/oht50-backend
git pull origin main

# Update dependencies
source venv/bin/activate
pip install -r requirements.txt

# Start service
sudo systemctl start oht50-backend
sudo systemctl status oht50-backend
```

### **Rollback Process**

```bash
# Stop service
sudo systemctl stop oht50-backend

# Restore backup
rm -rf /opt/oht50-backend
cp -r /opt/oht50-backend.backup /opt/oht50-backend

# Start service
sudo systemctl start oht50-backend
```

---

## 🚨 **Troubleshooting**

### **Common Issues**

#### **1. FW Connection Failed**
```bash
# Check FW service status
sudo systemctl status oht50

# Check network connectivity
ping 192.168.1.100

# Check FW API directly
curl -X GET http://192.168.1.100:8080/api/v1/system/health
```

#### **2. Backend Service Won't Start**
```bash
# Check logs
sudo journalctl -u oht50-backend -n 50

# Check configuration
python -c "import config; print(config.FW_HOST)"

# Check dependencies
pip list | grep aiohttp
```

#### **3. WebSocket Connection Issues**
```bash
# Test WebSocket connection
wscat -c ws://192.168.1.100:8081

# Check firewall
sudo ufw status
```

### **Debug Mode**

```python
# Enable debug mode
app.config['DEBUG'] = True
app.config['LOG_LEVEL'] = 'DEBUG'

# Run with debug
python app.py
```

---

## 📈 **Performance Optimization**

### **Connection Pooling**

```python
# Configure connection pooling
app.config['FW_MAX_CONNECTIONS'] = 10
app.config['FW_CONNECTION_TIMEOUT'] = 30
```

### **Caching**

```python
# Add Redis caching
import redis

redis_client = redis.Redis(host='localhost', port=6379, db=0)

# Cache system status
def get_cached_system_status():
    cached = redis_client.get('system_status')
    if cached:
        return json.loads(cached)
    
    status = fw_client.get_system_status()
    redis_client.setex('system_status', 30, json.dumps(status))
    return status
```

### **Load Balancing**

```nginx
# Nginx configuration
upstream oht50_backend {
    server 127.0.0.1:5000;
    server 127.0.0.1:5001;
    server 127.0.0.1:5002;
}

server {
    listen 80;
    server_name your-domain.com;
    
    location / {
        proxy_pass http://oht50_backend;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
    }
}
```

---

## 📋 **Deployment Checklist**

### **Pre-Deployment**
- [ ] Hardware requirements met
- [ ] Network connectivity verified
- [ ] Dependencies installed
- [ ] Configuration files created
- [ ] Security tokens generated
- [ ] Firewall configured

### **Deployment**
- [ ] Firmware deployed and tested
- [ ] Backend deployed and started
- [ ] FW integration initialized
- [ ] Connection established
- [ ] API endpoints tested
- [ ] Logging configured

### **Post-Deployment**
- [ ] Health checks passing
- [ ] Performance monitoring active
- [ ] Backup procedures tested
- [ ] Documentation updated
- [ ] Team training completed
- [ ] Support procedures established

---

## 📞 **Support & Contact**

### **Emergency Contacts**
- **Technical Lead:** tech-lead@company.com
- **System Administrator:** sysadmin@company.com
- **On-call Engineer:** oncall@company.com

### **Documentation Links**
- **API Documentation:** `/docs/api`
- **System Architecture:** `/docs/architecture`
- **Troubleshooting Guide:** `/docs/troubleshooting`

---

**Status:** ✅ DEPLOYMENT GUIDE COMPLETED  
**Next Step:** Execute deployment checklist  
**Estimated Time:** 2-4 hours
