# HƯỚNG DẪN KẾT NỐI VÀ ĐIỀU KHIỂN MODULE NGUỒN QUA RS485
## OHT-50 Master Module - Power Module Integration

**Phiên bản:** v2.0  
**Ngày tạo:** 2025-01-28  
**Team:** FW Team  
**Trạng thái:** Implementation Guide  
**Reference:** https://github.com/hiimshyy/stm32-power-management/blob/main/Docs/modbus_register_map.md

---

**Changelog:**
- v2.0 (2025-01-28): Updated to STM32 Power Management reference, added DalyBMS, SK60X, INA219 integration
- v1.2 (2025-01-27): Added System Registers (0x00F0-0x00FF) for auto-detect compatibility
- v1.1 (2025-01-27): Removed GPIO_DE pin (not needed for RS485)
- v1.0 (2025-01-27): Initial Power Module integration guide

## 🎯 **TỔNG QUAN**

### **Mục tiêu:**
- Kết nối Master Module (Orange Pi 5B) với Power Module qua RS485
- Quản lý nguồn điện (pin, charger, power distribution) với STM32 Power Management
- Giám sát trạng thái pin (voltage, current, temperature, SoC) qua DalyBMS
- Điều khiển sạc pin tự động và bảo vệ an toàn qua SK60X
- Monitoring power distribution qua INA219 sensors

### **Kiến trúc hệ thống:**
```
┌─────────────────┐    RS485    ┌─────────────────┐
│   Master Module │ ──────────── │  Power Module   │
│  (Orange Pi 5B) │             │   (STM32)       │
│                 │             │                 │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ HAL RS485   │ │             │ │ Power Ctrl  │ │
│ │ Driver      │ │             │ │ Logic       │ │
│ └─────────────┘ │             │ └─────────────┘ │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ Power Mgr   │ │             │ │ DalyBMS     │ │
│ │ Application │ │             │ │ Interface   │ │
│ └─────────────┘ │             │ └─────────────┘ │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ Modbus RTU  │ │             │ │ SK60X       │ │
│ │ Protocol    │ │             │ │ Charger     │ │
│ └─────────────┘ │             │ └─────────────┘ │
└─────────────────┘             │ ┌─────────────┐ │
                                │ │ INA219      │ │
                                │ │ Sensors     │ │
                                │ └─────────────┘ │
                                └─────────────────┘
```

---

## 🔌 **PHẦN CỨNG KẾT NỐI**

### **RS485 Interface:**
```
Master Module (Orange Pi 5B)    Power Module (STM32)
┌─────────────────┐            ┌─────────────────┐
│ UART1 (RS485)   │            │ UART (RS485)    │
│                 │            │                 │
│ TX (Pin 46) ────┼────────────┼─── RX           │
│ RX (Pin 47) ────┼────────────┼─── TX           │
│ GND ────────────┼────────────┼─── GND          │
│                 │            │                 │
│ DE (Pin 131) ───┼────────────┼─── DE           │
│ RE (Pin 132) ───┼────────────┼─── RE           │
└─────────────────┘            └─────────────────┘
```

### **Pin Configuration:**
```bash
# Orange Pi 5B UART1 RS485 Configuration
UART1_TX = GPIO1_D2 (Pin 46)
UART1_RX = GPIO1_D3 (Pin 47)
UART1_DE = GPIO1_D5 (Pin 131)  # Driver Enable
UART1_RE = GPIO1_D6 (Pin 132)  # Receiver Enable
```

### **RS485 Transceiver:**
- **Type:** MAX485 hoặc SN75176
- **Baudrate:** 115200 bps (configurable)
- **Parity:** None
- **Stop bits:** 1
- **Termination:** 120Ω resistor (nếu cần)

---

## 📡 **PROTOCOL MODBUS RTU**

### **Modbus Configuration:**
```c
// Power Module Modbus Settings
#define POWER_MODULE_ADDRESS    0x02
#define POWER_MODULE_BAUDRATE   115200
#define POWER_MODULE_PARITY     0       // None
#define POWER_MODULE_STOP_BITS  1
#define POWER_MODULE_FC_MASK    0x07    // FC3, FC6, FC16
```

### **Function Codes Supported:**
- **FC3 (0x03):** Read Holding Registers
- **FC6 (0x06):** Write Single Register  
- **FC16 (0x10):** Write Multiple Registers

### **Register Map (STM32 Power Management):**

#### **Modbus Configuration Registers (0x0100-0x0106):**
```c
#define POWER_REG_SLAVE_ID             0x0100  // Modbus slave address (1-247)
#define POWER_REG_BAUDRATE_CODE        0x0101  // UART baudrate setting
#define POWER_REG_PARITY               0x0102  // UART parity (0=None, 1=Even, 2=Odd)
#define POWER_REG_STOP_BITS            0x0103  // UART stop bits (1 or 2)
#define POWER_REG_FC_MASK              0x0104  // Supported function codes
#define POWER_REG_APPLY_CONFIG         0x0105  // Apply config flag
#define POWER_REG_CONFIG_CRC           0x0106  // Config CRC
```

#### **DalyBMS Status Registers (0x0000-0x002B):**
```c
#define POWER_REG_BATTERY_VOLTAGE      0x0000  // Battery pack voltage (V, /10.0 scaling)
#define POWER_REG_BATTERY_CURRENT      0x0001  // Battery pack current (A, /10.0 scaling)
#define POWER_REG_BATTERY_SOC          0x0002  // State of Charge (%, /10.0 scaling)
#define POWER_REG_MAX_CELL_V           0x0003  // Maximum cell voltage (mV)
#define POWER_REG_MIN_CELL_V           0x0004  // Minimum cell voltage (mV)
#define POWER_REG_CELL_DIFF            0x0007  // Voltage difference between cells (mV)
#define POWER_REG_TEMPERATURE          0x0008  // Average temperature (°C)
#define POWER_REG_CONNECTION_STATUS    0x0009  // BMS Connection Status
#define POWER_REG_CHARGE_MOS           0x000B  // Charge MOSFET status
#define POWER_REG_DISCHARGE_MOS        0x000C  // Discharge MOSFET status
#define POWER_REG_CELL_VOLTAGE_0       0x0014  // Cell 0 voltage (mV)
#define POWER_REG_CELL_VOLTAGE_1       0x0015  // Cell 1 voltage (mV)
#define POWER_REG_CELL_VOLTAGE_2       0x0016  // Cell 2 voltage (mV)
#define POWER_REG_CELL_VOLTAGE_3       0x0017  // Cell 3 voltage (mV)
#define POWER_REG_CELL_VOLTAGE_4       0x0018  // Cell 4 voltage (mV)
#define POWER_REG_CELL_VOLTAGE_5       0x0019  // Cell 5 voltage (mV)
#define POWER_REG_TEMPERATURE_0        0x001A  // Temperature sensor 0 (°C)
#define POWER_REG_TEMPERATURE_1        0x001B  // Temperature sensor 1 (°C)
#define POWER_REG_CELL_BALANCE_0       0x001C  // Cell 0 balancing status
#define POWER_REG_CELL_BALANCE_1       0x001D  // Cell 1 balancing status
#define POWER_REG_CELL_BALANCE_2       0x001E  // Cell 2 balancing status
#define POWER_REG_CELL_BALANCE_3       0x001F  // Cell 3 balancing status
#define POWER_REG_CELL_BALANCE_4       0x0020  // Cell 4 balancing status
#define POWER_REG_CELL_BALANCE_5       0x0021  // Cell 5 balancing status
#define POWER_REG_CELL_BALANCE_ACTIVE  0x0022  // Global balancing status
#define POWER_REG_FAULT_FLAGS          0x0023  // Fault status flags
```

#### **SK60X Data Registers (0x0030-0x003F):**
```c
#define POWER_REG_V_SET                0x0030  // Voltage setpoint (V, /10.0 scaling)
#define POWER_REG_I_SET                0x0031  // Current setpoint (A, /10.0 scaling)
#define POWER_REG_V_OUT                0x0032  // Output voltage (V, /10.0 scaling)
#define POWER_REG_I_OUT                0x0033  // Output current (A, /10.0 scaling)
#define POWER_REG_P_OUT                0x0034  // Output power (W, /10.0 scaling)
#define POWER_REG_V_IN                 0x0035  // Input voltage (V, /10.0 scaling)
#define POWER_REG_I_IN                 0x0036  // Input current (A, /10.0 scaling)
#define POWER_REG_TEMP                 0x0037  // Temperature of SK60X (°C)
#define POWER_REG_STATUS               0x003B  // Operational status
#define POWER_REG_ON_OFF               0x003C  // Output ON/OFF state
#define POWER_REG_CHARGE_RELAY         0x003D  // Charge relay status
#define POWER_REG_CHARGE_STATE         0x003E  // Charge state (0=IDLE, 1=WAITING, 2=CHARGE)
#define POWER_REG_CHARGE_REQUEST       0x003F  // Charge request
```

#### **INA219 Sensor Values (0x0040-0x0048):**
```c
#define POWER_REG_V_OUT_12V            0x0040  // 12V output voltage (V, /10.0 scaling)
#define POWER_REG_I_OUT_12V            0x0041  // 12V output current (A, /10.0 scaling)
#define POWER_REG_P_OUT_12V            0x0042  // 12V output power (W, /10.0 scaling)
#define POWER_REG_V_OUT_5V             0x0043  // 5V output voltage (V, /10.0 scaling)
#define POWER_REG_I_OUT_5V             0x0044  // 5V output current (A, /10.0 scaling)
#define POWER_REG_P_OUT_5V             0x0045  // 5V output power (W, /10.0 scaling)
#define POWER_REG_V_OUT_3V3            0x0046  // 3.3V output voltage (V, /10.0 scaling)
#define POWER_REG_I_OUT_3V3            0x0047  // 3.3V output current (A, /10.0 scaling)
#define POWER_REG_P_OUT_3V3            0x0048  // 3.3V output power (W, /10.0 scaling)
```

#### **Relay State (0x0049-0x004D):**
```c
#define POWER_REG_RL_12V               0x0049  // State of 12V relay
#define POWER_REG_RL_5V                0x004A  // State of 5V relay
#define POWER_REG_RL_3V3               0x004B  // State of 3V3 relay
#define POWER_REG_RL_FAUL              0x004C  // State of Faul relay
#define POWER_REG_USE_V_THS            0x004D  // Voltage usage threshold (/100.0 scaling)
```

#### **System Registers (0x00F0-0x00FF) - Auto Detect Support:**
```c
#define POWER_REG_DEVICE_ID              0x00F0  // Device ID (Modbus slave address)
#define POWER_REG_FIRMWARE_VERSION       0x00F1  // Firmware version (e.g. 0x0101 = v1.01)
#define POWER_REG_SYSTEM_STATUS          0x00F2  // System status (bit field)
#define POWER_REG_SYSTEM_ERROR           0x00F3  // System error (global error code)
#define POWER_REG_RESET_ERROR_CMD        0x00F4  // Reset error command (write 1 to reset all error flags)
#define POWER_REG_CONFIG_BAUDRATE        0x00F5  // Config baudrate (1=9600, 2=19200, 3=38400,...)
#define POWER_REG_CONFIG_PARITY          0x00F6  // Config parity (0=None, 1=Even, 2=Odd)
#define POWER_REG_MODULE_TYPE            0x00F7  // Module type (0x0002 = Power Module)
#define POWER_REG_MODULE_NAME_LOW        0x00F8  // Module name (low word)
#define POWER_REG_MODULE_NAME_HIGH       0x00F9  // Module name (high word)
#define POWER_REG_HARDWARE_VERSION       0x00FA  // Hardware version
#define POWER_REG_SERIAL_NUMBER_LOW      0x00FB  // Serial number (low word)
#define POWER_REG_SERIAL_NUMBER_HIGH     0x00FC  // Serial number (high word)
#define POWER_REG_BUILD_DATE_LOW         0x00FD  // Build date (low word)
#define POWER_REG_BUILD_DATE_HIGH        0x00FE  // Build date (high word)
#define POWER_REG_CHECKSUM               0x00FF  // Register checksum
```

---

## 🔧 **IMPLEMENTATION**

### **1. HAL RS485 Driver Setup:**
```c
// Initialize RS485 for Power Module
hal_status_t hal_rs485_init_power_module(void) {
    rs485_config_t config = {
        .baudrate = 115200,
        .parity = RS485_PARITY_NONE,
        .stop_bits = 1,
        .data_bits = 8,
        .timeout_ms = 1000,
        .retry_count = 3
    };
    
    return hal_rs485_init(&config);
}
```

### **2. Power Module Handler:**
```c
// Initialize Power Module Handler
hal_status_t power_module_handler_init(const power_module_config_t *config) {
    // Validate configuration
    if (!config) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize mutex
    pthread_mutex_init(&power_module_state.mutex, NULL);
    
    // Copy configuration
    memcpy(&power_module_state.config, config, sizeof(power_module_config_t));
    
    // Initialize RS485 communication
    hal_status_t status = hal_rs485_init_power_module();
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Auto-detect module
    status = power_module_handler_auto_detect(config->slave_id, 1000);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    power_module_state.initialized = true;
    return HAL_STATUS_OK;
}
```

### **3. Register Read/Write Functions:**
```c
// Read register from Power Module
hal_status_t power_module_handler_read_register(uint16_t register_addr, uint16_t *value) {
    if (!power_module_state.initialized || !value) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    // Modbus RTU read holding register
    uint8_t request[] = {
        power_module_state.config.slave_id,  // Slave address
        0x03,                                // Function code (Read Holding Registers)
        (register_addr >> 8) & 0xFF,         // Register address high
        register_addr & 0xFF,                // Register address low
        0x00, 0x01,                          // Quantity (1 register)
        0x00, 0x00                           // CRC (calculated)
    };
    
    // Calculate CRC
    uint16_t crc = modbus_crc16(request, 6);
    request[6] = crc & 0xFF;
    request[7] = (crc >> 8) & 0xFF;
    
    // Send request
    hal_status_t status = hal_rs485_send(request, 8);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Receive response
    uint8_t response[7];
    status = hal_rs485_receive(response, 7);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Parse response
    if (response[0] == power_module_state.config.slave_id && 
        response[1] == 0x03 && 
        response[2] == 0x02) {
        *value = (response[3] << 8) | response[4];
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_OK;
    }
    
    pthread_mutex_unlock(&power_module_state.mutex);
    return HAL_STATUS_ERROR;
}

// Write register to Power Module
hal_status_t power_module_handler_write_register(uint16_t register_addr, uint16_t value) {
    if (!power_module_state.initialized) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    pthread_mutex_lock(&power_module_state.mutex);
    
    // Modbus RTU write single register
    uint8_t request[] = {
        power_module_state.config.slave_id,  // Slave address
        0x06,                                // Function code (Write Single Register)
        (register_addr >> 8) & 0xFF,         // Register address high
        register_addr & 0xFF,                // Register address low
        (value >> 8) & 0xFF,                 // Value high
        value & 0xFF,                        // Value low
        0x00, 0x00                           // CRC (calculated)
    };
    
    // Calculate CRC
    uint16_t crc = modbus_crc16(request, 6);
    request[6] = crc & 0xFF;
    request[7] = (crc >> 8) & 0xFF;
    
    // Send request
    hal_status_t status = hal_rs485_send(request, 8);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Receive response (echo)
    uint8_t response[8];
    status = hal_rs485_receive(response, 8);
    if (status != HAL_STATUS_OK) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return status;
    }
    
    // Verify echo
    if (memcmp(request, response, 8) == 0) {
        pthread_mutex_unlock(&power_module_state.mutex);
        return HAL_STATUS_OK;
    }
    
    pthread_mutex_unlock(&power_module_state.mutex);
    return HAL_STATUS_ERROR;
}
```

---

## 📊 **USAGE EXAMPLES**

### **1. Read Battery Information:**
```c
// Read battery voltage
uint16_t voltage_raw;
hal_status_t status = power_module_handler_read_register(POWER_REG_BATTERY_VOLTAGE, &voltage_raw);
if (status == HAL_STATUS_OK) {
    float voltage = (float)voltage_raw / 10.0f;  // Scale to volts
    printf("Battery voltage: %.1fV\n", voltage);
}

// Read battery SOC
uint16_t soc_raw;
status = power_module_handler_read_register(POWER_REG_BATTERY_SOC, &soc_raw);
if (status == HAL_STATUS_OK) {
    float soc = (float)soc_raw / 10.0f;  // Scale to percentage
    printf("Battery SOC: %.1f%%\n", soc);
}
```

### **2. Control Charging:**
```c
// Set charging voltage to 25.2V
uint16_t voltage_setpoint = (uint16_t)(25.2f * 10.0f);  // Scale to register value
hal_status_t status = power_module_handler_write_register(POWER_REG_V_SET, voltage_setpoint);

// Set charging current to 5A
uint16_t current_setpoint = (uint16_t)(5.0f * 10.0f);  // Scale to register value
status = power_module_handler_write_register(POWER_REG_I_SET, current_setpoint);

// Enable charging
status = power_module_handler_write_register(POWER_REG_CHARGE_REQUEST, 1);
```

### **3. Control Power Outputs:**
```c
// Enable 12V output
hal_status_t status = power_module_handler_write_register(POWER_REG_RL_12V, 1);

// Enable 5V output
status = power_module_handler_write_register(POWER_REG_RL_5V, 1);

// Enable 3.3V output
status = power_module_handler_write_register(POWER_REG_RL_3V3, 1);
```

### **4. Read Power Distribution:**
```c
// Read 12V output voltage
uint16_t v_12v_raw;
hal_status_t status = power_module_handler_read_register(POWER_REG_V_OUT_12V, &v_12v_raw);
if (status == HAL_STATUS_OK) {
    float v_12v = (float)v_12v_raw / 10.0f;
    printf("12V output: %.1fV\n", v_12v);
}

// Read 12V output current
uint16_t i_12v_raw;
status = power_module_handler_read_register(POWER_REG_I_OUT_12V, &i_12v_raw);
if (status == HAL_STATUS_OK) {
    float i_12v = (float)i_12v_raw / 10.0f;
    printf("12V current: %.1fA\n", i_12v);
}

// Read 12V output power
uint16_t p_12v_raw;
status = power_module_handler_read_register(POWER_REG_P_OUT_12V, &p_12v_raw);
if (status == HAL_STATUS_OK) {
    float p_12v = (float)p_12v_raw / 10.0f;
    printf("12V power: %.1fW\n", p_12v);
}
```

### **5. Auto-Detection:**
```c
// Auto-detect Power Module
hal_status_t status = power_module_handler_auto_detect(0x02, 1000);
if (status == HAL_STATUS_OK) {
    printf("Power Module detected at address 0x02\n");
    
    // Get module information
    uint16_t device_id, module_type, firmware_version, hardware_version;
    status = power_module_handler_get_module_info(&device_id, &module_type, 
                                                 &firmware_version, &hardware_version);
    if (status == HAL_STATUS_OK) {
        printf("Device ID: 0x%04X\n", device_id);
        printf("Module Type: 0x%04X\n", module_type);
        printf("Firmware Version: %d.%d\n", firmware_version >> 8, firmware_version & 0xFF);
        printf("Hardware Version: %d.%d\n", hardware_version >> 8, hardware_version & 0xFF);
    }
}
```

---

## 🧪 **TESTING**

### **1. Basic Communication Test:**
```bash
# Test RS485 communication
python3 tools/rs485_quick_test.py --address 0x02 --register 0x00F0

# Expected output:
# Device ID: 0x0002
# Module Type: 0x0002
# Communication: OK
```

### **2. Register Read/Write Test:**
```bash
# Test reading battery voltage
python3 tools/rs485_quick_test.py --address 0x02 --register 0x0000 --read

# Test writing charge voltage
python3 tools/rs485_quick_test.py --address 0x02 --register 0x0030 --write 252
```

### **3. Power Module Handler Test:**
```bash
# Run Power Module Handler test suite
cd firmware/tests/app
make test_power_module_handler
./test_power_module_handler

# Expected output:
# === Power Module Handler Test Suite ===
# Testing power module initialization...
#   Init with valid config: PASS
#   Init with NULL config: PASS
#   Double init: PASS
# ...
# === All tests completed ===
```

---

## ⚠️ **TROUBLESHOOTING**

### **Common Issues:**

#### **1. Communication Timeout:**
```bash
# Check RS485 wiring
ls -la /dev/ttyOHT485

# Test UART1 pins
python3 EMBED/test_uart1_pins_46_47.py

# Check baudrate settings
python3 EMBED/monitor_uart1.py
```

#### **2. Register Access Errors:**
```bash
# Check module address
python3 tools/rs485_quick_test.py --address 0x02 --register 0x00F0

# Check function code support
python3 tools/rs485_quick_test.py --address 0x02 --register 0x0104
```

#### **3. Data Scaling Issues:**
```c
// Correct scaling for voltage (V)
float voltage = (float)raw_value / 10.0f;

// Correct scaling for current (A)
float current = (float)raw_value / 10.0f;

// Correct scaling for power (W)
float power = (float)raw_value / 10.0f;

// Correct scaling for SOC (%)
float soc = (float)raw_value / 10.0f;
```

---

## 📚 **REFERENCES**

### **Implementation Files:**
- `firmware/include/power_module_handler.h` - Header file
- `firmware/src/app/power_module_handler.c` - Implementation
- `firmware/tests/app/test_power_module_handler.c` - Test suite

### **External References:**
- [STM32 Power Management](https://github.com/hiimshyy/stm32-power-management) - Reference implementation
- DalyBMS Protocol Documentation
- SK60X Charger Documentation
- INA219 Sensor Documentation
- Modbus RTU Protocol Specification

---

**🚨 Lưu ý:** Power Module integration phải tuân thủ đúng register map và scaling factors để đảm bảo compatibility với STM32 Power Management.