# **CUSTOM MODULE INTEGRATION GUIDE**

## 📋 **TÀI LIỆU TÍCH HỢP MODULE CUSTOM RS485**

**Module Type:** Custom Development Module  
**Address:** 0x08+ (Configurable)  
**Version:** 1.0  
**Status:** 📋 Kế hoạch

---

## 🎯 **MỤC TIÊU**

### **Mục tiêu:**
- Kết nối Master Module (Orange Pi 5B) với Custom Module qua RS485
- Cung cấp flexible interface cho development và customization
- Hỗ trợ configurable GPIO, ADC, DAC, PWM
- Cho phép multiple communication protocols
- Hỗ trợ customizable firmware và development tools

### **Kiến trúc hệ thống:**
```
┌─────────────────┐    RS485    ┌─────────────────┐
│   Master Module │ ──────────── │  Custom Module │
│  (Orange Pi 5B) │             │   (Controller)  │
│                 │             │                 │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ HAL RS485   │ │             │ │ Custom      │ │
│ │ Driver      │ │             │ │ Controller  │ │
│ └─────────────┘ │             │ └─────────────┘ │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ Custom Mgr  │ │             │ │ Flexible    │ │
│ │ Application │ │             │ │ Interface   │ │
│ └─────────────┘ │             │ └─────────────┘ │
└─────────────────┘             │ ┌─────────────┐ │
                                │ │ GPIO/ADC/   │ │
                                │ │ DAC/PWM     │ │
                                └─────────────────┘
```

---

## 🔧 **HARDWARE SPECIFICATIONS**

### **1. Module Specifications:**
- **Module Type:** Custom Development Module
- **Address:** 0x08+ (Configurable via DIP switches)
- **Power Supply:** 24V DC
- **Current Consumption:** < 200mA
- **Operating Temperature:** -40°C to +85°C
- **Protection:** IP20

### **2. Microcontroller Specifications:**
- **Processor:** ARM Cortex-M4 (STM32F4 series)
- **Clock Speed:** 168 MHz
- **Memory:** 1MB Flash, 192KB RAM
- **GPIO:** 80+ configurable pins
- **ADC:** 16 channels, 12-bit resolution
- **DAC:** 2 channels, 12-bit resolution
- **PWM:** 16 channels, 16-bit resolution

### **3. Interface Specifications:**
- **GPIO:** 80+ pins (configurable as input/output)
- **ADC:** 16 channels (0-3.3V, 12-bit)
- **DAC:** 2 channels (0-3.3V, 12-bit)
- **PWM:** 16 channels (0-100%, 16-bit)
- **I2C:** 2 interfaces (up to 400kHz)
- **SPI:** 2 interfaces (up to 18MHz)
- **UART:** 4 interfaces (up to 6MHz)
- **CAN:** 2 interfaces (up to 1MHz)

### **4. Communication Features:**
- **RS485:** Modbus RTU protocol (primary)
- **WiFi:** 802.11 b/g/n (optional)
- **Bluetooth:** BLE 5.0 (optional)
- **Ethernet:** 10/100 Mbps (optional)
- **USB:** Device/Host (optional)

### **5. Development Features:**
- **Debug Interface:** SWD/JTAG
- **Bootloader:** Custom bootloader support
- **OTA Updates:** Over-the-air firmware updates
- **Configuration:** EEPROM for settings storage
- **Expansion:** Multiple expansion headers

---

## 🔌 **HARDWARE CONNECTION**

### **1. Power Connection:**
```
┌─────────────────────────────────────┐
│         Custom Module               │
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

### **3. GPIO Connections:**
```
┌─────────────────────────────────────┐
│         Custom Module               │
├─────────────────────────────────────┤
│ GPIO1 ─────────────────────────────┼─── External Device 1
│ GPIO2 ─────────────────────────────┼─── External Device 2
│ GPIO3 ─────────────────────────────┼─── External Device 3
│ GPIO4 ─────────────────────────────┼─── External Device 4
│ GPIO5 ─────────────────────────────┼─── External Device 5
│ GPIO6 ─────────────────────────────┼─── External Device 6
│ GPIO7 ─────────────────────────────┼─── External Device 7
│ GPIO8 ─────────────────────────────┼─── External Device 8
│ ... (up to 80+ GPIO pins)          │
└─────────────────────────────────────┘
```

### **4. Analog Connections:**
```
┌─────────────────────────────────────┐
│         Custom Module               │
├─────────────────────────────────────┤
│ ADC1 ──────────────────────────────┼─── Analog Input 1 (0-3.3V)
│ ADC2 ──────────────────────────────┼─── Analog Input 2 (0-3.3V)
│ ADC3 ──────────────────────────────┼─── Analog Input 3 (0-3.3V)
│ ADC4 ──────────────────────────────┼─── Analog Input 4 (0-3.3V)
│ ... (up to 16 ADC channels)        │
│ DAC1 ──────────────────────────────┼─── Analog Output 1 (0-3.3V)
│ DAC2 ──────────────────────────────┼─── Analog Output 2 (0-3.3V)
└─────────────────────────────────────┘
```

### **5. Communication Interfaces:**
```
┌─────────────────────────────────────┐
│         Custom Module               │
├─────────────────────────────────────┤
│ I2C1 (SDA/SCL) ────────────────────┼─── I2C Device 1
│ I2C2 (SDA/SCL) ────────────────────┼─── I2C Device 2
│ SPI1 (MOSI/MISO/SCK/CS) ───────────┼─── SPI Device 1
│ SPI2 (MOSI/MISO/SCK/CS) ───────────┼─── SPI Device 2
│ UART1 (TX/RX) ─────────────────────┼─── UART Device 1
│ UART2 (TX/RX) ─────────────────────┼─── UART Device 2
│ CAN1 (CANH/CANL) ──────────────────┼─── CAN Device 1
│ CAN2 (CANH/CANL) ──────────────────┼─── CAN Device 2
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

### **3. Custom Module Configuration:**
```json
{
  "custom_module": {
    "address": "0x08",
    "name": "Custom Module",
    "type": "custom_development",
    "version": "1.0",
    "enabled": true,
    "gpio_config": {
      "enabled": true,
      "pins": [
        {"id": 1, "mode": "output", "function": "led_control"},
        {"id": 2, "mode": "input", "function": "button_input"},
        {"id": 3, "mode": "pwm", "function": "motor_control"},
        {"id": 4, "mode": "adc", "function": "sensor_reading"},
        {"id": 5, "mode": "dac", "function": "analog_output"},
        {"id": 6, "mode": "i2c", "function": "sensor_interface"},
        {"id": 7, "mode": "spi", "function": "display_control"},
        {"id": 8, "mode": "uart", "function": "communication"}
      ]
    },
    "analog_config": {
      "enabled": true,
      "adc_channels": [
        {"id": 1, "enabled": true, "resolution": 12, "reference": 3.3},
        {"id": 2, "enabled": true, "resolution": 12, "reference": 3.3},
        {"id": 3, "enabled": true, "resolution": 12, "reference": 3.3},
        {"id": 4, "enabled": true, "resolution": 12, "reference": 3.3}
      ],
      "dac_channels": [
        {"id": 1, "enabled": true, "resolution": 12, "reference": 3.3},
        {"id": 2, "enabled": true, "resolution": 12, "reference": 3.3}
      ]
    },
    "pwm_config": {
      "enabled": true,
      "channels": [
        {"id": 1, "enabled": true, "frequency": 1000, "resolution": 16},
        {"id": 2, "enabled": true, "frequency": 5000, "resolution": 16},
        {"id": 3, "enabled": true, "frequency": 10000, "resolution": 16}
      ]
    },
    "communication_config": {
      "rs485_enabled": true,
      "i2c_enabled": true,
      "spi_enabled": true,
      "uart_enabled": true,
      "can_enabled": true
    },
    "development_config": {
      "debug_enabled": true,
      "bootloader_enabled": true,
      "ota_enabled": true,
      "logging_enabled": true
    }
  }
}
```

---

## 💻 **IMPLEMENTATION SOFTWARE**

### **1. HAL RS485 Driver (`hal_rs485.c`)**

#### **Core Functions:**
```c
// Custom Module specific functions
hal_status_t custom_module_init(void);
hal_status_t custom_module_set_gpio(uint8_t pin, bool state);
hal_status_t custom_module_get_gpio(uint8_t pin, bool *state);
hal_status_t custom_module_read_adc(uint8_t channel, uint16_t *value);
hal_status_t custom_module_set_dac(uint8_t channel, uint16_t value);
hal_status_t custom_module_set_pwm(uint8_t channel, uint16_t duty_cycle);
hal_status_t custom_module_i2c_write(uint8_t device_addr, uint8_t *data, size_t length);
hal_status_t custom_module_i2c_read(uint8_t device_addr, uint8_t *data, size_t length);
hal_status_t custom_module_spi_transfer(uint8_t *tx_data, uint8_t *rx_data, size_t length);
hal_status_t custom_module_uart_send(uint8_t *data, size_t length);
hal_status_t custom_module_uart_receive(uint8_t *data, size_t length);
hal_status_t custom_module_get_status(custom_status_t *status);
hal_status_t custom_module_update_firmware(const char *firmware_path);
```

### **2. Custom Manager Application (`custom_manager.c`)**

#### **Core Functions:**
```c
// Custom module management functions
custom_status_t custom_manager_get_status(void);
hal_status_t custom_manager_configure_gpio(gpio_config_t *config);
hal_status_t custom_manager_configure_analog(analog_config_t *config);
hal_status_t custom_manager_configure_pwm(pwm_config_t *config);
hal_status_t custom_manager_configure_communication(comm_config_t *config);
hal_status_t custom_manager_load_configuration(const char *config_path);
hal_status_t custom_manager_save_configuration(const char *config_path);
hal_status_t custom_manager_update_firmware(const char *firmware_path);
hal_status_t custom_manager_enter_bootloader(void);
hal_status_t custom_manager_calibrate_sensors(void);
```

---

## 📡 **MODBUS REGISTER MAP**

### **1. GPIO Control Registers (0x0000-0x00FF):**

#### **GPIO Status (0x0000-0x000F):**
```c
// GPIO status (80 pins)
#define CUSTOM_REG_GPIO_STATUS            0x0000  // Bit field (10 registers)

// GPIO pin states (individual pins)
#define CUSTOM_REG_GPIO1_STATE            0x000A  // 0=Low, 1=High

#define CUSTOM_REG_GPIO2_STATE            0x000B  // 0=Low, 1=High

#define CUSTOM_REG_GPIO3_STATE            0x000C  // 0=Low, 1=High

#define CUSTOM_REG_GPIO4_STATE            0x000D  // 0=Low, 1=High

#define CUSTOM_REG_GPIO5_STATE            0x000E  // 0=Low, 1=High

#define CUSTOM_REG_GPIO6_STATE            0x000F  // 0=Low, 1=High

#define CUSTOM_REG_GPIO7_STATE            0x0010  // 0=Low, 1=High

#define CUSTOM_REG_GPIO8_STATE            0x0011  // 0=Low, 1=High

// GPIO configuration
#define CUSTOM_REG_GPIO_CONFIG            0x0012  // Bit field

// GPIO direction (0=Input, 1=Output)
#define CUSTOM_REG_GPIO_DIRECTION         0x0013  // Bit field
```

### **2. ADC Data Registers (0x0100-0x01FF):**

#### **ADC Status (0x0100-0x010F):**
```c
// ADC status
#define CUSTOM_REG_ADC_STATUS             0x0100  // Bit field

// ADC channel 1 value (12-bit)
#define CUSTOM_REG_ADC1_VALUE             0x0101  // 16-bit unsigned

// ADC channel 2 value (12-bit)
#define CUSTOM_REG_ADC2_VALUE             0x0102  // 16-bit unsigned

// ADC channel 3 value (12-bit)
#define CUSTOM_REG_ADC3_VALUE             0x0103  // 16-bit unsigned

// ADC channel 4 value (12-bit)
#define CUSTOM_REG_ADC4_VALUE             0x0104  // 16-bit unsigned

// ADC channel 5 value (12-bit)
#define CUSTOM_REG_ADC5_VALUE             0x0105  // 16-bit unsigned

// ADC channel 6 value (12-bit)
#define CUSTOM_REG_ADC6_VALUE             0x0106  // 16-bit unsigned

// ADC channel 7 value (12-bit)
#define CUSTOM_REG_ADC7_VALUE             0x0107  // 16-bit unsigned

// ADC channel 8 value (12-bit)
#define CUSTOM_REG_ADC8_VALUE             0x0108  // 16-bit unsigned

// ADC reference voltage (mV)
#define CUSTOM_REG_ADC_REFERENCE          0x0109  // 16-bit unsigned

// ADC resolution
#define CUSTOM_REG_ADC_RESOLUTION         0x010A  // 16-bit unsigned
```

### **3. DAC Control Registers (0x0200-0x02FF):**

#### **DAC Status (0x0200-0x020F):**
```c
// DAC status
#define CUSTOM_REG_DAC_STATUS             0x0200  // Bit field

// DAC channel 1 value (12-bit)
#define CUSTOM_REG_DAC1_VALUE             0x0201  // 16-bit unsigned

// DAC channel 2 value (12-bit)
#define CUSTOM_REG_DAC2_VALUE             0x0202  // 16-bit unsigned

// DAC reference voltage (mV)
#define CUSTOM_REG_DAC_REFERENCE          0x0203  // 16-bit unsigned

// DAC resolution
#define CUSTOM_REG_DAC_RESOLUTION         0x0204  // 16-bit unsigned
```

### **4. PWM Control Registers (0x0300-0x03FF):**

#### **PWM Status (0x0300-0x030F):**
```c
// PWM status
#define CUSTOM_REG_PWM_STATUS             0x0300  // Bit field

// PWM channel 1 duty cycle (0-100%)
#define CUSTOM_REG_PWM1_DUTY_CYCLE        0x0301  // 16-bit unsigned

// PWM channel 1 frequency (Hz)
#define CUSTOM_REG_PWM1_FREQUENCY         0x0302  // 16-bit unsigned

// PWM channel 2 duty cycle (0-100%)
#define CUSTOM_REG_PWM2_DUTY_CYCLE        0x0303  // 16-bit unsigned

// PWM channel 2 frequency (Hz)
#define CUSTOM_REG_PWM2_FREQUENCY         0x0304  // 16-bit unsigned

// PWM channel 3 duty cycle (0-100%)
#define CUSTOM_REG_PWM3_DUTY_CYCLE        0x0305  // 16-bit unsigned

// PWM channel 3 frequency (Hz)
#define CUSTOM_REG_PWM3_FREQUENCY         0x0306  // 16-bit unsigned

// PWM resolution
#define CUSTOM_REG_PWM_RESOLUTION         0x0307  // 16-bit unsigned
```

### **5. Communication Registers (0x0400-0x04FF):**

#### **I2C Status (0x0400-0x040F):**
```c
// I2C status
#define CUSTOM_REG_I2C_STATUS             0x0400  // Bit field

// I2C1 device count
#define CUSTOM_REG_I2C1_DEVICE_COUNT      0x0401  // 16-bit unsigned

// I2C2 device count
#define CUSTOM_REG_I2C2_DEVICE_COUNT      0x0402  // 16-bit unsigned

// I2C error count
#define CUSTOM_REG_I2C_ERROR_COUNT        0x0403  // 16-bit unsigned
```

#### **SPI Status (0x0410-0x041F):**
```c
// SPI status
#define CUSTOM_REG_SPI_STATUS             0x0410  // Bit field

// SPI1 transfer count
#define CUSTOM_REG_SPI1_TRANSFER_COUNT    0x0411  // 16-bit unsigned

// SPI2 transfer count
#define CUSTOM_REG_SPI2_TRANSFER_COUNT    0x0412  // 16-bit unsigned

// SPI error count
#define CUSTOM_REG_SPI_ERROR_COUNT        0x0413  // 16-bit unsigned
```

#### **UART Status (0x0420-0x042F):**
```c
// UART status
#define CUSTOM_REG_UART_STATUS            0x0420  // Bit field

// UART1 baudrate
#define CUSTOM_REG_UART1_BAUDRATE         0x0421  // 16-bit unsigned

// UART2 baudrate
#define CUSTOM_REG_UART2_BAUDRATE         0x0422  // 16-bit unsigned

// UART error count
#define CUSTOM_REG_UART_ERROR_COUNT       0x0423  // 16-bit unsigned
```

### **6. System Status Registers (0x0500-0x05FF):**

#### **System Status (0x0500-0x050F):**
```c
// System status
#define CUSTOM_REG_SYSTEM_STATUS          0x0500  // Bit field

// Firmware version
#define CUSTOM_REG_FIRMWARE_VERSION       0x0501  // 16-bit unsigned

// Hardware version
#define CUSTOM_REG_HARDWARE_VERSION       0x0502  // 16-bit unsigned

// Uptime (hours)
#define CUSTOM_REG_UPTIME_HOURS           0x0503  // 16-bit unsigned

// Temperature (°C)
#define CUSTOM_REG_TEMPERATURE            0x0504  // 16-bit signed

// Memory usage (%)
#define CUSTOM_REG_MEMORY_USAGE           0x0505  // 16-bit unsigned

// Error count
#define CUSTOM_REG_ERROR_COUNT            0x0506  // 16-bit unsigned
```

### **7. Development Registers (0x0600-0x06FF):**

#### **Development Status (0x0600-0x060F):**
```c
// Development status
#define CUSTOM_REG_DEV_STATUS             0x0600  // Bit field

// Debug mode
#define CUSTOM_REG_DEBUG_MODE             0x0601  // 0=Normal, 1=Debug

// Bootloader status
#define CUSTOM_REG_BOOTLOADER_STATUS      0x0602  // 0=Application, 1=Bootloader

// OTA status
#define CUSTOM_REG_OTA_STATUS             0x0603  // 0=Idle, 1=Updating

// Log level
#define CUSTOM_REG_LOG_LEVEL              0x0604  // 0=Error, 1=Warning, 2=Info, 3=Debug

// Configuration version
#define CUSTOM_REG_CONFIG_VERSION         0x0605  // 16-bit unsigned
```

### **8. Fault Status Registers (0x0700-0x07FF):**

#### **Fault Status (0x0700-0x070F):**
```c
// Fault status register
#define CUSTOM_REG_FAULT_STATUS           0x0700  // Bit field

// GPIO fault
#define CUSTOM_REG_GPIO_FAULT             0x0701  // 0=OK, 1=Fault

// ADC fault
#define CUSTOM_REG_ADC_FAULT              0x0702  // 0=OK, 1=Fault

// DAC fault
#define CUSTOM_REG_DAC_FAULT              0x0703  // 0=OK, 1=Fault

// PWM fault
#define CUSTOM_REG_PWM_FAULT              0x0704  // 0=OK, 1=Fault

// Communication fault
#define CUSTOM_REG_COMM_FAULT             0x0705  // 0=OK, 1=Fault

// Memory fault
#define CUSTOM_REG_MEMORY_FAULT           0x0706  // 0=OK, 1=Fault

// Temperature fault
#define CUSTOM_REG_TEMP_FAULT             0x0707  // 0=OK, 1=Fault
```

---

## 🧪 **TESTING PROCEDURES**

### **1. Basic Communication Test:**
```python
#!/usr/bin/env python3
import serial
import time

def test_custom_communication():
    # Open serial port
    ser = serial.Serial(
        port='/dev/ttyS1',
        baudrate=115200,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=1
    )

    # Test communication with Custom module
    module_addr = 0x08
    print(f"Testing Custom module at address 0x{module_addr:02X}")

    # Read system status
    command = bytes([module_addr, 0x03, 0x05, 0x00, 0x00, 0x01])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')

    ser.write(command)
    response = ser.read(7)

    if len(response) == 7:
        print("  ✓ Custom module responded")
        print(f"  System status: 0x{response[3]:02X}")
    else:
        print("  ✗ Custom module no response")

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
    test_custom_communication()
```

### **2. GPIO Test:**
```python
def test_gpio():
    ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
    
    # Set GPIO1 to HIGH
    command = bytes([0x08, 0x06, 0x00, 0x0A, 0x00, 0x01])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(8)
    
    if len(response) == 8:
        print("  ✓ GPIO1 set to HIGH")
    
    time.sleep(1)
    
    # Set GPIO1 to LOW
    command = bytes([0x08, 0x06, 0x00, 0x0A, 0x00, 0x00])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(8)
    
    if len(response) == 8:
        print("  ✓ GPIO1 set to LOW")
    
    ser.close()
```

### **3. ADC Test:**
```python
def test_adc():
    ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
    
    # Read ADC channel 1
    command = bytes([0x08, 0x03, 0x01, 0x01, 0x00, 0x01])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(7)
    
    if len(response) == 7:
        adc_value = int.from_bytes(response[3:5], byteorder='big')
        voltage = (adc_value / 4095.0) * 3.3
        print(f"  ADC1: {adc_value} ({voltage:.2f}V)")
    
    ser.close()
```

### **4. DAC Test:**
```python
def test_dac():
    ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
    
    # Set DAC1 to 2V (approximately 2485 in 12-bit)
    dac_value = int((2.0 / 3.3) * 4095)
    command = bytes([0x08, 0x06, 0x02, 0x01, dac_value >> 8, dac_value & 0xFF])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(8)
    
    if len(response) == 8:
        print(f"  ✓ DAC1 set to {dac_value} ({2.0}V)")
    
    ser.close()
```

### **5. PWM Test:**
```python
def test_pwm():
    ser = serial.Serial('/dev/ttyS1', 115200, timeout=1)
    
    # Set PWM1 to 50% duty cycle at 1kHz
    duty_cycle = 50  # 50%
    frequency = 1000  # 1kHz
    
    # Set duty cycle
    command = bytes([0x08, 0x06, 0x03, 0x01, duty_cycle >> 8, duty_cycle & 0xFF])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(8)
    
    if len(response) == 8:
        print(f"  ✓ PWM1 duty cycle set to {duty_cycle}%")
    
    # Set frequency
    command = bytes([0x08, 0x06, 0x03, 0x02, frequency >> 8, frequency & 0xFF])
    crc = calculate_crc16(command)
    command += crc.to_bytes(2, byteorder='little')
    
    ser.write(command)
    response = ser.read(8)
    
    if len(response) == 8:
        print(f"  ✓ PWM1 frequency set to {frequency}Hz")
    
    ser.close()
```

---

## 📊 **PERFORMANCE METRICS**

### **1. Key Performance Indicators:**
- **GPIO Response Time:** < 1ms
- **ADC Accuracy:** ±0.1% full scale
- **DAC Accuracy:** ±0.1% full scale
- **PWM Frequency:** 1Hz to 1MHz
- **PWM Resolution:** 16-bit
- **I2C Speed:** Up to 400kHz
- **SPI Speed:** Up to 18MHz
- **UART Speed:** Up to 6MHz
- **Temperature Range:** -40°C to +85°C
- **Power Consumption:** < 200mA at 24V

### **2. Monitoring Metrics:**
- **GPIO States:** Real-time GPIO monitoring
- **Analog Values:** Real-time ADC/DAC monitoring
- **PWM Status:** Real-time PWM monitoring
- **Communication:** I2C/SPI/UART status
- **System Health:** Memory, temperature, uptime
- **Error Tracking:** Fault detection and reporting
- **Development:** Debug mode, bootloader status

---

## 🔧 **TROUBLESHOOTING**

### **1. Common Issues:**

#### **Communication Issues:**
- **Problem:** Module not responding
- **Solution:** Check RS485 wiring, baudrate, address
- **Diagnostic:** Use communication test script

#### **GPIO Issues:**
- **Problem:** GPIO not switching
- **Solution:** Check pin configuration, direction, voltage levels
- **Diagnostic:** Test GPIO with simple input/output

#### **ADC Issues:**
- **Problem:** ADC readings incorrect
- **Solution:** Check reference voltage, calibration, connections
- **Diagnostic:** Measure input voltage, verify ADC reading

#### **DAC Issues:**
- **Problem:** DAC output incorrect
- **Solution:** Check reference voltage, calibration, load
- **Diagnostic:** Measure output voltage, verify DAC setting

#### **PWM Issues:**
- **Problem:** PWM not working
- **Solution:** Check frequency, duty cycle, pin configuration
- **Diagnostic:** Measure PWM signal with oscilloscope

#### **Firmware Issues:**
- **Problem:** Module not booting
- **Solution:** Check firmware, bootloader, configuration
- **Diagnostic:** Use debug interface, check bootloader status

### **2. Error Codes:**
```c
// Error code definitions
#define CUSTOM_ERROR_NONE                 0x0000
#define CUSTOM_ERROR_COMMUNICATION        0x0001
#define CUSTOM_ERROR_GPIO                 0x0002
#define CUSTOM_ERROR_ADC                  0x0003
#define CUSTOM_ERROR_DAC                  0x0004
#define CUSTOM_ERROR_PWM                  0x0005
#define CUSTOM_ERROR_I2C                  0x0006
#define CUSTOM_ERROR_SPI                  0x0007
#define CUSTOM_ERROR_UART                 0x0008
#define CUSTOM_ERROR_MEMORY               0x0009
#define CUSTOM_ERROR_TEMPERATURE          0x000A
#define CUSTOM_ERROR_FIRMWARE             0x000B
```

---

## 🚀 **INTEGRATION VỚI SYSTEM**

### **1. System State Machine Integration:**

#### **Custom States:**
```c
typedef enum {
    CUSTOM_STATE_IDLE,
    CUSTOM_STATE_ACTIVE,
    CUSTOM_STATE_CONFIGURING,
    CUSTOM_STATE_UPDATING,
    CUSTOM_STATE_FAULT,
    CUSTOM_STATE_MAINTENANCE
} custom_state_t;
```

#### **State Transitions:**
```c
// Custom state machine integration
hal_status_t custom_manager_update_state(void) {
    custom_status_t status;
    hal_status_t result = custom_module_get_status(&status);
    
    if (result == HAL_STATUS_OK) {
        if (status.error_code != 0) {
            g_custom_manager.state = CUSTOM_STATE_FAULT;
        } else if (status.ota_status == CUSTOM_OTA_UPDATING) {
            g_custom_manager.state = CUSTOM_STATE_UPDATING;
        } else if (status.configuring) {
            g_custom_manager.state = CUSTOM_STATE_CONFIGURING;
        } else if (status.active) {
            g_custom_manager.state = CUSTOM_STATE_ACTIVE;
        } else {
            g_custom_manager.state = CUSTOM_STATE_IDLE;
        }
    }
    
    return HAL_STATUS_OK;
}
```

### **2. Configuration Management:**

#### **Dynamic Configuration:**
```c
// Dynamic configuration loading
hal_status_t custom_manager_load_dynamic_config(void) {
    // Load configuration from EEPROM
    custom_config_t config;
    hal_status_t result = custom_module_load_config(&config);
    
    if (result == HAL_STATUS_OK) {
        // Apply GPIO configuration
        for (int i = 0; i < config.gpio_count; i++) {
            custom_module_configure_gpio(&config.gpio_pins[i]);
        }
        
        // Apply analog configuration
        for (int i = 0; i < config.adc_count; i++) {
            custom_module_configure_adc(&config.adc_channels[i]);
        }
        
        // Apply PWM configuration
        for (int i = 0; i < config.pwm_count; i++) {
            custom_module_configure_pwm(&config.pwm_channels[i]);
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
- **Monthly:** Update firmware if needed
- **Quarterly:** Calibrate analog channels
- **Annually:** Full system diagnostics
- **As needed:** Configuration updates

### **2. Firmware Updates:**
```c
// Firmware update procedure
hal_status_t custom_module_update_firmware(const char *firmware_path) {
    // Enter bootloader mode
    hal_status_t result = custom_module_enter_bootloader();
    
    if (result == HAL_STATUS_OK) {
        // Upload new firmware
        result = custom_module_upload_firmware(firmware_path);
        
        if (result == HAL_STATUS_OK) {
            // Verify firmware
            result = custom_module_verify_firmware();
            
            if (result == HAL_STATUS_OK) {
                // Restart module
                custom_module_restart();
            }
        }
    }
    
    return result;
}
```

### **3. Calibration:**
```c
// ADC calibration
hal_status_t custom_module_calibrate_adc(void) {
    // Zero calibration
    custom_module_set_adc_reference(0);
    delay(1000);
    
    uint16_t zero_readings[16];
    for (int i = 0; i < 16; i++) {
        custom_module_read_adc(i, &zero_readings[i]);
    }
    
    // Full scale calibration
    custom_module_set_adc_reference(3300);
    delay(1000);
    
    uint16_t full_readings[16];
    for (int i = 0; i < 16; i++) {
        custom_module_read_adc(i, &full_readings[i]);
    }
    
    // Store calibration data
    custom_module_save_calibration(zero_readings, full_readings);
    
    return HAL_STATUS_OK;
}
```

---

**Changelog:**
- v1.0 (2025-01-27): Initial Custom Module integration guide
