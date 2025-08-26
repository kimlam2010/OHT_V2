#!/bin/bash
# Setup GPIO cho OHT-50 trên Orange Pi 5B
# EMBED Team - Hardware & Low-Level Drivers

set -e

echo "🔧 Setup GPIO cho OHT-50"
echo "========================"

# Check if running as root
if [ "$EUID" -ne 0 ]; then
    echo "❌ Script cần chạy với sudo"
    echo "Usage: sudo ./setup_oht_gpio.sh"
    exit 1
fi

echo "✅ Running as root"

# GPIO mapping cho OHT-50
declare -A GPIO_MAP=(
    ["UART1_TX"]="46"
    ["UART1_RX"]="47"
    ["RELAY1"]="51"
    ["RELAY2"]="50"
    ["LED_POWER"]="52"
    ["LED_SYSTEM"]="53"
    ["LED_COMM"]="54"
    ["LED_NETWORK"]="55"
    ["LED_ERROR"]="56"
    ["ESTOP_CH1"]="57"
    ["ESTOP_CH2"]="58"
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

# Set directions
set_direction "46" "out" "UART1_TX"
set_direction "47" "in" "UART1_RX"
set_direction "51" "out" "RELAY1"
set_direction "50" "out" "RELAY2"
set_direction "52" "out" "LED_POWER"
set_direction "53" "out" "LED_SYSTEM"
set_direction "54" "out" "LED_COMM"
set_direction "55" "out" "LED_NETWORK"
set_direction "56" "out" "LED_ERROR"
set_direction "57" "in" "ESTOP_CH1"
set_direction "58" "in" "ESTOP_CH2"

echo ""
echo "📋 Setting initial values..."

# Set initial values
set_value "46" "0" "UART1_TX"
set_value "51" "0" "RELAY1"
set_value "50" "0" "RELAY2"
set_value "52" "0" "LED_POWER"
set_value "53" "0" "LED_SYSTEM"
set_value "54" "0" "LED_COMM"
set_value "55" "0" "LED_NETWORK"
set_value "56" "0" "LED_ERROR"

echo ""
echo "📋 Reading current GPIO values..."

# Read current values
for name in "${!GPIO_MAP[@]}"; do
    pin="${GPIO_MAP[$name]}"
    get_value "$pin" "$name"
done

echo ""
echo "🎉 GPIO setup completed!"
echo ""
echo "📋 GPIO Summary:"
echo "  UART1: TX=GPIO46, RX=GPIO47"
echo "  Relay: RELAY1=GPIO51, RELAY2=GPIO50"
echo "  LED: POWER=GPIO52, SYSTEM=GPIO53, COMM=GPIO54, NETWORK=GPIO55, ERROR=GPIO56"
echo "  E-Stop: CH1=GPIO57, CH2=GPIO58"
echo ""
echo "🧪 Test commands:"
echo "  # Test LED"
echo "  echo 1 > /sys/class/gpio/gpio52/value  # LED_POWER ON"
echo "  echo 0 > /sys/class/gpio/gpio52/value  # LED_POWER OFF"
echo ""
echo "  # Test Relay"
echo "  echo 1 > /sys/class/gpio/gpio51/value  # RELAY1 ON"
echo "  echo 0 > /sys/class/gpio/gpio51/value  # RELAY1 OFF"
echo ""
echo "  # Read E-Stop"
echo "  cat /sys/class/gpio/gpio57/value  # ESTOP_CH1"
echo "  cat /sys/class/gpio/gpio58/value  # ESTOP_CH2"
