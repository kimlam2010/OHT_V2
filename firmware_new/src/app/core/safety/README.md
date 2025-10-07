# 🛡️ Safety System Domain

**Phiên bản:** 1.0.0  
**Ngày cập nhật:** 2025-10-07  
**Mục tiêu:** Real-time safety monitoring với multi-level graduated response

---

## 📖 Tổng Quan

Safety System domain là **SAFETY-CRITICAL** component chịu trách nhiệm monitoring và response cho toàn bộ hệ thống OHT-50.

**⚠️ WARNING:** Code trong domain này có thể trigger E-Stop và ảnh hưởng trực tiếp đến safety!

**Vị trí trong kiến trúc:**
- 🔴 **Layer:** Application Core (Safety-Critical)
- 🔴 **Dependencies:** State Management, HAL Safety
- 🔴 **Used by:** Control System, System Controller, Managers

---

## 🗂️ Components

### 1. **Safety Monitor** (`safety_monitor.c/h`)

**Dòng code:** 1,763 lines  
**Chức năng:** Main safety monitoring và coordination

#### Responsibilities:
- 🚨 **E-Stop Monitoring** - Response time < 50ms
- 🚨 **Safety Checks** - Continuous safety validation
- 🚨 **Fault Detection** - Hardware và software faults
- 🚨 **Safety Interlocks** - Prevent unsafe operations

#### Key Functions:
```c
// Initialization
hal_status_t safety_monitor_init(const safety_monitor_config_t *config);
hal_status_t safety_monitor_deinit(void);

// Runtime monitoring
hal_status_t safety_monitor_update(void);
hal_status_t safety_monitor_check_safety(void);

// E-Stop handling
hal_status_t safety_monitor_trigger_estop(const char *reason);
bool safety_monitor_is_estop_active(void);
hal_status_t safety_monitor_reset_estop(void);

// Status
hal_status_t safety_monitor_get_status(safety_monitor_status_t *status);
```

---

### 2. **Critical Module Detector** (`critical_module_detector.c/h`)

**Dòng code:** 985 lines  
**Chức năng:** Detect critical module failures và assess health

#### Responsibilities:
- 🔍 **Module Health Assessment** - Continuous health monitoring
- 🔍 **Critical Failure Detection** - Identify critical failures
- 🔍 **Response Level Determination** - Calculate safety response level
- 🔍 **Module Classification** - Critical vs non-critical modules

#### Health Levels:
```c
typedef enum {
    CRITICAL_MODULE_HEALTH_HEALTHY = 0,    // Normal operation
    CRITICAL_MODULE_HEALTH_DEGRADED,       // Performance degraded
    CRITICAL_MODULE_HEALTH_CRITICAL,       // Critical issues
    CRITICAL_MODULE_HEALTH_FAILED          // Complete failure
} critical_module_health_t;
```

#### Key Functions:
```c
// Initialization
hal_status_t critical_module_detector_init(void);

// Health assessment
hal_status_t critical_module_get_health_assessment(uint8_t module_addr, 
                                                   module_health_assessment_t *assessment);

// Response level determination
hal_status_t critical_module_determine_response_level(safety_response_level_t *level);

// Module registration
hal_status_t critical_module_register(uint8_t module_addr, module_criticality_t criticality);
```

---

### 3. **Graduated Response System** (`graduated_response_system.c/h`)

**Dòng code:** 936 lines  
**Chức năng:** Multi-level safety response với LED patterns và adaptive polling

#### Response Levels:
```c
typedef enum {
    RESPONSE_LEVEL_NORMAL = 0,      // ✅ All systems OK
    RESPONSE_LEVEL_WARNING,         // ⚠️ Minor issues detected
    RESPONSE_LEVEL_CRITICAL,        // 🔴 Critical issues
    RESPONSE_LEVEL_EMERGENCY        // 🚨 Emergency - E-Stop triggered
} safety_response_level_t;
```

#### Features:
- 💡 **LED Patterns** - Visual feedback theo response level
- ⚡ **Adaptive Polling** - Adjust polling rates based on health
- 🔄 **Auto Recovery** - Automatic de-escalation khi system improves
- 📊 **Transition Tracking** - Statistics và diagnostics

#### Key Functions:
```c
// System management
hal_status_t graduated_response_init(void);
hal_status_t graduated_response_start(void);
hal_status_t graduated_response_update(void);

// Response level management
hal_status_t graduated_response_set_level(safety_response_level_t level, const char *reason);
hal_status_t graduated_response_get_level(safety_response_level_t *level);

// LED patterns
hal_status_t graduated_response_update_led_patterns(safety_response_level_t level);

// Adaptive polling
hal_status_t graduated_response_get_polling_interval(uint8_t module_addr, uint32_t *interval);
```

---

### 4. **Safety RS485 Integration** (`safety_rs485_integration.c/h`)

**Dòng code:** 239 lines  
**Chức năng:** Safety-critical RS485 communication với slave modules

#### Responsibilities:
- 📡 **Safe Communication** - Validated RS485 communication
- 📡 **CRC Validation** - Data integrity checks
- 📡 **Timeout Handling** - Communication timeout detection
- 📡 **Statistics Tracking** - Communication health metrics

#### Key Functions:
```c
// Initialization
hal_status_t safety_rs485_init(void);

// Communication
hal_status_t safety_rs485_send_command(uint8_t module_addr, const uint8_t *data, size_t len);
hal_status_t safety_rs485_read_status(uint8_t module_addr, uint8_t *data, size_t len);

// Health monitoring
hal_status_t safety_rs485_get_stats(safety_rs485_stats_t *stats);
hal_status_t safety_rs485_check_health(void);
```

---

## 🔗 Domain Architecture

### Dependency Graph:
```
┌─────────────────────────────────────┐
│     State Management Domain         │ (No dependencies)
└─────────────────┬───────────────────┘
                  │
                  ↓
┌─────────────────────────────────────┐
│        Safety System Domain          │
│  ┌──────────────────────────────┐   │
│  │   safety_monitor             │   │
│  └──────────┬───────────────────┘   │
│             │                        │
│    ┌────────┴────────┐               │
│    ↓                 ↓               │
│  ┌─────────────┐  ┌──────────────┐  │
│  │  critical   │  │  graduated   │  │
│  │  detector   │  │  response    │  │
│  └──────┬──────┘  └──────┬───────┘  │
│         └────────┬────────┘          │
│                  ↓                   │
│         ┌────────────────┐           │
│         │ rs485_integration│         │
│         └────────────────┘           │
└─────────────────────────────────────┘
```

### Data Flow:
```
Module Status → RS485 Integration → Critical Detector
                                           ↓
                                    Health Assessment
                                           ↓
                                    Response Level
                                           ↓
                                  Graduated Response
                                    ↙          ↘
                            LED Patterns   Polling Rates
```

---

## ⚡ Performance Requirements

### Real-time Constraints:
| Operation | Target | Actual |
|-----------|--------|--------|
| E-Stop Response | < 50ms | ~20ms ✅ |
| Safety Check Interval | 50ms | 50ms ✅ |
| Critical Detection | < 100ms | ~50ms ✅ |
| LED Update | < 20ms | ~10ms ✅ |

### Resource Usage:
| Resource | Limit | Actual |
|----------|-------|--------|
| Memory | < 50KB | ~35KB ✅ |
| CPU (average) | < 10% | ~7% ✅ |
| CPU (peak) | < 30% | ~25% ✅ |

---

## 🧪 Testing

### Safety Tests:
```bash
# Run safety system tests
./scripts/safety/safety_test.sh

# Run latency tests
./build/tests/unit/test_safety_monitor_latency

# Run graduated response tests
./build/tests/unit/test_graduated_response
```

### Test Coverage:
- ✅ E-Stop response time < 50ms
- ✅ Critical module detection
- ✅ Graduated response transitions
- ✅ LED pattern updates
- ✅ Adaptive polling intervals
- ✅ Fault recovery

---

## 🔐 Safety Compliance

### Standards:
- 📋 **IEC 61508** - Functional safety (target: SIL 2)
- 📋 **ISO 13849-1** - Safety of machinery
- 📋 **MISRA C:2012** - Software safety guidelines

### Safety Mechanisms:
- ✅ Redundant safety checks
- ✅ Watchdog monitoring
- ✅ Fail-safe defaults
- ✅ Comprehensive error logging
- ✅ Defensive programming

---

## 🚨 Critical Notes

### ⚠️ SAFETY-CRITICAL CODE
**DO NOT modify without:**
1. ✅ Safety impact assessment
2. ✅ Comprehensive testing
3. ✅ Code review by safety expert
4. ✅ Documentation update

### ⚠️ PERFORMANCE-CRITICAL
**Response times are critical:**
- E-Stop: MUST < 50ms
- Safety check: MUST run every 50ms
- DO NOT add blocking operations

### ⚠️ THREAD SAFETY
**Not thread-safe by design:**
- Call only từ main control loop thread
- DO NOT call từ interrupt handlers
- DO NOT call từ multiple threads

---

## 📚 Related Documentation

- [State Management README](../state_management/README.md)
- [Control System README](../control/README.md)
- [Safety Test Guide](../../../../scripts/safety/README.md)
- [API Documentation](../../api/README.md)

---

**Maintained by:** Firmware Safety Team  
**Last Safety Review:** 2025-10-07  
**Next Review Due:** 2025-11-07

