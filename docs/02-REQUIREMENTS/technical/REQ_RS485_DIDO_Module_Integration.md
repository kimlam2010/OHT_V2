# **DI/DO MODULE INTEGRATION GUIDE**

## 📋 **TÀI LIỆU TÍCH HỢP MODULE DI/DO RS485**

**Module Type:** Digital Input/Output Module  
**Address:** 0x06  
**Version:** 1.0  
**Status:** 🔄 Đang phát triển

---

## 🎯 **MỤC TIÊU**

### **Mục tiêu:**
- Kết nối Master Module (Orange Pi 5B) với DI/DO Module qua RS485
- Điều khiển 8 kênh Digital Input và 8 kênh Digital Output
- Giám sát trạng thái input/output real-time
- Cung cấp safety features và protection
- Hỗ trợ opto-isolation và overcurrent protection

### **Kiến trúc hệ thống:**
```
┌─────────────────┐    RS485    ┌─────────────────┐
│   Master Module │ ──────────── │  DI/DO Module  │
│  (Orange Pi 5B) │             │   (Controller)  │
│                 │             │                 │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ HAL RS485   │ │             │ │ DI/DO       │ │
│ │ Driver      │ │             │ │ Controller  │ │
│ └─────────────┘ │             │ └─────────────┘ │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ DI/DO Mgr   │ │             │ │ 8x Digital  │ │
│ │ Application │ │             │ │ Inputs      │ │
│ └─────────────┘ │             │ └─────────────┘ │
└─────────────────┘             │ ┌─────────────┐ │
                                │ │ 8x Digital  │ │
                                │ │ Outputs     │ │
                                └─────────────────┘
```

---

## 🔧 **HARDWARE SPECIFICATIONS**

### **1. Module Specifications:**
- **Module Type:** Digital Input/Output Module
- **Address:** 0x06 (RS485)
- **Power Supply:** 24V DC
- **Current Consumption:** < 100mA
- **Operating Temperature:** -40°C to +85°C
- **Protection:** IP20

### **2. Digital Input Specifications:**
- **Channels:** 8 channels
- **Voltage Level:** 24V logic
- **Isolation:** Opto-isolated (2500V)
- **Input Impedance:** 3.3kΩ
- **Response Time:** < 1ms
- **Status LED:** Per channel

### **3. Digital Output Specifications:**
- **Channels:** 8 channels
- **Voltage Level:** 24V
- **Current Rating:** 2A per channel
- **Total Current:** 8A maximum
- **Protection:** Overcurrent, short-circuit
- **Status LED:** Per channel

### **4. Safety Features:**
- **Overcurrent Protection:** Automatic shutdown
- **Short-circuit Detection:** Real-time monitoring
- **Temperature Monitoring:** Overheat protection
- **Fault Reporting:** Comprehensive error codes
- **Emergency Stop:** Hardware E-Stop support

---

## 🔌 **HARDWARE CONNECTION**

### **1. Power Connection:**
```
┌─────────────────────────────────────┐
│         DI/DO Module                │
├─────────────────────────────────────┤
│ 24V+ ──────────────────────────────┼─── Power Supply 24V+
│ 24V- ──────────────────────────────┼─── Power Supply 24V-
│ GND  ──────────────────────────────┼─── Common Ground
└─────────────────────────────────────┘
```

### **2. RS485 Communication:**
```
┌─────────────────────────────────────┐
│         Orange Pi 5B                │
├─────────────────────────────────────┤
│ UART1_TX: GPIO1_A0 (Pin 8) ────────┼─── MAX485 DI
│ UART1_RX: GPIO1_A1 (Pin 10) ───────┼─── MAX485 RO  
│ GND: Common Ground ─────────────────┼─── MAX485 GND
└─────────────────────────────────────┘
```

### **3. Digital Input Connections:**
```
┌─────────────────────────────────────┐
│         DI/DO Module                │
├─────────────────────────────────────┤
│ DI1 ───────────────────────────────┼─── External Device 1
│ DI2 ───────────────────────────────┼─── External Device 2
│ DI3 ───────────────────────────────┼─── External Device 3
│ DI4 ───────────────────────────────┼─── External Device 4
│ DI5 ───────────────────────────────┼─── External Device 5
│ DI6 ───────────────────────────────┼─── External Device 6
│ DI7 ───────────────────────────────┼─── External Device 7
│ DI8 ───────────────────────────────┼─── External Device 8
└─────────────────────────────────────┘
```

### **4. Digital Output Connections:**
```
┌─────────────────────────────────────┐
│         DI/DO Module                │
├─────────────────────────────────────┤
│ DO1 ───────────────────────────────┼─── Load 1 (24V, 2A max)
│ DO2 ───────────────────────────────┼─── Load 2 (24V, 2A max)
│ DO3 ───────────────────────────────┼─── Load 3 (24V, 2A max)
│ DO4 ───────────────────────────────┼─── Load 4 (24V, 2A max)
│ DO5 ───────────────────────────────┼─── Load 5 (24V, 2A max)
│ DO6 ───────────────────────────────┼─── Load 6 (24V, 2A max)
│ DO7 ───────────────────────────────┼─── Load 7 (24V, 2A max)
│ DO8 ───────────────────────────────┼─── Load 8 (24V, 2A max)
└─────────────────────────────────────┘
```

---

## ⚙️ **SOFTWARE CONFIGURATION**

### **1. Device Tree Overlay:**
```dts
// /boot/armbianEnv.txt
overlays=uart1

// /boot/dtb/overlay/rockchip-uart1.dts
/dts-v1/;
/plugin/;

&uart1 {
    status = "okay";
    pinctrl-names = "default";
    pinctrl-0 = <&uart1m0_xfer>;
};
```

### **2. UART1 Configuration:**
```
# UART1 Configuration
UART1_TX: GPIO1_A0 (Pin 8)
UART1_RX: GPIO1_A1 (Pin 10)

# udev rule để tạo symlink
echo 'KERNEL=="ttyS1", SYMLINK+="ttyOHT485"' > /etc/udev/rules.d/99-oht-rs485.rules
```

### **3. DI/DO Module Configuration:**
```json
{
  "dido_module": {
    "address": "0x06",
    "name": "DI/DO Module",
    "type": "digital_io",
    "version": "1.0",
    "enabled": true,
    "digital_input_config": {
      "enabled": true,
      "channels": [
        {"id": 1, "enabled": true, "function": "emergency_stop"},
        {"id": 2, "enabled": true, "function": "manual_override"},
        {"id": 3, "enabled": true, "function": "safety_door"},
        {"id": 4, "enabled": true, "function": "load_detection"},
        {"id": 5, "enabled": true, "function": "position_sensor"},
        {"id": 6, "enabled": true, "function": "limit_switch"},
        {"id": 7, "enabled": true, "function": "manual_control"},
        {"id": 8, "enabled": true, "function": "system_ready"}
      ],
      "voltage_level": 24,
      "isolation": true
    },
    "digital_output_config": {
      "enabled": true,
      "channels": [
        {"id": 1, "enabled": true, "function": "motor_enable", "current_limit": 2000},
        {"id": 2, "enabled": true, "function": "brake_release", "current_limit": 2000},
        {"id": 3, "enabled": true, "function": "indicator_light", "current_limit": 500},
        {"id": 4, "enabled": true, "function": "alarm_system", "current_limit": 1000},
        {"id": 5, "enabled": true, "function": "conveyor_control", "current_limit": 2000},
        {"id": 6, "enabled": true, "function": "pump_control", "current_limit": 2000},
        {"id": 7, "enabled": true, "function": "valve_control", "current_limit": 2000},
        {"id": 8, "enabled": true, "function": "auxiliary_device", "current_limit": 2000}
      ],
      "voltage_level": 24,
      "overcurrent_protection": true
    },
    "safety_config": {
      "emergency_stop_enabled": true,
      "overcurrent_protection": true,
      "short_circuit_detection": true,
      "temperature_monitoring": true,
      "fault_reporting": true
    },
    "system_config": {
      "baudrate": 3,
      "parity": 0,
      "device_id": 6
    }
  }
}
```

---

## 💻 **IMPLEMENTATION SOFTWARE**

### **1. HAL RS485 Driver (`hal_rs485.c`)**

#### **Core Functions:**
```c
// DI/DO Module specific functions
hal_status_t dido_module_init(void);
hal_status_t dido_module_read_inputs(uint8_t *input_states);
hal_status_t dido_module_read_input(uint8_t channel, bool *state);
hal_status_t dido_module_set_output(uint8_t channel, bool state);
hal_status_t dido_module_get_output(uint8_t channel, bool *state);
hal_status_t dido_module_set_outputs(uint8_t *output_states);
hal_status_t dido_module_read_status(dido_status_t *status);
hal_status_t dido_module_read_faults(dido_fault_t *faults);
hal_status_t dido_module_emergency_stop(void);
hal_status_t dido_module_clear_faults(void);
```

### **2. DI/DO Manager Application (`dido_manager.c`)**

#### **Core Functions:**
```c
// DI/DO management functions
dido_status_t dido_manager_get_status(void);
hal_status_t dido_manager_monitor_inputs(void);
hal_status_t dido_manager_handle_emergency_stop(void);
hal_status_t dido_manager_control_outputs(void);
hal_status_t dido_manager_handle_faults(void);
hal_status_t dido_manager_save_config(void);
hal_status_t dido_manager_load_config(void);
hal_status_t dido_manager_calibrate(void);
```

---

## 📡 **MODBUS REGISTER MAP**

### **1. Digital Input Registers (0x0000-0x00FF):**

#### **Input Status (0x0000-0x000F):**
```c
// Digital input status (8 channels)
#define DIDO_REG_INPUT_STATUS            0x0000  // Bit field

// Individual input states
#define DIDO_REG_INPUT1_STATE            0x0001  // 0=Low, 1=High

#define DIDO_REG_INPUT2_STATE            0x0002  // 0=Low, 1=High

#define DIDO_REG_INPUT3_STATE            0x0003  // 0=Low, 1=High

#define DIDO_REG_INPUT4_STATE            0x0004  // 0=Low, 1=High

#define DIDO_REG_INPUT5_STATE            0x0005  // 0=Low, 1=High

#define DIDO_REG_INPUT6_STATE            0x0006  // 0=Low, 1=High

#define DIDO_REG_INPUT7_STATE            0x0007  // 0=Low, 1=High

#define DIDO_REG_INPUT8_STATE            0x0008  // 0=Low, 1=High

// Input configuration
#define DIDO_REG_INPUT_CONFIG            0x0009  // Bit field

// Input debounce time (ms)
#define DIDO_REG_INPUT_DEBOUNCE          0x000A  // 16-bit unsigned
```

### **2. Digital Output Registers (0x0100-0x01FF):**

#### **Output Control (0x0100-0x010F):**
```c
// Digital output status (8 channels)
#define DIDO_REG_OUTPUT_STATUS           0x0100  // Bit field

// Individual output states
#define DIDO_REG_OUTPUT1_STATE           0x0101  // 0=Off, 1=On

#define DIDO_REG_OUTPUT2_STATE           0x0102  // 0=Off, 1=On

#define DIDO_REG_OUTPUT3_STATE           0x0103  // 0=Off, 1=On

#define DIDO_REG_OUTPUT4_STATE           0x0104  // 0=Off, 1=On

#define DIDO_REG_OUTPUT5_STATE           0x0105  // 0=Off, 1=On

#define DIDO_REG_OUTPUT6_STATE           0x0106  // 0=Off, 1=On

#define DIDO_REG_OUTPUT7_STATE           0x0107  // 0=Off, 1=On

#define DIDO_REG_OUTPUT8_STATE           0x0108  // 0=Off, 1=On

// Output configuration
#define DIDO_REG_OUTPUT_CONFIG           0x0109  // Bit field

// Output current limits (mA)
#define DIDO_REG_OUTPUT1_CURRENT_LIMIT   0x010A  // 16-bit unsigned

#define DIDO_REG_OUTPUT2_CURRENT_LIMIT   0x010B  // 16-bit unsigned

#define DIDO_REG_OUTPUT3_CURRENT_LIMIT   0x010C  // 16-bit unsigned

#define DIDO_REG_OUTPUT4_CURRENT_LIMIT   0x010D  // 16-bit unsigned

#define DIDO_REG_OUTPUT5_CURRENT_LIMIT   0x010E  // 16-bit unsigned

#define DIDO_REG_OUTPUT6_CURRENT_LIMIT   0x010F  // 16-bit unsigned

#define DIDO_REG_OUTPUT7_CURRENT_LIMIT   0x0110  // 16-bit unsigned

#define DIDO_REG_OUTPUT8_CURRENT_LIMIT   0x0111  // 16-bit unsigned
```

### **3. Current Monitoring Registers (0x0200-0x02FF):**

#### **Current Status (0x0200-0x020F):**
```c
// Output current monitoring (mA)
#define DIDO_REG_OUTPUT1_CURRENT         0x0200  // 16-bit unsigned

#define DIDO_REG_OUTPUT2_CURRENT         0x0201  // 16-bit unsigned

#define DIDO_REG_OUTPUT3_CURRENT         0x0202  // 16-bit unsigned

#define DIDO_REG_OUTPUT4_CURRENT         0x0203  // 16-bit unsigned

#define DIDO_REG_OUTPUT5_CURRENT         0x0204  // 16-bit unsigned

#define DIDO_REG_OUTPUT6_CURRENT         0x0205  // 16-bit unsigned

#define DIDO_REG_OUTPUT7_CURRENT         0x0206  // 16-bit unsigned

#define DIDO_REG_OUTPUT8_CURRENT         0x0207  // 16-bit unsigned

// Total current consumption (mA)
#define DIDO_REG_TOTAL_CURRENT           0x0208  // 16-bit unsigned

// Current limit status
#define DIDO_REG_CURRENT_LIMIT_STATUS    0x0209  // Bit field
```

### **4. Safety Status Registers (0x0300-0x03FF):**

#### **Safety Status (0x0300-0x030F):**
```c
// Safety status register
#define DIDO_REG_SAFETY_STATUS           0x0300  // Bit field

// Emergency stop status
#define DIDO_REG_EMERGENCY_STOP_STATUS   0x0301  // 0=Normal, 1=Emergency

// Overcurrent protection status
#define DIDO_REG_OVERCURRENT_STATUS      0x0302  // Bit field

// Short circuit detection status
#define DIDO_REG_SHORT_CIRCUIT_STATUS    0x0303  // Bit field

// Temperature status (°C)
#define DIDO_REG_TEMPERATURE             0x0304  // 16-bit signed

// Module health status
#define DIDO_REG_HEALTH_STATUS           0x0305  // Bit field

// Fault counter
#define DIDO_REG_FAULT_COUNTER           0x0306  // 16-bit unsigned
```

### **5. Fault Status Registers (0x0400-0x04FF):**

#### **Fault Status (0x0400-0x040F):**
```c
// Fault status register
#define DIDO_REG_FAULT_STATUS            0x0400  // Bit field

// Input fault status
#define DIDO_REG_INPUT_FAULT_STATUS      0x0401  // Bit field

// Output fault status
#define DIDO_REG_OUTPUT_FAULT_STATUS     0x0402  // Bit field

// Communication fault
#define DIDO_REG_COMM_FAULT              0x0403  // 0=OK, 1=Fault

// Power fault
#define DIDO_REG_POWER_FAULT             0x0404  // 0=OK, 1=Fault

// Temperature fault
#define DIDO_REG_TEMP_FAULT              0x0405  // 0=OK, 1=Fault

// Overcurrent fault
#define DIDO_REG_OVERCURRENT_FAULT       0x0406  // 0=OK, 1=Fault

// Short circuit fault
#define DIDO_REG_SHORT_CIRCUIT_FAULT     0x0407  // 0=OK, 1=Fault
```

#### **System Registers (0x00F0-0x00FF):**
```c
// Device ID (Modbus slave address)
#define DIDO_REG_DEVICE_ID               0x00F0  // 16-bit unsigned

// Firmware version (e.g. 0x0101 = v1.01)
#define DIDO_REG_FIRMWARE_VERSION        0x00F1  // 16-bit unsigned

// System status (bit field)
#define DIDO_REG_SYSTEM_STATUS           0x00F2  // 16-bit unsigned

// System error (global error code)
#define DIDO_REG_SYSTEM_ERROR            0x00F3  // 16-bit unsigned

// Reset error command (write 1 to reset all error flags)
#define DIDO_REG_RESET_ERROR_CMD         0x00F4  // 16-bit unsigned

// Config baudrate (1=9600, 2=19200, 3=38400,...)
#define DIDO_REG_CONFIG_BAUDRATE         0x00F5  // 16-bit unsigned

// Config parity (0=None, 1=Even, 2=Odd)
#define DIDO_REG_CONFIG_PARITY           0x00F6  // 16-bit unsigned

// Module type (0x0006 = DI/DO Module)
#define DIDO_REG_MODULE_TYPE             0x00F7  // 16-bit unsigned

// Module name (low word)
#define DIDO_REG_MODULE_NAME_LOW         0x00F8  // 16-bit unsigned

// Module name (high word)
#define DIDO_REG_MODULE_NAME_HIGH        0x00F9  // 16-bit unsigned

// Hardware version
#define DIDO_REG_HARDWARE_VERSION        0x00FA  // 16-bit unsigned

// Serial number (low word)
#define DIDO_REG_SERIAL_NUMBER_LOW       0x00FB  // 16-bit unsigned

// Serial number (high word)
#define DIDO_REG_SERIAL_NUMBER_HIGH      0x00FC  // 16-bit unsigned

// Build date (low word)
#define DIDO_REG_BUILD_DATE_LOW          0x00FD  // 16-bit unsigned

// Build date (high word)
#define DIDO_REG_BUILD_DATE_HIGH         0x00FE  // 16-bit unsigned

// Register checksum
#define DIDO_REG_CHECKSUM                0x00FF  // 16-bit unsigned
```

---

## 🧪 **TESTING PROCEDURES**

### **1. Basic Communication Test:**
```python
#!/usr/bin/env python3
import serial
import time

def test_dido_communication():
    # Open serial port
    ser = serial.Serial(
        port='/dev/ttyS1',
        baudrate=115200,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=1
    )

    # Test communication with DI/DO module
    module_addr = 0x06
    print(f"Testing DI/DO module at address 0x{module_addr:02X}")

    # Read input status
    command = bytes([module_addr, 0x03, 0x00, 0x00, 0x00, 0x01])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')

    ser.write(command)
    response = ser.read(7)

    if len(response) == 7:
        print("  ✓ DI/DO module responded")
        print(f"  Input status: 0x{response[3]:02X}")
    else:
        print("  ✗ DI/DO module no response")

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
    test_dido_communication()
```

### **2. Digital Input Test:**
```python
def test_digital_inputs():
    ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
    
    # Read all input states
    command = bytes([0x06, 0x03, 0x00, 0x00, 0x00, 0x0A])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(25)  # Expected response length
    
    if len(response) == 25:
        print("Digital Input States:")
        for i in range(8):
            state = (response[3] >> i) & 0x01
            print(f"  DI{i+1}: {'HIGH' if state else 'LOW'}")
    
    ser.close()
```

### **3. Digital Output Test:**
```python
def test_digital_outputs():
    ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
    
    # Set output 1 to HIGH
    command = bytes([0x06, 0x06, 0x01, 0x01, 0x00, 0x01])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(8)
    
    if len(response) == 8:
        print("  ✓ Output 1 set to HIGH")
    
    time.sleep(1)
    
    # Set output 1 to LOW
    command = bytes([0x06, 0x06, 0x01, 0x01, 0x00, 0x00])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(8)
    
    if len(response) == 8:
        print("  ✓ Output 1 set to LOW")
    
    ser.close()
```

### **4. Safety Test:**
```python
def test_safety_features():
    ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
    
    # Read safety status
    command = bytes([0x06, 0x03, 0x03, 0x00, 0x00, 0x08])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(21)
    
    if len(response) == 21:
        emergency_stop = response[3]
        overcurrent = response[5]
        temperature = int.from_bytes(response[7:9], byteorder='big', signed=True)
        
        print(f"Emergency Stop: {'ACTIVE' if emergency_stop else 'NORMAL'}")
        print(f"Overcurrent Protection: {'ACTIVE' if overcurrent else 'NORMAL'}")
        print(f"Temperature: {temperature}°C")
    
    ser.close()
```

---

## 📊 **PERFORMANCE METRICS**

### **1. Key Performance Indicators:**
- **Input Response Time:** < 1ms
- **Output Response Time:** < 5ms
- **Current Accuracy:** ±2% full scale
- **Temperature Range:** -40°C to +85°C
- **Isolation Voltage:** 2500V
- **Update Rate:** 100Hz for inputs, 10Hz for outputs
- **Fault Detection Time:** < 10ms

### **2. Monitoring Metrics:**
- **Input States:** Real-time digital input monitoring
- **Output States:** Real-time digital output monitoring
- **Current Monitoring:** Real-time current consumption
- **Safety Status:** Real-time safety monitoring
- **Fault Detection:** Real-time fault monitoring
- **Communication:** RS485 communication status
- **Temperature:** Module temperature monitoring

---

## 🔧 **TROUBLESHOOTING**

### **1. Common Issues:**

#### **Communication Issues:**
- **Problem:** Module not responding
- **Solution:** Check RS485 wiring, baudrate, address
- **Diagnostic:** Use communication test script

#### **Input Issues:**
- **Problem:** Input not detecting signal
- **Solution:** Check voltage level, wiring, opto-isolation
- **Diagnostic:** Measure input voltage, check LED status

#### **Output Issues:**
- **Problem:** Output not switching
- **Solution:** Check load, current limit, protection
- **Diagnostic:** Measure output voltage, check current

#### **Safety Issues:**
- **Problem:** Emergency stop not working
- **Solution:** Check input wiring, configuration
- **Diagnostic:** Test emergency stop input

### **2. Error Codes:**
```c
// Error code definitions
#define DIDO_ERROR_NONE                  0x0000
#define DIDO_ERROR_COMMUNICATION         0x0001
#define DIDO_ERROR_OVERCURRENT           0x0002
#define DIDO_ERROR_SHORT_CIRCUIT         0x0003
#define DIDO_ERROR_TEMPERATURE           0x0004
#define DIDO_ERROR_POWER                 0x0005
#define DIDO_ERROR_CONFIGURATION         0x0006
#define DIDO_ERROR_HARDWARE              0x0007
```

---

## 🚀 **INTEGRATION VỚI SYSTEM**

### **1. System State Machine Integration:**

#### **DI/DO States:**
```c
typedef enum {
    DIDO_STATE_IDLE,
    DIDO_STATE_ACTIVE,
    DIDO_STATE_EMERGENCY,
    DIDO_STATE_FAULT,
    DIDO_STATE_MAINTENANCE
} dido_state_t;
```

#### **State Transitions:**
```c
// DI/DO state machine integration
hal_status_t dido_manager_update_state(void) {
    dido_status_t status;
    hal_status_t result = dido_module_read_status(&status);
    
    if (result == HAL_STATUS_OK) {
        if (status.emergency_stop) {
            g_dido_manager.state = DIDO_STATE_EMERGENCY;
        } else if (status.fault_code != 0) {
            g_dido_manager.state = DIDO_STATE_FAULT;
        } else if (status.active) {
            g_dido_manager.state = DIDO_STATE_ACTIVE;
        } else {
            g_dido_manager.state = DIDO_STATE_IDLE;
        }
    }
    
    return HAL_STATUS_OK;
}
```

### **2. Safety Integration:**

#### **Emergency Stop Handling:**
```c
// Emergency stop integration
hal_status_t dido_manager_handle_emergency_stop(void) {
    bool emergency_stop;
    hal_status_t result = dido_module_read_input(1, &emergency_stop);
    
    if (result == HAL_STATUS_OK && emergency_stop) {
        // Trigger system emergency stop
        system_manager_emergency_stop();
        
        // Disable all outputs
        uint8_t outputs[8] = {0};
        dido_module_set_outputs(outputs);
        
        return HAL_STATUS_OK;
    }
    
    return HAL_STATUS_ERROR;
}
```

---

## 📋 **MAINTENANCE**

### **1. Regular Maintenance:**
- **Monthly:** Check wiring connections
- **Quarterly:** Test emergency stop functionality
- **Annually:** Calibrate current monitoring
- **As needed:** Replace faulty components

### **2. Calibration:**
```c
// Current monitoring calibration
hal_status_t dido_module_calibrate_current(void) {
    // Zero current calibration
    dido_module_set_outputs(0);
    delay(1000);
    
    // Measure zero current
    uint16_t zero_current[8];
    for (int i = 0; i < 8; i++) {
        dido_module_read_output_current(i, &zero_current[i]);
    }
    
    // Store calibration data
    dido_module_save_calibration(zero_current);
    
    return HAL_STATUS_OK;
}
```

---

**Changelog:**
- v1.0 (2025-01-27): Initial DI/DO Module integration guide
- v1.1 (2025-01-27): Added System Registers (0x00F0-0x00FF) for auto-detect compatibility
