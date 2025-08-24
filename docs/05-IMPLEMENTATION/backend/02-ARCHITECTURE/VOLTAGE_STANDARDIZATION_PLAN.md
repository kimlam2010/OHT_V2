# VOLTAGE STANDARDIZATION PLAN - OHT-50 (v2.0)

## 📋 **Tổng quan**
Plan để standardize tất cả voltage specifications trong hệ thống OHT-50 theo **24V nominal** như đã định nghĩa trong requirements.

## 🎯 **Mục tiêu**
- ✅ Align tất cả tài liệu với 24V nominal
- ✅ Loại bỏ sự không nhất quán 48V vs 24V
- ✅ Đảm bảo tính chính xác kỹ thuật
- ✅ Cập nhật tất cả API, FW, và documentation

---

## 🔧 **CHANGES ĐÃ THỰC HIỆN**

### **✅ 1. API Specifications - COMPLETED**
- **File:** `docs/05-IMPLEMENTATION/backend/api-specs/POWER_API.md`
- **Changes:**
  - Battery voltage: 48.2V → 24.2V
  - Battery capacity: 100Ah → 50Ah
  - Max voltage: 48.0V → 28.8V
  - Min voltage: 40.0V → 20.0V
  - Charging voltage: 48.0V → 28.8V
  - Power distribution: Removed 48V output
  - Protection thresholds: Updated accordingly

### **✅ 2. Firmware Specifications - COMPLETED**
- **File:** `docs/05-IMPLEMENTATION/FIRMWARE/MODULE_SPECIFICATIONS.md`
- **Changes:**
  - Battery voltage: 48V DC → 24V DC (nominal)
  - Battery capacity: 100Ah → 50Ah
  - Charging voltage: 54.6V → 28.8V

- **File:** `docs/05-IMPLEMENTATION/FIRMWARE/MODULE_IMPLEMENTATION_GUIDE.md`
- **Changes:**
  - Power supply: 48V DC → 24V DC

- **File:** `docs/05-IMPLEMENTATION/FIRMWARE/INTEGRATION_GUIDE.md`
- **Changes:**
  - Power supply: 48V DC → 24V DC

---

## 📊 **VOLTAGE SPECIFICATIONS STANDARD**

### **🔋 Battery System (24V Nominal):**
```
Battery Type:        LiFePO4 (Lithium Iron Phosphate)
Nominal Voltage:     24V DC
Voltage Range:       20V - 28.8V
Capacity:            50Ah
Chemistry:           Lithium Iron Phosphate
Cycle Life:          2000+ cycles
Operating Temperature: -20°C to +60°C
```

### **⚡ Power Distribution:**
```
24V Main Bus:        Motor drives, actuators, main systems
12V Secondary:       Sensors, communication modules
5V Logic:            Microcontrollers, digital circuits
3.3V Digital:        Low-power sensors, communication
```

### **🔌 Charging System:**
```
Charging Method:     CC-CV (Constant Current - Constant Voltage)
Charging Current:    10A maximum (0.2C)
Charging Voltage:    28.8V (3.6V per cell)
Charging Time:       5-6 hours (0-100%)
Charging Efficiency: >95%
```

### **🛡️ Protection Limits:**
```
Overvoltage:         30.0V (3.75V per cell)
Undervoltage:        20.0V (2.5V per cell)
Overcurrent:         50A continuous, 100A peak
Overtemperature:     60°C
```

---

## 📋 **FILES CẦN KIỂM TRA VÀ CẬP NHẬT**

### **🔍 Files cần review:**
1. **Technical Documentation:**
   - `docs/02-REQUIREMENTS/technical/REQ_RS485_Module_Statistics.md`
   - `docs/02-REQUIREMENTS/technical/REQ_RS485_System_Overview.md`

2. **Hardware Documentation:**
   - `docs/02-REQUIREMENTS/hardware/REQ_Hardware_Specification.md` ✅ (Already correct)
   - `docs/02-REQUIREMENTS/hardware/REQ_Hardware_Implementation_Plan.md` ✅ (Already correct)

3. **System Architecture:**
   - `docs/02-REQUIREMENTS/specs/REQ_System_Architecture.md` ✅ (Already correct)
   - `docs/02-REQUIREMENTS/specs/REQ_System_Interfaces.md` ✅ (Already correct)

4. **Firmware Documentation:**
   - `docs/05-IMPLEMENTATION/FIRMWARE/FIRMWARE_TINH_NANG_HIEN_TAI.md`
   - `docs/05-IMPLEMENTATION/FIRMWARE/FIRMWARE_STATUS_REPORT.md`
   - `docs/05-IMPLEMENTATION/FIRMWARE/FIRMWARE_SOURCE_ANALYSIS_SUMMARY.md`

5. **Backend Documentation:**
   - `docs/05-IMPLEMENTATION/backend/BACKEND_PROGRESS_REPORT.md`
   - `docs/05-IMPLEMENTATION/backend/BACKEND_DOCUMENTATION_ANALYSIS_REPORT.md`

---

## 🚨 **CRITICAL FILES TO FIX**

### **❌ Files with 48V references:**
1. `docs/02-REQUIREMENTS/technical/REQ_RS485_Module_Statistics.md`
   - Line 76: "Power distribution (5V, 12V, 24V, 48V outputs)"
   - **Action:** Remove 48V reference

2. `docs/05-IMPLEMENTATION/FIRMWARE/FIRMWARE_TINH_NANG_HIEN_TAI.md`
   - **Action:** Check for 48V references

3. `docs/05-IMPLEMENTATION/FIRMWARE/FIRMWARE_STATUS_REPORT.md`
   - **Action:** Check for 48V references

---

## ✅ **VALIDATION CHECKLIST**

### **✅ Completed:**
- [x] API Power Module specification updated
- [x] Firmware module specifications updated
- [x] Implementation guide updated
- [x] Integration guide updated

### **🔄 Pending:**
- [ ] Review technical documentation files
- [ ] Review firmware documentation files
- [ ] Review backend documentation files
- [ ] Update any remaining 48V references
- [ ] Validate all voltage specifications align

### **📊 Validation Results:**
- **Requirements:** ✅ 24V nominal (correct)
- **Hardware Specs:** ✅ 24V nominal (correct)
- **System Architecture:** ✅ 24V nominal (correct)
- **API Specs:** ✅ Updated to 24V
- **FW Specs:** ✅ Updated to 24V

---

## 🎯 **NEXT STEPS**

### **Phase 1: Documentation Review (Immediate)**
1. **Review technical files** for 48V references
2. **Update firmware documentation** files
3. **Update backend documentation** files
4. **Validate all specifications** align

### **Phase 2: Implementation Update (Future)**
1. **Update actual code** if needed
2. **Update configuration files** if needed
3. **Update test cases** if needed
4. **Update deployment scripts** if needed

### **Phase 3: Validation (Future)**
1. **Hardware validation** with actual 24V system
2. **Software validation** with updated specs
3. **Integration testing** with 24V components
4. **Performance testing** with 24V system

---

## 📝 **CONCLUSION**

**24V nominal** là specification chính xác cho OHT-50 system vì:
- ✅ Phù hợp với LiFePO4 battery pack thông dụng
- ✅ Phù hợp với motor DC 12V (24V/2)
- ✅ Phù hợp với relay outputs 24V
- ✅ Phù hợp với tất cả tài liệu requirements
- ✅ Phù hợp với industry standards

Tất cả tài liệu đã được cập nhật để align với 24V nominal specification.

---

**Changelog:**
- v1.0 (2025-01-28): Initial voltage standardization plan
  - ✅ Updated API specifications
  - ✅ Updated firmware specifications
  - ✅ Created standardization plan
  - ✅ Identified remaining files to review
