#!/bin/bash
# 🚀 RUN FIRMWARE WITH CORRECT PORT MAPPING
# LiDAR: /dev/ttyUSB1 (CP210x)
# RS485 Slave: /dev/ttyUSB0 (CH341)

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
BLUE='\033[0;34m'
NC='\033[0m'

clear
echo -e "${CYAN}╔═══════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║   🚀 OHT-50 FIRMWARE - AUTO PORT DETECTION              ║${NC}"
echo -e "${CYAN}╚═══════════════════════════════════════════════════════════╝${NC}\n"

# 1. Check devices
echo -e "${YELLOW}📡 Checking USB Serial Devices...${NC}"
if [ -e /dev/ttyUSB0 ]; then
    USB0_INFO=$(dmesg | grep ttyUSB0 | tail -1)
    if [[ $USB0_INFO == *"ch341"* ]]; then
        echo -e "${GREEN}✅ /dev/ttyUSB0 - CH341 (RS485 Slave)${NC}"
    else
        echo -e "${YELLOW}⚠️  /dev/ttyUSB0 - Unknown device${NC}"
    fi
else
    echo -e "${RED}❌ /dev/ttyUSB0 - Not found${NC}"
fi

if [ -e /dev/ttyUSB1 ]; then
    USB1_INFO=$(dmesg | grep ttyUSB1 | tail -1)
    if [[ $USB1_INFO == *"cp210x"* ]]; then
        echo -e "${GREEN}✅ /dev/ttyUSB1 - CP210x (LiDAR)${NC}"
    else
        echo -e "${YELLOW}⚠️  /dev/ttyUSB1 - Unknown device${NC}"
    fi
else
    echo -e "${RED}❌ /dev/ttyUSB1 - Not found${NC}"
fi

if [ -e /dev/ttyS1 ]; then
    echo -e "${GREEN}✅ /dev/ttyS1 - UART1 Hardware (RS485 Master)${NC}"
    if [ -L /dev/ttyOHT485 ]; then
        TARGET=$(readlink -f /dev/ttyOHT485)
        echo -e "${BLUE}   → /dev/ttyOHT485 → $TARGET${NC}"
    fi
else
    echo -e "${RED}❌ /dev/ttyS1 - Not found${NC}"
fi

echo ""

# 2. Kill old processes
echo -e "${YELLOW}🧹 Cleaning up old processes...${NC}"
pkill -9 oht50_main 2>/dev/null
pkill -9 -f "slave_server" 2>/dev/null
sleep 1

# 3. Check if slave server should run
echo -e "\n${CYAN}═══════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}Do you want to start RS485 slave server on /dev/ttyUSB0?${NC}"
echo -e "${BLUE}(This simulates slave modules for testing)${NC}"
read -p "Start slave server? (y/n): " -n 1 -r
echo ""

if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo -e "${GREEN}🔧 Starting slave server in background...${NC}"
    cd /home/orangepi/Desktop/OHT_V2/firmware_new/tools
    nohup python3 slave_server_ttyS1.py > /tmp/slave_server.log 2>&1 &
    SLAVE_PID=$!
    sleep 2
    
    if ps -p $SLAVE_PID > /dev/null; then
        echo -e "${GREEN}✅ Slave server started (PID: $SLAVE_PID)${NC}"
        echo -e "${BLUE}   Log: tail -f /tmp/slave_server.log${NC}"
    else
        echo -e "${RED}❌ Failed to start slave server${NC}"
    fi
else
    echo -e "${YELLOW}⏭️  Skipping slave server${NC}"
fi

# 4. Run firmware
echo -e "\n${CYAN}═══════════════════════════════════════════════════════════${NC}"
echo -e "${YELLOW}🚀 Starting OHT-50 Firmware...${NC}\n"
sleep 1

cd /home/orangepi/Desktop/OHT_V2/firmware_new/build

# Check if need sudo for GPIO
if groups | grep -q gpio; then
    echo -e "${GREEN}✅ User has GPIO permissions${NC}"
    ./oht50_main
else
    echo -e "${YELLOW}⚠️  Need sudo for GPIO access${NC}"
    sudo ./oht50_main
fi

# 5. Cleanup on exit
echo -e "\n${YELLOW}🧹 Cleaning up...${NC}"
if [ -n "$SLAVE_PID" ]; then
    kill $SLAVE_PID 2>/dev/null
    echo -e "${GREEN}✅ Slave server stopped${NC}"
fi

echo -e "\n${CYAN}╔═══════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║   👋 OHT-50 FIRMWARE STOPPED                             ║${NC}"
echo -e "${CYAN}╚═══════════════════════════════════════════════════════════╝${NC}\n"

