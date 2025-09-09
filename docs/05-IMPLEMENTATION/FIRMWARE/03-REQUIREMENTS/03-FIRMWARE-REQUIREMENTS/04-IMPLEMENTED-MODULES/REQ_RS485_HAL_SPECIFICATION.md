# RS485 HAL LAYER SPECIFICATION - OHT-50 Master Module

**Phiên bản:** v1.0.0  
**Ngày tạo:** 2025-01-28  
**Team:** EMBED + FW  
**Trạng thái:** ✅ IMPLEMENTED (cơ bản)  
**Mục tiêu:** Định nghĩa chi tiết yêu cầu kỹ thuật và chức năng của RS485 HAL layer

---

## 🎯 **TỔNG QUAN**

### **Mục tiêu:**
- Cung cấp Hardware Abstraction Layer (HAL) cho RS485/Modbus RTU communication
- Hỗ trợ giao tiếp với tất cả slave modules qua RS485 bus
- Đảm bảo thread-safe và reliable communication
- Cung cấp error handling và retry mechanism

### **Phạm vi:**
- RS485 serial communication
- Modbus RTU protocol implementation
- Device management và configuration
- Error handling và statistics

---

## 🔧 **KIẾN TRÚC KỸ THUẬT**

### **1. Hardware Interface**

#### **UART1 RS485 Configuration:**
```c
// Hardware Configuration
#define RS485_DEVICE_PATH        "/dev/ttyOHT485"  // udev symlink
#define RS485_BAUD_RATE          115200
#define RS485_DATA_BITS          8
#define RS485_STOP_BITS          1
#define RS485_PARITY             0  // No parity
#define RS485_BUFFER_SIZE        1024
#define MODBUS_FRAME_SIZE        256
```

#### **Pin Assignment:**
```c
// UART1 Pin Configuration (Orange Pi 5B)
UART1_TX: GPIO1_D1 (Pin 8)    // Transmit
UART1_RX: GPIO1_D0 (Pin 10)   // Receive
GND:      Ground              // Common ground
```

### **2. Software Architecture**

#### **Layer Structure:**
```
┌─────────────────────────────────────┐
│           Application Layer         │
│     (Module Manager, API)          │
├─────────────────────────────────────┤
│         Communication Manager       │
│      (Modbus RTU Protocol)         │
├─────────────────────────────────────┤
│           RS485 HAL Layer           │
│      (Hardware Abstraction)         │
├─────────────────────────────────────┤
│         Linux Serial Driver         │
│         (/dev/ttyS1)               │
├─────────────────────────────────────┤
│         Hardware (UART1)           │
│      (Orange Pi 5B RK3588)         │
└─────────────────────────────────────┘
```

---

## 📋 **YÊU CẦU CHỨC NĂNG**

### **1. Core HAL Functions**

#### **1.1 Initialization & Configuration**
```c
// RS485 HAL Core Functions
hal_status_t hal_rs485_init(const rs485_config_t *config);
hal_status_t hal_rs485_deinit(void);
hal_status_t hal_rs485_open(void);
hal_status_t hal_rs485_close(void);
```

#### **Yêu cầu:**
- ✅ **Configuration Validation:** Validate tất cả parameters
- ✅ **Device Path Management:** Support udev symlink `/dev/ttyOHT485`
- ✅ **Serial Port Configuration:** Baud rate, data bits, parity, stop bits
- ✅ **Thread Safety:** Mutex-based synchronization
- ✅ **State Management:** Track initialization và device status

#### **1.2 Data Transmission**
```c
// Transmission Functions
hal_status_t hal_rs485_transmit(const uint8_t *data, size_t length);
hal_status_t hal_rs485_receive(uint8_t *data, size_t *length);
hal_status_t hal_rs485_send_receive(const uint8_t *tx_data, size_t tx_length, 
                                   uint8_t *rx_data, size_t *rx_length);
```

#### **Yêu cầu:**
- ✅ **Reliable Transmission:** Error detection và correction
- ✅ **Retry Mechanism:** Automatic retry với exponential backoff
- ✅ **Timeout Handling:** Configurable timeouts
- ✅ **Buffer Management:** Proper buffer handling
- ✅ **Flow Control:** Prevent buffer overflow

### **2. Modbus RTU Protocol**

#### **2.1 Function Codes Support**
```c
// Modbus Function Codes
typedef enum {
    MODBUS_FC_READ_COILS = 0x01,                    // Read discrete outputs
    MODBUS_FC_READ_DISCRETE_INPUTS = 0x02,          // Read discrete inputs
    MODBUS_FC_READ_HOLDING_REGISTERS = 0x03,        // Read holding registers
    MODBUS_FC_READ_INPUT_REGISTERS = 0x04,          // Read input registers
    MODBUS_FC_WRITE_SINGLE_COIL = 0x05,             // Write single coil
    MODBUS_FC_WRITE_SINGLE_REGISTER = 0x06,         // Write single register
    MODBUS_FC_WRITE_MULTIPLE_COILS = 0x0F,          // Write multiple coils
    MODBUS_FC_WRITE_MULTIPLE_REGISTERS = 0x10       // Write multiple registers
} modbus_function_code_t;
```

#### **2.2 Modbus Functions**
```c
// Modbus RTU Functions
hal_status_t hal_modbus_read_holding_registers(uint8_t slave_id, uint16_t start_addr, 
                                              uint16_t quantity, uint16_t *data);
hal_status_t hal_modbus_write_single_register(uint8_t slave_id, uint16_t address, uint16_t value);
hal_status_t hal_modbus_read_coils(uint8_t slave_id, uint16_t start_addr, 
                                  uint16_t quantity, bool *coils);
hal_status_t hal_modbus_write_single_coil(uint8_t slave_id, uint16_t address, bool value);
```

#### **Yêu cầu:**
- ✅ **CRC Calculation:** Modbus CRC16 calculation
- ✅ **Frame Validation:** Proper frame format validation
- ✅ **Exception Handling:** Modbus exception codes
- ✅ **Slave ID Management:** Support 1-247 slave addresses
- ✅ **Register Access:** 16-bit register read/write

### **3. Error Handling & Recovery**

#### **3.1 Error Types**
```c
// RS485 Error Types
typedef enum {
    RS485_ERROR_NONE = 0,
    RS485_ERROR_TIMEOUT,           // Communication timeout
    RS485_ERROR_CRC_FAILED,        // CRC validation failed
    RS485_ERROR_FRAME_ERROR,       // Frame format error
    RS485_ERROR_BUFFER_OVERFLOW,   // Buffer overflow
    RS485_ERROR_DEVICE_ERROR,      // Hardware error
    RS485_ERROR_PARAMETER_ERROR    // Invalid parameters
} rs485_error_t;
```

#### **3.2 Retry Mechanism**
```c
// Retry Configuration
typedef struct {
    uint32_t max_retries;          // Maximum retry attempts
    uint32_t retry_delay_ms;       // Delay between retries
    uint32_t timeout_ms;           // Communication timeout
    bool enable_retry;             // Enable retry mechanism
} rs485_retry_config_t;
```

#### **Yêu cầu:**
- ✅ **Automatic Retry:** Configurable retry count và delay
- ✅ **Exponential Backoff:** Increasing delay between retries
- ✅ **Error Logging:** Detailed error logging
- ✅ **Recovery Procedures:** Automatic recovery mechanisms
- ✅ **Statistics Tracking:** Error statistics collection

### **4. Statistics & Monitoring**

#### **4.1 Communication Statistics**
```c
// RS485 Statistics
typedef struct {
    uint32_t total_transmissions;      // Total transmissions
    uint32_t successful_transmissions; // Successful transmissions
    uint32_t failed_transmissions;     // Failed transmissions
    uint32_t timeout_count;           // Timeout occurrences
    uint32_t crc_error_count;         // CRC error count
    uint32_t frame_error_count;       // Frame error count
    uint32_t retry_count;             // Total retries
    uint64_t total_response_time;     // Total response time
    uint32_t average_response_time;   // Average response time
} rs485_statistics_t;
```

#### **4.2 Device Information**
```c
// Device Information
typedef struct {
    hal_device_type_t device_type;    // Device type
    hal_device_status_t status;       // Device status
    rs485_status_t rs485_status;      // RS485 status
    char device_name[64];             // Device name
    char device_version[32];          // Device version
    uint64_t timestamp_us;           // Last update timestamp
    uint32_t error_count;            // Error count
    uint32_t warning_count;          // Warning count
} rs485_device_info_t;
```

#### **Yêu cầu:**
- ✅ **Real-time Statistics:** Live communication statistics
- ✅ **Performance Monitoring:** Response time tracking
- ✅ **Error Tracking:** Detailed error statistics
- ✅ **Health Monitoring:** Device health status
- ✅ **Historical Data:** Statistics history

---

## 🔒 **YÊU CẦU AN TOÀN**

### **1. Thread Safety**
- ✅ **Mutex Protection:** All operations protected by mutex
- ✅ **Atomic Operations:** Critical operations atomic
- ✅ **Deadlock Prevention:** Proper mutex ordering
- ✅ **Resource Management:** Proper resource cleanup

### **2. Error Recovery**
- ✅ **Graceful Degradation:** Continue operation on errors
- ✅ **Automatic Recovery:** Self-healing mechanisms
- ✅ **Error Isolation:** Prevent error propagation
- ✅ **Safe Defaults:** Safe default values

### **3. Resource Management**
- ✅ **Memory Management:** Proper memory allocation/deallocation
- ✅ **File Descriptor Management:** Proper FD handling
- ✅ **Buffer Management:** Prevent buffer overflow
- ✅ **Cleanup Procedures:** Proper cleanup on exit

---

## 📊 **YÊU CẦU HIỆU NĂNG**

### **1. Timing Requirements**
```c
// Performance Requirements
#define RS485_MIN_BAUD_RATE      9600    // Minimum baud rate
#define RS485_MAX_BAUD_RATE      115200  // Maximum baud rate
#define RS485_DEFAULT_TIMEOUT    1000    // Default timeout (ms)
#define RS485_MAX_RETRIES        3       // Maximum retry attempts
#define RS485_RETRY_DELAY        100     // Retry delay (ms)
```

### **2. Throughput Requirements**
- **Data Rate:** Up to 115200 bps
- **Frame Rate:** Up to 1000 frames/second
- **Response Time:** < 100ms typical
- **Error Rate:** < 0.1% under normal conditions

### **3. Reliability Requirements**
- **Uptime:** 99.9% availability
- **Error Recovery:** < 1 second recovery time
- **Data Integrity:** CRC validation on all frames
- **Fault Tolerance:** Continue operation with degraded performance

---

## 🔧 **CONFIGURATION MANAGEMENT**

### **1. Configuration Structure**
```c
// RS485 Configuration
typedef struct {
    char device_path[64];        // Device path
    uint32_t baud_rate;          // Baud rate
    uint8_t data_bits;           // Data bits
    uint8_t stop_bits;           // Stop bits
    uint8_t parity;              // Parity
    uint32_t timeout_ms;         // Timeout
    uint32_t retry_count;        // Retry count
} rs485_config_t;
```

### **2. Configuration Validation**
- ✅ **Parameter Validation:** Validate all configuration parameters
- ✅ **Range Checking:** Ensure parameters within valid ranges
- ✅ **Consistency Checking:** Ensure parameter consistency
- ✅ **Default Values:** Provide safe default values

### **3. Dynamic Configuration**
- ✅ **Runtime Configuration:** Support runtime configuration changes
- ✅ **Hot Reload:** Support configuration reload without restart
- ✅ **Validation:** Validate configuration before applying
- ✅ **Rollback:** Support configuration rollback

---

## 🧪 **TESTING REQUIREMENTS**

### **1. Unit Testing**
- ✅ **Function Testing:** Test all HAL functions
- ✅ **Error Testing:** Test error conditions
- ✅ **Boundary Testing:** Test boundary conditions
- ✅ **Performance Testing:** Test performance requirements

### **2. Integration Testing**
- ✅ **Hardware Integration:** Test with actual hardware
- ✅ **Protocol Testing:** Test Modbus RTU protocol
- ✅ **Multi-module Testing:** Test with multiple modules
- ✅ **Stress Testing:** Test under load conditions

### **3. Validation Testing**
- ✅ **Compliance Testing:** Test protocol compliance
- ✅ **Interoperability Testing:** Test with different modules
- ✅ **Reliability Testing:** Test long-term reliability
- ✅ **Safety Testing:** Test safety requirements

---

## 📚 **DOCUMENTATION REQUIREMENTS**

### **1. API Documentation**
- ✅ **Function Documentation:** Complete function documentation
- ✅ **Parameter Documentation:** Parameter descriptions
- ✅ **Return Value Documentation:** Return value descriptions
- ✅ **Error Code Documentation:** Error code descriptions

### **2. Usage Examples**
- ✅ **Basic Usage:** Basic usage examples
- ✅ **Advanced Usage:** Advanced usage examples
- ✅ **Error Handling:** Error handling examples
- ✅ **Best Practices:** Best practices documentation

### **3. Troubleshooting Guide**
- ✅ **Common Issues:** Common issues và solutions
- ✅ **Debug Procedures:** Debug procedures
- ✅ **Performance Tuning:** Performance tuning guide
- ✅ **Maintenance Procedures:** Maintenance procedures

---

## 🔄 **MAINTENANCE & SUPPORT**

### **1. Version Management**
- ✅ **Version Control:** Proper version control
- ✅ **Backward Compatibility:** Maintain backward compatibility
- ✅ **Migration Guide:** Provide migration guides
- ✅ **Deprecation Policy:** Clear deprecation policy

### **2. Support Requirements**
- ✅ **Technical Support:** Technical support procedures
- ✅ **Bug Reporting:** Bug reporting procedures
- ✅ **Feature Requests:** Feature request procedures
- ✅ **Documentation Updates:** Documentation update procedures

---

## 📋 **IMPLEMENTATION STATUS**

### **✅ COMPLETED FEATURES:**
- ✅ RS485 HAL initialization và configuration
- ✅ Serial port management
- ✅ Thread-safe operations
- ✅ Basic error handling
- ✅ Statistics collection
- ✅ Modbus RTU protocol support
- ✅ CRC calculation và validation
- ✅ Retry mechanism
- ✅ Device information tracking

### **⚠️ PARTIALLY IMPLEMENTED:**
- ⚠️ Advanced error recovery
- ⚠️ Performance optimization
- ⚠️ Dynamic configuration
- ⚠️ Comprehensive testing

### **❌ NOT IMPLEMENTED:**
- ❌ Advanced monitoring features
- ❌ Performance profiling
- ❌ Advanced debugging tools
- ❌ Comprehensive documentation

---

## 🎯 **KẾT LUẬN**

### **Trạng thái hiện tại:** ✅ **IMPLEMENTED (cơ bản)**

**Điểm mạnh:**
- ✅ Core functionality hoàn thiện
- ✅ Thread-safe implementation
- ✅ Basic error handling
- ✅ Modbus RTU protocol support
- ✅ Statistics collection

**Cần cải thiện:**
- ⚠️ Advanced error recovery
- ⚠️ Performance optimization
- ⚠️ Comprehensive testing
- ⚠️ Advanced monitoring

**Khuyến nghị:**
1. **Ưu tiên testing** - Complete comprehensive testing
2. **Performance optimization** - Optimize for high throughput
3. **Advanced monitoring** - Add advanced monitoring features
4. **Documentation** - Complete API documentation

---

**📅 Next Review:** Sau khi hoàn thành testing phase  
**👥 Responsible:** EMBED Team + FW Team  
**📊 Success Metrics:** 100% test coverage, < 1ms response time, 99.9% reliability
