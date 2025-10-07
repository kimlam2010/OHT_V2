# KẾ HOẠCH TRIỂN KHAI – CONTROL LOOP CORE

Phiên bản: 1.1  
Ngày cập nhật: 2025-09-09  
Trạng thái: Draft (điều chỉnh theo kiến trúc VELOCITY + estimator)

---

## 1) Mục tiêu
- Gỡ toàn bộ giả lập; dùng dữ liệu thật từ IMU/RFID/LiDAR, tốc độ/điều khiển Motor.
- Chỉ giữ VELOCITY control + estimator 1D; hỗ trợ docking; E‑Stop < 100 ms.
- Loại bỏ Position/Homing/Torque ở Firmware; Position được xử lý ở Backend (Planner/segment).

## 2) Phạm vi
- Control Loop (FW): VELOCITY, estimator 1D (IMU + ZUPT + RFID anchor), chính sách tốc độ theo khoảng cách, docking bằng LiDAR.
- Backend: gửi segment và tham số tốc độ, bật/tắt docking, tính remaining_distance khi có map.
- Không thay đổi giao thức thiết bị; chỉ bổ sung API FW↔BE ở layer HTTP.

## 3) Công việc chính (theo thứ tự)
1. Loại bỏ mode không cần thiết
   - Remove: Position/Homing/Torque ở tài liệu và cấu hình; giữ Emergency.
2. Estimator 1D (IMU + RFID + ZUPT)
   - Tích hợp IMU để ước lượng vận tốc/vị trí (tích phân có ràng buộc), ZUPT để reset drift khi đứng yên.
   - Neo RFID: khi đọc thẻ → cập nhật x_est tuyệt đối, lọc theo `rfid_anchor_trust`.
3. VELOCITY control + profile
   - Áp `amax/jmax` trước PID; hỗ trợ chuyển pha v_max → v_slow khi gần đích (theo remaining_distance do Backend cung cấp).
4. Safety & Policies
   - Vòng kiểm tra E‑Stop/safety mỗi chu kỳ; WARNING → `warning_speed_factor`; CRITICAL → zero ngay.
5. Docking bằng LiDAR
   - Dock‑approach: đặt tốc crawl; dùng range‑gate và điều kiện ổn định `dock_stability_time_ms` trước khi dừng.
6. API FW↔BE
   - POST /api/v1/motion/segment/start { length_mm | x_target_mm, v_max_mm_s, v_slow_mm_s, amax_mm_s2, jmax_mm_s3, docking_enabled }
   - POST /api/v1/motion/segment/stop
   - GET /api/v1/motion/state → { x_est_mm, v_mm_s, remaining_mm, safety_state, docking_state }
7. Telemetry/Diagnostics & Stats
   - Xuất x_est, v, safety/docking, counters; log chẩn đoán ngắn gọn.
8. Cấu hình an toàn khởi đầu
   - `v_max_mm_s`, `v_slow_mm_s`, `amax_mm_s2`, `jmax_mm_s3`, `warning_speed_factor`, `zupt_threshold`, `rfid_anchor_trust`, `dock_crawl_speed_mm_s`, `lidar_gate_range_mm`, `dock_stability_time_ms`.

## 4) KPI & SLO
- Velocity: giữ `v_target` êm (vd 50 mm/s), không giật, tuân `amax/jmax`.
- Safety: WARNING phản ứng trong 1 chu kỳ; CRITICAL/E‑STOP < 100 ms (p95).
- Docking: dừng trong cửa sổ sai số cấu hình, ổn định trước hoàn tất.

## 5) Kiểm thử
- Unit: PID vận tốc, giới hạn v/a/jerk, safety policies, logic giảm tốc 2 m.
- Integration: IMU/RFID event path, LiDAR docking gate, API segment/state.
- Field test: segment dài có giảm tốc 2 m; WARNING/CRITICAL; RFID neo; docking.

## 6) Rủi ro & biện pháp
- Drift IMU khi không có RFID lâu → ZUPT/neo định kỳ, giám sát sai lệch, fail‑safe giảm tốc/dừng.
- Mất gói API Backend → giữ tốc độ an toàn theo policy; watchdog kênh lệnh.
- LiDAR nhiễu → range‑gate + trung bình trượt + điều kiện ổn định thời gian.

## 7) Checklist bàn giao
- [ ] Loại bỏ Position/Homing/Torque khỏi Firmware (tài liệu/cấu hình)
- [ ] VELOCITY + estimator 1D hoạt động; không giả lập runtime
- [ ] Safety: WARNING/CRITICAL đạt KPI; E‑Stop < 100 ms
- [ ] Docking bằng LiDAR đạt tiêu chí ổn định
- [ ] API segment/state hoạt động
- [ ] Telemetry/Diagnostics đầy đủ
- [ ] Tài liệu cập nhật

---

## 8) Data Sources & Fallback Matrix (kế hoạch áp dụng)
- Mapping providers
  - IMU: từ `MODULE_TYPE_DOCK` (DOCK & Location)
  - RFID: từ `MODULE_TYPE_DOCK`
  - LiDAR: từ `MODULE_TYPE_SAFETY`
  - MOTOR: từ `MODULE_TYPE_TRAVEL_MOTOR`
- Freshness & Health
  - imu.freshness_timeout_ms=100; lidar.freshness_timeout_ms=150; rfid.event_stale_ms=2000
  - health phải = ONLINE; nếu WARNING → áp `warning_speed_factor`
- Fallback hành động
  - IMU stale → v=v_slow; nếu quá timeout → STOP
  - RFID thiếu → chạy giới hạn; không docking theo RFID
  - LiDAR lỗi → cấm docking; limit speed hoặc STOP
  - MOTOR lỗi → STOP ngay
- QA Tasks
  - Test latency safety p95; fault injection (CRC/timeouts)
  - Verify policy chuyển trạng thái (SAFE→WARNING→CRITICAL)
