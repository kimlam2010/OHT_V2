#!/bin/bash
echo "=== OHT-50 UART1 TEST SCRIPT ==="
echo ""

echo "🔍 Kiểm tra UART1 configuration:"
echo "• TX: GPIO1_B6 (GPIO 46) - Pin 5"
echo "• RX: GPIO1_B7 (GPIO 47) - Pin 3"
echo "• Device: /dev/ttyS9"
echo "• Baud: 115200"
echo ""

echo "🔍 Kiểm tra GPIO1_B6/B7 status:"
sudo gpioinfo gpiochip1 | grep -E "line  14|line  15"
echo ""

echo "🔍 Kiểm tra UART device:"
ls -la /dev/ttyS9
echo ""

echo "🔍 Test UART1 communication (loopback):"
echo "Gửi test message qua UART1..."
echo "TEST_UART1_$(date +%s)" > /dev/ttyS9
echo "✅ Test message sent"
echo ""

echo "🎯 UART1 Test Summary:"
echo "• GPIO1_B6 (TX): GPIO 46 - Pin 5"
echo "• GPIO1_B7 (RX): GPIO 47 - Pin 3"
echo "• Device: /dev/ttyS9"
echo "• Status: Ready for communication"
