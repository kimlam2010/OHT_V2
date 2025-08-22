---
title: "06 Rs485 Analog Input Module Integration"
version: "v1.0.0"
date_created: "2025-08-22"
date_modified: "2025-08-22"
author: "QA Team"
reviewer: "PM Team"
approver: "CTO"
status: "Approved"
classification: "Internal"
security_level: "Internal"
tags: ['testing-validation', 'rs485', 'modbus', 'communication']
related_docs: []
compliance: ['ISO 9001:2015', 'IEEE 12207']
---

# **ANALOG INPUT MODULE INTEGRATION GUIDE**

## 📋 **TÀI LIỆU TÍCH HỢP MODULE ANALOG INPUT RS485**

**Module Type:** Analog Input Module  
**Address:** 0x07  
**Version:** 1.0  
**Status:** 🔄 Đang phát triển

---

## 🎯 **MỤC TIÊU**

### **Mục tiêu:**
- Kết nối Master Module (Orange Pi 5B) với Analog Input Module qua RS485
- Thu thập dữ liệu từ cảm biến khoảng cách và các cảm biến analog khác
- Cung cấp high-precision analog signal processing
- Hỗ trợ multiple analog input channels
- Cung cấp real-time analog data acquisition

### **Kiến trúc hệ thống:**
```
┌─────────────────┐    RS485    ┌─────────────────┐
│   Master Module │ ──────────── │ Analog Input   │
│  (Orange Pi 5B) │             │   Module       │
│                 │             │   (Controller)  │
│ ┌─────────────┐ │             │                 │
│ │ HAL RS485   │ │             │ ┌─────────────┐ │
│ │ Driver      │ │             │ │ Analog      │ │
│ └─────────────┘ │             │ │ Controller  │ │
│ ┌─────────────┐ │             │ └─────────────┘ │
│ │ Analog Mgr  │ │             │ ┌─────────────┐ │
│ │ Application │ │             │ │ 16x Analog  │ │
│ └─────────────┘ │             │ │ Inputs      │ │
└─────────────────┘             └─────────────────┘
```

---

## 🔧 **HARDWARE SPECIFICATIONS**

### **1. Module Specifications:**
- **Module Type:** Analog Input Module
- **Address:** 0x07 (RS485)
- **Power Supply:** 24V DC
- **Current Consumption:** < 100mA
- **Operating Temperature:** -40°C to +85°C
- **Protection:** IP20

### **2. Analog Input Specifications:**
- **Channels:** 16 channels
- **Resolution:** 16-bit ADC
- **Input Range:** 0-10V, 0-20mA, 4-20mA
- **Accuracy:** ±0.1% full scale
- **Sampling Rate:** Up to 1000 samples/second
- **Input Impedance:** 1MΩ (voltage), 250Ω (current)

### **3. Sensor Support:**
- **Distance Sensors:** Ultrasonic, IR, Laser rangefinder
- **Temperature Sensors:** PT100, PT1000, Thermocouple
- **Pressure Sensors:** 0-10V, 4-20mA pressure transmitters
- **Level Sensors:** Float switches, capacitive level sensors
- **Flow Sensors:** Turbine, magnetic, ultrasonic flow meters
- **Position Sensors:** Potentiometers, LVDT, encoders

### **4. Signal Conditioning:**
- **Filtering:** Low-pass, high-pass, band-pass filters
- **Amplification:** Programmable gain amplifiers
- **Isolation:** Opto-isolated inputs
- **Calibration:** Auto-calibration và manual calibration
- **Diagnostics:** Self-diagnostic functions

### **5. Communication Features:**
- **RS485:** Modbus RTU protocol (primary)
- **Ethernet:** 10/100 Mbps (optional)
- **USB:** Device interface (optional)

---

## 🔌 **HARDWARE CONNECTION**

### **1. Power Connection:**
```
┌─────────────────────────────────────┐
│      Analog Input Module            │
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

### **3. Analog Input Connections:**
```
┌─────────────────────────────────────┐
│      Analog Input Module            │
├─────────────────────────────────────┤
│ AI1+ ─────────────────────────────┼─── Sensor 1 Positive
│ AI1- ─────────────────────────────┼─── Sensor 1 Negative
│ AI2+ ─────────────────────────────┼─── Sensor 2 Positive
│ AI2- ─────────────────────────────┼─── Sensor 2 Negative
│ AI3+ ─────────────────────────────┼─── Sensor 3 Positive
│ AI3- ─────────────────────────────┼─── Sensor 3 Negative
│ AI4+ ─────────────────────────────┼─── Sensor 4 Positive
│ AI4- ─────────────────────────────┼─── Sensor 4 Negative
│ ... (up to 16 channels)           │
└─────────────────────────────────────┘
```

### **4. Sensor Type Examples:**
```
┌─────────────────────────────────────┐
│      Analog Input Module            │
├─────────────────────────────────────┤
│ AI1 ───────────────────────────────┼─── Distance Sensor (0-10V)
│ AI2 ───────────────────────────────┼─── Temperature Sensor (4-20mA)
│ AI3 ───────────────────────────────┼─── Pressure Sensor (0-10V)
│ AI4 ───────────────────────────────┼─── Level Sensor (4-20mA)
│ AI5 ───────────────────────────────┼─── Flow Sensor (0-10V)
│ AI6 ───────────────────────────────┼─── Position Sensor (0-10V)
│ AI7 ───────────────────────────────┼─── Load Cell (0-10V)
│ AI8 ───────────────────────────────┼─── Strain Gauge (0-10V)
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

### **3. Analog Input Module Configuration:**
```json
{
  "analog_input_module": {
    "address": "0x07",
    "name": "Analog Input Module",
    "type": "analog_input",
    "version": "1.0",
    "enabled": true,
    "analog_config": {
      "enabled": true,
      "channels": [
        {"id": 1, "enabled": true, "type": "voltage", "range": "0-10V", "sensor": "distance_sensor"},
        {"id": 2, "enabled": true, "type": "current", "range": "4-20mA", "sensor": "temperature_sensor"},
        {"id": 3, "enabled": true, "type": "voltage", "range": "0-10V", "sensor": "pressure_sensor"},
        {"id": 4, "enabled": true, "type": "current", "range": "4-20mA", "sensor": "level_sensor"},
        {"id": 5, "enabled": true, "type": "voltage", "range": "0-10V", "sensor": "flow_sensor"},
        {"id": 6, "enabled": true, "type": "voltage", "range": "0-10V", "sensor": "position_sensor"},
        {"id": 7, "enabled": true, "type": "voltage", "range": "0-10V", "sensor": "load_cell"},
        {"id": 8, "enabled": true, "type": "voltage", "range": "0-10V", "sensor": "strain_gauge"}
      ],
      "sampling_rate": 100,
      "resolution": 16,
      "calibration": true
    },
    "signal_conditioning": {
      "enabled": true,
      "filtering": {
        "low_pass": true,
        "high_pass": false,
        "band_pass": false
      },
      "amplification": {
        "gain": 1.0,
        "auto_gain": false
      },
      "isolation": {
        "enabled": true,
        "type": "opto_isolated"
      }
    },
    "diagnostics": {
      "enabled": true,
      "self_test": true,
      "calibration_check": true,
      "fault_detection": true
    },
    "system_config": {
      "baudrate": 3,
      "parity": 0,
      "device_id": 7
    }
  }
}
```

---

## 💻 **IMPLEMENTATION SOFTWARE**

### **1. HAL RS485 Driver (`hal_rs485.c`)**

#### **Core Functions:**
```c
// Analog Input Module specific functions
hal_status_t analog_input_module_init(void);
hal_status_t analog_input_module_read_channel(uint8_t channel, uint16_t *value);
hal_status_t analog_input_module_read_all_channels(uint16_t *values);
hal_status_t analog_input_module_set_channel_config(uint8_t channel, analog_config_t *config);
hal_status_t analog_input_module_get_channel_config(uint8_t channel, analog_config_t *config);
hal_status_t analog_input_module_calibrate_channel(uint8_t channel);
hal_status_t analog_input_module_get_status(analog_status_t *status);
hal_status_t analog_input_module_self_test(void);
hal_status_t analog_input_module_read_diagnostics(diagnostics_data_t *diagnostics);
```

### **2. Analog Input Manager Application (`analog_input_manager.c`)**

#### **Core Functions:**
```c
// Analog input management functions
analog_status_t analog_input_manager_get_status(void);
hal_status_t analog_input_manager_read_sensors(sensor_data_t *sensors);
hal_status_t analog_input_manager_process_distance_data(distance_data_t *distance);
hal_status_t analog_input_manager_process_temperature_data(temperature_data_t *temperature);
hal_status_t analog_input_manager_process_pressure_data(pressure_data_t *pressure);
hal_status_t analog_input_manager_calibrate_all_channels(void);
hal_status_t analog_input_manager_save_configuration(const char *config_path);
hal_status_t analog_input_manager_load_configuration(const char *config_path);
hal_status_t analog_input_manager_optimize_sampling(void);
```

---

## 📡 **MODBUS REGISTER MAP**

### **1. Analog Input Status Registers (0x0000-0x00FF):**

#### **Analog Input Status (0x0000-0x000F):**
```c
// Analog input module status
#define ANALOG_REG_STATUS                 0x0000  // Bit field

// Module status
#define ANALOG_REG_MODULE_STATUS          0x0001  // 0=Idle, 1=Active, 2=Calibrating

// Sampling status
#define ANALOG_REG_SAMPLING_STATUS        0x0002  // 0=Stopped, 1=Running, 2=Error

// Module type
#define ANALOG_REG_MODULE_TYPE            0x0003  // 0=Analog Input, 1=Analog Output, 2=Hybrid

// Module version
#define ANALOG_REG_MODULE_VERSION         0x0004  // 16-bit unsigned

// Sampling rate (Hz)
#define ANALOG_REG_SAMPLING_RATE          0x0005  // 16-bit unsigned

// Resolution (bits)
#define ANALOG_REG_RESOLUTION             0x0006  // 16-bit unsigned

// Active channels
#define ANALOG_REG_ACTIVE_CHANNELS        0x0007  // 16-bit unsigned

// Error code
#define ANALOG_REG_ERROR_CODE             0x0008  // 16-bit unsigned
```

### **2. Analog Input Data Registers (0x0100-0x01FF):**

#### **Analog Input Values (0x0100-0x010F):**
```c
// Analog input channel 1 value (16-bit)
#define ANALOG_REG_CH1_VALUE              0x0100  // 16-bit unsigned

// Analog input channel 2 value (16-bit)
#define ANALOG_REG_CH2_VALUE              0x0101  // 16-bit unsigned

// Analog input channel 3 value (16-bit)
#define ANALOG_REG_CH3_VALUE              0x0102  // 16-bit unsigned

// Analog input channel 4 value (16-bit)
#define ANALOG_REG_CH4_VALUE              0x0103  // 16-bit unsigned

// Analog input channel 5 value (16-bit)
#define ANALOG_REG_CH5_VALUE              0x0104  // 16-bit unsigned

// Analog input channel 6 value (16-bit)
#define ANALOG_REG_CH6_VALUE              0x0105  // 16-bit unsigned

// Analog input channel 7 value (16-bit)
#define ANALOG_REG_CH7_VALUE              0x0106  // 16-bit unsigned

// Analog input channel 8 value (16-bit)
#define ANALOG_REG_CH8_VALUE              0x0107  // 16-bit unsigned

// Analog input channel 9 value (16-bit)
#define ANALOG_REG_CH9_VALUE              0x0108  // 16-bit unsigned

// Analog input channel 10 value (16-bit)
#define ANALOG_REG_CH10_VALUE             0x0109  // 16-bit unsigned

// Analog input channel 11 value (16-bit)
#define ANALOG_REG_CH11_VALUE             0x010A  // 16-bit unsigned

// Analog input channel 12 value (16-bit)
#define ANALOG_REG_CH12_VALUE             0x010B  // 16-bit unsigned

// Analog input channel 13 value (16-bit)
#define ANALOG_REG_CH13_VALUE             0x010C  // 16-bit unsigned

// Analog input channel 14 value (16-bit)
#define ANALOG_REG_CH14_VALUE             0x010D  // 16-bit unsigned

// Analog input channel 15 value (16-bit)
#define ANALOG_REG_CH15_VALUE             0x010E  // 16-bit unsigned

// Analog input channel 16 value (16-bit)
#define ANALOG_REG_CH16_VALUE             0x010F  // 16-bit unsigned
```

### **3. Sensor Data Registers (0x0200-0x02FF):**

#### **IMU Data (0x0200-0x020F):**
```c
// IMU accelerometer X (0.01g resolution)
#define AI_REG_IMU_ACCEL_X                0x0200  // 16-bit signed

// IMU accelerometer Y (0.01g resolution)
#define AI_REG_IMU_ACCEL_Y                0x0201  // 16-bit signed

// IMU accelerometer Z (0.01g resolution)
#define AI_REG_IMU_ACCEL_Z                0x0202  // 16-bit signed

// IMU gyroscope X (0.01°/s resolution)
#define AI_REG_IMU_GYRO_X                 0x0203  // 16-bit signed

// IMU gyroscope Y (0.01°/s resolution)
#define AI_REG_IMU_GYRO_Y                 0x0204  // 16-bit signed

// IMU gyroscope Z (0.01°/s resolution)
#define AI_REG_IMU_GYRO_Z                 0x0205  // 16-bit signed

// IMU temperature (°C)
#define AI_REG_IMU_TEMPERATURE            0x0206  // 16-bit signed

// IMU status
#define AI_REG_IMU_STATUS                 0x0207  // Bit field
```

#### **Environmental Data (0x0210-0x021F):**
```c
// Temperature (°C)
#define AI_REG_TEMPERATURE                0x0210  // 16-bit signed

// Humidity (%)
#define AI_REG_HUMIDITY                   0x0211  // 16-bit unsigned

// Pressure (hPa)
#define AI_REG_PRESSURE                   0x0212  // 16-bit unsigned

// Light level (lux)
#define AI_REG_LIGHT_LEVEL                0x0213  // 16-bit unsigned

// Environmental status
#define AI_REG_ENVIRONMENTAL_STATUS       0x0214  // Bit field
```

### **4. Inference Results Registers (0x0300-0x03FF):**

#### **Detection Results (0x0300-0x030F):**
```c
// Number of detections
#define AI_REG_DETECTION_COUNT            0x0300  // 16-bit unsigned

// Detection 1 class ID
#define AI_REG_DETECTION1_CLASS           0x0301  // 16-bit unsigned

// Detection 1 confidence (0-100%)
#define AI_REG_DETECTION1_CONFIDENCE      0x0302  // 16-bit unsigned

// Detection 1 bounding box X
#define AI_REG_DETECTION1_BBOX_X          0x0303  // 16-bit unsigned

// Detection 1 bounding box Y
#define AI_REG_DETECTION1_BBOX_Y          0x0304  // 16-bit unsigned

// Detection 1 bounding box Width
#define AI_REG_DETECTION1_BBOX_W          0x0305  // 16-bit unsigned

// Detection 1 bounding box Height
#define AI_REG_DETECTION1_BBOX_H          0x0306  // 16-bit unsigned

// Detection 2 class ID
#define AI_REG_DETECTION2_CLASS           0x0307  // 16-bit unsigned

// Detection 2 confidence (0-100%)
#define AI_REG_DETECTION2_CONFIDENCE      0x0308  // 16-bit unsigned

// Detection 2 bounding box X
#define AI_REG_DETECTION2_BBOX_X          0x0309  // 16-bit unsigned

// Detection 2 bounding box Y
#define AI_REG_DETECTION2_BBOX_Y          0x030A  // 16-bit unsigned

// Detection 2 bounding box Width
#define AI_REG_DETECTION2_BBOX_W          0x030B  // 16-bit unsigned

// Detection 2 bounding box Height
#define AI_REG_DETECTION2_BBOX_H          0x030C  // 16-bit unsigned
```

### **5. System Status Registers (0x0400-0x04FF):**

#### **System Status (0x0400-0x040F):**
```c
// System status
#define AI_REG_SYSTEM_STATUS              0x0400  // Bit field

// Memory usage (%)
#define AI_REG_MEMORY_USAGE               0x0401  // 16-bit unsigned

// Storage usage (%)
#define AI_REG_STORAGE_USAGE              0x0402  // 16-bit unsigned

// CPU usage (%)
#define AI_REG_CPU_USAGE                  0x0403  // 16-bit unsigned

// Temperature (°C)
#define AI_REG_SYSTEM_TEMPERATURE         0x0404  // 16-bit signed

// Uptime (hours)
#define AI_REG_UPTIME_HOURS               0x0405  // 16-bit unsigned

// Model inference count
#define AI_REG_INFERENCE_COUNT            0x0406  // 32-bit unsigned

// Error count
#define AI_REG_ERROR_COUNT                0x0407  // 16-bit unsigned
```

### **6. Fault Status Registers (0x0500-0x05FF):**

#### **Fault Status (0x0500-0x050F):**
```c
// Fault status register
#define AI_REG_FAULT_STATUS               0x0500  // Bit field

// AI processing fault
#define AI_REG_AI_FAULT                   0x0501  // 0=OK, 1=Fault

// Camera fault
#define AI_REG_CAMERA_FAULT               0x0502  // 0=OK, 1=Fault

// Sensor fault
#define AI_REG_SENSOR_FAULT               0x0503  // 0=OK, 1=Fault

// Memory fault
#define AI_REG_MEMORY_FAULT               0x0504  // 0=OK, 1=Fault

// Storage fault
#define AI_REG_STORAGE_FAULT              0x0505  // 0=OK, 1=Fault

// Communication fault
#define AI_REG_COMM_FAULT                 0x0506  // 0=OK, 1=Fault

// Temperature fault
#define AI_REG_TEMP_FAULT                 0x0507  // 0=OK, 1=Fault
```

### **7. Data Validation Fault (0x0069):**
```c
// Data validation fault
#define ANALOG_REG_DATA_VALIDATION_FAULT 0x0069  // 0=OK, 1=Fault
```

#### **System Registers (0x00F0-0x00FF):**
```c
// Device ID (Modbus slave address)
#define ANALOG_REG_DEVICE_ID             0x00F0  // 16-bit unsigned

// Firmware version (e.g. 0x0101 = v1.01)
#define ANALOG_REG_FIRMWARE_VERSION      0x00F1  // 16-bit unsigned

// System status (bit field)
#define ANALOG_REG_SYSTEM_STATUS         0x00F2  // 16-bit unsigned

// System error (global error code)
#define ANALOG_REG_SYSTEM_ERROR          0x00F3  // 16-bit unsigned

// Reset error command (write 1 to reset all error flags)
#define ANALOG_REG_RESET_ERROR_CMD       0x00F4  // 16-bit unsigned

// Config baudrate (1=9600, 2=19200, 3=38400,...)
#define ANALOG_REG_CONFIG_BAUDRATE       0x00F5  // 16-bit unsigned

// Config parity (0=None, 1=Even, 2=Odd)
#define ANALOG_REG_CONFIG_PARITY         0x00F6  // 16-bit unsigned

// Module type (0x0007 = Analog Input Module)
#define ANALOG_REG_MODULE_TYPE           0x00F7  // 16-bit unsigned

// Module name (low word)
#define ANALOG_REG_MODULE_NAME_LOW       0x00F8  // 16-bit unsigned

// Module name (high word)
#define ANALOG_REG_MODULE_NAME_HIGH      0x00F9  // 16-bit unsigned

// Hardware version
#define ANALOG_REG_HARDWARE_VERSION      0x00FA  // 16-bit unsigned

// Serial number (low word)
#define ANALOG_REG_SERIAL_NUMBER_LOW     0x00FB  // 16-bit unsigned

// Serial number (high word)
#define ANALOG_REG_SERIAL_NUMBER_HIGH    0x00FC  // 16-bit unsigned

// Build date (low word)
#define ANALOG_REG_BUILD_DATE_LOW        0x00FD  // 16-bit unsigned

// Build date (high word)
#define ANALOG_REG_BUILD_DATE_HIGH       0x00FE  // 16-bit unsigned

// Register checksum
#define ANALOG_REG_CHECKSUM              0x00FF  // 16-bit unsigned
```

---

## 🧪 **TESTING PROCEDURES**

### **1. Basic Communication Test:**
```python
#!/usr/bin/env python3
import serial
import time

def test_analog_input_communication():
    # Open serial port
    ser = serial.Serial(
        port='/dev/ttyS1',
        baudrate=115200,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=1
    )

    # Test communication with Analog Input module
    module_addr = 0x07
    print(f"Testing Analog Input module at address 0x{module_addr:02X}")

    # Read analog input status
    command = bytes([module_addr, 0x03, 0x00, 0x00, 0x00, 0x01])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')

    ser.write(command)
    response = ser.read(7)

    if len(response) == 7:
        print("  ✓ Analog Input module responded")
        print(f"  Module status: 0x{response[3]:02X}")
    else:
        print("  ✗ Analog Input module no response")

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
    test_analog_input_communication()
```

### **2. Analog Input Test:**
```python
def test_analog_inputs():
    ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
    
    # Read all analog input values
    command = bytes([0x07, 0x03, 0x01, 0x00, 0x00, 0x10])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(35)  # Expected response length
    
    if len(response) == 35:
        print("Analog Input Values:")
        for i in range(16):
            value = int.from_bytes(response[3+i*2:5+i*2], byteorder='big')
            voltage = (value / 65535.0) * 10.0  # Convert to voltage (0-10V)
            print(f"  AI{i+1}: {value} ({voltage:.2f}V)")
    
    ser.close()
```

### **3. Distance Sensor Test:**
```python
def test_distance_sensor():
    ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
    
    # Read distance sensor (AI1)
    command = bytes([0x07, 0x03, 0x01, 0x00, 0x00, 0x01])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(7)
    
    if len(response) == 7:
        value = int.from_bytes(response[3:5], byteorder='big')
        voltage = (value / 65535.0) * 10.0  # Convert to voltage (0-10V)
        distance = voltage * 100  # Convert to distance (cm) - example conversion
        print(f"  Distance Sensor: {value} ({voltage:.2f}V, {distance:.1f}cm)")
    
    ser.close()
```

### **4. Temperature Sensor Test:**
```python
def test_temperature_sensor():
    ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
    
    # Read temperature sensor (AI2 - 4-20mA)
    command = bytes([0x07, 0x03, 0x01, 0x01, 0x00, 0x01])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(7)
    
    if len(response) == 7:
        value = int.from_bytes(response[3:5], byteorder='big')
        current = (value / 65535.0) * 20.0  # Convert to current (0-20mA)
        temperature = (current - 4.0) * 25.0  # Convert to temperature (°C) - example conversion
        print(f"  Temperature Sensor: {value} ({current:.2f}mA, {temperature:.1f}°C)")
    
    ser.close()
```

---

## 📊 **PERFORMANCE METRICS**

### **1. Key Performance Indicators:**
- **Inference Speed:** < 100ms per frame
- **Model Accuracy:** > 90% for object detection
- **Camera Resolution:** Up to 4K (3840x2160)
- **Frame Rate:** 30 FPS
- **Memory Usage:** < 80% of 4GB
- **Storage Usage:** < 70% of 32GB
- **Temperature Range:** -40°C to +85°C
- **Power Consumption:** < 500mA at 24V

### **2. Monitoring Metrics:**
- **AI Processing:** Real-time inference monitoring
- **Camera Status:** Real-time camera monitoring
- **Sensor Data:** Real-time sensor monitoring
- **System Health:** Memory, storage, CPU usage
- **Detection Results:** Object detection statistics
- **Error Tracking:** Fault detection and reporting
- **Performance:** Inference time and accuracy

---

## 🔧 **TROUBLESHOOTING**

### **1. Common Issues:**

#### **Communication Issues:**
- **Problem:** Module not responding
- **Solution:** Check RS485 wiring, baudrate, address
- **Diagnostic:** Use communication test script

#### **Model Loading Issues:**
- **Problem:** Model not loading
- **Solution:** Check model file, storage space, format
- **Diagnostic:** Verify model path and format

#### **Inference Issues:**
- **Problem:** Slow inference or low accuracy
- **Solution:** Optimize model, check input data
- **Diagnostic:** Monitor inference time and accuracy

#### **Camera Issues:**
- **Problem:** Camera not working
- **Solution:** Check camera connection, power, configuration
- **Diagnostic:** Test camera interface and settings

#### **Sensor Issues:**
- **Problem:** Sensor data incorrect
- **Solution:** Calibrate sensors, check connections
- **Diagnostic:** Verify sensor readings and calibration

### **2. Error Codes:**
```c
// Error code definitions
#define AI_ERROR_NONE                     0x0000
#define AI_ERROR_COMMUNICATION            0x0001
#define AI_ERROR_MODEL_LOADING            0x0002
#define AI_ERROR_INFERENCE                0x0003
#define AI_ERROR_CAMERA                   0x0004
#define AI_ERROR_SENSOR                   0x0005
#define AI_ERROR_MEMORY                   0x0006
#define AI_ERROR_STORAGE                  0x0007
#define AI_ERROR_TEMPERATURE              0x0008
#define AI_ERROR_HARDWARE                 0x0009
```

---

## 🚀 **INTEGRATION VỚI SYSTEM**

### **1. System State Machine Integration:**

#### **AI States:**
```c
typedef enum {
    AI_STATE_IDLE,
    AI_STATE_LOADING,
    AI_STATE_PROCESSING,
    AI_STATE_FAULT,
    AI_STATE_MAINTENANCE
} ai_state_t;
```

#### **State Transitions:**
```c
// AI state machine integration
hal_status_t ai_manager_update_state(void) {
    ai_status_t status;
    hal_status_t result = ai_module_get_status(&status);
    
    if (result == HAL_STATUS_OK) {
        if (status.error_code != 0) {
            g_ai_manager.state = AI_STATE_FAULT;
        } else if (status.inference_status == AI_INFERENCE_PROCESSING) {
            g_ai_manager.state = AI_STATE_PROCESSING;
        } else if (status.model_status == AI_MODEL_LOADING) {
            g_ai_manager.state = AI_STATE_LOADING;
        } else {
            g_ai_manager.state = AI_STATE_IDLE;
        }
    }
    
    return HAL_STATUS_OK;
}
```

### **2. Object Detection Integration:**

#### **Detection Handling:**
```c
// Object detection integration
hal_status_t ai_manager_handle_detections(void) {
    object_detection_t detections;
    hal_status_t result = ai_manager_detect_objects(&detections);
    
    if (result == HAL_STATUS_OK) {
        for (int i = 0; i < detections.count; i++) {
            // Process each detection
            if (detections.objects[i].confidence > 0.7) {
                // High confidence detection
                system_manager_handle_object_detection(&detections.objects[i]);
            }
        }
        
        return HAL_STATUS_OK;
    }
    
    return HAL_STATUS_ERROR;
}
```

---

## 📋 **MAINTENANCE**

### **1. Regular Maintenance:**
- **Weekly:** Check system performance metrics
- **Monthly:** Update AI models if needed
- **Quarterly:** Calibrate sensors
- **Annually:** Full system diagnostics
- **As needed:** Model optimization and retraining

### **2. Model Management:**
```c
// Model update procedure
hal_status_t ai_module_update_model(const char *model_path) {
    // Stop current inference
    ai_module_stop_inference();
    
    // Load new model
    hal_status_t result = ai_module_load_model(model_path);
    
    if (result == HAL_STATUS_OK) {
        // Validate model
        result = ai_module_validate_model();
        
        if (result == HAL_STATUS_OK) {
            // Start inference with new model
            ai_module_start_inference();
        }
    }
    
    return result;
}
```

### **3. Performance Optimization:**
```c
// Performance optimization
hal_status_t ai_manager_optimize_performance(void) {
    // Monitor system resources
    system_metrics_t metrics;
    system_manager_get_metrics(&metrics);
    
    if (metrics.memory_usage > 80) {
        // Optimize memory usage
        ai_module_optimize_memory();
    }
    
    if (metrics.cpu_usage > 90) {
        // Reduce inference frequency
        ai_module_adjust_inference_interval(200);
    }
    
    return HAL_STATUS_OK;
}
```

---

**Changelog:**
- v1.0 (2025-01-27): Initial Analog Input Module integration guide
- v1.1 (2025-01-27): Added System Registers (0x00F0-0x00FF) for auto-detect compatibility
