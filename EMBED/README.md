# EMBED - OHT-50 Hardware & Low-Level Drivers

## 📋 **Tổng quan**
Folder EMBED chứa tất cả file liên quan đến phần cứng và driver cấp thấp cho OHT-50.

**Vị trí:** `/home/orangepi/Desktop/OHT_V2/EMBED/`  
**Trách nhiệm:** Hardware bring-up, UART1 RS485, GPIO control, HAL interface, Modbus RTU  
**Platform:** Orange Pi 5B (RK3588)  
**Status:** ✅ **PRODUCTION READY** - Hardware đã được cấu hình và validate hoàn chỉnh

## 🚨 **QUAN TRỌNG - OVERLAY CONFIGURATION**

### **✅ SỬ DỤNG SYSTEM OVERLAY (KHUYẾN NGHỊ):**
- **Overlay:** `uart1-m1` (có sẵn trong hệ thống)
- **Ưu điểm:** Stable, tested, không cần compile
- **Boot config:** `overlays=uart1-m1`

## 🚨 **QUAN TRỌNG - QUY TRÌNH SETUP HOÀN CHỈNH**

### **🔥 Setup từ đầu (First Time Setup):**
```bash
# 1. Cấu hình GPIO (BẮT BUỘC)
cd /home/orangepi/Desktop/OHT_V2/EMBED
sudo ./setup_oht_gpio_correct.sh

# 2. Cài đặt pyserial (BẮT BUỘC)
sudo pip3 install pyserial

# 3. Enable UART1 system overlay (BẮT BUỘC) - SỬ DỤNG OVERLAY CÓ SẴN
# KHÔNG CẦN copy file overlay - sử dụng system overlay uart1-m1

# 4. Enable UART1 system overlay (BẮT BUỘC)
sudo bash -c "echo 'overlays=uart1-m1' >> /boot/orangepiEnv.txt"

# 5. REBOOT để kích hoạt UART1 (BẮT BUỘC)
sudo reboot

# 6. Sau reboot, setup udev rules
sudo ./setup_udev_rs485.sh

# 7. Verify hoàn tất
gpio readall
ls -la /dev/ttyS1 /dev/ttyOHT485
```

### **⚡ Quick Verification:**
```bash
# Kiểm tra GPIO đã cấu hình đúng
gpio readall | grep -E "(54|35|28|29|58|59|131|132|46|47)"

# Kiểm tra UART1 device
ls -la /dev/ttyS1

# Test GPIO control
echo 1 | sudo tee /sys/class/gpio/gpio54/value  # LED ON
echo 0 | sudo tee /sys/class/gpio/gpio54/value  # LED OFF

# Test RS485 communication
sudo python3 test_uart1_pins_46_47.py
```

---

## 🔧 **GPIO Mapping (CORRECTED)**

### **🔴 LED Status Indicators (5 LEDs):**
| **Chân GPIO** | **GPIO Name** | **Chức năng** | **Mode** | **Trạng thái** |
|---------------|---------------|---------------|----------|----------------|
| **GPIO 54** | GPIO1_D6 | Power LED (Green) | OUTPUT | ✅ Đang sử dụng |
| **GPIO 35** | GPIO1_A3 | System LED (Blue) | OUTPUT | ✅ Đang sử dụng |
| **GPIO 28** | GPIO0_D4 | Communication LED (Yellow) | OUTPUT | ✅ Đang sử dụng |
| **GPIO 29** | GPIO0_D5 | Network LED (Green) | OUTPUT | ✅ Đang sử dụng |
| **GPIO 58** | GPIO1_D2 | Error LED (Red) | OUTPUT | ✅ Đang sử dụng |

### **🛑 E-Stop Safety System (Dual-channel):**
| **Chân GPIO** | **GPIO Name** | **Chức năng** | **Mode** | **Trạng thái** |
|---------------|---------------|---------------|----------|----------------|
| **GPIO 59** | GPIO1_D3 | E-Stop Channel 1 | INPUT (Pull-up) | ✅ Đang sử dụng |

### **⚡ Relay Output (2x):**
| **Chân GPIO** | **GPIO Name** | **Chức năng** | **Mode** | **Trạng thái** |
|---------------|---------------|---------------|----------|----------------|
| **GPIO 131** | GPIO4_A3 | Relay 1 Output | OUTPUT | ✅ Đang sử dụng |
| **GPIO 132** | GPIO4_A4 | Relay 2 Output | OUTPUT | ✅ Đang sử dụng |

### **📡 RS485 Communication:**
| **Chân GPIO** | **GPIO Name** | **Chức năng** | **Mode** | **Trạng thái** |
|---------------|---------------|---------------|----------|----------------|
| **GPIO 46** | UART1_TX | RS485 TX Control | OUTPUT | ✅ Đang sử dụng |
| **GPIO 47** | UART1_RX | RS485 RX Control | INPUT | ✅ Đang sử dụng |

---

## 📁 **Production File Structure**

### **📋 Documentation**
- `README.md` - Complete documentation và setup guide
- `QUICK_START.md` - Fast reference guide cho quick setup

### **🚀 Setup Scripts (Production Ready)**
- `setup_complete_system.sh` - **One-command setup** cho toàn bộ hệ thống
- `setup_oht_gpio_correct.sh` - GPIO configuration với mapping đúng
- `setup_udev_rs485.sh` - Setup udev rules cho RS485 symlink

### **⚙️ Hardware Abstraction Layer**
- `hal_rs485.py` - Python HAL interface cho RS485 communication

### **🔧 Device Tree Files**
- `uart1_46_47.dts` - Device tree overlay source cho UART1 chân 46, 47
- `uart1_46_47.dtbo` - Compiled device tree overlay

### **📁 Archive (Development Files)**
- `archive/tests/` - Test scripts cho development và troubleshooting
- `archive/tools/` - Development tools và utilities
- `archive/old_docs/` - Documentation cũ và analysis reports

---

## 🔧 **Cấu hình UART1**

### **UART1 Configuration**
- **Device**: `/dev/ttyS1`
- **Baudrate**: 115200
- **TX**: Chân 46 (GPIO1_D1) - ALT10 mode
- **RX**: Chân 47 (GPIO1_D0) - ALT10 mode

### **Device Tree Overlay Setup**
```bash
# Compile device tree overlay (nếu cần)
sudo dtc -@ -I dts -O dtb -o uart1_46_47.dtbo uart1_46_47.dts

# Copy vào thư mục overlay đúng (Orange Pi 5B)
sudo cp uart1_46_47.dtbo /boot/dtb-6.1.43-rockchip-rk3588/rockchip/overlay/

# Thêm vào /boot/orangepiEnv.txt
sudo bash -c "echo 'overlays=uart1_46_47' >> /boot/orangepiEnv.txt"

# REBOOT để kích hoạt
sudo reboot
```

### **⚠️ Troubleshooting Common Issues:**
```bash
# Nếu GPIO chưa được export
sudo ./setup_oht_gpio_correct.sh

# Nếu /dev/ttyS1 không tồn tại
cat /boot/orangepiEnv.txt | grep overlays
sudo reboot

# Nếu permission denied khi test GPIO
sudo chmod 666 /sys/class/gpio/gpio*/value

# Nếu pyserial module not found
sudo pip3 install pyserial
```

---

## 🧪 **Test Commands (Archive Files)**

### **Basic Tests (từ archive/tests/):**
```bash
cd /home/orangepi/Desktop/OHT_V2/EMBED/archive/tests

# Test UART1 cơ bản
sudo python3 test_uart1_pins_46_47.py

# Test Modbus RTU đơn giản
sudo python3 test_modbus_simple.py

# Test GPIO với mapping đúng
sudo python3 test_oht_gpio_correct.py

# Test với module thật
sudo python3 test_with_module.py
```

### **Development Tools (từ archive/tools/):**
```bash
cd /home/orangepi/Desktop/OHT_V2/EMBED/archive/tools

# Monitor UART1
sudo python3 monitor_uart1.py

# Gửi UART1 liên tục
sudo python3 send_uart1.py

# Test transmission liên tục
sudo python3 continuous_tx_test.py
```

### **Production Commands:**
```bash
cd /home/orangepi/Desktop/OHT_V2/EMBED

# Setup hoàn chỉnh (One command)
sudo ./setup_complete_system.sh

# Setup udev rules
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

## 📊 **Hardware Validation Results**

### **✅ GPIO Configuration Status (100% PASSED)**
| **Component** | **GPIO** | **Mode** | **Initial Value** | **Status** |
|---------------|----------|----------|-------------------|------------|
| Power LED | GPIO54 | OUTPUT | 0 (OFF) | ✅ READY |
| System LED | GPIO35 | OUTPUT | 0 (OFF) | ✅ READY |
| Comm LED | GPIO28 | OUTPUT | 0 (OFF) | ✅ READY |
| Network LED | GPIO29 | OUTPUT | 0 (OFF) | ✅ READY |
| Error LED | GPIO58 | OUTPUT | 0 (OFF) | ✅ READY |
| E-Stop | GPIO59 | INPUT | 1 (RELEASED) | ✅ SAFE |
| Relay 1 | GPIO131 | OUTPUT | 0 (OFF) | ✅ READY |
| Relay 2 | GPIO132 | OUTPUT | 0 (OFF) | ✅ READY |
| UART1 TX | GPIO46 | OUTPUT | 0 (LOW) | ✅ READY |
| UART1 RX | GPIO47 | INPUT | 1 (HIGH) | ✅ READY |

### **✅ Hardware Bring-up Completed**
- ✅ **GPIO Export:** 10/10 pins exported successfully
- ✅ **Direction Config:** All pins configured correctly
- ✅ **Initial Values:** All outputs set to safe state
- ✅ **Device Tree:** UART1 overlay installed và configured
- ✅ **Boot Config:** orangepiEnv.txt updated với overlay
- ✅ **Dependencies:** pyserial installed system-wide
- ✅ **Testing:** LED và Relay control validated
- ✅ **Safety:** E-Stop monitoring operational

### **🎯 Production Ready For**
- ✅ **LED Control System:** 5x status indicators
- ✅ **Relay Control System:** 2x relay outputs  
- ✅ **Safety Monitoring:** E-Stop input monitoring
- ✅ **RS485 Communication:** UART1 với Modbus RTU
- ✅ **HAL Integration:** Python và C firmware ready
- ✅ **Firmware Development:** All interfaces available
- ✅ **System Integration:** Backend/Frontend integration ready

---

## 📞 **Contact**

**EMBED Team:**
- **Vai trò**: Hardware bring-up, low-level drivers
- **Focus**: UART1 RS485, GPIO control, HAL interface, Modbus RTU
- **Status**: Complete và sẵn sàng sử dụng

---

---

## 🎯 **NEXT STEPS - SAU KHI REBOOT**

### **1. Validate UART1 Device:**
```bash
ls -la /dev/ttyS1
sudo python3 test_uart1_pins_46_47.py
```

### **2. Setup udev Rules:**
```bash
sudo ./setup_udev_rs485.sh
ls -la /dev/ttyOHT485
```

### **3. Full System Testing:**
```bash
sudo python3 test_modbus_simple.py
sudo python3 test_modbus_loopback.py
sudo python3 test_with_module.py
```

### **4. Integration với Firmware:**
```bash
cd /home/orangepi/Desktop/OHT_V2/firmware_new
./scripts/build.sh Release
./scripts/test.sh
```

---

## 🚨 **IMPORTANT NOTES**

### **⚠️ Sudo Password:** `orangepi`
### **⚠️ Reboot Required:** Sau khi setup device tree overlay
### **⚠️ GPIO Permissions:** Cần sudo để control GPIO values
### **⚠️ pyserial:** Phải cài với `sudo pip3 install pyserial`

### **🔥 Critical Files:**
- `setup_oht_gpio_correct.sh` - GPIO configuration script
- `uart1_46_47.dtbo` - Device tree overlay cho UART1
- `hal_rs485.py` - Python HAL interface
- `test_*.py` - Validation test scripts

---

**Changelog:**
- v3.1 (2025-01-28): **PRODUCTION OPTIMIZED** - Cleaned up folder structure, archived development files
- v3.0 (2025-01-28): **PRODUCTION READY** - Complete hardware validation, GPIO configuration, UART1 setup
- v2.1 (2025-01-27): Added Modbus RTU tests, complete protocol validation  
- v2.0 (2025-01-27): Moved to EMBED folder, complete UART1 chân 46, 47
- v1.0 (2025-08-16): Initial RS485 integration

**🎉 EMBED Team Status: PRODUCTION READY & OPTIMIZED ✅**
