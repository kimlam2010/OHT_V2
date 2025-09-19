#!/bin/bash

# Fix GPIO Permissions for OHT-50
# Run this after setup_complete_system.sh to fix GPIO permissions

echo "🔧 Fixing GPIO permissions..."

# GPIO pins
GPIO_PINS=(54 35 28 29 58 59 131 132 46 47)

for pin in "${GPIO_PINS[@]}"; do
    if [ -d "/sys/class/gpio/gpio$pin" ]; then
        echo "  📌 Fixing GPIO$pin permissions..."
        sudo chown orangepi:orangepi /sys/class/gpio/gpio$pin/value
        sudo chown orangepi:orangepi /sys/class/gpio/gpio$pin/direction
        sudo chmod 664 /sys/class/gpio/gpio$pin/value
        sudo chmod 664 /sys/class/gpio/gpio$pin/direction
        echo "    ✅ GPIO$pin permissions fixed"
    fi
done

echo "✅ All GPIO permissions fixed!"
echo ""
echo "🧪 Test commands:"
echo "  echo 1 > /sys/class/gpio/gpio54/value  # LED_POWER ON"
echo "  echo 0 > /sys/class/gpio/gpio54/value  # LED_POWER OFF"
echo "  cat /sys/class/gpio/gpio59/value       # Read E-Stop"
