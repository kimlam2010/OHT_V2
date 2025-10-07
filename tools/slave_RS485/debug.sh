#!/usr/bin/env bash
# 🔍 RS485 DEBUG TOOL LAUNCHER

set -euo pipefail

echo ""
echo "╔═══════════════════════════════════════════════════════════╗"
echo "║       🔍 RS485 DEBUG TOOL - OHT-50                       ║"
echo "╚═══════════════════════════════════════════════════════════╝"
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Activate venv if exists
if [ -d ".venv" ]; then
    source .venv/bin/activate
    echo -e "${GREEN}✅ Virtual environment activated${NC}"
else
    echo -e "${YELLOW}⚠️  No venv found, using system python${NC}"
fi

echo ""
echo "Chọn debug mode:"
echo ""
echo -e "${BLUE}1)${NC} 📡 RAW Debug    - Hiển thị mọi byte nhận/gửi (Echo test)"
echo -e "${BLUE}2)${NC} 🔧 Modbus Debug - Parse Modbus frames + auto response"
echo -e "${BLUE}3)${NC} 🏃 Server Debug - Chạy pymodbus server với verbose logging"
echo -e "${BLUE}4)${NC} 📊 Port Info    - Kiểm tra thông tin serial port"
echo -e "${BLUE}q)${NC} 🚪 Quit"
echo ""

read -p "Chọn (1-4, q): " choice

case $choice in
    1)
        echo ""
        echo -e "${GREEN}🚀 Starting RAW Debug...${NC}"
        echo ""
        python3 debug_raw.py
        ;;
    2)
        echo ""
        echo -e "${GREEN}🚀 Starting Modbus Debug...${NC}"
        echo ""
        python3 debug_modbus.py
        ;;
    3)
        echo ""
        echo -e "${GREEN}🚀 Starting Pymodbus Server với DEBUG logging...${NC}"
        echo ""
        # Set debug logging
        export PYMODBUS_LOGLEVEL=DEBUG
        python3 -c "
import logging
logging.basicConfig(level=logging.DEBUG, format='[%(levelname)s] %(name)s: %(message)s')
logging.getLogger('pymodbus').setLevel(logging.DEBUG)
" &
        python3 server.py
        ;;
    4)
        echo ""
        echo -e "${GREEN}📊 Port Information:${NC}"
        echo ""
        
        # 🔍 AUTO-DETECT CH340
        echo -e "${BLUE}🔍 Detecting CH340 USB-to-Serial:${NC}"
        python3 port_detector.py
        
        echo ""
        echo -e "${BLUE}Available serial ports:${NC}"
        ls -la /dev/ttyUSB* 2>/dev/null || echo "  No /dev/ttyUSB* found"
        ls -la /dev/ttyS* 2>/dev/null | head -5 || echo "  No /dev/ttyS* found"
        
        echo ""
        PORT="${RS485_PORT:-}"
        if [ -z "$PORT" ]; then
            PORT=$(python3 port_detector.py 2>/dev/null | grep "Recommended port:" | awk '{print $3}')
        fi
        
        if [ -z "$PORT" ]; then
            PORT="/dev/ttyUSB1"
        fi
        
        echo -e "${BLUE}Checking port: $PORT${NC}"
        
        if [ -e "$PORT" ]; then
            echo -e "${GREEN}✅ Port exists${NC}"
            ls -la "$PORT"
            
            echo ""
            echo -e "${BLUE}Processes using this port:${NC}"
            sudo lsof "$PORT" 2>/dev/null || echo "  No processes found"
            
            echo ""
            echo -e "${BLUE}Port configuration:${NC}"
            stty -F "$PORT" 2>/dev/null || echo "  Cannot read configuration"
        else
            echo -e "${RED}❌ Port does not exist!${NC}"
        fi
        
        echo ""
        echo -e "${BLUE}Python serial detection:${NC}"
        python3 -c "
import serial.tools.list_ports
ports = list(serial.tools.list_ports.comports())
if ports:
    for p in ports:
        print(f'  • {p.device}: {p.description}')
else:
    print('  No serial ports detected by Python')
" 2>/dev/null || echo "  pyserial not installed"
        ;;
    q|Q)
        echo ""
        echo -e "${GREEN}👋 Bye!${NC}"
        exit 0
        ;;
    *)
        echo ""
        echo -e "${RED}❌ Invalid choice${NC}"
        exit 1
        ;;
esac

