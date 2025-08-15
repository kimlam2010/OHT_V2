#!/bin/bash
echo "=== OHT-50 GPIO TEST SCRIPT ==="
echo ""

# Test GPIO1_D2 (Status LED)
echo "🔍 Testing GPIO1_D2 (Status LED)..."
sudo gpioset gpiochip1 2=1
sleep 1
sudo gpioset gpiochip1 2=0
echo "✅ GPIO1_D2 test completed"

# Test GPIO1_D3 (RS485 DE/RE)
echo "🔍 Testing GPIO1_D3 (RS485 DE/RE)..."
sudo gpioset gpiochip1 3=1
sleep 1
sudo gpioset gpiochip1 3=0
echo "✅ GPIO1_D3 test completed"

echo ""
echo "🎯 GPIO Test Summary:"
echo "• GPIO1_D2: Status LED control"
echo "• GPIO1_D3: RS485 DE/RE control"
echo "• UART1: /dev/ttyS9 (cần test communication)"
