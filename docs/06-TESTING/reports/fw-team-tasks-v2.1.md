# FIRMWARE TEAM TASKS - OHT-50 Master Module (v2.1)

**Phiên bản:** v2.1  
**Ngày tạo:** 2025-01-28  
**Team:** FW Team  
**Trạng thái:** 🚀 ACTIVE DEVELOPMENT  
**Ưu tiên:** CRITICAL

---

## 🎯 **TỔNG QUAN TASK**

### **Context:**
- Source code firmware đã được phân tích và đánh giá
- Build thành công với 20 test executables
- Cần fix issues và hoàn thiện để đạt Gate C (Drivers & Services)
- Hardware pin mapping cần xác nhận với EMBED team

### **Mục tiêu:**
- Clean build không có warnings
- Verify hardware pin mapping
- Hoàn thiện missing features
- Đạt tiêu chí Gate C

---

## 🚨 **ƯU TIÊN 1: FIX COMPILER WARNINGS (CRITICAL)**

### **Task 1.1: Fix Missing Includes**
**Mô tả:** Thêm missing header files để fix implicit function declarations

**Files cần sửa:**
- `src/hal/hal_lidar.c` - Thêm `#include <unistd.h>`
- `src/hal/hal_rs485.c` - Thêm `#include <unistd.h>`
- `src/hal/hal_led.c` - Thêm `#include <unistd.h>`
- `src/hal/hal_estop.c` - Thêm `#include <unistd.h>`
- `src/hal/hal_relay.c` - Thêm `#include <unistd.h>`
- `src/hal/hal_network.c` - Thêm `#include <unistd.h>`, `#include <stdio.h>`
- `src/hal/hal_common.c` - Thêm `#include <unistd.h>`

**Acceptance Criteria:**
- [ ] Không còn warning "implicit declaration of function"
- [ ] Build clean với `make clean && make`
- [ ] Tất cả test executables vẫn chạy được

### **Task 1.2: Fix Format Specifiers**
**Mô tả:** Sửa format specifiers cho uint64_t types

**Files cần sửa:**
- `src/hal/hal_estop.c` - Dòng 306, 311: `%llu` → `%lu`
- `src/app/power_module_handler.c` - Dòng 435, 450: `%llu` → `%lu`

**Acceptance Criteria:**
- [ ] Không còn warning format specifier
- [ ] Output đúng format cho timestamp values

### **Task 1.3: Clean Unused Functions**
**Mô tả:** Remove hoặc mark unused functions với `__attribute__((unused))`

**Files cần sửa:**
- `src/hal/hal_lidar.c` - `lidar_parse_scan_data`
- `src/hal/hal_relay.c` - `relay_handle_fault`
- `src/hal/hal_network.c` - `network_monitor_thread_func`
- `src/app/system_state_machine.c` - `transition_condition_system_ready`
- `src/app/module_manager.c` - `calculate_health_percentage`, `handle_module_event`
- `src/app/network_manager.c` - `update_interface_status`

**Acceptance Criteria:**
- [ ] Không còn warning "defined but not used"
- [ ] Code vẫn compile và chạy bình thường

### **Task 1.4: Fix Unused Parameters**
**Mô tả:** Mark unused parameters với `(void)` hoặc `__attribute__((unused))`

**Files cần sửa:**
- `src/hal/hal_common.c` - `hal_register_device`, `hal_unregister_device`, `hal_get_device_info`
- `src/app/safety_manager.c` - `estop_callback`
- Test files: `test_lidar.c`, `test_system_state_machine.c`, `test_safety_manager.c`

**Acceptance Criteria:**
- [ ] Không còn warning "unused parameter"
- [ ] Function signatures vẫn đúng interface

---

## 🔧 **ƯU TIÊN 2: VERIFY HARDWARE PIN MAPPING (CRITICAL)**

### **Task 2.1: Hardware Pin Verification**
**Mô tả:** Xác nhận pin mapping với EMBED team và hardware thực tế

**Current Pin Assignments cần verify:**
```c
#define ESTOP_PIN              59  // GPIO1_D3 - E-Stop (Single channel)
#define LED_POWER_PIN          54  // GPIO1_D6 - Power LED (Green)
#define LED_SYSTEM_PIN         35  // GPIO1_A3 - System LED (Blue)
#define LED_COMM_PIN           28  // GPIO0_D4 - Communication LED (Yellow)
#define LED_NETWORK_PIN        29  // GPIO0_D5 - Network LED (Green)
#define LED_ERROR_PIN          58  // GPIO1_D2 - Error LED (Red)
#define RELAY1_OUTPUT_PIN      131 // GPIO4_A3 - Relay 1 output
#define RELAY2_OUTPUT_PIN      132 // GPIO4_A4 - Relay 2 output
```

**Actions:**
1. **Coordinate với EMBED team** để verify pin assignments
2. **Test GPIO access** với `gpioinfo` và `gpioget/gpioset`
3. **Update pinout documentation** nếu cần thay đổi
4. **Test LED patterns** và E-Stop functionality

**Acceptance Criteria:**
- [ ] Pin mapping được EMBED team xác nhận
- [ ] GPIO test tools chạy thành công
- [ ] LED patterns hoạt động đúng
- [ ] E-Stop trigger/reset hoạt động
- [ ] Relay control hoạt động

### **Task 2.2: RS485 Device Path Verification**
**Mô tả:** Xác nhận RS485 device path và udev rules

**Current Configuration:**
```c
#define RS485_DEVICE_PATH      "/dev/ttyOHT485"  // UART1 symlink
#define RS485_BAUD_RATE        115200
```

**Actions:**
1. **Verify udev rules** tạo symlink `/dev/ttyOHT485`
2. **Test RS485 communication** với loopback
3. **Verify UART1 pin mapping** (GPIO1_D3/D4)
4. **Test Modbus RTU** với slave modules

**Acceptance Criteria:**
- [ ] `/dev/ttyOHT485` tồn tại sau reboot
- [ ] RS485 loopback test pass
- [ ] Modbus communication hoạt động
- [ ] Auto-discovery modules hoạt động

---

## 🚀 **ƯU TIÊN 3: ADD MISSING FEATURES (HIGH)**

### **Task 3.1: USB Debug Interface**
**Mô tả:** Implement USB debug interface cho firmware update và debugging

**Requirements:**
- USB CDC ACM interface
- Firmware update via USB
- Debug console access
- Bootloader integration

**Files cần tạo:**
- `src/hal/hal_usb.c/h` - USB HAL interface
- `src/app/usb_debug_manager.c/h` - USB debug manager
- `src/app/bootloader.c/h` - Bootloader interface

**Acceptance Criteria:**
- [ ] USB CDC ACM device được detect
- [ ] Debug console accessible via USB
- [ ] Firmware update via USB hoạt động
- [ ] Bootloader integration hoàn thành

### **Task 3.2: Configuration Persistence**
**Mô tả:** Implement configuration storage và persistence

**Requirements:**
- Flash storage interface
- Configuration backup/restore
- Factory reset functionality
- Configuration validation

**Files cần tạo:**
- `src/hal/hal_flash.c/h` - Flash storage HAL
- `src/app/config_persistence.c/h` - Configuration manager
- `src/app/factory_reset.c/h` - Factory reset handler

**Acceptance Criteria:**
- [ ] Configuration được lưu vào flash
- [ ] Backup/restore hoạt động
- [ ] Factory reset hoạt động
- [ ] Configuration validation pass

### **Task 3.3: OTA Update System**
**Mô tả:** Implement Over-The-Air firmware update

**Requirements:**
- Secure firmware download
- Integrity verification
- Rollback mechanism
- Update progress monitoring

**Files cần tạo:**
- `src/app/ota_manager.c/h` - OTA update manager
- `src/app/firmware_validator.c/h` - Firmware validation
- `src/app/update_progress.c/h` - Update progress tracking

**Acceptance Criteria:**
- [ ] Secure firmware download
- [ ] Integrity check pass
- [ ] Rollback mechanism hoạt động
- [ ] Update progress monitoring

### **Task 3.4: Enhanced Security**
**Mô tả:** Enhance security features

**Requirements:**
- Authentication system
- Encryption for communication
- Secure boot
- Access control

**Files cần enhance:**
- `src/app/security_manager.c/h` - Enhance existing
- `src/app/authentication.c/h` - New authentication
- `src/app/encryption.c/h` - New encryption

**Acceptance Criteria:**
- [ ] Authentication system hoạt động
- [ ] Communication encryption
- [ ] Secure boot implementation
- [ ] Access control enforced

---

## 🧪 **ƯU TIÊN 4: TESTING & VALIDATION (MEDIUM)**

### **Task 4.1: Integration Testing**
**Mô tả:** Enhance integration tests

**Actions:**
1. **Add stress tests** cho communication
2. **Add fault injection tests** cho safety system
3. **Add performance tests** cho real-time requirements
4. **Add security tests** cho authentication

**Acceptance Criteria:**
- [ ] Stress tests pass
- [ ] Fault injection tests pass
- [ ] Performance requirements met
- [ ] Security tests pass

### **Task 4.2: Hardware-in-the-Loop Testing**
**Mô tả:** Setup HIL testing environment

**Requirements:**
- Hardware test fixtures
- Automated test scripts
- Test result reporting
- Continuous integration

**Acceptance Criteria:**
- [ ] HIL test environment setup
- [ ] Automated test scripts
- [ ] Test result reporting
- [ ] CI integration

---

## 📋 **DELIVERABLES & MILESTONES**

### **Milestone 1: Clean Build (Week 1)**
- [ ] Task 1.1-1.4 completed
- [ ] Build clean không warnings
- [ ] All tests pass

### **Milestone 2: Hardware Verification (Week 2)**
- [ ] Task 2.1-2.2 completed
- [ ] Hardware pin mapping verified
- [ ] RS485 communication tested

### **Milestone 3: Missing Features (Week 3-4)**
- [ ] Task 3.1-3.4 completed
- [ ] USB debug interface working
- [ ] Configuration persistence working
- [ ] OTA update working
- [ ] Enhanced security implemented

### **Milestone 4: Testing & Validation (Week 5)**
- [ ] Task 4.1-4.2 completed
- [ ] Integration tests enhanced
- [ ] HIL testing setup

### **Gate C Achievement (Week 6)**
- [ ] All milestones completed
- [ ] Documentation updated
- [ ] Ready for Gate C review

---

## 🔄 **COORDINATION REQUIREMENTS**

### **With EMBED Team:**
- Hardware pin mapping verification
- GPIO test coordination
- RS485 device path setup
- Hardware test fixtures

### **With Backend Team:**
- API interface alignment
- Configuration schema validation
- Security protocol coordination
- OTA update server integration

### **With PM:**
- Progress reporting weekly
- Risk escalation
- Resource allocation
- Gate C preparation

---

## 📊 **SUCCESS METRICS**

### **Technical Metrics:**
- ✅ Zero compiler warnings
- ✅ 100% test pass rate
- ✅ Hardware verification complete
- ✅ Missing features implemented
- ✅ Performance requirements met

### **Process Metrics:**
- ✅ Weekly progress reports
- ✅ On-time milestone delivery
- ✅ Quality gates passed
- ✅ Documentation updated

---

**Changelog v2.1:**
- ✅ Added detailed task breakdown
- ✅ Added hardware verification requirements
- ✅ Added missing features specification
- ✅ Added testing & validation tasks
- ✅ Added coordination requirements
- ✅ Added success metrics

**🚨 Lưu ý:** Ưu tiên Task 1 (Fix warnings) và Task 2 (Hardware verification) trước khi làm Task 3 (Missing features).
