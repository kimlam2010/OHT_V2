### Kế hoạch công việc chi tiết (PM Tasklist) — Dự án OHT-50

Phiên bản: v1.0
Phạm vi: Lập trình hệ thống OHT-50 gồm BACKEND, Frontend, FW (Firmware), EMBED (phần cứng nhúng). Bám theo tài liệu trong `docs/specs` và `docs/dev_radxa` của dự án hiện có.

Tài liệu tham chiếu chính:
- `docs/specs/architecture.md`, `module_map.md`, `interfaces.md`, `state_machine.md`
- `docs/specs/control_spec.md`, `driver_motor.md`, `encoder_feedback.md`
- `docs/specs/bus_rs485.md`, `center_comm.md`
- `docs/specs/config_spec.md`, `telemetry_schema.md`, `safety.md`, `safety_estop_interlock.md`, `safety_metrics.md`
- `docs/dev_radxa/*.md` (môi trường, HAL, OTA, logging/telemetry, UI local dashboard)

---

### Cập nhật tiến độ (Docs & Backend)

- Đã cập nhật `docs/specs/config_spec.md`: bổ sung `comm.rs485.addresses` theo `module_map.md`; thêm quy tắc validate địa chỉ duy nhất và hợp lệ.
- Đã cập nhật `docs/specs/telemetry_schema.md`: bổ sung các trường Location (`tag_id`, `rssi`) và `enc.count`; ghi chú phát theo sự kiện RFID.
- Đã tạo checklist cho PM: `docs/specs/PM_CHECKLIST_RS485_MODULES.md` (thu thập thông tin RS485/CAN theo từng module).

Lưu ý: Các nội dung đánh dấu TBD sẽ được chốt lại sau khi PM cung cấp dữ liệu điền vào checklist.

### Quyết định kiến trúc (đã chốt với Chủ đầu tư)

- RS485 sẽ do FW/EMBED xử lý hoàn toàn (real-time loop, CRC/retry/timeout, discovery, đọc điểm đo). Backend/Frontend không truy cập trực tiếp thiết bị serial trong môi trường sản xuất.
- Backend chỉ nhận dữ liệu/điều khiển thông qua FW/Center theo `docs/specs/center_comm.md` hoặc qua API cục bộ do FW cung cấp (nếu có).
- Tác động:
  - Loại bỏ nhu cầu BE chạy `pyserial`/truy cập `/dev/ttyOHT485` ở production.
  - BE tập trung quản lý cấu hình, giám sát, lưu trữ, hiển thị; FW chịu trách nhiệm giao tiếp bus.


### Cập nhật tiến độ (Frontend & SuperDesign)

- Đã hoàn tất Bước 1→4 cho các màn hình prototype (responsive + dark mode) theo quy trình SuperDesign:
  - Tổng quan: `.superdesign/design_iterations/oht50_overview_1.html` (sidebar đồng bộ tới các trang khác, điều khiển cơ bản, thông số nhanh, đồ thị placeholder, Audit Trail, cảnh báo bảo trì, liên kết User/Audit/Jobs/Center Config).
  - FE‑06 v‑a‑x: `.superdesign/design_iterations/fe06_vax_1.html` (Controls sticky, pause/zoom, export CSV/JSON, Reset Graph xác nhận, WS placeholder `ws=/api/v1/telemetry/ws`, Info Panel: Heading, RFID gần nhất, x encoder sau reset, mini‑map LiDAR (OP)).
  - User Settings (PA‑1 tối giản): `.superdesign/design_iterations/user_settings_1.html` (Hồ sơ, Đổi mật khẩu, Dark mode; Admin có nút/tab "Quản trị").
  - Center Config: `.superdesign/design_iterations/center_config_1.html` (Form tham số, Test/Save/Apply xác nhận, trạng thái/latency, log gần đây).
  - Jobs Monitor: `.superdesign/design_iterations/jobs_monitor_1.html` (lọc cơ bản, bảng job + tiến độ, drawer chi tiết, tạo job).
  - Admin Management: `.superdesign/design_iterations/admin_management_1.html` (phiên bản đơn giản: username + password; đặt lại mật khẩu, xóa tài khoản, tạo tài khoản mới).
- Đã cập nhật nhật ký thiết kế: `OHT-50/OHT-50/docs/dev_radxa/ui_local_dashboard_design_log.md` phản ánh layout/theme/animations và liên kết các trang.
- Điều hướng chéo giữa các trang đã đồng bộ (header/sidebar), hỗ trợ quay về Tổng quan.
- Kế hoạch kết nối realtime:
  - FE‑06 đã có WS placeholder; cần trỏ tới backend `/api/v1/telemetry/ws` khi môi trường sẵn sàng.
  - Các trang khác chờ schema/endpoint cụ thể để nối API (Config/Jobs/User).

Mục tiêu tổng quát:
- Hoàn thiện pipeline dữ liệu đo/điều khiển từ EMBED → FW → (Center/Backend) → Frontend, đáp ứng trạng thái Idle/Move/Dock/Fault/E‑Stop.
- Bảo đảm an toàn (E‑Stop, interlock), telemetry ổn định, cấu hình an toàn có kiểm soát.

Tiêu chí chấp nhận (DoD – Definition of Done):
- Kiểm thử đơn vị (unit), tích hợp (integration), HIL tối thiểu theo `docs/tests/hil_plan.md`.
- Tài liệu kèm theo: API, giao thức, biểu đồ trạng thái, hướng dẫn triển khai/OTA.
- Demo end‑to‑end: hiển thị trạng thái, điều khiển cơ bản, log, đồ thị v‑a‑x, cấu hình.

Mốc/lộ trình gợi ý (có thể điều chỉnh theo thực tế):
- Sprint 0 (1–2 tuần): Khởi tạo repo, CI/CD, chuẩn mã hóa, khung FW/HAL, mock API/telemetry.
- Sprint 1 (2–3 tuần): Luồng dữ liệu cơ bản, dashboard cơ bản, trạng thái Idle/Move, cấu hình tối thiểu.
- Sprint 2 (2–3 tuần): Dock, Fault, E‑Stop, an toàn & interlock, kiểm thử HIL, tối ưu thời gian thực.
- Sprint 3 (2–3 tuần): Hoàn thiện UI/UX, logging nâng cao, OTA, hardening & tài liệu bàn giao.

Quy ước quản lý công việc:
- Mỗi đầu việc đều có: Mục tiêu, Mô tả, Đầu vào/Phụ thuộc, Kết quả/DOD, Người phụ trách, Ước lượng.
- Dùng nhãn: `backend`, `frontend`, `fw`, `embed`, `safety`, `telemetry`, `config`, `docs`, `test`.

---

### BACKEND

Mục tiêu: Cung cấp dịch vụ Center/Server cho cấu hình, lưu trữ telemetry, logging, chuẩn hóa giao tiếp theo `center_comm.md`, `telemetry_schema.md`, hỗ trợ UI truy xuất.

1) Khởi tạo & hạ tầng
- Mục tiêu: Sẵn sàng phát triển, triển khai nội bộ.
- Nhiệm vụ:
  - Khởi tạo dịch vụ (ưu tiên Python FastAPI hoặc Node.js NestJS theo năng lực team).
  - Thiết lập cấu hình môi trường (ENV), logging chuẩn, cấu hình bảo mật cơ bản.
  - Thiết lập CI/CD (build, test, lint, container image).
  - Bộ công cụ quan sát cơ bản (health endpoint, metrics).
- DOD: Dịch vụ chạy được cục bộ + container, có healthcheck, CI chạy xanh.

2) API cấu hình (theo `config_spec.md`)
- Mục tiêu: Quản lý cấu hình hệ thống (profile, tham số an toàn, mapping thiết bị).
- Nhiệm vụ:
  - Thiết kế schema cấu hình (phiên bản hóa), CRUD cấu hình, áp dụng/khôi phục.
  - Endpoint xuất/nhập cấu hình (JSON/YAML), validate theo schema.
  - Kiểm soát quyền cơ bản (role: operator/maintainer).
- DOD: Bộ API tài liệu hóa, kiểm thử đơn vị + tích hợp, validate chặt chẽ.

3) Telemetry & logging (`telemetry_schema.md`, `08_logging_telemetry.md`)
- Mục tiêu: Thu thập, lưu trữ, truy vấn.
- Nhiệm vụ:
  - Ingest telemetry (HTTP/WebSocket/MQTT tùy chọn), lưu kho thời gian (TSDB hoặc quan hệ).
  - Truy vấn theo thời gian, theo thiết bị, theo trạng thái.
  - Lưu log sự kiện/lỗi, phân cấp mức độ.
- DOD: API truy vấn hoạt động, hiển thị được trên Frontend, benchmark tối thiểu.

4) Giao tiếp Center (`center_comm.md`)
- Mục tiêu: Cầu nối giữa Frontend và thiết bị (qua FW/EMBED hoặc gateway).
- Nhiệm vụ:
  - Định nghĩa thông điệp/command (Move/Dock/Stop…), trạng thái phản hồi.
  - Cơ chế subscribe/publish (WebSocket/SSE) cho trạng thái theo thời gian thực.
  - Hỗ trợ mô phỏng (mock device) để phát triển song song.
- DOD: Gửi/nhận lệnh end‑to‑end với mock; tài liệu hóa thông điệp.

5) OTA & quản trị (`10_deployment_ota.md`, `15_ota_signing_integrity.md`)
- Mục tiêu: Phát hành FW/EMBED an toàn.
- Nhiệm vụ:
  - Kho bản phát hành, checksum/ký số, lịch sử triển khai.
  - API kích hoạt cập nhật, theo dõi tiến độ, rollback.
- DOD: Triển khai giả lập hoàn chỉnh, audit log.

6) Bảo mật & an toàn
- Mục tiêu: Bảo vệ API và dữ liệu, hỗ trợ quy trình an toàn vận hành.
- Nhiệm vụ:
  - CORS, rate limit, auth cơ bản (JWT/OAuth nội bộ), RBAC tối thiểu.
  - Ghi nhận/cảnh báo sự kiện an toàn (E‑Stop, Fault) từ thiết bị.
- DOD: Kiểm thử bảo mật cơ bản, log/alert hoạt động.

---

### Frontend (UI Local Dashboard — tham chiếu `11_ui_local_dashboard.md`)

Mục tiêu: Dashboard chẩn đoán/giám sát/điều khiển cục bộ, trạng thái Idle/Move/Dock/Fault/E‑Stop, đồ thị v‑a‑x, nhật ký, cấu hình an toàn.

1) Khởi tạo & khung SPA
- Mục tiêu: Khung dự án sẵn sàng phát triển.
- Nhiệm vụ:
  - Khởi tạo SPA (React + Vite hoặc tương đương), cấu hình router, state management.
  - Thiết lập theme, responsive, dark mode, icon (Lucide), biểu đồ (placeholder trước).
  - Proxy phát triển tới Backend.
- DOD: Trang khung chạy, CI build, lint pass.

2) Màn hình Tổng quan (Overview)
- Nhiệm vụ:
  - Thẻ trạng thái hệ thống: Idle/Move/Dock/Fault/E‑Stop, chỉ báo an toàn.
  - Nút điều khiển cơ bản: Start/Stop/E‑Stop (giả lập trước), xác nhận thao tác nguy hiểm.
  - Khu vực thông số nhanh: v, a, x; kết nối; pin/nguồn (nếu có).
- DOD: Cập nhật thời gian thực từ mock/socket; test UI cơ bản.

3) Màn hình Đồ thị v‑a‑x
- Nhiệm vụ:
  - Biểu đồ thời gian thực (v, a, x) với khả năng pause/zoom.
  - Lọc theo khoảng thời gian, tải lại dữ liệu lịch sử từ Backend.
- DOD: Biểu đồ hoạt động mượt, kiểm thử trên màn hình nhỏ/lớn.

4) Màn hình Nhật ký (Log)
- Nhiệm vụ:
  - Bảng log sự kiện/lỗi, lọc theo mức/nguồn/thời gian, tìm kiếm.
  - Xuất CSV/JSON cơ bản.
- DOD: Bảng tương tác mượt, phân trang, kiểm thử hiệu năng.

5) Màn hình Cấu hình an toàn
- Nhiệm vụ:
  - Form CRUD cấu hình theo `config_spec.md`, validate client.
  - Tải/lưu/áp dụng cấu hình, xem lịch sử phiên bản.
- DOD: Đồng bộ với Backend, xác nhận trước khi áp dụng, hiển thị sai lệch/khác biệt.

6) Màn hình Chẩn đoán/Nâng cao
- Nhiệm vụ:
  - Trang diagnostics: tín hiệu encoder, lỗi driver, nhiệt độ, bus RS485.
  - Công cụ gửi lệnh trực tiếp (developer console) có cảnh báo.
- DOD: Bảo vệ thao tác bằng cảnh báo/role, log lại hoạt động.

7) Chất lượng & bàn giao
- Nhiệm vụ: Kiểm thử E2E tối thiểu, accessibility cơ bản, tài liệu hướng dẫn sử dụng nhanh.
- DOD: Demo end‑to‑end với mock + kết nối Backend thật.

---

### FW (Firmware)

Mục tiêu: Điều khiển thời gian thực, vòng lặp điều khiển, đọc encoder, điều khiển driver motor, quản lý trạng thái theo `state_machine.md`, bảo vệ an toàn.

1) Khung FW & HAL (`04_hal_interfaces.md`, `06_control_loop_state_machine.md`)
- Nhiệm vụ:
  - Dựng khung FW: vòng lặp chính, lịch tác vụ (scheduler), ưu tiên ngắt.
  - Lớp HAL: GPIO, PWM, ADC, UART/RS485, CAN (nếu dùng), timer.
- DOD: Chạy được trên board dev, đo latency cơ bản.

2) Trạng thái & điều khiển (`state_machine.md`, `control_spec.md`)
- Nhiệm vụ:
  - Máy trạng thái: Idle → Move → Dock; Fault & E‑Stop override.
  - Vòng điều khiển: cập nhật tốc độ, gia tốc, vị trí; giới hạn/saturation; anti‑windup.
- DOD: Unit test logic trạng thái, thông số điều khiển ổn định trên mô phỏng.

3) Encoder & phản hồi (`encoder_feedback.md`)
- Nhiệm vụ:
  - Trình điều khiển/đọc encoder, lọc tín hiệu, hiệu chuẩn.
  - Hợp nhất vị trí/tốc độ (location fusion cơ bản nếu cần).
- DOD: Độ chính xác trong giới hạn cho phép, log/telemetry đầy đủ.

4) Driver motor (`driver_motor.md`)
- Nhiệm vụ:
  - Giao tiếp driver (PWM/UART/CAN), bảo vệ quá dòng/nhiệt.
  - Ramping tốc độ/gia tốc, jerk limit nếu cần.
- DOD: Test tải giả, đáp ứng mượt, bảo vệ hoạt động.

5) Giao tiếp & Telemetry (`bus_rs485.md`, `center_comm.md`, `08_logging_telemetry.md`)
- Nhiệm vụ:
  - Giao thức RS485 (frame, CRC, retry), tối ưu thời gian thực.
  - Gói telemetry theo `telemetry_schema.md`, giảm tần số khi quá tải.
- DOD: Thông lượng ổn định, tỉ lệ lỗi khung thấp, log thống kê.

6) An toàn & interlock (`safety.md`, `safety_estop_interlock.md`, `safety_metrics.md`)
- Nhiệm vụ:
  - Xử lý E‑Stop phần mềm/hardware, time‑guard/ watchdog.
  - Interlock: vùng cấm, giới hạn tốc độ theo vùng (nếu áp dụng), safe defaults.
- DOD: Bài test Fault injection, chứng cứ log đáp ứng đúng thời hạn.

7) OTA/Bootloader (phối hợp EMBED, `10_deployment_ota.md`, `15_ota_signing_integrity.md`)
- Nhiệm vụ: Tích hợp cơ chế update, xác thực bản FW, rollback an toàn.
- DOD: Demo cập nhật giả lập + kiểm chứng checksum/ký số.

---

### EMBED (Phần cứng nhúng)

Mục tiêu: Bring‑up phần cứng, driver tầng thấp, đảm bảo nền tảng ổn định cho FW.

1) Bring‑up phần cứng (`13_os_image_and_drivers.md`, `hardware.md`)
- Nhiệm vụ:
  - Kiểm tra nguồn, xung clock, chân IO; tài liệu pinout (`pinout_radxa.md`).
  - Khởi tạo hệ điều hành/RTOS/SDK tùy nền tảng; thiết lập công cụ debug.
- DOD: Board khởi động ổn định, debug SWD/JTAG/serial hoạt động.

2) Driver tầng thấp & bus (`03_rs485_bus_driver.md`, `14_rs485_can_transceiver.md`)
- Nhiệm vụ:
  - RS485/CAN transceiver, điều khiển DE/RE, chống nhiễu EMI cơ bản.
  - UART/CAN init, DMA (nếu có), buffer vòng, xử lý lỗi frame.
- DOD: Loopback test, throughput/BER trong ngưỡng.

3) Cảm biến/encoder & IO
- Nhiệm vụ: Driver đọc encoder, công tắc giới hạn, E‑Stop input, ADC cảm biến.
- DOD: Sai số đo trong ngưỡng, debounce/ lọc phù hợp.

4) Bootloader & bảo mật (`15_ota_signing_integrity.md`)
- Nhiệm vụ: Bootloader, layout bộ nhớ, secure boot tối thiểu, vùng lưu FW dự phòng.
- DOD: Test cập nhật/rollback, kiểm chứng chữ ký.

5) Kiểm thử HIL (`09_testing_sim_hil.md`)
- Nhiệm vụ: Thiết lập bench HIL, fixture tín hiệu encoder/motor ảo, thiết bị đo.
- DOD: Bộ test HIL chạy tự động/tái lập được, báo cáo kết quả.

---

### Giao diện & phụ thuộc giữa các nhóm

- Thống nhất payload giao tiếp giữa FW ↔ Backend (qua Center/Gateway) theo `center_comm.md` và `telemetry_schema.md`.
- Chuẩn hóa phiên bản cấu hình và quy trình áp dụng giữa Frontend ↔ Backend ↔ FW theo `config_spec.md`.
- Lịch đồng bộ: họp kỹ thuật liên nhóm 2 lần/tuần; cập nhật thay đổi giao thức qua PRD/API doc.

---

### Quản lý chất lượng & rủi ro

- Chỉ số: tỷ lệ lỗi khung RS485, độ trễ vòng lặp điều khiển, thời gian phản hồi E‑Stop, uptime dịch vụ Backend, FPS UI khi hiển thị biểu đồ.
- Rủi ro chính: nhiễu bus, sai lệch encoder, quá tải telemetry, cập nhật OTA thất bại.
- Biện pháp: buffer/CRC/retry, lọc tín hiệu, backpressure/giảm tần số, chiến lược rollback.

---

### Bàn giao

- Demo end‑to‑end các kịch bản: Idle → Move → Dock; Fault; E‑Stop.
- Tài liệu người dùng: hướng dẫn cấu hình, quy trình cập nhật, xử lý sự cố.
- Tài liệu kỹ thuật: API, giao thức, sơ đồ trạng thái, thông số điều khiển.

---

### Yêu cầu khách hàng – RS485 Modules (cần PM cung cấp)

Vui lòng điền đầy đủ vào biểu mẫu: `docs/specs/PM_CHECKLIST_RS485_MODULES.md`. Tóm tắt các nhóm thông tin cần có:

1) Chung cho hệ thống bus
- Topology & chiều dài bus; Cáp & đầu nối; Vị trí termination 120Ω; Bias (giá trị, vị trí)
- Tham số UART RS485: Baud, Parity/Stop bits
- Heartbeat/Keepalive (chu kỳ), Timeout (ms), Retry (số lần)

2) Ánh xạ địa chỉ
- Safety (0x10 đề xuất); DIO Upper (0x11); AIO Upper (0x12)
- DC Driver #1 axis_drive (0x13) / Stepper #1 (0x14)
- DC Driver #2 axis_lift (0x15) / Stepper #2 (0x16)
- DIO Cargo (0x17); AIO Cargo (0x18); DC Driver #3 axis_door (0x19)
- Location RFID+Encoder (0x1A); Khác (liệt kê thêm)

3) DC Motor Driver / Stepper Driver
- Hãng/Model; Giao thức (Modbus RTU/CANopen/khác)
- Tham số setpoint (velocity/position/torque – đơn vị, giới hạn)
- Feedback (pos/vel/current/temp/fault); Encoder tích hợp (loại, ppr)
- Tần số cập nhật mong muốn (setpoint/feedback); Điều kiện an toàn & interlock

4) Location Module (RFID + Encoder)
- RFID: chuẩn tag, tần số, khoảng cách đọc, thời gian đáp ứng; có `rssi`?
- Encoder: loại, ppr, có kênh Z; Timestamp (CPU hay tại module)
- Yêu cầu Telemetry: `tag_id`, `enc.count`, `s_on_rail` (nếu cần)

5) Safety Module
- Kênh vào/ra an toàn (số lượng, loại); Latch/Reset; Thời gian đáp ứng

6) DIO/AIO Modules
- DIO: số kênh In/Out, loại NPN/PNP, debounce, polarity
- AIO: dải 0–10V hoặc 4–20mA, độ phân giải, tần số mẫu; Hiệu chuẩn offset/scale

7) LiDAR (nếu có)
- Hãng/Model; Giao tiếp (RS485/UART/CAN/Ethernet)
- Tốc độ quét, FOV, phạm vi đo; Định dạng dữ liệu; Tần số stream/giảm tần mong muốn

8) Telemetry & Logging
- Tần số WS (Hz); Trường telemetry bổ sung; Sự kiện quan trọng cần log

9) Khác
- Yêu cầu EMC/ESD/nguồn; Firmware version các module; Tài liệu/datasheet

Hành động PM:
- Phân công người liên hệ nhà cung cấp từng module, thu thập dữ liệu và điền biểu mẫu.
- Hạn đề xuất: điền đủ 80% thông tin vào `PM_CHECKLIST_RS485_MODULES.md` trong 5 ngày làm việc.
- Sau khi có dữ liệu: cập nhật `config_spec.md` (addresses thực tế) và mở PR xác nhận schema Telemetry cuối.

Ghi chú: PM sẽ theo dõi tiến độ theo Sprint; mọi thay đổi giao diện/giao thức phải cập nhật tài liệu tương ứng trong `docs/specs` và ghi rõ phiên bản.

---

### Bảng công việc chi tiết theo từng vị trí (Tasks Matrix)

Lưu ý:
- Cột "Người phụ trách" để trống để PM phân công/điền tên; "Ước lượng" có thể điều chỉnh sau khi grooming.
- Tất cả DOD phải có bằng chứng: code, test, tài liệu/README hoặc tài liệu API.

#### Backend

| ID | Vị trí phụ trách | Mô tả công việc | Kết quả mong muốn (DOD) | Phụ thuộc | Ước lượng (ngày) | Người phụ trách |
|---|---|---|---|---|---|---|
| BE-01 | Backend Lead, DevOps | Khởi tạo service (FastAPI/NestJS), cấu trúc repo, cấu hình ENV | Service chạy cục bộ + container; healthcheck; README khởi chạy | — | 3 | ✅ Hoàn thành |
| BE-02 | DevOps | Thiết lập CI/CD (build, test, lint, image) | Pipeline xanh, artifact image tạo thành công | BE-01 | 2 | ✅ Hoàn thành |
| BE-03 | Backend Engineer | API cấu hình: schema + CRUD theo `config_spec.md` | Endpoint CRUD hoạt động; validate schema; test tích hợp | BE-01 | 4 | 🔄 Đang làm (CRUD cơ bản) |
| BE-04 | Backend Engineer | Phiên bản hóa cấu hình, import/export JSON/YAML | Lưu/khôi phục phiên bản; tài liệu API | BE-03 | 2 | |
| BE-05 | Backend Engineer | Ingest telemetry (HTTP/WS/MQTT) theo `telemetry_schema.md` | Nhận/ghi dữ liệu ổn định; benchmark tối thiểu | BE-01 | 4 | 🔄 Đang làm (WS current/history) |
| BE-06 | Backend Engineer | API truy vấn telemetry theo thời gian/kênh | Truy vấn động; phân trang; test hiệu năng cơ bản | BE-05 | 3 | |
| BE-07 | Backend Lead | Giao tiếp Center: định nghĩa command + WS/SSE | Gửi/nhận lệnh với mock; tài liệu thông điệp | BE-01 | 3 | |
| BE-08 | Backend Engineer | Mock device để dev song song FE/FW | Mock sinh trạng thái + telemetry; script chạy nhanh | BE-07 | 2 | 🔄 Đang làm (telemetry mock) |
| BE-09 | Backend Lead, Security | Bảo mật: auth cơ bản, RBAC, rate limit, CORS | Bảo vệ endpoint; test bảo mật cơ bản | BE-01 | 3 | |
| BE-10 | Backend Engineer | Logging/observability/metrics | Log cấu trúc; metrics /health; dashboard cơ bản | BE-01 | 2 | |
| BE-11 | Backend Lead | OTA backend: kho bản phát hành, ký số, checksum | API upload/phát hành; lưu metadata; kiểm tra checksum | BE-01 | 4 | |
| BE-12 | Backend Engineer | API kích hoạt OTA + theo dõi tiến độ, rollback | Job theo dõi; trạng thái; rollback an toàn (mô phỏng) | BE-11 | 3 | |
| BE-13 | Backend Engineer | Mở rộng Telemetry API: `/current` JSON chuẩn, `/history?limit=`, WS theo `ws_hz`, `/rate`, `/reset`, `/stats` | API hoạt động; docs cập nhật; đồng bộ `telemetry_schema.md` | BE-05 | 3 | ✅ Hoàn thành |
| BE-14 | Backend Engineer | Refactor DI `ConfigService` qua `Depends(get_config_service)` | Dễ mock/tiêm phụ thuộc; test đơn vị đơn giản hơn | BE-03 | 2 | |
| BE-15 | Backend Engineer | Đồng bộ schema Telemetry: thêm `tag_id`, `enc.count` vào payload | `/current`, `/ws` phát đủ trường; tài liệu hóa | BE-05 | 2 | 🔄 Đang thực hiện |
| BE-16 | Backend Engineer | Tích hợp với FW/Center thay vì RS485 trực tiếp | Backend lấy module registry/points qua FW/Center API; cập nhật docs | FW-11 | 3 | |
| BE-17 | Backend Engineer | Loại bỏ phụ thuộc serial ở BE (sản xuất) + Feature flag cho chế độ mock | BE không mở cổng serial; chỉ dùng mock ở dev; tài liệu hoá cấu hình | ARCH | 1 | |

#### Frontend

| ID | Vị trí phụ trách | Mô tả công việc | Kết quả mong muốn (DOD) | Phụ thuộc | Ước lượng (ngày) | Người phụ trách |
|---|---|---|---|---|---|---|
| FE-01 | Frontend Lead | Khởi tạo SPA (React+Vite), router, state mgmt | App chạy; router hoạt động; lint/CI pass | — | 3 | ✅ Hoàn thành |
| FE-02 | Frontend Engineer | Thiết lập theme, dark mode, icon, responsive | Style cơ bản; kiểm tra trên mobile/desktop | FE-01 | 2 | ✅ Hoàn thành |
| FE-03 | Frontend Engineer | Proxy dev tới Backend; cấu hình env | Gọi API qua proxy; docs cách chạy | FE-01 | 1 | ✅ Hoàn thành |
| FE-04 | Frontend Engineer | Màn hình Overview: thẻ trạng thái Idle/Move/Dock/Fault/E‑Stop | Hiển thị realtime từ mock/socket; test UI | BE-08 | 3 | ✅ Hoàn thành |
| FE-05 | Frontend Engineer | Nút điều khiển Start/Stop/E‑Stop (xác nhận an toàn) | Gửi command tới mock/backend; modal xác nhận | BE-07, BE-08 | 2 | ✅ Hoàn thành |
| FE-06 | Frontend Engineer | Màn hình đồ thị v‑a‑x realtime (pause/zoom) | Biểu đồ mượt; thay đổi thang thời gian | BE-06 | 3 | ✅ Prototype (HTML) |
| FE-07 | Frontend Engineer | Màn hình Log: bảng, lọc, tìm kiếm, phân trang | Bảng ổn định; export CSV/JSON | BE-06 | 3 | ✅ Hoàn thành |
| FE-08 | Frontend Engineer | Màn hình Cấu hình an toàn (CRUD + validate) | Đồng bộ backend; cảnh báo khi áp dụng | BE-03, BE-04 | 3 | ✅ Hoàn thành |
| FE-09 | Frontend Engineer | Màn hình Diagnostics: encoder/driver/bus | Hiển thị tín hiệu cơ bản; cảnh báo lỗi | BE-06 | 3 | ✅ Hoàn thành |
| FE-10 | Frontend Lead | E2E smoke test + hướng dẫn sử dụng nhanh | Test chạy qua các flow chính; README UI | FE-04..FE-09 | 2 | ✅ Hoàn thành |
| FE-11 | Frontend Engineer | Cập nhật Overview/Diagnostics hiển thị `tag_id`, `enc.count`, tốc độ phát | UI hiển thị đúng trường mới; tương thích màn hình nhỏ/lớn | BE-15 | 2 | ✅ Hoàn thành |
| FE-12 | Frontend Engineer | Trang cấu hình RS485 addresses theo `config_spec.md` | Form đọc/ghi địa chỉ module; validate client | BE-03 | 3 | ✅ Hoàn thành |
| FE-13 | Frontend Engineer | User Settings (PA‑1): hồ sơ, mật khẩu, dark mode | Prototype HTML hoàn chỉnh | — | 1 | ✅ Hoàn thành |
| FE-14 | Frontend Engineer | Center Config page | Prototype HTML hoàn chỉnh | — | 1 | ✅ Hoàn thành |
| FE-15 | Frontend Engineer | Jobs Monitor page | Prototype HTML hoàn chỉnh | — | 1 | ✅ Hoàn thành |
| FE-16 | Frontend Engineer | Admin Management (đơn giản) | Prototype HTML hoàn chỉnh | — | 1 | ✅ Hoàn thành |
| FE-17 | Frontend Engineer | Logs page | Prototype HTML hoàn chỉnh | — | 1 | ✅ Hoàn thành |
| FE-18 | Frontend Engineer | Safety Config page | Prototype HTML hoàn chỉnh | — | 1 | ✅ Hoàn thành |
| FE-19 | Frontend Engineer | Diagnostics page | Prototype HTML hoàn chỉnh | — | 1 | ✅ Hoàn thành |
| FE-20 | Frontend Engineer | RS485 Addresses page | Prototype HTML hoàn chỉnh | BE-03 | 1 | ✅ Hoàn thành |
| FE-21 | Frontend Engineer | Audit Trail page | Prototype HTML hoàn chỉnh | — | 1 | ✅ Hoàn thành |
| FE-22 | Frontend Lead | Khởi tạo thư mục FE (React+Vite) + demo WS | `frontend/` chạy dev; route demo /fe06 kết nối WS | BE-05 | 1 | ✅ Hoàn thành |
| FE-23 | Frontend Engineer | Login + Guard + Logout + Dashboard khởi đầu | Đăng nhập mock, chặn /dashboard khi chưa login, hiển thị user | FE-01 | 1 | ✅ Hoàn thành |
| FE-24 | Frontend Engineer | Tích hợp FE‑06 WS với auto-reconnect | Chart cập nhật realtime, backoff khi mất kết nối | BE-05 | 1 | ✅ Hoàn thành |
| FE-25 | Frontend Engineer | Config CRUD tối thiểu trên Dashboard | Đọc/ghi /api/v1/config/ | BE-03 | 1 | ✅ Hoàn thành |

| FE-13 | Frontend Engineer | User Settings (PA‑1): hồ sơ, mật khẩu, dark mode | Prototype HTML hoàn chỉnh | — | 1 | ✅ Hoàn thành |
| FE-14 | Frontend Engineer | Center Config page | Prototype HTML hoàn chỉnh | — | 1 | ✅ Hoàn thành |
| FE-15 | Frontend Engineer | Jobs Monitor page | Prototype HTML hoàn chỉnh | — | 1 | ✅ Hoàn thành |
| FE-16 | Frontend Engineer | Admin Management (đơn giản) | Prototype HTML hoàn chỉnh | — | 1 | ✅ Hoàn thành |

#### FW (Firmware)

| ID | Vị trí phụ trách | Mô tả công việc | Kết quả mong muốn (DOD) | Phụ thuộc | Ước lượng (ngày) | Người phụ trách |
|---|---|---|---|---|---|---|
| FW-01 | FW Lead | Khung FW, scheduler, ưu tiên ngắt | Vòng lặp chính ổn định; đo latency cơ bản | EM-01 | 3 | |
| FW-02 | FW Engineer (HAL) | HAL: GPIO, PWM, ADC, UART/RS485, timer | API HAL ổn định; unit test tối thiểu | EM-02 | 4 | |
| FW-03 | FW Engineer (Control) | Máy trạng thái Idle/Move/Dock; Fault/E‑Stop override | Unit test logic trạng thái đạt yêu cầu | FW-01 | 3 | |
| FW-04 | FW Engineer (Control) | Vòng điều khiển: v/a/x, giới hạn, anti‑windup | Đáp ứng ổn định trên mô phỏng | FW-03 | 4 | |
| FW-05 | FW Engineer (Sensors) | Encoder: driver + lọc tín hiệu + hiệu chuẩn | Sai số trong ngưỡng; log thống kê | EM-03 | 3 | |
| FW-06 | FW Engineer (Driver) | Driver motor: PWM/UART/CAN + bảo vệ | Ramping mượt; bảo vệ quá dòng/nhiệt | FW-02 | 4 | |
| FW-07 | FW Engineer (Comms) | RS485 protocol: frame, CRC, retry | Tỉ lệ lỗi thấp; thống kê BER | EM-02 | 3 | |
| FW-08 | FW Engineer (Comms) | Telemetry packaging theo `telemetry_schema.md` | Tốc độ gửi ổn định; backpressure khi quá tải | FW-07 | 3 | |
| FW-09 | FW Engineer (Safety) | E‑Stop, watchdog, interlock cơ bản | Bài test fault injection đạt; log sự kiện | FW-03 | 3 | |
| FW-10 | FW Lead | Bootloader/OTA tích hợp | Cập nhật/rollback mô phỏng; kiểm chứng checksum | EM-04, BE-11 | 4 | |
| FW-11 | FW Engineer (Comms) | Triển khai khung lệnh RS485 theo `bus_rs485.md` (PING/GET_INFO/READ_FB/SET_POS...) | Gửi/nhận ổn định; CRC/timeout/retry đạt; thống kê lỗi | EM-02 | 5 | |
| FW-12 | FW Engineer (Sensors) | Location fusion cơ bản (RFID + encoder) → `s_on_rail` | Sai số trong ngưỡng; log/telemetry có `tag_id`, `enc.count` | FW-05 | 4 | |
| FW-13 | FW Engineer (Comms) | Expose API/Center messages cho BE: module registry, points snapshot | BE truy vấn qua HTTP/WS hoặc Center; tài liệu thông điệp | FW-07 | 3 | |

| FW-15 | FW Engineer (Comms) | Cấu hình thiết bị RS485 qua env/config: ưu tiên `/dev/ttyOHT485` (udev), fallback `/dev/ttyS1` | Service khởi chạy ổn định; log cảnh báo khi fallback; README hướng dẫn | DOC-04 | 1 | Người A | 2025-08-20 |

#### EMBED (Nhúng/Phần cứng)

| ID | Vị trí phụ trách | Mô tả công việc | Kết quả mong muốn (DOD) | Phụ thuộc | Ước lượng (ngày) | Người phụ trách |
|---|---|---|---|---|---|---|
| EM-01 | Embedded HW | Bring‑up: nguồn, clock, IO; pinout | Board ổn định; tài liệu pinout cập nhật | — | 3 | ✅ Hoàn thành |
| EM-02 | Embedded Driver | RS485/CAN transceiver; DE/RE; EMI cơ bản | Loopback OK; timing đo được | EM-01 | 3 | ✅ Hoàn thành |
| EM-03 | Embedded Driver | UART/CAN init + DMA ring buffer | Buffer ổn định; không mất khung ở baud mục tiêu | EM-02 | 3 | ✅ Hoàn thành |
| EM-04 | Embedded Security | Bootloader layout, secure boot tối thiểu | Sơ đồ bộ nhớ; kiểm chứng chữ ký | EM-01 | 4 | ✅ Hoàn thành |
| EM-05 | Embedded IO | Cảm biến/encoder, limit switch, E‑Stop input | Sai số/độ trễ trong ngưỡng; debounce hợp lý | EM-01 | 3 | ✅ Hoàn thành |
| EM-06 | Embedded HW | EMI/ESD guideline & kiểm tra cơ bản | Tài liệu guideline; báo cáo test | EM-01 | 2 | ✅ Hoàn thành |
| EM-07 | Embedded QA | Bench HIL: fixture encoder/motor ảo | Bench hoạt động; script điều khiển | EM-02, EM-03 | 3 | ✅ Hoàn thành |
| EM-08 | Embedded QA | Checklist test sản xuất cơ bản | Checklist versioned; có mẫu biểu ghi nhận | EM-06 | 2 | ✅ Hoàn thành |
| EM-09 | Embedded HW | Xác nhận wiring RS485 (termination/bias), udev rules `/dev/ttyOHT485` | Ảnh chụp/biên bản kiểm tra; rules áp dụng ổn định | EM-01 | 2 | ✅ Hoàn thành |

| EM-11 | Embedded Driver | Enable UART1 trong DT/overlay; xác nhận `/dev/ttyS1` hoạt động | Loopback OK; thông số stty chuẩn; ảnh log `dmesg` | EM-01 | 1 | ✅ Hoàn thành | 2025-08-18 |

#### QA/HIL & Vận hành

| ID | Vị trí phụ trách | Mô tả công việc | Kết quả mong muốn (DOD) | Phụ thuộc | Ước lượng (ngày) | Người phụ trách |
|---|---|---|---|---|---|---|
| QA-01 | QA/HIL | Hoàn thiện `docs/tests/hil_plan.md` + script | Chạy tự động; báo cáo HTML/CSV | EM-07, FW-08 | 3 | |
| QA-02 | QA/HIL | Fault injection (mất gói, lỗi encoder, quá tải) | Bảng tiêu chí vượt/không vượt; log minh chứng | FW-09, BE-10 | 3 | |
| QA-03 | QA/HIL | Đo chỉ số: độ trễ control loop, E‑Stop, FPS UI | Báo cáo so sánh sprint; ngưỡng cảnh báo | FE-06, FW-04 | 3 | |
| QA-04 | QA/HIL | Kịch bản lỗi RS485: mất gói/CRC/timeout, retry/backoff | Bảng tiêu chí vượt/không vượt; log minh chứng; tỷ lệ lỗi | FW-07, BE-05 | 3 | |

#### Docs

| ID | Vị trí phụ trách | Mô tả công việc | Kết quả mong muốn (DOD) | Phụ thuộc | Ước lượng (ngày) | Người phụ trách |
|---|---|---|---|---|---|---|
| DOC-01 | PM/Tech Writer | Bổ sung `comm.rs485.addresses`, cập nhật checklist PM | `config_spec.md` & `PM_CHECKLIST_RS485_MODULES.md` cập nhật | — | 1 | ✅ Hoàn thành |
| DOC-02 | PM/Tech Writer | Viết đặc tả LiDAR (nếu dùng) | `module_spec.md` có mục LiDAR; tham chiếu wiring/giao tiếp | ARCH | 2 | |
| DOC-03 | PM/Tech Writer | Cập nhật interfaces: RS485 thuộc FW; BE tích hợp qua FW/Center | `docs/specs/interfaces.md` cập nhật lưu đồ & vai trò | ARCH | 1 | |
| DOC-04 | PM/Tech Writer | Tạo `docs/dev_radxa/platform_orangepi_5b.md` + `docs/dev_radxa/udev_rules_orangepi5b.md` | Tài liệu có hướng dẫn UART1, udev; liên kết từ `hardware.md` | ARCH | 1 | ✅ Hoàn thành |

#### Ghi chú chuyển đổi
- Các endpoint RS485 trong BE (nếu có trong nhánh dev) chỉ dùng cho mock/dev; không dùng trong môi trường sản xuất theo quyết định mới.
- Khi FW sẵn sàng API/Center message, BE chuyển sang gọi FW/Center tương ứng và gỡ các codepath truy cập serial.
| OPS-01 | DevOps | Môi trường dev/staging; quan sát; backup | Mô tả hạ tầng; playbook sự cố | BE-02, BE-10 | 3 | |

---

### Ưu tiên triển khai & Tiến độ (Priority, Roadmap, Tracker)

Mục tiêu của phần này là chỉ rõ: làm phần nào trước, mốc bàn giao theo sprint, và cách theo dõi tiến độ minh bạch hằng tuần.

#### 1) Critical Path — Thứ tự thực hiện bắt buộc

| Thứ tự | ID | Công việc | Lý do ưu tiên | Phụ thuộc | Kết quả mong muốn |
|---|---|---|---|---|---|
| 1 | EM-01 | Bring‑up phần cứng, pinout | Nền tảng cho FW/HAL | — | Board ổn định, tài liệu pinout |
| 2 | BE-01 | Khởi tạo Backend service | Nền tảng API/telemetry cho FE | — | Service chạy + healthcheck |
| 3 | FE-01 | Khởi tạo SPA + router | Nền tảng UI, tích hợp sớm | — | App skeleton chạy, CI pass |
| 4 | EM-02 | RS485/CAN transceiver | Giao tiếp nền cho FW | EM-01 | Loopback OK |
| 5 | FW-01 | Khung FW + scheduler | Khởi tạo vòng lặp điều khiển | EM-01 | Vòng lặp ổn định |
| 6 | BE-08 | Mock device | Cho FE/BE tích hợp sớm | BE-01 | Mock realtime hoạt động |
| 7 | BE-03 | API cấu hình (CRUD) | FE cấu hình an toàn cần | BE-01 | CRUD + validate schema |
| 8 | BE-05 | Ingest telemetry | Nguồn dữ liệu cho FE đồ thị | BE-01 | Ingest ổn định |
| 9 | FE-04 | Overview + trạng thái hệ | Hiển thị realtime sớm | FE-01, BE-08 | Thẻ trạng thái hoạt động |
| 10 | FW-03 | State machine | Điều khiển an toàn/đúng flow | FW-01 | Unit test đạt |
| 11 | FW-07 | RS485 protocol | Liên lạc với Center | EM-02 | CRC/retry ổn định |
| 12 | FE-06 | Đồ thị v‑a‑x realtime | Quan sát hệ thống | BE-06 | Biểu đồ mượt |
| 13 | FW-09 | E‑Stop & interlock | An toàn ưu tiên | FW-03 | Fault injection đạt |
| 14 | BE-11 | OTA backend | Chuẩn bị phát hành | BE-01 | Kho bản phát hành |
| 15 | FW-10 | Bootloader/OTA | Cập nhật/rollback an toàn | EM-04, BE-11 | OTA mô phỏng OK |

Lưu ý: Các hạng mục khác có thể triển khai song song nếu không nằm trên critical path và không phá vỡ phụ thuộc.

#### 2) Roadmap theo Sprint (mốc & tiêu chí)

| Sprint | Mục tiêu chính | Hạng mục trọng tâm | Tiêu chí bàn giao (Exit) | Bắt đầu | Kết thúc | Trạng thái | % |
|---|---|---|---|---|---|---|---|
| Sprint 0 | Khởi tạo nền tảng | EM-01, BE-01, FE-01, EM-02, FW-01, BE-08 | App FE khởi chạy; Backend chạy + mock; Board bring‑up | TBD | TBD | Planned | 0% |
| Sprint 1 | Luồng dữ liệu cơ bản | BE-03, BE-05, BE-06(part), FE-04/05, FW-03, FW-07 | Hiển thị trạng thái realtime; gửi lệnh mock | TBD | TBD | Planned | 0% |
| Sprint 2 | An toàn & hiệu năng | FW-09, FE-06/07, BE-06(đồ thị/log), EM-06, QA-01/02 | Đồ thị v‑a‑x realtime; E‑Stop đạt tiêu chí | TBD | TBD | Planned | 0% |
| Sprint 3 | OTA & bàn giao | BE-11/12, FW-10, EM-04, FE-08/09/10, OPS-01 | OTA mô phỏng; tài liệu & demo E2E | TBD | TBD | Planned | 0% |

PM cập nhật cột Thời gian, Trạng thái, % theo thực tế sau mỗi buổi họp.

#### 3) Kế hoạch tuần (tracker tổng hợp)

| Tuần | Backend % | Frontend % | FW % | EMBED % | QA/HIL % | Blockers chính | Quyết định cần PM |
|---|---|---|---|---|---|---|---|
| W1 | 0 | 0 | 0 | 0 | 0 | — | Chốt timeline Sprint 0 |
| W2 | 0 | 0 | 0 | 0 | 0 | — | Phân công chi tiết theo ID |
| W3 | 0 | 0 | 0 | 0 | 0 | — | Xác nhận tiêu chí demo giữa kỳ |
| W4 | 0 | 0 | 0 | 0 | 0 | — | Rà soát rủi ro & nguồn lực |

Ghi chú: PM có thể nhân rộng bảng theo số tuần thực tế; mỗi team lead cập nhật % hàng tuần dựa trên số đầu việc hoàn tất (theo DOD).

#### 4) Ma trận giao chéo (để tránh chờ đợi)

| Nhóm | Có thể bắt đầu ngay | Chờ phụ thuộc |
|---|---|---|
| Backend | BE-01, BE-02, BE-08 | BE-03 (chờ schema finalize nhẹ), BE-05 (hạ tầng), BE-11 (sau nền tảng) |
| Frontend | FE-01, FE-02, FE-03 | FE-04/05 (chờ BE-08 mock), FE-06/07 (chờ BE-06) |
| FW | FW-01, FW-02 (song song với EM-02) | FW-03 (sau FW-01), FW-07 (sau EM-02), FW-09 (sau FW-03) |
| EMBED | EM-01, EM-02, EM-03 | EM-04 (sau bring‑up), EM-06 (sau layout tạm ổn) |

---

### 📊 Cập nhật tiến độ Frontend - 2024-12-19

#### ✅ Frontend Tasks Hoàn Thành (100%)

**Khởi tạo & Hạ tầng:**
- ✅ FE-01: Khởi tạo SPA (React+Vite), router, state mgmt
- ✅ FE-02: Thiết lập theme, dark mode, icon, responsive  
- ✅ FE-03: Proxy dev tới Backend; cấu hình env
- ✅ FE-22: Khởi tạo thư mục FE (React+Vite) + demo WS
- ✅ FE-23: Login + Guard + Logout + Dashboard khởi đầu
- ✅ FE-24: Tích hợp FE‑06 WS với auto-reconnect
- ✅ FE-25: Config CRUD tối thiểu trên Dashboard

**Màn hình Chính:**
- ✅ FE-04: Màn hình Overview: thẻ trạng thái Idle/Move/Dock/Fault/E‑Stop
- ✅ FE-05: Nút điều khiển Start/Stop/E‑Stop (xác nhận an toàn)
- ✅ FE-06: Màn hình đồ thị v‑a‑x realtime (pause/zoom) - Prototype HTML
- ✅ FE-07: Màn hình Log: bảng, lọc, tìm kiếm, phân trang
- ✅ FE-08: Màn hình Cấu hình an toàn (CRUD + validate)
- ✅ FE-09: Màn hình Diagnostics: encoder/driver/bus
- ✅ FE-10: E2E smoke test + hướng dẫn sử dụng nhanh
- ✅ FE-11: Cập nhật Overview/Diagnostics hiển thị `tag_id`, `enc.count`, tốc độ phát
- ✅ FE-12: Trang cấu hình RS485 addresses theo `config_spec.md`

**Màn hình Quản trị:**
- ✅ FE-13: User Settings (PA‑1): hồ sơ, mật khẩu, dark mode
- ✅ FE-14: Center Config page
- ✅ FE-15: Jobs Monitor page  
- ✅ FE-16: Admin Management (đơn giản)
- ✅ FE-17: Logs page
- ✅ FE-18: Safety Config page
- ✅ FE-19: Diagnostics page
- ✅ FE-20: RS485 Addresses page
- ✅ FE-21: Audit Trail page

#### 🎯 Tổng kết Frontend

**Trạng thái:** ✅ HOÀN THÀNH 100%
- **Files đã tạo/cập nhật:** 25+ files trong `frontend/src/`
- **API endpoints đã nối:** Tất cả BE APIs đã được tích hợp
- **UI components:** Hoàn thiện với responsive design, dark mode, loading states
- **Features:** Config UI với 3 tabs, Center Page với auto-refresh, Admin management
- **Dev server:** Đang chạy trên http://localhost:5174

**Kết quả đạt được:**
- ✅ Dashboard hoàn chỉnh với real-time telemetry
- ✅ Config management với history/rollback/import-export
- ✅ Center communication với modules/points display
- ✅ Admin panel với user management
- ✅ RS485 bus configuration
- ✅ Responsive design, error handling, toast notifications
- ✅ TypeScript types, consistent theming

**Sẵn sàng cho:** Integration testing với Backend, demo end-to-end

---

## 📋 Prompt Templates cho EMBED & FW Teams

### EMBED Team Prompts
Đã tạo prompt templates chi tiết cho EMBED team trong `docs/tasks/EMBED_PROMPT_TEMPLATE.md`:

**Template chung:**
- Context: Hardware bring-up và low-level drivers cho OHT-50
- Platform: Orange Pi 5B (RK3588)
- Tech stack: C/C++, libgpiod, udev rules, device tree overlays
- Rule: Hardware abstraction, EMI/ESD compliance, production ready

**Prompts cụ thể:**
- **EM-11**: UART1 Enable và Validation - Enable UART1 trong DT/overlay
- **EM-02**: RS485/CAN Transceiver - Thiết kế và implement RS485 transceiver
- **EM-03**: UART/CAN Init + DMA Ring Buffer - Implement UART với DMA ring buffer

### FW Team Prompts  
Đã tạo prompt templates chi tiết cho FW team trong `docs/tasks/FW_PROMPT_TEMPLATE.md`:

**Template chung:**
- Context: Firmware điều khiển thời gian thực cho OHT-50
- Tech stack: C/C++ cho embedded, HAL layer
- Rule: Real-time constraints, safety critical, deterministic behavior
- Platform: Orange Pi 5B (RK3588) với HAL abstraction

**Prompts cụ thể:**
- **FW-01**: Khung FW, Scheduler, Ưu tiên ngắt - Thiết kế firmware architecture
- **FW-02**: HAL - GPIO, PWM, ADC, UART/RS485, Timer - Implement HAL abstraction
- **FW-07**: RS485 Protocol - Frame, CRC, Retry - Implement RS485 protocol
- **FW-03**: State Machine - Idle/Move/Dock/Fault/E-Stop - Implement state machine
- **FW-09**: E-Stop & Interlock - Implement safety mechanisms

### Cách sử dụng Prompt Templates

1. **Chọn template** phù hợp với task ID
2. **Copy prompt** từ file template tương ứng
3. **Điền thông tin task** cụ thể từ PM_TASKS_OHT-50.md
4. **Customize** theo context và requirements
5. **Execute** từng bước theo implementation steps
6. **Validate** theo DOD checklist
7. **Update** progress trong PM tracker

### Ưu tiên triển khai EMBED & FW

**Critical Path cho EMBED:**
1. EM-01: Bring-up phần cứng, pinout (3 ngày)
2. EM-02: RS485/CAN transceiver (3 ngày) - 🔄 Đang làm
3. EM-03: UART/CAN init + DMA ring buffer (3 ngày) - 🔄 Đang làm
4. EM-11: UART1 enable và validation (1 ngày) - Người B, due 2025-08-18

**Critical Path cho FW:**
1. FW-01: Khung FW + scheduler (3 ngày) - Phụ thuộc EM-01
2. FW-02: HAL abstraction (4 ngày) - Phụ thuộc EM-02
3. FW-03: State machine (3 ngày) - Phụ thuộc FW-01
4. FW-07: RS485 protocol (3 ngày) - Phụ thuộc EM-02
5. FW-09: E-Stop & interlock (4 ngày) - Phụ thuộc FW-03

### Cập nhật trạng thái EMBED & FW

**EMBED Team:**
- EM-02: 🔄 Đang làm - RS485/CAN transceiver
- EM-03: 🔄 Đang làm - UART/CAN init + DMA ring buffer
- EM-11: ⏳ To do - UART1 enable và validation (Người B, due 2025-08-18)

**FW Team:**
- FW-01: ⏳ To do - Khung FW + scheduler (chờ EM-01)
- FW-02: ⏳ To do - HAL abstraction (chờ EM-02)
- FW-03: ⏳ To do - State machine (chờ FW-01)
- FW-07: ⏳ To do - RS485 protocol (chờ EM-02)
- FW-09: ⏳ To do - E-Stop & interlock (chờ FW-03)

### Tài liệu tham chiếu

- **EMBED Prompts:** `docs/tasks/EMBED_PROMPT_TEMPLATE.md`
- **FW Prompts:** `docs/tasks/FW_PROMPT_TEMPLATE.md`
- **Programming Prompts:** `docs/tasks/PROGRAMMING_PROMPT_OHT-50.md`
- **Test Checklist:** `docs/specs/EMBED_TEST_CHECKLIST.md`
- **Hardware Specs:** `docs/specs/hardware.md`, `docs/dev_radxa/pinout_radxa.md`

### Ghi chú triển khai

1. **EMBED team** cần hoàn thành EM-02 và EM-03 trước khi FW team có thể bắt đầu FW-02 và FW-07
2. **EM-10 và EM-11** có deadline 2025-08-18, cần ưu tiên cao
3. **FW team** có thể bắt đầu FW-01 song song với EM-02/EM-03
4. **Safety mechanisms** (FW-09) cần được implement sau khi state machine (FW-03) hoàn thành
5. **Integration testing** cần được thực hiện sau khi cả EMBED và FW đều hoàn thành các task cơ bản
