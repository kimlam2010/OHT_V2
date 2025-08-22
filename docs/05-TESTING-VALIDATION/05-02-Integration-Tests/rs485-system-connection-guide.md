---
title: "Rs485 System Connection Guide"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "QA Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['testing-validation', 'rs485', 'modbus', 'communication', 'guide', 'documentation']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# HƯỚNG DẪN KẾT NỐI HỆ THỐNG RS485 OHT-50
## OHT-50 Master Module - System Connection Guide

**Phiên bản:** v1.0  
**Ngày tạo:** 2025-01-27  
**Team:** FW Team  
**Trạng thái:** Connection Guide  

---

## 🎯 **TỔNG QUAN KẾT NỐI**

### **Mục tiêu:**
- Hướng dẫn kết nối Master Module với các module qua RS485
- Cấu hình phần cứng và phần mềm cho hệ thống
- Kiểm tra và xác nhận kết nối hoạt động
- Troubleshooting các vấn đề kết nối

### **Sơ đồ kết nối tổng thể:**
```
┌─────────────────────────────────────────────────────────┐
│                    OHT-50 SYSTEM                        │
├─────────────────────────────────────────────────────────┤
│  ⚙️ Master Module (Orange Pi 5B)                        │
│  ├── UART1_TX (Pin 8) ──┐                              │
│  ├── UART1_RX (Pin 10) ──┤                              │
│                          │                              │
│  └── GND ────────────────┤                              │
├─────────────────────────────────────────────────────────┤
│  📡 RS485 Bus Network                                   │
│  ├── A+ (Data+) ─────────┼── Power Module (0x02)       │
│  ├── B- (Data-) ─────────┼── Stepper Motor (0x03)      │
│  ├── DE (Data Enable) ───┼── DC Motor (0x04)           │
│  └── GND (Ground) ───────┼── Sensor Module (0x05)      │
│                           ├── DI/DO Module (0x06)      │
│                           ├── AI Module (0x07)         │
│                           └── Custom Modules (0x08+)   │
└─────────────────────────────────────────────────────────┘
```

---

## 🔧 **PHẦN CỨNG KẾT NỐI**

### **1. Master Module (Orange Pi 5B):**

#### **Pin Assignment:**
```bash
# UART1 Configuration
UART1_TX: GPIO1_A0 (Pin 8)    # Transmit Data
UART1_RX: GPIO1_A1 (Pin 10)   # Receive Data

GND:      Common Ground       # Ground reference
```

#### **RS485 Transceiver (MAX485):**
```
┌─────────────────┐
│    MAX485       │
├─────────────────┤
│ RO ── UART1_RX  │
│ RE ── NC        │
│ DE ── NC        │
│ DI ── UART1_TX  │
│ A  ── RS485_A+  │
│ B  ── RS485_B-  │
│ VCC ── 3.3V     │
│ GND ── GND      │
└─────────────────┘
```

### **2. RS485 Bus Wiring:**

#### **Cable Specification:**
```
Cable Type: Shielded Twisted Pair (STP)
Conductor: 2x 24AWG stranded copper
Shield: Aluminum foil + braid
Impedance: 120Ω ±10%
Capacitance: < 50pF/m
Max Length: 1200m at 115200 bps
```

#### **Termination:**
```
End-of-line termination:
├── 120Ω resistor between A+ and B-
├── Bias resistors: 4.7kΩ pull-up on A+, 4.7kΩ pull-down on B-
└── Shield connected to ground at one end only
```

### **3. Module Connection:**

#### **Standard Module Pinout:**
```
┌─────────────────┐
│   Module        │
├─────────────────┤
│ A+ ── RS485_A+  │
│ B- ── RS485_B-  │
│ DE ── RS485_DE  │
│ GND ── GND      │
│ VCC ── 5V/12V   │
└─────────────────┘
```

---

## 💻 **CẤU HÌNH PHẦN MỀM**

### **1. Device Tree Overlay:**

#### **UART1 Configuration:**
```bash
# Create device tree overlay
cat > uart1_rs485.dts << 'EOF'
/dts-v1/;
/plugin/;
/ {
    compatible = "rockchip,rk3588";
    fragment@0 {
        target = <&uart1>;
        __overlay__ {
            status = "okay";
            pinctrl-names = "default";
            pinctrl-0 = <&uart1_xfer>;
        };
    };
    fragment@1 {
        target = <&pinctrl>;
        __overlay__ {
            uart1_xfer: uart1-xfer {
                rockchip,pins = 
                    <1 17 1 &pcfg_pull_up>,  /* GPIO1_A0 (Pin 8) - TX */
                    <1 18 1 &pcfg_pull_up>;  /* GPIO1_A1 (Pin 10) - RX */
            };
        };
    };
};
EOF

# Compile and install
sudo dtc -@ -I dts -O dtb -o uart1_rs485.dtbo uart1_rs485.dts
sudo cp uart1_rs485.dtbo /boot/overlays/
```

#### **Boot Configuration:**
```bash
# Edit boot configuration
sudo nano /boot/orangepiEnv.txt

# Add overlay
overlays=uart1_rs485 uart0-m2 uart3-m0 uart4-m0
```

### **2. udev Rules:**

#### **Create udev rule:**
```bash
# Create udev rule for RS485 device
sudo nano /etc/udev/rules.d/99-oht-rs485.rules

# Add rule
KERNEL=="ttyS1", SYMLINK+="ttyOHT485", MODE="0666"
```

#### **Reload udev:**
```bash
# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger
```

### **3. Serial Configuration:**

#### **Configure serial port:**
```bash
# Configure UART1 for RS485
sudo stty -F /dev/ttyS1 115200 raw -echo -echoe -echok
sudo stty -F /dev/ttyS1 crtscts off clocal off hupcl off

# Or use udev rule for permanent configuration
sudo nano /etc/udev/rules.d/99-oht-rs485.rules

# Add configuration
KERNEL=="ttyS1", SYMLINK+="ttyOHT485", MODE="0666", RUN+="/bin/stty -F /dev/ttyS1 115200 raw -echo -echoe -echok crtscts off clocal off hupcl off"
```

---

## 🔧 **CẤU HÌNH MODULE**

### **1. Module Address Configuration:**

#### **Power Module (0x02):**
```bash
# Set module address using dip switches or configuration
# Address: 0x02 (Binary: 00000010)
# Dip switches: [OFF, ON, OFF, OFF, OFF, OFF, OFF, OFF]
```

#### **Stepper Motor Module (0x03):**
```bash
# Address: 0x03 (Binary: 00000011)
# Dip switches: [OFF, ON, ON, OFF, OFF, OFF, OFF, OFF]
```

#### **DC Motor Module (0x04):**
```bash
# Address: 0x04 (Binary: 00000100)
# Dip switches: [OFF, OFF, OFF, ON, OFF, OFF, OFF, OFF]
```

#### **Sensor Module (0x05):**
```bash
# Address: 0x05 (Binary: 00000101)
# Dip switches: [OFF, OFF, OFF, ON, ON, OFF, OFF, OFF]
```

### **2. Module Configuration Files:**

#### **Power Module Config:**
```json
{
  "power_module": {
    "address": "0x02",
    "name": "Power Management Module",
    "type": "power",
    "enabled": true,
    "timeout_ms": 1000,
    "retry_count": 3
  }
}
```

#### **Stepper Motor Config:**
```json
{
  "stepper_module": {
    "address": "0x03",
    "name": "Stepper Motor Module",
    "type": "stepper",
    "enabled": true,
    "timeout_ms": 1000,
    "retry_count": 3
  }
}
```

---

## 🧪 **KIỂM TRA KẾT NỐI**

### **1. Hardware Connection Test:**

#### **Continuity Test:**
```bash
# Test RS485 bus continuity
echo "Testing RS485 bus continuity..."

# Test A+ line
sudo gpioset gpiochip1 17=1  # Set TX high
sleep 1
sudo gpioinfo gpiochip1 18   # Read RX

# Test B- line
sudo gpioset gpiochip1 17=0  # Set TX low
sleep 1
sudo gpioinfo gpiochip1 18   # Read RX
```

#### **Voltage Test:**
```bash
# Test RS485 voltage levels
echo "Testing RS485 voltage levels..."

# Test A+ voltage (should be ~3.3V when idle)
sudo gpioset gpiochip1 17=1
sleep 1
# Measure voltage at A+ pin

# Test B- voltage (should be ~0V when idle)
sudo gpioset gpiochip1 17=0
sleep 1
# Measure voltage at B- pin
```

### **2. Communication Test:**

#### **Basic Communication Test:**
```python
#!/usr/bin/env python3
import serial
import time

def test_rs485_communication():
    # Open serial port
    ser = serial.Serial(
        port='/dev/ttyS1',
        baudrate=115200,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=1
    )
    
    # Test communication with each module
    modules = [0x02, 0x03, 0x04, 0x05]
    
    for addr in modules:
        print(f"Testing module at address 0x{addr:02X}")
        
        # Send Modbus RTU read holding registers command
        # Function code 0x03, starting address 0x0000, count 1
        command = bytes([addr, 0x03, 0x00, 0x00, 0x00, 0x01])
        
        # Calculate CRC
        crc = calculate_crc16(command)
        command += crc.to_bytes(2, byteorder='little')
        
        # Send command
        ser.write(command)
        
        # Read response
        response = ser.read(7)  # Expected response length
        
        if len(response) == 7:
            print(f"  ✓ Module 0x{addr:02X} responded")
        else:
            print(f"  ✗ Module 0x{addr:02X} no response")
        
        time.sleep(0.1)
    
    ser.close()

def calculate_crc16(data):
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x0001:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc

if __name__ == "__main__":
    test_rs485_communication()
```

#### **Advanced Communication Test:**
```python
#!/usr/bin/env python3
import serial
import struct
import time

class RS485Tester:
    def __init__(self, port='/dev/ttyS1', baudrate=115200):
        self.ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1
        )
    
    def send_modbus_command(self, address, function, data):
        # Build command
        command = bytes([address, function]) + data
        
        # Calculate CRC
        crc = self.calculate_crc16(command)
        command += crc.to_bytes(2, byteorder='little')
        
        # Send command
        self.ser.write(command)
        
        # Read response
        response = self.ser.read(256)
        
        return response
    
    def test_module(self, address, module_type):
        print(f"Testing {module_type} module at address 0x{address:02X}")
        
        # Test 1: Read module information
        response = self.send_modbus_command(address, 0x03, b'\x00\x00\x00\x10')
        if len(response) > 0:
            print(f"  ✓ Module information read: {response.hex()}")
        else:
            print(f"  ✗ No response from module")
            return False
        
        # Test 2: Read status registers
        response = self.send_modbus_command(address, 0x03, b'\x00\x20\x00\x10')
        if len(response) > 0:
            print(f"  ✓ Status registers read: {response.hex()}")
        else:
            print(f"  ✗ Status read failed")
            return False
        
        return True
    
    def calculate_crc16(self, data):
        crc = 0xFFFF
        for byte in data:
            crc ^= byte
            for _ in range(8):
                if crc & 0x0001:
                    crc = (crc >> 1) ^ 0xA001
                else:
                    crc >>= 1
        return crc
    
    def close(self):
        self.ser.close()

def main():
    tester = RS485Tester()
    
    # Test all modules
    modules = [
        (0x02, "Power"),
        (0x03, "Stepper Motor"),
        (0x04, "DC Motor"),
        (0x05, "Sensor")
    ]
    
    for addr, module_type in modules:
        success = tester.test_module(addr, module_type)
        if success:
            print(f"  ✓ {module_type} module test PASSED")
        else:
            print(f"  ✗ {module_type} module test FAILED")
        print()
    
    tester.close()

if __name__ == "__main__":
    main()
```

---

## 🔧 **TROUBLESHOOTING**

### **1. Common Issues:**

#### **No Communication:**
```
Problem: No response from modules
Possible Causes:
├── Incorrect wiring (A+/B- swapped)
├── Missing termination resistors
├── Wrong baud rate
├── Module not powered
├── Module address mismatch
└── RS485 transceiver fault

Solutions:
├── Check wiring connections
├── Verify termination resistors
├── Confirm baud rate settings
├── Check power supply
├── Verify module addresses
└── Test RS485 transceiver
```

#### **Intermittent Communication:**
```
Problem: Communication works sometimes
Possible Causes:
├── Loose connections
├── EMI interference
├── Ground loop
├── Cable quality issues
└── Power supply noise

Solutions:
├── Tighten all connections
├── Use shielded cable
├── Single point grounding
├── Replace with quality cable
└── Add power filtering
```

#### **High Error Rate:**
```
Problem: High CRC error rate
Possible Causes:
├── Cable too long
├── High noise environment
├── Incorrect termination
├── Baud rate too high
└── Poor cable quality

Solutions:
├── Reduce cable length
├── Add EMI shielding
├── Check termination
├── Reduce baud rate
└── Use better cable
```

### **2. Diagnostic Tools:**

#### **RS485 Analyzer:**
```python
#!/usr/bin/env python3
import serial
import time
import struct

class RS485Analyzer:
    def __init__(self, port='/dev/ttyS1', baudrate=115200):
        self.ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=0.1
        )
    
    def monitor_traffic(self, duration=60):
        print(f"Monitoring RS485 traffic for {duration} seconds...")
        start_time = time.time()
        packet_count = 0
        error_count = 0
        
        while time.time() - start_time < duration:
            if self.ser.in_waiting > 0:
                data = self.ser.read(self.ser.in_waiting)
                if data:
                    packet_count += 1
                    print(f"Packet {packet_count}: {data.hex()}")
                    
                    # Check for valid Modbus frame
                    if len(data) >= 4:
                        if not self.validate_crc(data):
                            error_count += 1
                            print(f"  CRC Error in packet {packet_count}")
        
        print(f"\nMonitoring complete:")
        print(f"  Total packets: {packet_count}")
        print(f"  Error packets: {error_count}")
        print(f"  Error rate: {error_count/packet_count*100:.2f}%" if packet_count > 0 else "  No packets received")
    
    def validate_crc(self, data):
        if len(data) < 4:
            return False
        
        # Extract data and CRC
        frame_data = data[:-2]
        received_crc = struct.unpack('<H', data[-2:])[0]
        
        # Calculate expected CRC
        expected_crc = self.calculate_crc16(frame_data)
        
        return received_crc == expected_crc
    
    def calculate_crc16(self, data):
        crc = 0xFFFF
        for byte in data:
            crc ^= byte
            for _ in range(8):
                if crc & 0x0001:
                    crc = (crc >> 1) ^ 0xA001
                else:
                    crc >>= 1
        return crc
    
    def close(self):
        self.ser.close()

# Usage
analyzer = RS485Analyzer()
analyzer.monitor_traffic(30)  # Monitor for 30 seconds
analyzer.close()
```

---

## 📋 **CHECKLIST KẾT NỐI**

### **1. Hardware Checklist:**
- [ ] Master Module powered on
- [ ] RS485 transceiver connected correctly
- [ ] A+ and B- lines connected properly
- [ ] Termination resistors installed (120Ω)
- [ ] Bias resistors installed (4.7kΩ)
- [ ] Ground connection established
- [ ] Shield connected to ground
- [ ] All modules powered on
- [ ] Module addresses set correctly

### **2. Software Checklist:**
- [ ] Device tree overlay installed
- [ ] Boot configuration updated
- [ ] udev rules created
- [ ] Serial port configured
- [ ] Module configuration files created
- [ ] HAL drivers installed
- [ ] Application software installed
- [ ] Test scripts available

### **3. Testing Checklist:**
- [ ] Hardware continuity test passed
- [ ] Voltage levels correct
- [ ] Basic communication test passed
- [ ] Module discovery working
- [ ] Register read/write working
- [ ] Error handling working
- [ ] Performance test passed
- [ ] Documentation completed

---

## 📚 **TÀI LIỆU THAM KHẢO**

### **1. Technical Standards:**
- **RS485 Standard:** TIA/EIA-485-A
- **Modbus RTU:** Modbus Organization
- **Industrial Communication:** IEC 61158

### **2. Hardware Documentation:**
- **Orange Pi 5B:** Radxa documentation
- **MAX485:** Maxim Integrated datasheet
- **RS485 Transceivers:** Various manufacturers

### **3. Software Documentation:**
- **Linux Serial Programming:** Linux kernel documentation
- **Python Serial:** PySerial library documentation
- **Modbus Protocol:** Modbus Organization specifications

---

**Changelog:**
- v1.0 (2025-01-27): Initial system connection guide
- v1.1 (2025-01-27): Removed GPIO_DE pin (not needed for RS485)
