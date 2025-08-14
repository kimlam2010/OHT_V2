# Đặc tả Encoder/Feedback

Phiên bản: v1.0 (khởi tạo)
Phạm vi: Cảm biến vị trí/tốc độ cho OHT-50, hỗ trợ encoder quay/tuyến tính hoặc cảm biến vị trí khác (TBD).

## 1) Loại & thông số
- Loại: Incremental/Absolute (TBD)
- Độ phân giải: TBD (ví dụ 1024/2048/4096 ppr hoặc 0.01 mm)
- Sai số hệ thống: ±TBD mm
- Nhiệt độ làm việc: TBD °C

## 2) Giao tiếp
- Electrical: 5V/24V (theo model), output kiểu open-collector/line-driver (RS422) → cần cách ly mức
- Giao thức: A/B/Z (incremental) hoặc SSI/BiSS/CANopen (absolute)
- Mapping tới HAL: `read_position()`, `read_velocity()`, `diagnostics()`

## 3) Lấy mẫu & lọc
- Tần số đọc: 500–1000 Hz (tối thiểu) → phù hợp chu kỳ điều khiển
- Lọc số: Moving Average/Alpha-Beta hoặc Savitzky–Golay (tối ưu sau)
- Bù trễ: ước lượng vận tốc để dự đoán vị trí giữa hai mẫu

## 4) Hiệu chuẩn
- Homing điểm gốc (công tắc hành trình hoặc mốc cơ khí)
- Hệ số quy đổi: x = k·count + offset (mm)
- Nhiệt/giãn nở: hệ số bù (optional) khi có dữ liệu thực nghiệm

## 5) Chẩn đoán & lỗi
- Phát hiện mất xung (glitch), mất nguồn, đứt dây
- So sánh chéo 2 nguồn (nếu có dual encoders)
- Mã lỗi: ENC_LOSS, ENC_DRIFT, ENC_NOISE_HIGH, ENC_PWR

## 6) Kiểm thử
- SIL: mô phỏng tín hiệu encoder; HIL: đo độ trễ, nhiễu
- Tiêu chí: sai số vị trí ≤ ±TBD mm trong dải tốc độ vận hành

## 7) Tích hợp HAL
- Trả về cấu trúc: `{pos_mm, vel_mms, ts, health, fault_code}`
- Timestamp đồng bộ (monotonic clock) để hòa nhập telemetry
