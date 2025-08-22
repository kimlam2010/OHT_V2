# Simulation Team Issues - Master Module

## 📋 **Tổng quan**
Issues cho SIM team để triển khai mô phỏng và xác minh hệ thống theo Gate E.

**Ngày tạo:** 2025-01-29  
**Priority:** High  
**Team:** Simulation  
**Sprint:** Master Module v3.0

---

## 🎯 **Issue #SIM-001: Build 1D Rail Motion Simulation**

### **Mô tả:**
Xây dựng framework mô phỏng chuyển động 1D theo ray với profile v‑a‑j, đồng bộ thời gian thực và HIL hooks.

### **Tasks:**
- [ ] Bộ tích phân thời gian thực (dt cấu hình)
- [ ] Mô hình động học vị trí‑vận tốc‑gia tốc
- [ ] Tham số hoá giới hạn v/a/j, ma sát
- [ ] Hooks HIL để thay thế nguồn telemetry

### **Priority:** High
### **Estimate:** 1 tuần

---

## 🎯 **Issue #SIM-002: Fault Injection & Safety Validation**

### **Mô tả:**
Bộ tiêm lỗi (encoder mất xung, quá dòng, mất truyền thông) và kịch bản xác minh E‑Stop < 100ms, interlock.

### **Tasks:**
- [ ] Tiêm lỗi encoder/comm/current/overtemp
- [ ] Kịch bản E‑Stop: đo latency, verify latch
- [ ] Báo cáo pass/fail theo tiêu chí Gate E

### **Priority:** High
### **Estimate:** 3 ngày

---

## 🎯 **Issue #SIM-003: Telemetry & Status Replay Harness**

### **Mô tả:**
Công cụ phát lại telemetry/status để FE kiểm thử real-time và BE load test WS.

### **Tasks:**
- [ ] Generator JSON frames (schema `docs/specs/telemetry_schema.md`)
- [ ] Tốc độ 10–50 Hz, downsample
- [ ] CLI cấu hình kịch bản

### **Priority:** Medium
### **Estimate:** 2 ngày

---

## 🎯 **Issue #SIM-004: End-to-End Scenario Tests**

### **Mô tả:**
Kịch bản e2e Move→Dock→Fault→Reset→Idle; báo cáo tiêu chí đạt.

### **Tasks:**
- [ ] Kịch bản chính và biên
- [ ] Thu thập metrics (latency, jitter)
- [ ] Báo cáo kết quả

### **Priority:** Medium
### **Estimate:** 3 ngày