# YÊU CẦU CHỨC NĂNG – CONTROL LOOP CORE

Phiên bản: 1.1  
Ngày cập nhật: 2025-09-09  
Trạng thái: Draft (điều chỉnh theo kiến trúc mới)

---

## 1) Mục tiêu
- Điều khiển vận tốc real‑time êm, an toàn cho trục Travel Motor (VELOCITY control).
- Ước lượng vị trí 1D (x_est) từ IMU + ZUPT + neo RFID; LiDAR phục vụ vùng an toàn/docking.
- Tuân thủ safety: WARNING → giảm tốc, CRITICAL/E‑STOP → dừng tức thời.
- Hỗ trợ tiếp cận/dừng chính xác khi docking (LiDAR range‑gate) mà không cần encoder.

## 2) Phạm vi
- Chỉ lớp Control Loop (thuật toán điều khiển real‑time tại Firmware).
- Trách nhiệm kiến trúc:
  - Backend (Planner): lập kế hoạch/segment, tính khoảng cách còn lại (nếu có map), điều phối tốc độ mục tiêu (`v_target`), bật/tắt docking.
  - Firmware (Control Loop): bám VELOCITY, ước lượng x_est, áp profile v/a/jerk, thực thi safety và docking.
- Nguồn dữ liệu: IMU, RFID, LiDAR; không sử dụng encoder. Cấm giả lập trong runtime.
- Loại bỏ các mode: Position, Homing, Torque tại Firmware.

## 3) Yêu cầu chức năng
- Velocity Control: đạt/giữ `v_target` êm; áp giới hạn `amax/jmax` trước PID vận tốc.
- Segment Follow: nhận lệnh segment từ Backend (length_mm | x_target_mm, v_max, v_slow, amax, jmax, docking_enabled) và thực thi real‑time.
- Distance‑Based Speed Policy: nếu có map và `remaining_distance ≤ 2 m` → giảm về `v_slow` theo cấu hình.
- Safety Policies: WARNING → nhân `warning_speed_factor` với `v_target`; CRITICAL/E‑STOP → output = 0 ngay.
- RFID Anchoring: khi đọc RFID → neo tuyệt đối cho x_est; nếu `docking_enabled=true` thì chuyển pha dock‑approach.
- Docking Support: dùng LiDAR range‑gate/cảm biến dock để dừng chính xác (crawl speed, điều kiện ổn định).
- Telemetry & Diagnostics: xuất trạng thái safety/docking, x_est, v, remaining, counters, thông điệp chẩn đoán.

## 4) Yêu cầu phi chức năng
- Tần số điều khiển: 200–1000 Hz; dt ổn định.
- Độ trễ E‑Stop → zero output: < 100 ms (p95).
- Không rung giật (giới hạn jerk), không vọt lố lớn khi chuyển pha tốc độ.

## 5) Giao diện dữ liệu
- Input (từ Backend và cảm biến):
  - `v_target` (mm/s) từ Backend; chính sách tốc độ theo khoảng cách (nếu có map).
  - Cảm biến: IMU (gia tốc), RFID events (id, chất lượng), LiDAR/dock range.
  - Config: PID vận tốc, `v_max/v_slow`, `amax/jmax`, `warning_speed_factor`, tham số estimator/docking.
- Trạng thái nội bộ (Firmware):
  - `x_est` (mm), `v_current` (mm/s), `remaining_mm` (nếu nhận được đích từ Backend).
- Output:
  - `control_output` (chuẩn hóa tới cơ cấu chấp hành), `safety_state`, `docking_state`, `telemetry`/diagnostics.

## 6) Ràng buộc
- Cấm dùng dữ liệu giả lập trong runtime.
- Mock chỉ dùng cho unit test với cờ `use_mock=true` + cảnh báo rõ ràng, không đóng gói vào firmware production.
- Ghi chú loại bỏ mode: Position/Homing/Torque KHÔNG thuộc Firmware; nếu cần, xử lý ở Backend như Planner.

## 7) Tiêu chí chấp nhận
- Velocity: ổn định ở `v_target` (ví dụ 50 mm/s) với giới hạn jerk/accel; không giật.
- Safety: WARNING → giảm tốc trong một chu kỳ điều khiển; CRITICAL/E‑STOP → zero output < 100 ms.
- Distance Policy: còn ≤ 2 m thì giảm về `v_slow` theo cấu hình (nếu có map).
- RFID: khi chạm thẻ → neo x_est; nếu `docking_enabled=true` → chuyển dock‑approach.
- Docking: dừng trong cửa sổ sai số đặt, duy trì ổn định trong `dock_stability_time_ms` rồi kết thúc → IDLE.
- Không sử dụng mock trong runtime; telemetry/diagnostics đầy đủ.

---

## 8) Data Sources & Fallback Matrix
- Providers & Health
  - IMU (từ `MODULE_TYPE_DOCK`): freshness_timeout_ms ≤ 100; health = ONLINE
  - RFID (từ `MODULE_TYPE_DOCK`): event_stale_ms ≤ 2000; health = ONLINE
  - LiDAR (từ `MODULE_TYPE_SAFETY`): freshness_timeout_ms ≤ 150; health = ONLINE
  - MOTOR (từ `MODULE_TYPE_TRAVEL_MOTOR`): health = ONLINE
- Fallback Rules
  - IMU missing/stale → giảm về `v_slow`; quá timeout → STOP an toàn
  - RFID missing lâu → tiếp tục nhưng không neo x_est; giới hạn tốc độ/quãng đường theo policy
  - LiDAR missing → cấm docking; giới hạn tốc độ hoặc STOP theo site policy
  - MOTOR lỗi → STOP ngay và báo FAULT
- Policies (tham số hoá)
  - `warning_speed_factor` ∈ [0.3, 0.7]; `slowdown_distance_m` = 2.0 (khi có map)
  - `zupt_threshold`, `rfid_anchor_trust`, `imu_bias_update_rate`

---

## 9) Truy vết mã nguồn (tham chiếu)
- `src/app/core/control_loop.c/.h`
- `src/app/core/safety_monitor.*` (E‑Stop)
- `src/app/modules/travel_motor_module_handler.*` (tốc độ/điều khiển)
- `src/app/managers/communication_manager.*` (Modbus)
 - `src/app/modules/*` (RFID/LiDAR events nếu có)
