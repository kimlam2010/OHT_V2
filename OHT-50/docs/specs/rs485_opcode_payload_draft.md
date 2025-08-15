# RS485 Opcode/Payload Draft - OHT-50 (Cập nhật)

Phiên bản: v1.0 (Cập nhật theo module thực tế)
Phạm vi: Định nghĩa khung lệnh cho RS485 giữa Master (Orange Pi 5B) và các Module thực tế

## 1) Khung bản tin (frame)
- Format: `[0xAA][ADDR][CMD][LEN][PAYLOAD...][CRC16_LE]`
  - `ADDR` (u8): Địa chỉ module (0x02-0x07)
  - `CMD` (u8): mã lệnh bên dưới
  - `LEN` (u8): số byte payload
  - `CRC16_LE`: CRC Modbus (little-endian)
- Endian: LITTLE-ENDIAN cho mọi số nhiều byte
- Timeout: 50 ms mặc định; Retry: 2 lần (backoff 10 ms)

Ví dụ hex (PING tới addr 0x02, không payload):
```
AA 02 01 00 | CRC16_LE
```

## 2) Hệ mã lệnh (Cập nhật theo module thực tế)

### 0x01 PING (Tất cả module)
- Req: none
- Resp payload: `u8 ok=1`

### 0x02 GET_INFO (Tất cả module)
- Req: none
- Resp payload (little-endian):
  - `u16 model_id`
  - `u8 fw_major`, `u8 fw_minor`
  - `u32 capabilities` (bitmask theo module type)

## Module 0x02 - Power Management

### 0x10 GET_BATTERY_STATUS
- Req: none
- Resp: `u8 soc_percent`, `u8 soh_percent`, `i16 temp_c10`, `u16 voltage_mv`, `u16 current_ma`

### 0x11 GET_CHARGER_STATUS
- Req: none
- Resp: `u8 charger_state`, `u16 charge_current_ma`, `u16 charge_voltage_mv`, `u8 charge_mode`

### 0x12 SET_CHARGE_MODE
- Req: `u8 mode` (0=disable, 1=enable, 2=fast, 3=trickle)
- Resp: `u8 ok`

### 0x13 GET_POWER_STATUS
- Req: none
- Resp: `u16 v12_mv`, `u16 v24_mv`, `u16 v48_mv`, `u16 v5_mv`, `u16 v33_mv`, `u16 fault_bits`

## Module 0x03 - Stepper Motor

### 0x20 SET_POSITION
- Req: `i32 pos_steps`
- Resp: `u8 ok`

### 0x21 SET_VELOCITY
- Req: `u32 vel_steps_per_sec`
- Resp: `u8 ok`

### 0x22 GET_POSITION
- Req: none
- Resp: `i32 pos_steps`, `i32 vel_steps_per_sec`, `u16 current_ma`, `u16 fault_bits`

### 0x23 ENABLE_MOTOR
- Req: `u8 enable` (1=enable, 0=disable)
- Resp: `u8 ok`

### 0x24 STOP_MOTOR
- Req: `u16 decel_steps_per_sec2` (0 = dùng mặc định an toàn)
- Resp: `u8 ok`

### 0x25 GET_LIMIT_STATUS
- Req: none
- Resp: `u8 limit_forward`, `u8 limit_reverse`, `u8 home_sensor`

## Module 0x04 - DC Motor Cargo

### 0x30 SET_LIFT_POSITION
- Req: `i32 pos_mm`
- Resp: `u8 ok`

### 0x31 SET_LIFT_VELOCITY
- Req: `u32 vel_mm_per_sec`
- Resp: `u8 ok`

### 0x32 GET_LIFT_STATUS
- Req: none
- Resp: `i32 pos_mm`, `i32 vel_mm_per_sec`, `u16 current_ma`, `u16 load_grams`, `u16 fault_bits`

### 0x33 ENABLE_LIFT
- Req: `u8 enable` (1=enable, 0=disable)
- Resp: `u8 ok`

### 0x34 STOP_LIFT
- Req: `u16 decel_mm_per_sec2` (0 = dùng mặc định an toàn)
- Resp: `u8 ok`

### 0x35 GET_LOAD_SENSOR
- Req: none
- Resp: `u16 load_grams`, `u8 overload_flag`

## Module 0x05 - DI/DO

### 0x40 READ_DI
- Req: none
- Resp: `u16 di_bitmask` (8 DI channels)

### 0x41 WRITE_DO
- Req: `u16 mask`, `u16 value`
- Resp: `u8 ok`

### 0x42 GET_DI_STATUS
- Req: none
- Resp: `u16 di_bitmask`, `u32 timestamp_ms`

### 0x43 GET_DO_STATUS
- Req: none
- Resp: `u16 do_bitmask`

### 0x44 SET_DO_PATTERN
- Req: `u8 pattern_id`, `u16 pattern_data`
- Resp: `u8 ok`

## Module 0x06 - Analog Input

### 0x50 READ_AI
- Req: `u8 channel` (0xFF = tất cả)
- Resp (tất cả): `u8 n`, tiếp theo `n * i16 ai_val` (đơn vị: 0.01V)
- Resp (1 kênh): `i16 ai_val`

### 0x51 GET_DISTANCE
- Req: `u8 sensor_id`
- Resp: `u16 distance_mm`, `u8 valid_flag`

### 0x52 SET_CALIBRATION
- Req: `u8 channel`, `i16 offset`, `u16 scale`
- Resp: `u8 ok`

### 0x53 GET_AI_STATUS
- Req: none
- Resp: `u8 n_channels`, `u16 calibration_data[8]` (offset, scale pairs)

## Module 0x07 - Location

### 0x60 READ_TAG_ID
- Req: none
- Resp: `u8 tag_len`, `u8[tag_len] tag_bytes`, `i8 rssi`

### 0x61 READ_ENCODER
- Req: none
- Resp: `u32 encoder_count`, `i32 encoder_velocity`

### 0x62 RESET_ENCODER
- Req: none
- Resp: `u8 ok`

### 0x63 GET_LIDAR_DATA
- Req: `u8 scan_mode` (0=point, 1=line, 2=area)
- Resp: `u8 n_points`, `u16 distances[n_points]` (mm)

### 0x64 GET_FUSED_POSITION
- Req: none
- Resp: `i32 pos_mm`, `u8 tag_id`, `u16 confidence_percent`, `u32 timestamp_ms`

## 3) Quy ước lỗi
- Nếu lỗi: trả về frame hợp lệ với `CMD` bit7=1 (CMD|0x80) và payload: `u8 err_code`
  - 0x01: CRC_ERROR, 0x02: TIMEOUT, 0x03: BAD_CMD, 0x04: BAD_PARAM, 0x05: DENIED
  - 0x06: MOTOR_FAULT, 0x07: OVERLOAD, 0x08: SAFETY_VIOLATION

## 4) Timing & an toàn
- Chu kỳ vòng lặp: 10–20 ms. Tổng thời gian transact mỗi module ≤ 2–3 ms.
- Lệnh điều khiển (SET_*) phải kiểm tra interlock/E‑Stop trước khi thực thi.
- Ưu tiên: đường an toàn không phụ thuộc giao tiếp; giao tiếp chỉ giám sát/latch khi có thể.

## 5) Ví dụ đóng gói

### PING tới Power Module (0x02)
- Tx: `AA 02 01 00 | CRC`
- Rx OK: `AA 02 01 01 01 | CRC`

### GET_BATTERY_STATUS tới Power Module (0x02)
- Tx: `AA 02 10 00 | CRC`
- Rx OK: `AA 02 10 08 64 95 00 14 0F 27 00 00 | CRC`
  - SOC: 100%, SOH: 95%, Temp: 20°C, Voltage: 4000mV, Current: 0mA

### SET_POSITION tới Stepper Motor (0x03)
- Tx: `AA 03 20 04 00 00 00 00 | CRC` (position = 0 steps)
- Rx OK: `AA 03 20 01 01 | CRC`

## 6) Mapping sang Telemetry/Points
- `READ_DI` → `di_bitmask` (points.digital)
- `READ_AI` → `ai[]` (points.analog)
- `GET_BATTERY_STATUS` → `power.battery`
- `GET_LIFT_STATUS` → `cargo.lift`
- `GET_POSITION` → `motor.position`
- `READ_TAG_ID` → `location.tag_id`
- `GET_FUSED_POSITION` → `location.position`

## 7) Module Address Mapping
- 0x02: Power Management Module
- 0x03: Stepper Motor Module
- 0x04: DC Motor Cargo Module
- 0x05: DI/DO Module
- 0x06: Analog Input Module
- 0x07: Location Module
