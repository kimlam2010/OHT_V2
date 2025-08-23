# Radxa OS Image, Kernel/Driver và udev/Serial Naming

Phiên bản: v1.0 (khởi tạo)
Phạm vi: Chuẩn bị hệ điều hành Radxa, driver kernel, cấu hình thiết bị nối tiếp và đặt tên ổn định cho serial/CAN/RS485.

## 1) Ảnh hệ điều hành (OS Image)
- Board: Radxa (model cụ thể: TBD — cập nhật chính xác theo phần cứng thực tế)
- Đề xuất: Debian-based official image (Bookworm) từ Radxa.
- Kernel tối thiểu: ≥ 5.10 (khuyến nghị ≥ 5.15). Kiểm tra:
```bash
uname -a
lsb_release -a | cat
```

### Gói hệ thống cần thiết
```bash
sudo apt update && sudo apt install -y \
  git curl wget vim tmux jq net-tools htop \
  python3 python3-pip python3-venv \
  can-utils python3-can \
  socat minicom screen \
  usbutils udev
```

## 2) Thiết lập thời gian và nhật ký
- Bật `systemd-timesyncd` hoặc NTP client tương đương.
- Chuẩn hoá timezone UTC:
```bash
sudo timedatectl set-timezone UTC
sudo timedatectl set-ntp true
```
- Nhật ký: giữ journald persistent (tùy dung lượng flash):
```bash
sudo mkdir -p /var/log/journal && sudo systemctl restart systemd-journald
```

## 3) Thiết bị nối tiếp, RS485/CAN
- Xác nhận mapping UART/CAN phần cứng của board (GPIO ↔ UART#, CAN#) — xem `dev_radxa/pinout_radxa.md`.
- Với adapter USB–Serial, kiểm tra thuộc tính:
```bash
udevadm info -a -n /dev/ttyUSB0 | less
```

## 4) Quy ước đặt tên thiết bị (udev rules)
Mục tiêu: tên ổn định cho driver, tránh lệ thuộc thứ tự cắm.

Ví dụ: tạo `/etc/udev/rules.d/99-oht-serial.rules`
```udev
# RS485 adapter (ví dụ FTDI) → /dev/ttyOHT485
SUBSYSTEM=="tty", ATTRS{idVendor}=="0403", ATTRS{idProduct}=="6001", SYMLINK+="ttyOHT485"

# Debug console USB–UART → /dev/ttyDBG
SUBSYSTEM=="tty", ATTRS{product}=="USB2.0-Serial", SYMLINK+="ttyDBG"

# CAN via SocketCAN (USB-CAN adapter): đặt tên canOHT0 qua link
SUBSYSTEM=="net", ACTION=="add", ATTRS{manufacturer}=="Seeed", NAME="canOHT0"
```
Sau khi thêm rules:
```bash
sudo udevadm control --reload-rules && sudo udevadm trigger
```

## 5) SocketCAN cấu hình
- Bật CAN interface (ví dụ `can0`) với bitrate phù hợp (ví dụ 500k):
```bash
sudo ip link set can0 up type can bitrate 500000 dbitrate 2000000 fd on
sudo ifconfig can0 up
```
- Kiểm tra:
```bash
ip -details -statistics link show can0 | cat
candump can0 | cat
```

## 6) RS485 cấu hình
- Thiết bị: `/dev/ttyOHT485` (được tạo bởi udev), 115200 8N1 (tham chiếu `14_rs485_can_transceiver.md`).
- Quy ước phần mềm: Modbus RTU/hoặc giao thức hãng — cấu hình trong `services/` và `drivers/`.

## 7) Nhóm quyền và dịch vụ hệ thống
- Thêm user chạy dịch vụ vào nhóm `dialout` (serial) và `netdev` (SocketCAN):
```bash
sudo usermod -aG dialout,netdev $USER
```
- Khung `systemd` service (ví dụ `oht.service`):
```ini
[Unit]
Description=OHT-50 Main Service
After=network-online.target

[Service]
Type=simple
User=oht
Group=oht
WorkingDirectory=/opt/oht
ExecStart=/usr/bin/python3 -m services.main --config /etc/oht/config.yaml
Restart=on-failure
RestartSec=3

[Install]
WantedBy=multi-user.target
```

## 8) Kiểm thử nhanh
- RS485: dùng `socat`/`minicom` để gửi/nhận khung; Modbus: `pymodbus`.
- CAN: `cansend can0 123#DEADBEEF`, quan sát `candump`.

Ghi chú: các thông số đánh dấu TBD cần điền theo model board Radxa và phần cứng thực tế.
