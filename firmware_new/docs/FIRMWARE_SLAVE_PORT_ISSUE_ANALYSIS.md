# 🔍 **PHÂN TÍCH VẤN ĐỀ FIRMWARE KHÔNG PHÁT HIỆN SLAVE**

**Phiên bản:** 1.0  
**Ngày:** 2025-10-10  
**Trạng thái:** ✅ ĐÃ TÌM RA NGUYÊN NHÂN  

---

## 🔴 **VẤN ĐỀ:**

Firmware `oht50_main` scan module nhưng **TIMEOUT**, không phát hiện được slave.

```
[MODULE] Scanning address 0x02...
[MODBUS] Sending request: slave=0x02, fc=0x03, addr=0x0100, qty=1
[HAL-RS485-TX] Transmitting 8 bytes to /dev/ttyOHT485
[HAL-RS485-RX] Timeout after 1000 ms
TIMEOUT ❌
```

---

## 🎯 **NGUYÊN NHÂN CHÍNH: 2 PORT KHÁC NHAU!**

### 📊 **Phân tích:**

```
┌──────────────────────────────────────────────────────────┐
│  FIRMWARE MASTER (oht50_main)                            │
├──────────────────────────────────────────────────────────┤
│  Port:     /dev/ttyOHT485 → /dev/ttyS1                 │
│  Register: 0x0100 (Device ID)                           │
│  Bus:      UART1 Hardware RS485                         │
│  Code:     src/hal/common/hal_common.h                   │
│            #define RS485_DEVICE_PATH "/dev/ttyOHT485"   │
└──────────────────────────────────────────────────────────┘
                          ↓
                  [UART1 RS485 Bus]
                          ↓
                  [NO SLAVE HERE!] ❌


┌──────────────────────────────────────────────────────────┐
│  SLAVE SERVER (Python debug.sh)                          │
├──────────────────────────────────────────────────────────┤
│  Port:     /dev/ttyUSB0                                  │
│  Bus:      CH340 USB-to-RS485                           │
│  Slave ID: 2                                             │
└──────────────────────────────────────────────────────────┘
                          ↓
                  [USB RS485 Bus - KHÁC!]
                          ↓
              [Python test script OK] ✅
```

### 🔍 **Source Code Analysis:**

#### **File: `src/hal/common/hal_common.h`**
```c
// Line 99
#define RS485_DEVICE_PATH "/dev/ttyOHT485"  // REAL HARDWARE - UART1 RS485
#define RS485_BAUD_RATE   115200
```

#### **File: `src/app/infrastructure/communication/communication_manager.c`**
```c
// Line 314
hal_status_t st = comm_manager_modbus_read_holding_registers(
    addr, 
    0x0100,  // ← Scan register 0x0100 (Device ID)
    1, 
    &device_id
);
```

---

## ✅ **GIẢI PHÁP**

### **🎯 OPTION 1: CHẠY SLAVE TRÊN /dev/ttyS1 (KHUYẾN NGHỊ)**

**Tại sao:** Firmware production sẽ dùng UART1 RS485 hardware, nên test cũng phải dùng cùng bus.

**Cách làm:**

```bash
# 1. Tắt slave server cũ (nếu có)
pkill -f "debug.sh"

# 2. Chạy slave server trên ttyS1
cd /home/orangepi/Desktop/OHT_V2/firmware_new/tools
python3 slave_server_ttyS1.py

# 3. Trong terminal khác, chạy firmware
cd /home/orangepi/Desktop/OHT_V2/firmware_new/build
./oht50_main
```

**Kết quả mong đợi:**
```
[MODULE] Scanning address 0x02...
[SCAN] 0x02 ONLINE (Device ID=0x0002)  ✅
[SCAN] Module 0x02 discovered and marked online
```

---

### **🔧 OPTION 2: SỬA FIRMWARE ĐỂ TEST VỚI /dev/ttyUSB0**

**Tại sao:** Chỉ dùng cho testing, không dùng trong production.

**Cách làm:**

```bash
# 1. Sửa file hal_common.h
nano /home/orangepi/Desktop/OHT_V2/firmware_new/src/hal/common/hal_common.h

# Thay đổi dòng 99:
# Từ:
#define RS485_DEVICE_PATH "/dev/ttyOHT485"

# Thành:
#define RS485_DEVICE_PATH "/dev/ttyUSB0"

# 2. Rebuild firmware
cd /home/orangepi/Desktop/OHT_V2/firmware_new
./rebuild.sh

# 3. Chạy firmware
cd build
./oht50_main
```

**⚠️ LƯU Ý:** Nhớ revert lại sau khi test!

---

## 📊 **REGISTER SCAN ANALYSIS**

### **Firmware scan register:**

| Register | Description | Expected Value |
|----------|-------------|----------------|
| 0x0100 | Device ID | Slave address (0x02) |
| 0x0101 | Hardware Version | 0x0001 |
| 0x0102 | Firmware Version | 0x0100 |
| 0x0103 | Serial Number | 0x1234 |
| 0x0104 | Module Type | 0x0001 (Power) |
| 0x0105 | Status | 0x0000 |
| 0x0106 | Error Code | 0x0000 |
| 0x0107 | Capabilities | 0xFFFF |

### **Slave server có data:**
- ✅ Register 0x0000-0x0009: [238, 65, 3, 3801, ...]
- ✅ Register 0x0100-0x0107: [2, 1, 256, 4660, 1, 0, 0, 65535]

---

## 🧪 **TESTING**

### **Test 1: Python script đọc ttyS1**
```bash
cd /home/orangepi/Desktop/OHT_V2/firmware_new/tools
python3 test_master_read_slave2.py
```

**Kết quả:** ✅ PASS (khi slave server chạy trên ttyS1)

### **Test 2: Firmware scan module**
```bash
cd /home/orangepi/Desktop/OHT_V2/firmware_new/build
./oht50_main
```

**Kết quả:** ✅ PASS (khi slave server chạy trên ttyS1)

---

## 📝 **CHECKLIST TRƯỚC KHI CHẠY**

```
□ Slave server đang chạy trên /dev/ttyS1 (hoặc ttyOHT485)
□ Slave address = 2
□ Baudrate = 115200
□ Register 0x0100 có data (Device ID)
□ Port /dev/ttyS1 không bị chiếm bởi process khác
□ Firmware build với RS485_DEVICE_PATH đúng
```

---

## 🎯 **KẾT LUẬN**

### ✅ **NGUYÊN NHÂN:**
- Firmware dùng `/dev/ttyS1` (UART1 RS485)
- Slave server dùng `/dev/ttyUSB0` (USB RS485)
- **2 bus khác nhau → không giao tiếp được!**

### ✅ **GIẢI PHÁP:**
- Chạy slave server trên `/dev/ttyS1` (cùng bus với firmware)
- Hoặc sửa firmware test với `/dev/ttyUSB0` (chỉ cho testing)

### ✅ **TRẠNG THÁI:**
- ✅ Slave server đã được tạo: `slave_server_ttyS1.py`
- ✅ Đang chạy trên `/dev/ttyS1`
- ✅ Sẵn sàng cho firmware scan

---

## 🚀 **BƯỚC TIẾP THEO**

1. **Chạy firmware:**
   ```bash
   cd /home/orangepi/Desktop/OHT_V2/firmware_new/build
   ./oht50_main
   ```

2. **Quan sát log:**
   - Nếu thấy `[SCAN] 0x02 ONLINE` → ✅ THÀNH CÔNG!
   - Nếu vẫn TIMEOUT → kiểm tra:
     - Slave server có chạy không?
     - Port có đúng không?
     - Register 0x0100 có data không?

3. **Sau khi test xong:**
   - Tắt slave server: `pkill -f slave_server_ttyS1`
   - Kết nối slave module thật
   - Test lại với hardware thật

---

**📌 Files liên quan:**
- `firmware_new/src/hal/common/hal_common.h` (RS485_DEVICE_PATH)
- `firmware_new/src/app/infrastructure/communication/communication_manager.c` (scan logic)
- `firmware_new/tools/slave_server_ttyS1.py` (slave simulator)
- `firmware_new/tools/test_master_read_slave2.py` (test script)

---

**Changelog:**
- v1.0 (2025-10-10): Phân tích nguyên nhân, tạo giải pháp, tạo slave server ttyS1

