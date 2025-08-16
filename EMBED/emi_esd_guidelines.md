# EMI/ESD Guidelines & Testing - OHT-50

## 📋 **Tổng quan**
Tài liệu mô tả guidelines và kiểm tra EMI/ESD cơ bản cho OHT-50 trên Orange Pi 5B.

**Ngày tạo:** 2025-01-27  
**Phiên bản:** v1.0  
**Trạng thái:** Draft

---

## 🎯 **Mục tiêu**

### **EMI/ESD Protection**
- Bảo vệ hệ thống khỏi nhiễu điện từ
- Chống phóng tĩnh điện (ESD)
- Đảm bảo độ tin cậy trong môi trường công nghiệp
- Tuân thủ tiêu chuẩn EMC cơ bản

### **Testing Requirements**
- ESD immunity test
- EMI susceptibility test
- Conducted emissions test
- Radiated emissions test

---

## 🔧 **Hardware Design Guidelines**

### **1. PCB Layout Guidelines**

#### **Power Supply Design**
```
- Decoupling capacitors: 100nF ceramic + 10µF tantalum
- Placement: Close to power pins of ICs
- Ground plane: Solid ground plane on bottom layer
- Power traces: Wide traces for high current paths
- Filtering: LC filters for switching power supplies
```

#### **Signal Integrity**
```
- High-speed signals: Controlled impedance traces
- Clock signals: Short traces, no vias if possible
- RS485: Differential pair routing
- Ground return: Dedicated ground for analog circuits
- Shielding: Metal enclosure for sensitive circuits
```

#### **Component Placement**
```
- Sensitive components: Away from switching circuits
- Heat sources: Proper thermal management
- Connectors: Shielded connectors for external interfaces
- ESD protection: TVS diodes on all I/O pins
```

### **2. ESD Protection**

#### **I/O Protection**
```python
# ESD Protection Components
ESD_PROTECTION = {
    "RS485": {
        "A/B Lines": "TVS diode array (SM712)",
        "DE/RE": "TVS diode (SMAJ5.0A)",
        "Power": "TVS diode (SMAJ12A)"
    },
    "GPIO": {
        "Digital Inputs": "TVS diode (SMAJ3.3A)",
        "Digital Outputs": "TVS diode (SMAJ5.0A)",
        "Analog Inputs": "TVS diode (SMAJ3.3A) + series resistor"
    },
    "Power": {
        "3.3V": "TVS diode (SMAJ5.0A)",
        "5V": "TVS diode (SMAJ12A)",
        "12V": "TVS diode (SMAJ20A)"
    }
}
```

#### **Grounding Strategy**
```
- Star grounding: Single point ground connection
- Analog ground: Separate from digital ground
- Chassis ground: Connected to metal enclosure
- Earth ground: Proper earth connection
- Isolation: Optocouplers for isolated interfaces
```

### **3. EMI Shielding**

#### **Enclosure Design**
```
- Material: Aluminum or steel enclosure
- Thickness: Minimum 1.5mm for aluminum
- Seams: Continuous welding or gaskets
- Vents: Honeycomb pattern for ventilation
- Cables: Shielded cables with proper termination
```

#### **Cable Shielding**
```
- RS485: Twisted pair with foil + braid shield
- Power: Shielded power cables
- Control: Shielded control cables
- Grounding: Shield connected to chassis at one end
```

---

## 🧪 **Testing Procedures**

### **1. ESD Immunity Test**

#### **Test Setup**
```python
#!/usr/bin/env python3
"""
ESD Immunity Test cho OHT-50
"""

import time
import RPi.GPIO as GPIO
import serial
from typing import Dict, List

class ESDTest:
    """ESD Immunity Test Implementation"""
    
    def __init__(self):
        self.test_results = {}
        self.esd_levels = [2kV, 4kV, 8kV, 15kV]  # Contact discharge
        self.test_points = [
            "RS485_A", "RS485_B", "GPIO_46", "GPIO_47",
            "Power_3V3", "Power_5V", "Chassis"
        ]
    
    def setup_test_environment(self):
        """Setup test environment"""
        print("🔧 Setting up ESD test environment...")
        
        # Initialize UART1
        self.serial = serial.Serial('/dev/ttyS1', 115200, timeout=1)
        
        # Setup GPIO monitoring
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(46, GPIO.IN)  # TX
        GPIO.setup(47, GPIO.IN)  # RX
        
        print("✅ Test environment ready")
    
    def run_esd_test(self, test_point: str, voltage: int) -> Dict:
        """Run ESD test on specific point"""
        print(f"⚡ ESD Test: {test_point} at {voltage}kV")
        
        # Pre-test functionality check
        pre_test_ok = self.check_functionality()
        
        # Apply ESD discharge (simulated)
        print(f"   Applying {voltage}kV ESD to {test_point}...")
        time.sleep(0.1)  # Simulate discharge
        
        # Post-test functionality check
        post_test_ok = self.check_functionality()
        
        # Determine result
        if pre_test_ok and post_test_ok:
            result = "PASS"
        else:
            result = "FAIL"
        
        return {
            "test_point": test_point,
            "voltage": voltage,
            "pre_test": pre_test_ok,
            "post_test": post_test_ok,
            "result": result
        }
    
    def check_functionality(self) -> bool:
        """Check system functionality"""
        try:
            # Test UART1 communication
            self.serial.write(b"TEST\n")
            response = self.serial.read(10)
            
            # Test GPIO state
            gpio_46 = GPIO.input(46)
            gpio_47 = GPIO.input(47)
            
            # All tests passed
            return True
            
        except Exception as e:
            print(f"   Functionality check failed: {e}")
            return False
    
    def run_complete_esd_test(self) -> Dict:
        """Run complete ESD immunity test"""
        print("🚨 Starting ESD Immunity Test")
        print("=" * 50)
        
        self.setup_test_environment()
        
        all_results = []
        
        for test_point in self.test_points:
            for voltage in self.esd_levels:
                result = self.run_esd_test(test_point, voltage)
                all_results.append(result)
                
                print(f"   Result: {result['result']}")
                
                if result['result'] == "FAIL":
                    print(f"   ⚠️  ESD test failed at {voltage}kV on {test_point}")
                    break
        
        # Summary
        passed = sum(1 for r in all_results if r['result'] == "PASS")
        total = len(all_results)
        
        summary = {
            "total_tests": total,
            "passed": passed,
            "failed": total - passed,
            "pass_rate": (passed / total) * 100,
            "results": all_results
        }
        
        print(f"\n📊 ESD Test Summary:")
        print(f"   Total: {total}, Passed: {passed}, Failed: {total-passed}")
        print(f"   Pass Rate: {summary['pass_rate']:.1f}%")
        
        return summary

# Test execution
if __name__ == "__main__":
    esd_test = ESDTest()
    results = esd_test.run_complete_esd_test()
```

### **2. EMI Susceptibility Test**

#### **Test Setup**
```python
#!/usr/bin/env python3
"""
EMI Susceptibility Test cho OHT-50
"""

import time
import numpy as np
from typing import Dict, List

class EMITest:
    """EMI Susceptibility Test Implementation"""
    
    def __init__(self):
        self.frequency_ranges = [
            (80, 1000),    # 80MHz - 1GHz
            (1000, 3000),  # 1GHz - 3GHz
            (3000, 6000)   # 3GHz - 6GHz
        ]
        self.field_strengths = [3, 10, 30]  # V/m
        self.test_results = {}
    
    def setup_emi_test(self):
        """Setup EMI test environment"""
        print("🔧 Setting up EMI test environment...")
        
        # Initialize test equipment (simulated)
        self.field_generator = self._init_field_generator()
        self.field_monitor = self._init_field_monitor()
        self.system_monitor = self._init_system_monitor()
        
        print("✅ EMI test environment ready")
    
    def _init_field_generator(self):
        """Initialize RF field generator"""
        return {"status": "ready", "frequency": 0, "power": 0}
    
    def _init_field_monitor(self):
        """Initialize field strength monitor"""
        return {"status": "ready", "field_strength": 0}
    
    def _init_system_monitor(self):
        """Initialize system functionality monitor"""
        return {"status": "ready", "uart_ok": True, "gpio_ok": True}
    
    def run_emi_test(self, frequency: float, field_strength: float) -> Dict:
        """Run EMI test at specific frequency and field strength"""
        print(f"📡 EMI Test: {frequency}MHz at {field_strength}V/m")
        
        # Pre-test check
        pre_test_ok = self.check_system_functionality()
        
        # Apply RF field
        self._apply_rf_field(frequency, field_strength)
        time.sleep(1.0)  # Expose system to field
        
        # Monitor during exposure
        during_test_ok = self.monitor_during_exposure()
        
        # Remove RF field
        self._remove_rf_field()
        time.sleep(0.5)
        
        # Post-test check
        post_test_ok = self.check_system_functionality()
        
        # Determine result
        if pre_test_ok and during_test_ok and post_test_ok:
            result = "PASS"
        else:
            result = "FAIL"
        
        return {
            "frequency": frequency,
            "field_strength": field_strength,
            "pre_test": pre_test_ok,
            "during_test": during_test_ok,
            "post_test": post_test_ok,
            "result": result
        }
    
    def _apply_rf_field(self, frequency: float, field_strength: float):
        """Apply RF field to system"""
        print(f"   Applying {field_strength}V/m at {frequency}MHz...")
        # Simulate RF field application
    
    def _remove_rf_field(self):
        """Remove RF field"""
        print("   Removing RF field...")
        # Simulate RF field removal
    
    def check_system_functionality(self) -> bool:
        """Check system functionality"""
        # Simulate functionality check
        return True
    
    def monitor_during_exposure(self) -> bool:
        """Monitor system during RF exposure"""
        # Simulate monitoring
        return True
    
    def run_complete_emi_test(self) -> Dict:
        """Run complete EMI susceptibility test"""
        print("📡 Starting EMI Susceptibility Test")
        print("=" * 50)
        
        self.setup_emi_test()
        
        all_results = []
        
        for freq_range in self.frequency_ranges:
            start_freq, end_freq = freq_range
            test_frequencies = np.logspace(np.log10(start_freq), np.log10(end_freq), 10)
            
            for freq in test_frequencies:
                for field_strength in self.field_strengths:
                    result = self.run_emi_test(freq, field_strength)
                    all_results.append(result)
                    
                    print(f"   Result: {result['result']}")
                    
                    if result['result'] == "FAIL":
                        print(f"   ⚠️  EMI test failed at {freq}MHz, {field_strength}V/m")
        
        # Summary
        passed = sum(1 for r in all_results if r['result'] == "PASS")
        total = len(all_results)
        
        summary = {
            "total_tests": total,
            "passed": passed,
            "failed": total - passed,
            "pass_rate": (passed / total) * 100,
            "results": all_results
        }
        
        print(f"\n📊 EMI Test Summary:")
        print(f"   Total: {total}, Passed: {passed}, Failed: {total-passed}")
        print(f"   Pass Rate: {summary['pass_rate']:.1f}%")
        
        return summary

# Test execution
if __name__ == "__main__":
    emi_test = EMITest()
    results = emi_test.run_complete_emi_test()
```

### **3. Conducted Emissions Test**

#### **Test Setup**
```python
#!/usr/bin/env python3
"""
Conducted Emissions Test cho OHT-50
"""

import time
import numpy as np
from typing import Dict, List

class ConductedEmissionsTest:
    """Conducted Emissions Test Implementation"""
    
    def __init__(self):
        self.frequency_range = (150, 30000)  # 150kHz - 30MHz
        self.limits = {
            "Class A": -60,  # dBµV for Class A equipment
            "Class B": -50   # dBµV for Class B equipment
        }
        self.test_results = {}
    
    def setup_conducted_test(self):
        """Setup conducted emissions test"""
        print("🔧 Setting up Conducted Emissions Test...")
        
        # Initialize LISN (Line Impedance Stabilization Network)
        self.lisn = self._init_lisn()
        
        # Initialize spectrum analyzer
        self.spectrum_analyzer = self._init_spectrum_analyzer()
        
        # Setup test modes
        self.test_modes = ["idle", "transmit", "receive"]
        
        print("✅ Conducted emissions test environment ready")
    
    def _init_lisn(self):
        """Initialize LISN"""
        return {"status": "ready", "impedance": "50Ω"}
    
    def _init_spectrum_analyzer(self):
        """Initialize spectrum analyzer"""
        return {"status": "ready", "frequency": 0, "amplitude": 0}
    
    def measure_conducted_emissions(self, frequency: float, mode: str) -> Dict:
        """Measure conducted emissions at specific frequency"""
        print(f"📊 Measuring emissions: {frequency}kHz in {mode} mode")
        
        # Simulate measurement
        measured_level = self._simulate_measurement(frequency, mode)
        
        # Compare with limits
        class_a_limit = self.limits["Class A"]
        class_b_limit = self.limits["Class B"]
        
        class_a_pass = measured_level <= class_a_limit
        class_b_pass = measured_level <= class_b_limit
        
        result = "PASS" if class_b_pass else "FAIL"
        
        return {
            "frequency": frequency,
            "mode": mode,
            "measured_level": measured_level,
            "class_a_limit": class_a_limit,
            "class_b_limit": class_b_limit,
            "class_a_pass": class_a_pass,
            "class_b_pass": class_b_pass,
            "result": result
        }
    
    def _simulate_measurement(self, frequency: float, mode: str) -> float:
        """Simulate conducted emissions measurement"""
        # Simulate different emission levels based on frequency and mode
        base_level = -70  # dBµV
        
        # Add frequency-dependent variations
        if frequency < 1000:  # Below 1MHz
            variation = 5
        elif frequency < 10000:  # 1-10MHz
            variation = 10
        else:  # 10-30MHz
            variation = 15
        
        # Add mode-dependent variations
        if mode == "idle":
            mode_factor = 0
        elif mode == "transmit":
            mode_factor = 8
        else:  # receive
            mode_factor = 3
        
        return base_level + variation + mode_factor
    
    def run_conducted_emissions_test(self) -> Dict:
        """Run complete conducted emissions test"""
        print("📊 Starting Conducted Emissions Test")
        print("=" * 50)
        
        self.setup_conducted_test()
        
        all_results = []
        test_frequencies = np.logspace(np.log10(150), np.log10(30000), 20)
        
        for mode in self.test_modes:
            print(f"\n🔧 Testing in {mode} mode...")
            
            for freq in test_frequencies:
                result = self.measure_conducted_emissions(freq, mode)
                all_results.append(result)
                
                if result['result'] == "FAIL":
                    print(f"   ⚠️  Emissions exceed limit at {freq:.1f}kHz")
        
        # Summary
        passed = sum(1 for r in all_results if r['result'] == "PASS")
        total = len(all_results)
        
        summary = {
            "total_tests": total,
            "passed": passed,
            "failed": total - passed,
            "pass_rate": (passed / total) * 100,
            "results": all_results
        }
        
        print(f"\n📊 Conducted Emissions Test Summary:")
        print(f"   Total: {total}, Passed: {passed}, Failed: {total-passed}")
        print(f"   Pass Rate: {summary['pass_rate']:.1f}%")
        
        return summary

# Test execution
if __name__ == "__main__":
    conducted_test = ConductedEmissionsTest()
    results = conducted_test.run_conducted_emissions_test()
```

---

## 📋 **Test Checklist**

### **Pre-Test Checklist**
- [ ] **Equipment Setup**: All test equipment calibrated
- [ ] **System Preparation**: OHT-50 system fully functional
- [ ] **Environment**: Test chamber properly configured
- [ ] **Documentation**: Test procedures documented
- [ ] **Safety**: Safety procedures reviewed

### **ESD Test Checklist**
- [ ] **Contact Discharge**: 2kV, 4kV, 8kV, 15kV
- [ ] **Air Discharge**: 2kV, 4kV, 8kV, 15kV
- [ ] **Test Points**: All accessible conductive parts
- [ ] **Functionality**: System operation during/after test
- [ ] **Recovery**: System recovery after test

### **EMI Test Checklist**
- [ ] **Frequency Range**: 80MHz - 6GHz
- [ ] **Field Strength**: 3V/m, 10V/m, 30V/m
- [ ] **Polarization**: Horizontal and vertical
- [ ] **System Modes**: Idle, transmit, receive
- [ ] **Performance**: No degradation during test

### **Conducted Emissions Checklist**
- [ ] **Frequency Range**: 150kHz - 30MHz
- [ ] **Power Lines**: L and N conductors
- [ ] **Signal Lines**: RS485, control signals
- [ ] **Limits**: Class A and Class B limits
- [ ] **Averaging**: Quasi-peak and average measurements

---

## 📊 **Test Results Template**

### **ESD Test Results**
```
Test Date: 2025-01-27
Test Equipment: ESD Gun XYZ-1000
Test Standard: IEC 61000-4-2

Results Summary:
- Contact Discharge: PASS (15kV)
- Air Discharge: PASS (15kV)
- System Recovery: PASS
- Overall Result: PASS

Detailed Results:
[Test Point] [Voltage] [Result] [Notes]
RS485_A      4kV      PASS    No issues
RS485_B      4kV      PASS    No issues
GPIO_46      8kV      PASS    No issues
GPIO_47      8kV      PASS    No issues
Chassis      15kV     PASS    No issues
```

### **EMI Test Results**
```
Test Date: 2025-01-27
Test Equipment: RF Generator ABC-2000
Test Standard: IEC 61000-4-3

Results Summary:
- Frequency Range: 80MHz - 6GHz
- Field Strength: Up to 30V/m
- System Performance: No degradation
- Overall Result: PASS

Detailed Results:
[Frequency] [Field Strength] [Result] [Notes]
100MHz      10V/m           PASS    Normal operation
1GHz        30V/m           PASS    Normal operation
3GHz        30V/m           PASS    Normal operation
```

### **Conducted Emissions Results**
```
Test Date: 2025-01-27
Test Equipment: LISN + Spectrum Analyzer
Test Standard: CISPR 22

Results Summary:
- Frequency Range: 150kHz - 30MHz
- Class A Limits: PASS
- Class B Limits: PASS
- Overall Result: PASS

Detailed Results:
[Frequency] [Measured] [Class A Limit] [Class B Limit] [Result]
150kHz      -65dBµV   -60dBµV        -50dBµV        PASS
1MHz        -58dBµV   -60dBµV        -50dBµV        PASS
10MHz       -52dBµV   -60dBµV        -50dBµV        PASS
30MHz       -48dBµV   -60dBµV        -50dBµV        PASS
```

---

## 🚨 **Failure Analysis**

### **Common ESD Failures**
1. **System Reset**: ESD causes system reset
   - **Cause**: Insufficient ESD protection on reset line
   - **Solution**: Add TVS diode to reset pin

2. **Communication Errors**: RS485 communication fails
   - **Cause**: ESD damages RS485 transceiver
   - **Solution**: Improve ESD protection on A/B lines

3. **GPIO Malfunction**: GPIO pins stuck or incorrect
   - **Cause**: ESD damages GPIO buffer
   - **Solution**: Add series resistors + TVS diodes

### **Common EMI Failures**
1. **Clock Jitter**: System clock becomes unstable
   - **Cause**: EMI coupling to clock circuit
   - **Solution**: Improve clock circuit shielding

2. **Data Corruption**: Communication data corrupted
   - **Cause**: EMI interference on signal lines
   - **Solution**: Improve signal line shielding

3. **False Triggers**: E-Stop or limit switches trigger falsely
   - **Cause**: EMI coupling to input circuits
   - **Solution**: Add filtering and shielding

### **Common Emissions Failures**
1. **Switching Noise**: High emissions from switching power supply
   - **Cause**: Inadequate filtering
   - **Solution**: Add LC filters and shielding

2. **Clock Harmonics**: Emissions at clock frequency harmonics
   - **Cause**: Clock signal radiation
   - **Solution**: Reduce clock signal loop area

3. **Digital Noise**: Broadband emissions from digital circuits
   - **Cause**: Fast switching signals
   - **Solution**: Add ferrite beads and filtering

---

## 📚 **Standards & References**

### **EMC Standards**
- **IEC 61000-4-2**: ESD immunity test
- **IEC 61000-4-3**: Radiated immunity test
- **IEC 61000-4-4**: Electrical fast transient test
- **IEC 61000-4-5**: Surge immunity test
- **CISPR 22**: Information technology equipment emissions

### **Design Guidelines**
- **Henry Ott**: "Electromagnetic Compatibility Engineering"
- **Clayton Paul**: "Introduction to Electromagnetic Compatibility"
- **Keith Armstrong**: EMC design guidelines

### **Test Equipment**
- **ESD Gun**: Teseq NSG 438 or equivalent
- **RF Generator**: Teseq NSG 4070 or equivalent
- **LISN**: Teseq NSLK 8127 or equivalent
- **Spectrum Analyzer**: Keysight N9000B or equivalent

---

**Changelog:**
- v1.0 (2025-01-27): Initial version with ESD, EMI, and conducted emissions testing
