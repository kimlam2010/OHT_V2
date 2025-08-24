# 📋 02-REQUIREMENTS - Yêu cầu hệ thống OHT-50

**Mục đích:** Định nghĩa yêu cầu kỹ thuật và chức năng của hệ thống OHT-50 Master Module

---

## 📁 **Cấu trúc thư mục mới (Phương án 1 - Phân lớp rõ ràng):**

### **📄 Thư mục chính:**
- **01-SYSTEM-REQUIREMENTS/** - Yêu cầu hệ thống tổng thể
- **02-HARDWARE-REQUIREMENTS/** - Yêu cầu phần cứng
- **03-FIRMWARE-REQUIREMENTS/** - Yêu cầu Firmware ⭐ MỚI
- **04-BACKEND-REQUIREMENTS/** - Yêu cầu Backend ⭐ MỚI
- **05-FRONTEND-REQUIREMENTS/** - Yêu cầu Frontend ⭐ MỚI
- **06-INTEGRATION-REQUIREMENTS/** - Yêu cầu tích hợp

### **📄 Thư mục legacy (sẽ migrate):**
- **technical/** - Tài liệu kỹ thuật (RS485, Modbus, LiDAR)
- **specs/** - Đặc tả kỹ thuật chi tiết
- **hardware/** - Yêu cầu phần cứng (cũ)

---

## 🎯 **Đối tượng sử dụng:**

### **Cho System Architects:**
- Bắt đầu với **01-SYSTEM-REQUIREMENTS/**
- Xem **06-INTEGRATION-REQUIREMENTS/** cho integration design

### **Cho EMBED Team (Hardware + Firmware):**
- Đọc **02-HARDWARE-REQUIREMENTS/** cho hardware design
- Xem **03-FIRMWARE-REQUIREMENTS/** cho firmware development

### **Cho Backend Team:**
- Đọc **04-BACKEND-REQUIREMENTS/** cho backend development

### **Cho Frontend Team:**
- Đọc **05-FRONTEND-REQUIREMENTS/** cho frontend development

### **Cho Integration Team:**
- Xem **06-INTEGRATION-REQUIREMENTS/** cho system integration

---

## 📖 **Hướng dẫn đọc theo role:**

### **System Architects:**
1. **01-SYSTEM-REQUIREMENTS/** → Hiểu yêu cầu tổng thể
2. **06-INTEGRATION-REQUIREMENTS/** → Thiết kế tích hợp
3. **02-HARDWARE-REQUIREMENTS/** → Hardware constraints

### **EMBED Team:**
1. **02-HARDWARE-REQUIREMENTS/** → Hardware specifications
2. **03-FIRMWARE-REQUIREMENTS/** → Firmware requirements
3. **06-INTEGRATION-REQUIREMENTS/** → Integration protocols

### **Backend Team:**
1. **04-BACKEND-REQUIREMENTS/** → Backend specifications
2. **01-SYSTEM-REQUIREMENTS/** → System context
3. **06-INTEGRATION-REQUIREMENTS/** → API integration

### **Frontend Team:**
1. **05-FRONTEND-REQUIREMENTS/** → Frontend specifications
2. **04-BACKEND-REQUIREMENTS/** → API requirements
3. **06-INTEGRATION-REQUIREMENTS/** → UI integration

---

## 🔗 **Liên kết nhanh:**
- [📚 Main Documentation](../README.md)
- [🏗️ Architecture](../03-ARCHITECTURE/)
- [⚙️ Implementation](../05-IMPLEMENTATION/)
- [🎨 Design](../04-DESIGN/)

---

## 📊 **Trạng thái migration:**

### **✅ Hoàn thành:**
- Cấu trúc thư mục mới
- README cho tất cả thư mục
- Index navigation

### **⏳ Đang thực hiện:**
- Migrate existing requirements
- Tạo missing requirements
- Update cross-references

### **📋 Kế hoạch:**
- **TUẦN 1:** Migrate system & hardware requirements
- **TUẦN 2:** Tạo FW/BE/FE requirements từ implementation
- **TUẦN 3:** Tạo integration requirements

---

## 🚨 **Lưu ý quan trọng:**

1. **Cấu trúc mới** tuân thủ ISO standards và phân lớp rõ ràng
2. **Mỗi team** có tài liệu yêu cầu riêng biệt
3. **Cross-references** giữa các layer được maintain
4. **Legacy folders** sẽ được migrate dần dần

---

**Cập nhật lần cuối:** 2025-01-24
**Phiên bản:** v2.0 (Phương án 1 - Phân lớp rõ ràng)
