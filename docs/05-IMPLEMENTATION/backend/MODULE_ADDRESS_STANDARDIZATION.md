# MODULE ADDRESS STANDARDIZATION - OHT-50 v2.0

**Phiên bản:** 1.0.0  
**Ngày tạo:** 2025-01-28  
**Mục đích:** Chuẩn hóa module addresses cho OHT-50 Architecture v2.0

---

## 📋 TỔNG QUAN

Tài liệu này chuẩn hóa module addresses cho OHT-50 system để đảm bảo tính nhất quán giữa requirements, firmware, và backend implementation.

---

## 🎯 CHUẨN HÓA MODULE ADDRESSES

### **📊 COMPARISON ANALYSIS:**

#### **Requirements (System Architecture):**
```yaml
Power Module: ID 0x02 (BẮT BUỘC)
Safety Module: ID 0x03 (BẮT BUỘC)  
Travel Motor: ID 0x04 (BẮT BUỘC)
Dock & Location: ID 0x05 (BẮT BUỘC)
Master Control: ID 0x00 (BẮT BUỘC)
```

#### **Backend Implementation:**
```yaml
Power Module: Address 0x01 ✅
Safety Module: Address 0x02 ✅
Travel Motor: Address 0x03 ✅
Dock & Location: Address 0x04 ✅
Master Control: Address 0x05 ✅
```

#### **Firmware Implementation:**
```yaml
Power Module: Address 0x01 ✅
Safety Module: Address 0x02 ✅
Travel Motor: Address 0x03 ✅
Dock & Location: Address 0x04 ✅
Master Control: Address 0x05 ✅
```

---

## ✅ CHUẨN HÓA ĐƯỢC CHỌN

### **STANDARDIZED ADDRESSES (0x01-0x05):**

| Module | Address | Type | Status |
|--------|---------|------|--------|
| Power Module | 0x01 | BẮT BUỘC | ✅ Standardized |
| Safety Module | 0x02 | BẮT BUỘC | ✅ Standardized |
| Travel Motor | 0x03 | BẮT BUỘC | ✅ Standardized |
| Dock & Location | 0x04 | BẮT BUỘC | ✅ Standardized |
| Master Control | 0x05 | BẮT BUỘC | ✅ Standardized |

### **OPTIONAL MODULES (0x06-0x0F):**

| Module | Address | Type | Status |
|--------|---------|------|--------|
| Lifter Motor | 0x06 | TÙY CHỌN | 🔄 To be implemented |
| Cargo Door | 0x07 | TÙY CHỌN | 🔄 To be implemented |
| Safety Extended | 0x08 | TÙY CHỌN | 🔄 To be implemented |
| RFID Reader | 0x09 | PLUG-PLAY | 🔄 To be implemented |
| Camera Module | 0x0A | PLUG-PLAY | 🔄 To be implemented |
| Environmental Sensor | 0x0B | PLUG-PLAY | 🔄 To be implemented |

---

## 🔧 IMPLEMENTATION PLAN

### **Phase 1: Update Requirements (Immediate)**
1. Update `docs/02-REQUIREMENTS/specs/REQ_System_Architecture.md`
2. Change Power Module từ ID 0x02 → 0x01
3. Change Safety Module từ ID 0x03 → 0x02
4. Change Travel Motor từ ID 0x04 → 0x03
5. Change Dock & Location từ ID 0x05 → 0x04
6. Change Master Control từ ID 0x00 → 0x05

### **Phase 2: Update Backend Documentation (1 day)**
1. Update `docs/05-IMPLEMENTATION/backend/api-specs/README.md`
2. Update all API specification files
3. Update architecture documentation
4. Update business logic documentation

### **Phase 3: Update Firmware Documentation (1 day)**
1. Update `docs/05-IMPLEMENTATION/FIRMWARE/MODULE_SPECIFICATIONS.md`
2. Update firmware implementation guides
3. Update testing procedures

---

## 📝 CHANGELOG

### **v1.0.0 (2025-01-28)**
- ✅ Created standardization document
- ✅ Defined 0x01-0x05 range cho mandatory modules
- ✅ Defined 0x06-0x0F range cho optional modules
- ✅ Created implementation plan

---

## ✅ NEXT STEPS

1. **Update Requirements Documentation** - Change module IDs
2. **Update Backend Documentation** - Align với standardized addresses
3. **Update Firmware Documentation** - Align với standardized addresses
4. **Create Optional Module APIs** - Implement optional module documentation

---

**Status:** Standardization Plan Complete  
**Next Action:** Begin Phase 1 implementation
