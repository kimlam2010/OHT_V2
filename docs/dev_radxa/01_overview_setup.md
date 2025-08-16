# 01) Tổng quan nền tảng & chuẩn bị môi trường (Orange Pi 5B)

## Mục tiêu
- Thiết lập Orange Pi 5B (RK3588) để chạy dịch vụ OHT-50: HAL RS485, Control loop, Center client, Telemetry
- Sử dụng `opwiring` cho UART/GPIO test và cấu hình

## Hệ điều hành & gói cơ bản
- **OS**: Orange Pi OS (Debian/Ubuntu) với kernel 6.1.43-rk3588
- **Cập nhật hệ thống**:
  ```bash
  sudo apt update && sudo apt upgrade -y
  ```
- **Gói cơ bản**:
  ```bash
  sudo apt install -y python3.11 python3.11-venv python3-pip git make gcc
  sudo apt install -y mosquitto-clients screen minicom  # nếu cần MQTT
  ```

## Cài đặt opwiring
- **Cài đặt opwiring**:
  ```bash
  sudo apt install opwiring
  ```
- **Kiểm tra cài đặt**:
  ```bash
  opwiring --version
  ```
- **Test quyền**:
  ```bash
  sudo opwiring uart list
  ```

## Python & môi trường ảo
- **Tạo venv**:
  ```bash
  python3.11 -m venv ~/oht50-venv
  source ~/oht50-venv/bin/activate
  ```
- **Cài dependencies** (theo `requirements.txt`):
  ```bash
  pip install -r requirements.txt
  ```

## UART/RS485 với opwiring
- **Xác định UART**:
  ```bash
  opwiring uart list
  ls -l /dev/ttyS*
  ```
- **Mapping UART Orange Pi 5B**:
  - **UART0**: Console debug (không dùng cho điều khiển)
  - **UART1**: RS485 bus (GPIO0_A2/A3) - **CHÍNH**
  - **UART2**: Motor driver (GPIO0_A4/A5)
  - **UART3**: CAN bus (GPIO0_A6/A7)
- **Bật UART1 cho RS485**:
  ```bash
  opwiring uart enable 1
  opwiring uart status 1
  ```
- **Cấu hình RS485**:
  ```bash
  opwiring uart config 1 --baud 115200
  opwiring uart config 1 --rs485
  opwiring uart config 1 --rs485-auto-rts
  ```

## GPIO với opwiring
- **Liệt kê GPIO**:
  ```bash
  opwiring gpio list
  opwiring gpio list --bank 1
  ```
- **Mapping GPIO Orange Pi 5B**:
  - **GPIO1_D3**: Relay1/DE-RE control
  - **GPIO1_D2**: Relay2/DE-RE control (đang dùng cho status_led)
- **Test GPIO**:
  ```bash
  opwiring gpio set 1 3 1
  sleep 1
  opwiring gpio set 1 3 0
  ```

## Tài khoản dịch vụ & quyền
- **Tạo user `oht`** (không login) chạy dịch vụ:
  ```bash
  sudo useradd -r -s /bin/false oht
  ```
- **Thêm vào nhóm**:
  ```bash
  sudo usermod -a -G dialout,systemd-journal oht
  ```
- **Quyền opwiring**:
  ```bash
  sudo usermod -a -G gpio oht  # nếu cần
  ```

## Cấu trúc thư mục đề xuất trên thiết bị
```
/opt/oht50/           # mã nguồn + venv
├── config/
│   └── system.yaml   # cấu hình
├── logs/             # log files
├── tests/            # test scripts
│   └── test_rs485.sh # hardware test
└── docs/             # documentation
```

## Kiểm tra nhanh phần cứng với opwiring
- **Test UART1**:
  ```bash
  opwiring uart test 1
  ```
- **Test GPIO DE/RE**:
  ```bash
  opwiring gpio set 1 3 1  # bật DE/RE
  opwiring gpio set 1 3 0  # tắt DE/RE
  ```
- **Test RS485 protocol**:
  ```bash
  opwiring uart write 1 --hex "AA 01 01 00 00 00"  # PING
  opwiring uart read 1 --hex
  ```

## Wiring RS485
- **Kết nối phần cứng**:
  ```
  Orange Pi 5B UART1:
  - TX (GPIO0_A2) → RS485 Transceiver TX
  - RX (GPIO0_A3) → RS485 Transceiver RX
  - GND → RS485 Transceiver GND
  
  RS485 Transceiver:
  - A → Module RS485 A
  - B → Module RS485 B
  - DE/RE → RTS (auto-direction) hoặc GPIO1_D3 (manual)
  ```
- **Termination & Bias**:
  - Termination 120Ω hai đầu
  - Bias resistor (680Ω-10kΩ)

## Script test tự động
- **Chạy test đầy đủ**:
  ```bash
  chmod +x test_rs485.sh
  ./test_rs485.sh
  ```

## Systemd services
- **Tạo service**:
  ```bash
  sudo systemctl enable oht50.service
  sudo systemctl start oht50.service
  ```

## Troubleshooting
- **UART1 không hoạt động**:
  ```bash
  opwiring overlay list
  opwiring overlay enable rk3588-uart1-m1
  ```
- **GPIO không hoạt động**:
  ```bash
  opwiring gpio bank info 1
  opwiring pin conflict GPIO1_D3
  ```
- **RS485 không nhận được data**:
  ```bash
  opwiring uart test 1
  opwiring uart config 1 --show
  ```

## TBD
- Ảnh OS/flash, phiên bản kernel chính xác
- Thiết bị UART mapping cuối cùng
- Chân GPIO DE/RE chính xác (nếu không dùng auto-RTS)
- Module RS485 thật để test end-to-end
