# 01) Tổng quan nền tảng & chuẩn bị môi trường

## Mục tiêu
- Thiết lập Radxa S0 để chạy dịch vụ OHT-50: HAL RS485, Control loop, Center client, Telemetry.

## Hệ điều hành & gói cơ bản
- OS: Debian/Ubuntu (TBD bản). Cập nhật hệ thống:
  - `sudo apt update && sudo apt upgrade -y`
- Gói: `python3.11`, `python3.11-venv`, `python3-pip`, `git`, `make`, `gcc`, `mosquitto-clients` (nếu MQTT), `screen`, `minicom`.

## Python & môi trường ảo
- Tạo venv: `python3.11 -m venv ~/oht50-venv && source ~/oht50-venv/bin/activate`
- Cài deps (TBD theo `requirements.txt`).

## UART/RS485
- Xác định thiết bị serial: `/dev/ttyS*` hoặc `/dev/ttyAMA*` (TBD theo Radxa S0).
- Cấu hình RS485 transceiver (DE/RE):
  - Kết nối DE/RE đến GPIO (TBD) hoặc dùng auto‑direction nếu phần cứng hỗ trợ.
  - Udev rules (tùy chọn) cấp quyền nhóm `dialout` cho user dịch vụ.
- Thông số khuyến nghị ban đầu: baud 115200, 8N1, no flow control.

## Tài khoản dịch vụ & quyền
- Tạo user `oht` (không login) chạy dịch vụ.
- Thêm `oht` vào nhóm `dialout`, `systemd-journal`.

## Cấu trúc thư mục đề xuất trên thiết bị
- `/opt/oht50/` mã nguồn + venv
- `/opt/oht50/config/system.yaml` cấu hình
- `/var/log/oht50/` log
- `systemd` units: `oht50.service`, `oht50-center.service`

## Kiểm tra nhanh phần cứng
- Dùng `minicom/screen` mở cổng RS485, loopback để thử gửi/nhận.
- Kiểm tra GPIO DE/RE bật đúng khi truyền.

## TBD
- Ảnh OS/flash, phiên bản kernel, chân GPIO DE/RE chính xác, thiết bị UART.
