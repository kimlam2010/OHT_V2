# ADR-0001: System Architecture Baseline

- Trạng thái: Accepted
- Ngày: 2025-08-19

## Context
Hệ thống OHT-50 cần kiến trúc rõ ràng Master ↔ Slave qua RS485 và kết nối Center để đảm bảo an toàn, bảo mật, khả năng mở rộng.

## Decision
- Chọn kiến trúc Master Module (RK3588) điều phối bus RS485 Modbus RTU (tối đa 32 thiết bị) và kết nối Center qua LAN/Wi‑Fi (TLS).
- E‑Stop kênh kép phần cứng + giám sát phần mềm.
- Telemetry/logs đồng nhất; cấu hình versioned, audit trail.

## Consequences
- Dễ mở rộng số module; plug-and-play theo địa chỉ.
- Yêu cầu quản lý termination/bias tốt và retry/CRC trên RS485.

## Alternatives
- CAN thay RS485 (bị loại do hiện trạng linh kiện và chuẩn docs hiện có).
- Kiến trúc phân tán nhiều Masters (phức tạp hơn cho OHT-50 baseline).

## References
- `docs/specs/architecture.md`
- `docs/technical/00-rs485-system-overview.md`


