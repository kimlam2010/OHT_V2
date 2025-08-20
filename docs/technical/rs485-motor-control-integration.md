# HƯỚNG DẪN KẾT NỐI VÀ ĐIỀU KHIỂN MODULE ĐỘNG CƠ QUA RS485
## OHT-50 Master Module - Motor Control Integration

**Phiên bản:** v1.0  
**Ngày tạo:** $(date +%Y-%m-%d)  
**Team:** FW Team  
**Trạng thái:** Implementation Guide  

---

## 🎯 **TỔNG QUAN**

### **Mục tiêu:**
- Kết nối Master Module (Orange Pi 5B) với module động cơ qua RS485
- Điều khiển động cơ di chuyển (vận tốc, hướng, vị trí)
- Giám sát trạng thái động cơ (vị trí, vận tốc, lỗi)
- Đảm bảo giao tiếp ổn định và an toàn

### **Kiến trúc hệ thống:**
```
┌─────────────────┐    RS485    ┌─────────────────┐
│   Master Module │ ──────────── │  Motor Module   │
│  (Orange Pi 5B) │             │   (Controller)  │
│                 │             │                 │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ HAL RS485   │ │             │ │ Motor Ctrl  │ │
│ │ Driver      │ │             │ │ Logic       │ │
│ └─────────────┘ │             │ └─────────────┘ │
│ ┌─────────────┐ │             │ ┌─────────────┐ │
│ │ Motor Mgr   │ │             │ │ Motor       │ │
│ │ Application │ │             │ │ Hardware    │ │
│ └─────────────┘ │             │ └─────────────┘ │
└─────────────────┘             └─────────────────┘
```

---

## 🔧 **PHẦN CỨNG VÀ KẾT NỐI**

### **1. Thông số kỹ thuật RS485:**

#### **Master Module (Orange Pi 5B):**
- **UART Interface:** UART1 (`/dev/ttyS1`)
- **RS485 Transceiver:** MAX485 hoặc tương đương
- **Baud Rate:** 115200 bps (có thể điều chỉnh)
- **Data Bits:** 8
- **Stop Bits:** 1
- **Parity:** None
- **Flow Control:** None

#### **Motor Module:**
- **Protocol:** Modbus RTU hoặc custom protocol
- **Slave Address:** 1 (có thể cấu hình)
- **Response Time:** < 100ms
- **Error Detection:** CRC-16

### **2. Kết nối phần cứng:**

#### **Wiring Diagram:**
```
Orange Pi 5B          RS485 Bus          Motor Module
┌─────────────┐      ┌─────────┐        ┌─────────────┐
│ UART1_TX    │──────│   A+    │────────│   A+        │
│ (GPIO)      │      │         │        │             │
│             │      │         │        │             │
│ UART1_RX    │──────│   B-    │────────│   B-        │
│ (GPIO)      │      │         │        │             │
│             │      │         │        │             │
│             │──────│         │────────│             │
│ (Control)   │      │         │        │             │
│             │      │         │        │             │
│ GND         │──────│   GND   │────────│   GND       │
└─────────────┘      └─────────┘        └─────────────┘
```

#### **Termination:**
- **120Ω Resistor:** Giữa A+ và B- ở cuối bus
- **Bias Resistors:** 4.7kΩ pull-up cho A+, pull-down cho B-
- **Cable:** Shielded twisted pair (STP)

### **3. Cấu hình GPIO:**

#### **Pin Assignment:**
```bash
# UART1 Configuration
UART1_TX: GPIO1_A0 (Pin 8)
UART1_RX: GPIO1_A1 (Pin 10)


# udev rule để tạo symlink
echo 'KERNEL=="ttyS1", SYMLINK+="ttyOHT485"' > /etc/udev/rules.d/99-oht-rs485.rules
```

---

## 💻 **IMPLEMENTATION SOFTWARE**

### **1. HAL RS485 Driver (`hal_rs485.c`)**

#### **Core Functions:**
```c
// Initialization
hal_status_t hal_rs485_init(const hal_rs485_config_t *config);
hal_status_t hal_rs485_deinit(void);

// Communication
hal_status_t hal_rs485_send_frame(const uint8_t *data, uint16_t length);
hal_status_t hal_rs485_receive_frame(uint8_t *data, uint16_t max_length, uint16_t *received_length);
hal_status_t hal_rs485_transceive(const uint8_t *tx_data, uint16_t tx_length, 
                                 uint8_t *rx_data, uint16_t max_rx_length, 
                                 uint16_t *rx_length);

// Control
hal_status_t hal_rs485_set_baud_rate(uint32_t baud_rate);
hal_status_t hal_rs485_set_timeout(uint32_t timeout_ms);
hal_status_t hal_rs485_flush_rx_buffer(void);
hal_status_t hal_rs485_flush_tx_buffer(void);

// Status
hal_status_t hal_rs485_get_status(hal_rs485_status_t *status);
hal_status_t hal_rs485_get_statistics(hal_rs485_stats_t *stats);
```

#### **Configuration Structure:**
```c
typedef struct {
    const char *device_path;        // "/dev/ttyOHT485"
    uint32_t baud_rate;            // 115200
    uint8_t data_bits;             // 8
    uint8_t stop_bits;             // 1
    hal_uart_parity_t parity;      // HAL_UART_PARITY_NONE
    uint32_t timeout_ms;           // 1000ms
    uint32_t retry_count;          // 3
    bool enable_flow_control;      // false
    uint32_t rx_buffer_size;       // 1024 bytes
    uint32_t tx_buffer_size;       // 1024 bytes
} hal_rs485_config_t;
```

### **2. Motor Manager Application (`motor_manager.c`)**

#### **Core Functions:**
```c
// Initialization
hal_status_t motor_manager_init(const motor_mgr_config_t *config);
hal_status_t motor_manager_deinit(void);

// Motor Control
hal_status_t motor_manager_set_speed(uint8_t motor_id, float speed_rpm);
hal_status_t motor_manager_set_position(uint8_t motor_id, float position_deg);
hal_status_t motor_manager_set_direction(uint8_t motor_id, motor_direction_t direction);
hal_status_t motor_manager_start_motor(uint8_t motor_id);
hal_status_t motor_manager_stop_motor(uint8_t motor_id);
hal_status_t motor_manager_emergency_stop(uint8_t motor_id);

// Status & Monitoring
hal_status_t motor_manager_get_motor_status(uint8_t motor_id, motor_status_t *status);
hal_status_t motor_manager_get_motor_position(uint8_t motor_id, float *position_deg);
hal_status_t motor_manager_get_motor_speed(uint8_t motor_id, float *speed_rpm);
hal_status_t motor_manager_get_motor_error(uint8_t motor_id, motor_error_t *error);

// Configuration
hal_status_t motor_manager_set_motor_config(uint8_t motor_id, const motor_config_t *config);
hal_status_t motor_manager_get_motor_config(uint8_t motor_id, motor_config_t *config);
hal_status_t motor_manager_calibrate_motor(uint8_t motor_id);
```

#### **Motor Control Protocol:**

##### **Command Frame Structure:**
```
┌─────────┬─────────┬─────────┬─────────┬─────────┬─────────┐
│ Address │ Command │ Data    │ Data    │ CRC_L   │ CRC_H   │
│ (1 byte)│ (1 byte)│ (N bytes)│ (N bytes)│ (1 byte)│ (1 byte)│
└─────────┴─────────┴─────────┴─────────┴─────────┴─────────┘
```

##### **Command Codes:**
```c
#define MOTOR_CMD_SET_SPEED      0x01
#define MOTOR_CMD_SET_POSITION   0x02
#define MOTOR_CMD_SET_DIRECTION  0x03
#define MOTOR_CMD_START          0x04
#define MOTOR_CMD_STOP           0x05
#define MOTOR_CMD_EMERGENCY_STOP 0x06
#define MOTOR_CMD_GET_STATUS     0x07
#define MOTOR_CMD_GET_POSITION   0x08
#define MOTOR_CMD_GET_SPEED      0x09
#define MOTOR_CMD_GET_ERROR      0x0A
#define MOTOR_CMD_CALIBRATE      0x0B
#define MOTOR_CMD_RESET          0x0C
```

##### **Response Frame Structure:**
```
┌─────────┬─────────┬─────────┬─────────┬─────────┬─────────┐
│ Address │ Command │ Status  │ Data    │ Data    │ CRC_L   │ CRC_H   │
│ (1 byte)│ (1 byte)│ (1 byte)│ (N bytes)│ (N bytes)│ (1 byte)│ (1 byte)│
└─────────┴─────────┴─────────┴─────────┴─────────┴─────────┴─────────┘
```

### **3. Implementation Example:**

#### **Motor Manager Implementation:**
```c
#include "motor_manager.h"
#include "hal_rs485.h"

static motor_mgr_t g_motor_manager = {0};

hal_status_t motor_manager_init(const motor_mgr_config_t *config) {
    if (config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Initialize RS485 HAL
    hal_rs485_config_t rs485_config = {
        .device_path = "/dev/ttyOHT485",
        .baud_rate = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = HAL_UART_PARITY_NONE,
        .timeout_ms = 1000,
        .retry_count = 3,
        .enable_flow_control = false,
        .rx_buffer_size = 1024,
        .tx_buffer_size = 1024
    };
    
    hal_status_t status = hal_rs485_init(&rs485_config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Initialize motor manager
    memcpy(&g_motor_manager.config, config, sizeof(motor_mgr_config_t));
    g_motor_manager.initialized = true;
    
    return HAL_STATUS_OK;
}

hal_status_t motor_manager_set_speed(uint8_t motor_id, float speed_rpm) {
    if (!g_motor_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // Prepare command frame
    uint8_t command_frame[8];
    command_frame[0] = motor_id;                    // Address
    command_frame[1] = MOTOR_CMD_SET_SPEED;         // Command
    command_frame[2] = (uint8_t)(speed_rpm * 10);   // Speed (0.1 RPM resolution)
    command_frame[3] = (uint8_t)((uint16_t)(speed_rpm * 10) >> 8);
    
    // Calculate CRC
    uint16_t crc = calculate_crc16(command_frame, 4);
    command_frame[4] = (uint8_t)(crc & 0xFF);
    command_frame[5] = (uint8_t)(crc >> 8);
    
    // Send command
    hal_status_t status = hal_rs485_send_frame(command_frame, 6);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Wait for response
    uint8_t response_frame[8];
    uint16_t response_length;
    status = hal_rs485_receive_frame(response_frame, 8, &response_length);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Validate response
    if (response_length < 5 || response_frame[0] != motor_id || 
        response_frame[1] != MOTOR_CMD_SET_SPEED) {
        return HAL_STATUS_COMMUNICATION_ERROR;
    }
    
    // Check status
    if (response_frame[2] != 0x00) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}

hal_status_t motor_manager_get_motor_status(uint8_t motor_id, motor_status_t *status) {
    if (!g_motor_manager.initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Prepare command frame
    uint8_t command_frame[5];
    command_frame[0] = motor_id;                    // Address
    command_frame[1] = MOTOR_CMD_GET_STATUS;        // Command
    
    // Calculate CRC
    uint16_t crc = calculate_crc16(command_frame, 2);
    command_frame[2] = (uint8_t)(crc & 0xFF);
    command_frame[3] = (uint8_t)(crc >> 8);
    
    // Send command
    hal_status_t result = hal_rs485_send_frame(command_frame, 4);
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    // Wait for response
    uint8_t response_frame[8];
    uint16_t response_length;
    result = hal_rs485_receive_frame(response_frame, 8, &response_length);
    if (result != HAL_STATUS_OK) {
        return result;
    }
    
    // Parse response
    if (response_length >= 7) {
        status->motor_id = response_frame[0];
        status->running = (response_frame[3] & 0x01) != 0;
        status->direction = (motor_direction_t)(response_frame[3] >> 1);
        status->speed_rpm = (float)((uint16_t)response_frame[4] | 
                                   ((uint16_t)response_frame[5] << 8)) / 10.0f;
        status->error_code = response_frame[6];
    }
    
    return HAL_STATUS_OK;
}
```

---

## 🔧 **CẤU HÌNH VÀ TROUBLESHOOTING**

### **1. Cấu hình hệ thống:**

#### **Device Tree Overlay:**
```dts
// /boot/overlay-user/oht-rs485.dts
/dts-v1/;
/plugin/;

&uart1 {
    status = "okay";
    pinctrl-names = "default";
    pinctrl-0 = <&uart1m0_xfer>;
};

&pinctrl {
    uart1 {
        uart1m0_xfer: uart1m0-xfer {
            rockchip,pins = <1 RK_PA0 1 &pcfg_pull_up>,
                           <1 RK_PA1 1 &pcfg_pull_up>;
        };
    };
    
    rs485_de {
        rs485_de_pin: rs485-de-pin {
            rockchip,pins = <1 RK_PD3 RK_FUNC_GPIO &pcfg_pull_up>;
        };
    };
};
```

#### **udev Rules:**
```bash
# /etc/udev/rules.d/99-oht-rs485.rules
KERNEL=="ttyS1", SYMLINK+="ttyOHT485", MODE="0666"
```

### **2. Testing và Validation:**

#### **RS485 Loopback Test:**
```c
hal_status_t test_rs485_loopback(void) {
    hal_rs485_config_t config = {
        .device_path = "/dev/ttyOHT485",
        .baud_rate = 115200,
        .timeout_ms = 1000
    };
    
    hal_status_t status = hal_rs485_init(&config);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Test data
    uint8_t test_data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    uint8_t received_data[10];
    uint16_t received_length;
    
    // Send test data
    status = hal_rs485_send_frame(test_data, sizeof(test_data));
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Receive data (loopback)
    status = hal_rs485_receive_frame(received_data, 10, &received_length);
    if (status != HAL_STATUS_OK) {
        return status;
    }
    
    // Verify data
    if (received_length != sizeof(test_data) || 
        memcmp(test_data, received_data, sizeof(test_data)) != 0) {
        return HAL_STATUS_ERROR;
    }
    
    return HAL_STATUS_OK;
}
```

#### **Motor Communication Test:**
```c
hal_status_t test_motor_communication(uint8_t motor_id) {
    motor_status_t status;
    
    // Test get status
    hal_status_t result = motor_manager_get_motor_status(motor_id, &status);
    if (result != HAL_STATUS_OK) {
        printf("Failed to get motor status: %d\n", result);
        return result;
    }
    
    printf("Motor %d Status:\n", motor_id);
    printf("  Running: %s\n", status.running ? "Yes" : "No");
    printf("  Direction: %d\n", status.direction);
    printf("  Speed: %.1f RPM\n", status.speed_rpm);
    printf("  Error Code: 0x%02X\n", status.error_code);
    
    return HAL_STATUS_OK;
}
```

### **3. Troubleshooting:**

#### **Common Issues:**
1. **No Communication:**
   - Kiểm tra wiring (A+, B-, GND)
   - Kiểm tra baud rate và cấu hình UART
   - Kiểm tra termination resistors

2. **CRC Errors:**
   - Kiểm tra cable quality
   - Giảm baud rate
   - Kiểm tra noise và interference

3. **Timeout Errors:**
   - Tăng timeout value
   - Kiểm tra motor module response time
   - Kiểm tra cable length

4. **Data Corruption:**
   - Kiểm tra grounding
   - Sử dụng shielded cable
   - Kiểm tra power supply noise

#### **Debug Commands:**
```bash
# Check UART device
ls -la /dev/ttyOHT485

# Test UART communication
stty -F /dev/ttyOHT485 115200 raw -echo -echoe -echok

# Monitor RS485 traffic
cat /dev/ttyOHT485 | hexdump -C

# Check GPIO for DE control
gpioinfo | grep -A 5 "GPIO1_D3"
```

---

## 📊 **INTEGRATION VỚI SYSTEM**

### **1. System State Machine Integration:**

#### **Motor States:**
```c
typedef enum {
    MOTOR_STATE_IDLE,
    MOTOR_STATE_RUNNING,
    MOTOR_STATE_STOPPING,
    MOTOR_STATE_ERROR,
    MOTOR_STATE_CALIBRATING
} motor_state_t;
```

#### **State Transitions:**
```c
// Motor state machine integration
hal_status_t motor_manager_update_state(void) {
    for (int i = 0; i < g_motor_manager.config.max_motors; i++) {
        motor_status_t status;
        hal_status_t result = motor_manager_get_motor_status(i, &status);
        
        if (result == HAL_STATUS_OK) {
            // Update motor state based on status
            if (status.error_code != 0) {
                g_motor_manager.motors[i].state = MOTOR_STATE_ERROR;
            } else if (status.running) {
                g_motor_manager.motors[i].state = MOTOR_STATE_RUNNING;
            } else {
                g_motor_manager.motors[i].state = MOTOR_STATE_IDLE;
            }
        }
    }
    
    return HAL_STATUS_OK;
}
```

### **2. Performance Monitoring Integration:**

#### **Motor Performance Metrics:**
```c
// Add motor metrics to Performance Manager
hal_status_t motor_manager_register_performance_metrics(void) {
    performance_manager_register_metric(PERFORMANCE_METRIC_MOTOR_SPEED, 
                                      "Motor Speed", 80.0f, 90.0f, 95.0f);
    performance_manager_register_metric(PERFORMANCE_METRIC_MOTOR_POSITION, 
                                      "Motor Position", 0.0f, 0.0f, 0.0f);
    performance_manager_register_metric(PERFORMANCE_METRIC_MOTOR_ERROR_RATE, 
                                      "Motor Error Rate", 5.0f, 10.0f, 20.0f);
    
    return HAL_STATUS_OK;
}
```

### **3. Diagnostics Integration:**

#### **Motor Health Monitoring:**
```c
// Add motor components to Diagnostics Manager
hal_status_t motor_manager_register_diagnostics_components(void) {
    diagnostics_manager_register_component(DIAGNOSTICS_COMPONENT_MOTOR_1, "Motor 1");
    diagnostics_manager_register_component(DIAGNOSTICS_COMPONENT_MOTOR_2, "Motor 2");
    diagnostics_manager_register_component(DIAGNOSTICS_COMPONENT_RS485, "RS485 Communication");
    
    return HAL_STATUS_OK;
}
```

---

## 🚀 **NEXT STEPS**

### **1. Implementation Priority:**
1. **HAL RS485 Driver:** Implement basic RS485 communication
2. **Motor Manager:** Implement motor control functions
3. **Testing Framework:** Create comprehensive test suite
4. **Integration:** Integrate với existing system modules
5. **Optimization:** Performance và reliability optimization

### **2. Testing Strategy:**
- **Unit Tests:** Individual function testing
- **Integration Tests:** Motor communication testing
- **System Tests:** End-to-end motor control testing
- **Performance Tests:** Communication performance testing
- **Reliability Tests:** Long-term stability testing

### **3. Documentation:**
- **API Documentation:** Complete function documentation
- **Integration Guide:** System integration procedures
- **Troubleshooting Guide:** Common issues và solutions
- **Performance Guide:** Optimization strategies

---

**Tài liệu được tạo bởi: FW Team**  
**Ngày:** $(date +%Y-%m-%d)  
**Phiên bản:** v1.1  
**Trạng thái:** Implementation Guide

**Changelog:**
- v1.0 (2025-01-27): Initial motor control integration guide
- v1.1 (2025-01-27): Removed GPIO_DE pin (not needed for RS485)