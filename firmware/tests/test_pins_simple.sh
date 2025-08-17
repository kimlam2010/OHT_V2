#!/bin/bash

echo "=== GPIO Pin Test Script ==="
echo "Testing pins: 54, 35, 28, 29, 58, 59, 131, 132"
echo ""

# Function to test a GPIO pin
test_pin() {
    local pin=$1
    local name=$2
    
    echo "Testing $name (GPIO $pin)..."
    
    # Export GPIO
    echo $pin > /sys/class/gpio/export 2>/dev/null
    if [ $? -ne 0 ]; then
        echo "❌ Failed to export GPIO $pin"
        return 1
    fi
    
    # Wait a moment
    sleep 0.1
    
    # Check if GPIO directory exists
    if [ ! -d "/sys/class/gpio/gpio$pin" ]; then
        echo "❌ GPIO $pin directory not found"
        return 1
    fi
    
    # Set direction to output
    echo "out" > /sys/class/gpio/gpio$pin/direction 2>/dev/null
    if [ $? -ne 0 ]; then
        echo "❌ Failed to set direction for GPIO $pin"
        return 1
    fi
    
    # Set value to HIGH
    echo "1" > /sys/class/gpio/gpio$pin/value 2>/dev/null
    if [ $? -ne 0 ]; then
        echo "❌ Failed to set value for GPIO $pin"
        return 1
    fi
    
    echo "✅ GPIO $pin set to HIGH"
    
    # Wait 500ms
    sleep 0.5
    
    # Set value to LOW
    echo "0" > /sys/class/gpio/gpio$pin/value 2>/dev/null
    if [ $? -eq 0 ]; then
        echo "✅ GPIO $pin set to LOW"
    fi
    
    # Unexport
    echo $pin > /sys/class/gpio/unexport 2>/dev/null
    
    return 0
}

# Test all pins
passed=0
total=0

echo "🔴 Testing LED pins..."
if test_pin 54 "LED Power"; then passed=$((passed + 1)); fi
total=$((total + 1))

if test_pin 35 "LED System"; then passed=$((passed + 1)); fi
total=$((total + 1))

if test_pin 28 "LED Communication"; then passed=$((passed + 1)); fi
total=$((total + 1))

if test_pin 29 "LED Network"; then passed=$((passed + 1)); fi
total=$((total + 1))

if test_pin 58 "LED Error"; then passed=$((passed + 1)); fi
total=$((total + 1))

echo ""
echo "🛑 Testing E-Stop pins..."
if test_pin 59 "E-Stop Channel 1"; then passed=$((passed + 1)); fi
total=$((total + 1))

if test_pin 131 "E-Stop Channel 2"; then passed=$((passed + 1)); fi
total=$((total + 1))

echo ""
echo "⚡ Testing Relay pin..."
if test_pin 132 "Relay Output"; then passed=$((passed + 1)); fi
total=$((total + 1))

echo ""
echo "=== Test Summary ==="
echo "Passed: $passed/$total"
echo "Success rate: $(echo "scale=1; $passed * 100 / $total" | bc)%"

if [ $passed -eq $total ]; then
    echo "✅ All GPIO pins working correctly!"
    exit 0
else
    echo "❌ Some GPIO pins failed"
    exit 1
fi
