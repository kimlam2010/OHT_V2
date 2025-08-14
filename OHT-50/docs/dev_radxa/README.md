# Tài liệu lập trình Radxa S0 (Dev Guide)

Thư mục này chứa tài liệu dành cho dev, sắp xếp theo thứ tự ưu tiên đọc/hiểu để triển khai nhanh trên Radxa S0. Không thay đổi các tài liệu tổng quan trong `docs/`.

Thứ tự ưu tiên (đọc trước `00_cursor_rules_dev_radxa.md`):
1. 01_overview_setup.md — Tổng quan nền tảng & chuẩn bị môi trường
2. 02_config_system_yaml.md — Cấu hình hệ thống (`config/system.yaml`)
3. 03_rs485_bus_driver.md — Driver RS485 (frame, CRC, timeout, retry)
4. 04_hal_interfaces.md — HAL Interfaces (motor, io, safety, location)
5. 05_location_fusion.md — Hợp nhất vị trí (RFID + encoder)
6. 06_control_loop_state_machine.md — Vòng điều khiển & State machine
7. 07_center_client.md — Client Center (HTTP/WS hoặc MQTT)
8. 08_logging_telemetry.md — Ghi log & Telemetry
9. 09_testing_sim_hil.md — Kiểm thử (unit/integration/sim/HIL)
10. 10_deployment_ota.md — Triển khai dịch vụ & OTA
11. 11_ui_local_dashboard.md — UI Local Dashboard (Thiết kế giao diện & tính năng)
12. 12_figma_mcp_setup.md — Kết nối MCP với Figma (Cursor, Windows/PowerShell)

Yêu cầu chung:
- Ngôn ngữ: tiếng Việt; sơ đồ dùng Mermaid.
- Không bịa thông số; đánh dấu TBD để chốt sau.
- Bám khớp với các tài liệu: `architecture.md`, `interfaces.md`, `bus_rs485.md`, `module_map.md`, `module_spec.md`, `center_comm.md`.
