#!/bin/bash
# 🔧 TẮT FIRMWARE VÀ TEST SLAVE

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
CYAN='\033[0;36m'
NC='\033[0m'

echo -e "${CYAN}╔═══════════════════════════════════════════════════════════╗${NC}"
echo -e "${CYAN}║   🔧 KIỂM TRA VÀ FIX PORT CONFLICT                      ║${NC}"
echo -e "${CYAN}╚═══════════════════════════════════════════════════════════╝${NC}\n"

echo -e "${YELLOW}1️⃣ Tắt firmware (oht50_main)...${NC}"
pkill -9 -f "oht50_main" 2>/dev/null
sleep 1
if pgrep -f "oht50_main" > /dev/null; then
    echo -e "${RED}❌ Firmware vẫn còn chạy!${NC}"
else
    echo -e "${GREEN}✅ Firmware đã tắt${NC}"
fi

echo -e "\n${YELLOW}2️⃣ Kiểm tra slave server...${NC}"
if pgrep -f "debug.sh" > /dev/null; then
    echo -e "${GREEN}✅ Slave server đang chạy${NC}"
else
    echo -e "${RED}❌ Slave server không chạy${NC}"
    echo -e "${YELLOW}   Cần bật lại slave server!${NC}"
fi

echo -e "\n${YELLOW}3️⃣ Kiểm tra port /dev/ttyOHT485...${NC}"
if lsof /dev/ttyOHT485 2>/dev/null; then
    echo -e "${RED}❌ Port đang được sử dụng!${NC}"
else
    echo -e "${GREEN}✅ Port free${NC}"
fi

echo -e "\n${YELLOW}4️⃣ Test lại với Python...${NC}"
sleep 1
cd /home/orangepi/Desktop/OHT_V2/firmware_new/tools
python3 test_register_0x0100.py

echo -e "\n${CYAN}═══════════════════════════════════════════════════════════${NC}"
echo -e "${CYAN}📋 KẾT LUẬN${NC}"
echo -e "${CYAN}═══════════════════════════════════════════════════════════${NC}\n"

echo -e "${YELLOW}NẾU TEST OK:${NC}"
echo -e "  ${GREEN}→ Vấn đề là firmware và Python cùng dùng 1 port${NC}"
echo -e "  ${GREEN}→ Cần phải tắt firmware trước khi test với Python${NC}\n"

echo -e "${YELLOW}NẾU VẪN TIMEOUT:${NC}"
echo -e "  ${CYAN}→ Kiểm tra slave server có đang chạy không${NC}"
echo -e "  ${CYAN}→ Kiểm tra slave có lắng nghe đúng port không${NC}\n"

