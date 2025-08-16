# 🌐 Backend Team Quick Guide - UPDATED FOR CLEAN DOCS

**Team:** Backend (API & Services)  
**Status:** 🔄 IN PROGRESS  
**Completed:** BE-01, BE-13

---

## 📁 **Tài liệu của bạn (CẬP NHẬT CẤU TRÚC MỚI)**

### **📋 Task Management:**
- **Main tasks:** `../tasks/PM_TASKS_OHT-50.md`
- **Status tracking:** `../tasks/EMBED_FW_STATUS_SUMMARY.md`

### **🔧 Backend Documentation:**
- **Deployment:** `DEPLOYMENT.md`
- **Telemetry API:** `TELEMETRY_API.md`
- **Completion Summary:** `BE-02_COMPLETION_SUMMARY.md`

### **🚨 QUAN TRỌNG - CẤU TRÚC MỚI:**
**Tất cả docs đã được gom về 1 thư mục duy nhất: `/home/orangepi/OHT-50/docs/`**

**KHÔNG CÒN:**
- ❌ `/home/orangepi/OHT-50/OHT-50/OHT-50/backend/docs/`
- ❌ `/home/orangepi/OHT-50/OHT-50/docs/`
- ❌ `/home/orangepi/OHT-50/EMBED/docs/`

**CHỈ CÓ:**
- ✅ `/home/orangepi/OHT-50/docs/` ← **DUY NHẤT**

---

## 🔄 **CURRENT STATUS**

### **✅ Completed Tasks:**
- **BE-01:** Khởi tạo service (FastAPI/NestJS), cấu trúc repo, cấu hình ENV
- **BE-13:** Mở rộng Telemetry API: `/current` JSON chuẩn, `/history?limit=`, WS theo `ws_hz`, `/rate`, `/reset`, `/stats`

### **🔄 In Progress:**
- **BE-03:** API cấu hình: schema + CRUD theo `config_spec.md`
- **BE-05:** Ingest telemetry (HTTP/WS/MQTT) theo `telemetry_schema.md`
- **BE-08:** Mock device để dev song song FE/FW
- **BE-15:** Đồng bộ schema Telemetry: thêm `tag_id`, `enc.count` vào payload

### **📋 To Do:**
- **BE-04:** Phiên bản hóa cấu hình, import/export JSON/YAML
- **BE-06:** API truy vấn telemetry theo thời gian/kênh
- **BE-07:** Giao tiếp Center: định nghĩa command + WS/SSE
- **BE-09:** Bảo mật: auth cơ bản, RBAC, rate limit, CORS
- **BE-10:** Logging/observability/metrics
- **BE-11:** OTA backend: kho bản phát hành, ký số, checksum
- **BE-12:** API kích hoạt OTA + theo dõi tiến độ, rollback
- **BE-14:** Refactor DI `ConfigService` qua `Depends(get_config_service)`
- **BE-16:** Tích hợp với FW/Center thay vì RS485 trực tiếp
- **BE-17:** Loại bỏ phụ thuộc serial ở BE (sản xuất) + Feature flag cho chế độ mock

---

## 📝 **Quy tắc sử dụng (CẬP NHẬT):**

### **1. Task Management:**
```bash
# Đọc task assignments trong docs/tasks/
vim ../tasks/PM_TASKS_OHT-50.md
```

### **2. Status Updates:**
```bash
# Cập nhật status trong docs/tasks/
vim ../tasks/EMBED_FW_STATUS_SUMMARY.md
```

### **3. Backend Documentation:**
```bash
# Cập nhật backend docs trong docs/backend/
vim DEPLOYMENT.md
vim TELEMETRY_API.md
```

---

## 🔗 **Dependencies & Integration**

### **✅ Dependencies Cleared:**
- **EMBED Team:** Hardware bring-up complete
- **FW Team:** Ready to start (dependencies cleared)
- **Frontend Team:** 100% complete

### **🔄 Integration Points:**
- **Frontend:** API endpoints cho UI
- **FW Team:** Telemetry và configuration
- **EMBED Team:** Hardware abstraction layer

---

## 🎯 **Recommended Next Steps:**

### **Immediate (This Week):**
1. **Complete BE-03:** API cấu hình CRUD
2. **Complete BE-05:** Telemetry ingest
3. **Complete BE-08:** Mock device

### **Next Week:**
4. **BE-06:** API truy vấn telemetry
5. **BE-07:** Giao tiếp Center
6. **BE-09:** Bảo mật cơ bản

---

## 📞 **Support & Communication (CẬP NHẬT):**

### **PM Communication:**
- **Status updates:** `../tasks/EMBED_FW_STATUS_SUMMARY.md`
- **Task completion:** Update trong `../tasks/PM_TASKS_OHT-50.md`

### **Team Coordination:**
- **Frontend Team:** API integration
- **FW Team:** Telemetry và configuration
- **EMBED Team:** Hardware abstraction

---

## 🚨 **Important Notes (CẬP NHẬT):**

### **1. Architecture Decision:**
- **RS485 sẽ do FW/EMBED xử lý hoàn toàn**
- **Backend chỉ nhận dữ liệu/điều khiển thông qua FW/Center**
- **Loại bỏ nhu cầu BE chạy `pyserial`/truy cập `/dev/ttyOHT485` ở production**

### **2. Integration Strategy:**
- **BE tập trung quản lý cấu hình, giám sát, lưu trữ, hiển thị**
- **FW chịu trách nhiệm giao tiếp bus**
- **Mock device cho development**

### **3. Performance Requirements:**
- **API response time:** < 100ms
- **WebSocket latency:** < 50ms
- **Database queries:** Optimized với indexing

---

## 🚨 **LƯU Ý QUAN TRỌNG:**

### **Cấu trúc docs đã thay đổi:**
- **Tất cả docs** đã được gom về `/home/orangepi/OHT-50/docs/`
- **Không còn** các thư mục docs lộn xộn
- **Chỉ có 1 thư mục docs** duy nhất

### **Đường dẫn mới:**
- **Task management:** `docs/tasks/`
- **Backend docs:** `docs/backend/`
- **Technical specs:** `docs/specs/`
- **EMBED files:** `docs/EMBED/`
- **Radxa docs:** `docs/dev_radxa/`

### **Tránh nhầm lẫn:**
- **KHÔNG** đọc từ `/home/orangepi/OHT-50/OHT-50/OHT-50/backend/docs/`
- **KHÔNG** đọc từ `/home/orangepi/OHT-50/OHT-50/docs/`
- **CHỈ** đọc từ `/home/orangepi/OHT-50/docs/`

---

**🌐 Backend Team: Tiếp tục phát triển! Dependencies đã clear! 🌐**

**📚 Tài liệu đã được dọn dẹp và tổ chức chuyên nghiệp!**
