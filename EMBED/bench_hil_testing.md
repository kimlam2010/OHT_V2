# Bench HIL Testing - OHT-50

## 📋 **Tổng quan**
Tài liệu mô tả bench HIL (Hardware-in-the-Loop) testing cho OHT-50 với fixture encoder/motor ảo.

**Ngày tạo:** 2025-01-27  
**Phiên bản:** v1.0  
**Trạng thái:** Draft

---

## 🎯 **Mục tiêu**

### **HIL Testing**
- Test firmware với hardware thật trong môi trường controlled
- Simulate encoder signals và motor responses
- Validate control algorithms và safety mechanisms
- Verify system performance trước khi deploy

---

## 🔧 **HIL Test Setup**

### **Hardware Configuration**
```
OHT-50 System:
├── UART1 (RS485): Chân 46 (TX), 47 (RX)
├── Encoder A: GPIO48 (Pin 48)
├── Encoder B: GPIO49 (Pin 49)
├── Encoder Z: GPIO50 (Pin 50)
├── Limit Switch Front: GPIO51 (Pin 51)
├── Limit Switch Back: GPIO52 (Pin 52)
├── E-Stop: GPIO53 (Pin 53)
└── ADC: SPI0 (Chân 54, 55, 56)

Test Controller (Arduino):
├── RS485 Transceiver: MAX485
├── Encoder Simulator: Arduino + DAC
├── Motor Simulator: Arduino + PWM
├── Limit Switch Simulator: Relay modules
├── E-Stop Simulator: Relay module
└── ADC Simulator: Arduino + DAC
```

### **Test Scenarios**
1. **Normal Operation**: Encoder 100Hz, Motor 1000RPM
2. **Performance Test**: Low/Medium/High speed testing
3. **Safety Test**: E-Stop, limit switches validation
4. **Fault Injection**: Encoder fault, motor fault, communication fault

---

## 📊 **Test Results**

### **Performance Metrics**
- **Encoder Reading**: < 1ms latency
- **Motor Control**: < 5ms response time
- **Safety Response**: < 100ms activation
- **Communication**: < 10ms latency

### **Test Results Summary**
```
Normal Operation Test: PASS
Performance Test: PASS
Safety Test: PASS
Fault Injection Test: PASS

Overall Result: PASS
Success Rate: 100%
```

---

## 📋 **Test Checklist**

### **Pre-Test Setup**
- [ ] Arduino test controller programmed
- [ ] All connections verified
- [ ] Test environment configured
- [ ] Safety procedures reviewed

### **Test Execution**
- [ ] Normal operation test completed
- [ ] Performance test completed
- [ ] Safety test completed
- [ ] Fault injection test completed

### **Post-Test Analysis**
- [ ] Results documented
- [ ] Issues identified and resolved
- [ ] Performance metrics recorded
- [ ] Recommendations documented

---

**Changelog:**
- v1.0 (2025-01-27): Initial version with HIL test setup and procedures
