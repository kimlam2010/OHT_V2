# FW PROMPT TEMPLATE - OHT-50

## Template Prompt cho FW Team

```
Bạn là FW Lead/Engineer trong team OHT-50.

CONTEXT:
- Dự án: Firmware điều khiển thời gian thực cho OHT-50
- Tech stack: C/C++ cho embedded, HAL layer
- Tài liệu: docs/specs/state_machine.md, control_spec.md, safety.md
- Rule: Real-time constraints, safety critical, deterministic behavior
- Platform: Orange Pi 5B (RK3588) với HAL abstraction

TASK: [FW-XX] - [TÊN TASK]
Mô tả: [COPY TỪ PM_TASKS_OHT-50.md]
DOD: [COPY TỪ PM_TASKS_OHT-50.md]
Phụ thuộc: [COPY TỪ PM_TASKS_OHT-50.md]
Ước lượng: [X] ngày

YÊU CẦU:
1. Tuân thủ state machine trong docs/specs/state_machine.md
2. Real-time constraints (latency < Xms)
3. Safety critical code theo docs/specs/safety.md
4. HAL abstraction layer
5. Deterministic behavior
6. Comprehensive error handling
7. Unit tests cho logic
8. RS485 protocol implementation

IMPLEMENTATION:
1. Thiết kế architecture theo HAL
2. Implement core logic
3. Add safety mechanisms
4. Optimize for real-time
5. Write unit tests
6. Integration testing
7. Performance validation
8. RS485 communication

DELIVERABLES:
- [ ] Firmware source code
- [ ] HAL interface defined
- [ ] Unit tests pass
- [ ] Real-time performance verified
- [ ] Safety mechanisms tested
- [ ] Documentation updated
- [ ] RS485 protocol working

SPECIFIC REQUIREMENTS:
- State machine: Idle/Move/Dock/Fault/E-Stop
- Control loop: v-a-x profile
- Safety: E-Stop, interlock, watchdog
- RS485: frame, CRC, retry, timeout
- HAL: GPIO, UART, timer, PWM
- Real-time: deterministic timing
- Error handling: graceful degradation
```

## Prompt cho FW-01: Khung FW, Scheduler, Ưu tiên ngắt

```
Bạn là FW Lead/Engineer trong team OHT-50.

CONTEXT:
- Dự án: Firmware điều khiển thời gian thực cho OHT-50
- Tech stack: C/C++ cho embedded, HAL layer
- Tài liệu: docs/specs/state_machine.md, control_spec.md, safety.md
- Rule: Real-time constraints, safety critical, deterministic behavior
- Platform: Orange Pi 5B (RK3588) với HAL abstraction

TASK: FW-01 - Khung FW, scheduler, ưu tiên ngắt
Mô tả: Thiết kế và implement khung firmware với scheduler real-time, ưu tiên ngắt cho control loop
DOD: Vòng lặp chính ổn định; đo latency cơ bản; scheduler deterministic
Phụ thuộc: EM-01 (bring-up cơ bản)
Ước lượng: 3 ngày

YÊU CẦU:
1. Tuân thủ state machine trong docs/specs/state_machine.md
2. Real-time scheduler với priority levels
3. Interrupt priority management
4. Control loop timing deterministic
5. Latency measurement
6. HAL abstraction layer
7. Error handling framework
8. Unit tests cho scheduler

IMPLEMENTATION:
1. Thiết kế firmware architecture
2. Implement real-time scheduler
3. Setup interrupt priorities
4. Implement control loop
5. Add latency measurement
6. Create HAL abstraction
7. Add error handling
8. Write unit tests

DELIVERABLES:
- [ ] Firmware architecture design
- [ ] Real-time scheduler implementation
- [ ] Interrupt priority management
- [ ] Control loop với timing
- [ ] Latency measurement tools
- [ ] HAL abstraction layer
- [ ] Error handling framework
- [ ] Unit tests pass

SPECIFIC REQUIREMENTS:
- Scheduler: preemptive, priority-based
- Interrupt priorities: control > safety > comms
- Control loop: 1ms cycle time
- Latency measurement: < 100μs jitter
- HAL: GPIO, UART, timer, PWM
- Error handling: graceful degradation
- Unit tests: scheduler logic
```

## Prompt cho FW-02: HAL - GPIO, PWM, ADC, UART/RS485, Timer

```
Bạn là FW Lead/Engineer trong team OHT-50.

CONTEXT:
- Dự án: Firmware điều khiển thời gian thực cho OHT-50
- Tech stack: C/C++ cho embedded, HAL layer
- Tài liệu: docs/specs/state_machine.md, control_spec.md, safety.md
- Rule: Real-time constraints, safety critical, deterministic behavior
- Platform: Orange Pi 5B (RK3588) với HAL abstraction

TASK: FW-02 - HAL: GPIO, PWM, ADC, UART/RS485, timer
Mô tả: Implement HAL abstraction layer cho GPIO, PWM, ADC, UART/RS485, timer với API ổn định
DOD: API HAL ổn định; unit test tối thiểu; performance validated; error handling
Phụ thuộc: EM-02 (RS485 transceiver)
Ước lượng: 4 ngày

YÊU CẦU:
1. Tuân thủ HAL abstraction trong docs/specs/interfaces.md
2. GPIO control với debouncing
3. PWM generation với frequency control
4. ADC reading với filtering
5. UART/RS485 communication
6. Timer management
7. Error handling và recovery
8. Unit tests cho HAL functions

IMPLEMENTATION:
1. Thiết kế HAL interface
2. Implement GPIO HAL
3. Implement PWM HAL
4. Implement ADC HAL
5. Implement UART/RS485 HAL
6. Implement timer HAL
7. Add error handling
8. Write unit tests

DELIVERABLES:
- [ ] HAL interface definition
- [ ] GPIO HAL implementation
- [ ] PWM HAL implementation
- [ ] ADC HAL implementation
- [ ] UART/RS485 HAL implementation
- [ ] Timer HAL implementation
- [ ] Error handling tested
- [ ] Unit tests pass

SPECIFIC REQUIREMENTS:
- GPIO: input/output, debouncing, interrupt
- PWM: frequency, duty cycle, dead time
- ADC: sampling rate, filtering, calibration
- UART/RS485: baud rate, frame, CRC
- Timer: periodic, one-shot, capture
- Error handling: timeout, retry, recovery
- Unit tests: HAL functions
```

## Prompt cho FW-07: RS485 Protocol - Frame, CRC, Retry

```
Bạn là FW Lead/Engineer trong team OHT-50.

CONTEXT:
- Dự án: Firmware điều khiển thời gian thực cho OHT-50
- Tech stack: C/C++ cho embedded, HAL layer
- Tài liệu: docs/specs/state_machine.md, bus_rs485.md, center_comm.md
- Rule: Real-time constraints, safety critical, deterministic behavior
- Platform: Orange Pi 5B (RK3588) với HAL abstraction

TASK: FW-07 - RS485 protocol: frame, CRC, retry
Mô tả: Implement RS485 protocol với frame format, CRC calculation, retry mechanism, timeout handling
DOD: Tỉ lệ lỗi thấp; thống kê BER; retry mechanism working; timeout handling
Phụ thuộc: EM-02 (RS485 transceiver)
Ước lượng: 3 ngày

YÊU CẦU:
1. Tuân thủ protocol trong docs/specs/bus_rs485.md
2. Frame format với header, data, CRC
3. CRC calculation và validation
4. Retry mechanism với backoff
5. Timeout handling
6. Error statistics (BER)
7. Performance optimization
8. Unit tests cho protocol

IMPLEMENTATION:
1. Thiết kế frame format
2. Implement CRC calculation
3. Implement retry mechanism
4. Add timeout handling
5. Add error statistics
6. Optimize performance
7. Add error handling
8. Write unit tests

DELIVERABLES:
- [ ] Frame format specification
- [ ] CRC calculation implementation
- [ ] Retry mechanism working
- [ ] Timeout handling tested
- [ ] Error statistics collection
- [ ] Performance optimization
- [ ] Error handling tested
- [ ] Unit tests pass

SPECIFIC REQUIREMENTS:
- Frame: header, data, CRC-16
- CRC: polynomial, calculation, validation
- Retry: exponential backoff, max retries
- Timeout: configurable, adaptive
- BER: bit error rate calculation
- Performance: < 1ms latency
- Unit tests: protocol functions
```

## Prompt cho FW-03: State Machine - Idle/Move/Dock/Fault/E-Stop

```
Bạn là FW Lead/Engineer trong team OHT-50.

CONTEXT:
- Dự án: Firmware điều khiển thời gian thực cho OHT-50
- Tech stack: C/C++ cho embedded, HAL layer
- Tài liệu: docs/specs/state_machine.md, control_spec.md, safety.md
- Rule: Real-time constraints, safety critical, deterministic behavior
- Platform: Orange Pi 5B (RK3588) với HAL abstraction

TASK: FW-03 - State machine: Idle/Move/Dock/Fault/E-Stop
Mô tả: Implement state machine cho các trạng thái Idle, Move, Dock, Fault, E-Stop với transition logic
DOD: Unit test đạt; state transitions correct; safety mechanisms working; error handling
Phụ thuộc: FW-01 (khung FW)
Ước lượng: 3 ngày

YÊU CẦU:
1. Tuân thủ state machine trong docs/specs/state_machine.md
2. State transitions với validation
3. Safety mechanisms cho E-Stop
4. Error handling cho Fault state
5. State persistence
6. State logging
7. Unit tests cho state machine
8. Integration với control loop

IMPLEMENTATION:
1. Thiết kế state machine
2. Implement state transitions
3. Add safety mechanisms
4. Add error handling
5. Add state persistence
6. Add state logging
7. Write unit tests
8. Integrate với control loop

DELIVERABLES:
- [ ] State machine design
- [ ] State transitions implementation
- [ ] Safety mechanisms working
- [ ] Error handling tested
- [ ] State persistence working
- [ ] State logging functional
- [ ] Unit tests pass
- [ ] Control loop integration

SPECIFIC REQUIREMENTS:
- States: Idle, Move, Dock, Fault, E-Stop
- Transitions: validated, logged
- Safety: E-Stop priority, interlock
- Error handling: graceful degradation
- Persistence: state recovery
- Logging: state changes, events
- Unit tests: state machine logic
```

## Prompt cho FW-09: E-Stop & Interlock

```
Bạn là FW Lead/Engineer trong team OHT-50.

CONTEXT:
- Dự án: Firmware điều khiển thời gian thực cho OHT-50
- Tech stack: C/C++ cho embedded, HAL layer
- Tài liệu: docs/specs/state_machine.md, safety.md, safety_estop_interlock.md
- Rule: Real-time constraints, safety critical, deterministic behavior
- Platform: Orange Pi 5B (RK3588) với HAL abstraction

TASK: FW-09 - E-Stop & interlock
Mô tả: Implement E-Stop mechanism và interlock system với safety validation
DOD: Fault injection đạt; E-Stop response time < 100ms; interlock working; safety validated
Phụ thuộc: FW-03 (state machine)
Ước lượng: 4 ngày

YÊU CẦU:
1. Tuân thủ safety requirements trong docs/specs/safety.md
2. E-Stop mechanism với hardware input
3. Interlock system với validation
4. Safety validation với fault injection
5. Response time measurement
6. Error handling và recovery
7. Unit tests cho safety
8. Integration testing

IMPLEMENTATION:
1. Thiết kế E-Stop mechanism
2. Implement interlock system
3. Add safety validation
4. Add fault injection
5. Measure response time
6. Add error handling
7. Write unit tests
8. Integration testing

DELIVERABLES:
- [ ] E-Stop mechanism implementation
- [ ] Interlock system working
- [ ] Safety validation tested
- [ ] Fault injection working
- [ ] Response time measured
- [ ] Error handling tested
- [ ] Unit tests pass
- [ ] Integration tests pass

SPECIFIC REQUIREMENTS:
- E-Stop: hardware input, software response
- Interlock: multiple conditions, validation
- Safety: fault injection, validation
- Response time: < 100ms
- Error handling: graceful degradation
- Unit tests: safety functions
- Integration: end-to-end testing
```

## FW-11: LiDAR Integration

### **Mục tiêu**
Integrate LiDAR với FW framework, implement obstacle detection, navigation integration, và safety system integration.

### **Yêu cầu kỹ thuật**
- **LiDAR Driver Integration**: Integration với FW framework
- **Obstacle Detection**: Real-time obstacle detection algorithm
- **Navigation Integration**: Path planning với obstacle avoidance
- **Safety Integration**: E-Stop và speed control dựa trên LiDAR data
- **Real-time Processing**: < 100ms response time

### **Deliverables**
1. **LiDAR Driver Integration**: Integration với FW framework
2. **Obstacle Detection Algorithm**: Real-time obstacle detection
3. **Navigation Integration**: Path planning và collision avoidance
4. **Safety Integration**: E-Stop và speed control
5. **Test Scripts**: Integration test scripts
6. **Documentation**: Integration documentation

### **SPECIFIC REQUIREMENTS**
- LiDAR driver integration với FW framework
- Real-time obstacle detection algorithm
- Navigation system integration với path planning
- Safety system integration với E-Stop và speed control
- Performance optimization cho real-time processing
- Comprehensive test coverage
- Integration documentation

### **Prompt Template**
```
Bạn là FW team member của dự án OHT-50, chịu trách nhiệm LiDAR integration.

## TASK: FW-11 - LiDAR Integration

### Mục tiêu
Integrate LiDAR với FW framework, implement obstacle detection, navigation integration, và safety system integration.

### Yêu cầu kỹ thuật
- LiDAR Driver Integration: Integration với FW framework
- Obstacle Detection: Real-time obstacle detection algorithm
- Navigation Integration: Path planning với obstacle avoidance
- Safety Integration: E-Stop và speed control dựa trên LiDAR data
- Real-time Processing: < 100ms response time

### Deliverables cần tạo
1. LiDAR Driver Integration: Integration với FW framework
2. Obstacle Detection Algorithm: Real-time obstacle detection
3. Navigation Integration: Path planning và collision avoidance
4. Safety Integration: E-Stop và speed control
5. Test Scripts: Integration test scripts
6. Documentation: Integration documentation

### SPECIFIC REQUIREMENTS
- LiDAR driver integration với FW framework
- Real-time obstacle detection algorithm
- Navigation system integration với path planning
- Safety system integration với E-Stop và speed control
- Performance optimization cho real-time processing
- Comprehensive test coverage
- Integration documentation

### Hardware Platform
- Orange Pi 5B (RK3588)
- USB LiDAR sensor (RPLIDAR A1/A2)
- Real-time Linux kernel

### Software Stack
- C/C++ cho real-time processing
- Python cho LiDAR driver
- Threading cho concurrent operations
- Real-time scheduling

### Integration Requirements
- Integration với existing FW framework
- Real-time data processing
- Obstacle detection algorithm
- Navigation system integration
- Safety system integration
- Performance optimization

### Testing Requirements
- Integration test với FW framework
- Obstacle detection validation
- Navigation integration test
- Safety system test
- Performance benchmarking
- Real-time response test

### Documentation Requirements
- Integration guide
- API documentation
- Test procedures
- Performance metrics
- Troubleshooting guide

Hãy implement LiDAR integration theo yêu cầu trên và tạo đầy đủ deliverables.
```
