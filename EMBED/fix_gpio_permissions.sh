#!/bin/bash
# OHT-50 GPIO Permissions Fix Script
# EMBED Team - Fix SYSFS GPIO access permissions

echo "=========================================="
echo "OHT-50 GPIO Permissions Fix Script"
echo "Fixing SYSFS GPIO access for Orange Pi 5B"
echo "=========================================="

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "❌ Please run as root (sudo)"
    exit 1
fi

echo ""
echo "🔧 Fixing GPIO permissions..."

# Create gpio group if it doesn't exist
if ! getent group gpio > /dev/null 2>&1; then
    echo "Creating gpio group..."
    groupadd gpio
fi

# Add current user to gpio group
echo "Adding user to gpio group..."
usermod -a -G gpio $SUDO_USER

# Set permissions for GPIO export/unexport
echo "Setting GPIO export/unexport permissions..."
chown root:gpio /sys/class/gpio/export
chown root:gpio /sys/class/gpio/unexport
chmod 220 /sys/class/gpio/export
chmod 220 /sys/class/gpio/unexport

# Create udev rules for GPIO access
echo "Creating udev rules..."
cat > /etc/udev/rules.d/99-gpio.rules << 'EOF'
# GPIO permissions for OHT-50
SUBSYSTEM=="gpio", GROUP="gpio", MODE="0660"
SUBSYSTEM=="bcm2835-gpiomem", GROUP="gpio", MODE="0660"

# Orange Pi 5B specific GPIO permissions
SUBSYSTEM=="gpio", KERNEL=="gpiochip*", GROUP="gpio", MODE="0660"
SUBSYSTEM=="gpio", KERNEL=="gpio*", GROUP="gpio", MODE="0660"
EOF

# Reload udev rules
echo "Reloading udev rules..."
udevadm control --reload-rules
udevadm trigger

# Test GPIO access
echo ""
echo "🧪 Testing GPIO access..."

# Test export GPIO 54
echo "Testing GPIO 54 export..."
if echo 54 > /sys/class/gpio/export 2>/dev/null; then
    echo "  ✅ GPIO 54 export successful"
    
    # Test set direction
    if echo out > /sys/class/gpio/gpio54/direction 2>/dev/null; then
        echo "  ✅ GPIO 54 direction set"
        
        # Test set value
        if echo 1 > /sys/class/gpio/gpio54/value 2>/dev/null; then
            echo "  ✅ GPIO 54 value set HIGH"
            sleep 1
            echo 0 > /sys/class/gpio/gpio54/value
            echo "  ✅ GPIO 54 value set LOW"
        else
            echo "  ❌ GPIO 54 value set failed"
        fi
    else
        echo "  ❌ GPIO 54 direction set failed"
    fi
    
    # Unexport
    echo 54 > /sys/class/gpio/unexport
else
    echo "  ❌ GPIO 54 export failed"
fi

# Test export GPIO 59
echo "Testing GPIO 59 export..."
if echo 59 > /sys/class/gpio/export 2>/dev/null; then
    echo "  ✅ GPIO 59 export successful"
    
    # Test set direction
    if echo in > /sys/class/gpio/gpio59/direction 2>/dev/null; then
        echo "  ✅ GPIO 59 direction set"
        
        # Test read value
        value=$(cat /sys/class/gpio/gpio59/value)
        echo "  ✅ GPIO 59 value read: $value"
    else
        echo "  ❌ GPIO 59 direction set failed"
    fi
    
    # Unexport
    echo 59 > /sys/class/gpio/unexport
else
    echo "  ❌ GPIO 59 export failed"
fi

echo ""
echo "📋 Instructions for FW:"
echo "1. Logout and login again to apply group changes"
echo "2. Or run: newgrp gpio"
echo "3. Test FW without sudo: ./oht_firmware"
echo ""
echo "🔧 Alternative: Run FW with sudo: sudo ./oht_firmware"

echo ""
echo "=========================================="
echo "GPIO Permissions Fix Complete"
echo "=========================================="
