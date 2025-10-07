# 🗂️ Core Domain Quick Index

**Phiên bản:** 1.0.1  
**Ngày:** 2025-10-07

---

## 📚 Documentation Quick Links

### Main Documentation:
- 📖 [Core Overview](README.md) - Architecture và overview
- 📖 [Migration Log](../../../MIGRATION_LOG_v1.0.1.md) - v1.0.1 migration details

### Domain Documentation:
- 🎛️ [State Management](state_management/README.md) - System states & lifecycle
- 🛡️ [Safety System](safety/README.md) - Safety-critical monitoring
- ⚙️ [Control System](control/README.md) - Motion control

---

## 🎯 Quick Navigation

### By Domain:

#### 🎛️ State Management
```
📁 state_management/
├── 📄 system_state_machine.c (962 lines) - State transitions
├── 📄 system_state_machine.h
├── 📄 system_controller.c (752 lines) - System coordination
├── 📄 system_controller.h
├── ⚙️ CMakeLists.txt
└── 📖 README.md
```

#### 🛡️ Safety System
```
📁 safety/
├── 📄 safety_monitor.c (1,762 lines) - Main safety monitor
├── 📄 safety_monitor.h
├── 📄 critical_module_detector.c (985 lines) - Critical detection
├── 📄 critical_module_detector.h
├── 📄 graduated_response_system.c (936 lines) - Graduated response
├── 📄 graduated_response_system.h
├── 📄 safety_rs485_integration.c (239 lines) - RS485 safety
├── 📄 safety_rs485_integration.h
├── ⚙️ CMakeLists.txt
└── 📖 README.md
```

#### ⚙️ Control System
```
📁 control/
├── 📄 control_loop.c (664 lines) - PID control
├── 📄 control_loop.h
├── 📄 estimator_1d.c (36 lines) - Position estimator
├── 📄 estimator_1d.h
├── ⚙️ CMakeLists.txt
└── 📖 README.md
```

---

## 🔍 Find by Function

### State Management Functions:
```c
// Initialization
system_state_machine_init()
system_controller_init()

// State operations
system_state_machine_set_state()
system_state_machine_get_state()
system_state_machine_handle_event()

// System control
system_controller_start()
system_controller_stop()
system_controller_update()
```

### Safety Functions:
```c
// Safety monitoring
safety_monitor_init()
safety_monitor_update()
safety_monitor_check_safety()

// E-Stop
safety_monitor_trigger_estop()
safety_monitor_is_estop_active()
safety_monitor_reset_estop()

// Critical detection
critical_module_get_health_assessment()
critical_module_determine_response_level()

// Graduated response
graduated_response_set_level()
graduated_response_update_led_patterns()
```

### Control Functions:
```c
// Control loop
control_loop_init()
control_loop_update()
control_loop_set_velocity_target()
control_loop_stop()
control_loop_emergency_stop()

// Estimation
estimator_1d_update()
estimator_1d_get_state()
estimator_1d_reset()
```

---

## 📊 Statistics

### Code Metrics:
| Domain | Files | C Lines | H Lines | Total | % of Core |
|--------|-------|---------|---------|-------|-----------|
| State Management | 4 | 1,346 | 368 | 1,714 | 27% |
| Safety System | 8 | 3,157 | 766 | 3,923 | 62% |
| Control System | 4 | 565 | 135 | 700 | 11% |
| **Total** | **16** | **5,068** | **1,269** | **6,337** | **100%** |

### Complexity:
| Domain | Avg Lines/File | Complexity | Criticality |
|--------|----------------|------------|-------------|
| State Management | 429 | Medium | High |
| Safety System | 490 | High | **CRITICAL** 🔴 |
| Control System | 175 | Low-Medium | Medium |

---

## 🔗 Integration Guide

### For Other Modules:

#### Using Core in Your Code:
```c
// In your .c file
#include "system_state_machine.h"  // State management
#include "safety_monitor.h"        // Safety monitoring
#include "control_loop.h"          // Control functions

// In your CMakeLists.txt
target_link_libraries(your_module
    app_core  # Auto-includes all 3 domains
)
```

#### Domain-Specific Usage:
```c
// If you only need state management
#include "system_state_machine.h"

// In CMakeLists.txt
target_link_libraries(your_module
    app_core_state_management  # Only state management
)
```

---

## 🧪 Testing

### Test Commands:
```bash
# State management tests
./build/tests/unit/test_system_state_machine

# Safety tests
./build/tests/unit/test_safety_monitor
./build/tests/unit/test_safety_monitor_latency

# Control tests
./build/tests/unit/test_control_loop_timing
./build/tests/unit/test_control_loop_limits
```

### Test Coverage:
- State Management: 95%+
- Safety System: 90%+
- Control System: 92%+

---

## 🚨 Important Notes

### Safety-Critical Code:
- 🔴 **safety/** domain contains SAFETY-CRITICAL code
- 🔴 Modifications require safety review
- 🔴 Must pass safety test suite
- 🔴 See [Safety README](safety/README.md) for details

### Performance Requirements:
- ⚡ E-Stop response: < 50ms
- ⚡ Safety check interval: 50ms
- ⚡ Control loop: 100 Hz (10ms)
- ⚡ State updates: < 10ms

---

## 🔄 Version History

### v1.0.1 (2025-10-07)
- ✅ Domain-Driven Architecture migration
- ✅ Created 3 domain subfolders
- ✅ Modular build system
- ✅ Comprehensive documentation

### v1.0.0 (2025-10-07)
- ✅ Initial monolithic structure
- ✅ All functionality working

---

## 📖 Related Documentation

- [Main README](../../../README.md)
- [Code Structure](../../../CODE_STRUCTURE.md)
- [Build Guide](../../../BUILD_GUIDE.md)
- [Migration Log](../../../MIGRATION_LOG_v1.0.1.md)

---

**Last Updated:** 2025-10-07  
**Maintained By:** Firmware Team

