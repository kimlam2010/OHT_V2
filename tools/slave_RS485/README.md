# 🔧 RS485 Slave Simulator - AUTO CH340 DETECT

Modbus RTU slave simulator với **tự động phát hiện CH340 USB-to-Serial** cho OHT-50.

## ✨ Tính năng mới v2.0

- 🔍 **Auto-detect CH340** - Không cần set port thủ công
- ✅ **Tự động verify** port có thể mở được  
- 📊 **Debug tools** chi tiết cho RS485 và Modbus
- 🎯 **Không còn lỗi port** - Tự động tìm đúng port CH340
- 🚀 **Chạy ngay** - Không cần config phức tạp

---

## 🚀 Cách sử dụng nhanh

### 1️⃣ **Chạy server bình thường** (tự động detect CH340)

```bash
cd /home/orangepi/Desktop/OHT_V2/tools/slave_RS485
./run.sh
```

**Output:**
```
[INFO] 🔍 Đang tìm CH340 USB-to-Serial converter...
[INFO] ✅ Tự động detect: /dev/ttyUSB0
[INFO] Starting RS485 Modbus RTU slave simulator on /dev/ttyUSB0 @ 115200bps
```

### 2️⃣ **Debug tools** (nếu có vấn đề)

```bash
./debug.sh
```

**Menu:**
```
1) 📡 RAW Debug    - Echo test (test hardware TX/RX)
2) 🔧 Modbus Debug - Parse frames + auto response  
3) 🏃 Server Debug - Pymodbus với verbose logging
4) 📊 Port Info    - Kiểm tra CH340 detection
```

---

## 🔍 Tự động phát hiện CH340

### Kiểm tra CH340 có được detect không:

```bash
python3 port_detector.py
```

**Output mẫu:**
```
🔍 Đang tìm CH340 USB-to-Serial converters...

✅ Tìm thấy 1 CH340 port(s):

📌 Port #1:
   Device:       /dev/ttyUSB0
   Description:  USB Serial
   VID:PID:      1a86:7523
   Serial:       None
   Manufacturer: QinHeng Electronics
   Status:       ✅ OK - Có thể mở được

🎯 Recommended port: /dev/ttyUSB0
```

---

## 📦 Cài đặt (lần đầu tiên)

```bash
cd /home/orangepi/Desktop/OHT_V2/tools/slave_RS485

# Tạo virtual environment
python3 -m venv .venv

# Activate
source .venv/bin/activate

# Install dependencies
pip install -r requirements.txt
```

---

## ⚙️ Configuration

### Modbus Unit IDs:

| Module | Unit ID | Hex | Mô tả |
|--------|---------|-----|-------|
| Motor | 4 | 0x04 | Travel motor control |
| Power | 2 | 0x02 | Power management |
| Safety | 3 | 0x03 | Safety system |
| Dock | 5 | 0x05 | Docking control |

### Environment Variables:

| Variable | Mô tả | Mặc định |
|----------|-------|----------|
| `RS485_PORT` | Serial port | **Auto-detect CH340** |
| `RS485_BAUD` | Baud rate | `115200` |
| `RS485_PARITY` | Parity | `N` (None) |
| `RS485_STOPBITS` | Stop bits | `1` |
| `RS485_BYTESIZE` | Byte size | `8` |
| `UNIT_MOTOR` | Motor unit ID | `0x04` |
| `UNIT_POWER` | Power unit ID | `0x02` |
| `UNIT_SAFETY` | Safety unit ID | `0x03` |
| `UNIT_DOCK` | Dock unit ID | `0x05` |
| `EXTRA_MOTOR_UNITS` | Extra motor slaves | `0x06,0x07,0x08` |

### Ví dụ override:

```bash
# Set port thủ công (không dùng auto-detect)
export RS485_PORT=/dev/ttyUSB0
./run.sh

# Set baudrate khác
export RS485_BAUD=9600
./run.sh

# Set parity
export RS485_PARITY=E  # Even parity
./run.sh
```

---

## 🛠️ Files trong project

| File | Mô tả |
|------|-------|
| `run.sh` | **Main script** - Chạy server với auto-detect |
| `server.py` | Pymodbus RTU server implementation |
| `port_detector.py` | 🆕 Auto-detect CH340 USB converter |
| `debug.sh` | 🆕 Debug tools menu |
| `debug_raw.py` | 🆕 RAW echo test (test hardware) |
| `debug_modbus.py` | 🆕 Modbus parser + auto response |
| `DEBUG_GUIDE.md` | 🆕 Hướng dẫn debug chi tiết |
| `modules/` | Modbus register definitions |
| `requirements.txt` | Python dependencies |

---

## 🔧 Troubleshooting

### ❌ Vấn đề 1: Không tìm thấy CH340

**Triệu chứng:**
```
[ERROR] ❌ Không tìm thấy CH340!
```

**Kiểm tra:**

```bash
# 1. USB có cắm không?
lsusb | grep CH340
# Nên thấy: Bus 005 Device 002: ID 1a86:7523 QinHeng Electronics CH340

# 2. Driver đã load không?
lsmod | grep ch341

# 3. Kernel log
dmesg | tail -20
# Nên thấy: usb 5-1: ch341-uart converter now attached to ttyUSB0
```

**Fix:**

```bash
# Install CH340 driver
sudo apt install linux-modules-extra-$(uname -r)

# Reload driver
sudo modprobe ch341

# Verify
lsmod | grep ch341
```

---

### ❌ Vấn đề 2: Permission denied

**Triệu chứng:**
```
PermissionError: [Errno 13] Permission denied: '/dev/ttyUSB0'
```

**Fix:**

```bash
# Cách 1: Quick fix (tạm thời)
sudo chmod 666 /dev/ttyUSB0

# Cách 2: Permanent fix (khuyên dùng)
sudo usermod -a -G dialout $USER
# Logout và login lại để apply
```

---

### ❌ Vấn đề 3: Port bị chiếm

**Triệu chứng:**
```
SerialException: Could not open port /dev/ttyUSB0: Device or resource busy
```

**Kiểm tra:**

```bash
# Xem process nào đang dùng
sudo lsof /dev/ttyUSB0

# Hoặc
sudo fuser -v /dev/ttyUSB0
```

**Fix:**

```bash
# Kill process cũ
sudo pkill -f "server.py"

# Hoặc kill by PID
sudo kill -9 <PID>

# Script run.sh đã tự động làm việc này
```

---

### ❌ Vấn đề 4: RX LED sáng nhưng TX không sáng

**Nguyên nhân:** Slave không phản hồi

**Debug:**

```bash
# Test 1: RAW echo test
./debug.sh
# Chọn option 1

# Nếu TX LED sáng trong RAW test → Hardware OK
# Nếu TX LED không sáng → Lỗi hardware/wiring

# Test 2: Modbus debug
./debug.sh
# Chọn option 2

# Xem có gửi response không
```

**Xem thêm:** [DEBUG_GUIDE.md](DEBUG_GUIDE.md)

---

## 📊 Testing Guide

### Test 1: Port Detection

```bash
python3 port_detector.py
```

**Kết quả mong đợi:** Tìm thấy CH340 và hiển thị `/dev/ttyUSB0` (hoặc ttyUSB1)

---

### Test 2: RAW Echo Test (Hardware)

```bash
./debug.sh
# Chọn 1 (RAW Debug)
```

**Kết quả mong đợi:**
- 📥 RX LED sáng khi Master gửi data
- 📤 TX LED sáng khi Slave echo lại
- Terminal hiển thị HEX dump của data

**Ví dụ output:**
```
📥 RX [8 bytes] Total: 8
   HEX:   01 03 00 00 00 02 C4 0B
   ASCII: ........
📤 TX [8 bytes] Total: 8 (ECHO)
   → Đã gửi phản hồi!
```

---

### Test 3: Modbus Parser Test

```bash
./debug.sh
# Chọn 2 (Modbus Debug)
```

**Kết quả mong đợi:**
- Parse được Modbus RTU frames
- CRC check OK
- Tự động tạo và gửi response

**Ví dụ output:**
```
📥 Frame #1 [8 bytes]
   RAW: 01 03 00 00 00 02 C4 0B
   📊 Parsed:
      • slave_id: 1
      • function_code: 3
      • type: READ_HOLDING_REGISTERS
      ✅ crc_ok: True
   📤 Response sent [7 bytes]:
      01 03 04 12 34 56 78 A1 B2
      ✅ TX LED should blink now!
```

---

### Test 4: Full Server Test

```bash
./run.sh
```

**Kết quả mong đợi:**
```
[INFO] ✅ Tự động detect: /dev/ttyUSB0
[INFO] Starting RS485 Modbus RTU slave simulator on /dev/ttyUSB0 @ 115200bps
```

**Test từ Master:**
```bash
# Read Holding Registers từ Motor (0x04)
# Function Code: 0x03
# Start Address: 0x0000
# Quantity: 2 registers

# Expected: Nhận được response với data
```

---

## 🎯 Debug Flow Chart

```
1. Kiểm tra CH340 detect
   ├─ OK ───────────────────────────┐
   └─ FAIL → Fix driver/permission  │
                                     ▼
2. Test RAW echo (Hardware)         
   ├─ TX LED sáng → Hardware OK ────┐
   └─ TX không sáng → Check wiring  │
                                     ▼
3. Test Modbus parser               
   ├─ Response OK → Modbus OK ──────┐
   └─ Fail → Check baudrate/parity  │
                                     ▼
4. Run full server                  
   ├─ Works → ✅ DONE!
   └─ Fail → Check pymodbus version
```

---

## 🔄 So sánh Version

| Feature | v1.0 (cũ) | v2.0 (mới) |
|---------|-----------|------------|
| Port config | ❌ Hardcode `/dev/ttyUSB1` | ✅ Auto-detect CH340 |
| Error handling | ❌ Generic error | ✅ Clear error messages |
| Debug tools | ❌ Không có | ✅ 4 debug modes |
| Port verify | ❌ Không kiểm tra | ✅ Auto verify |
| Documentation | ❌ Minimal | ✅ Chi tiết với icons |

---

## 📚 References

- **Pymodbus Documentation:** https://pymodbus.readthedocs.io/
- **CH340 Linux Driver:** https://github.com/torvalds/linux/blob/master/drivers/usb/serial/ch341.c
- **Modbus RTU Spec:** https://modbus.org/docs/Modbus_over_serial_line_V1_02.pdf
- **Debug Guide:** [DEBUG_GUIDE.md](DEBUG_GUIDE.md)

---

## 🤝 Support

Nếu gặp vấn đề, cung cấp:

1. ✅ Output của `python3 port_detector.py`
2. ✅ Output của `lsusb | grep CH340`
3. ✅ Output của debug tool (RAW hoặc Modbus)
4. ✅ Screenshot LED (nếu có thể)

---

## 📝 Changelog

### v2.0 (2025-01-07)
- ✅ Auto-detect CH340 USB converter
- ✅ Thêm `port_detector.py`
- ✅ Thêm debug tools (`debug.sh`, `debug_raw.py`, `debug_modbus.py`)
- ✅ Cải thiện error handling
- ✅ Documentation đầy đủ với icons và màu sắc
- ✅ Không còn hardcode port

### v1.0 (trước đó)
- ✅ Basic Modbus RTU server
- ✅ Support 4 module types
- ❌ Hardcode port `/dev/ttyUSB1`

---

**🔧 Tạo bởi:** OHT-50 EMBED Team  
**📅 Cập nhật:** 2025-01-07  
**📦 Version:** 2.0
