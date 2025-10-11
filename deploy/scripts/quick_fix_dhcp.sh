#!/bin/bash
# Quick fix DHCP for WiFi AP - NO SUDO PROMPT
# Run: sudo bash quick_fix_dhcp.sh

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}🚀 Quick Fix DHCP - OHT-50 WiFi AP${NC}"
echo ""

# Kill system dnsmasq
echo -e "${YELLOW}1️⃣  Stopping system dnsmasq...${NC}"
systemctl stop dnsmasq 2>/dev/null || true
killall dnsmasq 2>/dev/null || true
sleep 2

# Verify no dnsmasq running
if ps aux | grep -v grep | grep dnsmasq > /dev/null; then
    echo -e "${RED}⚠️  Killing remaining dnsmasq...${NC}"
    killall -9 dnsmasq 2>/dev/null || true
    sleep 1
fi

# Start dnsmasq for wlan0
echo -e "${YELLOW}2️⃣  Starting dnsmasq for wlan0...${NC}"
dnsmasq -C /tmp/dnsmasq.conf --pid-file=/tmp/dnsmasq_wlan0.pid 2>&1

sleep 2

# Check if running
if ps aux | grep -v grep | grep "dnsmasq.*wlan0" > /dev/null; then
    echo -e "${GREEN}✅ dnsmasq started successfully!${NC}"
    ps aux | grep -v grep | grep dnsmasq
else
    echo -e "${RED}❌ dnsmasq failed to start, trying alternative...${NC}"
    # Try with explicit options
    dnsmasq --interface=wlan0 \
            --dhcp-range=192.168.4.2,192.168.4.20,255.255.255.0,24h \
            --dhcp-option=3,192.168.4.1 \
            --dhcp-option=6,192.168.4.1 \
            --server=8.8.8.8 \
            --listen-address=192.168.4.1 \
            --bind-interfaces \
            --no-daemon &
    
    sleep 2
    echo -e "${GREEN}✅ dnsmasq started with explicit options!${NC}"
fi

echo ""
echo -e "${GREEN}🎉 DHCP Server is ready!${NC}"
echo -e "   📍 IP Range: ${YELLOW}192.168.4.2 - 192.168.4.20${NC}"
echo -e "   🌐 Gateway:  ${YELLOW}192.168.4.1${NC}"
echo -e "   ⏰ Lease:    ${YELLOW}24 hours${NC}"
echo ""
echo -e "${BLUE}📱 Bây giờ từ điện thoại:${NC}"
echo -e "   1️⃣  Forget/Quên WiFi OHT-50-Robot-AP"
echo -e "   2️⃣  Kết nối lại"
echo -e "   3️⃣  Sẽ tự động nhận IP: ${GREEN}192.168.4.x${NC}"
echo ""

