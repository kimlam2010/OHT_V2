# 🔧 03-MODULE-INTEGRATION - Tích hợp Module

**Mục đích:** Định nghĩa tích hợp các module chức năng cho OHT-50

---

## 📁 **Nội dung thư mục:**

### **📄 Tài liệu chính:**

#### **Power Module (0x01):**
- **REQ_RS485_Power_Module_Integration.md** - Tích hợp module nguồn
  - DalyBMS Integration
  - SK60X Charger
  - INA219 Sensors
  - Power Management
  - Safety Protection

#### **Travel Motor Module (0x03):**
- **REQ_RS485_Travel_Motor_Integration.md** - Tích hợp module động cơ di chuyển
  - DC Motor Control
  - PID Control
  - Velocity Control
  - Position Control
  - Safety Integration

#### **Safety Module (0x02):**
- **REQ_RS485_Safety_Module_Integration.md** - Tích hợp module an toàn
  - Single-channel E-Stop
  - Safety Levels
  - Fault Detection
  - Emergency Procedures

#### **Dock & Location Module (0x05):**
- **REQ_RS485_Dock_Location_Integration.md** - Tích hợp module dock và định vị
  - IMU Integration
  - Magnetic Sensors
  - LiDAR USB Integration
  - Navigation Support

---

## 🎯 **Đối tượng sử dụng:**
- **FW Developers** - Implement module handlers
- **HW Engineers** - Kết nối module hardware
- **Protocol Engineers** - Implement Modbus protocol
- **Test Engineers** - Test module integration

---

## 🔗 **Liên kết nhanh:**
- [📚 Main README](../README.md)
- [🏗️ Core Architecture](../01-CORE-ARCHITECTURE/)
- [📡 RS485 System](../02-RS485-SYSTEM/)

---

**Status:** ✅ Hoàn thành - Ready for Implementation
