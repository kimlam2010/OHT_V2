# RS485 Opcode/Payload Draft (FW-11)

Phiên bản: draft v0.1
Phạm vi: Định nghĩa khung lệnh tối thiểu cho RS485 giữa Master (FW) và các Module (motor/IO/location…)

## 1) Khung bản tin (frame)
- Format: `[0xAA][ADDR][CMD][LEN][PAYLOAD...][CRC16_LE]`
  - `ADDR` (u8): 1..247 (0 = broadcast nếu áp dụng)
  - `CMD` (u8): mã lệnh bên dưới
  - `LEN` (u8): số byte payload
  - `CRC16_LE`: CRC Modbus (little-endian)
- Endian: LITTLE-ENDIAN cho mọi số nhiều byte
- Timeout: 50 ms mặc định; Retry: 2 lần (backoff 10 ms)

Ví dụ hex (PING tới addr 0x1A, không payload):
```
AA 1A 01 00 | CRC16_LE
```

## 2) Hệ mã lệnh (tối thiểu)

### 0x01 PING
- Req: none
- Resp payload: `u8 ok=1`

### 0x02 GET_INFO
- Req: none
- Resp payload (little-endian):
  - `u16 model_id`
  - `u8 fw_major`, `u8 fw_minor`
  - `u32 capabilities` (bitmask: bit0=DIO, bit1=AIO, bit2=SAFETY, bit3=DCM, bit4=STEP, bit5=LOCATION)

### 0x10 READ_DI (DIO)
- Req: none
- Resp: `u16 di_bitmask`

### 0x12 WRITE_DO (DIO)
- Req: `u16 mask`, `u16 value`
- Resp: `u8 ok` (1=success)

### 0x11 READ_AI (AIO)
- Req: `u8 channel` (0xFF = tất cả)
- Resp (tất cả): `u8 n`, tiếp theo `n * i16 ai_val` (đơn vị: 0.01V hoặc 0.01mA theo module)
- Resp (1 kênh): `i16 ai_val`

### 0x20 READ_COUNT (Encoder / Location)
- Req: none
- Resp: `u32 encoder_count`

### 0x21 RESET_COUNT (Encoder / Location)
- Req: none
- Resp: `u8 ok`

### 0x22 READ_TAG_ID (RFID / Location)
- Req: none
- Resp: `u8 tag_len`, `u8[tag_len] tag_bytes`, `i8 rssi`

### 0x30 READ_FB (Motor Feedback)
- Req: none
- Resp: `i32 pos_01mm`, `i32 vel_01mms`, `i16 current_mA`, `i16 temp_c10`, `u16 fault_bits`
  - pos_01mm: vị trí (0.01 mm)
  - vel_01mms: vận tốc (0.01 mm/s)
  - temp_c10: nhiệt độ (°C * 10)

### 0x31 SET_POS (Motor)
- Req: `i32 pos_01mm`
- Resp: `u8 ok`

### 0x32 SET_VEL (Motor)
- Req: `i32 vel_01mms`
- Resp: `u8 ok`

### 0x33 STOP_RAMP (Motor)
- Req: `u16 decel_01mms2` (0 = dùng mặc định an toàn)
- Resp: `u8 ok`

### 0x40 ENABLE (Motor)
- Req: `u8 enable` (1=enable, 0=disable)
- Resp: `u8 ok`

## 3) Quy ước lỗi
- Nếu lỗi: trả về frame hợp lệ với `CMD` bit7=1 (CMD|0x80) và payload: `u8 err_code`
  - 0x01: CRC_ERROR, 0x02: TIMEOUT, 0x03: BAD_CMD, 0x04: BAD_PARAM, 0x05: DENIED

## 4) Timing & an toàn
- Chu kỳ vòng lặp: 10–20 ms. Tổng thời gian transact mỗi module ≤ 2–3 ms.
- Lệnh điều khiển (SET_*) phải kiểm tra interlock/E‑Stop trước khi thực thi.
- Ưu tiên: đường an toàn không phụ thuộc giao tiếp; giao tiếp chỉ giám sát/latch khi có thể.

## 5) Ví dụ đóng gói
- `READ_COUNT` tới addr 0x1A (không payload):
  - Tx: `AA 1A 20 00 | CRC`
  - Rx OK: `AA 1A 20 04 C0 5D 01 00 | CRC` → encoder_count = 0x00015DC0 = 90,000

## 6) Mapping sang Telemetry/Points
- `READ_DI` → `di_bitmask` (points.digital)
- `READ_AI` → `ai[]` (points.analog)
- `READ_FB` → motors[{pos, vel, current, temp, fault}]
- `READ_COUNT` → `encoder_count`
- `READ_TAG_ID` → `tag_id`, `rssi`

## 7) Test & xác minh
- Tool host (Python/C) gửi/nhận khung để bench: đo BER, latency; stress timeout/retry/CRC.
- Lưu log dạng CSV: `t, addr, cmd, ok, retries, latency_ms, err_code?`.

Ghi chú: Đây là draft; FW có thể tinh chỉnh field/độ phân giải tùy theo khả năng module. Mọi thay đổi cần cập nhật đồng bộ schema BE/FE.
