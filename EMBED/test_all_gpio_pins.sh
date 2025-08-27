#!/bin/bash
# OHT-50 All GPIO Pins Test Script
# EMBED Team - Test all GPIO pins after permissions fix

echo "=========================================="
echo "OHT-50 All GPIO Pins Test Script"
echo "Testing all GPIO pins after permissions fix"
echo "=========================================="

# Check if user is in gpio group
if ! groups | grep -q gpio; then
    echo "❌ User not in gpio group. Run: newgrp gpio"
    exit 1
fi

echo ""
echo "🔍 Testing all GPIO pins..."

# Function to test GPIO pin
test_gpio_pin() {
    local pin=$1
    local name=$2
    local direction=$3
    
    echo ""
    echo "Testing $name (GPIO $pin)..."
    
    # Export pin
    if echo $pin > /sys/class/gpio/export 2>/dev/null; then
        echo "  ✅ GPIO $pin export successful"
        
        # Wait for udev to set permissions
        sleep 0.5
        
        # Set direction
        if echo $direction > /sys/class/gpio/gpio$pin/direction 2>/dev/null; then
            echo "  ✅ GPIO $pin direction set to $direction"
            
            if [ "$direction" = "out" ]; then
                # Test output functionality
                if echo 1 > /sys/class/gpio/gpio$pin/value 2>/dev/null; then
                    echo "  ✅ GPIO $pin set HIGH"
                    sleep 0.5
                    if echo 0 > /sys/class/gpio/gpio$pin/value 2>/dev/null; then
                        echo "  ✅ GPIO $pin set LOW"
                    else
                        echo "  ❌ GPIO $pin set LOW failed"
                    fi
                else
                    echo "  ❌ GPIO $pin set HIGH failed"
                fi
            else
                # Test input functionality
                value=$(cat /sys/class/gpio/gpio$pin/value 2>/dev/null)
                if [ $? -eq 0 ]; then
                    echo "  ✅ GPIO $pin read value: $value"
                else
                    echo "  ❌ GPIO $pin read failed"
                fi
            fi
        else
            echo "  ❌ GPIO $pin direction set failed"
        fi
        
        # Unexport
        echo $pin > /sys/class/gpio/unexport 2>/dev/null
        echo "  ✅ GPIO $pin unexported"
    else
        echo "  ❌ GPIO $pin export failed"
    fi
}

# Test all LED pins (OUTPUT)
echo "🔴 Testing LED Pins (OUTPUT):"
echo "=============================="
test_gpio_pin 54 "Power LED" "out"
test_gpio_pin 35 "System LED" "out"
test_gpio_pin 28 "Communication LED" "out"
test_gpio_pin 29 "Network LED" "out"
test_gpio_pin 58 "Error LED" "out"

# Test E-Stop pin (INPUT)
echo ""
echo "🛑 Testing E-Stop Pin (INPUT):"
echo "=============================="
test_gpio_pin 59 "E-Stop" "in"

# Test Relay pins (OUTPUT)
echo ""
echo "🔌 Testing Relay Pins (OUTPUT):"
echo "==============================="
test_gpio_pin 131 "Relay 1" "out"
test_gpio_pin 132 "Relay 2" "out"

echo ""
echo "📊 Final Status Check:"
echo "======================"

# Quick status check of all pins
for pin in 54 35 28 29 58 59 131 132; do
    if echo $pin > /sys/class/gpio/export 2>/dev/null; then
        echo "GPIO $pin: ✅ Available"
        echo $pin > /sys/class/gpio/unexport 2>/dev/null
    else
        echo "GPIO $pin: ❌ Not available"
    fi
done

echo ""
echo "=========================================="
echo "All GPIO Pins Test Complete"
echo "=========================================="
echo ""
echo "🎯 If all tests passed:"
echo "   ✅ FW can now use all GPIO pins"
echo "   ✅ No more 'Failed to export GPIO' errors"
echo "   ✅ LED and E-Stop functionality ready"
echo ""
echo "📋 Next steps:"
echo "   1. Test FW: ./oht_firmware"
echo "   2. Check LED indicators"
echo "   3. Test E-Stop functionality"
