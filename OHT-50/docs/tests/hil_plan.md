# Kế hoạch kiểm thử HIL (tối thiểu)

Phiên bản: v1.0 (khởi tạo)

## 1) Thiết lập
- Bench: Radxa + driver + động cơ + encoder + rail ngắn/giả lập
- Kết nối an toàn: E‑Stop 2 lớp hoạt động

## 2) Kịch bản
1) Enable/Disable: thời gian phản hồi < 200 ms
2) Move đến vị trí A: sai số dừng ≤ ±2 mm
3) Dock: tốc độ ≤ vmax_zone, điều kiện dừng đúng mốc
4) Fault (mất encoder): chuyển Fault trong < 100 ms, log đúng mã lỗi
5) E‑Stop: thời gian phản ứng tổng < TBD ms, khoảng cách dừng < TBD mm

## 3) Telemetry/Log
- Ghi pos/vel/state/fault, timestamp monotonic

## 4) Tiêu chí pass/fail
- Tất cả kịch bản đạt chỉ tiêu; không lỗi chưa rõ nguyên nhân
