#!/bin/bash
# Setup GPIO cho OHT-50 theo đúng danh sách firmware
# EMBED Team - Hardware & Low-Level Drivers

set -e

echo "🔧 Setup GPIO cho OHT-50 (CORRECTED)"
echo "===================================="

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "❌ Script cần chạy với sudo"
    echo "Usage: sudo ./setup_oht_gpio_correct.sh"
    exit 1
fi

echo "✅ Running as root"

# GPIO mapping đúng theo firmware
declare -A GPIO_MAP=(
    # LED Status Indicators (5 LEDs)
    ["LED_POWER"]="54"      # GPIO1_D6 - Power LED (Green)
    ["LED_SYSTEM"]="35"     # GPIO1_A3 - System LED (Blue)
    ["LED_COMM"]="28"       # GPIO0_D4 - Communication LED (Yellow)
    ["LED_NETWORK"]="29"    # GPIO0_D5 - Network LED (Green)
    ["LED_ERROR"]="58"      # GPIO1_D2 - Error LED (Red)
    
    # E-Stop Safety System (Dual-channel)
    ["ESTOP_CH1"]="59"      # GPIO1_D3 - E-Stop Channel 1
    
    # Relay Output (2x)
    ["RELAY1"]="131"        # GPIO4_A3 - Relay 1 Output
    ["RELAY2"]="132"        # GPIO4_A4 - Relay 2 Output
    
    # RS485 Communication - KHÔNG EXPORT (được quản lý bởi Device Tree)
    # ["UART1_TX"]="46"       # UART1_TX - Managed by Device Tree overlay
    # ["UART1_RX"]="47"       # UART1_RX - Managed by Device Tree overlay
)

# Function để export GPIO
export_gpio() {
    local pin=$1
    local name=$2
    
    if [ ! -d "/sys/class/gpio/gpio$pin" ]; then
        echo "$pin" > /sys/class/gpio/export
        echo "✅ Exported GPIO$pin ($name)"
    else
        echo "⚠️  GPIO$pin ($name) already exported"
    fi
}

# Function để set direction
set_direction() {
    local pin=$1
    local direction=$2
    local name=$3
    
    echo "$direction" > /sys/class/gpio/gpio$pin/direction
    echo "✅ Set GPIO$pin ($name) to $direction"
}

# Function để set value
set_value() {
    local pin=$1
    local value=$2
    local name=$3
    
    echo "$value" > /sys/class/gpio/gpio$pin/value
    echo "✅ Set GPIO$pin ($name) = $value"
}

# Function để get value
get_value() {
    local pin=$1
    local name=$2
    
    local value=$(cat /sys/class/gpio/gpio$pin/value)
    echo "📊 GPIO$pin ($name) = $value"
}

echo ""
echo "📋 Exporting GPIO pins..."

# Export tất cả GPIO
for name in "${!GPIO_MAP[@]}"; do
    pin="${GPIO_MAP[$name]}"
    export_gpio "$pin" "$name"
done

echo ""
echo "📋 Setting GPIO directions..."

# Set directions cho LED (OUTPUT)
set_direction "54" "out" "LED_POWER"
set_direction "35" "out" "LED_SYSTEM"
set_direction "28" "out" "LED_COMM"
set_direction "29" "out" "LED_NETWORK"
set_direction "58" "out" "LED_ERROR"

# Set directions cho E-Stop (INPUT)
set_direction "59" "in" "ESTOP_CH1"

# Set directions cho Relay (OUTPUT)
set_direction "131" "out" "RELAY1"
set_direction "132" "out" "RELAY2"

# UART1 pins KHÔNG được set direction - được quản lý bởi Device Tree overlay
# set_direction "46" "out" "UART1_TX"  # KHÔNG EXPORT - Device Tree quản lý
# set_direction "47" "in" "UART1_RX"   # KHÔNG EXPORT - Device Tree quản lý

echo ""
echo "📋 Setting initial values..."

# Set initial values cho LED (OFF)
set_value "54" "0" "LED_POWER"
set_value "35" "0" "LED_SYSTEM"
set_value "28" "0" "LED_COMM"
set_value "29" "0" "LED_NETWORK"
set_value "58" "0" "LED_ERROR"

# Set initial values cho Relay (OFF)
set_value "131" "0" "RELAY1"
set_value "132" "0" "RELAY2"

# UART1_TX KHÔNG được set value - được quản lý bởi Device Tree overlay
# set_value "46" "0" "UART1_TX"  # KHÔNG EXPORT - Device Tree quản lý

echo ""
echo "📋 Reading current GPIO values..."

# Read current values (chỉ GPIO đã export, không đọc UART pins)
for name in "${!GPIO_MAP[@]}"; do
    pin="${GPIO_MAP[$name]}"
    # Skip UART pins vì chúng được quản lý bởi Device Tree
    if [[ "$name" != "UART1_TX" && "$name" != "UART1_RX" ]]; then
        get_value "$pin" "$name"
    fi
done

echo ""
echo "🎉 GPIO setup completed (CORRECTED)!"
echo ""
echo "📋 GPIO Summary (CORRECTED):"
echo "  LED: POWER=GPIO54, SYSTEM=GPIO35, COMM=GPIO28, NETWORK=GPIO29, ERROR=GPIO58"
echo "  E-Stop: CH1=GPIO59"
echo "  Relay: RELAY1=GPIO131, RELAY2=GPIO132"
echo "  UART1: TX=GPIO46, RX=GPIO47 (Managed by Device Tree overlay - NOT exported)"
echo ""
echo "🧪 Test commands:"
echo "  # Test LED"
echo "  echo 1 > /sys/class/gpio/gpio54/value  # LED_POWER ON"
echo "  echo 0 > /sys/class/gpio/gpio54/value  # LED_POWER OFF"
echo ""
echo "  # Test Relay"
echo "  echo 1 > /sys/class/gpio/gpio131/value  # RELAY1 ON"
echo "  echo 0 > /sys/class/gpio/gpio131/value  # RELAY1 OFF"
echo ""
echo "  # Read E-Stop"
echo "  cat /sys/class/gpio/gpio59/value  # ESTOP_CH1"
