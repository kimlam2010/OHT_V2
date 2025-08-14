# 02) Cấu hình hệ thống (`config/system.yaml`)

## Mục tiêu
- Chuẩn hoá cấu hình cho RS485, module, trục, cảm biến, Center, Telemetry.

## Cấu trúc đề xuất
```yaml
rs485:
  port: /dev/ttyS0
  baud: 115200
  parity: none
  stop_bits: 1
  de_re_gpio: TBD  # nếu dùng GPIO điều khiển
  retry: 2
  timeout_ms: 30

modules:
  safety: 0x10
  dio_upper: 0x11
  aio_upper: 0x12
  dcm1: 0x13
  stm1: 0x14
  dcm2: 0x15
  stm2: 0x16
  dio_cargo: 0x17
  aio_cargo: 0x18
  dcm3: 0x19
  location: 0x1A

axes:
  axis_drive:
    type: dc   # dc|stepper
    driver_addr: 0x13
    encoder: external   # integrated|external|none
    limits: { v_max: TBD, a_max: TBD, j_max: TBD }
    dt: 0.02
    rail_length: TBD
  axis_lift:
    type: stepper
    driver_addr: 0x16
    stroke: TBD
    speed: TBD
  axis_door:
    type: dc
    driver_addr: 0x19
    open_time_s: TBD
    close_time_s: TBD

sensors:
  dock_align_di: dio_upper:0  # bit index
  door_open_sw: dio_cargo:0
  door_close_sw: dio_cargo:1
  obstacle_di: dio_upper:1
  cargo_present_ai: aio_cargo:0

location:
  tag_table:
    # tag_id: position_m
    1001: 0.0
    1002: 2.0
    1003: 4.0
  encoder_ppr: TBD
  wheel_radius_m: TBD

center:
  mode: ws   # ws|mqtt
  url: ws://center.local/robot
  auth_token: TBD
  heartbeat_s: 2

telemetry:
  rate_hz: 20
  fields: [s_on_rail, tag_id, encoder_count, axes, safety, io]
```

## Ghi chú
- Địa chỉ/module khớp `module_map.md`.
- Giới hạn trục khớp `control_spec.md`.
- Tham số RFID/encoder khớp `module_spec.md`.
