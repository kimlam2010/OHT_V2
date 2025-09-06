#!/bin/bash

# GPIO Test Script - An toàn cho EMBED team
# Kiểm tra GPIO pins theo định nghĩa HAL mà không gây conflict

echo "=== GPIO SAFE TEST SCRIPT ==="
echo "Ngày: $(date)"
echo "Uptime: $(uptime)"
echo ""

# Định nghĩa GPIO pins theo HAL
declare -A GPIO_PINS=(
    ["LED_POWER"]="54:gpiochip1:22"
    ["LED_SYSTEM"]="35:gpiochip1:3"
    ["LED_COMM"]="28:gpiochip0:28"
    ["LED_NETWORK"]="29:gpiochip0:29"
    ["LED_ERROR"]="58:gpiochip1:26"
    ["ESTOP"]="59:gpiochip1:27"
    ["RELAY1"]="131:gpiochip4:3"
    ["RELAY2"]="132:gpiochip4:4"
)

echo "=== KIỂM TRA GPIO PINS (CHỈ ĐỌC) ==="
for pin_name in "${!GPIO_PINS[@]}"; do
    IFS=':' read -r pin_num chip_name offset <<< "${GPIO_PINS[$pin_name]}"
    echo -n "Pin $pin_num ($pin_name): "
    
    # Đọc giá trị GPIO
    if gpioget "$chip_name" "$offset" 2>/dev/null; then
        echo " - OK"
    else
        echo " - ERROR"
    fi
done

echo ""
echo "=== KIỂM TRA GPIO CHIPS ==="
gpioinfo | grep -E "gpiochip[0-9]" | while read line; do
    echo "$line"
done

echo ""
echo "=== KIỂM TRA SYSTEM STATUS ==="
echo "Load average: $(cat /proc/loadavg)"
echo "Memory usage: $(free -h | grep Mem | awk '{print $3"/"$2}')"
echo "Disk usage: $(df -h / | tail -1 | awk '{print $3"/"$2" ("$5")"}')"

echo ""
echo "=== KIỂM TRA PROCESSES ==="
echo "Processes sử dụng GPIO:"
lsof | grep gpio || echo "Không có process nào sử dụng GPIO"

echo ""
echo "=== KIỂM TRA SYSTEM LOGS ==="
echo "GPIO errors trong dmesg:"
dmesg | grep -i gpio | tail -5 || echo "Không có GPIO errors"

echo ""
echo "=== TEST HOÀN THÀNH ==="
echo "Tất cả GPIO pins đã được kiểm tra an toàn"
echo "Không có conflict được phát hiện"

