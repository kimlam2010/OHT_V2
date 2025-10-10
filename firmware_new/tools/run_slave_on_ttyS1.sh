#!/bin/bash
# 🔧 Chạy slave server trên /dev/ttyS1 (cùng bus với firmware)

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}╔═══════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║   🔧 CHẠY SLAVE SERVER TRÊN /dev/ttyS1                  ║${NC}"
echo -e "${CYAN}╚═══════════════════════════════════════════════════════════╝${NC}\n"

echo -e "${YELLOW}📋 VẤN ĐỀ HIỆN TẠI:${NC}"
echo -e "  ${RED}❌ Slave server: /dev/ttyUSB0 (USB-to-RS485)${NC}"
echo -e "  ${RED}❌ Firmware:     /dev/ttyS1 (Hardware RS485)${NC}"
echo -e "  ${RED}→ 2 port khác nhau, không giao tiếp được!${NC}\n"

echo -e "${GREEN}✅ GIẢI PHÁP:${NC}"
echo -e "  ${GREEN}→ Chạy slave server trên /dev/ttyS1${NC}"
echo -e "  ${GREEN}→ Firmware và slave cùng bus RS485${NC}\n"

echo -e "${YELLOW}1️⃣ Tắt slave server cũ (ttyUSB0)...${NC}"
pkill -9 -f "debug.sh"
sleep 1

echo -e "${YELLOW}2️⃣ Kiểm tra port /dev/ttyS1 và /dev/ttyOHT485...${NC}"
ls -la /dev/ttyS1 /dev/ttyOHT485 2>&1

echo -e "\n${YELLOW}3️⃣ Chạy slave server trên /dev/ttyS1...${NC}"
echo -e "${CYAN}Lệnh:${NC}"
echo -e "  ${CYAN}cd /home/orangepi/Desktop/OHT_V2/tools/slave_RS485${NC}"
echo -e "  ${CYAN}./debug.sh${NC}"
echo -e "  ${CYAN}Chọn option 2 (Modbus Debug)${NC}"
echo -e "  ${CYAN}Nhập port: /dev/ttyS1 hoặc /dev/ttyOHT485${NC}\n"

echo -e "${RED}⚠️  LƯU Ý:${NC}"
echo -e "  ${YELLOW}1. Cần chạy manual vì script cần interactive input${NC}"
echo -e "  ${YELLOW}2. Slave address: 2 (mặc định)${NC}"
echo -e "  ${YELLOW}3. Baudrate: 115200${NC}"
echo -e "  ${YELLOW}4. Sau khi slave chạy, mới start firmware!${NC}\n"

echo -e "${CYAN}════════════════════════════════════════════════════════${NC}\n"

