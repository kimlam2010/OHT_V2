# 🚀 OHT-50 EMBED - QUICK START GUIDE

## ⚡ **Setup Nhanh (5 phút)**

### **🔥 One-Command Setup:**
```bash
cd /home/orangepi/Desktop/OHT_V2/EMBED
sudo ./setup_complete_system.sh
sudo reboot
```

### **✅ Sau Reboot - Verification:**
```bash
cd /home/orangepi/Desktop/OHT_V2/EMBED

# 1. Kiểm tra GPIO
gpio readall | grep -E "(54|35|28|29|58|59|131|132|46|47)"

# 2. Kiểm tra UART1
ls -la /dev/ttyS1

# 3. Setup udev rules
sudo ./setup_udev_rs485.sh

# 4. Test hệ thống
sudo python3 test_uart1_pins_46_47.py
```

---

## 🎯 **Manual Setup (nếu cần)**

### **Bước 1: GPIO Configuration**
```bash
sudo ./setup_oht_gpio_correct.sh
```

### **Bước 2: Dependencies**
```bash
sudo pip3 install pyserial
```

### **Bước 3: Device Tree**
```bash
sudo cp uart1_46_47.dtbo /boot/dtb-6.1.43-rockchip-rk3588/rockchip/overlay/
sudo bash -c "echo 'overlays=uart1_46_47' >> /boot/orangepiEnv.txt"
sudo reboot
```

### **Bước 4: Test**
```bash
sudo python3 test_uart1_pins_46_47.py
sudo python3 test_modbus_simple.py
```

---

## 🧪 **Quick Tests**

### **LED Test:**
```bash
echo 1 | sudo tee /sys/class/gpio/gpio54/value  # LED ON
echo 0 | sudo tee /sys/class/gpio/gpio54/value  # LED OFF
```

### **Relay Test:**
```bash
echo 1 | sudo tee /sys/class/gpio/gpio131/value  # RELAY ON
echo 0 | sudo tee /sys/class/gpio/gpio131/value  # RELAY OFF
```

### **E-Stop Check:**
```bash
cat /sys/class/gpio/gpio59/value  # Should be 1 (Safe)
```

---

## 📊 **Expected Results**

### **GPIO Readall:**
```
GPIO 54: OUT 0 (LED_POWER)
GPIO 35: OUT 0 (LED_SYSTEM)  
GPIO 28: OUT 0 (LED_COMM)
GPIO 29: OUT 0 (LED_NETWORK)
GPIO 58: OUT 0 (LED_ERROR)
GPIO 59: IN  1 (E-STOP - Safe)
GPIO131: OUT 0 (RELAY1)
GPIO132: OUT 0 (RELAY2)
GPIO 46: OUT 0 (UART1_TX)
GPIO 47: IN  1 (UART1_RX)
```

### **Devices:**
```
/dev/ttyS1      - UART1 device
/dev/ttyOHT485  - RS485 symlink (after udev setup)
```

---

## 🚨 **Troubleshooting**

### **GPIO không hoạt động:**
```bash
sudo ./setup_oht_gpio_correct.sh
gpio readall
```

### **UART1 không tồn tại:**
```bash
cat /boot/orangepiEnv.txt | grep overlays
sudo reboot
```

### **Permission denied:**
```bash
sudo chmod 666 /sys/class/gpio/gpio*/value
```

### **Python module error:**
```bash
sudo pip3 install pyserial
```

---

## 📞 **Support**

**EMBED Team Status:** ✅ **PRODUCTION READY**  
**Platform:** Orange Pi 5B (RK3588)  
**Version:** 3.0  
**Hardware:** 100% Validated  

**🎉 Ready for Firmware Integration!**
