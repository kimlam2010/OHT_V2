# 🔧 Troubleshooting Guide - OHT-50 Firmware

**Version:** 1.0.1  
**Last Updated:** 2025-10-07

---

## 📋 **COMMON ISSUES**

### **1. Build Failed - Missing Dependencies**

**Triệu chứng:**
```
CMake Error: Could not find OpenSSL
```

**Nguyên nhân:** Thiếu OpenSSL development library

**Giải pháp:**
```bash
sudo apt install -y libssl-dev libcrypto++-dev
rm -rf build && mkdir build && cd build
cmake ..
make -j$(nproc)
```

---

### **2. /dev/ttyOHT485 Not Found**

**Triệu chứng:**
```
[OHT-50] hal_rs485_init failed: Device not found
```

**Nguyên nhân:** UART1 chưa được cấu hình hoặc udev rule chưa có

**Giải pháp:**
```bash
# Check UART1
ls -l /dev/ttyS1

# Tạo udev rule
sudo nano /etc/udev/rules.d/99-oht-rs485.rules
# Thêm: KERNEL=="ttyS1", SYMLINK+="ttyOHT485", MODE="0666"

# Reload
sudo udevadm control --reload-rules
sudo udevadm trigger

# Verify
ls -l /dev/ttyOHT485
```

---

### **3. Permission Denied**

**Triệu chứng:**
```
Error: Permission denied opening /dev/ttyOHT485
```

**Nguyên nhân:** User không có quyền truy cập UART

**Giải pháp:**
```bash
# Thêm user vào dialout group
sudo usermod -a -G dialout $USER

# Logout và login lại
exit
```

---

### **4. Port 8080 Already in Use**

**Triệu chứng:**
```
[OHT-50] API Manager init failed: Address already in use
```

**Nguyên nhân:** Process khác đang dùng port 8080

**Giải pháp:**
```bash
# Tìm process
sudo lsof -i :8080

# Kill process
sudo kill -9 <PID>

# Hoặc dùng auto cleanup
./oht50_main  # Firmware tự động cleanup
```

---

### **5. Module Not Discovered**

**Triệu chứng:**
```
WARNING: Only X/4 mandatory slave modules online
```

**Nguyên nhân:** Module offline, lỗi RS485, hoặc địa chỉ sai

**Giải pháp:**
```bash
# Check RS485 stats
curl http://localhost:8080/api/v1/communication/stats

# Scan lại
curl -X POST http://localhost:8080/api/v1/modules/discover

# Check hardware:
# - RS485 cable connection
# - Termination resistor 120Ω
# - Module power supply
# - Module address jumpers
```

---

### **6. High CPU Usage**

**Triệu chứng:** CPU usage > 90%

**Nguyên nhân:** Loop quá nhanh hoặc blocking operations

**Giải pháp:**
```bash
# Check performance stats
journalctl -u oht50 -f | grep PERF

# Tăng sleep time
# Edit constants.h:
#define MIN_LOOP_INTERVAL_MS 10  # Từ 5 → 10

# Rebuild
```

---

### **7. RS485 Communication Errors**

**Triệu chứng:**
```
[RS485] CRC error count: 50
[RS485] Timeout count: 20
```

**Nguyên nhân:** Nhiễu, termination sai, hoặc baudrate không khớp

**Giải pháp:**
```bash
# Test loopback
cd scripts/rs485
./rs485_loop_tester.py

# Check hardware:
# - Termination resistor 120Ω ở 2 đầu bus
# - Bias resistors (pull-up/down)
# - Cable quality (twisted pair, shielded)
# - Cable length < 1200m
# - Baudrate match (115200)
```

---

### **8. LiDAR Not Responding**

**Triệu chứng:**
```
[OHT-50] hal_lidar_init failed: Device not found
```

**Nguyên nhân:** LiDAR không kết nối hoặc permission denied

**Giải pháp:**
```bash
# Check device
ls -l /dev/ttyUSB0

# Fix permissions
sudo chmod 666 /dev/ttyUSB0

# Add udev rule (permanent)
sudo nano /etc/udev/rules.d/99-oht-lidar.rules
# Thêm: KERNEL=="ttyUSB*", MODE="0666"

# Test
cd scripts/lidar
./lidar_test.sh
```

---

### **9. State Machine Stuck**

**Triệu chứng:** Robot stuck ở một state

**Nguyên nhân:** Deadlock hoặc missing event

**Giải pháp:**
```bash
# Check current state
curl http://localhost:8080/api/v1/robot/status

# Force state transition
curl -X POST http://localhost:8080/api/v1/robot/command \
     -d '{"command": "set_state", "state": "IDLE"}'

# Restart service
sudo systemctl restart oht50
```

---

### **10. Memory Leak**

**Triệu chứng:** Memory usage tăng dần theo thời gian

**Nguyên nhân:** Memory leak trong code

**Giải pháp:**
```bash
# Check với valgrind
valgrind --leak-check=full \
         --show-leak-kinds=all \
         ./oht50_main --dry-run

# Analyze report
# Fix leaks và rebuild
```

---

## 📊 **Diagnostic Tools**

### **System Health Check**

```bash
# CPU, Memory, Disk
top
htop
free -h
df -h

# Network
ip addr
ping 8.8.8.8
netstat -tulpn

# Processes
ps aux | grep oht50
pgrep -f oht50_main

# Logs
journalctl -u oht50 -f
dmesg | tail -50
```

---

## 🆘 **Getting Help**

Nếu vấn đề chưa được giải quyết:

1. 📖 Check [USAGE.md](USAGE.md) và [INSTALLATION.md](INSTALLATION.md)
2. 🔍 Search GitHub issues: https://github.com/your-org/OHT-50-firmware/issues
3. 🐛 Create issue với:
   - Triệu chứng rõ ràng
   - Steps to reproduce
   - Logs đầy đủ
   - Environment info
4. 📧 Email support: support@oht50.com
5. 💬 Slack: #oht50-support

---

**Version:** 1.0.1  
**Maintained By:** OHT-50 Support Team

