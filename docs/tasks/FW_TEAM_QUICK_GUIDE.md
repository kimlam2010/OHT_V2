# ⚡ FW Team Quick Guide - UPDATED FOR CLEAN DOCS

**Team:** FW (Firmware & Real-time Control)  
**Status:** 🚀 READY TO START (0/15 tasks)  
**Dependencies:** ✅ All cleared

---

## 📁 **Tài liệu của bạn (CẬP NHẬT CẤU TRÚC MỚI)**

### **📋 Task Management:**
- **Main tasks:** `PM_TASKS_OHT-50.md`
- **Your prompts:** `FW_PROMPT_TEMPLATE.md`
- **Status tracking:** `EMBED_FW_STATUS_SUMMARY.md`

### **🔧 Technical Specifications:**
- **IO Drivers:** `../specs/embedded_io_drivers.md`
- **RS485 Wiring:** `../specs/rs485_wiring_verification.md`
- **HIL Testing:** `../specs/bench_hil_testing.md`
- **LiDAR Integration:** `../specs/lidar_integration_guide.md`

### **🚨 QUAN TRỌNG - CẤU TRÚC MỚI:**
**Tất cả docs đã được gom về 1 thư mục duy nhất: `/home/orangepi/OHT-50/docs/`**

**KHÔNG CÒN:**
- ❌ `/home/orangepi/OHT-50/OHT-50/docs/`
- ❌ `/home/orangepi/OHT-50/EMBED/docs/`
- ❌ `/home/orangepi/OHT-50/OHT-50/OHT-50/docs/`

**CHỈ CÓ:**
- ✅ `/home/orangepi/OHT-50/docs/` ← **DUY NHẤT**

---

## 🚀 **READY TO START TASKS:**

### **Priority 1 (Critical Path):**
1. **FW-01:** Khung FW + scheduler (3 ngày)
2. **FW-02:** HAL abstraction (4 ngày)
3. **FW-07:** RS485 protocol (3 ngày)
4. **FW-11:** LiDAR Integration (3 ngày) ← **Chờ EM-12**

### **Priority 2:**
5. **FW-03:** State machine (3 ngày)
6. **FW-09:** Safety & interlock (3 ngày)
7. **FW-10:** OTA/Bootloader (4 ngày)

### **Priority 3:**
8. **FW-04:** Control loop (4 ngày)
9. **FW-05:** Telemetry (3 ngày)
10. **FW-06:** Configuration (2 ngày)
11. **FW-08:** Error handling (2 ngày)
12. **FW-12:** HIL testing (3 ngày)
13. **FW-13:** Performance optimization (3 ngày)
14. **FW-14:** Documentation (2 ngày)
15. **FW-15:** Final integration (3 ngày)

---

## 📝 **Quy tắc sử dụng (CẬP NHẬT):**

### **1. Bắt đầu với prompt template:**
```bash
# Đọc prompt template trong docs/tasks/
vim docs/tasks/FW_PROMPT_TEMPLATE.md
```

### **2. Chọn task và implement:**
```bash
# Ví dụ: FW-01
# Copy prompt từ docs/tasks/FW_PROMPT_TEMPLATE.md
# Implement theo yêu cầu
```

### **3. Cập nhật progress:**
```bash
# Cập nhật status trong docs/tasks/
vim docs/tasks/EMBED_FW_STATUS_SUMMARY.md
```

---

## 🔗 **Dependencies đã sẵn sàng:**

### **✅ EMBED Team Deliverables:**
- **Hardware:** UART1 RS485 với pins 46, 47
- **HAL Interface:** `hal_rs485.py` hoàn chỉnh
- **Test Scripts:** 8 scripts test đầy đủ
- **Documentation:** Tất cả EMBED files trong `docs/EMBED/`

### **⚠️ Pending Dependency:**
- **EM-12:** LiDAR Driver & USB Integration (EMBED team)
- **Impact:** FW-11 (LiDAR Integration) chờ EM-12

---

## 🎯 **Recommended Start Order:**

### **Week 1:**
1. **FW-01:** Khung FW + scheduler
2. **FW-02:** HAL abstraction
3. **FW-07:** RS485 protocol

### **Week 2:**
4. **FW-03:** State machine
5. **FW-09:** Safety & interlock
6. **FW-10:** OTA/Bootloader

### **Week 3:**
7. **FW-11:** LiDAR Integration (nếu EM-12 đã xong)
8. **FW-04:** Control loop
9. **FW-05:** Telemetry

---

## 📞 **Support & Communication (CẬP NHẬT):**

### **EMBED Team Support:**
- **Hardware questions:** `../EMBED/EMBED_TEAM_NOTICE.md`
- **Progress tracking:** `../EMBED/EMBED_PROGRESS_UPDATE.md`

### **PM Communication:**
- **Status updates:** `EMBED_FW_STATUS_SUMMARY.md`
- **Task completion:** Update trong `PM_TASKS_OHT-50.md`

---

## 🚨 **Important Notes (CẬP NHẬT):**

### **1. LiDAR Integration:**
- **FW-11** phụ thuộc vào **EM-12** (EMBED team)
- **Documentation:** Đã sẵn sàng trong `docs/specs/`
- **Prompt template:** Đã có trong `docs/tasks/FW_PROMPT_TEMPLATE.md`

### **2. Real-time Constraints:**
- **Control loop:** < 1ms response time
- **Safety response:** < 100ms E-Stop
- **Performance:** Optimize cho Orange Pi 5B (RK3588)

### **3. Integration Points:**
- **Backend:** Telemetry và configuration
- **Frontend:** Real-time status và control
- **EMBED:** Hardware abstraction layer

---

## 🚨 **LƯU Ý QUAN TRỌNG:**

### **Cấu trúc docs đã thay đổi:**
- **Tất cả docs** đã được gom về `/home/orangepi/OHT-50/docs/`
- **Không còn** các thư mục docs lộn xộn
- **Chỉ có 1 thư mục docs** duy nhất

### **Đường dẫn mới:**
- **Task management:** `docs/tasks/`
- **Technical specs:** `docs/specs/`
- **EMBED files:** `docs/EMBED/`
- **Backend docs:** `docs/backend/`
- **Radxa docs:** `docs/dev_radxa/`

### **Tránh nhầm lẫn:**
- **KHÔNG** đọc từ `/home/orangepi/OHT-50/OHT-50/docs/`
- **KHÔNG** đọc từ `/home/orangepi/OHT-50/EMBED/docs/`
- **CHỈ** đọc từ `/home/orangepi/OHT-50/docs/`

---

**🚀 FW Team: Sẵn sàng bắt đầu! Tất cả dependencies đã clear! 🚀**

**📚 Tài liệu đã được dọn dẹp và tổ chức chuyên nghiệp!**
