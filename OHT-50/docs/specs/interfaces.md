# Giao diện hệ thống (HAL, API, Telemetry)

## HAL (mức tổng quát)
- `rs485_bus` (FW/EMBED đảm nhiệm): mở/kết nối, gửi/nhận khung, retry/timeout, CRC.
- `module_registry` (FW/EMBED): khám phá, lưu meta (loại, địa chỉ, capabilities), expose cho BE qua Center/API.
- `motor`: giao diện trừu tượng cho DC và Stepper.
- `io`: giao diện DI/DO và AI/AO.
- `safety`: giao diện safety module (E‑Stop, latch, trạng thái).

## Motor Interface (thống nhất DC/Stepper)
- Lệnh: `enable/disable`, `set_velocity(v)`, `set_position(x)`, `stop_ramp(limit)`.
- Phản hồi: `{pos, vel, current?, temp?, fault}`; có thể thiếu trường tùy module.
- Ánh xạ:
  - DC: `set_velocity`/`set_torque`; nếu hỗ trợ vị trí, map `set_position`.
  - Stepper: `set_position`/`set_velocity`; nếu có encoder, closed‑loop.

## IO Interface
- `read_di() -> bitmask`, `write_do(bitmask)`.
- `read_ai() -> list[float]`, `write_ao(list[float])`.

## Safety Interface
- `read_status() -> {estop, interlock, latched}`.
- `latch_reset()` khi điều kiện an toàn thoả mãn.

## Center Interface
- WS hoặc MQTT Client:
  - Heartbeat, Telemetry stream.
  - Mission nhận từ Center, Permit (enter_station/open_door/leave_station).
  - Events upload (reached_station, drop_done, fault...).
  - (Mới) Module registry & points snapshot: FW cung cấp để BE truy vấn khi cần chẩn đoán.

## API (HTTP) cục bộ (tối thiểu)
- `GET /status`
- `POST /command` `{type: move|dock|estop_on|estop_off, target?: number, axis?: id}`.
- (Mới, từ FW cho BE) `GET /modules` → danh sách module `{address, type, name, online}`
- (Mới) `GET /points` → ảnh chụp điểm đo `{di_bitmask, ai[], motors[], encoder_count, tag_id?, rssi?}`

## Telemetry (WS)
- 10–50 Hz: `{t, s_on_rail, tag_id?, encoder_count, axes: [{id, x, v, fault}], state, safety: {estop, fault}, io: {di, ai}, mission}`.

## Cấu hình
- RS485 (ở FW): `{baud, addresses:{dio, aio, safety, dcm1, dcm2, stm1, stm2,...}}`.
- Center: `{mode: ws|mqtt, url|broker, topics?, auth?}`.
- Giới hạn `v_max/a_max/j_max` theo từng trục; geofence; timeout; tham số sim.

## Location Interface
- `read_tag_event() -> {t, tag_id, rssi?}`
- `read_encoder() -> {count, vel}`
- `reset_encoder()`
- (Tuỳ chọn) `get_position() -> s_on_rail` nếu tính tại HAL.
