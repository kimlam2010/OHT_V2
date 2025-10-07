# HƯỚNG DẪN CHO DEV – CONTROL LOOP CORE

Phiên bản: 1.1  
Ngày cập nhật: 2025-09-09

---

## 1) Kiến trúc nhanh
- Chỉ giữ VELOCITY control tại Firmware.
- Ước lượng vị trí 1D `x_est` bằng IMU + ZUPT + neo RFID; LiDAR dùng cho vùng an toàn/docking.
- Backend là Planner: gửi segment/`v_target`, tính `remaining_distance` (nếu có map), bật/tắt docking.
- Vòng FW: áp profile v/a/jerk → PID vận tốc → kiểm tra safety → cập nhật telemetry (x_est, v, safety/docking).

## 2) Không dùng giả lập trong runtime
- Mock chỉ cho unit test (`use_mock=true`) và phải log cảnh báo rõ ràng.
- Runtime/staging/prod: luôn dùng dữ liệu thật (IMU/RFID/LiDAR/Motor IO).

## 3) Nguồn dữ liệu thật (gợi ý nối)
- IMU: gia tốc dọc trục → lọc → tích phân thành v → tích phân có ràng buộc thành x; dùng ZUPT reset trôi khi v≈0.
- RFID: sự kiện thẻ (id, offset) → neo tuyệt đối x_est theo `rfid_anchor_trust`.
- LiDAR: range‑gate cho vùng an toàn và docking; không dùng để đo vị trí tuyệt đối toàn tuyến.
- Motor IO: kênh điều khiển thực thi tốc độ (không yêu cầu encoder).

## 4) Mode vận hành
- Velocity: đặt `v_target` → PID vận tốc, có hạn chế `amax/jmax` và chính sách WARNING/CRITICAL.
- Emergency: E‑Stop/CRITICAL → zero output tức thì.
- Ghi chú: Position/Homing/Torque đã loại khỏi Firmware. Position được lập kế hoạch ở Backend theo segment.

## 5) Limits & Settings (khởi đầu an toàn)
- v_max_mm_s, v_slow_mm_s, amax_mm_s2, jmax_mm_s3
- warning_speed_factor (vd 0.5), slowdown_distance_m (=2.0)
- zupt_threshold, rfid_anchor_trust, imu_bias_update_rate
- docking_enabled, dock_crawl_speed_mm_s, lidar_gate_range_mm, dock_stability_time_ms

## 6) PID khởi đầu an toàn (vận tốc)
- Velocity: Kp=0.5, Ki=0.0, Kd=0.05, output ±1.0; integral ±0.2.

## 7) Diagnostics
- Dùng `control_loop_get_diagnostics(...)` để in nhanh state/mode/v_target/x_est/v/errors/output/safety/docking.

## 8) Test thực địa (gợi ý kịch bản)
- Segment dài (có map): quan sát tự giảm về v_slow khi còn 2 m.
- WARNING → giảm tốc trong 1 chu kỳ; CRITICAL/E‑STOP → zero <100 ms.
- RFID neo x_est: trước/sau thẻ sai số giảm; log cập nhật.
- Docking: LiDAR range‑gate; dừng trong cửa sổ sai số + ổn định thời gian.

---

## 9) Truy vết mã nguồn
- `src/app/core/control_loop.c/.h`
- `src/app/core/safety_monitor.*`
- `src/app/modules/travel_motor_module_handler.*`
- `src/app/modules/*` (RFID/LiDAR events nếu có)

---

## 10) Data Sources & Fallback Matrix (hướng dẫn dev)
- Providers
  - IMU: lấy từ DOCK & Location module; đảm bảo `freshness_ms ≤ 100`
  - RFID: sự kiện thẻ từ DOCK module; `event_stale_ms ≤ 2000`
  - LiDAR: từ SAFETY module; `freshness_ms ≤ 150`
  - MOTOR: Travel Motor module để thực thi tốc độ
- Kiểm tra chất lượng trước khi dùng
  - `health == ONLINE` và `fresh == true`; nếu `WARNING` → áp `warning_speed_factor`
- Fallback
  - IMU stale → hạ `v_target` về `v_slow`; quá timeout → STOP
  - RFID thiếu → không neo x_est; giới hạn tốc độ/quãng đường
  - LiDAR lỗi → cấm docking; limit speed/STOP
  - MOTOR lỗi → STOP ngay, phát FAULT
