# 📚 OHT-50 Documentation Structure - CLEANED & ORGANIZED

**Phiên bản:** v2.1  
**Ngày cập nhật:** 2025-01-27  
**PM:** OHT-50 Project Manager  
**Status:** ✅ COMPLETELY CLEANED - Tất cả docs đã được gom về một chỗ!

---

## 🎉 **ĐÃ DỌN DẸP HOÀN TOÀN!**

### **Trước đây:** 5 thư mục `docs/` lộn xộn
- ❌ `/home/orangepi/OHT-50/docs`
- ❌ `/home/orangepi/OHT-50/OHT-50/OHT-50/backend/docs`
- ❌ `/home/orangepi/OHT-50/OHT-50/OHT-50/docs`
- ❌ `/home/orangepi/OHT-50/EMBED/docs`
- ❌ `/home/orangepi/OHT-50/OHT-50/docs`

### **Bây giờ:** 1 thư mục `docs/` duy nhất ✅
- ✅ `/home/orangepi/OHT-50/docs` ← **DUY NHẤT**

---

## 🗂️ **Cấu trúc thư mục tài liệu (ĐÃ DỌN DẸP HOÀN TOÀN)**

```
docs/
├── README.md                    # ← Bạn đang đọc file này
├── EMBED/                       # EMBED team files
│   ├── EMBED_PROGRESS_UPDATE.md
│   ├── EMBED_TEAM_NOTICE.md
│   ├── PM_NOTICE.md
│   └── EMBED_TEAM_QUICK_GUIDE.md  # ← Hướng dẫn EMBED team
├── backend/                     # Backend team docs
│   ├── DEPLOYMENT.md
│   ├── TELEMETRY_API.md
│   ├── BE-02_COMPLETION_SUMMARY.md
│   └── BACKEND_TEAM_QUICK_GUIDE.md  # ← Hướng dẫn Backend team
├── design/                      # 🎨 Design System & UI/UX
│   ├── DESIGN_SYSTEM.md         # ← Design tokens, colors, typography
│   ├── COMPONENT_LIBRARY.md     # ← Reusable components
│   └── ACCESSIBILITY.md         # ← WCAG 2.1 AA guidelines
├── architecture/                # Architecture decisions
│   └── decisions/               # ADR (Architecture Decision Records)
│       ├── ADR-001-SYSTEM_ARCHITECTURE.md
│       ├── ADR-002-AUTHENTICATION_SYSTEM.md
│       ├── ADR-003-DATABASE_ARCHITECTURE.md
│       ├── ADR-004-COMMUNICATION_PROTOCOLS.md
│       ├── ADR-005-SAFETY_ARCHITECTURE.md
│       ├── ADR-FE-01-ROUTING_ARCHITECTURE.md
│       ├── ADR-FE-02-DATA_FETCHING.md
│       ├── ADR-FE-03-CHARTING_LIBRARY.md
│       ├── ADR-FE-04-DESIGN_SYSTEM.md
│       ├── ADR-FE-05-INTERNATIONALIZATION.md
│       └── ADR-FE-06-PWA_STRATEGY.md
├── dev_radxa/                   # Radxa development docs
├── specs/                       # Technical specifications
│   ├── embedded_io_drivers.md
│   ├── emi_esd_guidelines.md
│   ├── production_test_checklist.md
│   ├── bench_hil_testing.md
│   ├── bootloader_layout.md
│   ├── rs485_wiring_verification.md
│   ├── lidar_wiring_guide.md
│   ├── lidar_setup_guide.md
│   ├── lidar_test_procedures.md
│   └── lidar_integration_guide.md
├── tasks/                       # Task management & prompts
│   ├── PM_TASKS_OHT-50.md
│   ├── EMBED_PROMPT_TEMPLATE.md
│   ├── FW_PROMPT_TEMPLATE.md
│   ├── PROGRAMMING_PROMPT_OHT-50.md
│   ├── PROMPT_USAGE_GUIDE.md
│   ├── EMBED_FW_STATUS_SUMMARY.md
│   ├── PM_EMBED_REVIEW_REPORT.md
│   ├── PM_EMBED_COMPLETION_NOTICE.md
│   ├── EMBED_TEAM_QUICK_GUIDE.md
│   ├── FW_TEAM_QUICK_GUIDE.md      # ← Hướng dẫn FW team
│   └── FRONTEND_TEAM_QUICK_GUIDE.md # ← Hướng dẫn Frontend team
└── tests/                       # Test documentation
```

---

## 👥 **Team Quick Guides (MỚI - CẬP NHẬT)**

### **🔧 EMBED Team:**
- **File:** `EMBED/EMBED_TEAM_QUICK_GUIDE.md`
- **Status:** ✅ 90.9% COMPLETE (10/11 tasks)
- **Next Task:** EM-12 (LiDAR Driver & USB Integration)
- **Cấu trúc docs:** Đã cập nhật với đường dẫn mới

### **⚡ FW Team:**
- **File:** `tasks/FW_TEAM_QUICK_GUIDE.md`
- **Status:** 🚀 READY TO START (0/15 tasks)
- **Dependencies:** ✅ All cleared
- **Cấu trúc docs:** Đã cập nhật với đường dẫn mới

### **🌐 Backend Team:**
- **File:** `backend/BACKEND_TEAM_QUICK_GUIDE.md`
- **Status:** 🔄 IN PROGRESS (2/17 tasks completed)
- **Current:** BE-03, BE-05, BE-08, BE-15 in progress
- **Cấu trúc docs:** Đã cập nhật với đường dẫn mới

### **🎨 Frontend Team:**
- **File:** `tasks/FRONTEND_TEAM_QUICK_GUIDE.md`
- **Status:** ✅ 100% COMPLETE (25/25 tasks)
- **Achievement:** All tasks completed successfully!
- **Design System:** ✅ Complete (`design/DESIGN_SYSTEM.md`)
- **Component Library:** ✅ Complete (`design/COMPONENT_LIBRARY.md`)
- **Accessibility:** ✅ Complete (`design/ACCESSIBILITY.md`)
- **ADR Frontend:** ✅ Complete (6 ADR files)
- **Cấu trúc docs:** Đã cập nhật với đường dẫn mới

---

## 🚨 **LƯU Ý QUAN TRỌNG CHO TẤT CẢ TEAMS**

### **Cấu trúc docs đã thay đổi hoàn toàn:**
- **Tất cả docs** đã được gom về `/home/orangepi/OHT-50/docs/`
- **Không còn** các thư mục docs lộn xộn

### **🎨 Design System đã được bổ sung:**
- **Design System:** `docs/design/DESIGN_SYSTEM.md` - Tokens, colors, typography
- **Component Library:** `docs/design/COMPONENT_LIBRARY.md` - Reusable components
- **Accessibility:** `docs/design/ACCESSIBILITY.md` - WCAG 2.1 AA guidelines
- **Frontend ADR:** 6 ADR files trong `docs/architecture/decisions/`
- **Chỉ có 1 thư mục docs** duy nhất

### **Đường dẫn mới (CẬP NHẬT):**
- **Task management:** `docs/tasks/`
- **Backend docs:** `docs/backend/`
- **Technical specs:** `docs/specs/`
- **EMBED files:** `docs/EMBED/`
- **Radxa docs:** `docs/dev_radxa/`
- **Test docs:** `docs/tests/`

### **Tránh nhầm lẫn:**
- **KHÔNG** đọc từ `/home/orangepi/OHT-50/OHT-50/docs/`
- **KHÔNG** đọc từ `/home/orangepi/OHT-50/EMBED/docs/`
- **KHÔNG** đọc từ `/home/orangepi/OHT-50/OHT-50/OHT-50/docs/`
- **CHỈ** đọc từ `/home/orangepi/OHT-50/docs/`

---

## 🔧 Deploy/Ports/Services (Production Standard - No Nginx)

- Services:
  - `oht50.service` (FW) → runs firmware binary. No HTTP.
  - `oht50-backend.service` (BE) → FastAPI on port 8000.
  - `oht50-frontend.service` (FE) → Node static server on port 8081 serving `frontend/dist`.
- Ports:
  - FW: n/a
  - BE: 8000 (`GET /health` returns 200 JSON `{status:"ok"}`)
  - FE dev: 5173 (Vite)
  - FE prod: 8081 (Node service)
- Policy:
  - No Nginx in deploy path. CI fails on any `nginx` reference under `deploy/`.
  - No nested `OHT-50/OHT-50/**` directories.


## 📋 **Team Status Overview**

### **✅ Frontend Team:** 100% COMPLETE
- **25/25 tasks** completed
- **Dashboard hoàn chỉnh** với real-time telemetry
- **Ready for integration testing**

### **🔄 Backend Team:** IN PROGRESS
- **2/17 tasks** completed
- **BE-03, BE-05, BE-08, BE-15** in progress
- **API development** ongoing

### **🔧 EMBED Team:** 90.9% COMPLETE
- **10/11 tasks** completed
- **EM-12 (LiDAR)** pending
- **Hardware bring-up** complete

### **⚡ FW Team:** READY TO START
- **0/15 tasks** started
- **All dependencies** cleared
- **Ready to begin** FW development

---

## 🎯 **Next Steps for Each Team**

### **🔧 EMBED Team:**
1. **Complete EM-12** (LiDAR Driver & USB Integration)
2. **Update progress** trong `docs/EMBED/EMBED_PROGRESS_UPDATE.md`
3. **Notify PM** khi hoàn thành

### **⚡ FW Team:**
1. **Start FW-01** (Khung FW + scheduler)
2. **Use prompt template** từ `docs/tasks/FW_PROMPT_TEMPLATE.md`
3. **Update status** trong `docs/tasks/EMBED_FW_STATUS_SUMMARY.md`

### **🌐 Backend Team:**
1. **Complete BE-03** (API cấu hình CRUD)
2. **Complete BE-05** (Telemetry ingest)
3. **Complete BE-08** (Mock device)

### **🎨 Frontend Team:**
1. **Support integration testing** với other teams
2. **Monitor performance** của real-time features
3. **Documentation updates** khi cần thiết

---

## 📞 **PM Communication**

### **Status Updates:**
- **File:** `docs/tasks/EMBED_FW_STATUS_SUMMARY.md`
- **Updated:** Daily by PM
- **Teams:** Update progress here

### **Task Management:**
- **File:** `docs/tasks/PM_TASKS_OHT-50.md`
- **Updated:** When tasks completed
- **Teams:** Reference for task assignments

### **Team Notices:**
- **EMBED:** `docs/EMBED/PM_NOTICE.md`
- **All Teams:** Use respective quick guides

---

## 🎉 **Kết quả dọn dẹp**

### **✅ Đã hoàn thành:**
- **Consolidated** 5 docs directories → 1
- **Created** team-specific quick guides
- **Updated** all file paths và references
- **Organized** documentation hierarchy
- **Eliminated** confusion và scattered files

### **✅ Benefits:**
- **Single source of truth** cho documentation
- **Clear team responsibilities** và file locations
- **Professional organization** cho client
- **Easy navigation** cho all teams
- **No more documentation mess**

---

**📚 Tài liệu đã được dọn dẹp và tổ chức chuyên nghiệp!**

**🎯 Tất cả teams có thể tìm thấy tài liệu của mình dễ dàng!**

**🚀 Project ready for efficient development!**

## System Core Specs & ADRs

- [System Architecture](specs/architecture.md)
- [State Machine](specs/state_machine.md)
- [Interfaces](specs/interfaces.md)
- [ADR-0001 System Architecture Baseline](architecture/decisions/0001-system-architecture-baseline.md)
- [ADR-0002 Runtime Config vs Firmware](architecture/decisions/0002-runtime-config-vs-firmware.md)
- [ADR-0003 Safety: E-Stop Dual Channel](architecture/decisions/0003-safety-estop-dual-channel.md)
