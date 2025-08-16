# 📢 THÔNG BÁO CHO PM VÀ TEAM EMBED

## 🎯 **Thông báo quan trọng**

**Ngày:** 2025-01-27  
**Từ:** EMBED Team  
**Đến:** PM và tất cả thành viên EMBED

---

## 📁 **Vị trí EMBED đã thay đổi**

### **Vị trí cũ:**
- File EMBED nằm rải rác trong `/home/orangepi/OHT-50/`

### **Vị trí mới:**
- **📁 Folder EMBED:** `/home/orangepi/OHT-50/EMBED/`
- **📋 Tài liệu:** `/home/orangepi/OHT-50/docs/EMBED_COMPLETE_GUIDE.md`
- **📊 Báo cáo:** `/home/orangepi/OHT-50/EMBED_FINAL_REPORT.md`

---

## 📋 **File đã được di chuyển**

### **Device Tree Overlays**
- ✅ `uart1_46_47.dts` → `/home/orangepi/OHT-50/EMBED/`
- ✅ `uart1_46_47.dtbo` → `/home/orangepi/OHT-50/EMBED/`

### **Test Scripts**
- ✅ `test_uart1_pins_46_47.py` → `/home/orangepi/OHT-50/EMBED/`
- ✅ `send_uart1.py` → `/home/orangepi/OHT-50/EMBED/`
- ✅ `monitor_uart1.py` → `/home/orangepi/OHT-50/EMBED/`
- ✅ `test_with_module.py` → `/home/orangepi/OHT-50/EMBED/`
- ✅ `continuous_tx_test.py` → `/home/orangepi/OHT-50/EMBED/`

### **HAL & Setup**
- ✅ `hal_rs485.py` → `/home/orangepi/OHT-50/EMBED/`
- ✅ `setup_udev_rs485.sh` → `/home/orangepi/OHT-50/EMBED/`

---

## 🔧 **Cách sử dụng mới**

### **1. Truy cập EMBED folder**
```bash
cd /home/orangepi/OHT-50/EMBED
```

### **2. Chạy test scripts**
```bash
# Test UART1 cơ bản
sudo python3 test_uart1_pins_46_47.py

# Gửi UART1 liên tục
sudo python3 send_uart1.py

# Monitor UART1
sudo python3 monitor_uart1.py
```

### **3. Setup udev rules**
```bash
sudo ./setup_udev_rs485.sh
```

---

## 📊 **Trạng thái hiện tại**

### **✅ Hoàn thành**
- UART1 mapping chân 46 (TX), 47 (RX)
- Device tree overlay hoạt động
- HAL RS485 interface
- Test scripts đầy đủ
- Udev rules setup
- **Organization**: Tất cả file đã được tổ chức gọn gàng

### **🎯 Sẵn sàng cho**
- RS485 communication với module OHT-50
- Firmware development
- Hardware integration

---

## 📞 **Liên hệ**

**EMBED Team:**
- **Vai trò**: Hardware bring-up, low-level drivers
- **Focus**: UART1 RS485, GPIO control, HAL interface
- **Status**: Complete và sẵn sàng sử dụng
- **Vị trí làm việc**: `/home/orangepi/OHT-50/EMBED/`

---

## ⚠️ **Lưu ý quan trọng**

1. **Tất cả file EMBED mới phải được tạo trong folder `/home/orangepi/OHT-50/EMBED/`**
2. **Không tạo file EMBED ở nơi khác**
3. **Luôn cập nhật README.md trong folder EMBED khi có thay đổi**
4. **Thông báo cho team khi có file mới**

---

**Changelog:**
- v2.0 (2025-01-27): Moved to EMBED folder, complete organization
- v1.0 (2025-08-16): Initial RS485 integration
