# ⚙️ FW TEAM PROMPT TEMPLATE - OHT-50

**Version:** 2.0.0  
**Date:** 2025-01-27  
**Status:** Framework Foundation Complete ✅

---

## 🎯 **FW TEAM CONTEXT**

### **Current Status**
- **Framework Foundation**: ✅ HOÀN THÀNH
- **HAL Abstraction**: ✅ HOÀN THÀNH  
- **RS485 Protocol**: ✅ HOÀN THÀNH
- **Next Phase**: State Machine, Control Loop, Safety Mechanisms

### **Completed Tasks**
- ✅ **FW-01**: Khung FW + Scheduler (Real-time scheduler với multi-threading)
- ✅ **FW-02**: HAL Abstraction (Complete hardware abstraction layer)
- ✅ **FW-07**: RS485 Protocol (Comprehensive protocol implementation)

### **Framework Architecture**
```
firmware/
├── src/
│   ├── main.c                 # Main entry point ✅
│   ├── scheduler/scheduler.c  # Real-time scheduler ✅
│   ├── hal/hal_gpio.c         # GPIO implementation ✅
│   ├── hal/hal_stubs.c        # HAL stubs ✅
│   ├── comms/rs485_protocol.c # RS485 protocol ✅
│   ├── state_machine/         # State machine 📋
│   ├── control/               # Control loop 📋
│   └── safety/                # Safety mechanisms 📋
├── include/
│   ├── scheduler.h            # Scheduler interface ✅
│   ├── hal.h                  # HAL interface ✅
│   ├── rs485_protocol.h       # RS485 protocol ✅
│   ├── state_machine.h        # State machine ✅
│   └── control.h              # Control interface 📋
├── tests/test_framework.c     # Framework test ✅
├── CMakeLists.txt            # Build configuration ✅
└── README.md                 # Project documentation ✅
```

---

## 📋 **AVAILABLE TASKS**

### **🔄 Ready to Start**
| Task ID | Description | Dependencies | Estimate | Priority |
|---------|-------------|--------------|----------|----------|
| FW-03 | State Machine Implementation | FW-01 ✅ | 3 days | HIGH |
| FW-04 | Control Loop Implementation | FW-02 ✅ | 4 days | HIGH |
| FW-09 | Safety Mechanisms | FW-05 | 2 days | CRITICAL |
| FW-05 | Safety System | FW-03 | 3 days | CRITICAL |
| FW-06 | Communication Stack | FW-07 ✅ | 2 days | MEDIUM |
| FW-08 | Configuration System | FW-02 ✅ | 2 days | MEDIUM |

### **📋 Future Tasks**
| Task ID | Description | Dependencies | Estimate | Priority |
|---------|-------------|--------------|----------|----------|
| FW-10 | Unit Tests | FW-01-FW-09 | 3 days | HIGH |
| FW-11 | Integration Testing | FW-10 | 2 days | HIGH |
| FW-12 | Performance Optimization | FW-11 | 2 days | MEDIUM |
| FW-13 | Documentation | FW-12 | 1 day | LOW |
| FW-14 | Final Integration | FW-13 | 1 day | CRITICAL |

---

## 🏗️ **FRAMEWORK COMPONENTS**

### **✅ Completed Components**

#### **Real-time Scheduler (FW-01)**
- **Location**: `firmware/src/scheduler/scheduler.c`
- **Features**: Priority-based scheduling, latency measurement, statistics
- **Performance**: < 100μs max latency, < 10μs jitter
- **Threads**: Scheduler (1kHz), Control (100Hz), Comms (50Hz)

#### **HAL Abstraction (FW-02)**
- **Location**: `firmware/include/hal.h`, `firmware/src/hal/`
- **Features**: GPIO, PWM, ADC, UART, Timer, RS485 interfaces
- **Implementation**: Full GPIO driver với libgpiod, complete stub implementations
- **Error Handling**: Comprehensive error codes và handling

#### **RS485 Protocol (FW-07)**
- **Location**: `firmware/include/rs485_protocol.h`, `firmware/src/comms/rs485_protocol.c`
- **Features**: Frame handling, CRC16, retry mechanism, heartbeat system
- **Performance**: < 1ms latency, < 0.1% BER
- **Commands**: 25+ commands cho tất cả modules

### **📋 Components to Implement**

#### **State Machine (FW-03)**
- **Location**: `firmware/include/state_machine.h`, `firmware/src/state_machine/`
- **States**: Idle, Move, Approach, Align, Door Open, Cargo Op, Door Close, Leave, Fault, E-Stop
- **Events**: Command events, sensor events, safety events
- **Dependencies**: FW-01 (Scheduler)

#### **Control Loop (FW-04)**
- **Location**: `firmware/include/control.h`, `firmware/src/control/`
- **Features**: Motion control, position control, velocity control
- **Performance**: 100Hz control loop
- **Dependencies**: FW-02 (HAL)

#### **Safety Mechanisms (FW-05, FW-09)**
- **Location**: `firmware/src/safety/`
- **Features**: E-Stop handling, interlock system, fault detection
- **Priority**: CRITICAL
- **Dependencies**: FW-03 (State Machine)

---

## 🎯 **DEVELOPMENT GUIDELINES**

### **Code Standards**
- **Language**: C/C++ với C11/C++14 standards
- **Style**: Consistent naming, comprehensive comments
- **Error Handling**: Return codes, error logging
- **Thread Safety**: Mutex protection cho shared resources

### **Performance Requirements**
- **Scheduler**: < 100μs max latency
- **Control Loop**: 100Hz stable frequency
- **Communication**: < 1ms frame latency
- **Safety**: < 10μs response time

### **Testing Requirements**
- **Unit Tests**: > 90% code coverage
- **Integration Tests**: All subsystems working together
- **Performance Tests**: Latency và throughput validation
- **Safety Tests**: E-Stop và fault scenarios

### **Documentation Requirements**
- **API Documentation**: Function descriptions, parameters, return values
- **Architecture Documentation**: System design, component interactions
- **User Manual**: Setup, configuration, troubleshooting

---

## 🚀 **DEVELOPMENT WORKFLOW**

### **Task Implementation Process**
1. **Analysis**: Review requirements và dependencies
2. **Design**: Create interface design và architecture
3. **Implementation**: Write code với unit tests
4. **Testing**: Run unit tests và integration tests
5. **Documentation**: Update documentation
6. **Review**: Code review và performance validation

### **Build Process**
```bash
cd firmware/
mkdir build && cd build
cmake ..
make
```

### **Test Process**
```bash
# Run unit tests
./test_framework

# Run specific tests
./test_framework --test=hal
./test_framework --test=scheduler
./test_framework --test=rs485
```

### **Debug Process**
- **Logging**: Use printf với timestamps
- **Performance**: Use scheduler statistics
- **Memory**: Use valgrind cho memory leaks
- **Threading**: Use gdb cho thread debugging

---

## 📊 **PERFORMANCE METRICS**

### **Current Performance**
- **Scheduler Latency**: < 100μs ✅
- **RS485 Latency**: < 1ms ✅
- **GPIO Response**: < 1μs ✅
- **Memory Usage**: < 10MB ✅

### **Target Performance**
- **Control Loop**: 100Hz stable
- **Safety Response**: < 10μs
- **Communication**: < 0.1% BER
- **System Uptime**: > 99.9%

---

## 🔧 **AVAILABLE TOOLS**

### **Build Tools**
- **CMake**: Build system configuration
- **Make**: Build automation
- **GCC**: C/C++ compiler

### **Testing Tools**
- **Unit Test Framework**: Custom test framework
- **Performance Tools**: Latency measurement
- **Memory Tools**: Valgrind, AddressSanitizer

### **Debug Tools**
- **GDB**: Debugger
- **Strace**: System call tracing
- **Perf**: Performance profiling

---

## 📋 **TASK TEMPLATES**

### **New Component Template**
```c
/**
 * Component Name
 * 
 * @file component_name.c
 * @version 1.0.0
 * @date YYYY-MM-DD
 * @author FW Team
 */

#include "component_name.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Component implementation
int component_init(void) {
    // Implementation
    return HAL_OK;
}

void component_cleanup(void) {
    // Cleanup
}
```

### **Test Template**
```c
static void test_component(void) {
    printf("\n=== Component Test ===\n");
    
    // Test initialization
    if (component_init() != HAL_OK) {
        printf("[TEST] Component init failed\n");
        return;
    }
    printf("[TEST] Component init OK\n");
    
    // Test functionality
    // ...
    
    component_cleanup();
}
```

---

## 🎯 **NEXT ACTIONS**

### **Immediate (This Week)**
1. **FW-03**: Begin state machine implementation
2. **FW-04**: Begin control loop implementation
3. **FW-05**: Begin safety system implementation

### **This Week**
1. **FW-03**: Complete state machine với unit tests
2. **FW-04**: Complete control loop với performance validation
3. **FW-05**: Complete safety system với safety tests

### **Next Week**
1. **FW-09**: Complete E-Stop & interlock system
2. **FW-06**: Complete communication stack
3. **FW-08**: Complete configuration system

---

## 📞 **SUPPORT & RESOURCES**

### **Documentation**
- **Framework Guide**: `firmware/README.md`
- **API Reference**: Header files trong `firmware/include/`
- **Test Examples**: `firmware/tests/test_framework.c`

### **Dependencies**
- **Hardware**: Orange Pi 5B với UART1 RS485
- **Libraries**: libgpiod, pthread, rt
- **Tools**: CMake, GCC, GDB

### **Contact**
- **FW Team Lead**: For technical decisions
- **PM**: For project coordination
- **EMBED Team**: For hardware questions

---

**🚀 Framework foundation complete! Ready for next phase development! 🚀**

**📋 Focus on FW-03, FW-04, FW-05 for immediate progress! 📋**
