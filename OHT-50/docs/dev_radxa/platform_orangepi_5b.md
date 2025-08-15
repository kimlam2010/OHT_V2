# Ghi chú nền tảng Orange Pi 5B — UART1, GPIO1_D3/D2, udev (v1.0)

Mục tiêu: Chuẩn hóa bring‑up phần cứng cho OHT‑50 trên Orange Pi 5B (RK3588), bật UART1 cho RS485, xác định GPIO `GPIO1_D3`/`GPIO1_D2` (điều khiển rơ‑le/DE‑RE nếu dùng), và cấu hình udev alias ổn định.

> OS/KERNEL tham chiếu: linux 6.1.43-rk3588 (Debian/Ubuntu bản Orange Pi). Có thể khác nhẹ theo distro — đánh dấu TBD nếu cần điều chỉnh.

## 1) Bật UART1 cho RS485

- Mặc định thiết bị serial hiển thị dạng `ttyS*`. Mục tiêu: sử dụng `UART1` → kỳ vọng `/dev/ttyS1`.
- Trên Orange Pi 5B, bật UART1 qua cấu hình boot (tùy distro):

### Cách A — `orangepiEnv.txt` (phổ biến trên image chính thức)
1. Mở file cấu hình boot (đường dẫn có thể là `/boot/orangepiEnv.txt`):
   - Thêm/điều chỉnh dòng overlays (nếu có):
     ```
     overlays=rk3588-uart1-m1
     ```
   - Nếu dùng auto‑direction RS485 bằng RTS của UART1 (khuyến nghị khi wiring DE/RE→RTS): có thể cần bật RTS/CTS (tùy image):
     ```
     param_uart1_rtscts=on
     ```
2. Lưu file, khởi động lại.

### Cách B — Overlay/DT khác (Armbian/Ubuntu tuỳ biến)
- Sử dụng công cụ cấu hình (ví dụ `armbian-config`) hoặc chỉnh Device Tree Overlay tương ứng để enable `uart1`.
- Tham khảo tài liệu distro nếu không có `orangepiEnv.txt`.

### Xác nhận sau reboot
```bash
ls -l /dev/ttyS* | cat
dmesg | grep -iE "tty|serial|uart" | cat
[ -e /dev/ttyS1 ] && stty -F /dev/ttyS1 -a | cat || echo 'ttyS1 not present'
```

Nếu không thấy `ttyS1`, cần kiểm tra lại overlay hoặc xung đột tài nguyên.

## 2) Wiring RS485 và DE/RE

- Hai phương án điều khiển nửa song công (half‑duplex):
  - Auto‑direction bằng `RTS` của `UART1`: đơn giản, giảm độ trễ chuyển hướng. Yêu cầu wiring DE/RE→RTS.
  - GPIO rời: DE/RE nối GPIO riêng. Cần HAL điều khiển trước/sau khi TX/RX.
- Lưu ý EMI/ESD theo `docs/dev_radxa/14_rs485_can_transceiver.md` và `docs/specs/bus_rs485.md`.

## 3) Xác định GPIO `GPIO1_D3` và `GPIO1_D2`

- Mục tiêu: có bảng ánh xạ `gpiochip:line` để dùng với `libgpiod` (cho rơ‑le hoặc DE/RE nếu đi GPIO rời).
- Cài công cụ (nếu thiếu):
```bash
sudo apt-get update && sudo apt-get install -y gpiod python3-libgpiod
```
- Liệt kê chip/line:
```bash
gpiodetect | cat
gpioinfo | cat
```
- Ghi lại bảng ánh xạ (điền sau khi đo):

| Tên logic | gpiochip | line | Ghi chú |
|---|---|---:|---|
| GPIO1_D3 | gpiochip1 | 3 | Relay1 hoặc DE/RE (nếu dùng) |
| GPIO1_D2 | gpiochip1 | 2 | Relay2 hoặc DE/RE (nếu dùng) - **Đang dùng cho status_led** |

Ví dụ kiểm tra nhanh với `gpioset` (đổi số chip/line thực tế):
```bash
sudo gpioset gpiochip1 3=1; sleep 1; sudo gpioset gpiochip1 3=0
```

## 4) udev alias cố định cho RS485

- Tạo alias `/dev/ttyOHT485` → giúp dịch vụ không phụ thuộc tên kernel.
- Xem hướng dẫn chi tiết tại `docs/dev_radxa/udev_rules_orangepi5b.md`.

## 5) Ví dụ Python RS485 (auto‑RTS)

```python
import serial
from serial.rs485 import RS485Settings

ser = serial.Serial(
    port="/dev/ttyS1", baudrate=115200, bytesize=serial.EIGHTBITS,
    parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout=0.2
)
ser.rs485_mode = RS485Settings(rts_level_for_tx=True, rts_level_for_rx=False,
                               delay_before_tx=0, delay_before_rx=0)
ser.write(b"\xAA\x01\x01\x00\x00\x00")
print(ser.read(64))
ser.close()
```

## 6) Checklist bring‑up (EM‑01/EM‑02/EM‑03 liên quan)

- UART1 đã enable, thấy `ttyS1` sau reboot.
- Wiring RS485: A/B đúng cực; termination 120Ω hai đầu; bias hiện diện.
- DE/RE: chọn 1 trong 2 phương án (RTS hoặc GPIO rời) và ghi rõ trong pinout.
- Chạy script loopback/BER: `tools/rs485/rs485_loop_tester.py`.
- Cập nhật `docs/specs/EMBED_TEST_CHECKLIST.md` với kết quả.

Tham chiếu: `docs/specs/hardware.md`, `docs/dev_radxa/pinout_radxa.md`, `docs/specs/bus_rs485.md`.

## 7) Trạng thái hiện tại (EM-02/EM-03)

### Đã hoàn thành:
- ✅ Cài đặt công cụ: `gpiod`, `python3-libgpiod`, `pyserial`
- ✅ Xác định GPIO mapping: `GPIO1_D3` = `gpiochip1:3`, `GPIO1_D2` = `gpiochip1:2`
- ✅ Tạo udev alias: `/dev/ttyOHT485` → `ttyS9` (tạm thời)
- ✅ Tạo script test: `tools/rs485/rs485_loop_tester.py` (loopback, BER, auto-RTS)

### Cần wiring để test:
- ⏳ UART1 chưa bật được (chỉ có `ttyS9`)
- ⏳ Test loopback/BER cần wiring RS485 thật
- ⏳ Test auto-RTS cần wiring DE/RE → RTS

### Hướng dẫn wiring test:
1. Kết nối A/B của RS485 transceiver
2. Termination 120Ω hai đầu
3. Bias resistor (680Ω-10kΩ)
4. DE/RE → RTS (nếu dùng auto-RTS)
5. Chạy test: `python3 tools/rs485/rs485_loop_tester.py --dev /dev/ttyOHT485 --baud 115200 --auto_rts --count 5000 --payload 16`


