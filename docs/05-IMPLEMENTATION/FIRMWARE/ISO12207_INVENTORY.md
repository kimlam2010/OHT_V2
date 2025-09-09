# ISO/IEC/IEEE 12207 Inventory & Mapping - OHT-50 Firmware

**Phiên bản:** 1.0  
**Ngày cập nhật:** 2025-09-09  
**Phạm vi:** Danh mục tài liệu hiện có và ánh xạ đích theo cấu trúc 12207

---

## 📦 Phạm vi inventory (đợt 1)
- 01-QMS/
- 02-HAL/
- 02-REQUIREMENTS/
- 02-RISK/
- 03-SLC/
- 04-SAFETY/
- 05-QUALITY/
- ISSUES/

Ghi chú: Chỉ inventory và lập kế hoạch di chuyển, không đổi vị trí file trong đợt 1.

---

## 🔗 Bảng ánh xạ cấp thư mục
| Nguồn hiện tại | Đích 12207 | Ghi chú |
|---|---|---|
| 01-QMS/ | 08-MANAGEMENT/02-quality-management/ | QMS là thành phần quản lý chất lượng |
| 02-HAL/ | 03-DESIGN/02-detailed-design/ và 04-IMPLEMENTATION/01-coding/ | Tách thiết kế vs triển khai |
| 02-REQUIREMENTS/ | 02-REQUIREMENTS/01-system-requirements/, 02-software-requirements/, 03-interface-requirements/ | Giữ nguyên nội dung, chuẩn hoá phân loại |
| 02-RISK/ | 01-PLANNING/03-risk-planning/ và 08-MANAGEMENT/01-project-management/ | Kế hoạch rủi ro + hồ sơ quản trị |
| 03-SLC/ | 01-PLANNING/ và 08-MANAGEMENT/01-project-management/ | Tài liệu kế hoạch & báo cáo tiến độ |
| 04-SAFETY/ | 02-REQUIREMENTS, 03-DESIGN, 05-TESTING | Tách theo loại: req/design/test |
| 05-QUALITY/ | 08-MANAGEMENT/02-quality-management/ | Báo cáo QA/QC |
| ISSUES/ | 08-MANAGEMENT/03-configuration-management/ | Nhật ký thay đổi/issue tracking |

---

## 📄 Mục chi tiết (ví dụ đại diện)

### 01-QMS/
- 01-policies/quality_policy.md → 08-MANAGEMENT/02-quality-management/quality_policy.md
- README.md → 08-MANAGEMENT/02-quality-management/README.md

### 02-HAL/
- README.md → 03-DESIGN/02-detailed-design/hal/README.md (bản thiết kế)
- 03-lidar/LIDAR_DEBUG_GUIDE.md → 05-TESTING/02-integration-testing/lidar_debug_guide.md (hướng dẫn thử/GUIDE)

### 02-REQUIREMENTS/
- 03-FIRMWARE-REQUIREMENTS/04-IMPLEMENTED-MODULES/REQ_RS485_HAL_SPECIFICATION.md → 02-REQUIREMENTS/02-software-requirements/rs485_hal_spec.md
- 03-FIRMWARE-REQUIREMENTS/REQ_API_INTEGRATION_SPECIFICATION.md → 02-REQUIREMENTS/02-software-requirements/api_integration_spec.md

### 02-RISK/
- 01-risk-assessment/hazard_analysis.md → 01-PLANNING/03-risk-planning/hazard_analysis.md
- README.md → 01-PLANNING/03-risk-planning/README.md

### 03-SLC/
- 01-planning/API_INTEGRATION_IMPLEMENTATION_PLAN.md → 01-PLANNING/01-project-planning/api_integration_plan.md
- 01-planning/implementation/retrospective/MANAGERS_REMEDIATION_PLAN.md → 08-MANAGEMENT/01-project-management/managers_remediation_plan.md
- CHANGELOG.md → 08-MANAGEMENT/01-project-management/changelog_firmware_summaries.md

### 04-SAFETY/
- 02-safety-integration/LIDAR_SAFETY_INTEGRATION_FINAL_REPORT.md → 05-TESTING/03-system-testing/lidar_safety_final_report.md
- 04-safety-api/SAFETY_MONITOR_API_REFERENCE.md → 03-DESIGN/03-interface-design/safety_monitor_api_reference.md

### 05-QUALITY/
- 01-qa-reports/QA_QC_FIRMWARE_REPORT.md → 08-MANAGEMENT/02-quality-management/qa_qc_firmware_report.md

---

## ✅ Checklist di chuyển (đợt 2)
- [ ] Tạo subfolder đích theo bảng ánh xạ (chỉ khi cần)
- [ ] Thêm README ở mỗi subfolder (mục lục con)
- [ ] Tạo alias link từ vị trí cũ → đích (relative links)
- [ ] Cập nhật tất cả cross-links (README chính + tài liệu liên quan)
- [ ] Kiểm tra link gãy (md-link-check) và sửa

---

## 🧪 QA/QC kiểm tra
- [ ] Liên kết hợp lệ trong `firmware_new/docs` và `docs/05-IMPLEMENTATION/FIRMWARE`
- [ ] Traceability: mỗi tài liệu có mục "Liên kết liên quan"
- [ ] Changelog cập nhật khi di chuyển

---

## 📝 Changelog
- v1.0 (2025-09-09): Khởi tạo inventory và bảng ánh xạ chi tiết (đại diện)
