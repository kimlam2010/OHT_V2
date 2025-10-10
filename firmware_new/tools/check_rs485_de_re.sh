#!/bin/bash
# 🔍 Kiểm tra RS485 DE/RE GPIO control
# Mục đích: Xác định xem DE/RE có được control đúng không

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m'

echo -e "${BLUE}╔═══════════════════════════════════════════════════════════╗${NC}"
echo -e "${BLUE}║   🔍 KIỂM TRA RS485 DE/RE CONTROL                       ║${NC}"
echo -e "${BLUE}╚═══════════════════════════════════════════════════════════╝${NC}\n"

echo -e "${YELLOW}📋 VẤN ĐỀ: Master đang nhận ECHO của chính frame gửi đi${NC}"
echo -e "${YELLOW}   → RS485 transceiver ở chế độ RECEIVE khi TRANSMIT${NC}"
echo -e "${YELLOW}   → Cần control DE/RE pin để chuyển TX/RX mode${NC}\n"

echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}🔍 BƯỚC 1: Kiểm tra GPIO DE/RE có được export không${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}\n"

# Kiểm tra gpioinfo
if command -v gpioinfo &> /dev/null; then
    echo -e "${GREEN}✅ gpioinfo có sẵn${NC}\n"
    
    echo -e "${CYAN}Tìm GPIO1_D3 (Relay1) và GPIO1_D2 (Relay2):${NC}"
    gpioinfo | grep -E "GPIO1_D3|GPIO1_D2|line.*27|line.*26"
    
    echo -e "\n${CYAN}Hoặc tìm theo gpiochip1:${NC}"
    gpioinfo gpiochip1 2>/dev/null | head -30
else
    echo -e "${RED}❌ gpioinfo không có${NC}"
    echo -e "${YELLOW}Cài đặt: sudo apt-get install gpiod${NC}\n"
fi

echo -e "\n${BLUE}═══════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}🔍 BƯỚC 2: Kiểm tra device tree có enable RS485 mode không${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}\n"

if [ -d /proc/device-tree/serial@feb50000 ]; then
    echo -e "${GREEN}✅ UART1 (serial@feb50000) có trong device tree${NC}"
    
    if [ -f /proc/device-tree/serial@feb50000/linux,rs485-enabled-at-boot-time ]; then
        echo -e "${GREEN}✅ RS485 mode được enable trong device tree!${NC}"
    else
        echo -e "${RED}❌ RS485 mode KHÔNG được enable trong device tree!${NC}"
        echo -e "${YELLOW}Cần thêm vào device tree overlay:${NC}"
        echo -e "${YELLOW}   linux,rs485-enabled-at-boot-time;${NC}"
        echo -e "${YELLOW}   rs485-rts-active-low;${NC}"
    fi
    
    if [ -d /proc/device-tree/serial@feb50000/rs485-rts-delay ]; then
        echo -e "${CYAN}RS485 RTS delay:${NC}"
        cat /proc/device-tree/serial@feb50000/rs485-rts-delay/* 2>/dev/null | hexdump -C
    fi
else
    echo -e "${YELLOW}⚠️  Device tree node không tìm thấy${NC}"
fi

echo -e "\n${BLUE}═══════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}🔍 BƯỚC 3: Kiểm tra RS485 mode trong driver${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}\n"

# Kiểm tra RS485 ioctl
if [ -e /dev/ttyS1 ]; then
    echo -e "${GREEN}✅ /dev/ttyS1 tồn tại${NC}\n"
    
    # Dùng Python để check RS485 config
    python3 << 'EOF'
import serial
import fcntl
import struct
import sys

try:
    ser = serial.Serial('/dev/ttyS1', 115200)
    
    # TIOCGRS485 = 0x542E (get RS485 config)
    TIOCGRS485 = 0x542E
    
    # struct serial_rs485 (simplified)
    # flags (4 bytes) + delay_rts_before_send (4 bytes) + delay_rts_after_send (4 bytes) + padding
    buf = bytearray(32)
    
    try:
        result = fcntl.ioctl(ser.fileno(), TIOCGRS485, buf)
        flags = struct.unpack('I', buf[0:4])[0]
        delay_before = struct.unpack('I', buf[4:8])[0]
        delay_after = struct.unpack('I', buf[8:12])[0]
        
        print(f"📊 RS485 Configuration:")
        print(f"   Flags: 0x{flags:08X}")
        print(f"   Delay RTS before send: {delay_before} ms")
        print(f"   Delay RTS after send: {delay_after} ms")
        
        # Check flags
        SER_RS485_ENABLED = 0x01
        SER_RS485_RTS_ON_SEND = 0x02
        SER_RS485_RTS_AFTER_SEND = 0x04
        SER_RS485_RX_DURING_TX = 0x10
        
        if flags & SER_RS485_ENABLED:
            print(f"   ✅ RS485 mode ENABLED")
        else:
            print(f"   ❌ RS485 mode DISABLED")
            
        if flags & SER_RS485_RTS_ON_SEND:
            print(f"   ✅ RTS active ON send")
        else:
            print(f"   ⚠️  RTS inactive ON send")
            
        if flags & SER_RS485_RX_DURING_TX:
            print(f"   ⚠️  RX DURING TX enabled (sẽ nhận echo!)")
        else:
            print(f"   ✅ RX DURING TX disabled (không nhận echo)")
            
    except OSError as e:
        print(f"❌ Không thể get RS485 config: {e}")
        print(f"   → Driver không hỗ trợ RS485 mode hoặc chưa enable")
    
    ser.close()
    
except Exception as e:
    print(f"❌ Lỗi: {e}")
    sys.exit(1)
EOF
else
    echo -e "${RED}❌ /dev/ttyS1 không tồn tại!${NC}"
fi

echo -e "\n${BLUE}═══════════════════════════════════════════════════════════${NC}"
echo -e "${BLUE}📋 KHUYẾN NGHỊ${NC}"
echo -e "${BLUE}═══════════════════════════════════════════════════════════${NC}\n"

echo -e "${YELLOW}1. NẾU RS485 mode DISABLED:${NC}"
echo -e "   ${CYAN}→ Cần enable RS485 mode trong device tree hoặc runtime${NC}"
echo -e "   ${CYAN}→ Hoặc control DE/RE thủ công bằng GPIO${NC}\n"

echo -e "${YELLOW}2. NẾU RX DURING TX enabled:${NC}"
echo -e "   ${CYAN}→ Đây là nguyên nhân gây ECHO!${NC}"
echo -e "   ${CYAN}→ Cần disable flag này${NC}\n"

echo -e "${YELLOW}3. NẾU không có RS485 support trong driver:${NC}"
echo -e "   ${CYAN}→ Cần control DE/RE thủ công:${NC}"
echo -e "   ${CYAN}   • Set DE/RE = HIGH trước khi gửi${NC}"
echo -e "   ${CYAN}   • Đợi gửi xong${NC}"
echo -e "   ${CYAN}   • Set DE/RE = LOW để nhận${NC}\n"

echo -e "${GREEN}✅ Script hoàn thành!${NC}\n"

