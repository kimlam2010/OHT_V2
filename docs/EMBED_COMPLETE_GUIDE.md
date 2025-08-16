# EMBED Complete Guide - OHT-50

## 📋 **Tổng quan**
Hướng dẫn hoàn chỉnh EMBED cho OHT-50, bao gồm cấu hình UART1 RS485 và GPIO.

**Phiên bản:** v2.0  
**Ngày cập nhật:** 2025-01-27  
**Trạng thái:** ✅ Complete

**📁 Vị trí EMBED:** `/home/orangepi/OHT-50/EMBED/`

---

## 🎯 **Vai trò EMBED**

### **Mục tiêu**
- Nền tảng phần cứng ổn định cho OHT-50
- Giao tiếp RS485 xác định qua UART1 (chân 46, 47)
- HAL/API rõ ràng cho firmware
- Lỗi được phát hiện và khôi phục minh bạch

### **Phạm vi trách nhiệm**
- **Hardware bring-up**: UART1, GPIO, RS485 transceiver
- **Driver level**: HAL UART/RS485, GPIO/Relay
- **System integration**: udev rules, device symlinks
- **Validation**: Hardware tests, BER/latency measurement

---

## 🔧 **Cấu hình UART1 với chân 46, 47**

### **Bước 1: Tạo Device Tree Overlay**
```bash
# Chuyển đến folder EMBED
cd /home/orangepi/OHT-50/EMBED

# Tạo file uart1_46_47.dts
cat > uart1_46_47.dts << 'EOF'
/dts-v1/;
/plugin/;

/ {
    compatible = "rockchip,rk3588";

    fragment@0 {
        target = <&uart1>;
        __overlay__ {
            status = "okay";
            pinctrl-names = "default";
            pinctrl-0 = <&uart1_46_47_xfer>;
        };
    };

    fragment@1 {
        target = <&pinctrl>;
        __overlay__ {
            uart1_46_47_xfer: uart1-46-47-xfer {
                rockchip,pins = 
                    <1 17 1 &pcfg_pull_up>,  /* GPIO1_D1 (Pin 46) - TX */
                    <1 16 1 &pcfg_pull_up>;  /* GPIO1_D0 (Pin 47) - RX */
            };
        };
    };
};
EOF
```

### **Bước 2: Compile và Install**
```bash
# Compile overlay
sudo dtc -@ -I dts -O dtb -o uart1_46_47.dtbo uart1_46_47.dts

# Copy vào /boot
sudo cp uart1_46_47.dtbo /boot/overlays/

# Thêm vào /boot/orangepiEnv.txt
sudo sed -i 's/overlays=/overlays=uart1_46_47 /' /boot/orangepiEnv.txt

# Reboot
sudo reboot
```

### **Bước 3: Kiểm tra cấu hình**
```bash
# Kiểm tra UART1
ls -la /dev/ttyS1

# Kiểm tra GPIO 46, 47
gpio readall | grep -E "(46|47)"

# Test UART1
cd /home/orangepi/OHT-50/EMBED
sudo python3 test_uart1_pins_46_47.py
```

---

## 📊 **Kết quả cấu hình**

### **UART1 Configuration**
- **Device**: `/dev/ttyS1`
- **Baudrate**: 115200
- **TX**: Chân 46 (GPIO1_D1) - ALT10 mode
- **RX**: Chân 47 (GPIO1_D0) - ALT10 mode
- **Status**: ✅ Hoạt động bình thường

### **GPIO Configuration**
- **GPIO 46**: OUT mode (TX) - ALT10 function
- **GPIO 47**: IN mode (RX) - ALT10 function
- **Status**: ✅ Mapping đúng UART1

---

## 🧪 **Test Scripts**

### **1. Test UART1 cơ bản**
```bash
cd /home/orangepi/OHT-50/EMBED
sudo python3 test_uart1_pins_46_47.py
```

### **2. Gửi UART1 liên tục**
```bash
sudo python3 send_uart1.py
```

### **3. Monitor UART1**
```bash
sudo python3 monitor_uart1.py
```

### **4. Test với module**
```bash
sudo python3 test_with_module.py
```

---

## 📁 **File Structure**

```
OHT-50/
├── docs/
│   └── EMBED_COMPLETE_GUIDE.md          # Tài liệu này
└── EMBED/                               # 📁 FOLDER EMBED
    ├── README.md                        # README cho EMBED
    ├── uart1_46_47.dts                  # Device tree overlay
    ├── uart1_46_47.dtbo                 # Compiled overlay
    ├── test_uart1_pins_46_47.py         # Test UART1 cơ bản
    ├── send_uart1.py                    # Gửi UART1 liên tục
    ├── monitor_uart1.py                 # Monitor UART1
    ├── test_with_module.py              # Test với module
    ├── hal_rs485.py                     # HAL RS485
    ├── setup_udev_rs485.sh              # Setup udev rules
    └── continuous_tx_test.py            # Test liên tục
```

---

## 🔗 **Tài liệu tham khảo**

- [Orange Pi 5B Pinout](https://wiki.orange-pi.com/Orange_Pi_5B)
- [RK3588 UART Documentation](https://www.rock-chips.com/a/en/products/RK35_Series/2022/0829/20701.html)
- [RS485 Wiring Guide](https://www.maxlinear.com/ds/MAX485.pdf)

---

**Changelog:**
- v2.0 (2025-01-27): Complete guide với UART1 chân 46, 47, moved to EMBED folder
- v1.0 (2025-08-16): Initial version
