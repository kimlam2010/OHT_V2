# Ghi chú nền tảng Orange Pi 5B — UART/GPIO với opwiring (v2.0)

Mục tiêu: Chuẩn hóa bring‑up phần cứng cho OHT‑50 trên Orange Pi 5B (RK3588), sử dụng `opwiring` để kiểm tra và cấu hình UART/GPIO cho RS485.

> OS/KERNEL: linux 6.1.43-rk3588 (Orange Pi OS)
> Công cụ chính: `opwiring` (Orange Pi Wiring Tool)

## 1) Cài đặt opwiring

```bash
# Cài đặt opwiring
sudo apt update
sudo apt install opwiring

# Kiểm tra phiên bản
opwiring --version
```

## 2) Kiểm tra UART với opwiring

### Liệt kê tất cả UART
```bash
opwiring uart list
```

### Kiểm tra UART cụ thể
```bash
# Kiểm tra UART0 (console)
opwiring uart info 0

# Kiểm tra UART1 (RS485)
opwiring uart info 1

# Kiểm tra UART2, UART3, etc.
opwiring uart info 2
opwiring uart info 3
```

### Bật UART1 cho RS485
```bash
# Bật UART1
opwiring uart enable 1

# Kiểm tra trạng thái
opwiring uart status 1

# Xem device node
ls -l /dev/ttyS*
```

## 3) Kiểm tra GPIO với opwiring

### Liệt kê GPIO
```bash
# Liệt kê tất cả GPIO
opwiring gpio list

# Kiệt kê theo bank
opwiring gpio list --bank 1
opwiring gpio list --bank 2
```

### Kiểm tra GPIO cụ thể
```bash
# Kiểm tra GPIO1_D3 (Relay1/DE-RE)
opwiring gpio info 1 3

# Kiểm tra GPIO1_D2 (Relay2/DE-RE)
opwiring gpio info 1 2

# Kiểm tra GPIO khác
opwiring gpio info 1 0
opwiring gpio info 1 1
```

### Test GPIO
```bash
# Test GPIO1_D3 (Relay1)
opwiring gpio set 1 3 1
sleep 1
opwiring gpio set 1 3 0

# Test GPIO1_D2 (Relay2)
opwiring gpio set 1 2 1
sleep 1
opwiring gpio set 1 2 0
```

## 4) Mapping UART cho RS485

### UART mapping Orange Pi 5B
| Chức năng | UART# | TX Pin | RX Pin | Device | Ghi chú |
|---|---:|---|---|---|---|
| Console | UART0 | GPIO0_A0 | GPIO0_A1 | /dev/ttyS0 | Debug console |
| **RS485** | **UART1** | **GPIO0_A2** | **GPIO0_A3** | **/dev/ttyS1** | **RS485 bus** |
| Motor | UART2 | GPIO0_A4 | GPIO0_A5 | /dev/ttyS2 | Motor driver |
| CAN | UART3 | GPIO0_A6 | GPIO0_A7 | /dev/ttyS3 | CAN bus |

### Kiểm tra pin mapping
```bash
# Kiểm tra pin UART1
opwiring pin info GPIO0_A2  # TX
opwiring pin info GPIO0_A3  # RX

# Kiểm tra pin GPIO1_D3/D2
opwiring pin info GPIO1_D3  # Relay1/DE-RE
opwiring pin info GPIO1_D2  # Relay2/DE-RE
```

## 5) Wiring RS485 với UART1

### Kết nối phần cứng
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

### Test wiring cơ bản
```bash
# Test UART1 loopback (nối TX→RX)
opwiring uart test 1

# Test với RS485 transceiver
echo "test" | opwiring uart write 1
opwiring uart read 1
```

## 6) Cấu hình RS485 với opwiring

### Cấu hình UART1 cho RS485
```bash
# Cấu hình baud rate
opwiring uart config 1 --baud 115200

# Cấu hình RS485 mode
opwiring uart config 1 --rs485

# Cấu hình auto-RTS (DE/RE control)
opwiring uart config 1 --rs485-auto-rts

# Xem cấu hình hiện tại
opwiring uart config 1 --show
```

### Test RS485 protocol
```bash
# Test PING command (0x01)
opwiring uart write 1 --hex "AA 01 01 00 00 00"

# Đọc response
opwiring uart read 1 --hex

# Test GET_INFO command (0x02)
opwiring uart write 1 --hex "AA 01 02 00 00 00"
opwiring uart read 1 --hex
```

## 7) Script test tự động

### Tạo script test RS485
```bash
#!/bin/bash
# test_rs485.sh

echo "=== RS485 Test Script ==="

# Kiểm tra UART1
echo "1. Kiểm tra UART1..."
opwiring uart status 1

# Test GPIO DE/RE
echo "2. Test GPIO DE/RE..."
opwiring gpio set 1 3 1
sleep 0.1
opwiring gpio set 1 3 0

# Test RS485 loopback
echo "3. Test RS485 loopback..."
opwiring uart test 1

# Test protocol
echo "4. Test RS485 protocol..."
opwiring uart write 1 --hex "AA 01 01 00 00 00"
sleep 0.1
opwiring uart read 1 --hex

echo "=== Test hoàn thành ==="
```

## 8) Checklist bring‑up với opwiring

### Đã hoàn thành:
- ✅ Cài đặt `opwiring`
- ✅ Kiểm tra UART mapping
- ✅ Kiểm tra GPIO mapping
- ✅ Tạo script test

### Cần wiring để test:
- ⏳ Kết nối UART1 → RS485 transceiver
- ⏳ Kết nối RS485 transceiver → module
- ⏳ Termination 120Ω hai đầu
- ⏳ Bias resistor (680Ω-10kΩ)
- ⏳ Test loopback thật

### Hướng dẫn wiring:
1. **UART1 TX (GPIO0_A2)** → RS485 Transceiver TX
2. **UART1 RX (GPIO0_A3)** → RS485 Transceiver RX
3. **GND** → RS485 Transceiver GND
4. **RS485 A/B** → Module RS485 A/B
5. **DE/RE** → RTS (auto) hoặc GPIO1_D3 (manual)

### Test sau wiring:
```bash
# Test cơ bản
./test_rs485.sh

# Test với module thật
opwiring uart write 1 --hex "AA 01 01 00 00 00"  # PING
opwiring uart read 1 --hex

opwiring uart write 1 --hex "AA 01 02 00 00 00"  # GET_INFO
opwiring uart read 1 --hex
```

## 9) Troubleshooting

### UART1 không hoạt động
```bash
# Kiểm tra device tree
opwiring dt info uart1

# Kiểm tra overlay
opwiring overlay list
opwiring overlay info rk3588-uart1-m1

# Bật overlay
opwiring overlay enable rk3588-uart1-m1
```

### GPIO không hoạt động
```bash
# Kiểm tra GPIO bank
opwiring gpio bank info 1

# Kiểm tra pin conflict
opwiring pin conflict GPIO1_D3
```

### RS485 không nhận được data
```bash
# Kiểm tra wiring
opwiring uart test 1

# Kiểm tra baud rate
opwiring uart config 1 --show

# Test với scope/logic analyzer
```

Tham chiếu: `docs/specs/hardware.md`, `docs/dev_radxa/pinout_radxa.md`, `docs/specs/bus_rs485.md`.


