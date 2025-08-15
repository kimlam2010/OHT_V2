# Đặc tả Cấu hình (YAML/JSON), Validation & Rollback

Phiên bản: v1.0 (khởi tạo)

## 1) Cấu trúc YAML (ví dụ)
```yaml
version: 1
hardware:
  motor:
    interface: rs485   # rs485|can
    r: 0.05            # bán kính puly (m)
    vmax_mms: 500.0
    amax_mms2: 300.0
    jmax_mms3: 5000.0
  encoder:
    type: incremental
    resolution_ppr: 2048
    invert: false
safety:
  zones:
    - {start_mm: 0, end_mm: 200, vmax_mms: 200}
  estop_reset_policy: manual
telemetry:
  ws_hz: 10
comm:
  rs485:
    device: /dev/ttyOHT485   # Trên Orange Pi 5B: udev symlink → /dev/ttyS1 (UART1)
    baud: 115200
    addresses:               # ánh xạ địa chỉ RS485 theo module (tham chiếu module_map.md)
      safety: 0x10
      dio_upper: 0x11
      aio_upper: 0x12
      dcm1: 0x13            # DC Motor Driver #1 (axis_drive)
      stm1: 0x14            # Stepper Driver #1 (axis_drive alt)
      dcm2: 0x15            # DC Motor Driver #2 (axis_lift)
      stm2: 0x16            # Stepper Driver #2 (axis_lift alt)
      dio_cargo: 0x17
      aio_cargo: 0x18
      dcm3: 0x19            # DC Motor Driver #3 (axis_door)
      location: 0x1A        # Location Module (RFID + Encoder)
  can:
    iface: can0
```

## 2) Validation rules
- Kiểu dữ liệu, miền giá trị (vmax > 0, amax > 0, jmax > 0)
- Tính nhất quán: nếu interface=rs485, phải có `comm.rs485`
- Nếu có `comm.rs485.addresses`, đảm bảo các địa chỉ là duy nhất, trong miền hợp lệ (ví dụ 1..247 nếu Modbus RTU)
- Lược đồ JSON Schema (tham chiếu) cho kiểm tra tự động

## 3) Versioning & Rollback
- `version` tăng dần; lưu history với timestamp
- Rollback: chọn bản N-1; xác minh tính hợp lệ trước áp dụng

## 4) Tải & áp dụng an toàn
- Kiểm tra trên sandbox trước khi apply
- Restart dịch vụ tối thiểu; không gây gián đoạn an toàn

## 5) Tham chiếu sơ đồ quy trình cấu hình
- Sơ đồ luồng EMBED ↔ FW ↔ BE: xem `docs/rule.mdc` (mục 4.4, sequenceDiagram)
- Pipeline cấu hình FE → BE → FW → EMBED: xem `docs/rule.mdc` (flowchart kế tiếp)
