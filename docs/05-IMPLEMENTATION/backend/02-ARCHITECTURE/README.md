# 02-ARCHITECTURE - Kiến trúc hệ thống

**Mục đích:** Tài liệu kiến trúc tổng thể và thiết kế hệ thống OHT-50 Backend

---

## 📋 **Nội dung thư mục**

### **🏗️ Kiến trúc hệ thống**
- **CORE_ARCHITECTURE.md** - Kiến trúc tổng thể (CORE)
- **BUSINESS_LOGIC_SPEC.md** - Business logic specification (CORE)
- **SYSTEM_DESIGN.md** - Thiết kế hệ thống
- **INTEGRATION_PATTERNS.md** - Mẫu tích hợp

### **🔧 Cấu hình và chuẩn hóa**
- **MODULE_ADDRESS_STANDARDIZATION.md** - Chuẩn hóa địa chỉ module
- **VOLTAGE_STANDARDIZATION_PLAN.md** - Kế hoạch chuẩn hóa điện áp

---

## 🎯 **Hướng dẫn đọc**

### **Cho System Architect:**
1. **CORE_ARCHITECTURE.md** - Hiểu kiến trúc tổng thể
2. **BUSINESS_LOGIC_SPEC.md** - Hiểu business logic
3. **SYSTEM_DESIGN.md** - Hiểu thiết kế chi tiết
4. **INTEGRATION_PATTERNS.md** - Hiểu mẫu tích hợp

### **Cho Senior Developer:**
1. **CORE_ARCHITECTURE.md** - Hiểu kiến trúc
2. **BUSINESS_LOGIC_SPEC.md** - Hiểu business rules
3. **MODULE_ADDRESS_STANDARDIZATION.md** - Hiểu communication

### **Cho DevOps Engineer:**
1. **CORE_ARCHITECTURE.md** - Hiểu deployment architecture
2. **SYSTEM_DESIGN.md** - Hiểu system requirements
3. **VOLTAGE_STANDARDIZATION_PLAN.md** - Hiểu hardware requirements

---

## ⚡ **Quick Links**

- **Quick Start:** [../01-QUICK-START/](../01-QUICK-START/)
- **API Specifications:** [../03-API-SPECIFICATIONS/](../03-API-SPECIFICATIONS/)
- **Implementation:** [../04-IMPLEMENTATION/](../04-IMPLEMENTATION/)
- **Data Models:** [../05-DATA-MODELS/](../05-DATA-MODELS/)

---

## 📊 **Thời gian đọc ước tính**

| Tài liệu | Thời gian | Mục đích |
|----------|-----------|----------|
| **CORE_ARCHITECTURE.md** | 45-90 phút | Hiểu kiến trúc tổng thể |
| **BUSINESS_LOGIC_SPEC.md** | 60-120 phút | Hiểu business logic |
| **SYSTEM_DESIGN.md** | 30-60 phút | Hiểu thiết kế hệ thống |
| **INTEGRATION_PATTERNS.md** | 30-45 phút | Hiểu mẫu tích hợp |
| **MODULE_ADDRESS_STANDARDIZATION.md** | 15-30 phút | Hiểu communication |
| **VOLTAGE_STANDARDIZATION_PLAN.md** | 15-30 phút | Hiểu hardware |

**Tổng thời gian:** 3-6 giờ để hiểu toàn bộ kiến trúc hệ thống

---

## 🔑 **Tài liệu CORE (Quan trọng nhất)**

### **CORE_ARCHITECTURE.md** 🔴 **CRITICAL**
- Kiến trúc layered (Presentation, Business Logic, Integration, Data)
- Service layer architecture với 14 services
- Communication architecture với RS485
- State machine và validation rules
- Error handling hierarchy
- Security architecture
- Performance characteristics

### **BUSINESS_LOGIC_SPEC.md** 🔴 **CRITICAL**
- Mission management lifecycle
- Safety system với E-Stop và zones
- State management với validation
- Movement control và path planning
- Module coordination
- Business rules engine
- Error handling và recovery strategies
