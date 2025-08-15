# HƯỚNG DẪN SỬ DỤNG PROMPT TEMPLATES - OHT-50

## Tổng quan

Prompt templates được tạo để chuẩn hóa quy trình thực hiện task cho EMBED và FW teams trong dự án OHT-50. Mỗi template cung cấp context, yêu cầu, implementation steps và deliverables cụ thể.

## Cấu trúc Prompt Template

### 1. Context & Background
```
Bạn là [VỊ TRÍ] trong team OHT-50.

CONTEXT:
- Dự án: [Mô tả dự án]
- Tech stack: [Công nghệ sử dụng]
- Tài liệu: [Tài liệu tham chiếu]
- Rule: [Quy tắc cần tuân thủ]
- Platform: [Nền tảng phần cứng]
```

### 2. Task Specification
```
TASK: [ID] - [TÊN TASK]
Mô tả: [COPY TỪ PM_TASKS_OHT-50.md]
DOD: [COPY TỪ PM_TASKS_OHT-50.md]
Phụ thuộc: [COPY TỪ PM_TASKS_OHT-50.md]
Ước lượng: [X] ngày
```

### 3. Technical Requirements
```
YÊU CẦU:
1. [Yêu cầu kỹ thuật 1]
2. [Yêu cầu kỹ thuật 2]
...
```

### 4. Implementation Steps
```
IMPLEMENTATION:
1. [Bước thực hiện 1]
2. [Bước thực hiện 2]
...
```

### 5. Deliverables & DOD Check
```
DELIVERABLES:
- [ ] [Deliverable 1]
- [ ] [Deliverable 2]
...
```

## Prompt Templates có sẵn

### EMBED Team Templates

**File:** `docs/tasks/EMBED_PROMPT_TEMPLATE.md`

**Template chung:**
- Context: Hardware bring-up và low-level drivers cho OHT-50
- Platform: Orange Pi 5B (RK3588)
- Tech stack: C/C++, libgpiod, udev rules, device tree overlays
- Rule: Hardware abstraction, EMI/ESD compliance, production ready

**Prompts cụ thể:**

1. **EM-11: UART1 Enable và Validation**
   - Mục tiêu: Enable UART1 trong DT/overlay, xác nhận /dev/ttyS1
   - Tech: Device tree overlay, udev rules
   - Deliverables: DT overlay, /dev/ttyS1 working, udev rules

3. **EM-02: RS485/CAN Transceiver**
   - Mục tiêu: Thiết kế và implement RS485 transceiver
   - Tech: RS485 transceiver circuit, DE/RE control
   - Deliverables: Circuit design, DE/RE control, EMI guidelines

4. **EM-03: UART/CAN Init + DMA Ring Buffer**
   - Mục tiêu: Implement UART với DMA ring buffer
   - Tech: UART driver, DMA ring buffer, baud rate support
   - Deliverables: UART driver, DMA buffer, performance validation

### FW Team Templates

**File:** `docs/tasks/FW_PROMPT_TEMPLATE.md`

**Template chung:**
- Context: Firmware điều khiển thời gian thực cho OHT-50
- Tech stack: C/C++ cho embedded, HAL layer
- Rule: Real-time constraints, safety critical, deterministic behavior
- Platform: Orange Pi 5B (RK3588) với HAL abstraction

**Prompts cụ thể:**

1. **FW-01: Khung FW, Scheduler, Ưu tiên ngắt**
   - Mục tiêu: Thiết kế firmware architecture với real-time scheduler
   - Tech: Real-time scheduler, interrupt priorities, control loop
   - Deliverables: Architecture design, scheduler, latency measurement

2. **FW-02: HAL - GPIO, PWM, ADC, UART/RS485, Timer**
   - Mục tiêu: Implement HAL abstraction layer
   - Tech: HAL interface, GPIO/PWM/ADC/UART/Timer drivers
   - Deliverables: HAL interface, driver implementations, unit tests

3. **FW-07: RS485 Protocol - Frame, CRC, Retry**
   - Mục tiêu: Implement RS485 protocol với error handling
   - Tech: Frame format, CRC calculation, retry mechanism
   - Deliverables: Protocol specification, CRC implementation, error stats

4. **FW-03: State Machine - Idle/Move/Dock/Fault/E-Stop**
   - Mục tiêu: Implement state machine cho control system
   - Tech: State transitions, safety mechanisms, error handling
   - Deliverables: State machine, transitions, safety mechanisms

5. **FW-09: E-Stop & Interlock**
   - Mục tiêu: Implement safety mechanisms
   - Tech: E-Stop mechanism, interlock system, fault injection
   - Deliverables: E-Stop implementation, interlock system, safety validation

## Cách sử dụng Prompt Templates

### Bước 1: Chọn Template
1. Xác định task ID (EM-XX hoặc FW-XX)
2. Mở file template tương ứng
3. Tìm prompt cho task cụ thể

### Bước 2: Copy và Customize
1. Copy prompt template
2. Điền thông tin task cụ thể từ `PM_TASKS_OHT-50.md`
3. Customize theo context và requirements
4. Thêm specific requirements nếu cần

### Bước 3: Execute
1. Thực hiện từng bước theo implementation steps
2. Validate theo DOD checklist
3. Update progress trong PM tracker
4. Tạo deliverables theo yêu cầu

### Bước 4: Review và Update
1. Code review trước khi merge
2. Update documentation
3. Update test results
4. Update PM tracker với status

## Ví dụ sử dụng



### Ví dụ 1: FW-01 - Khung FW, Scheduler, Ưu tiên ngắt

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

## Lưu ý quan trọng

### 1. Tuân thủ quy trình
- Luôn tham chiếu tài liệu trong `docs/specs/` và `docs/dev_radxa/`
- Tuân thủ rule: Python/Node.js, tiếng Việt, thiết kế trước code
- Update PM tracker sau khi hoàn thành task
- Code review bắt buộc trước merge
- Demo theo DOD trước khi đóng task

### 2. Dependencies và Critical Path
- EMBED team cần hoàn thành EM-02 và EM-03 trước khi FW team có thể bắt đầu FW-02 và FW-07
- EM-10 và EM-11 có deadline 2025-08-18, cần ưu tiên cao
- FW team có thể bắt đầu FW-01 song song với EM-02/EM-03
- Safety mechanisms (FW-09) cần được implement sau khi state machine (FW-03) hoàn thành

### 3. Quality Assurance
- Unit tests bắt buộc cho tất cả logic
- Integration testing cần được thực hiện sau khi cả EMBED và FW đều hoàn thành các task cơ bản
- Performance validation cho real-time constraints
- Safety validation cho critical systems

### 4. Documentation
- Tất cả tài liệu phải bằng tiếng Việt
- Cập nhật tài liệu kỹ thuật sau khi hoàn thành task
- Tạo test reports và validation documents
- Update changelog và version history

## Tài liệu tham chiếu

- **EMBED Prompts:** `docs/tasks/EMBED_PROMPT_TEMPLATE.md`
- **FW Prompts:** `docs/tasks/FW_PROMPT_TEMPLATE.md`
- **Programming Prompts:** `docs/tasks/PROGRAMMING_PROMPT_OHT-50.md`
- **PM Tasks:** `docs/tasks/PM_TASKS_OHT-50.md`
- **Test Checklist:** `docs/specs/EMBED_TEST_CHECKLIST.md`
- **Hardware Specs:** `docs/specs/hardware.md`, `docs/dev_radxa/pinout_radxa.md`

## Support và Escalation

Nếu gặp vấn đề trong quá trình thực hiện task:

1. **Technical Issues:** Tham khảo tài liệu kỹ thuật trong `docs/specs/`
2. **Process Issues:** Liên hệ PM để được hướng dẫn
3. **Dependency Issues:** Cập nhật PM tracker và thông báo blockers
4. **Quality Issues:** Sử dụng checklist và validation procedures

---

**Phiên bản:** v1.0  
**Cập nhật:** 2024-12-19  
**Người tạo:** PM Team OHT-50
