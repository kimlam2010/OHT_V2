#!/bin/bash
# test_rs485.sh - RS485 Test Script với opwiring
# Sử dụng cho Orange Pi 5B

set -e  # Exit on error

echo "=== RS485 Test Script với opwiring ==="
echo "Board: Orange Pi 5B (RK3588)"
echo "Tool: opwiring"
echo "Date: $(date)"
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Function to print status
print_status() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✓ $2${NC}"
    else
        echo -e "${RED}✗ $2${NC}"
        return 1
    fi
}

# Function to print warning
print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

# Check if opwiring is installed
echo "1. Kiểm tra opwiring..."
if command -v opwiring &> /dev/null; then
    print_status 0 "opwiring đã cài đặt"
    opwiring --version
else
    print_status 1 "opwiring chưa cài đặt"
    echo "Cài đặt: sudo apt install opwiring"
    exit 1
fi
echo ""

# Check UART1 status
echo "2. Kiểm tra UART1..."
if opwiring uart status 1 2>/dev/null; then
    print_status 0 "UART1 đã bật"
else
    print_warning "UART1 chưa bật, đang bật..."
    if opwiring uart enable 1; then
        print_status 0 "UART1 đã bật thành công"
    else
        print_status 1 "Không thể bật UART1"
        echo "Kiểm tra device tree overlay: opwiring overlay list"
    fi
fi
echo ""

# Check device nodes
echo "3. Kiểm tra device nodes..."
if ls -l /dev/ttyS* 2>/dev/null; then
    print_status 0 "Device nodes UART"
else
    print_status 1 "Không tìm thấy device nodes UART"
fi
echo ""

# Check GPIO
echo "4. Kiểm tra GPIO..."
if opwiring gpio info 1 3 2>/dev/null; then
    print_status 0 "GPIO1_D3 (Relay1/DE-RE) hoạt động"
else
    print_status 1 "GPIO1_D3 không hoạt động"
fi

if opwiring gpio info 1 2 2>/dev/null; then
    print_status 0 "GPIO1_D2 (Relay2/DE-RE) hoạt động"
else
    print_status 1 "GPIO1_D2 không hoạt động"
fi
echo ""

# Test GPIO toggle
echo "5. Test GPIO toggle..."
echo "Test GPIO1_D3 (Relay1/DE-RE)..."
if opwiring gpio set 1 3 1; then
    sleep 0.1
    if opwiring gpio set 1 3 0; then
        print_status 0 "GPIO1_D3 toggle thành công"
    else
        print_status 1 "GPIO1_D3 toggle thất bại"
    fi
else
    print_status 1 "Không thể set GPIO1_D3"
fi

echo "Test GPIO1_D2 (Relay2/DE-RE)..."
if opwiring gpio set 1 2 1; then
    sleep 0.1
    if opwiring gpio set 1 2 0; then
        print_status 0 "GPIO1_D2 toggle thành công"
    else
        print_status 1 "GPIO1_D2 toggle thất bại"
    fi
else
    print_status 1 "Không thể set GPIO1_D2"
fi
echo ""

# Configure UART1 for RS485
echo "6. Cấu hình UART1 cho RS485..."
if opwiring uart config 1 --baud 115200; then
    print_status 0 "Baud rate 115200"
else
    print_status 1 "Không thể set baud rate"
fi

if opwiring uart config 1 --rs485; then
    print_status 0 "RS485 mode"
else
    print_status 1 "Không thể set RS485 mode"
fi

if opwiring uart config 1 --rs485-auto-rts; then
    print_status 0 "Auto-RTS mode"
else
    print_status 1 "Không thể set auto-RTS mode"
fi
echo ""

# Show current UART1 config
echo "7. Cấu hình hiện tại UART1..."
if opwiring uart config 1 --show; then
    print_status 0 "Hiển thị cấu hình thành công"
else
    print_status 1 "Không thể hiển thị cấu hình"
fi
echo ""

# Test UART1 loopback (if TX-RX connected)
echo "8. Test UART1 loopback..."
print_warning "Cần nối TX→RX để test loopback"
if opwiring uart test 1; then
    print_status 0 "UART1 loopback thành công"
else
    print_warning "UART1 loopback thất bại (có thể chưa nối TX→RX)"
fi
echo ""

# Test RS485 protocol commands
echo "9. Test RS485 protocol commands..."
echo "Gửi PING command (0x01) tới addr 0x01..."
if opwiring uart write 1 --hex "AA 01 01 00 00 00"; then
    print_status 0 "Gửi PING thành công"
    sleep 0.1
    echo "Đọc response..."
    if opwiring uart read 1 --hex; then
        print_status 0 "Đọc response thành công"
    else
        print_warning "Không có response (có thể chưa có module)"
    fi
else
    print_status 1 "Gửi PING thất bại"
fi
echo ""

echo "Gửi GET_INFO command (0x02) tới addr 0x01..."
if opwiring uart write 1 --hex "AA 01 02 00 00 00"; then
    print_status 0 "Gửi GET_INFO thành công"
    sleep 0.1
    echo "Đọc response..."
    if opwiring uart read 1 --hex; then
        print_status 0 "Đọc response thành công"
    else
        print_warning "Không có response (có thể chưa có module)"
    fi
else
    print_status 1 "Gửi GET_INFO thất bại"
fi
echo ""

# Check udev alias
echo "10. Kiểm tra udev alias..."
if [ -e /dev/ttyOHT485 ]; then
    print_status 0 "udev alias /dev/ttyOHT485 tồn tại"
    ls -l /dev/ttyOHT485
else
    print_warning "udev alias /dev/ttyOHT485 không tồn tại"
    echo "Tạo alias: sudo ln -s /dev/ttyS1 /dev/ttyOHT485"
fi
echo ""

# Summary
echo "=== Tóm tắt kết quả ==="
echo "✅ Software: opwiring, UART1, GPIO đã sẵn sàng"
echo "⚠️  Hardware: Cần wiring RS485 để test thật"
echo ""
echo "=== Hướng dẫn wiring ==="
echo "1. UART1 TX (GPIO0_A2) → RS485 Transceiver TX"
echo "2. UART1 RX (GPIO0_A3) → RS485 Transceiver RX"
echo "3. GND → RS485 Transceiver GND"
echo "4. RS485 A/B → Module RS485 A/B"
echo "5. DE/RE → RTS (auto) hoặc GPIO1_D3 (manual)"
echo "6. Termination 120Ω hai đầu"
echo "7. Bias resistor (680Ω-10kΩ)"
echo ""
echo "=== Test sau wiring ==="
echo "Chạy lại script: ./test_rs485.sh"
echo "Hoặc test riêng:"
echo "  opwiring uart write 1 --hex \"AA 01 01 00 00 00\"  # PING"
echo "  opwiring uart read 1 --hex"
echo ""
echo "=== Test hoàn thành ==="
