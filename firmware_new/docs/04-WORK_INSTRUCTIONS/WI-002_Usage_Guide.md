# 📖 Hướng Dẫn Sử Dụng OHT-50 Firmware

**Version:** 1.0.1  
**Last Updated:** 2025-10-07  
**Platform:** Orange Pi 5B (RK3588)

---

## 📋 **MỤC LỤC**

1. [Khởi Động Firmware](#khởi-động-firmware)
2. [Chế Độ Chạy](#chế-độ-chạy)
3. [Cấu Hình Hệ Thống](#cấu-hình-hệ-thống)
4. [Sử Dụng API](#sử-dụng-api)
5. [Quản Lý Module](#quản-lý-module)
6. [Giám Sát Hệ Thống](#giám-sát-hệ-thống)
7. [Emergency Stop](#emergency-stop)
8. [Xử Lý Lỗi](#xử-lý-lỗi)

---

## 🚀 **KHỞI ĐỘNG FIRMWARE**

### **1. Khởi động cơ bản**

```bash
# Chạy firmware
./oht50_main

# Output mẫu:
[OHT-50] 🧹 Starting auto cleanup system...
[CLEANUP] ✅ Process cleanup completed
[CLEANUP] ✅ Ports 8080 and 8081 are now free
[OHT-50] Starting main application...
[OHT-50] System starting in BOOT state...
[OHT-50] BOOT -> INIT transition
[OHT-50] INIT -> IDLE transition
[OHT-50] Boot sequence completed, system ready in < 20ms
[OHT-50] ✅ Communication manager started successfully (HTTP API)
[OHT-50] ✅ API Manager started on port 8080 (HTTP)
[OHT-50] Entering main loop. Press Ctrl+C to exit.
```

### **2. Khởi động với options**

```bash
# Dry-run mode (không chạm hardware)
./oht50_main --dry-run

# Debug mode (verbose logging)
./oht50_main --debug

# Cả hai
./oht50_main --dry-run --debug

# Help
./oht50_main --help
```

### **3. Khởi động như service**

```bash
# Start service
sudo systemctl start oht50.service

# Stop service
sudo systemctl stop oht50.service

# Restart service
sudo systemctl restart oht50.service

# Check status
sudo systemctl status oht50.service

# View logs
journalctl -u oht50.service -f
```

---

## 🎛️ **CHẾ ĐỘ CHẠY**

### **Máy Trạng Thái Hệ Thống**

Firmware hoạt động theo máy trạng thái với các trạng thái sau:

```
┌─────────┐     ┌──────┐     ┌──────┐     ┌───────┐     ┌─────────┐
│  BOOT   │────▶│ INIT │────▶│ IDLE │────▶│ READY │────▶│ RUNNING │
└─────────┘     └──────┘     └──────┘     └───────┘     └─────────┘
                                                                │
                                                                ▼
┌──────────┐    ┌───────┐                              ┌───────────┐
│ SHUTDOWN │◀───│ FAULT │◀─────────────────────────────│  E-STOP   │
└──────────┘    └───────┘                              └───────────┘
```

| **Trạng thái** | **Mô tả** | **LED System** | **Có thể điều khiển?** |
|----------------|-----------|----------------|------------------------|
| 🔵 **BOOT** | Khởi động hệ thống | Blink fast | ❌ Không |
| 🟡 **INIT** | Khởi tạo hardware | Blink slow | ❌ Không |
| 🟢 **IDLE** | Chờ lệnh | ON solid | ⚠️ Một phần |
| 🟢 **READY** | Sẵn sàng hoạt động | ON solid | ✅ Có |
| 🟢 **RUNNING** | Đang hoạt động | Blink slow | ✅ Có |
| 🔴 **FAULT** | Lỗi hệ thống | Blink fast | ❌ Không |
| 🔴 **E-STOP** | Emergency stop | OFF | ❌ Không |
| ⚫ **SHUTDOWN** | Đang tắt | OFF | ❌ Không |

### **Chuyển trạng thái thủ công**

```bash
# Thông qua API
curl -X POST http://localhost:8080/api/v1/robot/command \
     -H "Content-Type: application/json" \
     -d '{"command": "set_state", "state": "IDLE"}'
```

---

## ⚙️ **CẤU HÌNH HỆ THỐNG**

### **1. Cấu hình Module Registry**

File: `/etc/oht50/modules.yaml`

```yaml
# Module Registry Configuration
modules:
  - address: 0x02
    type: POWER
    name: "Power Module"
    enabled: true
    polling_interval_ms: 1000
    timeout_ms: 500

  - address: 0x03
    type: SAFETY
    name: "Safety Module"
    enabled: true
    polling_interval_ms: 500
    timeout_ms: 200

  - address: 0x04
    type: TRAVEL_MOTOR
    name: "Travel Motor Module"
    enabled: true
    polling_interval_ms: 100
    timeout_ms: 200

  - address: 0x05
    type: DOCK
    name: "Dock Module"
    enabled: true
    polling_interval_ms: 1000
    timeout_ms: 500
```

### **2. Cấu hình Safety**

File: `/etc/oht50/safety_config.yaml`

```yaml
# Safety Configuration
safety:
  estop_pin: 27
  response_time_ms: 100
  debounce_time_ms: 10
  safety_check_interval_ms: 50
  fault_clear_timeout_ms: 5000
  
  # Safety zones (LiDAR)
  zones:
    emergency_stop_mm: 500
    warning_mm: 1000
    safe_mm: 2000
```

### **3. Áp dụng cấu hình**

```bash
# Reload configuration
curl -X POST http://localhost:8080/api/v1/config/reload

# Hoặc restart service
sudo systemctl restart oht50.service
```

---

## 🌐 **SỬ DỤNG API**

### **1. Robot Status**

```bash
# Lấy trạng thái robot
curl http://localhost:8080/api/v1/robot/status

# Response:
{
  "success": true,
  "data": {
    "state": "IDLE",
    "system_ready": true,
    "safety_ok": true,
    "communication_ok": true,
    "modules_online": 4,
    "uptime_seconds": 3600
  }
}
```

### **2. Robot Control**

```bash
# Gửi lệnh di chuyển
curl -X POST http://localhost:8080/api/v1/robot/command \
     -H "Content-Type: application/json" \
     -d '{
       "command": "move",
       "parameters": {
         "velocity": 1000,
         "acceleration": 500,
         "position": 5000
       }
     }'

# Dừng robot
curl -X POST http://localhost:8080/api/v1/robot/command \
     -H "Content-Type: application/json" \
     -d '{"command": "stop"}'

# Emergency stop
curl -X POST http://localhost:8080/api/v1/safety/emergency
```

### **3. Module Management**

```bash
# Danh sách modules
curl http://localhost:8080/api/v1/modules

# Chi tiết module
curl http://localhost:8080/api/v1/modules/0x02

# Scan modules mới
curl -X POST http://localhost:8080/api/v1/modules/discover
```

### **4. Telemetry**

```bash
# Telemetry hiện tại
curl http://localhost:8080/api/v1/telemetry/current

# Response:
{
  "timestamp": 1696680000,
  "robot": {
    "position": 2500,
    "velocity": 1000,
    "battery_level": 87,
    "temperature": 42.5
  },
  "modules": {
    "0x02": {"voltage": 48.5, "current": 2.3},
    "0x03": {"safety_status": "NORMAL"},
    "0x04": {"motor_speed": 1500, "torque": 5.2},
    "0x05": {"dock_status": "READY"}
  }
}
```

---

## 📦 **QUẢN LÝ MODULE**

### **1. Auto-Discovery**

Firmware tự động tìm kiếm slave modules khi khởi động:

```
[OHT-50] Starting module discovery in background...
[OHT-50] Initial module discovery completed
[OHT-50] Adding discovered modules to polling manager...
[OHT-50] All discovered modules added to polling manager
```

Modules được phát hiện:
- **0x02**: Power Module
- **0x03**: Safety Module
- **0x04**: Travel Motor Module
- **0x05**: Dock Module

### **2. Module Polling**

Firmware tự động poll các modules theo interval:

| **Module** | **Interval** | **Purpose** |
|------------|--------------|-------------|
| Power (0x02) | 1000ms | Battery, voltage, current |
| Safety (0x03) | 500ms | Safety status, faults |
| Motor (0x04) | 100ms | Position, velocity, torque |
| Dock (0x05) | 1000ms | Dock status, alignment |

### **3. Module Health Check**

```bash
# Kiểm tra health của tất cả modules
curl http://localhost:8080/api/v1/modules/health

# Response:
{
  "modules": [
    {
      "address": "0x02",
      "status": "ONLINE",
      "last_seen_ms": 100,
      "response_time_ms": 15,
      "error_count": 0
    },
    {
      "address": "0x03",
      "status": "ONLINE",
      "last_seen_ms": 50,
      "response_time_ms": 12,
      "error_count": 0
    }
  ]
}
```

---

## 📊 **GIÁM SÁT HỆ THỐNG**

### **1. System Telemetry**

Firmware broadcast telemetry mỗi 1 giây:

```bash
# View real-time telemetry
curl http://localhost:8080/api/v1/telemetry/stream

# Hoặc xem logs
journalctl -u oht50.service -f | grep TELEMETRY
```

### **2. LED Status Indicators**

| **LED** | **Trạng thái** | **Ý nghĩa** |
|---------|---------------|-------------|
| 🔴 **Power** | ON | Hệ thống có nguồn |
| 🟢 **System** | Blink 500ms | Heartbeat (hệ thống hoạt động) |
| 🔵 **Comm** | ON | Tất cả 4 modules online |
| 🔵 **Comm** | Blink fast | Một số modules missing |
| 🔵 **Comm** | OFF | Không có modules online |
| 🟡 **Network** | ON | Network connected |
| 🔴 **Error** | ON | Có lỗi hệ thống |

### **3. Performance Monitoring**

Firmware có performance monitoring tích hợp:

```
[PERF] Loops: 1000, Avg: 8.5 μs, Max: 25000 μs, Slow: 2 (0.2%)
```

Thông tin:
- **Loops**: Số vòng lặp đã chạy
- **Avg**: Thời gian trung bình mỗi vòng lặp
- **Max**: Thời gian max
- **Slow**: Số vòng lặp chậm (>20ms)

---

## 🛑 **EMERGENCY STOP**

### **1. Hardware E-Stop**

Nhấn nút **E-Stop** trên thiết bị:
- ✅ Hệ thống chuyển sang trạng thái **E-STOP** ngay lập tức
- ✅ Tất cả motor dừng
- ✅ Relay tắt
- ✅ LED Error ON

### **2. Software E-Stop**

```bash
# Gửi E-Stop qua API
curl -X POST http://localhost:8080/api/v1/safety/emergency

# Response time: < 100ms
```

### **3. Recovery từ E-Stop**

```bash
# 1. Kiểm tra nguyên nhân
curl http://localhost:8080/api/v1/safety/status

# 2. Clear faults
curl -X POST http://localhost:8080/api/v1/safety/clear_faults

# 3. Reset E-Stop (nếu safe)
curl -X POST http://localhost:8080/api/v1/safety/reset_estop

# 4. Quay về IDLE
curl -X POST http://localhost:8080/api/v1/robot/command \
     -d '{"command": "set_state", "state": "IDLE"}'
```

---

## ⚠️ **XỬ LÝ LỖI**

### **Lỗi 1: Module Offline**

**Hiện tượng:**
- COMM LED blink fast
- Log: `WARNING: Only X/4 mandatory slave modules online`

**Xử lý:**
```bash
# 1. Kiểm tra modules
curl http://localhost:8080/api/v1/modules

# 2. Scan lại
curl -X POST http://localhost:8080/api/v1/modules/discover

# 3. Kiểm tra RS485 connection
# - Check cáp RS485
# - Check termination resistor
# - Check power supply
```

---

### **Lỗi 2: RS485 Communication Error**

**Hiện tượng:**
- Log: `[RS485] CRC error count: XX`
- Log: `[RS485] Timeout count: XX`

**Xử lý:**
```bash
# 1. Check RS485 stats
curl http://localhost:8080/api/v1/communication/stats

# 2. Test loopback
cd scripts/rs485
./rs485_loop_tester.py

# 3. Check hardware
# - Termination resistor 120Ω
# - Bias resistors
# - Cable quality
```

---

### **Lỗi 3: LiDAR Not Responding**

**Hiện tượng:**
- Log: `[OHT-50] hal_lidar_init failed`

**Xử lý:**
```bash
# 1. Check LiDAR device
ls -l /dev/ttyUSB0

# 2. Check permissions
sudo chmod 666 /dev/ttyUSB0

# 3. Test LiDAR
cd scripts/lidar
./lidar_test.sh
```

---

## 📚 **TÀI LIỆU THAM KHẢO**

- [API_REFERENCE.md](API_REFERENCE.md) - API documentation đầy đủ
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Khắc phục sự cố chi tiết
- [DEVELOPMENT.md](DEVELOPMENT.md) - Hướng dẫn phát triển
- [SECURITY.md](SECURITY.md) - Chính sách bảo mật

---

## 💡 **TIPS & TRICKS**

### **1. Performance Tuning**

```bash
# Giảm CPU usage bằng cách tăng sleep time
# Edit constants.h:
#define MIN_LOOP_INTERVAL_MS 10  # Tăng từ 5 lên 10

# Giảm telemetry frequency
# Thay đổi từ 1s → 2s trong main.c
```

### **2. Debug Mode**

```bash
# Chạy với debug để xem chi tiết
./oht50_main --debug

# Lọc debug logs
./oht50_main --debug 2>&1 | grep "DEBUG"

# Performance logs
./oht50_main --debug 2>&1 | grep "PERF"
```

### **3. Quick Commands**

```bash
# Alias hữu ích
alias oht-start='sudo systemctl start oht50'
alias oht-stop='sudo systemctl stop oht50'
alias oht-status='sudo systemctl status oht50'
alias oht-logs='journalctl -u oht50 -f'
alias oht-api='curl http://localhost:8080/api/v1'
```

---

## 🆘 **HỖ TRỢ**

Nếu cần hỗ trợ:

1. 📖 Đọc [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
2. 🐛 Tạo issue trên GitHub với logs
3. 📧 Email: support@oht50.com
4. 💬 Slack: #oht50-support

---

**Version:** 1.0.1  
**Last Updated:** 2025-10-07  
**Maintained By:** OHT-50 Firmware Team

