# Bộ tài liệu thiết kế OHT-50

Mục tiêu: cung cấp khung tài liệu từ phần cứng đến phần mềm làm chuẩn triển khai và kiểm thử. Nội dung ngắn gọn, ưu tiên interface ổn định và tiêu chí an toàn/kiểm thử.

## Danh mục tài liệu
- [Phần cứng & kết nối](./hardware.md)
- [Bus RS485 & giao thức](./bus_rs485.md)
- [Giao tiếp Center (điều phối)](./center_comm.md)
- [Đặc tả module (cấu tạo & khả năng)](./module_spec.md)
- [Ánh xạ module theo phần (Upper/Cargo)](./module_map.md)
- [Kiến trúc tổng thể](./architecture.md)
- [Đặc tả điều khiển (Control)](./control_spec.md)
- [Trạng thái hệ thống (State Machine)](./state_machine.md)
- [An toàn & E‑Stop](./safety.md)
- [Giao diện hệ thống (HAL, API, Telemetry)](./interfaces.md)

## Ghi chú chung
- Thông số chưa có sẽ đánh dấu "TBD" và tập trung trong bảng cấu hình.
- Sơ đồ sử dụng Mermaid để dễ cập nhật.
- Khi tài liệu được duyệt, sẽ hiện thực trong `control/`, `drivers/`, `services/`, `sim/`, `tests/`.
