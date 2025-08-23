# Kiến trúc hệ thống OHT-50 (Phiên bản 1.0)

## Mục tiêu
- Tổng quan kiến trúc phần cứng và phần mềm ở mức hệ thống cho OHT-50.
- Rõ vai trò Master ↔ Slave (RS485) ↔ Center; an toàn, bảo mật, khả năng mở rộng.

## Thành phần chính
- Master Module: Orange Pi 5B (RK3588), RS485, LAN 1G, Wi‑Fi 2.4/5G, USB‑C debug, LED, Relay, E‑Stop kênh kép.
- RS485 Bus (Modbus RTU): tối đa 32 thiết bị, termination 120Ω hai đầu, bias chuẩn.
- Center (Backend/SCADA): API cấu hình/telemetry, giám sát, cập nhật.
- Các Slave Modules (tham chiếu): Power/BMS, Drive (Travel), Lift, IO/Sensor Hub, Dock/Charge, (tùy chọn) Guidance/Position.

## Luồng dữ liệu tổng quan
```mermaid
flowchart LR
  CE[Center/SCADA] <--LAN/Wi‑Fi--> MM[Master Module\nRK3588 + TLS + E‑Stop]
  subgraph RS485 Bus (Modbus RTU)
    MM <--RS485--> PWR[Power/BMS]
    MM <--RS485--> DRV[Travel Drive]
    MM <--RS485--> LFT[Hoist/Lift]
    MM <--RS485--> IO[IO/Sensor Hub]
    MM <--RS485--> DCK[Dock/Charge]
  end
  ESTOP[E‑Stop Dual Channel] --> MM
```

## Nguyên tắc an toàn
- E‑Stop kênh kép, giám sát liên tục; dừng trong giới hạn < 100ms (mục tiêu).
- Interlock sạc/docking; bảo vệ nguồn qua BMS/contactor.
- Chế độ an toàn ưu tiên: Fault/E‑Stop luôn ưu tiên hơn lệnh thường.

## Bảo mật & vận hành
- Giao tiếp Center dùng TLS, authN/authZ; cấu hình versioned, audit trail.
- OTA/upgrade có rollback; telemetry/log theo schema thống nhất.

## Khả năng mở rộng
- Plug-and-play theo địa chỉ RS485; auto‑discover/inventory; tối đa 32 thiết bị/bus.
- Phân tách firmware bất biến với cấu hình runtime (ghi NVM), cho phép thay đổi không cần biên dịch lại.

## Tham chiếu liên quan
- `docs/specs/state_machine.md`
- `docs/specs/interfaces.md`
- `docs/technical/final-modbus-register-map.md`
- `docs/dev_radxa/04_hal_interfaces.md`

## Changelog
- 1.0: Tạo tài liệu, bổ sung sơ đồ kiến trúc và nguyên tắc an toàn/bảo mật.


