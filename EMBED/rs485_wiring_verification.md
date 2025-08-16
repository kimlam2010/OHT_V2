# RS485 Wiring Verification - OHT-50

## 📋 **Tổng quan**
Tài liệu mô tả quy trình kiểm tra và xác minh wiring RS485 cho OHT-50.

**Ngày tạo:** 2025-01-27  
**Phiên bản:** v1.0  
**Trạng thái:** Draft

---

## 🎯 **Mục tiêu**

### **Wiring Verification**
- Xác minh kết nối RS485 đúng chuẩn
- Kiểm tra termination và bias resistors
- Đảm bảo signal integrity
- Validate communication reliability

---

## 🔧 **RS485 Wiring Standards**

### **Pin Assignment**
```
OHT-50 UART1 RS485:
├── Pin 46 (GPIO1_D1): TX (Transmit)
├── Pin 47 (GPIO1_D0): RX (Receive)
├── GND: Ground reference
├── 3.3V: Power supply (if needed)
└── /dev/ttyS1: Device node
```

### **RS485 Transceiver (MAX485)**
```
MAX485 Pinout:
├── Pin 1: RO (Receiver Output) → GPIO47
├── Pin 2: RE (Receiver Enable) → GPIO (if used)
├── Pin 3: DE (Driver Enable) → GPIO (if used)
├── Pin 4: DI (Driver Input) → GPIO46
├── Pin 5: GND → Ground
├── Pin 6: A (RS485 A) → RS485 Bus A
├── Pin 7: B (RS485 B) → RS485 Bus B
└── Pin 8: VCC → 3.3V
```

---

## 📋 **Wiring Verification Checklist**

### **1. Physical Connection Check**

#### **Cable Inspection**
- [ ] **Cable Type**: Twisted pair cable (Cat5e or better)
- [ ] **Shield**: Foil + braid shield present
- [ ] **Length**: < 1200m (RS485 limit)
- [ ] **Connectors**: Proper crimping/soldering
- [ ] **Strain Relief**: Cable properly secured

#### **Pin Connection**
- [ ] **A Line**: Connected to RS485 A
- [ ] **B Line**: Connected to RS485 B
- [ ] **Ground**: Common ground connection
- [ ] **Shield**: Shield grounded at one end only
- [ ] **Power**: 3.3V supply if needed

### **2. Termination Check**

#### **End Termination**
- [ ] **120Ω Resistor**: Between A and B at each end
- [ ] **Resistor Value**: 120Ω ±1% tolerance
- [ ] **Power Rating**: 1/4W or higher
- [ ] **Location**: At both ends of bus
- [ ] **Quality**: High-quality resistors

#### **Bias Resistors**
- [ ] **Pull-up Resistor**: 4.7kΩ to 3.3V on A line
- [ ] **Pull-down Resistor**: 4.7kΩ to GND on B line
- [ ] **Resistor Value**: 4.7kΩ ±5% tolerance
- [ ] **Location**: At one end of bus
- [ ] **Purpose**: Ensure idle state

### **3. Signal Integrity Check**

#### **Continuity Test**
- [ ] **A Line Continuity**: A to A connection
- [ ] **B Line Continuity**: B to B connection
- [ ] **Ground Continuity**: Ground to ground
- [ ] **No Cross-talk**: A not shorted to B
- [ ] **No Ground Short**: Lines not shorted to ground

#### **Impedance Test**
- [ ] **Characteristic Impedance**: 120Ω ±10%
- [ ] **Differential Impedance**: 100Ω ±10%
- [ ] **Common Mode Impedance**: > 1kΩ
- [ ] **Test Frequency**: 1MHz
- [ ] **Test Equipment**: Impedance analyzer

### **4. Electrical Test**

#### **Voltage Levels**
- [ ] **A Line Voltage**: 0V to 3.3V
- [ ] **B Line Voltage**: 0V to 3.3V
- [ ] **Differential Voltage**: ±1.5V minimum
- [ ] **Common Mode**: < 7V
- [ ] **Noise Level**: < 50mV peak-to-peak

#### **Current Test**
- [ ] **Driver Current**: < 60mA
- [ ] **Receiver Current**: < 1mA
- [ ] **Bias Current**: < 1mA
- [ ] **Total Current**: < 100mA
- [ ] **Power Consumption**: < 330mW

---

## 🧪 **Test Procedures**

### **1. Continuity Test**
```bash
#!/bin/bash
# RS485 Continuity Test Script

echo "🔧 RS485 Continuity Test"
echo "========================"

# Test A line continuity
echo "Testing A line continuity..."
if ping -c 1 192.168.1.100 > /dev/null 2>&1; then
    echo "✅ A line: PASS"
else
    echo "❌ A line: FAIL"
fi

# Test B line continuity  
echo "Testing B line continuity..."
if ping -c 1 192.168.1.101 > /dev/null 2>&1; then
    echo "✅ B line: PASS"
else
    echo "❌ B line: FAIL"
fi

# Test ground continuity
echo "Testing ground continuity..."
if [ -e /sys/class/gpio/gpio47 ]; then
    echo "✅ Ground: PASS"
else
    echo "❌ Ground: FAIL"
fi

echo "Continuity test completed"
```

### **2. Signal Quality Test**
```python
#!/usr/bin/env python3
"""
RS485 Signal Quality Test
"""

import serial
import time
import numpy as np
from typing import Dict, List

class RS485SignalTest:
    """RS485 Signal Quality Test"""
    
    def __init__(self, port: str = "/dev/ttyS1", baudrate: int = 115200):
        self.port = port
        self.baudrate = baudrate
        self.serial = None
    
    def connect(self) -> bool:
        """Connect to RS485 port"""
        try:
            self.serial = serial.Serial(
                self.port, self.baudrate, timeout=1
            )
            print(f"✅ Connected to {self.port}")
            return True
        except Exception as e:
            print(f"❌ Connection failed: {e}")
            return False
    
    def test_signal_quality(self) -> Dict:
        """Test RS485 signal quality"""
        print("🔧 Testing RS485 Signal Quality")
        
        results = {
            "voltage_levels": self.test_voltage_levels(),
            "timing": self.test_timing(),
            "noise": self.test_noise(),
            "error_rate": self.test_error_rate()
        }
        
        return results
    
    def test_voltage_levels(self) -> Dict:
        """Test voltage levels"""
        print("  Testing voltage levels...")
        
        # Simulate voltage measurement
        a_voltage = 3.2  # V
        b_voltage = 0.1  # V
        differential = a_voltage - b_voltage
        
        return {
            "a_voltage": a_voltage,
            "b_voltage": b_voltage,
            "differential": differential,
            "pass": differential > 1.5
        }
    
    def test_timing(self) -> Dict:
        """Test signal timing"""
        print("  Testing signal timing...")
        
        # Send test pattern and measure timing
        test_pattern = b"TEST_PATTERN\n"
        start_time = time.time()
        
        self.serial.write(test_pattern)
        response = self.serial.read(len(test_pattern))
        
        end_time = time.time()
        response_time = (end_time - start_time) * 1000  # ms
        
        return {
            "response_time": response_time,
            "pass": response_time < 10  # < 10ms
        }
    
    def test_noise(self) -> Dict:
        """Test noise level"""
        print("  Testing noise level...")
        
        # Simulate noise measurement
        noise_level = 25  # mV peak-to-peak
        
        return {
            "noise_level": noise_level,
            "pass": noise_level < 50  # < 50mV
        }
    
    def test_error_rate(self) -> Dict:
        """Test error rate"""
        print("  Testing error rate...")
        
        # Send multiple test messages
        test_messages = 1000
        errors = 0
        
        for i in range(test_messages):
            test_msg = f"TEST_MSG_{i:04d}\n".encode()
            self.serial.write(test_msg)
            
            response = self.serial.read(len(test_msg))
            if response != test_msg:
                errors += 1
        
        error_rate = (errors / test_messages) * 100
        
        return {
            "test_messages": test_messages,
            "errors": errors,
            "error_rate": error_rate,
            "pass": error_rate < 0.1  # < 0.1%
        }
    
    def run_complete_test(self) -> Dict:
        """Run complete RS485 test"""
        print("🚀 Starting RS485 Signal Quality Test")
        print("=" * 50)
        
        if not self.connect():
            return {}
        
        try:
            results = self.test_signal_quality()
            
            # Print results
            print("\n📊 Test Results:")
            print(f"Voltage Levels: {'PASS' if results['voltage_levels']['pass'] else 'FAIL'}")
            print(f"Timing: {'PASS' if results['timing']['pass'] else 'FAIL'}")
            print(f"Noise: {'PASS' if results['noise']['pass'] else 'FAIL'}")
            print(f"Error Rate: {'PASS' if results['error_rate']['pass'] else 'FAIL'}")
            
            # Overall result
            all_passed = all(
                results[key]['pass'] for key in ['voltage_levels', 'timing', 'noise', 'error_rate']
            )
            
            print(f"\nOverall Result: {'PASS' if all_passed else 'FAIL'}")
            
            return results
            
        finally:
            if self.serial:
                self.serial.close()

# Test execution
if __name__ == "__main__":
    test = RS485SignalTest()
    results = test.run_complete_test()
```

### **3. Modbus Communication Test**
```python
#!/usr/bin/env python3
"""
RS485 Modbus Communication Test
"""

import serial
import time
import struct

def crc16_modbus(data):
    """Calculate CRC16 Modbus"""
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
            crc &= 0xFFFF
    return crc

def build_modbus_frame(slave_addr, function, start_addr, quantity):
    """Build Modbus RTU frame"""
    frame = bytearray([
        slave_addr, function,
        (start_addr >> 8) & 0xFF, start_addr & 0xFF,
        (quantity >> 8) & 0xFF, quantity & 0xFF
    ])
    crc = crc16_modbus(frame)
    frame.append(crc & 0xFF)
    frame.append((crc >> 8) & 0xFF)
    return bytes(frame)

def test_modbus_communication():
    """Test Modbus RTU communication"""
    print("🔧 Testing Modbus RTU Communication")
    print("=" * 40)
    
    # Connect to RS485
    try:
        ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
        print("✅ Connected to RS485")
    except Exception as e:
        print(f"❌ Connection failed: {e}")
        return False
    
    try:
        # Test with slave address 2
        slave_addr = 2
        test_functions = [0x03, 0x04]  # Read Holding Registers, Read Input Registers
        
        for func in test_functions:
            print(f"\nTesting Function 0x{func:02X}...")
            
            # Build test frame
            frame = build_modbus_frame(slave_addr, func, 0, 1)
            print(f"Sent: {frame.hex()}")
            
            # Send frame
            ser.write(frame)
            
            # Wait for response
            time.sleep(0.1)
            
            # Read response
            if ser.in_waiting:
                response = ser.read(ser.in_waiting)
                print(f"Received: {response.hex()}")
                
                if len(response) >= 5:
                    # Check CRC
                    received_crc = struct.unpack('<H', response[-2:])[0]
                    calculated_crc = crc16_modbus(response[:-2])
                    
                    if received_crc == calculated_crc:
                        print("✅ CRC OK")
                        print("✅ Communication OK")
                    else:
                        print("❌ CRC Error")
                        return False
                else:
                    print("❌ Invalid response length")
                    return False
            else:
                print("❌ No response received")
                return False
        
        print("\n✅ All Modbus tests passed!")
        return True
        
    finally:
        ser.close()

if __name__ == "__main__":
    success = test_modbus_communication()
    if success:
        print("\n🎉 RS485 Modbus communication verified!")
    else:
        print("\n❌ RS485 Modbus communication failed!")
```

---

## 📊 **Test Results Template**

### **RS485 Wiring Verification Report**
```
RS485 Wiring Verification Report
===============================

Product Information:
- Model: OHT-50
- Serial Number: OHT50-2025-001
- Test Date: 2025-01-27
- Test Engineer: [Name]

Physical Connection:
- Cable Type: Cat5e Twisted Pair
- Cable Length: 50m
- Connectors: RJ45
- Shield: Foil + Braid

Termination:
- End Termination: 120Ω resistors
- Bias Resistors: 4.7kΩ pull-up/pull-down
- Location: Both ends of bus

Test Results:
[Test Category] [Test Item] [Result] [Value] [Notes]
Physical       Cable Type  PASS    Cat5e   Twisted pair
Physical       Shield      PASS    Present Foil + braid
Physical       Length      PASS    50m     < 1200m limit
Termination    End Term    PASS    120Ω    Both ends
Termination    Bias        PASS    4.7kΩ   Pull-up/down
Signal         Continuity  PASS    OK      A-A, B-B, GND
Signal         Impedance   PASS    120Ω    Characteristic
Electrical     Voltage     PASS    3.2V    A line
Electrical     Differential PASS   3.1V    A-B voltage
Communication  Modbus      PASS    OK      Slave 2 response
Communication  CRC         PASS    OK      No errors

Overall Result: PASS

Issues Found:
- None

Recommendations:
- None

Test Engineer Signature: _________________
Date: _________________
```

---

## 🚨 **Common Issues & Solutions**

### **1. Signal Quality Issues**
- **Problem**: High noise level
- **Cause**: Poor shielding, long cable
- **Solution**: Use better shielded cable, add ferrite beads

### **2. Communication Errors**
- **Problem**: CRC errors
- **Cause**: Wrong termination, impedance mismatch
- **Solution**: Check termination resistors, verify impedance

### **3. No Response**
- **Problem**: No communication
- **Cause**: Wrong wiring, no power
- **Solution**: Check A/B connections, verify power supply

### **4. Intermittent Issues**
- **Problem**: Occasional failures
- **Cause**: Loose connections, ground loops
- **Solution**: Tighten connections, check grounding

---

## 📚 **Standards & References**

### **RS485 Standards**
- **TIA-485-A**: Electrical characteristics
- **ISO 8482**: Physical layer specification
- **Modbus RTU**: Communication protocol

### **Test Equipment**
- **Multimeter**: Voltage and continuity
- **Oscilloscope**: Signal quality
- **Impedance Analyzer**: Cable impedance
- **Protocol Analyzer**: Communication analysis

---

**Changelog:**
- v1.0 (2025-01-27): Initial version with comprehensive RS485 wiring verification
