# HAL GPIO MODULE DOCUMENTATION

**File:** `src/hal/hal_gpio.c`  
**Header:** `include/hal_gpio.h`  
**Phiên bản:** 1.0.0  
**Ngày tạo:** 2025-01-27  
**Team:** EMBED  
**Task:** EM-05 (IO Drivers)

## 📋 TỔNG QUAN

HAL GPIO module cung cấp interface để điều khiển GPIO pins và Relay outputs trên Orange Pi 5B. Module này hỗ trợ cả GPIO control cơ bản và Relay control cho các ứng dụng điều khiển ngoại vi.

## 🏗️ KIẾN TRÚC MODULE

```mermaid
graph TB
    subgraph "HAL GPIO Module"
        subgraph "Core Functions"
            C1[Initialization]
            C2[GPIO Control]
            C3[Relay Control]
            C4[Utility Functions]
        end
        
        subgraph "Hardware Interface"
            H1[Orange Pi 5B GPIO]
            H2[LED Status Indicators]
            H3[E-Stop Safety System]
            H4[Relay Outputs]
        end
        
        subgraph "State Management"
            S1[Thread Safety]
            S2[Device Info]
            S3[Statistics]
            S4[Configuration]
        end
        
        subgraph "External Interface"
            E1[Application Layer]
            E2[Safety Manager]
            E3[Module Manager]
        end
    end
    
    C1 --> S1
    C2 --> H1
    C2 --> H2
    C3 --> H4
    C4 --> S3
    
    E1 --> C1
    E1 --> C2
    E2 --> C2
    E3 --> C3
```

## 🔧 HARDWARE MAPPING

### Orange Pi 5B GPIO Pin Assignment

```mermaid
graph LR
    subgraph "Orange Pi 5B GPIO"
        subgraph "LED Status Indicators"
            L1[Pin 54 - Power LED]
            L2[Pin 35 - System LED]
            L3[Pin 28 - Comm LED]
            L4[Pin 29 - Network LED]
            L5[Pin 58 - Error LED]
        end
        
        subgraph "Safety System"
            S1[Pin 59 - E-Stop Ch1]
            S2[Pin 131 - E-Stop Ch2]
        end
        
        subgraph "Output Control"
            R1[Pin 132 - Relay Output]
        end
    end
    
    subgraph "GPIO Chip"
        G1[gpiochip1]
    end
    
    L1 --> G1
    L2 --> G1
    L3 --> G1
    L4 --> G1
    L5 --> G1
    S1 --> G1
    S2 --> G1
    R1 --> G1
```

### Hardware Interface Summary

| **Category** | **Pin** | **GPIO** | **Function** | **Direction** |
|--------------|---------|----------|--------------|---------------|
| **Power LED** | 54 | GPIO1_D6 | Power status indicator | Output |
| **System LED** | 35 | GPIO1_A3 | System status indicator | Output |
| **Comm LED** | 28 | GPIO0_D4 | Communication status | Output |
| **Network LED** | 29 | GPIO0_D5 | Network status | Output |
| **Error LED** | 58 | GPIO1_D2 | Error status indicator | Output |
| **E-Stop Ch1** | 59 | GPIO1_D3 | Emergency stop channel 1 | Input |
| **E-Stop Ch2** | 131 | GPIO4_A3 | Emergency stop channel 2 | Input |
| **Relay Output** | 132 | GPIO4_A4 | Relay control output | Output |

## 🔧 API REFERENCE

### Initialization Functions

| **Function** | **Parameters** | **Returns** | **Description** |
|--------------|----------------|-------------|-----------------|
| `hal_gpio_init()` | `void` | `hal_status_t` | Khởi tạo GPIO HAL module |
| `hal_gpio_deinit()` | `void` | `hal_status_t` | Hủy khởi tạo GPIO HAL module |

### GPIO Control Functions (Stubs)

| **Function** | **Parameters** | **Returns** | **Description** |
|--------------|----------------|-------------|-----------------|
| `hal_gpio_configure_pin()` | `const gpio_config_t *config` | `hal_status_t` | Configure GPIO pin |
| `hal_gpio_set_direction()` | `uint32_t pin, gpio_direction_t direction` | `hal_status_t` | Set pin direction |
| `hal_gpio_set_value()` | `uint32_t pin, bool value` | `hal_status_t` | Set pin value |
| `hal_gpio_get_value()` | `uint32_t pin, bool *value` | `hal_status_t` | Get pin value |
| `hal_gpio_set_edge()` | `uint32_t pin, gpio_edge_t edge` | `hal_status_t` | Set edge detection |
| `hal_gpio_set_bias()` | `uint32_t pin, gpio_bias_t bias` | `hal_status_t` | Set pull-up/down |
| `hal_gpio_set_drive()` | `uint32_t pin, gpio_drive_t drive` | `hal_status_t` | Set drive strength |
| `hal_gpio_wait_for_event()` | `uint32_t pin, gpio_event_t *event, uint32_t timeout_ms` | `hal_status_t` | Wait for GPIO event |

### Relay Control Functions (Stubs)

| **Function** | **Parameters** | **Returns** | **Description** |
|--------------|----------------|-------------|-----------------|
| `hal_relay_init()` | `const relay_config_t *config` | `hal_status_t` | Initialize relay |
| `hal_relay_deinit()` | `void` | `hal_status_t` | Deinitialize relay |
| `hal_relay_set()` | `uint8_t channel, bool state` | `hal_status_t` | Set relay state |
| `hal_relay_get()` | `uint8_t channel, bool *state` | `hal_status_t` | Get relay state |
| `hal_relay_pulse()` | `uint8_t channel, uint32_t duration_ms` | `hal_status_t` | Pulse relay |
| `hal_relay_toggle()` | `uint8_t channel` | `hal_status_t` | Toggle relay |

### Statistics & Health Functions

| **Function** | **Parameters** | **Returns** | **Description** |
|--------------|----------------|-------------|-----------------|
| `hal_gpio_get_statistics()` | `gpio_statistics_t *statistics` | `hal_status_t` | Get GPIO statistics |
| `hal_gpio_reset_statistics()` | `void` | `hal_status_t` | Reset GPIO statistics |
| `hal_gpio_health_check()` | `void` | `hal_status_t` | GPIO health check |
| `hal_relay_get_statistics()` | `relay_statistics_t *statistics` | `hal_status_t` | Get relay statistics |
| `hal_relay_reset_statistics()` | `void` | `hal_status_t` | Reset relay statistics |
| `hal_relay_health_check()` | `void` | `hal_status_t` | Relay health check |

## 🔍 LOGIC ANALYSIS

### Internal State Management

```mermaid
graph TB
    subgraph "Global State Structure"
        GS1[Initialization Flag]
        GS2[Thread Mutex]
        GS3[GPIO Statistics]
        GS4[Relay Statistics]
        GS5[Device Info]
        GS6[Last Operation Time]
    end
    
    subgraph "State Management Flow"
        SM1[Check Initialization]
        SM2[Lock Mutex]
        SM3[Perform Operation]
        SM4[Update Statistics]
        SM5[Unlock Mutex]
    end
    
    GS1 --> SM1
    GS2 --> SM2
    SM3 --> GS6
    SM4 --> GS3
    SM4 --> GS4
```

### GPIO Configuration Logic

```mermaid
graph LR
    subgraph "GPIO Configuration"
        C1[Pin Number] --> C2[Direction]
        C2 --> C3[Edge Detection]
        C3 --> C4[Bias Setting]
        C4 --> C5[Drive Strength]
        C5 --> C6[Active Low]
        C6 --> C7[Debounce Time]
    end
    
    subgraph "Configuration Types"
        T1[Input Mode]
        T2[Output Mode]
        T3[Interrupt Mode]
    end
    
    C2 --> T1
    C2 --> T2
    C3 --> T3
```

### Relay Control Logic

```mermaid
sequenceDiagram
    participant App as Application
    participant HAL as HAL GPIO
    participant Relay as Relay Hardware
    participant Stats as Statistics
    
    App->>HAL: hal_relay_set(channel, state)
    HAL->>HAL: Validate channel
    HAL->>HAL: Check initialization
    HAL->>Relay: Set relay state
    Relay-->>HAL: Operation result
    HAL->>Stats: Update statistics
    HAL-->>App: Return status
```

## 🧪 TESTING CONSIDERATIONS

### Unit Test Matrix

| **Test Category** | **Test Cases** | **Expected Results** | **Status** |
|-------------------|----------------|---------------------|------------|
| **Initialization** | Init/deinit/reinit | Proper state management | ⚠️ Cần implement |
| **Thread Safety** | Concurrent operations | No race conditions | ⚠️ Cần implement |
| **GPIO Configuration** | Pin setup/teardown | Correct pin state | ⚠️ Cần implement |
| **GPIO Operations** | Read/write operations | Accurate I/O | ⚠️ Cần implement |
| **Event Handling** | Edge detection | Proper event capture | ⚠️ Cần implement |
| **Relay Control** | Set/get/pulse/toggle | Correct relay behavior | ⚠️ Cần implement |
| **Statistics** | Collection/reset | Accurate statistics | ⚠️ Cần implement |
| **Error Handling** | Invalid parameters | Proper error codes | ⚠️ Cần implement |

### Integration Test Scenarios

```mermaid
graph TB
    subgraph "Hardware Integration Tests"
        HI1[GPIO Pin Testing]
        HI2[Relay Testing]
        HI3[LED Testing]
        HI4[E-Stop Testing]
    end
    
    subgraph "System Integration Tests"
        SI1[Safety System Integration]
        SI2[Module Manager Integration]
        SI3[Application Layer Integration]
        SI4[Error Propagation]
    end
    
    subgraph "Test Scenarios"
        TS1[Pin control accuracy]
        TS2[Relay timing accuracy]
        TS3[LED pattern control]
        TS4[E-Stop response time]
        TS5[Safety interlock]
        TS6[Emergency shutdown]
    end
    
    HI1 --> TS1
    HI2 --> TS2
    HI3 --> TS3
    HI4 --> TS4
    SI1 --> TS5
    SI2 --> TS6
```

## 🚨 LIMITATIONS & TODO

### Current Limitations

| **Limitation** | **Impact** | **Priority** | **Solution** |
|----------------|------------|--------------|--------------|
| **Implementation Status** | Tất cả functions đều là stubs | CRITICAL | Implement GPIO functions |
| **Hardware Support** | Chưa implement GPIO interface | HIGH | Implement sysfs/libgpiod |
| **Safety Features** | Chưa có safety checks | HIGH | Implement safety logic |
| **Error Handling** | Chưa có error handling thực tế | MEDIUM | Implement error handling |

### TODO Items

| **Item** | **Description** | **Priority** | **Timeline** |
|----------|-----------------|--------------|--------------|
| **GPIO Implementation** | Implement sysfs/libgpiod interface | CRITICAL | Immediate |
| **Relay Implementation** | Implement relay control logic | HIGH | Next sprint |
| **Safety Implementation** | Add E-Stop monitoring | HIGH | Next sprint |
| **Enhanced Features** | Add interrupt handling | MEDIUM | Future sprint |
| **Performance Optimization** | Add caching mechanisms | LOW | Future sprint |

## 📊 PERFORMANCE METRICS

### Memory Usage Analysis

| **Component** | **Size** | **Type** | **Description** |
|---------------|----------|----------|-----------------|
| **Static allocation** | ~1KB | Static | Module state, mutex |
| **Per-pin overhead** | ~100 bytes | Dynamic | Pin configuration |
| **Statistics overhead** | ~200 bytes | Static | GPIO/relay statistics |
| **Total estimated** | ~8KB | Static | Cho 64 pins |

### Timing Performance

| **Operation** | **Target** | **Current** | **Status** |
|---------------|------------|-------------|------------|
| **GPIO read** | < 10μs | Not implemented | ❌ Cần implement |
| **GPIO write** | < 10μs | Not implemented | ❌ Cần implement |
| **Relay activation** | < 1ms | Not implemented | ❌ Cần implement |
| **Event detection** | < 100μs | Not implemented | ❌ Cần implement |

### Scalability Metrics

| **Metric** | **Current Limit** | **Planned Enhancement** |
|------------|-------------------|-------------------------|
| **Maximum pins** | 64 (GPIO_MAX_PINS) | Configurable limit |
| **Maximum relays** | 2 (RELAY_MAX_CHANNELS) | Expandable to 8 |
| **Concurrent operations** | Thread-safe | Async operations |
| **Event queue** | Unlimited (planned) | Circular buffer |

## 🔧 HARDWARE INTEGRATION

### Interface Requirements

```mermaid
graph TB
    subgraph "Interface Layers"
        subgraph "Application Layer"
            A1[GPIO API]
            A2[Relay API]
            A3[Safety API]
        end
        
        subgraph "HAL Layer"
            H1[GPIO HAL]
            H2[Relay HAL]
            H3[Safety HAL]
        end
        
        subgraph "System Interface"
            S1[Sysfs Interface]
            S2[Libgpiod Interface]
            S3[Direct GPIO Access]
        end
        
        subgraph "Hardware"
            HW1[Orange Pi 5B]
            HW2[GPIO Pins]
            HW3[Relay Module]
        end
    end
    
    A1 --> H1
    A2 --> H2
    A3 --> H3
    
    H1 --> S1
    H1 --> S2
    H2 --> S3
    
    S1 --> HW1
    S2 --> HW1
    S3 --> HW2
    S3 --> HW3
```

### Hardware Validation Requirements

| **Requirement** | **Validation Method** | **Expected Result** |
|-----------------|----------------------|-------------------|
| **Pin availability** | Check `/sys/class/gpio/` | All pins accessible |
| **Pin capability** | Test read/write operations | Successful I/O |
| **Hardware compatibility** | Test with actual hardware | Proper functionality |
| **Safety validation** | Test E-Stop functionality | Safe operation |

## 🔄 CHANGELOG

### v1.0.0 (2025-01-27)
- ✅ Initial module structure
- ✅ Basic initialization/deinitialization
- ✅ Thread-safe design
- ✅ Device info management
- ✅ Statistics structures
- ✅ GPIO function stubs
- ✅ Relay function stubs
- ✅ Utility function stubs
- ✅ Hardware mapping definition
- ✅ Added Mermaid diagrams
- ✅ Added comprehensive tables

---

**Lưu ý:** Module này cần được implement đầy đủ trước khi sử dụng trong production. Hiện tại tất cả functions đều là stubs và cần actual hardware implementation.
