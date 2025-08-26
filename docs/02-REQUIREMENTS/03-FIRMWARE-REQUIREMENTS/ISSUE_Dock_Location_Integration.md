# ISSUE: Implement OHT-50 Dock & Location Module Integration

## Issue Title
**Implement OHT-50 Dock & Location Module Integration with RS485 Modbus RTU**

## Issue Description

### 🎯 **Overview**
Implement a complete STM32-based Dock & Location Module for OHT-50 system with RS485 Modbus RTU communication protocol.

### 📋 **Requirements**

#### **Hardware Components:**
- **1x Accelerometer:** Grove MMA7660FC (±1.5g, 21LSB/g sensitivity)
- **4x DI Magnetic Switches:** Digital input magnetic switches
- **1x RFID/NFC Reader:** RFID/NFC card reader
- **STM32 Controller:** STM32F4 series recommended
- **RS485 Transceiver:** MAX485 or equivalent
- **Power Supply:** 3.3V/5V regulated supply

#### **Communication Protocol:**
- **Protocol:** Modbus RTU
- **Slave Address:** 0x05 (fixed)
- **Baud Rate:** 115200 bps
- **Data Bits:** 8
- **Stop Bits:** 1
- **Parity:** None
- **Error Detection:** CRC-16

### 📊 **Register Mapping**

#### **Accelerometer Data (0x0000-0x0004):**
```
0x0000: Accelerometer X (mg) - 16-bit signed
0x0001: Accelerometer Y (mg) - 16-bit signed  
0x0002: Accelerometer Z (mg) - 16-bit signed
0x0003: Accelerometer Status - 16-bit unsigned
0x0004: Accelerometer Error Code - 16-bit unsigned
```

#### **DI Data (0x0010-0x0015):**
```
0x0010: DI 1 (Magnetic Switch) - 16-bit unsigned
0x0011: DI 2 (Magnetic Switch) - 16-bit unsigned
0x0012: DI 3 (Magnetic Switch) - 16-bit unsigned
0x0013: DI 4 (Magnetic Switch) - 16-bit unsigned
0x0014: DI Status - 16-bit unsigned
0x0015: DI Error Code - 16-bit unsigned
```

#### **RFID/NFC Data (0x0020-0x0025):**
```
0x0020: RFID/NFC Data Low - 16-bit unsigned
0x0021: RFID/NFC Data High - 16-bit unsigned
0x0022: RFID/NFC Status - 16-bit unsigned
0x0023: RFID/NFC Error Code - 16-bit unsigned
0x0024: RFID/NFC Card Type - 16-bit unsigned
0x0025: RFID/NFC Card ID - 16-bit unsigned
```

#### **System Data (0x0030-0x0031):**
```
0x0030: System Status - 16-bit unsigned
0x0031: System Error - 16-bit unsigned
```

#### **Configuration (0x0060-0x0064):**
```
0x0060: Accelerometer Sample Rate (Hz) - 16-bit unsigned
0x0061: DI Debounce Time (ms) - 16-bit unsigned
0x0062: RFID Read Timeout (ms) - 16-bit unsigned
0x0063: Data Validation - 16-bit unsigned
0x0064: Fault Reporting - 16-bit unsigned
```

#### **Auto-Detect Registers (0x00F0-0x00FF):**
```
0x00F0: Device ID (Modbus slave address) - 16-bit unsigned
0x00F1: Firmware Version (0x0101 = v1.01) - 16-bit unsigned
0x00F2: System Status (bit field) - 16-bit unsigned
0x00F3: System Error (global error code) - 16-bit unsigned
0x00F4: Reset Error Command (write 1 to reset) - 16-bit unsigned
0x00F5: Config Baudrate (1=9600, 2=19200, 3=38400, 4=57600, 5=115200) - 16-bit unsigned
0x00F6: Config Parity (0=None, 1=Even, 2=Odd) - 16-bit unsigned
0x00F7: Module Type (0x0005 = Sensor Module) - 16-bit unsigned
0x00F8: Module Name Low (ASCII characters) - 16-bit unsigned
0x00F9: Module Name High (ASCII characters) - 16-bit unsigned
0x00FA: Hardware Version (0x0101 = v1.01) - 16-bit unsigned
0x00FB: Serial Number Low - 16-bit unsigned
0x00FC: Serial Number High - 16-bit unsigned
0x00FD: Build Date Low - 16-bit unsigned
0x00FE: Build Date High - 16-bit unsigned
0x00FF: Checksum (CRC16 of registers 0x00F0-0x00FE) - 16-bit unsigned
```

### 🔧 **Technical Specifications**

#### **Accelerometer (MMA7660FC):**
- **Range:** ±1.5g
- **Sensitivity:** 21 LSB/g
- **Interface:** I2C
- **Sample Rate:** Configurable (10-120 Hz)
- **Power Supply:** 3.3V

#### **Magnetic Switches:**
- **Type:** Hall effect magnetic switches
- **Input Voltage:** 3.3V/5V compatible
- **Response Time:** < 1ms
- **Contact Type:** Normally open/closed configurable

#### **RFID/NFC Reader:**
- **Supported Cards:** ISO14443A, ISO14443B, ISO15693
- **Read Distance:** 0-50mm
- **Interface:** SPI/UART
- **Power Supply:** 3.3V

### 💻 **Software Requirements**

#### **Core Functions:**
```c
// Module initialization
hal_status_t dock_location_module_init(void);

// Accelerometer functions
hal_status_t read_accelerometer_data(accelerometer_data_t *data);
hal_status_t configure_accelerometer(accelerometer_config_t *config);

// DI functions
hal_status_t read_di_data(di_data_t *data);
hal_status_t configure_di_debounce(uint16_t debounce_time_ms);

// RFID/NFC functions
hal_status_t read_rfid_data(rfid_data_t *data);
hal_status_t configure_rfid_timeout(uint16_t timeout_ms);

// System functions
hal_status_t read_system_status(system_status_t *status);
hal_status_t reset_system_errors(void);
hal_status_t get_module_info(module_info_t *info);
```

#### **Modbus RTU Implementation:**
- **Slave Address:** 0x05
- **Function Codes:** 03 (Read Holding), 04 (Read Input), 06 (Write Single), 16 (Write Multiple)
- **Register Access:** Read/Write according to register definitions
- **Error Handling:** Standard Modbus exception codes
- **CRC Calculation:** CRC-16 (Modbus polynomial)

### 🧪 **Testing Requirements**

#### **Unit Tests:**
- Accelerometer data reading and calibration
- DI switch debouncing and state detection
- RFID/NFC card reading and validation
- Modbus RTU communication protocol
- Auto-detect register functionality
- Error handling and recovery

#### **Integration Tests:**
- Full module initialization sequence
- Real-time data acquisition
- Communication with OHT-50 Master Module
- Fault detection and reporting
- Power management and low-power modes

#### **Performance Tests:**
- Response time < 100ms for all operations
- Data accuracy validation
- Communication reliability testing
- Power consumption optimization

### 📁 **Project Structure**
```
stm32-dock_and_location-manager/
├── src/
│   ├── main.c
│   ├── modbus_rtu.c
│   ├── accelerometer.c
│   ├── di_switches.c
│   ├── rfid_reader.c
│   ├── system_status.c
│   └── auto_detect.c
├── inc/
│   ├── modbus_rtu.h
│   ├── accelerometer.h
│   ├── di_switches.h
│   ├── rfid_reader.h
│   ├── system_status.h
│   └── auto_detect.h
├── docs/
│   ├── README.md
│   ├── register_map.md
│   ├── wiring_diagram.md
│   └── testing_procedures.md
├── tests/
│   ├── unit_tests/
│   ├── integration_tests/
│   └── performance_tests/
└── config/
    ├── modbus_config.h
    ├── hardware_config.h
    └── system_config.h
```

### 🎯 **Deliverables**

#### **Code Deliverables:**
- [ ] Complete STM32 firmware implementation
- [ ] Modbus RTU protocol implementation
- [ ] Accelerometer driver and calibration
- [ ] DI switch management with debouncing
- [ ] RFID/NFC reader driver
- [ ] Auto-detect functionality
- [ ] Error handling and fault management

#### **Documentation Deliverables:**
- [ ] Complete API documentation
- [ ] Register map documentation
- [ ] Wiring diagrams and schematics
- [ ] Testing procedures and test cases
- [ ] Integration guide with OHT-50 Master Module
- [ ] Troubleshooting guide

#### **Testing Deliverables:**
- [ ] Unit test suite with >90% coverage
- [ ] Integration test procedures
- [ ] Performance test results
- [ ] Hardware validation test procedures
- [ ] Communication protocol validation

### 📅 **Timeline**
- **Week 1-2:** Hardware setup and basic drivers
- **Week 3-4:** Modbus RTU implementation
- **Week 5-6:** Sensor integration and calibration
- **Week 7-8:** Testing and documentation
- **Week 9:** Integration with OHT-50 Master Module
- **Week 10:** Final validation and delivery

### 🔗 **References**
- [OHT-50 RS485 Dock & Location Integration Guide](docs/02-REQUIREMENTS/03-FIRMWARE-REQUIREMENTS/REQ_RS485_Dock_Location_Integration.md)
- [Grove MMA7660FC Accelerometer Datasheet](https://hshop.vn/grove-3-axis-digital-accelerometer-1-5g-cam-bien-gia-toc-3-truc)
- [Modbus RTU Protocol Specification](https://modbus.org/specs.php)
- [STM32 HAL Documentation](https://www.st.com/en/embedded-software/stm32cube-mcu-mpu-packages.html)

### 🏷️ **Labels**
- `enhancement`
- `hardware`
- `firmware`
- `modbus`
- `sensors`
- `integration`
- `documentation`

### 👥 **Assignees**
- [ ] Hardware Engineer
- [ ] Firmware Developer
- [ ] Test Engineer
- [ ] Documentation Writer

---

**Note:** This issue is part of the OHT-50 Master Module project. Please refer to the complete requirements document for detailed specifications and integration guidelines.
