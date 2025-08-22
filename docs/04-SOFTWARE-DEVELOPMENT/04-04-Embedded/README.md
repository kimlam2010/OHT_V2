---
title: "Readme"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "EMBED Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['software-development']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# EMBED - OHT-50 Hardware & Low-Level Drivers

## 📋 **Tổng quan**
Folder EMBED chứa tất cả file liên quan đến phần cứng và driver cấp thấp cho OHT-50.

**Vị trí:** `/home/orangepi/OHT-50/EMBED/`  
**Trách nhiệm:** Hardware bring-up, UART1 RS485, GPIO control, HAL interface, Modbus RTU

---

## 📁 **File Structure**

### **Device Tree Overlays**
- `uart1_46_47.dts` - Device tree overlay source cho UART1 chân 46, 47
- `uart1_46_47.dtbo` - Compiled device tree overlay

### **Test Scripts**
- `test_uart1_pins_46_47.py` - Test UART1 cơ bản với chân 46, 47
- `send_uart1.py` - Gửi UART1 liên tục
- `monitor_uart1.py` - Monitor UART1 để xem dữ liệu nhận được
- `test_with_module.py` - Test với module OHT-50
- `continuous_tx_test.py` - Test gửi liên tục

### **Modbus RTU Tests**
- `test_modbus_simple.py` - Test Modbus RTU đơn giản
- `test_modbus_rtu.py` - Test Modbus RTU chi tiết
- `test_modbus_loopback.py` - Test Modbus RTU với loopback

### **HAL & Setup**
- `hal_rs485.py` - Hardware Abstraction Layer cho RS485
- `setup_udev_rs485.sh` - Setup udev rules cho RS485

---

## 🔧 **Cấu hình UART1**

### **UART1 Configuration**
- **Device**: `/dev/ttyS1`
- **Baudrate**: 115200
- **TX**: Chân 46 (GPIO1_D1) - ALT10 mode
- **RX**: Chân 47 (GPIO1_D0) - ALT10 mode

### **Quick Setup**
```bash
# Compile device tree overlay
sudo dtc -@ -I dts -O dtb -o uart1_46_47.dtbo uart1_46_47.dts

# Copy vào /boot
sudo cp uart1_46_47.dtbo /boot/overlays/

# Thêm vào /boot/orangepiEnv.txt
sudo sed -i 's/overlays=/overlays=uart1_46_47 /' /boot/orangepiEnv.txt

# Reboot
sudo reboot
```

---

## 🧪 **Test Commands**

### **1. Test UART1 cơ bản**
```bash
cd /home/orangepi/OHT-50/EMBED
sudo python3 test_uart1_pins_46_47.py
```

### **2. Gửi UART1 liên tục**
```bash
sudo python3 send_uart1.py
```

### **3. Monitor UART1**
```bash
sudo python3 monitor_uart1.py
```

### **4. Test với module**
```bash
sudo python3 test_with_module.py
```

### **5. Test Modbus RTU đơn giản**
```bash
sudo python3 test_modbus_simple.py
```

### **6. Test Modbus RTU chi tiết**
```bash
sudo python3 test_modbus_rtu.py
```

### **7. Test Modbus RTU loopback**
```bash
sudo python3 test_modbus_loopback.py
```

### **8. Setup udev rules**
```bash
sudo ./setup_udev_rs485.sh
```

---

## 📊 **Modbus RTU Test Results**

### **Test Results**
```
✅ UART1 mở thành công: /dev/ttyS1
✅ Modbus RTU frames gửi thành công
✅ Response từ slave devices detected
✅ CRC validation hoạt động
```

### **Modbus Functions Tested**
- **0x03**: Read Holding Registers
- **0x04**: Read Input Registers  
- **0x06**: Write Single Register

### **Slave Addresses Tested**
- **Slave 1**: Response detected
- **Slave 2**: Response detected
- **Slave 3**: No response (expected)

---

## 📊 **Status**

### **✅ Hoàn thành**
- UART1 mapping chân 46 (TX), 47 (RX)
- Device tree overlay hoạt động
- HAL RS485 interface
- Test scripts đầy đủ
- Udev rules setup
- **Modbus RTU**: Protocol test thành công
- **Organization**: Tất cả file đã được tổ chức gọn gàng

### **🎯 Sẵn sàng cho**
- RS485 communication với module OHT-50
- Modbus RTU protocol implementation
- Firmware development
- Hardware integration

---

## 📞 **Contact**

**EMBED Team:**
- **Vai trò**: Hardware bring-up, low-level drivers
- **Focus**: UART1 RS485, GPIO control, HAL interface, Modbus RTU
- **Status**: Complete và sẵn sàng sử dụng

---

**Changelog:**
- v2.1 (2025-01-27): Added Modbus RTU tests, complete protocol validation
- v2.0 (2025-01-27): Moved to EMBED folder, complete UART1 chân 46, 47
- v1.0 (2025-08-16): Initial RS485 integration
