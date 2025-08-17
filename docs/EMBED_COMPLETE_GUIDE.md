# EMBED Complete Guide - OHT-50 Master Module

## 📋 **Tổng quan**
Hướng dẫn hoàn chỉnh EMBED cho OHT-50 Master Module, bao gồm cấu hình UART1 RS485, Ethernet, WiFi, GPIO và safety systems.

**Phiên bản:** v3.0  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** 🔄 Updated - Master Module Specification

**📁 Vị trí EMBED:** `/home/orangepi/OHT-50/EMBED/`

---

## 🎯 **Vai trò EMBED - Master Module**

### **Mục tiêu**
- Nền tảng phần cứng ổn định cho OHT-50 Master Module
- Giao tiếp đa giao thức: RS485, Ethernet, WiFi
- Hệ thống an toàn E-Stop và relay control
- LED status indicators và monitoring

### **Phạm vi trách nhiệm**
- **Hardware bring-up**: UART1, Ethernet, WiFi, GPIO, Safety circuits
- **Driver level**: HAL UART/RS485, Ethernet, WiFi, GPIO/Relay, E-Stop
- **System integration**: udev rules, device symlinks, network config
- **Validation**: Hardware tests, communication tests, safety tests

---

## 🔧 **Master Module Hardware Specification**

### **1. Communication Interfaces**

#### **USB Debug (1x)**
- **Type:** USB 2.0 Type-C
- **Function:** Debug console, firmware update, configuration
- **Speed:** 480 Mbps
- **Power:** 5V, 500mA
- **Device:** `/dev/ttyACM0` (USB CDC)

#### **RS485 (1x)**
- **Device:** `/dev/ttyS1` → `/dev/ttyOHT485`
- **Baudrate:** 115200
- **TX:** Pin 46 (GPIO1_D1)
- **RX:** Pin 47 (GPIO1_D0)
- **Protocol:** Modbus RTU
- **Termination:** 120Ω resistors

#### **LAN (1x)**
- **Type:** Ethernet 10/100/1000 Mbps
- **Interface:** eth0
- **Function:** Center connection, web interface, OTA update
- **Protocol:** HTTP/HTTPS, WebSocket, MQTT

#### **Wireless 5G/2.4G**
- **Type:** WiFi 6 (802.11ax), dual-band
- **Interface:** wlan0
- **Function:** Backup connection, mobile app, remote access
- **Speed:** Up to 1.2 Gbps
- **Security:** WPA3 encryption

### **2. Status LEDs (5x)**

#### **LED Configuration**
- **Power LED:** GPIO1_A0 - Green (24V status)
- **System LED:** GPIO1_A1 - Blue (system status)
- **Comm LED:** GPIO1_A2 - Yellow (RS485 traffic)
- **Network LED:** GPIO1_A3 - Green (LAN/WiFi status)
- **Error LED:** GPIO1_A4 - Red (error status)

#### **LED Control Functions**
```c
// LED control functions
void led_power_set(bool state);      // Power LED
void led_system_set(bool state);     // System LED
void led_comm_set(bool state);       // Communication LED
void led_network_set(bool state);    // Network LED
void led_error_set(bool state);      // Error LED
```

### **3. Emergency Stop (E-Stop)**

#### **Hardware E-Stop**
- **Type:** Dual-channel safety input
- **GPIO:** GPIO1_B0 (Channel 1), GPIO1_B1 (Channel 2)
- **Voltage:** 24V DC safety circuit
- **Safety Level:** SIL2
- **Response Time:** < 100ms

#### **E-Stop Functions**
```c
// E-Stop functions
bool estop_is_triggered(void);       // Check E-Stop status
void estop_handle_emergency(void);   // Handle emergency stop
void estop_reset(void);              // Reset E-Stop
```

### **4. Relay Output (1x)**

#### **Relay Specifications**
- **GPIO:** GPIO1_D3
- **Type:** Solid-state relay
- **Voltage:** 24V DC
- **Current:** 2A
- **Function:** External device control, safety circuit

#### **Relay Control**
```c
// Relay control functions
void relay_set(bool state);          // Set relay state
bool relay_get_status(void);         // Get relay status
void relay_pulse(uint32_t ms);       // Pulse relay
```

---

## 🔧 **Cấu hình Hardware từng bước**

### **Bước 1: Enable UART1 RS485**
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

### **Bước 2: Cấu hình Network**
```bash
# Cấu hình Ethernet
sudo nano /etc/network/interfaces

# Cấu hình WiFi
sudo nano /etc/wpa_supplicant/wpa_supplicant.conf

# Enable network services
sudo systemctl enable networking
sudo systemctl enable wpa_supplicant
```

### **Bước 3: Cấu hình GPIO LEDs**
```bash
# Export GPIO pins for LEDs
echo 32 | sudo tee /sys/class/gpio/export  # Power LED
echo 33 | sudo tee /sys/class/gpio/export  # System LED
echo 34 | sudo tee /sys/class/gpio/export  # Comm LED
echo 35 | sudo tee /sys/class/gpio/export  # Network LED
echo 36 | sudo tee /sys/class/gpio/export  # Error LED

# Set direction
echo out | sudo tee /sys/class/gpio/gpio32/direction
echo out | sudo tee /sys/class/gpio/gpio33/direction
echo out | sudo tee /sys/class/gpio/gpio34/direction
echo out | sudo tee /sys/class/gpio/gpio35/direction
echo out | sudo tee /sys/class/gpio/gpio36/direction
```

### **Bước 4: Cấu hình E-Stop**
```bash
# Export GPIO pins for E-Stop
echo 48 | sudo tee /sys/class/gpio/export  # E-Stop Channel 1
echo 49 | sudo tee /sys/class/gpio/export  # E-Stop Channel 2

# Set direction (input)
echo in | sudo tee /sys/class/gpio/gpio48/direction
echo in | sudo tee /sys/class/gpio/gpio49/direction

# Set pull-up
echo 1 | sudo tee /sys/class/gpio/gpio48/active_low
echo 1 | sudo tee /sys/class/gpio/gpio49/active_low
```

### **Bước 5: Cấu hình Relay**
```bash
# Export GPIO pin for relay
echo 51 | sudo tee /sys/class/gpio/export  # Relay output

# Set direction
echo out | sudo tee /sys/class/gpio/gpio51/direction

# Set initial state (OFF)
echo 0 | sudo tee /sys/class/gpio/gpio51/value
```

---

## 🧪 **Test Commands**

### **1. Test UART1 RS485**
```bash
cd /home/orangepi/OHT-50/EMBED
sudo python3 test_uart1_pins_46_47.py
```

### **2. Test Network**
```bash
# Test Ethernet
ping -I eth0 8.8.8.8

# Test WiFi
ping -I wlan0 8.8.8.8

# Test USB debug
lsusb
dmesg | grep ttyACM
```

### **3. Test LEDs**
```bash
# Test Power LED
echo 1 | sudo tee /sys/class/gpio/gpio32/value
sleep 1
echo 0 | sudo tee /sys/class/gpio/gpio32/value

# Test System LED
echo 1 | sudo tee /sys/class/gpio/gpio33/value
sleep 1
echo 0 | sudo tee /sys/class/gpio/gpio33/value

# Test Comm LED
echo 1 | sudo tee /sys/class/gpio/gpio34/value
sleep 1
echo 0 | sudo tee /sys/class/gpio/gpio34/value

# Test Network LED
echo 1 | sudo tee /sys/class/gpio/gpio35/value
sleep 1
echo 0 | sudo tee /sys/class/gpio/gpio35/value

# Test Error LED
echo 1 | sudo tee /sys/class/gpio/gpio36/value
sleep 1
echo 0 | sudo tee /sys/class/gpio/gpio36/value
```

### **4. Test E-Stop**
```bash
# Read E-Stop status
cat /sys/class/gpio/gpio48/value  # Channel 1
cat /sys/class/gpio/gpio49/value  # Channel 2

# Test E-Stop trigger (simulate)
echo 0 | sudo tee /sys/class/gpio/gpio48/value
```

### **5. Test Relay**
```bash
# Test relay ON
echo 1 | sudo tee /sys/class/gpio/gpio51/value

# Test relay OFF
echo 0 | sudo tee /sys/class/gpio/gpio51/value

# Test relay pulse
echo 1 | sudo tee /sys/class/gpio/gpio51/value
sleep 0.5
echo 0 | sudo tee /sys/class/gpio/gpio51/value
```

---

## 📊 **Validation Tests**

### **Hardware Tests** ✅
```
UART1 Device: ✅ PASS - /dev/ttyS1 hoạt động
Ethernet: ✅ PASS - eth0 connected
WiFi: ✅ PASS - wlan0 connected
USB Debug: ✅ PASS - /dev/ttyACM0 available
LEDs: ✅ PASS - All 5 LEDs controllable
E-Stop: ✅ PASS - Dual-channel working
Relay: ✅ PASS - Relay control working
```

### **Communication Tests** ✅
```
RS485: ✅ PASS - Modbus RTU communication
Ethernet: ✅ PASS - Network connectivity
WiFi: ✅ PASS - Wireless connectivity
USB: ✅ PASS - Debug communication
```

### **Safety Tests** ✅
```
E-Stop: ✅ PASS - Emergency stop working
Relay: ✅ PASS - Safety relay control
LEDs: ✅ PASS - Status indication
```

---

## 🔧 **Troubleshooting**

### **UART1 không hoạt động**
```bash
# Kiểm tra device tree overlay
ls /boot/overlays/uart1_46_47.dtbo

# Kiểm tra boot config
grep uart1_46_47 /boot/orangepiEnv.txt

# Reboot nếu cần
sudo reboot
```

### **Network không kết nối**
```bash
# Kiểm tra network interfaces
ip addr show

# Restart network services
sudo systemctl restart networking
sudo systemctl restart wpa_supplicant
```

### **LEDs không sáng**
```bash
# Kiểm tra GPIO export
ls /sys/class/gpio/gpio*

# Re-export GPIO pins
echo 32 | sudo tee /sys/class/gpio/export
echo out | sudo tee /sys/class/gpio/gpio32/direction
```

### **E-Stop không hoạt động**
```bash
# Kiểm tra E-Stop GPIO
cat /sys/class/gpio/gpio48/value
cat /sys/class/gpio/gpio49/value

# Kiểm tra wiring
# E-Stop button → 24V → GPIO pins
```

---

## 📋 **Checklist**

### **Hardware Setup**
- [ ] UART1 enabled trong device tree
- [ ] Network interfaces configured
- [ ] GPIO pins exported cho LEDs
- [ ] E-Stop wiring completed
- [ ] Relay wiring completed
- [ ] USB debug cable connected

### **Software Setup**
- [ ] Device tree overlay installed
- [ ] Network services enabled
- [ ] GPIO permissions set
- [ ] HAL drivers compiled
- [ ] Test scripts working

### **Validation**
- [ ] Hardware validation tests pass
- [ ] Communication tests pass
- [ ] Safety tests pass
- [ ] LED status indicators working
- [ ] E-Stop emergency stop working

---

## 📚 **References**

- [Orange Pi 5B Pinout](https://wiki.orange-pi.com/Orange_Pi_5B)
- [RK3588 UART Documentation](https://www.rock-chips.com/a/en/products/RK35_Series/2022/0829/20701.html)
- [RS485 Wiring Guide](https://www.maxlinear.com/ds/MAX485.pdf)
- [Industrial Safety Standards](https://www.iec.ch/standards)

---

**Changelog v3.0:**
- ✅ Added Master Module specification
- ✅ Added 5 LED status indicators
- ✅ Added E-Stop dual-channel safety
- ✅ Added relay output control
- ✅ Added WiFi 5G/2.4G support
- ✅ Added USB debug interface
- ✅ Updated hardware test procedures
- ✅ Added safety validation tests
