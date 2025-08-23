# 📋 Chuẩn đặt tên file theo ISO - OHT-50

**Phiên bản:** v1.0  
**Ngày cập nhật:** 2025-01-24  
**PM:** OHT-50 Project Manager  
**Status:** ✅ ISO STANDARD

---

## 🎯 **Chuẩn đặt tên file ISO thực tế**

### **📁 Cấu trúc thư mục ISO:**
```
01-OVERVIEW/     # Project Overview
02-REQUIREMENTS/ # System Requirements  
03-ARCHITECTURE/ # System Architecture
04-DESIGN/       # Detailed Design
05-IMPLEMENTATION/ # Implementation
06-TESTING/      # Testing & Validation
07-DEPLOYMENT/   # Deployment
08-OPERATIONS/   # Operations
09-MAINTENANCE/  # Maintenance
10-REFERENCES/   # References
```

### **📄 Quy tắc đặt tên file:**

#### **1. Chuẩn ISO thực tế:**
- **Format:** `[Category]_[Description].md`
- **Ví dụ:** `OVERVIEW_Project_Summary.md`

#### **2. Categories theo ISO:**
- **OVERVIEW** - Tổng quan dự án
- **REQ** - Yêu cầu hệ thống
- **ARCH** - Kiến trúc hệ thống
- **DESIGN** - Thiết kế chi tiết
- **IMPL** - Triển khai
- **TEST** - Kiểm thử
- **DEPLOY** - Triển khai
- **OPS** - Vận hành
- **MAINT** - Bảo trì
- **REF** - Tài liệu tham khảo

---

## 📋 **Mapping file names:**

### **01-OVERVIEW/**
- ✅ `OVERVIEW_Project_Progress.md` - Báo cáo tiến độ dự án
- ✅ `OVERVIEW_Master_Module_Summary.md` - Tóm tắt Master Module
- ✅ `OVERVIEW_Hardware_Integration.md` - Báo cáo tích hợp hardware
- ✅ `INDEX.md` - Index cho thư mục

### **02-REQUIREMENTS/**
- ✅ `INDEX.md` - Index cho thư mục
- **specs/** - Technical specifications
- **technical/** - Technical documentation
- **hardware/** - Hardware requirements

### **03-ARCHITECTURE/**
- ✅ `INDEX.md` - Index cho thư mục
- `ARCH_CTO_Decisions.md` - Quyết định CTO
- **architecture/decisions/** - ADR files

### **04-DESIGN/**
- **design/** - Design system documentation

### **05-IMPLEMENTATION/**
- ✅ `INDEX.md` - Index cho thư mục
- `IMPL_Embed_Complete_Guide.md` - Hướng dẫn EMBED
- `IMPL_Embed_UART_Setup.md` - Setup UART
- **EMBED/** - Embedded documentation
- **FIRMWARE/** - Firmware documentation
- **dev_radxa/** - Radxa platform docs
- **backend/** - Backend documentation

### **06-TESTING/**
- **integration/** - Integration testing

### **07-DEPLOYMENT/**
- (Empty - ready for future)

### **08-OPERATIONS/**
- (Empty - ready for future)

### **09-MAINTENANCE/**
- (Empty - ready for future)

### **10-REFERENCES/**
- ✅ `INDEX.md` - Index cho thư mục
- **tasks/** - Task management
- **issues/** - Historical issues
- **CTO/** - CTO decisions

---

## 🔄 **Quy tắc đặt tên:**

### **1. File chính:**
- **Format:** `[CATEGORY]_[Description].md`
- **Ví dụ:** `OVERVIEW_Project_Progress.md`

### **2. File index:**
- **Format:** `INDEX.md` (giữ nguyên)

### **3. File trong thư mục con:**
- **Format:** `[SubCategory]_[Description].md`
- **Ví dụ:** `ADR_System_Architecture.md`

### **4. File issues:**
- **Format:** `[Team]_[Module]_Issues.md`
- **Ví dụ:** `EMBED_Master_Module_Issues.md`

---

## ✅ **Lợi ích chuẩn ISO:**

### **📈 Cải thiện tổ chức:**
- **Dễ tìm kiếm** - Tên file rõ ràng
- **Phân loại tốt** - Theo category ISO
- **Navigation nhanh** - Ít confusion

### **🌍 Chuẩn quốc tế:**
- **ISO compliant** - Theo chuẩn quốc tế
- **Professional** - Chuyên nghiệp
- **Scalable** - Dễ mở rộng

### **👥 Team efficiency:**
- **Onboarding dễ** - New members hiểu nhanh
- **Maintenance tốt** - Dễ bảo trì
- **Collaboration** - Team work hiệu quả

---

**Changelog v1.0:**
- ✅ Defined ISO naming standards
- ✅ Created category mapping
- ✅ Established naming rules
- ✅ Documented benefits
