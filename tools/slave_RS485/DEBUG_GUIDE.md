# 🔍 RS485 DEBUG GUIDE - KHÔNG PHẢN HỒI (TX LED KHÔNG SÁNG)

## 🚨 Vấn đề hiện tại

- ✅ **RX LED sáng** → Slave đang nhận được data từ Master
- ❌ **TX LED không sáng** → Slave không gửi phản hồi về Master

---

## 🛠️ CÁCH DEBUG (3 BƯỚC)

### 📍 BƯỚC 1: Kiểm tra Port Information

```bash
./debug.sh
# Chọn option 4 (Port Info)
```

**Kiểm tra:**
- ✅ Port tồn tại (`/dev/ttyUSB1` hoặc `/dev/ttyS1`)
- ✅ Permission OK (có thể đọc/ghi)
- ✅ Không bị process khác chiếm

---

### 📍 BƯỚC 2: RAW Debug (Echo Test)

```bash
./debug.sh
# Chọn option 1 (RAW Debug)
```

**Script này sẽ:**
1. 📥 Hiển thị MỌI byte nhận được (HEX + ASCII)
2. 📤 **TỰ ĐỘNG ECHO** lại data nhận được
3. 📊 Đếm số bytes RX/TX

**Kết quả mong đợi:**
- Nếu **TX LED sáng** → Hardware RS485 OK ✅
- Nếu **TX LED vẫn không sáng** → Lỗi hardware hoặc wiring ❌

**Nếu TX LED sáng trong RAW test:**
→ Vấn đề nằm ở **Pymodbus server** → Chuyển sang BƯỚC 3

---

### 📍 BƯỚC 3: Modbus Debug

```bash
./debug.sh
# Chọn option 2 (Modbus Debug)
```

**Script này sẽ:**
1. 📥 Parse Modbus RTU frames chi tiết
2. ✅ Kiểm tra CRC
3. 📤 **TỰ ĐỘNG TẠO RESPONSE** hợp lệ
4. 📊 Hiển thị hex dump của response

**Kết quả mong đợi:**
- Nếu **TX LED sáng** → Modbus logic OK ✅
- Nếu **TX LED vẫn không sáng** → Xem phần "NGUYÊN NHÂN" bên dưới

---

## 🔍 NGUYÊN NHÂN THƯỜNG GẶP

### ❌ 1. Pymodbus Server Timeout

**Triệu chứng:** RX LED sáng nhưng không có response

**Nguyên nhân:**
```python
StartSerialServer(
    context=context,
    timeout=1,  # ⚠️ Quá ngắn?
    ...
)
```

**Giải pháp:**
```python
timeout=3,  # Tăng lên 3 giây
```

---

### ❌ 2. Slave Address Không Khớp

**Triệu chứng:** Master hỏi slave ID 0x01 nhưng server chỉ có 0x02, 0x03, 0x04, 0x05

**Kiểm tra:**
```bash
./debug.sh
# Option 2, xem "slave_id" trong parsed data
```

**Giải pháp:** Đảm bảo slave ID match với Master request

---

### ❌ 3. Baudrate/Parity Không Khớp

**Triệu chứng:** CRC error hoặc không parse được frame

**Kiểm tra cấu hình:**
```python
# Master
BAUDRATE = 115200
PARITY = 'N'
STOPBITS = 1

# Slave (phải giống!)
BAUDRATE = 115200
PARITY = 'N' 
STOPBITS = 1
```

---

### ❌ 4. Pymodbus Version Issue

**Triệu chứng:** Server start nhưng không phản hồi

**Kiểm tra:**
```bash
pip list | grep pymodbus
# Nên là: pymodbus==3.6.6
```

**Giải pháp:**
```bash
pip install pymodbus==3.6.6 --force-reinstall
```

---

### ❌ 5. Serial Buffer Full

**Triệu chứng:** TX LED sáng rất yếu hoặc delay lâu

**Giải pháp:**
```python
ser = serial.Serial(
    ...
    write_timeout=1.0,  # Đặt timeout cho write
)

# Hoặc flush buffer
ser.reset_output_buffer()
ser.reset_input_buffer()
```

---

## 📊 SO SÁNH KẾT QUẢ

| Test | RX LED | TX LED | Kết luận |
|------|--------|--------|----------|
| **Raw Echo** | ✅ | ❌ | Lỗi hardware wiring hoặc RS485 chip |
| **Raw Echo** | ✅ | ✅ | Hardware OK, lỗi ở Modbus logic |
| **Modbus Debug** | ✅ | ✅ | Modbus OK, lỗi ở Pymodbus server |
| **All Tests** | ❌ | ❌ | Lỗi wiring hoặc port config |

---

## 🔧 FIX PYMODBUS SERVER

Nếu RAW và Modbus Debug đều OK nhưng `server.py` không hoạt động:

### Fix 1: Thêm Debug Logging

```python
# Thêm vào đầu server.py
import logging
logging.basicConfig(
    level=logging.DEBUG,
    format='[%(asctime)s] [%(levelname)s] %(name)s: %(message)s'
)
logging.getLogger('pymodbus').setLevel(logging.DEBUG)
logging.getLogger('pymodbus.server').setLevel(logging.DEBUG)
```

### Fix 2: Tăng Timeout

```python
StartSerialServer(
    context=context,
    framer=ModbusRtuFramer,
    port=port,
    timeout=3,  # ← Tăng lên
    ...
)
```

### Fix 3: Kiểm tra Serial Config

```python
# Thêm vào sau StartSerialServer
import serial
ser = serial.Serial(port, BAUD)
print(f"Serial config: {ser}")
print(f"In waiting: {ser.in_waiting}")
print(f"Out waiting: {ser.out_waiting}")
```

---

## 🎯 CHECKLIST CUỐI CÙNG

- [ ] Port tồn tại và có quyền đọc/ghi
- [ ] Không có process nào chiếm port
- [ ] Baudrate/Parity/Stopbits khớp với Master
- [ ] Slave address match với request
- [ ] RAW echo test → TX LED sáng
- [ ] Modbus debug → TX LED sáng khi có request hợp lệ
- [ ] Pymodbus version = 3.6.6
- [ ] Timeout đủ lớn (>= 1s)

---

## 💡 SUPPORT

Nếu vẫn không giải quyết được, cung cấp:

1. Output của `./debug.sh` option 4 (Port Info)
2. Output của RAW Debug khi Master gửi request
3. Output của Modbus Debug với 1 frame hoàn chỉnh
4. Ảnh chụp LED hoặc oscilloscope trace (nếu có)

---

**🔧 Tạo bởi:** OHT-50 EMBED Team  
**📅 Ngày:** 2025-01-07

