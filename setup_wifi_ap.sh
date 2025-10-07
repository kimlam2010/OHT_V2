#!/bin/bash
# Setup WiFi AP for OHT-50
# Phiên bản: 1.0.0
# Ngày: 2025-10-07

set -e

echo "🔧 ============================================"
echo "📡 OHT-50 WiFi AP Setup Script"
echo "🔧 ============================================"
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Check if running as root
if [ "$EUID" -ne 0 ]; then 
    echo -e "${RED}❌ Vui lòng chạy với sudo:${NC}"
    echo -e "${YELLOW}   sudo bash $0${NC}"
    exit 1
fi

echo -e "${BLUE}📦 Bước 1: Cài đặt packages cần thiết...${NC}"
apt-get update -qq
apt-get install -y hostapd dnsmasq iw

echo ""
echo -e "${BLUE}🛑 Bước 2: Stop NetworkManager để không can thiệp...${NC}"
systemctl stop NetworkManager 2>/dev/null || true

echo ""
echo -e "${BLUE}🔌 Bước 3: Setup interface wlan0...${NC}"
# Bring wlan0 down
ip link set wlan0 down 2>/dev/null || true

# Set wlan0 to AP mode
iw dev wlan0 set type __ap 2>/dev/null || echo "Note: Could not set AP mode (may not be needed)"

# Bring wlan0 up
ip link set wlan0 up

# Set IP address
ip addr flush dev wlan0
ip addr add 192.168.4.1/24 dev wlan0

echo ""
echo -e "${BLUE}🚀 Bước 4: Start hostapd...${NC}"
if [ -f /tmp/hostapd.conf ]; then
    echo "   Config file: /tmp/hostapd.conf"
    cat /tmp/hostapd.conf
    echo ""
    
    # Kill existing hostapd
    pkill hostapd 2>/dev/null || true
    sleep 1
    
    # Start hostapd in background
    hostapd -B /tmp/hostapd.conf -P /tmp/hostapd.pid
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ hostapd started successfully!${NC}"
        echo "   PID: $(cat /tmp/hostapd.pid 2>/dev/null || echo 'N/A')"
    else
        echo -e "${RED}❌ Failed to start hostapd${NC}"
        exit 1
    fi
else
    echo -e "${RED}❌ Config file /tmp/hostapd.conf not found!${NC}"
    echo "   Vui lòng gọi API start WiFi AP trước:"
    echo -e "${YELLOW}   curl -X POST http://localhost:8080/api/v1/network/ap/start ...${NC}"
    exit 1
fi

echo ""
echo -e "${BLUE}🌐 Bước 5: Start dnsmasq (DHCP server)...${NC}"
if [ -f /tmp/dnsmasq.conf ]; then
    echo "   Config file: /tmp/dnsmasq.conf"
    
    # Kill existing dnsmasq on wlan0
    pkill -f "dnsmasq.*wlan0" 2>/dev/null || true
    sleep 1
    
    # Start dnsmasq
    dnsmasq -C /tmp/dnsmasq.conf --pid-file=/tmp/dnsmasq.pid
    
    if [ $? -eq 0 ]; then
        echo -e "${GREEN}✅ dnsmasq started successfully!${NC}"
        echo "   PID: $(cat /tmp/dnsmasq.pid 2>/dev/null || echo 'N/A')"
    else
        echo -e "${RED}❌ Failed to start dnsmasq${NC}"
    fi
else
    echo -e "${YELLOW}⚠️  dnsmasq config not found, skipping DHCP server${NC}"
fi

echo ""
echo -e "${GREEN}✅ ============================================${NC}"
echo -e "${GREEN}🎉 WiFi AP Setup Complete!${NC}"
echo -e "${GREEN}✅ ============================================${NC}"
echo ""
echo -e "${BLUE}📡 Thông tin WiFi AP:${NC}"
if [ -f /tmp/hostapd.conf ]; then
    SSID=$(grep "^ssid=" /tmp/hostapd.conf | cut -d= -f2)
    PASSWORD=$(grep "^wpa_passphrase=" /tmp/hostapd.conf | cut -d= -f2)
    CHANNEL=$(grep "^channel=" /tmp/hostapd.conf | cut -d= -f2)
    
    echo -e "   📶 SSID:     ${GREEN}$SSID${NC}"
    echo -e "   🔐 Password: ${GREEN}$PASSWORD${NC}"
    echo -e "   📻 Channel:  ${GREEN}$CHANNEL${NC}"
    echo -e "   🌐 IP:       ${GREEN}192.168.4.1${NC}"
fi

echo ""
echo -e "${BLUE}🔍 Kiểm tra trạng thái:${NC}"
echo -e "   ${YELLOW}ps aux | grep hostapd${NC}"
echo -e "   ${YELLOW}iw dev wlan0 info${NC}"
echo -e "   ${YELLOW}ip addr show wlan0${NC}"

echo ""
echo -e "${BLUE}📱 Kết nối từ điện thoại/laptop:${NC}"
echo -e "   1️⃣  Mở WiFi settings"
echo -e "   2️⃣  Tìm SSID: ${GREEN}$SSID${NC}"
echo -e "   3️⃣  Nhập password: ${GREEN}$PASSWORD${NC}"
echo -e "   4️⃣  Kết nối và sẽ được cấp IP: ${GREEN}192.168.4.x${NC}"

echo ""

