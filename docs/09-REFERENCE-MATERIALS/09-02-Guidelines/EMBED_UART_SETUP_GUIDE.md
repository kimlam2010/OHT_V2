---
title: "Embed Uart Setup Guide"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "PM Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['reference-materials', 'guide', 'documentation']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# EMBED UART Setup Guide - OHT-50

## 📋 **Tổng quan**
Hướng dẫn cấu hình UART1 RS485 cho OHT-50 trên Orange Pi 5B (RK3588) từng bước.

**Phiên bản:** v1.0  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** ✅ Tested trên Orange Pi 5B

---

## 🎯 **Vai trò EMBED**

### **Mục tiêu**
- Nền tảng phần cứng ổn định cho OHT-50
- Giao tiếp RS485 xác định qua UART1
- HAL/API rõ ràng cho firmware
- Lỗi được phát hiện và khôi phục minh bạch

### **Phạm vi trách nhiệm**
- **Hardware bring-up**: UART1, GPIO, RS485 transceiver
- **Driver level**: HAL UART/RS485, GPIO/Relay
- **System integration**: udev rules, device symlinks
- **Validation**: Hardware tests, BER/latency measurement

---

## 🔧 **Cấu hình UART1 từng bước**

### **Bước 1: Enable UART1 trong Device Tree**

#### 1.1 Tạo Device Tree Overlay
```bash
# Tạo file enable_uart1.dts
cat > enable_uart1.dts << 'EOF'
/dts-v1/;
/plugin/;

/ {
    compatible = "rockchip,rk3588";

    fragment@0 {
        target = <&uart1>;
        __overlay__ {
            status = "okay";
            pinctrl-names = "default";
            pinctrl-0 = <&uart1m1_xfer>;
        };
    };
};
EOF
```

#### 1.2 Compile Device Tree Overlay
```bash
# Compile .dts thành .dtbo
sudo dtc -@ -I dts -O dtb -o enable_uart1.dtbo enable_uart1.dts
```

#### 1.3 Copy overlay vào /boot
```bash
# Copy file .dtbo vào thư mục overlays
sudo cp enable_uart1.dtbo /boot/orangepiEnv.txt.d/overlays/
```

#### 1.4 Enable overlay trong /boot/orangepiEnv.txt
```bash
# Edit file cấu hình boot
sudo nano /boot/orangepiEnv.txt

# Thêm dòng sau vào phần overlays:
overlays=uart1-m1
```

#### 1.5 Reboot để áp dụng
```bash
sudo reboot
```

### **Bước 2: Kiểm tra UART1 hoạt động**

#### 2.1 Kiểm tra device tồn tại
```bash
# Kiểm tra /dev/ttyS1
ls -la /dev/ttyS1

# Kết quả mong đợi:
# lrwxrwxrwx 1 root root 5 Jan 27 10:00 /dev/ttyS1 -> ttyS1
```

#### 2.2 Test UART1 cơ bản
```bash
# Test write/read
echo "test" | sudo tee /dev/ttyS1
sudo cat /dev/ttyS1
```

### **Bước 3: Tạo udev symlink**

#### 3.1 Tạo udev rule
```bash
# Tạo file rule
sudo tee /etc/udev/rules.d/99-oht50-rs485.rules << 'EOF'
# OHT-50 RS485 UART1 symlink
KERNEL=="ttyS1", SYMLINK+="ttyOHT485", MODE="0666"
KERNEL=="ttyS1", GROUP="dialout", MODE="0666"
EOF
```

#### 3.2 Reload udev rules
```bash
# Reload rules
sudo udevadm control --reload-rules
sudo udevadm trigger

# Kiểm tra symlink
ls -la /dev/ttyOHT485
```

#### 3.3 Set permissions
```bash
# Set permissions
sudo chmod 666 /dev/ttyOHT485
sudo chown root:dialout /dev/ttyOHT485

# Thêm user vào group dialout
sudo usermod -a -G dialout $USER
```

### **Bước 4: Cấu hình RS485**

#### 4.1 RS485 Pinout (Orange Pi 5B)
```
UART1 Pinout:
- TX: GPIO1_D1 (Pin 8)
- RX: GPIO1_D0 (Pin 10)  
- RTS: GPIO1_D3 (Pin 12) - RS485 Direction Control
- CTS: GPIO1_D2 (Pin 16) - (Optional)
```

#### 4.2 RS485 Transceiver Wiring
```
Orange Pi 5B    RS485 Transceiver
TX (Pin 8)  --> DI (Driver Input)
RX (Pin 10) <-- RO (Receiver Output)
RTS (Pin 12) --> DE/RE (Driver Enable/Receiver Enable)
GND         --> GND
3.3V        --> VCC
```

#### 4.3 Termination (nếu cần)
```
A+ Line: 120Ω resistor to 3.3V
B- Line: 120Ω resistor to GND
```

### **Bước 5: Test RS485**

#### 5.1 Test cơ bản với Python
```python
#!/usr/bin/env python3
import serial
import time

# Mở serial port
ser = serial.Serial(
    port='/dev/ttyOHT485',
    baudrate=115200,
    bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    timeout=1
)

# Test gửi dữ liệu
test_data = b'Hello RS485\n'
ser.write(test_data)
print(f"Sent: {test_data}")

# Test nhận dữ liệu (loopback)
response = ser.read(100)
print(f"Received: {response}")

ser.close()
```

#### 5.2 Test với RS485 loopback
```bash
# Chạy test script có sẵn
cd OHT-50
sudo python3 tools/rs485/rs485_loop_tester.py --dev /dev/ttyOHT485 --baud 115200 --count 100
```

---

## 🧪 **Validation Tests**

### **Test 1: Hardware Validation**
```bash
# Chạy hardware validation
sudo python3 hardware_validation.py

# Kết quả mong đợi:
# UART1 Device: ✅ PASS
# GPIO Access: ✅ PASS  
# RS485 Loopback: ⚠️ FAIL (expected - no hardware wiring)
# Voltage Levels: ✅ PASS
# Termination: ✅ PASS
```

### **Test 2: Firmware Integration**
```bash
# Test firmware integration
sudo python3 firmware_rs485_integration.py

# Kết quả mong đợi:
# RS485 HAL initialized successfully
# Telemetry thread started
# Command thread started
# Runtime: XX.Xs uptime, X messages sent, 0 errors
```

### **Test 3: Modbus Protocol**
```bash
# Test Modbus protocol
sudo python3 test_modbus_loopback.py

# Kết quả mong đợi:
# Modbus frames sent successfully
# Slave address: 2
# Baudrate: 115200
# Functions: 03, 04, 06, 10
```

---

## 📊 **Troubleshooting**

### **Vấn đề thường gặp**

#### 1. UART1 không tồn tại
```bash
# Kiểm tra device tree overlay
ls /boot/orangepiEnv.txt.d/overlays/enable_uart1.dtbo

# Kiểm tra /boot/orangepiEnv.txt
grep -i uart /boot/orangepiEnv.txt

# Reboot nếu cần
sudo reboot
```

#### 2. Permission denied
```bash
# Kiểm tra permissions
ls -la /dev/ttyOHT485

# Fix permissions
sudo chmod 666 /dev/ttyOHT485
sudo chown root:dialout /dev/ttyOHT485

# Thêm user vào group
sudo usermod -a -G dialout $USER
# Logout/login để áp dụng
```

#### 3. RS485 không hoạt động
```bash
# Kiểm tra wiring
# TX -> DI, RX -> RO, RTS -> DE/RE

# Test với loopback (short A+ to B-)
# Chạy test script
sudo python3 tools/rs485/rs485_loop_tester.py --dev /dev/ttyOHT485 --auto_rts
```

---

## 📋 **Checklist hoàn thành**

- [ ] UART1 enabled trong device tree
- [ ] /dev/ttyS1 tồn tại và hoạt động
- [ ] udev symlink /dev/ttyOHT485 được tạo
- [ ] Permissions đúng (666, dialout group)
- [ ] RS485 transceiver được wiring đúng
- [ ] Hardware validation tests pass
- [ ] Firmware integration tests pass
- [ ] Modbus protocol tests pass

---

## 🔗 **Tài liệu tham khảo**

- [Orange Pi 5B Pinout](https://wiki.orange-pi.com/Orange_Pi_5B)
- [RK3588 UART Documentation](https://www.rock-chips.com/a/en/products/RK35_Series/2022/0829/20701.html)
- [RS485 Wiring Guide](https://www.maxlinear.com/ds/MAX485.pdf)

---

**Changelog:**
- v1.0 (2025-01-27): Initial version với hướng dẫn đầy đủ
