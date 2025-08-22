#!/bin/bash
# Setup udev rules cho OHT-50 RS485
# Tạo symlink /dev/ttyOHT485 → /dev/ttyS1

set -e

echo "🔧 Setup udev rules cho OHT-50 RS485"
echo "=================================="

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "❌ Script cần chạy với sudo"
    echo "Usage: sudo ./setup_udev_rs485.sh"
    exit 1
fi

# Check if UART1 exists
if [ ! -e "/dev/ttyS1" ]; then
    echo "❌ /dev/ttyS1 không tồn tại"
    echo "Hãy enable UART1 trong /boot/orangepiEnv.txt trước:"
    echo "  overlays=uart1-m1"
    exit 1
fi

echo "✅ /dev/ttyS1 tồn tại"

# Get UART1 device info
echo "📋 UART1 device info:"
ls -la /dev/ttyS1

# Create udev rule
UDEV_RULE="/etc/udev/rules.d/99-oht50-rs485.rules"
RULE_CONTENT='# OHT-50 RS485 UART1 symlink
KERNEL=="ttyS1", SYMLINK+="ttyOHT485", MODE="0666"
KERNEL=="ttyS1", GROUP="dialout", MODE="0666"'

echo "📝 Tạo udev rule: $UDEV_RULE"
echo "$RULE_CONTENT" | sudo tee "$UDEV_RULE"

# Reload udev rules
echo "🔄 Reload udev rules..."
sudo udevadm control --reload-rules
sudo udevadm trigger

# Wait for symlink to be created
echo "⏳ Chờ symlink được tạo..."
sleep 2

# Check if symlink exists
if [ -e "/dev/ttyOHT485" ]; then
    echo "✅ Symlink /dev/ttyOHT485 đã được tạo"
    echo "📋 Symlink info:"
    ls -la /dev/ttyOHT485
else
    echo "❌ Symlink /dev/ttyOHT485 chưa được tạo"
    echo "Thử tạo manual symlink..."
    sudo ln -sf /dev/ttyS1 /dev/ttyOHT485
    if [ -e "/dev/ttyOHT485" ]; then
        echo "✅ Manual symlink thành công"
    else
        echo "❌ Manual symlink thất bại"
        exit 1
    fi
fi

# Test symlink
echo "🧪 Test symlink..."
if [ -e "/dev/ttyOHT485" ] && [ "$(readlink /dev/ttyOHT485)" = "/dev/ttyS1" ]; then
    echo "✅ Symlink test passed"
else
    echo "❌ Symlink test failed"
    exit 1
fi

# Set permissions
echo "🔐 Set permissions..."
sudo chmod 666 /dev/ttyOHT485
sudo chown root:dialout /dev/ttyOHT485

# Add user to dialout group
CURRENT_USER=$(logname || echo $SUDO_USER)
if [ -n "$CURRENT_USER" ]; then
    echo "👤 Thêm user $CURRENT_USER vào group dialout..."
    sudo usermod -a -G dialout "$CURRENT_USER"
    echo "⚠️  Cần logout/login để group có hiệu lực"
fi

# Create test script
TEST_SCRIPT="test_udev_rs485.py"
cat > "$TEST_SCRIPT" << 'EOF'
#!/usr/bin/env python3
"""
Test udev RS485 setup
"""
import os
import serial
import time

def test_udev_symlink():
    """Test udev symlink"""
    print("🔍 Test udev symlink...")
    
    if not os.path.exists("/dev/ttyOHT485"):
        print("❌ /dev/ttyOHT485 không tồn tại")
        return False
    
    if not os.path.islink("/dev/ttyOHT485"):
        print("❌ /dev/ttyOHT485 không phải symlink")
        return False
    
    target = os.readlink("/dev/ttyOHT485")
    if target != "/dev/ttyS1":
        print(f"❌ Symlink target sai: {target}")
        return False
    
    print("✅ Udev symlink test passed")
    return True

def test_serial_access():
    """Test serial access"""
    print("🔍 Test serial access...")
    
    try:
        ser = serial.Serial(
            port='/dev/ttyOHT485',
            baudrate=115200,
            timeout=1
        )
        ser.close()
        print("✅ Serial access test passed")
        return True
    except Exception as e:
        print(f"❌ Serial access test failed: {e}")
        return False

def main():
    """Main function"""
    print("🧪 Test udev RS485 setup")
    print("=" * 40)
    
    tests = [
        ("Udev Symlink", test_udev_symlink),
        ("Serial Access", test_serial_access)
    ]
    
    passed = 0
    total = len(tests)
    
    for test_name, test_func in tests:
        print(f"\n📋 Running: {test_name}")
        if test_func():
            passed += 1
            print(f"✅ PASS: {test_name}")
        else:
            print(f"❌ FAIL: {test_name}")
    
    print(f"\n📊 Results: {passed}/{total} tests passed")
    
    if passed == total:
        print("🎉 Tất cả tests PASSED - Udev setup thành công!")
    else:
        print("⚠️  Một số tests FAILED - Kiểm tra setup")

if __name__ == "__main__":
    main()
EOF

chmod +x "$TEST_SCRIPT"

echo ""
echo "🎉 Setup udev rules hoàn thành!"
echo ""
echo "📋 Summary:"
echo "  ✅ Udev rule: $UDEV_RULE"
echo "  ✅ Symlink: /dev/ttyOHT485 → /dev/ttyS1"
echo "  ✅ Permissions: 666, group: dialout"
echo "  ✅ Test script: $TEST_SCRIPT"
echo ""
echo "🧪 Chạy test:"
echo "  python3 $TEST_SCRIPT"
echo ""
echo "⚠️  Lưu ý:"
echo "  - Cần logout/login để group dialout có hiệu lực"
echo "  - Symlink sẽ tự động tạo sau reboot"
echo "  - HAL có thể sử dụng /dev/ttyOHT485 thay vì /dev/ttyS1"
