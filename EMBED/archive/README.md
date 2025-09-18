# EMBED Archive - Development & Test Files

## 📋 **Mục đích**
Thư mục archive chứa các file development và test đã được di chuyển từ thư mục chính để giữ production folder gọn gàng.

## 📁 **Cấu trúc Archive**

### **tests/** - Test Scripts Cần Thiết
- `test_uart1_pins_46_47.py` - Test UART1 cơ bản với chân 46, 47
- `test_modbus_simple.py` - Test Modbus RTU đơn giản
- `test_oht_gpio_correct.py` - Test GPIO với mapping đúng
- `test_with_module.py` - Test communication với module thật

### **tools/** - Development Tools
- `continuous_tx_test.py` - Tool test transmission liên tục
- `monitor_uart1.py` - Tool monitor UART1 data
- `send_uart1.py` - Tool gửi UART1 liên tục

### **old_docs/** - Documentation Cũ
- `EMBED_FIRMWARE_ANALYSIS.md` - Analysis firmware cũ
- `EMBED_TEST_SUMMARY.md` - Test summary cũ

## 🧪 **Cách Sử Dụng Archive Files**

### **Chạy Test Scripts:**
```bash
cd /home/orangepi/Desktop/OHT_V2/EMBED/archive/tests

# Test UART1 cơ bản
sudo python3 test_uart1_pins_46_47.py

# Test Modbus RTU
sudo python3 test_modbus_simple.py

# Test GPIO
sudo python3 test_oht_gpio_correct.py

# Test với module thật
sudo python3 test_with_module.py
```

### **Sử Dụng Development Tools:**
```bash
cd /home/orangepi/Desktop/OHT_V2/EMBED/archive/tools

# Monitor UART1
sudo python3 monitor_uart1.py

# Gửi UART1 liên tục
sudo python3 send_uart1.py

# Test transmission liên tục
sudo python3 continuous_tx_test.py
```

## ⚠️ **Lưu Ý**
- Các file trong archive chỉ dùng cho development và troubleshooting
- Không cần thiết cho production deployment
- Có thể xóa toàn bộ archive nếu không cần development

## 🎯 **Production Files**
Các file production chính nằm trong thư mục `/home/orangepi/Desktop/OHT_V2/EMBED/`:
- `README.md` - Documentation chính
- `QUICK_START.md` - Hướng dẫn nhanh
- `setup_complete_system.sh` - Setup tự động
- `setup_oht_gpio_correct.sh` - Setup GPIO
- `setup_udev_rs485.sh` - Setup udev
- `hal_rs485.py` - HAL interface
- `uart1_46_47.dts/.dtbo` - Device tree files

---
**Ngày tạo:** $(date)  
**Mục đích:** Dọn dẹp EMBED folder cho production readiness
