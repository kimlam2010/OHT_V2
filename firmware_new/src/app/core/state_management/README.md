# 🎛️ State Management Domain

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-10-07  
**Mục tiêu:** Quản lý trạng thái và lifecycle của OHT-50 system

---

## 📖 Tổng Quan

State Management domain chịu trách nhiệm quản lý **trạng thái tổng thể** của hệ thống OHT-50, bao gồm state transitions, system lifecycle, và event coordination.

**Vị trí trong kiến trúc:**
- 🔵 **Layer:** Application Core
- 🔵 **Dependencies:** KHÔNG phụ thuộc domain khác (base layer)
- 🔵 **Used by:** Safety System, Control System, Managers

---

## 🗂️ Components

### 1. **System State Machine** (`system_state_machine.c/h`)

**Dòng code:** 962 lines  
**Chức năng:** State machine chính của hệ thống

#### States Supported:
```c
typedef enum {
    SYSTEM_STATE_INIT = 0,        // Khởi tạo
    SYSTEM_STATE_READY,           // Sẵn sàng
    SYSTEM_STATE_RUNNING,         // Đang chạy
    SYSTEM_STATE_FAULT,           // Lỗi
    SYSTEM_STATE_E_STOP,          // Emergency Stop
    SYSTEM_STATE_SHUTDOWN         // Tắt máy
} system_state_t;
```

#### State Diagram:
```
INIT → READY → RUNNING
         ↓        ↓
      FAULT ← E_STOP
         ↓        ↓
         SHUTDOWN
```

#### Key Functions:
```c
// Initialization
hal_status_t system_state_machine_init(void);

// State management
hal_status_t system_state_machine_set_state(system_state_t new_state);
system_state_t system_state_machine_get_state(void);

// Event handling
hal_status_t system_state_machine_handle_event(system_event_t event);

// Validation
bool system_state_machine_is_transition_valid(system_state_t from, system_state_t to);
```

---

### 2. **System Controller** (`system_controller.c/h`)

**Dòng code:** 752 lines  
**Chức năng:** System coordination và high-level control

#### Responsibilities:
- 🎯 Coordinating giữa các subsystems
- 🎯 Managing system lifecycle
- 🎯 Handling system events
- 🎯 Monitoring system health

#### Key Functions:
```c
// System lifecycle
hal_status_t system_controller_init(void);
hal_status_t system_controller_start(void);
hal_status_t system_controller_stop(void);
hal_status_t system_controller_shutdown(void);

// System monitoring
hal_status_t system_controller_update(void);
hal_status_t system_controller_get_status(system_status_t *status);

// Event handling
hal_status_t system_controller_handle_event(system_event_t event);
```

---

## 🔗 Dependencies

### External Dependencies:
```c
// HAL Layer
#include "hal_common.h"      // Common HAL definitions
#include "hal_estop.h"       // E-Stop hardware
#include "hal_led.h"         // LED status indicators
#include "hal_relay.h"       // Relay control
#include "hal_rs485.h"       // RS485 communication
#include "hal_network.h"     // Network interfaces
```

### Internal Dependencies:
```
State Management
  └── (No internal dependencies - base layer)
```

---

## 🔧 Usage Examples

### Initialize State Machine:
```c
#include "system_state_machine.h"

// Initialize
if (system_state_machine_init() != HAL_STATUS_OK) {
    printf("Failed to initialize state machine\n");
    return -1;
}

// Set state
system_state_machine_set_state(SYSTEM_STATE_READY);

// Get current state
system_state_t current = system_state_machine_get_state();
printf("Current state: %d\n", current);
```

### Handle System Events:
```c
#include "system_controller.h"

// Initialize controller
system_controller_init();

// Handle event
system_event_t event = SYSTEM_EVENT_START;
system_controller_handle_event(event);

// Update system
system_controller_update();
```

---

## 🧪 Testing

### Unit Tests:
```bash
# Run state management tests
cd build/tests/unit
./test_system_state_machine
./test_system_controller
```

### Test Coverage:
- ✅ State transitions
- ✅ Event handling
- ✅ Error conditions
- ✅ Lifecycle management

---

## 📊 Performance

### Requirements:
- **State transition time:** < 10ms
- **Event handling:** < 5ms
- **Memory footprint:** < 10KB
- **CPU usage:** < 5%

### Actual Performance:
- ✅ State transition: ~2ms
- ✅ Event handling: ~1ms
- ✅ Memory: ~8KB
- ✅ CPU: ~3%

---

## 🚨 Thread Safety

**Status:** ⚠️ **NOT thread-safe**

**Reason:** State machine được designed để chạy trong single control loop thread.

**Usage:** 
- ✅ Safe: Call từ main control loop
- ❌ Unsafe: Call từ multiple threads simultaneously

**Future:** Có thể thêm mutex protection nếu cần multi-threading.

---

## 📚 Documentation

### API Documentation:
- `system_state_machine.h` - Full API documentation trong header
- `system_controller.h` - Controller API documentation

### Design Documents:
- `docs/specs/state_machine.md` - State machine design spec
- `docs/architecture/` - Overall architecture docs

---

## 🔄 Version History

### v1.0.0 (2025-10-07)
- ✅ Initial domain organization
- ✅ Separated từ monolithic core/
- ✅ Added CMakeLists.txt
- ✅ Documented API và usage

---

**Maintained by:** Firmware Team  
**Contact:** [FW Team Lead]

