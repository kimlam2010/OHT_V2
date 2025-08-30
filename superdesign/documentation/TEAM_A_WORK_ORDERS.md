# LỆNH TRIỂN KHAI - TEAM A (Layout & Navigation)

Phiên bản: v1.0  
Ngày phát hành: 2025-01-28  
Tham chiếu: `.superdesign/documentation/TEAM_A_PLAN.md`, `NAVIGATION_GUIDE.md`, `PAGE_TEMPLATE.html`

---

## 1) Mục tiêu bắt buộc (Immediate)
- Loại bỏ sidebar bên trái ở TẤT CẢ các trang trong `.superdesign/design_iterations/`.
- Bắt buộc Top Navigation + Emergency Controls (E-STOP, Pause, Reset) hiện diện trên mọi trang.
- Breadcrumb trong `main` của từng page.
- Responsive + Accessibility đạt WCAG 2.1 AA (skip-link, focus-visible, ARIA cơ bản).

Deadline: 3 ngày làm việc.

---

## 2) Phân công & Công việc cụ thể

### W1. Cập nhật Navigation cho trang hiện có (Ưu tiên cao)
- Cập nhật:
  - `core_pages/1-1-auth-login.html`
  - `core_pages/2-1-dashboard-main.html` (chỉ verify, đã đúng)
  - `core_pages/4-1-map-interface.html`
- Cập nhật foundation demos có navigation cũ (toàn bộ file trong `foundation/`).
- Kiểm tra responsive (mobile/tablet/desktop), keyboard navigation, skip-link.

Kết quả bàn giao: PR "TEAM-A-NAVIGATION-UPDATE" + checklist pass.

### W2. Tạo trang còn thiếu theo template
- Tạo mới (sử dụng `PAGE_TEMPLATE.html`):
  - `core_pages/3-1-control-panel.html`
  - `functional_pages/6-1-configuration.html`
  - `functional_pages/7-1-reports.html`
  - `advanced_pages/8-1-alerts.html`
  - `advanced_pages/9-1-voice-control.html`
- Mỗi trang phải có: Top Nav + Emergency, Breadcrumb, `page-content` khung section.

Kết quả bàn giao: PR "TEAM-A-NEW-PAGES" + ảnh chụp responsive.

### W3. Rà soát Dashboard
- `2-1-dashboard-main.html`: xác nhận đủ section, phím tắt, realtime mock.
- Sửa lỗi layout nếu có; đảm bảo không tràn mobile.

Kết quả bàn giao: biên bản kiểm tra + ảnh chụp 3 breakpoint.

### W4. Cập nhật tài liệu & báo cáo
- Cập nhật `TEAM_A_COMPLETION_REPORT.md` phần tuân thủ navigation mới.
- Nếu phát hiện lệch so với guide, cập nhật `NAVIGATION_GUIDE.md` (minor) và nêu rõ trong PR.

Kết quả bàn giao: PR "TEAM-A-DOCS-UPDATE".

---

## 3) Tiêu chí chấp nhận (Gate)
- 100% trang có Top Navigation + Emergency Controls.
- 0 trang còn sidebar trái.
- 100% trang có Breadcrumb.
- Responsive pass (≤768, 768–1024, ≥1024).
- Keyboard nav: Tab/Shift+Tab, Enter/Space hoạt động; skip-link visible.

---

## 4) Quy trình PR & Review
- Nhánh: `feat/team-a/navigation-update` và `feat/team-a/new-pages`.
- PR phải kèm: mô tả, screenshots 3 breakpoint, checklist A11y.
- Người review: Designer Leader + 1 peer Team A.

Checklist PR:
- [ ] Không còn sidebar trái ở bất kỳ trang nào.
- [ ] Top Nav + Emergency hiện diện, hoạt động.
- [ ] Breadcrumb đúng thứ bậc.
- [ ] Responsive + A11y đạt yêu cầu.
- [ ] Liên kết không gãy.

---

## 5) Liên hệ & Hỗ trợ
- Template & Guide: `PAGE_TEMPLATE.html`, `NAVIGATION_GUIDE.md`
- Theme: `themes/theme_oht50_unified.css`, `themes/theme_components.css`
- Vấn đề blocker báo ngay Designer Leader.

— End of Order —
