# PHƯƠNG ÁN CẬP NHẬT KIẾN TRÚC HỆ THỐNG OHT-50
**Phiên bản:** 1.4  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** Phase 1 Documentation Completed + LiDAR Files Restored  
**Mục tiêu:** Cập nhật toàn bộ hệ thống theo kiến trúc mới với 5 module bắt buộc

---

## 🎯 **MỤC TIÊU CẬP NHẬT**
### **Yêu cầu mới:**
- **5 Module bắt buộc:** Power, Safety, Travel Motor, Dock & Location, Master Control
- **Module tùy chọn:** Lifter Motor, Cargo Door, Safety Extended
- **Module mở rộng:** RFID, Camera, Environmental Sensor, Custom modules
- **Plug-and-Play:** Auto-discovery và hot-swap capability
- **Location-based Safety:** Bắt buộc có positioning system
- **RS485 Standard:** Tất cả modules đều sử dụng RS485/Modbus RTU
- **LiDAR Integration:** LiDAR sử dụng USB connection để tích hợp với Dock & Location module

### **Thay đổi chính:**
1. **Dock & Location Module** chuyển từ tùy chọn thành **BẮT BUỘC**
2. **Master Control Module** được định nghĩa rõ ràng
3. **Location-based Safety** được tích hợp vào tất cả modules
4. **Auto-discovery** được chuẩn hóa cho tất cả modules
5. **RS485 Standard** được áp dụng cho tất cả modules
6. **LiDAR USB Integration** được khôi phục và cập nhật

---

## 📋 **DANH SÁCH FILE CẦN CẬP NHẬT**
### **📄 Tài liệu Requirements (02-REQUIREMENTS):**
#### **✅ Đã cập nhật:**
- `specs/REQ_System_Architecture.md` - ✅ Cập nhật v2.0
- `specs/REQ_State_Machine.md` - ✅ Cập nhật v2.0 với location requirements
- `specs/REQ_System_Interfaces.md` - ✅ Cập nhật v2.1 với RS485 cho tất cả modules
- `specs/REQ_Telemetry_Schema.md` - ✅ Cập nhật v2.0 với location và navigation data
- `hardware/REQ_Hardware_Specification.md` - ✅ Cập nhật v2.0 với Dock & Location module
- `hardware/REQ_Hardware_Implementation_Plan.md` - ✅ Cập nhật v2.0 với implementation plan
- `hardware/REQ_Motor_Control_Module.md` - ✅ Cập nhật v2.1 với DC brushed motor 12V và PID speed control

#### **🔄 Đã khôi phục và cập nhật:**
- `specs/REQ_LiDAR_Integration_Guide.md` - ✅ Khôi phục v2.0 với USB connection
- `specs/REQ_LiDAR_Setup_Guide.md` - ✅ Khôi phục v2.0 với USB connection
- `specs/REQ_LiDAR_Test_Procedures.md` - ✅ Khôi phục v2.0 với USB connection
- `specs/REQ_LiDAR_Wiring_Guide.md` - ✅ Khôi phục v2.0 với USB connection

#### **🗑️ Đã xóa:**
- ~~`specs/REQ_LiDAR_Integration_Guide.md`~~ - ✅ Đã khôi phục
- ~~`specs/REQ_LiDAR_Setup_Guide.md`~~ - ✅ Đã khôi phục
- ~~`specs/REQ_LiDAR_Test_Procedures.md`~~ - ✅ Đã khôi phục
- ~~`specs/REQ_LiDAR_Wiring_Guide.md`~~ - ✅ Đã khôi phục

### **📄 Tài liệu Implementation (05-IMPLEMENTATION):**
#### **🔄 Cần cập nhật:**
- `backend/` - Cần cập nhật models, API endpoints, services
- `frontend/` - Cần cập nhật UI components, API services
- `firmware/` - Cần cập nhật headers, source code, tests

### **📄 Tài liệu khác:**
#### **🔄 Cần cập nhật:**
- `docs/README.md` - Cập nhật overview
- `docs/03-ARCHITECTURE/` - Cập nhật architecture documents
- `docs/06-TESTING/` - Cập nhật test plans

---

## 📅 **LỊCH TRÌNH THỰC HIỆN**

### **Phase 1: Documentation Update (Week 1-2) ✅ COMPLETED**
#### **Week 1: Core Documentation ✅**
- ✅ Update System Architecture (v2.0)
- ✅ Update State Machine (v2.0)
- ✅ Update System Interfaces (v2.1)
- ✅ Update Telemetry Schema (v2.0)
- ✅ Delete obsolete LiDAR files

#### **Week 2: Hardware Documentation ✅**
- ✅ Update Hardware Specification (v2.0)
- ✅ Update Hardware Implementation Plan (v2.0)
- ✅ Update Motor Control Module (v2.1)
- ✅ **RESTORE LiDAR files with USB integration (v2.0)**

### **Phase 2: Implementation Update (Week 3-4) 🔄 IN PROGRESS**
#### **Week 3: Backend Implementation**
- 🔄 Update backend models
- 🔄 Update API endpoints
- 🔄 Update services
- 🔄 Update configurations

#### **Week 4: Frontend Implementation**
- 🔄 Update UI components
- 🔄 Update API services
- 🔄 Update configurations

### **Phase 3: Firmware Update (Week 5-6) ⏳ PENDING**
#### **Week 5: Firmware Implementation**
- ⏳ Update firmware headers
- ⏳ Update source code
- ⏳ Update tests

#### **Week 6: Integration Testing**
- ⏳ System integration
- ⏳ Testing and validation

### **Phase 4: Integration & Testing (Week 7) ⏳ PENDING**
- ⏳ System integration
- ⏳ Testing and validation
- ⏳ Documentation review

---

## 📊 **TIẾN ĐỘ THỰC HIỆN**

### **Phase 1: Documentation Update ✅ COMPLETED**
```
Week 1: Core Documentation ✅
├── System Architecture (v2.0) ✅
├── State Machine (v2.0) ✅
├── System Interfaces (v2.1) ✅
├── Telemetry Schema (v2.0) ✅
└── Delete obsolete files ✅

Week 2: Hardware Documentation ✅
├── Hardware Specification (v2.0) ✅
├── Hardware Implementation Plan (v2.0) ✅
├── Motor Control Module (v2.1) ✅
└── RESTORE LiDAR files (v2.0) ✅
```

### **Phase 2: Implementation Update 🔄 IN PROGRESS**
```
Week 3: Backend Implementation 🔄
├── Update backend models 🔄
├── Update API endpoints 🔄
├── Update services 🔄
└── Update configurations 🔄

Week 4: Frontend Implementation ⏳
├── Update UI components ⏳
├── Update API services ⏳
└── Update configurations ⏳
```

### **Phase 3: Firmware Update ⏳ PENDING**
```
Week 5: Firmware Implementation ⏳
├── Update firmware headers ⏳
├── Update source code ⏳
└── Update tests ⏳

Week 6: Integration Testing ⏳
├── System integration ⏳
└── Testing and validation ⏳
```

### **Phase 4: Integration & Testing ⏳ PENDING**
```
Week 7: Final Integration ⏳
├── System integration ⏳
├── Testing and validation ⏳
└── Documentation review ⏳
```

---

## 🔧 **LIÊN QUAN ĐẾN LIDAR**

### **LiDAR Integration với Dock & Location Module:**
```
LiDAR (USB) → Orange Pi 5B → Dock & Location Module (RS485)
    │              │                    │
    ├── Scan Data  ├── Process Data     ├── Register Map
    ├── Status     ├── Localization     ├── Safety Integration
    └── Health     └── Mapping          └── Navigation
```

### **LiDAR Files đã khôi phục:**
```
✅ REQ_LiDAR_Integration_Guide.md (v2.0) - USB integration
✅ REQ_LiDAR_Setup_Guide.md (v2.0) - USB setup
✅ REQ_LiDAR_Test_Procedures.md (v2.0) - USB testing
✅ REQ_LiDAR_Wiring_Guide.md (v2.0) - USB wiring
```

### **LiDAR Specifications:**
```
Model:               RPLIDAR A1M8
Interface:           USB 2.0
Power:               5V DC via USB
Current:             1.5A maximum
Range:               0.15m - 12m
Scan Frequency:      5.5Hz
Integration:         Dock & Location Module
```

---

## 📋 **NEXT STEPS**

### **Immediate Actions:**
1. ✅ **Phase 1 Documentation Completed**
2. 🔄 **Begin Phase 2 - Backend Implementation Update**
3. ⏳ **Continue with Frontend Implementation**
4. ⏳ **Proceed to Firmware Update**

### **Current Status:**
- **Phase 1:** ✅ **COMPLETED** - All documentation updated
- **Phase 2:** 🔄 **IN PROGRESS** - Backend implementation update
- **Phase 3:** ⏳ **PENDING** - Firmware implementation
- **Phase 4:** ⏳ **PENDING** - Integration & testing

---

**Status:** Phase 1 Documentation Completed + LiDAR Files Restored ✅  
**Next Steps:** Continue Phase 2 - Backend Implementation Update
