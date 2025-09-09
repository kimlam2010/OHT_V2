# ISO/IEC/IEEE 12207 Mapping & Migration Plan - OHT-50 Firmware

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-09-09  
**Phạm vi:** Restructure tài liệu firmware theo chuẩn ISO/IEC/IEEE 12207:2017

---

## 🎯 Mục tiêu
- Chuẩn hoá cấu trúc tài liệu theo vòng đời phần mềm 12207
- Duy trì traceability: Yêu cầu → Thiết kế → Triển khai → Kiểm thử → Bảo trì
- Không làm gãy liên kết: cung cấp alias/link chuyển hướng trong giai đoạn chuyển tiếp

---

## 🗂️ Cấu trúc thư mục mục tiêu (12207)
```
firmware_new/docs/
├── 01-PLANNING/
│   ├── 01-project-planning/
│   ├── 02-quality-planning/
│   └── 03-risk-planning/
├── 02-REQUIREMENTS/
│   ├── 01-system-requirements/
│   ├── 02-software-requirements/
│   └── 03-interface-requirements/
├── 03-DESIGN/
│   ├── 01-architecture-design/
│   ├── 02-detailed-design/
│   └── 03-interface-design/
├── 04-IMPLEMENTATION/
│   ├── 01-coding/
│   ├── 02-integration/
│   └── 03-deployment/
├── 05-TESTING/
│   ├── 01-unit-testing/
│   ├── 02-integration-testing/
│   ├── 03-system-testing/
│   └── 04-acceptance-testing/
├── 06-MAINTENANCE/
│   ├── 01-corrective/
│   ├── 02-adaptive/
│   └── 03-perfective/
├── 07-SUPPORT/
│   ├── 01-user-support/
│   ├── 02-technical-support/
│   └── 03-training/
└── 08-MANAGEMENT/
    ├── 01-project-management/
    ├── 02-quality-management/
    └── 03-configuration-management/
```

---

## 🔗 Bảng ánh xạ tài liệu hiện tại → 12207
- `03-SLC/` → `01-PLANNING/` (kế hoạch) và `08-MANAGEMENT/01-project-management/` (theo nội dung)
- `01-QMS/` → `08-MANAGEMENT/02-quality-management/` (chính) và tham chiếu chéo tới `01-PLANNING/02-quality-planning/`
- `02-REQUIREMENTS/` → `02-REQUIREMENTS/01-system-requirements/` và `02-software-requirements/`
- `02-HAL/` (thiết kế/triển khai HAL) → `03-DESIGN/02-detailed-design/` + `04-IMPLEMENTATION/01-coding/`
- `04-SAFETY/` → tách: yêu cầu/thiết kế vào `02-REQUIREMENTS` & `03-DESIGN`; kiểm thử/validation vào `05-TESTING`
- `05-QUALITY/` → hồ sơ QA/QC sang `08-MANAGEMENT/02-quality-management/`
- `ISSUES/` → hồ sơ cấu hình/chất lượng: `08-MANAGEMENT/03-configuration-management/`

Chi tiết ánh xạ mức file sẽ được liệt kê trong bảng kiểm di chuyển (sau khi inventory).

---

## ✅ Phạm vi di chuyển đợt 1 (không phá liên kết)
1. Tạo skeleton thư mục 12207 với README stub
2. Cập nhật index để hiển thị cấu trúc mới (dual-nav trong giai đoạn chuyển tiếp)
3. Inventory & lập bảng ánh xạ tệp → đích
4. Tạo alias link (relative links) từ vị trí cũ đến vị trí mới (chuyển dần)

---

## 🧪 Tiêu chí QA/QC
- Không có liên kết gãy trong `firmware_new/docs` và `docs/05-IMPLEMENTATION/FIRMWARE`
- README ở mỗi cấp liệt kê rõ tài liệu con và đường dẫn mới
- Bảng ánh xạ đầy đủ ≥ 95% tài liệu trong lần rà soát đầu

---

## 📅 Timeline đề xuất
- Tuần 1: Tạo skeleton + cập nhật index; inventory và bảng ánh xạ
- Tuần 2: Di chuyển theo nhóm (Requirements → Design → Testing), thêm alias và fix link

---

## 👥 Trách nhiệm
- **Doc Lead (FW):** Kiến trúc thư mục, chuẩn định danh
- **QA/QC:** Kiểm link, checklist 12207, traceability
- **Các nhóm module:** Cập nhật nội dung README con sau khi di chuyển

---

## 📝 Changelog
- v1.0 (2025-09-09): Khởi tạo kế hoạch và cấu trúc 12207, định nghĩa ánh xạ cấp thư mục
