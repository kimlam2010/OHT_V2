# Production Test Checklist - OHT-50

## 📋 **Tổng quan**
Production test checklist cho OHT-50 với versioned checklist và mẫu biểu ghi nhận.

**Ngày tạo:** 2025-01-27  
**Phiên bản:** v1.0  
**Trạng thái:** Draft

---

## 🎯 **Mục tiêu**

### **Production Testing**
- Đảm bảo chất lượng sản phẩm trước khi xuất xưởng
- Phát hiện lỗi sớm trong quá trình sản xuất
- Chuẩn hóa quy trình kiểm tra
- Lưu trữ kết quả test để traceability

---

## 📋 **Production Test Checklist**

### **1. Hardware Inspection**

#### **Visual Inspection**
- [ ] **PCB Assembly**: Kiểm tra linh kiện đúng vị trí
- [ ] **Soldering Quality**: Kiểm tra chất lượng hàn
- [ ] **Component Orientation**: Kiểm tra hướng linh kiện
- [ ] **Physical Damage**: Kiểm tra hư hỏng vật lý
- [ ] **Labeling**: Kiểm tra nhãn mác, serial number

#### **Power Supply Test**
- [ ] **3.3V Supply**: Đo điện áp 3.3V ±5%
- [ ] **5V Supply**: Đo điện áp 5V ±5%
- [ ] **12V Supply**: Đo điện áp 12V ±5%
- [ ] **Current Draw**: Đo dòng tiêu thụ
- [ ] **Power Sequencing**: Kiểm tra thứ tự bật nguồn

### **2. GPIO Test**

#### **Digital Inputs**
- [ ] **GPIO48 (Encoder A)**: Kiểm tra input với pull-up
- [ ] **GPIO49 (Encoder B)**: Kiểm tra input với pull-up
- [ ] **GPIO50 (Encoder Z)**: Kiểm tra input với pull-up
- [ ] **GPIO51 (Limit Front)**: Kiểm tra input với pull-up
- [ ] **GPIO52 (Limit Back)**: Kiểm tra input với pull-up
- [ ] **GPIO53 (E-Stop)**: Kiểm tra input với pull-up

#### **Digital Outputs**
- [ ] **GPIO46 (UART1 TX)**: Kiểm tra output function
- [ ] **GPIO47 (UART1 RX)**: Kiểm tra input function
- [ ] **Relay Outputs**: Kiểm tra relay control

### **3. Communication Test**

#### **UART1 RS485**
- [ ] **Device Detection**: Kiểm tra `/dev/ttyS1` tồn tại
- [ ] **Baudrate Test**: Test 115200 baud
- [ ] **Loopback Test**: Gửi/nhận dữ liệu
- [ ] **Modbus RTU**: Test protocol với slave address 2
- [ ] **CRC Validation**: Kiểm tra CRC calculation
- [ ] **Error Handling**: Test timeout và retry

#### **SPI Interface**
- [ ] **SPI0 Detection**: Kiểm tra SPI interface
- [ ] **ADC Communication**: Test ADC reading
- [ ] **Data Integrity**: Kiểm tra tính toàn vẹn dữ liệu

### **4. Functionality Test**

#### **Encoder Simulation**
- [ ] **Encoder A Signal**: Generate 100Hz signal
- [ ] **Encoder B Signal**: Generate quadrature signal
- [ ] **Encoder Z Signal**: Generate index pulse
- [ ] **Count Accuracy**: Kiểm tra độ chính xác đếm
- [ ] **Direction Detection**: Test clockwise/counter-clockwise

#### **Limit Switch Test**
- [ ] **Front Limit**: Activate/deactivate switch
- [ ] **Back Limit**: Activate/deactivate switch
- [ ] **Debounce Function**: Kiểm tra debounce 50ms
- [ ] **Interrupt Response**: Test interrupt handling

#### **E-Stop Test**
- [ ] **E-Stop Activation**: Activate E-Stop
- [ ] **Response Time**: Đo thời gian phản hồi < 100ms
- [ ] **System Shutdown**: Kiểm tra shutdown sequence
- [ ] **Recovery**: Test recovery after E-Stop release

### **5. Performance Test**

#### **Speed Test**
- [ ] **Low Speed**: Test 50Hz encoder, 500RPM motor
- [ ] **Medium Speed**: Test 200Hz encoder, 1500RPM motor
- [ ] **High Speed**: Test 500Hz encoder, 2500RPM motor
- [ ] **Stability**: Kiểm tra ổn định trong 10 phút

#### **Accuracy Test**
- [ ] **Position Accuracy**: ±1mm tolerance
- [ ] **Velocity Accuracy**: ±1% tolerance
- [ ] **Acceleration Accuracy**: ±2% tolerance

### **6. Safety Test**

#### **Safety Functions**
- [ ] **E-Stop Response**: < 100ms activation
- [ ] **Limit Switch Response**: < 200ms activation
- [ ] **Multiple Safety Inputs**: Test simultaneous activation
- [ ] **Fault Detection**: Test fault injection

#### **Emergency Functions**
- [ ] **Emergency Stop**: Immediate system halt
- [ ] **Safe Shutdown**: Proper shutdown sequence
- [ ] **Recovery Mode**: System recovery after fault
- [ ] **Fault Logging**: Log fault events

### **7. Environmental Test**

#### **Temperature Test**
- [ ] **Operating Temperature**: 0°C to 50°C
- [ ] **Storage Temperature**: -20°C to 70°C
- [ ] **Thermal Cycling**: 10 cycles
- [ ] **Thermal Stability**: Stable operation

#### **EMI/ESD Test**
- [ ] **ESD Immunity**: 8kV contact discharge
- [ ] **EMI Susceptibility**: 10V/m field strength
- [ ] **Conducted Emissions**: Class B limits
- [ ] **Radiated Emissions**: Class B limits

### **8. Reliability Test**

#### **Long-term Test**
- [ ] **24-hour Test**: Continuous operation
- [ ] **7-day Test**: Extended operation
- [ ] **Stress Test**: Maximum load conditions
- [ ] **Aging Test**: Accelerated aging

#### **MTBF Calculation**
- [ ] **Failure Rate**: Calculate failure rate
- [ ] **MTBF**: Mean Time Between Failures
- [ ] **Reliability**: 99.9% uptime target
- [ ] **Warranty**: 2-year warranty period

---

## 📊 **Test Results Template**

### **Production Test Report**
```
OHT-50 Production Test Report
============================

Product Information:
- Model: OHT-50
- Serial Number: OHT50-2025-001
- Test Date: 2025-01-27
- Test Engineer: [Name]
- Test Location: [Location]

Test Results Summary:
- Hardware Inspection: PASS/FAIL
- GPIO Test: PASS/FAIL
- Communication Test: PASS/FAIL
- Functionality Test: PASS/FAIL
- Performance Test: PASS/FAIL
- Safety Test: PASS/FAIL
- Environmental Test: PASS/FAIL
- Reliability Test: PASS/FAIL

Overall Result: PASS/FAIL

Detailed Results:
[Test Category] [Test Item] [Result] [Notes]
Hardware       PCB Assembly PASS    All components correct
Hardware       Power Supply PASS    3.3V=3.31V, 5V=5.02V
GPIO           Encoder A    PASS    Input working correctly
GPIO           Encoder B    PASS    Input working correctly
Communication  UART1 RS485  PASS    Modbus RTU working
Functionality  E-Stop       PASS    Response time 85ms
Performance    Speed Test   PASS    All speeds stable
Safety         E-Stop       PASS    < 100ms response
Environmental  Temperature  PASS    0-50°C operation
Reliability    Long-term    PASS    24h continuous OK

Issues Found:
- None

Recommendations:
- None

Test Engineer Signature: _________________
Date: _________________
```

### **Test Data Log**
```
Test Data Log - OHT-50 Serial: OHT50-2025-001
==============================================

Timestamp: 2025-01-27 14:30:00
Test Phase: Power Supply Test

Measurements:
- 3.3V Supply: 3.31V (Target: 3.3V ±5%)
- 5V Supply: 5.02V (Target: 5V ±5%)
- 12V Supply: 12.05V (Target: 12V ±5%)
- Current Draw: 1.2A (Target: < 2A)

Status: PASS

Timestamp: 2025-01-27 14:35:00
Test Phase: Communication Test

Measurements:
- UART1 Baudrate: 115200 bps
- Modbus Response: OK
- CRC Validation: PASS
- Response Time: 8ms

Status: PASS

Timestamp: 2025-01-27 14:40:00
Test Phase: Safety Test

Measurements:
- E-Stop Response Time: 85ms (Target: < 100ms)
- Limit Switch Response: 120ms (Target: < 200ms)
- Multiple Safety Inputs: PASS
- Fault Detection: PASS

Status: PASS
```

---

## 🚨 **Failure Analysis**

### **Common Failures**
1. **Power Supply Failure**
   - **Symptoms**: Voltage out of range
   - **Causes**: Bad regulator, wrong component
   - **Actions**: Replace power supply components

2. **Communication Failure**
   - **Symptoms**: No response from RS485
   - **Causes**: Bad transceiver, wiring issue
   - **Actions**: Check wiring, replace transceiver

3. **GPIO Failure**
   - **Symptoms**: Input/output not working
   - **Causes**: Bad GPIO, wrong configuration
   - **Actions**: Check configuration, replace component

4. **Safety Function Failure**
   - **Symptoms**: E-Stop not responding
   - **Causes**: Bad E-Stop circuit, software issue
   - **Actions**: Check circuit, debug software

### **Corrective Actions**
1. **Immediate Actions**: Stop production, isolate defective units
2. **Root Cause Analysis**: Investigate failure cause
3. **Corrective Measures**: Implement fixes
4. **Preventive Actions**: Update procedures to prevent recurrence
5. **Documentation**: Update test procedures and documentation

---

## 📈 **Quality Metrics**

### **Production Metrics**
- **First Pass Yield**: Target > 95%
- **Test Time**: Target < 30 minutes per unit
- **Defect Rate**: Target < 2%
- **Rework Rate**: Target < 1%

### **Quality Control**
- **Incoming Inspection**: 100% of components
- **In-Process Inspection**: Key checkpoints
- **Final Inspection**: 100% of finished products
- **Sample Testing**: Statistical sampling

---

## 📚 **References**

### **Standards**
- **ISO 9001**: Quality management systems
- **IEC 61000-4-2**: ESD immunity test
- **IEC 61000-4-3**: EMI susceptibility test
- **CISPR 22**: Information technology equipment emissions

### **Procedures**
- **SOP-001**: Production test procedure
- **SOP-002**: Failure analysis procedure
- **SOP-003**: Corrective action procedure
- **SOP-004**: Quality control procedure

---

**Changelog:**
- v1.0 (2025-01-27): Initial version with comprehensive production test checklist
