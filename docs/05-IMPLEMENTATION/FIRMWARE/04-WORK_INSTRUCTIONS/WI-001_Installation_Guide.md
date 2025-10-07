# 🔧 Hướng Dẫn Cài Đặt OHT-50 Firmware

**Version:** 1.0.1  
**Last Updated:** 2025-10-07  
**Platform:** Orange Pi 5B (RK3588)

---

## 📋 **MỤC LỤC**

1. [Yêu Cầu Hệ Thống](#yêu-cầu-hệ-thống)
2. [Chuẩn Bị Môi Trường](#chuẩn-bị-môi-trường)
3. [Cài Đặt Phụ Thuộc](#cài-đặt-phụ-thuộc)
4. [Build Firmware](#build-firmware)
5. [Cấu Hình Hardware](#cấu-hình-hardware)
6. [Triển Khai Lên Thiết Bị](#triển-khai-lên-thiết-bị)
7. [Xác Minh Cài Đặt](#xác-minh-cài-đặt)
8. [Troubleshooting](#troubleshooting)

---

## 📌 **YÊU CẦU HỆ THỐNG**

### **Hardware Requirements**

| **Component** | **Specification** | **Required** |
|---------------|-------------------|--------------|
| 🖥️ **Board** | Orange Pi 5B (RK3588) | ✅ Bắt buộc |
| 💾 **RAM** | 4GB minimum, 8GB khuyến nghị | ✅ Bắt buộc |
| 💽 **Storage** | 16GB eMMC/SD Card minimum | ✅ Bắt buộc |
| 📡 **RS485** | UART1 `/dev/ttyOHT485` @ 115200 baud | ✅ Bắt buộc |
| 🔌 **GPIO** | GPIO1_D3 (Relay1), GPIO1_D2 (Relay2) | ✅ Bắt buộc |
| 💡 **LED** | 5x status LEDs (Power, System, Comm, Network, Error) | ✅ Bắt buộc |
| 🛑 **E-Stop** | Hardware E-Stop button | ✅ Bắt buộc |
| 🌐 **Network** | Ethernet 10/100/1000 hoặc WiFi 5G/2.4G | ✅ Bắt buộc |
| 🔵 **LiDAR** | RPLiDAR A1/A2 (tùy chọn) | ⚠️ Tùy chọn |

### **Software Requirements**

| **Software** | **Version** | **Purpose** |
|--------------|-------------|-------------|
| 🐧 **Linux** | Ubuntu 20.04/22.04 hoặc Armbian | OS cho Orange Pi 5B |
| 🔧 **GCC** | 9.0+ | C11 compiler |
| 🏗️ **CMake** | 3.16+ | Build system |
| 📦 **Git** | 2.0+ | Version control |
| 🔐 **OpenSSL** | 1.1.1+ | SSL/TLS support |
| 🧵 **pthread** | POSIX threads | Multi-threading |
| 📊 **libm** | Math library | Mathematical functions |

---

## 🛠️ **CHUẨN BỊ MÔI TRƯỜNG**

### **Bước 1: Cài Đặt Hệ Điều Hành**

#### **1.1. Flash Armbian/Ubuntu lên Orange Pi 5B**

```bash
# Download Armbian cho Orange Pi 5B
wget https://github.com/armbian/build/releases/download/latest/Armbian_XX.XX_Orangepi5b.img.xz

# Flash lên SD Card (thay /dev/sdX bằng đường dẫn SD Card)
xz -d Armbian_XX.XX_Orangepi5b.img.xz
sudo dd if=Armbian_XX.XX_Orangepi5b.img of=/dev/sdX bs=4M status=progress
sync
```

#### **1.2. Boot và Cấu Hình Ban Đầu**

```bash
# Đăng nhập lần đầu (mặc định)
# Username: root
# Password: 1234

# Đổi password root và tạo user mới
passwd
adduser oht50

# Update hệ thống
apt update && apt upgrade -y
```

### **Bước 2: Cài Đặt SSH (Tùy chọn)**

```bash
# Enable SSH
systemctl enable ssh
systemctl start ssh

# Cho phép SSH qua firewall
ufw allow 22/tcp
ufw enable
```

---

## 📦 **CÀI ĐẶT PHỤ THUỘC**

### **Bước 1: Cài Đặt Build Tools**

```bash
# Update package list
sudo apt update

# Cài đặt build essentials
sudo apt install -y \
    build-essential \
    gcc \
    g++ \
    cmake \
    make \
    git \
    pkg-config

# Xác nhận version
gcc --version      # Should be >= 9.0
cmake --version    # Should be >= 3.16
```

### **Bước 2: Cài Đặt Libraries**

```bash
# Cài đặt OpenSSL development
sudo apt install -y \
    libssl-dev \
    libcrypto++-dev

# Cài đặt pthread (thường đã có sẵn)
sudo apt install -y libc6-dev

# Cài đặt math library (thường đã có sẵn)
sudo apt install -y libm-dev

# Cài đặt YAML parser (tùy chọn)
sudo apt install -y libyaml-dev
```

### **Bước 3: Cài Đặt Development Tools (Tùy chọn)**

```bash
# Cài đặt clang-format (code formatting)
sudo apt install -y clang-format

# Cài đặt clang-tidy (static analysis)
sudo apt install -y clang-tidy

# Cài đặt valgrind (memory debugging)
sudo apt install -y valgrind

# Cài đặt gdb (debugging)
sudo apt install -y gdb
```

---

## 🏗️ **BUILD FIRMWARE**

### **Bước 1: Clone Repository**

```bash
# Clone từ Git repository
git clone https://github.com/your-org/OHT-50-firmware.git
cd OHT-50-firmware/firmware_new

# Hoặc nếu đã có source code
cd /path/to/OHT_V2/firmware_new
```

### **Bước 2: Tạo Build Directory**

```bash
# Tạo build directory
mkdir -p build
cd build
```

### **Bước 3: Configure CMake**

#### **3.1. Build Debug (cho development)**

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Debug \
    -DENABLE_COVERAGE=ON \
    -DENABLE_SANITIZERS=ON \
    -DBUILD_TESTING=ON
```

#### **3.2. Build Release (cho production)**

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_STATIC_ANALYSIS=ON \
    -DENABLE_LIDAR=ON \
    -DENABLE_WIFI_AP=ON \
    -DENABLE_WEBSOCKET=OFF \
    -DENABLE_HTTPS=OFF
```

#### **3.3. Build Options**

| **Option** | **Default** | **Description** |
|------------|-------------|-----------------|
| `CMAKE_BUILD_TYPE` | Debug | Debug/Release/RelWithDebInfo |
| `BUILD_TESTING` | ON | Enable test suite |
| `ENABLE_COVERAGE` | OFF | Code coverage analysis |
| `ENABLE_SANITIZERS` | OFF | AddressSanitizer/UBSan |
| `ENABLE_STATIC_ANALYSIS` | OFF | clang-tidy analysis |
| `ENABLE_LIDAR` | ON | LiDAR support |
| `ENABLE_WIFI_AP` | ON | WiFi Access Point |
| `ENABLE_WEBSOCKET` | OFF | WebSocket support (DEPRECATED) |
| `ENABLE_HTTPS` | OFF | HTTPS support |

### **Bước 4: Build**

```bash
# Build với all CPU cores
make -j$(nproc)

# Hoặc build với verbose output
make VERBOSE=1
```

### **Bước 5: Verify Build**

```bash
# Check executable đã được tạo
ls -lh oht50_main

# Check file size (khoảng 2-5 MB)
du -h oht50_main

# Check dependencies
ldd oht50_main
```

---

## ⚙️ **CẤU HÌNH HARDWARE**

### **Bước 1: Cấu Hình UART1 cho RS485**

#### **1.1. Device Tree Overlay**

```bash
# Copy device tree overlay
sudo cp ../overlays/uart1_correct.dtbo /boot/dtb/rockchip/overlay/

# Edit /boot/armbianEnv.txt
sudo nano /boot/armbianEnv.txt

# Thêm dòng:
overlays=uart1_correct

# Reboot
sudo reboot
```

#### **1.2. Tạo udev rule cho /dev/ttyOHT485**

```bash
# Tạo udev rule
sudo nano /etc/udev/rules.d/99-oht-rs485.rules

# Thêm nội dung:
KERNEL=="ttyS1", SYMLINK+="ttyOHT485", MODE="0666", GROUP="dialout"

# Reload udev rules
sudo udevadm control --reload-rules
sudo udevadm trigger

# Verify
ls -l /dev/ttyOHT485
```

### **Bước 2: Cấu Hình GPIO cho Relays**

```bash
# Export GPIO (nếu cần)
echo 59 > /sys/class/gpio/export  # GPIO1_D3 (Relay1)
echo 58 > /sys/class/gpio/export  # GPIO1_D2 (Relay2)

# Set direction
echo out > /sys/class/gpio/gpio59/direction
echo out > /sys/class/gpio/gpio58/direction

# Test relay
echo 1 > /sys/class/gpio/gpio59/value  # Turn ON
echo 0 > /sys/class/gpio/gpio59/value  # Turn OFF
```

### **Bước 3: Cấu Hình Permissions**

```bash
# Thêm user vào dialout group (cho RS485)
sudo usermod -a -G dialout $USER

# Thêm user vào gpio group
sudo usermod -a -G gpio $USER

# Logout và login lại để apply
exit
```

---

## 🚀 **TRIỂN KHAI LÊN THIẾT BỊ**

### **Bước 1: Copy Executable**

```bash
# Copy từ build machine sang Orange Pi
scp build/oht50_main oht50@orangepi5b:/home/oht50/

# Hoặc nếu build trực tiếp trên Orange Pi
sudo cp build/oht50_main /usr/local/bin/
sudo chmod +x /usr/local/bin/oht50_main
```

### **Bước 2: Tạo Configuration Directory**

```bash
# Tạo config directory
sudo mkdir -p /etc/oht50
sudo chown oht50:oht50 /etc/oht50

# Copy config files
sudo cp config/modules.yaml /etc/oht50/
sudo cp config/safety_config.yaml /etc/oht50/
```

### **Bước 3: Tạo Systemd Service (Tùy chọn)**

```bash
# Tạo service file
sudo nano /etc/systemd/system/oht50.service
```

**Nội dung file:**

```ini
[Unit]
Description=OHT-50 Master Module Firmware
After=network.target

[Service]
Type=simple
User=oht50
Group=oht50
WorkingDirectory=/home/oht50
ExecStart=/usr/local/bin/oht50_main
Restart=on-failure
RestartSec=10
StandardOutput=journal
StandardError=journal

[Install]
WantedBy=multi-user.target
```

```bash
# Enable và start service
sudo systemctl daemon-reload
sudo systemctl enable oht50.service
sudo systemctl start oht50.service

# Check status
sudo systemctl status oht50.service
```

---

## ✅ **XÁC MINH CÀI ĐẶT**

### **Bước 1: Test Run**

```bash
# Chạy firmware (dry-run mode)
./oht50_main --dry-run

# Chạy firmware với debug
./oht50_main --debug

# Chạy firmware bình thường
./oht50_main
```

### **Bước 2: Kiểm Tra Hardware**

```bash
# Kiểm tra UART1
ls -l /dev/ttyOHT485

# Kiểm tra GPIO
gpioinfo | grep -i "gpio1"

# Kiểm tra network
ip addr show

# Kiểm tra LiDAR (nếu có)
ls -l /dev/ttyUSB0
```

### **Bước 3: Test API**

```bash
# Test HTTP API (port 8080)
curl http://localhost:8080/api/v1/robot/status

# Test module discovery
curl http://localhost:8080/api/v1/modules

# Test telemetry
curl http://localhost:8080/api/v1/telemetry/current
```

### **Bước 4: Kiểm Tra Logs**

```bash
# Real-time logs
journalctl -u oht50.service -f

# Recent logs
journalctl -u oht50.service -n 100

# Errors only
journalctl -u oht50.service -p err
```

---

## 🔥 **TROUBLESHOOTING**

### **Vấn Đề 1: Build Failed - Missing Dependencies**

**Triệu chứng:**
```
CMake Error: Could not find OpenSSL
```

**Giải pháp:**
```bash
# Cài đặt lại OpenSSL
sudo apt install -y libssl-dev libcrypto++-dev
rm -rf build && mkdir build && cd build
cmake ..
```

---

### **Vấn Đề 2: /dev/ttyOHT485 Not Found**

**Triệu chứng:**
```
[OHT-50] hal_rs485_init failed: Device not found
```

**Giải pháp:**
```bash
# Kiểm tra UART1
ls -l /dev/ttyS1

# Nếu không có, check device tree
dmesg | grep ttyS1

# Tạo lại udev rule
sudo nano /etc/udev/rules.d/99-oht-rs485.rules
# Thêm: KERNEL=="ttyS1", SYMLINK+="ttyOHT485", MODE="0666"
sudo udevadm control --reload-rules
sudo reboot
```

---

### **Vấn Đề 3: Permission Denied khi chạy**

**Triệu chứng:**
```
Error: Permission denied opening /dev/ttyOHT485
```

**Giải pháp:**
```bash
# Thêm user vào dialout group
sudo usermod -a -G dialout $USER

# Hoặc chạy với sudo (không khuyến nghị)
sudo ./oht50_main
```

---

### **Vấn Đề 4: Port 8080 Already in Use**

**Triệu chứng:**
```
[OHT-50] API Manager init failed: Address already in use
```

**Giải pháp:**
```bash
# Tìm process đang dùng port 8080
sudo lsof -i :8080

# Kill process
sudo kill -9 <PID>

# Hoặc dùng auto cleanup (firmware tự động làm)
./oht50_main  # Auto cleanup enabled
```

---

## 📚 **TÀI LIỆU THAM KHẢO**

- [BUILD_GUIDE.md](BUILD_GUIDE.md) - Hướng dẫn build chi tiết
- [USAGE.md](USAGE.md) - Hướng dẫn sử dụng
- [DEVELOPMENT.md](DEVELOPMENT.md) - Hướng dẫn phát triển
- [TROUBLESHOOTING.md](TROUBLESHOOTING.md) - Khắc phục sự cố
- [API_REFERENCE.md](API_REFERENCE.md) - Tài liệu API

---

## 🆘 **HỖ TRỢ**

Nếu gặp vấn đề khi cài đặt:

1. 📖 Đọc [TROUBLESHOOTING.md](TROUBLESHOOTING.md)
2. 🐛 Tạo issue trên GitHub
3. 📧 Email: support@oht50.com
4. 💬 Slack channel: #oht50-support

---

**Version:** 1.0.1  
**Last Updated:** 2025-10-07  
**Maintained By:** OHT-50 Firmware Team

