# 🚀 OHT-50 DEPLOYMENT GUIDE

**Version:** 1.0.0  
**Last Updated:** 2025-01-28  
**Team:** PM (Project Management)  
**Status:** ✅ PRODUCTION READY  
**Target Platform:** Orange Pi 5B (RK3588)

---

## 📋 **TỔNG QUAN**

Thư mục `deploy/` chứa tất cả tài liệu và script cần thiết để triển khai OHT-50 Master Module vào production environment.

### **Cấu trúc thư mục:**
```
deploy/
├── README.md                    # Tài liệu này
├── scripts/                     # Deployment scripts
│   ├── install.sh              # Production installation
│   ├── upgrade.sh              # System upgrade
│   ├── uninstall.sh            # Complete uninstall
│   ├── startup_test.sh         # Startup verification
│   ├── estop_latency_test.sh   # E-Stop performance test
│   └── rs485_fault_test.sh     # RS485 fault injection test
└── systemd/                     # Systemd service files
    ├── oht50.service           # Main firmware service
    ├── oht50-backend.service   # Backend API service
    └── oht50-frontend.service  # Frontend service
```

---

## 🎯 **MỤC TIÊU DEPLOYMENT**

### **Production Requirements:**
- ✅ **System Reliability:** 99.9% uptime target
- ✅ **Safety Compliance:** SIL2 standards
- ✅ **Performance:** E-Stop < 100ms response time
- ✅ **Security:** Authentication và encryption
- ✅ **Monitoring:** Real-time health checks
- ✅ **Maintenance:** Easy upgrade và rollback

### **Deployment Phases:**
1. **Phase 1:** Hardware preparation (Orange Pi 5B)
2. **Phase 2:** System installation (OS, dependencies)
3. **Phase 3:** OHT-50 software deployment
4. **Phase 4:** Configuration và testing
5. **Phase 5:** Production validation

---

## 🔧 **SYSTEM REQUIREMENTS**

### **Hardware Requirements:**
- **Platform:** Orange Pi 5B (RK3588)
- **RAM:** Minimum 4GB, Recommended 8GB
- **Storage:** Minimum 16GB eMMC/SD card
- **Network:** Ethernet 10/100/1000 Mbps
- **USB:** USB 2.0 Type-C (debug)
- **GPIO:** RS485, LED indicators, E-Stop, Relay outputs

### **Software Requirements:**
- **OS:** Ubuntu 22.04 LTS hoặc Armbian
- **Kernel:** Linux 6.1+ với RK3588 support
- **Python:** 3.8+ với venv support
- **Node.js:** 18+ (cho frontend)
- **Build Tools:** gcc, make, cmake
- **Systemd:** Service management

### **Network Requirements:**
- **Ethernet:** Static IP hoặc DHCP
- **Firewall:** Port 8000 (backend), 8081 (frontend)
- **DNS:** Hostname resolution
- **NTP:** Time synchronization

---

## 📦 **INSTALLATION PROCESS**

### **Quick Installation:**
```bash
# 1. Clone repository
git clone https://github.com/your-org/OHT-50.git
cd OHT-50

# 2. Run installation script
sudo ./deploy/scripts/install.sh

# 3. Verify installation
sudo ./deploy/scripts/startup_test.sh
```

### **Detailed Installation Steps:**

#### **Step 1: System Preparation**
```bash
# Update system
sudo apt update && sudo apt upgrade -y

# Install dependencies
sudo apt install -y python3 python3-venv python3-pip git curl
sudo apt install -y build-essential gcc make cmake
sudo apt install -y libgpiod-dev gpiod
```

#### **Step 2: OHT-50 Installation**
```bash
# Run installation script
sudo ./deploy/scripts/install.sh
```

#### **Step 3: Configuration**
```bash
# Edit system configuration
sudo nano /opt/oht50/config/system.yaml

# Set network configuration
sudo nano /etc/netplan/01-netcfg.yaml
```

#### **Step 4: Verification**
```bash
# Run startup test
sudo ./deploy/scripts/startup_test.sh

# Check service status
sudo systemctl status oht50.service
```

---

## 🔄 **UPGRADE PROCESS**

### **Automatic Upgrade:**
```bash
# 1. Prepare new code
cp -r /path/to/new/OHT-50 /tmp/OHT-50

# 2. Run upgrade script
sudo ./deploy/scripts/upgrade.sh

# 3. Verify upgrade
sudo ./deploy/scripts/startup_test.sh
```

### **Manual Upgrade:**
```bash
# 1. Stop services
sudo systemctl stop oht50.service oht50-backend.service oht50-frontend.service

# 2. Backup current installation
sudo cp -r /opt/oht50 /opt/oht50.backup.$(date +%Y%m%d_%H%M%S)

# 3. Update code
sudo cp -r /path/to/new/OHT-50/* /opt/oht50/

# 4. Rebuild firmware
cd /opt/oht50/firmware
sudo -u oht make clean && sudo -u oht make all

# 5. Update dependencies
sudo -u oht /opt/oht50/venv/bin/pip install -r backend/requirements.txt --upgrade

# 6. Restart services
sudo systemctl start oht50.service oht50-backend.service oht50-frontend.service
```

---

## 🧪 **TESTING & VALIDATION**

### **Startup Test:**
```bash
# Verify system startup sequence
sudo ./deploy/scripts/startup_test.sh
```

**Expected Results:**
- ✅ Service starts within 30s
- ✅ LED system initialized
- ✅ E-Stop safety system ready
- ✅ RS485 communication established
- ✅ Module scanning active
- ✅ Backend API accessible
- ✅ System reaches IDLE state within 120s

### **E-Stop Latency Test:**
```bash
# Measure E-Stop response time
sudo ./deploy/scripts/estop_latency_test.sh
```

**Expected Results:**
- ✅ E-Stop latency < 100ms
- ✅ Emergency handling activated
- ✅ Safety state transition recorded

### **RS485 Fault Test:**
```bash
# Test RS485 fault handling
sudo ./deploy/scripts/rs485_fault_test.sh
```

**Expected Results:**
- ✅ Timeout errors handled gracefully
- ✅ CRC errors detected (if supported)
- ✅ Error statistics recorded

---

## 📊 **MONITORING & MAINTENANCE**

### **Service Status:**
```bash
# Check all services
sudo systemctl status oht50.service oht50-backend.service oht50-frontend.service

# View logs
sudo journalctl -u oht50.service -f
sudo journalctl -u oht50-backend.service -f
sudo journalctl -u oht50-frontend.service -f
```

### **Health Checks:**
```bash
# Backend health
curl http://localhost:8000/health

# Frontend status
curl http://localhost:8081/health

# CLI tools
oht50-cli status
```

### **Log Management:**
```bash
# View recent logs
sudo journalctl -u oht50.service --since "1 hour ago"

# Export logs
sudo journalctl -u oht50.service --since "2025-01-28" > oht50_logs.txt

# Clear old logs
sudo journalctl --vacuum-time=7d
```

---

## 🔒 **SECURITY CONSIDERATIONS**

### **Access Control:**
- **User:** `oht` (dedicated service user)
- **Permissions:** Minimal required privileges
- **Network:** Firewall rules for required ports
- **Files:** Secure file permissions

### **Network Security:**
```bash
# Configure firewall
sudo ufw allow 8000/tcp  # Backend API
sudo ufw allow 8081/tcp  # Frontend
sudo ufw enable
```

### **Service Security:**
- Services run as dedicated user `oht`
- No root privileges for application code
- Secure configuration file permissions
- Encrypted communication (HTTPS/WSS)

---

## 🚨 **TROUBLESHOOTING**

### **Common Issues:**

#### **Service Won't Start:**
```bash
# Check service status
sudo systemctl status oht50.service

# View detailed logs
sudo journalctl -u oht50.service --no-pager -l

# Check dependencies
sudo systemctl list-dependencies oht50.service
```

#### **RS485 Communication Issues:**
```bash
# Check device permissions
ls -la /dev/ttyOHT485

# Test RS485 manually
sudo ./firmware/build/test_rs485

# Check GPIO configuration
sudo gpioinfo
```

#### **Backend API Issues:**
```bash
# Check backend service
sudo systemctl status oht50-backend.service

# Test API directly
curl http://localhost:8000/health

# Check Python environment
sudo -u oht /opt/oht50/venv/bin/python -c "import app; print('OK')"
```

#### **Frontend Issues:**
```bash
# Check frontend service
sudo systemctl status oht50-frontend.service

# Test frontend directly
curl http://localhost:8081/

# Check Node.js installation
node --version
npm --version
```

### **Recovery Procedures:**

#### **Service Recovery:**
```bash
# Restart specific service
sudo systemctl restart oht50.service

# Restart all services
sudo systemctl restart oht50.service oht50-backend.service oht50-frontend.service
```

#### **Configuration Recovery:**
```bash
# Restore from backup
sudo cp /opt/oht50/backups/latest/config/* /opt/oht50/config/

# Restart services
sudo systemctl restart oht50.service
```

#### **Complete Recovery:**
```bash
# Stop all services
sudo systemctl stop oht50.service oht50-backend.service oht50-frontend.service

# Restore from backup
sudo cp -r /opt/oht50/backups/latest/* /opt/oht50/

# Restart services
sudo systemctl start oht50.service oht50-backend.service oht50-frontend.service
```

---

## 📚 **CONFIGURATION REFERENCE**

### **System Configuration (`/opt/oht50/config/system.yaml`):**
```yaml
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

safety:
  estop_timeout: 100
  max_voltage: 18.0
  max_current: 13.0
  max_temperature: 10.0
```

### **Network Configuration:**
```yaml
# /etc/netplan/01-netcfg.yaml
network:
  version: 2
  ethernets:
    eth0:
      dhcp4: false
      addresses:
        - 192.168.1.100/24
      gateway4: 192.168.1.1
      nameservers:
        addresses: [8.8.8.8, 8.8.4.4]
```

---

## 📝 **CHANGELOG**

**v1.0.0 (2025-01-28):**
- ✅ Initial deployment guide
- ✅ Complete installation scripts
- ✅ Systemd service configuration
- ✅ Testing và validation procedures
- ✅ Troubleshooting guide
- ✅ Security considerations
- ✅ Configuration reference

---

## 🔗 **RELATED DOCUMENTATION**

- **Hardware Setup:** `docs/HARDWARE/ORANGE_PI_5B_SETUP.md`
- **Firmware Development:** `docs/FIRMWARE/README.md`
- **Backend API:** `docs/BACKEND/API_REFERENCE.md`
- **Frontend Development:** `docs/FRONTEND/README.md`
- **Safety System:** `docs/SAFETY/SYSTEM_SPECIFICATION.md`

---

**🚀 Ready for Production Deployment!**
