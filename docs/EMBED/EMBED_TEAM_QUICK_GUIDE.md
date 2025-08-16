# 🔧 EMBED Team Quick Guide - UPDATED FOR CLEAN DOCS

**Team:** EMBED (Hardware & Low-level Drivers)  
**Status:** ✅ 90.9% COMPLETE (10/11 tasks)  
**Next Task:** EM-12 (LiDAR Driver & USB Integration)

---

## 📁 **Tài liệu của bạn (CẬP NHẬT CẤU TRÚC MỚI)**

### **📋 Task Management:**
- **Main tasks:** `../tasks/PM_TASKS_OHT-50.md`
- **Your prompts:** `../tasks/EMBED_PROMPT_TEMPLATE.md`
- **Status tracking:** `../tasks/EMBED_FW_STATUS_SUMMARY.md`

### **📊 Progress & Communication:**
- **Progress update:** `EMBED_PROGRESS_UPDATE.md` ← **Cập nhật ở đây**
- **Team notices:** `EMBED_TEAM_NOTICE.md`
- **PM communication:** `PM_NOTICE.md`

### **🔧 Technical Specifications:**
- **IO Drivers:** `../specs/embedded_io_drivers.md`
- **EMI/ESD Guidelines:** `../specs/emi_esd_guidelines.md`
- **Production Testing:** `../specs/production_test_checklist.md`
- **HIL Testing:** `../specs/bench_hil_testing.md`
- **Bootloader:** `../specs/bootloader_layout.md`
- **RS485 Wiring:** `../specs/rs485_wiring_verification.md`

### **🚨 QUAN TRỌNG - CẤU TRÚC MỚI:**
**Tất cả docs đã được gom về 1 thư mục duy nhất: `/home/orangepi/OHT-50/docs/`**

**KHÔNG CÒN:**
- ❌ `/home/orangepi/OHT-50/OHT-50/docs/`
- ❌ `/home/orangepi/OHT-50/EMBED/docs/`
- ❌ `/home/orangepi/OHT-50/OHT-50/OHT-50/docs/`

**CHỈ CÓ:**
- ✅ `/home/orangepi/OHT-50/docs/` ← **DUY NHẤT**

---

## 🚨 **CURRENT PRIORITY: EM-12**

### **Task:** LiDAR Driver & USB Integration
### **Status:** ❌ PENDING - Cần implement

### **Documentation đã sẵn sàng:**
- `../specs/lidar_wiring_guide.md`
- `../specs/lidar_setup_guide.md`
- `../specs/lidar_test_procedures.md`
- `../specs/lidar_integration_guide.md`

### **Prompt template:** `../tasks/EMBED_PROMPT_TEMPLATE.md` (EM-12 section)

---

## ✅ **Completed Tasks (10/11):**
1. ✅ EM-01: Hardware bring-up
2. ✅ EM-02: RS485/CAN transceiver
3. ✅ EM-03: UART/CAN init + DMA
4. ✅ EM-04: Bootloader layout
5. ✅ EM-05: IO drivers (encoder, limit switch, E-Stop)
6. ✅ EM-06: EMI/ESD guidelines
7. ✅ EM-07: HIL testing bench
8. ✅ EM-08: Production test checklist
9. ✅ EM-09: RS485 wiring verification
10. ✅ EM-11: UART1 enable và validation

---

## 📝 **Quy tắc cập nhật (CẬP NHẬT):**

### **1. Progress Update:**
```bash
# Cập nhật progress trong docs/EMBED/
vim docs/EMBED/EMBED_PROGRESS_UPDATE.md
```

### **2. Technical Documentation:**
```bash
# Cập nhật specs trong docs/specs/
vim docs/specs/[spec_name].md
```

### **3. PM Communication:**
```bash
# Thông báo cho PM trong docs/EMBED/
vim docs/EMBED/PM_NOTICE.md
```

---

## 🎯 **Next Steps (CẬP NHẬT):**

1. **Read LiDAR documentation** trong `docs/specs/`
2. **Use prompt template** từ `docs/tasks/EMBED_PROMPT_TEMPLATE.md`
3. **Implement EM-12** (LiDAR Driver & USB Integration)
4. **Update progress** trong `docs/EMBED/EMBED_PROGRESS_UPDATE.md`
5. **Notify PM** khi hoàn thành

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

**🎉 Chúc may mắn! EMBED team đã làm rất tốt! 🎉**

**📚 Tài liệu đã được dọn dẹp và tổ chức chuyên nghiệp!**
