# Đặc tả Driver Động cơ (Motor Driver)

Phiên bản: v1.0 (khởi tạo)
Phạm vi: Giao tiếp điều khiển động cơ cho OHT-50, hỗ trợ 2 profile: Modbus RTU (RS485) và CANopen CiA 402 (CAN).

## 1) Mục tiêu
- Điều khiển chế độ vị trí/tốc độ; giới hạn v/a/j; dừng khẩn.
- Phát hiện và báo lỗi; khôi phục an toàn.

## 2) Tầng giao tiếp
- RS485/Modbus RTU (8N1, 115200 bps mặc định, địa chỉ slave cấu hình)
- CAN/CANopen CiA 402 (11-bit ID mặc định; bitrate 500k)

## 3) Thông số chung
- Đơn vị: tốc độ [mm/s], vị trí [mm], gia tốc [mm/s^2]. Nội bộ driver có thể dùng [rpm/pulse] → map trong HAL.
- Giới hạn: v_max, a_max, j_max cấu hình từ `config.yaml`.
- Timeout giao tiếp: 100 ms (RTU)/50 ms (CAN) mặc định.

## 4) Hồ sơ Modbus RTU (ví dụ)
- Thanh ghi (Holding Registers):
  - 0x0001: Enable (1)/Disable (0)
  - 0x0002: Mode (0: Velocity, 1: Position)
  - 0x0003–0x0004: Target Velocity (int32, unit: 0.1 mm/s)
  - 0x0005–0x0006: Target Position (int32, unit: 0.01 mm)
  - 0x0007–0x0008: Accel (int32, 0.1 mm/s^2)
  - 0x0009–0x000A: Decel (int32, 0.1 mm/s^2)
  - 0x0010: Fault Reset (write 0xBEEF)
- Trạng thái (Input Registers):
  - 0x1001: StatusWord (bit: Ready, Enabled, Fault, Moving, Homed)
  - 0x1002–0x1003: Actual Velocity (int32)
  - 0x1004–0x1005: Actual Position (int32)
  - 0x1006: FaultCode

Ghi chú: Địa chỉ/thanh ghi là mẫu; thay theo driver thực tế hoặc map tại HAL.

## 5) Hồ sơ CANopen (CiA 402) rút gọn
- PDO/SDO chính:
  - Controlword (0x6040), Statusword (0x6041)
  - Modes of Operation (0x6060), Display (0x6061)
  - Target Velocity (0x60FF), Actual Velocity (0x606C)
  - Target Position (0x607A), Actual Position (0x6064)
  - Acc/Dec (0x6083/0x6084)
- Trình tự Enable (Profile Position/Velocity):
  1) Shutdown → Switch On → Enable Operation (Controlword bit)
  2) Set Mode (6060)
  3) Viết Target + trigger (bit start)
- Fault reset: đặt bit Reset Fault trong Controlword.

## 6) Timing & Retry
- Chu kỳ lệnh: 10–20 ms (tuỳ chế độ).
- Retry: 3 lần với backoff 10/20/40 ms.
- Mark fault nếu > N lỗi liên tiếp hoặc timeout > 200 ms.

## 7) Xử lý lỗi
- FaultCode chuẩn: OVERVOLT, OVERCURR, OVERTEMP, ENCODER_LOSS, COMMS_TIMEOUT, ESTOP.
- Khi Fault: dừng an toàn (free-run hoặc brake tuỳ cấu hình), log, yêu cầu reset.

## 8) Tích hợp HAL
- Hàm tối thiểu:
  - `enable(bool)`, `set_mode(mode)`, `set_target(v/pos)`, `set_limits(v,a,j)`
  - `read_state() -> {ready, enabled, fault, pos, vel}`
  - `reset_fault()`

## 9) Kiểm thử
- SIL: mock Modbus/CAN.
- HIL: đo latency, tính đúng vị trí/tốc độ, phản hồi fault, dừng khẩn.
