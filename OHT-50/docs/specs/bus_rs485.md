# Bus RS485 & giao thức

## Yêu cầu chung
- Topology: multi‑drop, half‑duplex, 1 master (Radxa S0), nhiều slave (modules).
- Độ tin cậy: CRC khung, retry, timeout; heartbeat/keepalive; địa chỉ hoá duy nhất.

## Khung bản tin (gợi ý)
- Byte start: 0xAA
- Address: 1 byte (1–247), 0 = broadcast (TBD)
- Command: 1 byte
- Length: 1 byte (payload)
- Payload: 0..N bytes
- CRC16: 2 bytes (Modbus/CCITT – TBD)

Tổng quát: `[0xAA][ADDR][CMD][LEN][PAYLOAD...][CRC16]`

## Lệnh cơ bản (TBD cụ thể từng module)
- System: PING, HEARTBEAT, GET_INFO, SET_ADDR, FW_VERSION.
- DIO: READ_DI, WRITE_DO, CONFIG_MODE.
- AIO: READ_AI[n], WRITE_AO[n], CONFIG_RANGE.
- SAFETY: READ_STATUS, ESTOP_LATCH, RESET_SAFETY.
- DC MOTOR: SET_VEL, SET_POS, SET_TORQUE, READ_FB (x,v,i,temp,fault), ENABLE/DISABLE, STOP_RAMP.
- STEPPER: SET_POS, SET_VEL, HOME, READ_FB (pos, fault), ENABLE/DISABLE, STOP_RAMP.
- LOCATION/ENCODER: READ_COUNT, RESET_COUNT, READ_VEL, READ_STATUS.
- RFID: READ_TAG_ID, SUBSCRIBE_TAG (event), CLEAR_TAG_EVENT.

## Mapping trục (ví dụ)
- `axis_drive` → DC/Stepper địa chỉ `dcm1/stm1`; Location Module `0x1A`.
- `axis_lift` → DC/Stepper địa chỉ `dcm2/stm2`.
- `axis_door` → DC địa chỉ `dcm3`.
- `dio_upper`, `aio_upper`, `dio_cargo`, `aio_cargo`, `safety` → địa chỉ riêng.

## Thời gian & timeout
- Chu kỳ vòng điều khiển: 10–20 ms (TBD), telemetry 10–50 Hz.
- Timeout module: 3× chu kỳ → đánh lỗi, ngắt an toàn trục tương ứng.
- Retry: n lần (TBD) với backoff ngắn; sau đó set fault.

## Khởi tạo & khám phá
- Quét địa chỉ hợp lệ; `GET_INFO` → `module_registry`.
- Đổi địa chỉ an toàn (`SET_ADDR`) chỉ khi đơn lẻ trên bus.

## Tương thích Modbus RTU (tùy chọn)
- Nếu dùng Modbus RTU: map thanh ghi cho các lệnh trên, thống nhất với HAL.
