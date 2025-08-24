#!/bin/bash

# GPIO Permissions Fix Script for OHT-50 Testing
# This script fixes GPIO permissions for testing purposes

echo "🔧 Fixing GPIO permissions for OHT-50 testing..."

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "❌ This script must be run as root (use sudo)"
    exit 1
fi

# Fix GPIO export/unexport permissions
echo "📝 Setting GPIO export/unexport permissions..."
chmod 666 /sys/class/gpio/export 2>/dev/null
chmod 666 /sys/class/gpio/unexport 2>/dev/null

# Fix GPIO direction and value permissions for E-Stop pin (59)
echo "🔧 Setting E-Stop GPIO permissions..."
if [ -d "/sys/class/gpio/gpio59" ]; then
    chmod 666 /sys/class/gpio/gpio59/direction 2>/dev/null
    chmod 666 /sys/class/gpio/gpio59/value 2>/dev/null
    echo "✅ E-Stop GPIO permissions set"
else
    echo "⚠️ E-Stop GPIO not exported yet"
fi

# Fix GPIO permissions for LED pins
echo "🔧 Setting LED GPIO permissions..."
for pin in 54 35 28 29 58; do
    if [ -d "/sys/class/gpio/gpio$pin" ]; then
        chmod 666 /sys/class/gpio/gpio$pin/direction 2>/dev/null
        chmod 666 /sys/class/gpio/gpio$pin/value 2>/dev/null
        echo "✅ LED GPIO $pin permissions set"
    else
        echo "⚠️ LED GPIO $pin not exported yet"
    fi
done

# Fix GPIO permissions for Relay pins
echo "🔧 Setting Relay GPIO permissions..."
for pin in 131 132; do
    if [ -d "/sys/class/gpio/gpio$pin" ]; then
        chmod 666 /sys/class/gpio/gpio$pin/direction 2>/dev/null
        chmod 666 /sys/class/gpio/gpio$pin/value 2>/dev/null
        echo "✅ Relay GPIO $pin permissions set"
    else
        echo "⚠️ Relay GPIO $pin not exported yet"
    fi
done

# Create udev rules for persistent permissions
echo "📝 Creating udev rules for persistent permissions..."
cat > /etc/udev/rules.d/99-oht50-gpio.rules << EOF
# OHT-50 GPIO permissions
SUBSYSTEM=="gpio", KERNEL=="gpiochip*", ACTION=="add", PROGRAM="/bin/sh -c 'chown -R root:gpio /sys/class/gpio && chmod -R 770 /sys/class/gpio'"
SUBSYSTEM=="gpio", KERNEL=="gpio*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:gpio /sys/class/gpio/export /sys/class/gpio/unexport && chmod 220 /sys/class/gpio/export /sys/class/gpio/unexport'"
SUBSYSTEM=="gpio", KERNEL=="gpio*", ACTION=="add", PROGRAM="/bin/sh -c 'chown root:gpio /sys/%p/active_low /sys/%p/direction /sys/%p/edge /sys/%p/value && chmod 660 /sys/%p/active_low /sys/%p/direction /sys/%p/edge /sys/%p/value'"
EOF

# Reload udev rules
udevadm control --reload-rules
udevadm trigger

echo "✅ GPIO permissions fixed successfully!"
echo "📋 Note: You may need to restart the system for all changes to take effect"
echo "🔧 For immediate testing, run this script before each test session"
