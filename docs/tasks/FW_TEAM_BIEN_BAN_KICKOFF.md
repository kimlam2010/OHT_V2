# 📋 BIÊN BẢN KICKOFF FW TEAM - OHT-50

**Ngày:** 2025-01-27  
**Thời gian:** 14:55  
**Địa điểm:** Virtual Meeting  
**Chủ trì:** PM OHT-50  
**Thành phần tham dự:** FW Team, EMBED Team Lead

---

## 🎯 **Mục đích cuộc họp**

- Bàn giao deliverables từ EMBED team sang FW team
- Kickoff FW team với các task FW-01, FW-02, FW-07
- Xác nhận cấu trúc thư mục và development environment
- Lập kế hoạch timeline và milestones

---

## ✅ **EMBED Team Deliverables - HOÀN THÀNH**

### **Hardware Integration** ✅
- ✅ **UART1 RS485**: `/dev/ttyS1` hoạt động hoàn hảo
- ✅ **GPIO Pins**: Chân 46 (TX), 47 (RX) - ALT10 mode
- ✅ **Device Tree**: Custom overlay `uart1_46_47.dtbo` thành công
- ✅ **Boot Config**: `orangepiEnv.txt` đã cập nhật

### **Software Stack** ✅
- ✅ **HAL Interface**: `EMBED/hal_rs485.py` hoàn chỉnh và test OK
- ✅ **Modbus RTU**: Protocol implementation và test thành công
- ✅ **Test Scripts**: 8 scripts test đầy đủ
- ✅ **Udev Rules**: `/dev/ttyOHT485` symlink hoạt động

### **Documentation** ✅
- ✅ **File Organization**: `/home/orangepi/OHT-50/EMBED/` hoàn chỉnh
- ✅ **Progress Reports**: Báo cáo tiến độ chi tiết
- ✅ **Team Notices**: Thông báo team đầy đủ
- ✅ **Final Report**: Báo cáo cuối cùng hoàn chỉnh

---

## 🚀 **FW Team Kickoff - SẴN SÀNG BẮT ĐẦU**

### **Cấu trúc thư mục đã tạo** ✅
```
firmware/
├── src/
│   ├── scheduler/             # FW-01: Real-time scheduler
│   ├── hal/                   # FW-02: HAL abstraction
│   ├── comms/                 # FW-07: RS485 protocol
│   ├── state_machine/         # FW-03: State machine
│   ├── control/               # FW-04: Control loop
│   └── safety/                # FW-09: Safety mechanisms
├── include/                   # Header files
├── tests/                     # Unit tests
├── docs/                      # Documentation
├── CMakeLists.txt            # Build configuration
└── README.md                 # Project guide
```

### **Build System** ✅
- ✅ **CMakeLists.txt**: Cấu hình build system hoàn chỉnh
- ✅ **Dependencies**: gpiod, serialport, pthread, rt
- ✅ **Testing**: Unit test framework setup
- ✅ **Installation**: Target installation configured

---

## 📋 **FW Team Tasks - Priority Order**

### **FW-01: Khung FW + Scheduler** (3 ngày)
**Người phụ trách:** FW Lead  
**Phụ thuộc:** EM-01 ✅ (bring-up cơ bản)

**Deliverables:**
- [ ] Firmware architecture design
- [ ] Real-time scheduler implementation  
- [ ] Interrupt priority management
- [ ] Control loop với timing (1ms cycle)
- [ ] Latency measurement tools (< 100μs jitter)
- [ ] HAL abstraction layer foundation
- [ ] Error handling framework
- [ ] Unit tests cho scheduler

### **FW-02: HAL Abstraction** (4 ngày)
**Người phụ trách:** FW Engineer (HAL)  
**Phụ thuộc:** EM-02 ✅ (RS485 transceiver)

**Deliverables:**
- [ ] HAL interface definition
- [ ] GPIO HAL implementation (input/output, debouncing, interrupt)
- [ ] PWM HAL implementation (frequency, duty cycle, dead time)
- [ ] ADC HAL implementation (sampling rate, filtering, calibration)
- [ ] UART/RS485 HAL implementation (baud rate, frame, CRC)
- [ ] Timer HAL implementation (periodic, one-shot, capture)
- [ ] Error handling tested (timeout, retry, recovery)
- [ ] Unit tests pass cho HAL functions

### **FW-07: RS485 Protocol** (3 ngày)
**Người phụ trách:** FW Engineer (Comms)  
**Phụ thuộc:** EM-02 ✅ (RS485 transceiver)

**Deliverables:**
- [ ] RS485 protocol specification
- [ ] Frame format implementation (0xAA start, address, command, length, payload, CRC16)
- [ ] CRC16 calculation (Modbus/CCITT)
- [ ] Retry mechanism với backoff
- [ ] Timeout handling (3× cycle time)
- [ ] Error statistics và logging
- [ ] Basic commands (PING, HEARTBEAT, GET_INFO)
- [ ] Unit tests cho protocol

---

## 🛠️ **Development Environment**

### **Required Tools** ✅
```bash
# Install build tools
sudo apt update
sudo apt install build-essential cmake gdb

# Install development libraries
sudo apt install libgpiod-dev libserialport-dev

# Setup cross-compilation (if needed)
sudo apt install gcc-aarch64-linux-gnu g++-aarch64-linux-gnu
```

### **Build Instructions** ✅
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

## 🎯 **Next Steps**

### **Immediate Actions (Today)**
1. **FW Team Environment Setup:** Setup development environment
2. **Task Assignment:** Assign FW-01, FW-02, FW-07 to team members
3. **Resource Allocation:** Ensure team has required resources
4. **Kickoff Meeting:** Schedule detailed technical kickoff

### **Week 1 Goals**
1. **FW-01 Complete:** Khung FW + scheduler working
2. **FW-02 Start:** HAL abstraction implementation
3. **Integration Planning:** Plan integration với EMBED

### **Week 2 Goals**
1. **FW-02 Complete:** HAL abstraction working
2. **FW-07 Complete:** RS485 protocol working
3. **Integration Testing:** Test với EMBED hardware

---

## ✅ **Quyết định & Cam kết**

### **EMBED Team** ✅
- ✅ Hoàn thành 100% deliverables
- ✅ Bàn giao hardware và software cho FW team
- ✅ Sẵn sàng hỗ trợ integration testing

### **FW Team** 🚀
- 🚀 Sẵn sàng bắt đầu development
- 🚀 Cam kết tuân thủ timeline
- 🚀 Cam kết quality standards

### **PM** 📋
- 📋 Theo dõi progress hàng ngày
- 📋 Hỗ trợ giải quyết blockers
- 📋 Cập nhật documentation

---

## 📝 **Ghi chú**

- **Cấu trúc thư mục firmware đã được tạo đúng quy chuẩn**
- **Build system CMake đã được cấu hình hoàn chỉnh**
- **Tất cả dependencies từ EMBED team đã được clear**
- **FW team có thể bắt đầu ngay lập tức**

---

**Chủ trì:** PM OHT-50  
**Thư ký:** [Tên thư ký]  
**Ngày:** 2025-01-27  
**Thời gian:** 14:55  
**Version:** 1.0

**Chữ ký:**
- PM OHT-50: _________________
- EMBED Team Lead: _________________
- FW Team Lead: _________________
