# TEAM A - KẾ HOẠCH TRIỂN KHAI (v1.0)

Phiên bản: v1.0  
Cập nhật: 2025-01-28  
Phạm vi: Layout & Navigation cho OHT-50 (không dùng sidebar trái, bắt buộc Top Navigation + Emergency Controls)

---

## 1) Mục tiêu
- Chuẩn hóa navigation: 100% trang dùng Top Navigation + Emergency Controls, Breadcrumb trong main.
- Xóa bỏ sidebar trái trên toàn bộ pages/foundation demos.
- Hoàn thiện layout chuẩn: page-header, page-main, page-footer, grid 12 cột responsive.
- Đảm bảo WCAG 2.1 AA, keyboard nav, screen reader.

---

## 2) Phạm vi & Artefacts
- Thư mục nguồn: `.superdesign/design_iterations/`
- Tài liệu quy chiếu: `.superdesign/documentation/NAVIGATION_GUIDE.md`, `.superdesign/templates/PAGE_TEMPLATE.html`
- Trang cốt lõi (core):
  - `core_pages/1-1-auth-login.html`
  - `core_pages/2-1-dashboard-main.html`
  - `core_pages/4-1-map-interface.html`
  - (Tạo mới) `core_pages/3-1-control-panel.html`
- Trang chức năng (functional):
  - `functional_pages/5-1-monitoring-interface.html` (nếu chưa có, tạo)
  - (Tạo mới) `functional_pages/6-1-configuration.html`
  - (Tạo mới) `functional_pages/7-1-reports.html`
- Trang nâng cao (advanced):
  - (Tạo mới) `advanced_pages/8-1-alerts.html`
  - (Tạo mới) `advanced_pages/9-1-voice-control.html`
- Foundation demos: cập nhật toàn bộ file có cấu trúc navigation cũ.

---

## 3) Kế hoạch theo giai đoạn

### PHASE 1 — Navigation Update (Ưu tiên)
- Loại bỏ sidebar trái ở tất cả pages.
- Áp dụng Top Navigation với Emergency Controls (E-STOP, Pause, Reset).
- Thêm Breadcrumb vào `main` của mỗi page.
- Responsive kiểm tra: mobile <768, tablet 768–1024, desktop ≥1024.
- Accessibility: landmark roles, skip-link, focus-visible.

Deliverables:
- Cập nhật: `1-1-auth-login.html`, `2-1-dashboard-main.html` (verify), `4-1-map-interface.html`, foundation demos.
- Tài liệu: cập nhật `NAVIGATION_GUIDE.md` (đã có) nếu có điều chỉnh nhỏ.

### PHASE 2 — Tạo Core/Functional Pages còn thiếu
- Tạo `3-1-control-panel.html` theo template chuẩn.
- Tạo `6-1-configuration.html`, `7-1-reports.html` trong `functional_pages/`.
- Tạo `8-1-alerts.html`, `9-1-voice-control.html` trong `advanced_pages/`.

Deliverables: 5 file HTML theo `PAGE_TEMPLATE.html` + breadcrumb + sections khung.

### PHASE 3 — Hoàn thiện Dashboard
- Rà soát các section đã có: Status, Position & Speed, Safety, Communication, Manual Control, Map, Logs, Performance, Alerts.
- Kiểm tra realtime update giả lập, phím tắt (Esc E-Stop; 1/2/3 chọn mode; mũi tên di chuyển).

Deliverables: Dashboard pass QC checklist, không lỗi responsive/accessibility.

### PHASE 4 — Kiểm thử & Tài liệu
- Cross-browser check (Chromium/Firefox).
- Accessibility quick audit (keyboard only; screen reader labels).
- Cập nhật báo cáo tiến độ và hướng dẫn tích hợp.

Deliverables: `TEAM_A_COMPLETION_REPORT.md` cập nhật mục tuân thủ navigation mới.

---

## 4) Danh sách công việc chi tiết

1. Chuẩn hóa Top Navigation trên mọi trang
   - Brand, menu chính (Dashboard, Control, Map, Monitoring, Config, Reports)
   - Emergency Controls luôn hiển thị (E-STOP, Pause, Reset)
   - Actions: Notifications, User menu

2. Loại bỏ sidebar trái
   - Xóa `aside.sidebar-navigation` nếu còn
   - Điều chỉnh layout để không chừa khoảng trống

3. Thêm Breadcrumb trong `main`
   - Cấu trúc: Home/Dashboard → Current Page
   - Đảm bảo focus order hợp lý

4. Áp dụng `PAGE_TEMPLATE.html` cho trang mới
   - Skip link, landmarks, responsive, focus-visible
   - Lấy token từ theme OHT-50

5. Foundation demos cập nhật
   - `component_library.html`, `design_system.html`, `button_variants.html`, `card_system.html`, `form_components.html`, `accessibility_framework.html`…
   - Đảm bảo ví dụ navigation dùng Top Nav, không có left sidebar

6. Kiểm thử nhanh
   - Mobile menu: ẩn/thu gọn menu; emergency vẫn còn
   - Keyboard: Tab/Shift+Tab; Enter/Space kích hoạt
   - Màn hình nhỏ: không tràn, không clip điều khiển khẩn

---

## 5) Tiêu chí chấp nhận (DoD)
- 100% trang có Top Navigation + Emergency Controls.
- 0 trang còn sidebar trái.
- 100% trang có Breadcrumb trong main.
- Responsive pass ở 3 breakpoint chính.
- WCAG 2.1 AA (keyboard nav, focus visible, aria-label cơ bản).

---

## 6) Timeline đề xuất
- Tuần 1: PHASE 1 (update navigation + foundation demos)
- Tuần 2: PHASE 2 (tạo trang thiếu)
- Tuần 3: PHASE 3 (dashboard hoàn thiện) + PHASE 4 (kiểm thử/tài liệu)

---

## 7) Rủi ro & Giảm thiểu
- Không đồng nhất navigation giữa các trang → Áp dụng nghiêm `PAGE_TEMPLATE.html` + review cặp.
- Vỡ layout mobile do emergency group → Kiểm thử thực tế; co giãn nút; ưu tiên hiển thị.
- Thiếu aria/skip-link → Checklist A11y bắt buộc trước review.

---

## 8) Tài liệu & Chuẩn tham chiếu
- NAVIGATION GUIDE: `.superdesign/documentation/NAVIGATION_GUIDE.md`
- TEMPLATE: `.superdesign/templates/PAGE_TEMPLATE.html`
- THEME: `.superdesign/themes/theme_oht50_unified.css`, `theme_components.css`

---

## 9) Changelog
- v1.0 (2025-01-28): Khởi tạo kế hoạch Team A theo rule bỏ sidebar trái, bắt buộc Top Navigation + Emergency Controls; thêm danh mục file cần tạo/cập nhật, DoD, timeline.
