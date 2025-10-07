#!/bin/bash
# Fix dnsmasq for WiFi AP
# Phiên bản: 1.0.0

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}🔧 Fix dnsmasq for WiFi AP...${NC}"
echo ""

if [ "$EUID" -ne 0 ]; then 
    echo -e "${RED}❌ Cần sudo: sudo bash $0${NC}"
    exit 1
fi

echo -e "${YELLOW}⚠️  Stopping system dnsmasq...${NC}"
systemctl stop dnsmasq 2>/dev/null || true
systemctl disable dnsmasq 2>/dev/null || true

echo -e "${YELLOW}🔪 Killing all dnsmasq processes...${NC}"
pkill dnsmasq 2>/dev/null || true
sleep 2

echo -e "${BLUE}🚀 Starting dnsmasq for wlan0 AP...${NC}"
if [ ! -f /tmp/dnsmasq.conf ]; then
    echo -e "${RED}❌ /tmp/dnsmasq.conf not found!${NC}"
    exit 1
fi

# Start dnsmasq with no IPv6
dnsmasq -C /tmp/dnsmasq.conf --pid-file=/tmp/dnsmasq.pid --no-daemon &
DNSMASQ_PID=$!
sleep 2

if ps -p $DNSMASQ_PID > /dev/null; then
    echo -e "${GREEN}✅ dnsmasq started! PID: $DNSMASQ_PID${NC}"
    echo $DNSMASQ_PID > /tmp/dnsmasq.pid
    
    # Move to background
    disown $DNSMASQ_PID
else
    echo -e "${RED}❌ dnsmasq failed to start${NC}"
    exit 1
fi

echo ""
echo -e "${GREEN}✅ DHCP Server ready!${NC}"
echo -e "   📍 IP Range: ${YELLOW}192.168.4.2 - 192.168.4.20${NC}"
echo -e "   ⏰ Lease: ${YELLOW}24 hours${NC}"
echo ""

