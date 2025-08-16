# 🚀 FW TEAM KICKOFF GUIDE - OHT-50

**Ngày:** 2025-01-27  
**Từ:** PM OHT-50  
**Đến:** FW Team  
**Subject:** FW Team Ready to Start - EMBED Complete ✅

---

## 🎯 **Tình hình hiện tại**

### EMBED Team Status ✅ COMPLETE
- **Hardware:** UART1 RS485 ready với pins 46, 47
- **HAL:** RS485 interface complete và tested  
- **Documentation:** Tất cả EMBED files trong `/home/orangepi/OHT-50/EMBED/`
- **Dependencies:** Tất cả cleared cho FW development

### FW Team Status 🚀 READY TO START
- **Tổng số task:** 14 tasks
- **Sẵn sàng bắt đầu:** 14 tasks
- **Dependencies:** All cleared ✅
- **Status:** READY TO START

---

## 📋 **FW Team Tasks - Priority Order**

### **FW-01: Khung FW + Scheduler** (3 ngày)
**Người phụ trách:** FW Lead  
**Phụ thuộc:** EM-01 ✅ (bring-up cơ bản)

**Mục tiêu:**
- Thiết kế firmware architecture với real-time scheduler
- Implement control loop với timing deterministic
- Setup interrupt priorities và latency measurement

**Deliverables:**
- [ ] Firmware architecture design
- [ ] Real-time scheduler implementation  
- [ ] Interrupt priority management
- [ ] Control loop với timing (1ms cycle)
- [ ] Latency measurement tools (< 100μs jitter)
- [ ] HAL abstraction layer foundation
- [ ] Error handling framework
- [ ] Unit tests cho scheduler

**Tài liệu tham chiếu:**
- `docs/specs/state_machine.md` - State machine design
- `docs/specs/control_spec.md` - Control requirements
- `docs/specs/safety.md` - Safety requirements

---

### **FW-02: HAL Abstraction** (4 ngày)
**Người phụ trách:** FW Engineer (HAL)  
**Phụ thuộc:** EM-02 ✅ (RS485 transceiver)

**Mục tiêu:**
- Implement HAL abstraction layer cho GPIO, PWM, ADC, UART/RS485, timer
- Tạo API ổn định cho hardware access
- Unit test tối thiểu cho HAL functions

**Deliverables:**
- [ ] HAL interface definition
- [ ] GPIO HAL implementation (input/output, debouncing, interrupt)
- [ ] PWM HAL implementation (frequency, duty cycle, dead time)
- [ ] ADC HAL implementation (sampling rate, filtering, calibration)
- [ ] UART/RS485 HAL implementation (baud rate, frame, CRC)
- [ ] Timer HAL implementation (periodic, one-shot, capture)
- [ ] Error handling tested (timeout, retry, recovery)
- [ ] Unit tests pass cho HAL functions

**Tài liệu tham chiếu:**
- `docs/specs/interfaces.md` - HAL interface specs
- `EMBED/hal_rs485.py` - EMBED RS485 implementation
- `docs/dev_radxa/pinout_radxa.md` - Pinout reference

---

### **FW-07: RS485 Protocol** (3 ngày)
**Người phụ trách:** FW Engineer (Comms)  
**Phụ thuộc:** EM-02 ✅ (RS485 transceiver)

**Mục tiêu:**
- Implement RS485 protocol với frame format, CRC calculation
- Retry mechanism và timeout handling
- Thống kê BER và error handling

**Deliverables:**
- [ ] RS485 protocol specification
- [ ] Frame format implementation (0xAA start, address, command, length, payload, CRC16)
- [ ] CRC16 calculation (Modbus/CCITT)
- [ ] Retry mechanism với backoff
- [ ] Timeout handling (3× cycle time)
- [ ] Error statistics và logging
- [ ] Basic commands (PING, HEARTBEAT, GET_INFO)
- [ ] Unit tests cho protocol

**Tài liệu tham chiếu:**
- `docs/specs/bus_rs485.md` - RS485 protocol specs
- `docs/specs/center_comm.md` - Communication requirements
- `EMBED/test_modbus_rtu.py` - EMBED Modbus implementation

---

## 🛠️ **Technical Requirements**

### **Platform & Tools**
- **Platform:** Orange Pi 5B (RK3588)
- **Language:** C/C++ cho embedded
- **Build System:** CMake hoặc Make
- **Debug:** GDB, JTAG (nếu có)
- **Testing:** Unit tests, integration tests

### **Real-time Constraints**
- **Control Loop:** 1ms cycle time
- **Latency:** < 100μs jitter
- **Interrupt Priority:** Control > Safety > Comms
- **Deterministic Behavior:** Required

### **Safety Requirements**
- **E-Stop Response:** < 100ms
- **Fault Handling:** Graceful degradation
- **Watchdog:** Hardware/software watchdog
- **Error Recovery:** Automatic recovery khi possible

---

## 📁 **File Structure & Organization**

### **FW Source Code Structure**
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
└── CMakeLists.txt
```

### **Integration với EMBED**
- **HAL Layer:** Sử dụng EMBED HAL interface
- **RS485:** Tích hợp với EMBED RS485 implementation
- **Testing:** Sử dụng EMBED test scripts

---

## 🎯 **Success Criteria**

### **FW-01 Success Criteria**
- [ ] Vòng lặp chính ổn định
- [ ] Scheduler deterministic với priority levels
- [ ] Latency measurement < 100μs jitter
- [ ] Interrupt priorities setup correctly
- [ ] Unit tests pass cho scheduler logic

### **FW-02 Success Criteria**
- [ ] API HAL ổn định
- [ ] Unit test tối thiểu cho HAL functions
- [ ] Performance validated
- [ ] Error handling working
- [ ] Integration với EMBED hardware

### **FW-07 Success Criteria**
- [ ] Tỉ lệ lỗi thấp (BER < threshold)
- [ ] Thống kê BER working
- [ ] Retry mechanism working
- [ ] Timeout handling working
- [ ] Basic commands functional

---

## 📅 **Timeline & Milestones**

### **Week 1 (FW-01 + FW-02)**
- **Day 1-3:** FW-01 - Khung FW + scheduler
- **Day 4-7:** FW-02 - HAL abstraction

### **Week 2 (FW-07 + Integration)**
- **Day 1-3:** FW-07 - RS485 protocol
- **Day 4-7:** Integration testing với EMBED

### **Week 3 (FW-03 + FW-09)**
- **Day 1-3:** FW-03 - State machine
- **Day 4-7:** FW-09 - E-Stop & interlock

---

## 🔧 **Development Environment Setup**

### **Required Tools**
```bash
# Install build tools
sudo apt update
sudo apt install build-essential cmake gdb

# Install development libraries
sudo apt install libgpiod-dev libserialport-dev

# Setup cross-compilation (if needed)
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

### **Project Setup**
```bash
# Clone project (if not already done)
cd /home/orangepi/OHT-50

# Create firmware directory
mkdir -p firmware/src firmware/include firmware/tests firmware/docs

# Setup build system
touch firmware/CMakeLists.txt
```

---

## 📞 **Communication & Support**

### **Daily Standup**
- **Thời gian:** 9:00 AM hàng ngày
- **Format:** 15 phút, 3 câu hỏi chuẩn
- **Tool:** Slack/Zoom

### **Weekly Review**
- **Thời gian:** Thứ 6, 2:00 PM
- **Agenda:** Progress review, blockers, next week planning
- **Output:** Updated PM tracker

### **Escalation Path**
1. **Technical Issues:** FW Engineer → FW Lead → PM
2. **Process Issues:** FW Team Member → PM
3. **Resource Issues:** PM → Management

---

## 📚 **Resources & References**

### **EMBED Deliverables**
- **Hardware:** UART1 RS485 ready với pins 46, 47
- **Software:** `EMBED/hal_rs485.py` - RS485 HAL implementation
- **Documentation:** `EMBED/README.md` - Complete EMBED documentation
- **Test Scripts:** `EMBED/test_*.py` - Test scripts for validation

### **Technical Documentation**
- **State Machine:** `docs/specs/state_machine.md`
- **Control Spec:** `docs/specs/control_spec.md`
- **Safety:** `docs/specs/safety.md`
- **RS485 Protocol:** `docs/specs/bus_rs485.md`
- **Hardware:** `docs/specs/hardware.md`
- **Pinout:** `docs/dev_radxa/pinout_radxa.md`

### **Prompt Templates**
- **FW Templates:** `docs/tasks/FW_PROMPT_TEMPLATE.md`
- **Programming Guide:** `docs/tasks/PROGRAMMING_PROMPT_OHT-50.md`

---

## 🎯 **Next Steps**

### **Immediate Actions (Today)**
1. **FW Team Kickoff Meeting:** Schedule kickoff meeting
2. **Environment Setup:** Setup development environment
3. **Task Assignment:** Assign FW-01, FW-02, FW-07 to team members
4. **Resource Allocation:** Ensure team has required resources

### **Week 1 Goals**
1. **FW-01 Complete:** Khung FW + scheduler working
2. **FW-02 Start:** HAL abstraction implementation
3. **Integration Planning:** Plan integration với EMBED

### **Week 2 Goals**
1. **FW-02 Complete:** HAL abstraction working
2. **FW-07 Complete:** RS485 protocol working
3. **Integration Testing:** Test với EMBED hardware

---

## 🚀 **Ready to Start!**

**FW Team có thể bắt đầu ngay lập tức!** 

EMBED team đã hoàn thành tất cả dependencies và cung cấp:
- ✅ Hardware ready (UART1 RS485)
- ✅ HAL interface complete
- ✅ Documentation comprehensive
- ✅ Test scripts functional

**Good luck và happy coding!** 🎉

---

**Contact:** PM OHT-50  
**Date:** 2025-01-27  
**Version:** 1.0
