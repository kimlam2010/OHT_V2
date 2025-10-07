# KẾ HOẠCH THỰC THI (EXEC PLAN) – CONTROL LOOP CORE

Phiên bản: 1.0  
Ngày cập nhật: 2025-09-09  
Trạng thái: Approved for Execution

---

## 1) 🎯 Mục tiêu
- Chuyển Control Loop sang kiến trúc: VELOCITY + estimator 1D (IMU + ZUPT + RFID), LiDAR cho safety/docking.
- Loại bỏ Position/Homing/Torque ở Firmware; Position do Backend điều phối (segment). 
- Đảm bảo an toàn: WARNING → giảm tốc; CRITICAL/E‑STOP → dừng < 100 ms (p95).

## 2) 🧭 Phạm vi & Kiến trúc (Compliance bắt buộc)
- Firmware: real‑time control, safety monitor, estimator 1D, docking logic (LiDAR gate), không dùng mock runtime.
- Backend: Planner/segment + map, tính remaining_distance, quyết định v_max/v_slow, bật/tắt docking (HTTP API). 
- RS485/Modbus chỉ do Firmware giao tiếp với module (KHÔNG Backend). 
- Tuân thủ: OHT-50 Backend Rules (HTTP/REST), Security by Design, Performance targets.

## 3) 📦 Deliverables
1) Cập nhật tài liệu (đã xong): REQ v1.1, PLAN v1.1, DEV GUIDE v1.1 + Fallback Matrix.  
2) Thực thi trên Firmware: 
   - VELOCITY control + amax/jmax
   - Estimator 1D: IMU + ZUPT + neo RFID
   - Safety policies: WARNING/CRITICAL
   - Docking bằng LiDAR (range‑gate + ổn định thời gian)
   - API HTTP: segment/start, segment/stop, motion/state
3) Bộ test: Unit/Integration/Field + báo cáo latency E‑Stop.

## 4) 🛠️ Công việc chi tiết theo GATE
- Gate A – Clean up modes (0.5 ngày)
  - Remove Position/Homing/Torque khỏi config/tài liệu runtime; giữ Emergency.
  - Bật cờ cảnh báo nếu bản build còn đường dẫn mock (fail build).

- Gate B – Estimator 1D (2 ngày)
  - Tích hợp IMU (lọc, tích phân hạn chế), ZUPT khi v≈0. 
  - RFID anchoring: cập nhật x_est theo `rfid_anchor_trust`; log trước/sau neo.
  - Freshness/health checks theo Fallback Matrix.

- Gate C – VELOCITY + Profile (1 ngày)
  - Áp `amax/jmax` trước PID; xử lý chuyển pha v_max→v_slow khi `remaining ≤ 2 m` (nếu có map).
  - WARNING → `warning_speed_factor` nhân vào v_target; CRITICAL → zero output ngay.

- Gate D – Docking (1.5 ngày)
  - Dock‑approach: crawl speed; LiDAR range‑gate; điều kiện ổn định `dock_stability_time_ms` → dừng.
  - Block docking nếu thiếu LiDAR hoặc health≠ONLINE.

- Gate E – API & Telemetry (1 ngày)
  - HTTP: POST /api/v1/motion/segment/start|stop; GET /api/v1/motion/state.
  - Telemetry: x_est, v, remaining, safety_state, docking_state, freshness/health.

- Gate F – Test & Handover (1 ngày)
  - Unit/Integration: safety policies, fallback paths, API. 
  - Field test: segment dài + giảm tốc 2 m; RFID neo; docking; E‑Stop latency.
  - Báo cáo: KPI, logs, đồ thị.

## 5) ⏱️ Timeline đề xuất
- Tổng: ~7 ngày làm việc. Có thể rút ngắn khi chạy song song B/C với D.

## 6) 👥 Phân công (RACI)
- Dev1: Estimator 1D (IMU, ZUPT, RFID), Fallback checks.  
- Dev2: VELOCITY + amax/jmax, WARNING/CRITICAL policies.  
- Dev3: Docking + LiDAR gate + ổn định.  
- Dev4: HTTP API + Telemetry.  
- QA: Test plan + đo latency, fault injection (CRC/timeout).  
- Lead: Review kiến trúc, đảm bảo compliance, merge.

## 7) ⚙️ Cấu hình chuẩn (tham chiếu)
- v_max_mm_s, v_slow_mm_s, amax_mm_s2, jmax_mm_s3
- slowdown_distance_m=2.0; warning_speed_factor∈[0.3,0.7]
- zupt_threshold; rfid_anchor_trust; imu_bias_update_rate
- docking_enabled; dock_crawl_speed_mm_s; lidar_gate_range_mm; dock_stability_time_ms

## 8) 🔬 Test Plan (tóm tắt)
- Unit: PID vận tốc, limiter v/a/jerk, chuyển pha v, policies WARNING/CRITICAL.
- Integration: IMU freshness, RFID anchoring, LiDAR gate; API start/stop/state.
- Field: 
  - Segment 10 m → tự giảm tốc 2 m; dừng an toàn.
  - WARNING stress → giảm tốc trong 1 chu kỳ; CRITICAL → zero<100 ms.
  - RFID neo: kiểm tra sai số x_est trước/sau thẻ.
  - Docking: dừng trong cửa sổ sai số + ổn định thời gian.

## 9) 🧩 Data Sources & Fallback Matrix (thi hành)
- Providers: IMU (DOCK), RFID (DOCK), LiDAR (SAFETY), MOTOR (TRAVEL_MOTOR)
- Freshness: IMU≤100 ms; LiDAR≤150 ms; RFID stale≤2000 ms; Health=ONLINE
- Fallback:
  - IMU stale → v_slow; quá timeout → STOP
  - RFID thiếu → chạy giới hạn, không neo x_est
  - LiDAR lỗi → cấm docking; limit speed/STOP
  - MOTOR lỗi → STOP ngay; báo FAULT

## 10) ⚠️ Rủi ro & Rollback
- Drift IMU khi thiếu RFID lâu → ZUPT/neo định kỳ; giới hạn v/quãng; cảnh báo Backend.
- LiDAR nhiễu → range‑gate + trung bình trượt; block docking khi không chắc chắn.
- Mất lệnh Backend → policy giữ tốc an toàn/fall back; watchdog kênh HTTP.
- Rollback: giữ nhánh release trước; flag tắt docking; hạ chuẩn tốc độ toàn hệ.

## 11) 📈 KPI & Chất lượng
- E‑Stop: <100 ms (p95); WARNING: điều chỉnh v trong 1 chu kỳ.
- Drift sau neo RFID: giảm rõ rệt so với trước neo (log/đồ thị).
- Docking: đạt cửa sổ sai số + ổn định thời gian.
- Không mock trong runtime; logs/telemetry đầy đủ.

## 12) 🚫 Nghiêm cấm & Compliance
- Cấm giả lập dữ liệu trong runtime. 
- Backend không giao tiếp RS485; chỉ HTTP/REST tới Firmware.
- Bất kỳ vi phạm → chặn merge.

## 13) ✅ Checklist bàn giao
- [ ] VELOCITY + estimator 1D chạy ổn định; không mock runtime
- [ ] WARNING/CRITICAL đạt KPI; E‑Stop < 100 ms
- [ ] Docking hoạt động; block khi LiDAR lỗi
- [ ] API segment/state OK; Telemetry đủ
- [ ] Báo cáo test + logs + đồ thị
- [ ] Tài liệu cập nhật (REQ/PLAN/DEV GUIDE/EXEC PLAN)

## 14) 📚 Truy vết tài liệu
- REQ: `docs/03-REQUIREMENTS/03-FIRMWARE-REQUIREMENTS/REQ_CONTROL_LOOP_SPEC.md` (v1.1)
- PLAN: `docs/04-SLC/01-planning/CONTROL_LOOP_IMPLEMENTATION_PLAN.md` (v1.1)
- DEV GUIDE: `docs/02-HAL/CONTROL_LOOP_DEV_GUIDE.md` (v1.1)
- EXEC PLAN: tài liệu hiện tại

---

## 15) 📣 Hướng dẫn vận hành (bắt buộc, khẩn)

- Lệnh tạm dừng phát triển: Dừng toàn bộ feature mới cho đến khi hoàn tất toàn bộ Gate A→F theo kế hoạch này. Mọi thay đổi ngoài phạm vi phải được Lead approve.
- Báo cáo tiến độ: Mỗi ngày 2 lần (10:00, 17:00) gửi báo cáo ngắn: Gate đang làm, kết quả/risks, kế hoạch 24h tiếp theo.
- Chặn mock runtime: Build sẽ FAIL nếu phát hiện ký tự "mock_" trong mã nguồn runtime `src/`. Unit/integration test có thể giữ mock trong `tests/`.

## 16) 🔌 API đã bật (Gate E)

- POST `/api/v1/motion/segment/start` → Bật control VELOCITY, bắt đầu segment hiện tại.
- POST `/api/v1/motion/segment/stop` → Dừng an toàn, đặt tốc độ mục tiêu 0.
- GET `/api/v1/motion/state` → Trả về: `x_est`, `v`, `remaining`, `safety.{estop,p95}`, `docking`, `freshness`.

Ghi chú: `remaining` và `map` do Backend xác định; FW cung cấp `x_est`, `v`, trạng thái an toàn và docking. Khi có LiDAR/estimator đầy đủ, FW sẽ bổ sung freshness theo Fallback Matrix.

## 17) 📑 QA – Chỉ số cần đo và nộp

- E‑Stop p95 < 100 ms (log + đồ thị).  
- WARNING: điều chỉnh tốc độ trong 1 chu kỳ control.  
- Báo cáo drift trước/sau neo RFID (bảng + đồ thị).  
- Docking: ổn định theo cấu hình `dock_stability_time_ms` và cờ health LiDAR.

## 18) 📦 Điều kiện bàn giao bổ sung

- Hoàn thành toàn bộ checklist mục 13.  
- Đính kèm log, đồ thị, và báo cáo QA theo mục 17.  
- Xác nhận build guard mock hoạt động (screenshot lỗi khi chèn `mock_` thử nghiệm vào `src/`).

