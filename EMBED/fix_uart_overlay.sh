#!/bin/bash

# 🚨 FIX UART OVERLAY - Sử dụng overlay có sẵn của hệ thống
# Version: 1.0
# Mục đích: Khắc phục vấn đề UART1 không xuất hiện sau reboot

echo "🔧 FIXING UART1 OVERLAY ISSUE..."
echo "=================================="

# Backup boot config
echo "📁 Backing up orangepiEnv.txt..."
sudo cp /boot/orangepiEnv.txt /boot/orangepiEnv.txt.backup.$(date +%Y%m%d_%H%M%S)

# Update boot config to use system overlay
echo "⚙️ Updating boot config to use system overlay..."
sudo sed -i 's/overlays=uart1_correct/overlays=uart1-m1/' /boot/orangepiEnv.txt
sudo sed -i 's/overlays=rk3588-uart1-m1/overlays=uart1-m1/' /boot/orangepiEnv.txt

# Verify change
echo "✅ New boot config:"
cat /boot/orangepiEnv.txt | grep overlays

# Check if system overlay exists
echo ""
echo "🔍 Checking system overlay exists..."
if [ -f "/boot/dtb-6.1.43-rockchip-rk3588/rockchip/overlay/rk3588-uart1-m1.dtbo" ]; then
    echo "✅ System overlay found: rk3588-uart1-m1.dtbo (used as uart1-m1)"
    ls -la /boot/dtb-6.1.43-rockchip-rk3588/rockchip/overlay/rk3588-uart1-m1.dtbo
else
    echo "❌ System overlay not found!"
    exit 1
fi

echo ""
echo "🎉 UART1 overlay fix completed!"
echo "==============================="
echo ""
echo "🚨 REBOOT REQUIRED to apply changes"
echo ""
echo "After reboot, UART1 should appear as:"
echo "  /dev/ttyS1 (UART1 device)"
echo "  /dev/ttyOHT485 (symlink via udev)"
echo ""
echo "🔄 Reboot now? (y/n)"
read -r response
if [[ $response =~ ^[Yy]$ ]]; then
    echo "🚀 Rebooting..."
    sudo reboot
else
    echo "⚠️ Remember to reboot to apply changes!"
fi
