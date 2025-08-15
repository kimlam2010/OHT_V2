# Ghi chú nền tảng Orange Pi 5B — UART/GPIO với Device Tree Overlay (v3.0)

Mục tiêu: Chuẩn hóa bring‑up phần cứng cho OHT‑50 trên Orange Pi 5B (RK3588), sử dụng **Device Tree Overlay** để cấu hình UART1 cho RS485.

> OS/KERNEL: linux 6.1.43-rk3588 (Orange Pi OS)
> Phương pháp: Device Tree Overlay (khuyến nghị) + GPIO sysfs

## 1) Cấu hình UART1 với Device Tree Overlay

### Bước 1: Chỉnh sửa file cấu hình
```bash
# Backup file cấu hình
sudo cp /boot/orangepiEnv.txt /boot/orangepiEnv.txt.backup

# Thêm overlay UART1
echo "overlays=uart1-m1" | sudo tee -a /boot/orangepiEnv.txt

# Kiểm tra cấu hình
cat /boot/orangepiEnv.txt | grep overlays
```

### Bước 2: Reboot để load overlay
```bash
sudo reboot
```

### Bước 3: Kiểm tra UART1
```bash
# Kiểm tra device node
ls -la /dev/ttyS*

# Kết quả mong đợi:
# crw-rw---- 1 root dialout 4, 65 /dev/ttyS1  # UART1
# crw-rw---- 1 root dialout 4, 73 /dev/ttyS9  # UART debug
```

## 2) Pin Mapping UART1

### UART1 Mapping (uart1-m1 overlay)
| Chức năng | UART# | TX Pin | RX Pin | GPIO | Device | Ghi chú |
|---|---:|---|---|---|---|
| **RS485** | **UART1** | **Pin 5** | **Pin 3** | **GPIO0_A2/A3** | **/dev/ttyS1** | **RS485 bus** |

### 26-pin Header Mapping
```
 +------+-----+----------+--------+---+   PI5B   +---+--------+----------+-----+------+
 | GPIO | wPi |   Name   |  Mode  | V | Physical | V |  Mode  | Name     | wPi | GPIO |
 +------+-----+----------+--------+---+----++----+---+--------+----------+-----+------+
 |      |     |     3.3V |        |   |  1 || 2  |   |        | 5V       |     |      |
 |   47 |   0 |    SDA.5 |    OUT | 0 |  3 || 4  |   |        | 5V       |     |      |
 |   46 |   1 |    SCL.5 |     IN | 1 |  5 || 6  |   |        | GND      |     |      |
 |   54 |   2 |    PWM15 |     IN | 1 |  7 || 8  | 1 | IN     | RXD.0    | 3   | 131  |
 |      |     |      GND |        |   |  9 || 10 | 1 | IN     | TXD.0    | 4   | 132  |
```

### UART1 Pin Assignment
- **UART1_TX_M1**: Pin 5 (26-pin header) - GPIO0_A2
- **UART1_RX_M1**: Pin 3 (26-pin header) - GPIO0_A3

## 3) Test UART1

### Test cơ bản
```bash
# Test mở UART1
sudo python3 -c "
import serial
with serial.Serial('/dev/ttyS1', 115200) as ser:
    ser.write(b'OHT50_TEST\\r\\n')
    print('✓ UART1 hoạt động')
"
```

### Test TX liên tục
```bash
# Chạy script test TX liên tục
sudo python3 continuous_tx_test.py

# Chọn pattern:
# 1. Text đơn giản (1s interval)
# 2. RS485 Protocol (0.5s interval)
# 3. Binary data (0.2s interval)
# 4. Text dài (2s interval)
```

## 4) GPIO Control cho RS485

### GPIO1_D3 (Relay1/DE-RE Control)
```bash
# Sử dụng gpio command
gpio mode 0 out    # GPIO 47 (wPi 0)
gpio write 0 1     # HIGH
gpio write 0 0     # LOW

# Sử dụng sysfs
echo 35 | sudo tee /sys/class/gpio/export
echo out | sudo tee /sys/class/gpio/gpio35/direction
echo 1 | sudo tee /sys/class/gpio/gpio35/value
echo 0 | sudo tee /sys/class/gpio/gpio35/value
```

### GPIO1_D2 (Relay2/DE-RE Control)
```bash
# Sử dụng gpio command
gpio mode 1 out    # GPIO 46 (wPi 1)
gpio write 1 1     # HIGH
gpio write 1 0     # LOW
```

## 5) Scripts Test

### quick_test.py
```bash
# Test nhanh UART1/GPIO/RS485
sudo python3 quick_test.py
```

### continuous_tx_test.py
```bash
# TX liên tục với nhiều patterns
sudo python3 continuous_tx_test.py
```

### hardware_test.py
```bash
# Test comprehensive
sudo python3 hardware_test.py
```

## 6) Troubleshooting

### UART1 không xuất hiện
```bash
# Kiểm tra overlay
cat /boot/orangepiEnv.txt | grep overlays

# Kiểm tra dmesg
dmesg | grep -i uart

# Reboot nếu cần
sudo reboot
```

### Permission denied
```bash
# Thêm user vào group dialout
sudo usermod -a -G dialout $USER

# Logout và login lại
```

### GPIO không hoạt động
```bash
# Kiểm tra GPIO mapping
gpio readall

# Kiểm tra sysfs
ls -la /sys/class/gpio/
```

## 7) Production Setup

### RS485 Wiring
```
Orange Pi 5B UART1:
- Pin 5 (TX) → RS485 Transceiver TX
- Pin 3 (RX) → RS485 Transceiver RX
- GND → RS485 Transceiver GND

RS485 Transceiver:
- A → Module RS485 A
- B → Module RS485 B
- DE/RE → GPIO1_D3 (manual control)
```

### Termination
- **Termination**: 120Ω resistors hai đầu
- **Bias**: 680Ω-10kΩ resistors (nếu cần)

### Test Production
```bash
# BER test (15 phút)
# Latency test
# EMI/ESD test
```

---

**Changelog v3.0:**
- Thêm Device Tree Overlay method
- Cập nhật pin mapping chính xác
- Thêm scripts test
- Cải thiện troubleshooting


