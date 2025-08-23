# 06) Vòng điều khiển & State machine (Runtime)

## Chu kỳ & lịch
- Tick điều khiển: `dt = 20 ms` (gợi ý), jitter < 2 ms.
- Vòng điều khiển: đọc feedback → update state machine → planner (axis_drive) → phát setpoint → log/telemetry.

## Sơ đồ runtime
```mermaid
sequenceDiagram
  participant LOOP as ControlLoop (dt)
  participant HAL as HAL
  participant SM as StateMachine
  participant PL as Planner

  LOOP->>HAL: read(feedback: axes, io, safety, location)
  LOOP->>SM: step(events from sensors/center)
  SM->>PL: request trajectory (axis_drive)
  PL-->>SM: next setpoint {x,v,a}
  SM->>HAL: command axes/io
  LOOP->>LOOP: telemetry/log
```

## Ưu tiên & ngắt an toàn
- E‑Stop/Fault tiền định ngắt mọi lệnh, ramp xuống 0.
- Timeout từng bước `Align/Door/Cargo`.

## Đồng bộ với Center
- Nhận mission/permit qua WS/MQTT; hàng đợi sự kiện outbound; reconnect backoff.

## TBD
- Tick chính xác, thread/asyncio, phân luồng (bus IO vs control loop).
