#!/bin/bash
echo "=== Cấu hình GPIO 47/46 thành UART TX/RX ==="

# Unexport GPIO 47 và 46 trước
echo "1. Unexport GPIO 47 và 46..."
echo 47 > /sys/class/gpio/unexport 2>/dev/null
echo 46 > /sys/class/gpio/unexport 2>/dev/null

# Export lại GPIO 47 và 46
echo "2. Export GPIO 47 và 46..."
echo 47 > /sys/class/gpio/export
echo 46 > /sys/class/gpio/export

# Cấu hình GPIO 47 thành output (TX)
echo "3. Cấu hình GPIO 47 thành output (TX)..."
echo out > /sys/class/gpio/gpio47/direction

# Cấu hình GPIO 46 thành input (RX)
echo "4. Cấu hình GPIO 46 thành input (RX)..."
echo in > /sys/class/gpio/gpio46/direction

# Test TX
echo "5. Test TX (GPIO 47)..."
for i in {1..5}; do
    echo 1 > /sys/class/gpio/gpio47/value
    sleep 0.1
    echo 0 > /sys/class/gpio/gpio47/value
    sleep 0.1
    echo "TX pulse $i/5"
done

# Đọc RX
echo "6. Đọc RX (GPIO 46)..."
for i in {1..10}; do
    value=$(cat /sys/class/gpio/gpio46/value)
    echo "RX value: $value"
    sleep 0.5
done

echo "=== Hoàn thành cấu hình ==="
