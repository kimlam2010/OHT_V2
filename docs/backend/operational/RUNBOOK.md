# OPERATIONAL RUNBOOK - OHT-50 Backend

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-28  
**Team:** Backend Team  
**Trạng thái:** 🔧 CẦN IMPLEMENT

---

## 🚨 **EMERGENCY PROCEDURES**

### **1. System Down - Emergency Response**
```bash
# 1. Immediate Actions
sudo systemctl stop oht50-backend
sudo systemctl stop oht50-firmware

# 2. Check System Status
sudo systemctl status oht50-backend
sudo systemctl status oht50-firmware

# 3. Check Logs
sudo journalctl -u oht50-backend -n 50
sudo journalctl -u oht50-firmware -n 50

# 4. Restart Services
sudo systemctl start oht50-firmware
sudo systemctl start oht50-backend

# 5. Verify Recovery
curl http://localhost:8000/health/
curl http://localhost:8081/health/
```

### **2. E-Stop Triggered - Safety Response**
```bash
# 1. Verify E-Stop Status
curl http://localhost:8000/api/v1/safety/status

# 2. Check Hardware E-Stop
sudo cat /sys/class/gpio/gpio59/value

# 3. Reset E-Stop (if safe)
curl -X POST http://localhost:8000/api/v1/safety/reset

# 4. Verify System Recovery
curl http://localhost:8000/api/v1/system/status
```

### **3. Communication Failure - Network Issues**
```bash
# 1. Check Network Connectivity
ping 8.8.8.8
ping center-server

# 2. Check Network Interfaces
ip addr show
ip route show

# 3. Restart Network Services
sudo systemctl restart networking
sudo systemctl restart wpa_supplicant

# 4. Check RS485 Communication
sudo python3 tools/rs485_test.py
```

---

## 🔧 **ROUTINE MAINTENANCE**

### **1. Daily Health Checks**
```bash
# 1. System Health
curl http://localhost:8000/health/detailed

# 2. Service Status
sudo systemctl status oht50-backend
sudo systemctl status oht50-firmware

# 3. Disk Space
df -h
du -sh /opt/oht50/logs/

# 4. Memory Usage
free -h
top -n 1

# 5. Network Status
netstat -tlnp | grep :8000
netstat -tlnp | grep :8081
```

### **2. Weekly Maintenance**
```bash
# 1. Log Rotation
sudo logrotate -f /etc/logrotate.d/oht50

# 2. Database Backup (if applicable)
sudo tar -czf /backup/oht50-$(date +%Y%m%d).tar.gz /opt/oht50/data/

# 3. Configuration Backup
sudo cp /opt/oht50/config/* /backup/config/

# 4. System Updates
sudo apt update
sudo apt upgrade -y

# 5. Service Restart
sudo systemctl restart oht50-backend
sudo systemctl restart oht50-firmware
```

### **3. Monthly Maintenance**
```bash
# 1. Full System Backup
sudo tar -czf /backup/full-backup-$(date +%Y%m%d).tar.gz /opt/oht50/

# 2. Log Analysis
sudo journalctl -u oht50-backend --since "1 month ago" > /tmp/backend-logs.txt
sudo journalctl -u oht50-firmware --since "1 month ago" > /tmp/firmware-logs.txt

# 3. Performance Analysis
curl http://localhost:8000/api/v1/monitoring/metrics

# 4. Security Audit
sudo apt audit
sudo systemctl status --failed
```

---

## 🐛 **TROUBLESHOOTING GUIDE**

### **1. Service Won't Start**
```bash
# Check Dependencies
sudo systemctl status postgresql
sudo systemctl status redis

# Check Configuration
sudo cat /opt/oht50/config/system.yaml
sudo cat /opt/oht50/.env

# Check Permissions
ls -la /opt/oht50/
sudo chown -R oht:oht /opt/oht50/

# Check Port Conflicts
sudo netstat -tlnp | grep :8000
sudo lsof -i :8000

# Check Logs
sudo journalctl -u oht50-backend -f
```

### **2. API Endpoints Not Responding**
```bash
# Check Service Status
curl http://localhost:8000/health/

# Check Authentication
curl -H "Authorization: Bearer <token>" http://localhost:8000/api/v1/config/

# Check CORS
curl -H "Origin: http://localhost:3000" http://localhost:8000/api/v1/config/

# Check Rate Limiting
for i in {1..10}; do curl http://localhost:8000/api/v1/config/; done

# Check Network
curl -v http://localhost:8000/health/
```

### **3. Database Connection Issues**
```bash
# Check Database Status
sudo systemctl status postgresql

# Check Connection
psql -h localhost -U oht50 -d oht50_db

# Check Logs
sudo tail -f /var/log/postgresql/postgresql-*.log

# Check Configuration
sudo cat /etc/postgresql/*/main/postgresql.conf | grep listen_addresses
sudo cat /etc/postgresql/*/main/pg_hba.conf
```

### **4. Memory/CPU Issues**
```bash
# Check Resource Usage
htop
iotop
nethogs

# Check Process Details
ps aux | grep oht50
sudo cat /proc/$(pgrep oht50-backend)/status

# Check Memory Leaks
sudo cat /proc/$(pgrep oht50-backend)/maps
sudo cat /proc/$(pgrep oht50-backend)/smaps

# Restart with Monitoring
sudo systemctl restart oht50-backend
watch -n 1 'ps aux | grep oht50'
```

---

## 📊 **MONITORING & ALERTS**

### **1. Key Metrics to Monitor**
```bash
# System Metrics
curl http://localhost:8000/api/v1/monitoring/metrics

# Health Status
curl http://localhost:8000/health/detailed

# Performance Metrics
curl http://localhost:8000/api/v1/monitoring/performance

# Error Rates
curl http://localhost:8000/api/v1/monitoring/errors
```

### **2. Alert Thresholds**
```yaml
# CPU Usage
cpu_usage_warning: 80%
cpu_usage_critical: 95%

# Memory Usage
memory_usage_warning: 85%
memory_usage_critical: 95%

# Disk Usage
disk_usage_warning: 80%
disk_usage_critical: 90%

# Response Time
response_time_warning: 500ms
response_time_critical: 1000ms

# Error Rate
error_rate_warning: 5%
error_rate_critical: 10%
```

### **3. Alert Actions**
```bash
# High CPU Usage
sudo systemctl restart oht50-backend
sudo systemctl restart oht50-firmware

# High Memory Usage
sudo systemctl restart oht50-backend
sudo sync && sudo echo 3 > /proc/sys/vm/drop_caches

# High Disk Usage
sudo find /opt/oht50/logs/ -name "*.log" -mtime +7 -delete
sudo logrotate -f /etc/logrotate.d/oht50

# High Error Rate
sudo journalctl -u oht50-backend --since "1 hour ago" | grep ERROR
sudo systemctl restart oht50-backend
```

---

## 🔒 **SECURITY INCIDENTS**

### **1. Unauthorized Access**
```bash
# Check Access Logs
sudo tail -f /var/log/auth.log
sudo journalctl -u oht50-backend | grep "unauthorized"

# Check Active Connections
sudo netstat -tlnp | grep :8000
sudo ss -tlnp | grep :8000

# Block Suspicious IPs
sudo iptables -A INPUT -s <suspicious_ip> -j DROP

# Restart with Enhanced Security
sudo systemctl restart oht50-backend
```

### **2. Data Breach**
```bash
# Stop Services
sudo systemctl stop oht50-backend
sudo systemctl stop oht50-firmware

# Backup Current State
sudo tar -czf /backup/incident-$(date +%Y%m%d-%H%M%S).tar.gz /opt/oht50/

# Check Logs for Breach
sudo journalctl -u oht50-backend --since "24 hours ago" | grep -i "breach\|hack\|unauthorized"

# Rotate Credentials
sudo cp /opt/oht50/.env /opt/oht50/.env.backup
# Update SECRET_KEY and other credentials

# Restart with New Credentials
sudo systemctl start oht50-backend
```

---

## 🔄 **RECOVERY PROCEDURES**

### **1. Service Recovery**
```bash
# Automatic Recovery
sudo systemctl enable oht50-backend
sudo systemctl enable oht50-firmware

# Manual Recovery
sudo systemctl restart oht50-backend
sudo systemctl restart oht50-firmware

# Verify Recovery
curl http://localhost:8000/health/
curl http://localhost:8081/health/
```

### **2. Data Recovery**
```bash
# Restore from Backup
sudo tar -xzf /backup/oht50-$(date +%Y%m%d).tar.gz -C /

# Restore Configuration
sudo cp /backup/config/* /opt/oht50/config/

# Verify Data Integrity
curl http://localhost:8000/api/v1/config/
curl http://localhost:8000/api/v1/telemetry/current
```

### **3. System Recovery**
```bash
# Full System Restore
sudo tar -xzf /backup/full-backup-$(date +%Y%m%d).tar.gz -C /

# Reinstall Dependencies
sudo apt update
sudo apt install -y python3.11 python3-pip postgresql redis-server

# Restore Services
sudo systemctl enable oht50-backend
sudo systemctl enable oht50-firmware
sudo systemctl start oht50-backend
sudo systemctl start oht50-firmware
```

---

## 📞 **ESCALATION MATRIX**

### **Level 1 - On-Call Engineer**
- **Response Time:** 15 minutes
- **Actions:** Initial diagnosis, basic troubleshooting
- **Contact:** oncall@company.com

### **Level 2 - Senior Engineer**
- **Response Time:** 30 minutes
- **Actions:** Advanced troubleshooting, configuration changes
- **Contact:** senior@company.com

### **Level 3 - System Architect**
- **Response Time:** 1 hour
- **Actions:** Architecture changes, major incidents
- **Contact:** architect@company.com

### **Level 4 - CTO**
- **Response Time:** 2 hours
- **Actions:** Critical business impact, security incidents
- **Contact:** cto@company.com

---

## 📋 **CHECKLISTS**

### **Pre-Deployment Checklist**
- [ ] All tests passing
- [ ] Security scan completed
- [ ] Performance tests passed
- [ ] Documentation updated
- [ ] Backup completed
- [ ] Rollback plan ready

### **Post-Deployment Checklist**
- [ ] Services started successfully
- [ ] Health checks passing
- [ ] API endpoints responding
- [ ] Monitoring alerts configured
- [ ] Logs being generated
- [ ] Performance metrics normal

### **Incident Response Checklist**
- [ ] Incident identified and logged
- [ ] Initial assessment completed
- [ ] Stakeholders notified
- [ ] Containment measures implemented
- [ ] Root cause analysis started
- [ ] Recovery plan executed
- [ ] Post-incident review scheduled

---

**🚨 Lưu ý:** Runbook này cần được cập nhật thường xuyên dựa trên thực tế vận hành và các incident đã xảy ra.
