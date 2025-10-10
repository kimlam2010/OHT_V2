#!/bin/bash
# 🔍 RS485 Debug Script - OHT-50
# Phiên bản: 1.0
# Mục đích: Chẩn đoán vấn đề RS485 không có phản hồi từ slave

set -e

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}🔍 RS485 DEBUG SCRIPT - OHT-50${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# 1. Kiểm tra device tồn tại
echo -e "${YELLOW}📌 [1/8] Kiểm tra /dev/ttyOHT485...${NC}"
if [ -e /dev/ttyOHT485 ]; then
    ls -la /dev/ttyOHT485
    echo -e "${GREEN}✅ Device tồn tại${NC}"
else
    echo -e "${RED}❌ Device không tồn tại! Kiểm tra udev rules.${NC}"
    exit 1
fi
echo ""

# 2. Kiểm tra UART1 trong kernel
echo -e "${YELLOW}📌 [2/8] Kiểm tra UART1 trong kernel log...${NC}"
if dmesg | grep -q "ttyS1"; then
    dmesg | grep ttyS1 | tail -5
    echo -e "${GREEN}✅ UART1 đã được nhận diện${NC}"
else
    echo -e "${RED}❌ UART1 không xuất hiện trong kernel log!${NC}"
fi
echo ""

# 3. Kiểm tra cấu hình UART
echo -e "${YELLOW}📌 [3/8] Kiểm tra cấu hình UART hiện tại...${NC}"
stty -F /dev/ttyOHT485 -a
echo ""

# 4. Cấu hình lại UART chuẩn RS485 Modbus RTU
echo -e "${YELLOW}📌 [4/8] Cấu hình UART 115200 8N1...${NC}"
stty -F /dev/ttyOHT485 \
    115200 \
    cs8 \
    -cstopb \
    -parenb \
    -echo \
    raw
echo -e "${GREEN}✅ Đã cấu hình UART${NC}"
echo ""

# 5. Kiểm tra GPIO DE/RE
echo -e "${YELLOW}📌 [5/8] Kiểm tra GPIO cho DE/RE control...${NC}"
if command -v gpioinfo &> /dev/null; then
    gpioinfo | grep -E "GPIO1_D3|GPIO1_D2" || echo -e "${YELLOW}⚠️  Không tìm thấy GPIO info${NC}"
else
    echo -e "${YELLOW}⚠️  gpioinfo không có, cài đặt: sudo apt-get install gpiod${NC}"
fi
echo ""

# 6. Loopback test
echo -e "${YELLOW}📌 [6/8] Loopback test (nối A-B ngắn mạch)...${NC}"
echo -e "${YELLOW}Bạn cần ngắn mạch A-B để test!${NC}"
read -p "Đã ngắn mạch A-B? (y/n): " -n 1 -r
echo
if [[ $REPLY =~ ^[Yy]$ ]]; then
    echo "TEST123" > /dev/ttyOHT485 &
    sleep 0.2
    RECEIVED=$(timeout 1 cat /dev/ttyOHT485 || echo "TIMEOUT")
    if [[ "$RECEIVED" == *"TEST123"* ]]; then
        echo -e "${GREEN}✅ Loopback OK - UART hoạt động!${NC}"
    else
        echo -e "${RED}❌ Loopback FAILED - Kiểm tra hardware!${NC}"
        echo "Received: $RECEIVED"
    fi
else
    echo -e "${YELLOW}⚠️  Bỏ qua loopback test${NC}"
fi
echo ""

# 7. Test Modbus RTU read
echo -e "${YELLOW}📌 [7/8] Test Modbus RTU read slave...${NC}"
read -p "Nhập Slave Address (1-247, mặc định 1): " SLAVE_ADDR
SLAVE_ADDR=${SLAVE_ADDR:-1}

# Convert to hex
SLAVE_HEX=$(printf '%02x' $SLAVE_ADDR)

echo -e "Gửi Modbus RTU Read Holding Register request..."
echo -e "  Slave: $SLAVE_ADDR (0x$SLAVE_HEX)"
echo -e "  Function: 0x03 (Read Holding Registers)"
echo -e "  Address: 0x0000"
echo -e "  Count: 1 register"

# Modbus frame: [SlaveID] [Function] [AddrHi] [AddrLo] [CountHi] [CountLo] [CRC]
# CRC tính cho slave 1: 0x84 0x0A
# Bạn cần tính lại CRC cho slave khác!

if [ "$SLAVE_ADDR" -eq 1 ]; then
    echo -ne '\x01\x03\x00\x00\x00\x01\x84\x0A' > /dev/ttyOHT485
    echo "Đã gửi frame Modbus cho slave 1"
else
    echo -e "${YELLOW}⚠️  CRC chưa tính cho slave $SLAVE_ADDR, dùng modpoll thay thế!${NC}"
fi

echo "Đợi phản hồi (timeout 2s)..."
RESPONSE=$(timeout 2 cat /dev/ttyOHT485 | hexdump -C || echo "TIMEOUT")

if [[ "$RESPONSE" == "TIMEOUT" ]]; then
    echo -e "${RED}❌ TIMEOUT - Không có phản hồi từ slave!${NC}"
    echo ""
    echo -e "${YELLOW}Các nguyên nhân có thể:${NC}"
    echo "  1. Slave address sai"
    echo "  2. Slave baudrate khác 115200"
    echo "  3. Slave không được cấp nguồn"
    echo "  4. Wiring sai (A-B bị đảo)"
    echo "  5. Thiếu termination 120Ω"
    echo "  6. DE/RE không được control"
else
    echo -e "${GREEN}✅ Có phản hồi từ slave!${NC}"
    echo "$RESPONSE"
fi
echo ""

# 8. Tổng kết và khuyến nghị
echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}📋 TỔNG KẾT VÀ KHUYẾN NGHỊ${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""
echo -e "${YELLOW}🔧 Nếu vẫn không có phản hồi, kiểm tra:${NC}"
echo ""
echo -e "${YELLOW}1. Hardware:${NC}"
echo "   - Đo điện trở A-B: phải ~60Ω (2 termination)"
echo "   - Đo differential voltage idle: phải > 0.2V"
echo "   - Kiểm tra wiring: A-A, B-B (không chéo)"
echo "   - Kiểm tra nguồn slave: LED power có sáng?"
echo ""
echo -e "${YELLOW}2. Software:${NC}"
echo "   - Slave baudrate: PHẢI 115200, 8N1"
echo "   - Slave address: xác nhận lại từ manual"
echo "   - Timing: phải có gap T3.5 giữa frames"
echo "   - DE/RE control: kiểm tra GPIO toggle"
echo ""
echo -e "${YELLOW}3. Tools:${NC}"
echo "   - Dùng modpoll: sudo apt-get install modpoll"
echo "   - Dùng oscilloscope để xem tín hiệu"
echo "   - Dùng logic analyzer để decode Modbus"
echo ""
echo -e "${GREEN}✅ Script hoàn thành!${NC}"

