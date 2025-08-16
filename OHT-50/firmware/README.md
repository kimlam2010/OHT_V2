# OHT-50 Firmware

**Version:** 1.0.0  
**Platform:** Orange Pi 5B (RK3588)  
**Build Date:** 2025-01-27

---

## 📁 Cấu trúc thư mục

```
firmware/
├── src/
│   ├── main.c                 # Main entry point
│   ├── scheduler/             # FW-01: Real-time scheduler
│   ├── hal/                   # FW-02: HAL abstraction
│   │   ├── hal_gpio.c
│   │   ├── hal_pwm.c
│   │   ├── hal_adc.c
│   │   ├── hal_uart.c
│   │   └── hal_timer.c
│   ├── comms/                 # FW-07: RS485 protocol
│   │   ├── rs485_protocol.c
│   │   ├── rs485_frame.c
│   │   └── rs485_crc.c
│   ├── state_machine/         # FW-03: State machine
│   ├── control/               # FW-04: Control loop
│   └── safety/                # FW-09: Safety mechanisms
├── include/
│   ├── hal.h                  # HAL interface
│   ├── scheduler.h
│   ├── rs485_protocol.h
│   └── state_machine.h
├── tests/
│   ├── test_scheduler.c
│   ├── test_hal.c
│   └── test_rs485.c
├── docs/
│   ├── firmware_architecture.md
│   └── api_reference.md
├── CMakeLists.txt
└── README.md
```

---

## 🛠️ Build Instructions

### Prerequisites
```bash
# Install build tools
sudo apt update
sudo apt install build-essential cmake gdb

# Install development libraries
sudo apt install libgpiod-dev libserialport-dev

# Setup cross-compilation (if needed)
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

### Build
```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
make -j4

# Run tests
make test
```

### Install
```bash
# Install firmware
sudo make install
```

---

## 🚀 Development Tasks

### FW-01: Khung FW + Scheduler (3 ngày)
- [ ] Firmware architecture design
- [ ] Real-time scheduler implementation
- [ ] Interrupt priority management
- [ ] Control loop với timing (1ms cycle)
- [ ] Latency measurement tools (< 100μs jitter)
- [ ] HAL abstraction layer foundation
- [ ] Error handling framework
- [ ] Unit tests cho scheduler

### FW-02: HAL Abstraction (4 ngày)
- [ ] HAL interface definition
- [ ] GPIO HAL implementation
- [ ] PWM HAL implementation
- [ ] ADC HAL implementation
- [ ] UART/RS485 HAL implementation
- [ ] Timer HAL implementation
- [ ] Error handling tested
- [ ] Unit tests pass cho HAL functions

### FW-07: RS485 Protocol (3 ngày)
- [ ] RS485 protocol specification
- [ ] Frame format implementation
- [ ] CRC16 calculation
- [ ] Retry mechanism với backoff
- [ ] Timeout handling
- [ ] Error statistics và logging
- [ ] Basic commands (PING, HEARTBEAT, GET_INFO)
- [ ] Unit tests cho protocol

---

## 📚 Tài liệu tham chiếu

- **State Machine:** `../docs/specs/state_machine.md`
- **Control Spec:** `../docs/specs/control_spec.md`
- **Safety:** `../docs/specs/safety.md`
- **RS485 Protocol:** `../docs/specs/bus_rs485.md`
- **Hardware:** `../docs/specs/hardware.md`
- **Pinout:** `../docs/dev_radxa/pinout_radxa.md`

---

## 🔧 Configuration

### Real-time Constraints
- **Control Loop:** 1ms cycle time
- **Latency:** < 100μs jitter
- **Interrupt Priority:** Control > Safety > Comms
- **Deterministic Behavior:** Required

### Safety Requirements
- **E-Stop Response:** < 100ms
- **Fault Handling:** Graceful degradation
- **Watchdog:** Hardware/software watchdog
- **Error Recovery:** Automatic recovery khi possible

---

## 🧪 Testing

### Unit Tests
```bash
# Run all tests
make test

# Run specific test
./build/oht50_tests
```

### Integration Tests
```bash
# Test với EMBED hardware
cd ../EMBED
python3 test_modbus_rtu.py
```

---

## 📞 Support

- **FW Lead:** [Tên FW Lead]
- **PM:** [Tên PM]
- **Documentation:** `../docs/tasks/FW_TEAM_KICKOFF_GUIDE.md`

---

**Build Date:** 2025-01-27  
**Version:** 1.0.0
