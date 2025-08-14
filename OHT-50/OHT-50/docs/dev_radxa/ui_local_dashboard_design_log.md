## Quyết định thiết kế UI Local Dashboard — OHT-50

- Phiên bản: v1.0
- Ngày cập nhật: hiện tại

### 1) Layout
- Chọn phương án: Sidebar trái cố định (PA-1).
- Thành phần: Header (trạng thái kết nối, Mock, Dark/Light), Sidebar (Tổng quan, Đồ thị v‑a‑x, Nhật ký, Cấu hình an toàn, Chẩn đoán), Nội dung (Thẻ trạng thái Idle/Move/Dock/Fault/E‑Stop; Điều khiển Start/Stop/E‑Stop có xác nhận; Thông số nhanh v/a/x; Đồ thị realtime; Bảng nhật ký), Footer (version/build).

### 2) Theme (màu, font, spacing)
- Tên theme: OHT-50 Vintage Workshop (v1).
- Mục tiêu: Phong cách vintage/industrial (ấm áp, dễ đọc), tránh tông xanh bootstrap.
- Font: `Inter` (UI), `Merriweather` (heading). Sẽ import qua Google Fonts trong HTML.
- Màu chủ đạo: nền parchment (ấm), chữ nâu đậm, nhấn terracotta/olive/ochre.
- File CSS: `.superdesign/design_iterations/oht50_vintage_1.css`.

### 3) Animation (đã duyệt)
- Sidebar: 300ms ease‑out [X‑240→0, α0→1] mở; 250ms ease‑in đóng.
- Header indicators: 250ms [α0.6→1, S0.98→1] khi trạng thái kết nối thay đổi.
- Status badges: active 250ms [bg→primary/secondary, S1→1.03→1]; fault 400ms [α0.6→1, border→destructive] + shake 250ms; E‑Stop pulse 900ms ∞.
- Buttons: hover 180ms [S1→1.03, shadow↗]; press 120ms [S1→0.97→1]; danger (E‑Stop) 160ms [S1→0.96→1, R±2°].
- Modal xác nhận: 280ms [Y+16→0, α0→1], overlay 220ms [α0→1].
- Quick metrics: 300ms stagger 60ms [Y+10→0, α0→1].
- Chart: mount 400ms [α0→1]; smooth update 350–450ms ease; pause/zoom 220ms.
- Log row: new 220ms [Y+6→0, α0→1]; error 280ms shake [X±4].

### 4) HTML (đã tạo)
- File: `.superdesign/design_iterations/oht50_overview_1.html`.
- Công nghệ: TailwindCDN + Flowbite + Lucide; tham chiếu theme CSS; Google Fonts.
- Nội dung: Header, Sidebar (PA‑1), Thẻ trạng thái, Điều khiển (Start/Stop/E‑Stop + modal Flowbite), Thông số nhanh, Đồ thị placeholder (SVG), Bảng nhật ký, Audit Trail, Footer. Responsive.

### 5) Bổ sung Audit & tài khoản
- Header: nút tài khoản (icon user) với dropdown: Hồ sơ, Audit Trail, Đăng xuất.
- Sidebar: liên kết mới "Audit Trail" dẫn tới bảng nhật ký hành động người dùng.

### 6) Cập nhật theo Phương án 2 (tiêu chuẩn sản xuất)
- Nút điều khiển: Start, Stop, E‑Stop (SW), Reset Fault, Dock, Undock, Pause/Resume, Mode Auto/Manual, Jog ± (chỉ Manual, có interlock).
- Chức năng mở rộng: chọn điểm Dock, giới hạn tốc độ theo vùng, cảnh báo bảo trì (runtime/nhiệt/dòng), export log CSV/JSON.
- HTML cập nhật: `.superdesign/design_iterations/oht50_overview_1.html` đã bổ sung các nút và modals (Reset, Dock, Undock), toggle Pause/Resume, toggle Auto/Manual + enable Jog.

### 7) FE-06 (Đồ thị v‑a‑x) — Layout & HTML
- Layout PA‑3 (Controls sticky + Chart lớn + khối dưới Stats/Info). Overlays: RFID, Encoder reset, State markers. Side Info: Heading, RFID gần nhất, x (encoder sau reset), mini‑map LiDAR (OP), connectivity.
- HTML: `.superdesign/design_iterations/fe06_vax_1.html` (Tailwind + Flowbite + Lucide, dark mode toggle, WS placeholder `ws=/api/v1/telemetry/ws`, export CSV/JSON, Reset Graph modal, responsive).
  - Bổ sung nút quay về "Tổng quan" ở header.

### 8) Trang mới
- User Settings: `.superdesign/design_iterations/user_settings_1.html` — Hồ sơ, vai trò & quyền, API tokens, phiên/thiết bị, danger zone.
  - Điều chỉnh theo PA‑1: giữ Hồ sơ + Đổi mật khẩu + Dark mode; riêng Admin có tab/nút "Quản trị" (ẩn với role khác).
- Admin Management: `.superdesign/design_iterations/admin_management_1.html` — Trang quản trị người dùng: tìm kiếm, lọc role/status, mời user, đổi role, khóa/mở khóa, vô hiệu hóa/kích hoạt, reset PW (xác nhận an toàn), dark mode.
 - Logs: `.superdesign/design_iterations/logs_1.html` — Lọc mức/nguồn/thời gian, tìm kiếm, export CSV/JSON.
 - Safety Config: `.superdesign/design_iterations/safety_config_1.html` — Zones, Validate/Apply (xác nhận), Rollback.
 - Diagnostics: `.superdesign/design_iterations/diagnostics_1.html` — Encoder/Driver/RS485 cards, developer console (guarded).
 - RS485 Addresses: `.superdesign/design_iterations/rs485_addresses_1.html` — Bảng ánh xạ địa chỉ theo `config_spec.md`.
- Audit Trail: `.superdesign/design_iterations/audit_trail_1.html` — Lọc user/action/time, bảng chi tiết, export CSV (placeholder).
- Center Config: `.superdesign/design_iterations/center_config_1.html` — Form tham số kết nối, trạng thái, nhật ký gần đây, test/save/apply (xác nhận).
- Job Monitor: `.superdesign/design_iterations/jobs_monitor_1.html` — Lọc, bảng job, drawer chi tiết, tạo job (modal).

### 3) Kế hoạch tiếp theo
- Bước 3 (Animation): xác nhận micro-animations cho message/log, controls, modal, sidebar (dự thảo sau khi PM duyệt theme).
- Bước 4 (HTML): tạo 1 file HTML tổng hợp, sử dụng TailwindCDN + Flowbite + theme ở trên, responsive.

### 4) Liên kết tài liệu liên quan
- Tham chiếu `docs/specs/interfaces.md`, `state_machine.md`, `center_comm.md`, `telemetry_schema.md` để đồng bộ trạng thái/đồ thị/nhật ký.
- PM theo dõi tại `docs/tasks/PM_TASKS_OHT-50.md` (Frontend FE-02/FE-04/FE-06/FE-07).


