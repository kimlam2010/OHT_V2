#!/usr/bin/env python3
"""
🔍 TEST REGISTER 0x0100 - Kiểm tra xem slave có data ở địa chỉ này không
"""

import serial
import time

# ANSI Colors
RED = '\033[0;31m'
GREEN = '\033[0;32m'
YELLOW = '\033[1;33m'
CYAN = '\033[0;36m'
NC = '\033[0m'

def calculate_crc16_modbus(data: bytes) -> bytes:
    """Tính CRC-16 Modbus"""
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x0001:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc.to_bytes(2, 'little')

def build_request(slave_addr: int, function: int, start_addr: int, count: int) -> bytes:
    """Build Modbus request"""
    frame = bytes([
        slave_addr,
        function,
        (start_addr >> 8) & 0xFF,
        start_addr & 0xFF,
        (count >> 8) & 0xFF,
        count & 0xFF
    ])
    crc = calculate_crc16_modbus(frame)
    return frame + crc

def print_hex(data: bytes, prefix: str = ""):
    """Print hex"""
    hex_str = ' '.join(f'{b:02X}' for b in data)
    print(f"{prefix}{hex_str}")

print(f"\n{CYAN}╔═══════════════════════════════════════════════════════════╗{NC}")
print(f"{CYAN}║   🔍 TEST REGISTER 0x0100 vs 0x0000                     ║{NC}")
print(f"{CYAN}╚═══════════════════════════════════════════════════════════╝{NC}\n")

port = "/dev/ttyOHT485"
slave_addr = 2

try:
    ser = serial.Serial(
        port=port,
        baudrate=115200,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=2.0
    )
    print(f"{GREEN}✅ Kết nối {port} thành công{NC}\n")
    
    # Test register 0x0000
    print(f"{YELLOW}{'='*60}{NC}")
    print(f"{YELLOW}TEST 1: Đọc register 0x0000 (như Python test){NC}")
    print(f"{YELLOW}{'='*60}{NC}")
    
    request = build_request(slave_addr, 0x03, 0x0000, 1)
    print(f"{CYAN}Request:{NC}")
    print_hex(request, f"  ")
    
    ser.reset_input_buffer()
    ser.write(request)
    ser.flush()
    time.sleep(0.2)
    
    response = ser.read(1000)
    if len(response) > 0:
        print(f"{GREEN}✅ Có phản hồi ({len(response)} bytes):{NC}")
        print_hex(response, f"  ")
    else:
        print(f"{RED}❌ TIMEOUT - Không có phản hồi{NC}")
    
    # Test register 0x0100
    print(f"\n{YELLOW}{'='*60}{NC}")
    print(f"{YELLOW}TEST 2: Đọc register 0x0100 (như Firmware){NC}")
    print(f"{YELLOW}{'='*60}{NC}")
    
    request = build_request(slave_addr, 0x03, 0x0100, 1)
    print(f"{CYAN}Request:{NC}")
    print_hex(request, f"  ")
    
    ser.reset_input_buffer()
    ser.write(request)
    ser.flush()
    time.sleep(0.2)
    
    response = ser.read(1000)
    if len(response) > 0:
        print(f"{GREEN}✅ Có phản hồi ({len(response)} bytes):{NC}")
        print_hex(response, f"  ")
        
        # Parse response
        if len(response) >= 5:
            if response[1] & 0x80:  # Error
                error_code = response[2]
                error_names = {
                    0x01: "ILLEGAL_FUNCTION",
                    0x02: "ILLEGAL_DATA_ADDRESS",
                    0x03: "ILLEGAL_DATA_VALUE",
                    0x04: "SLAVE_DEVICE_FAILURE"
                }
                print(f"\n{RED}❌ MODBUS ERROR:{NC}")
                print(f"  {RED}Error Code: {error_code} ({error_names.get(error_code, 'UNKNOWN')}){NC}")
    else:
        print(f"{RED}❌ TIMEOUT - Không có phản hồi{NC}")
    
    ser.close()
    
    print(f"\n{CYAN}{'='*60}{NC}")
    print(f"{CYAN}KẾT LUẬN:{NC}")
    print(f"{CYAN}{'='*60}{NC}")
    print(f"\n{YELLOW}Nếu 0x0000 OK nhưng 0x0100 TIMEOUT hoặc ERROR:${NC}")
    print(f"  {CYAN}→ Slave không có data ở register 0x0100${NC}")
    print(f"  {CYAN}→ Firmware cần scan register 0x0000 thay vì 0x0100${NC}\n")
    
except Exception as e:
    print(f"{RED}❌ Lỗi: {e}{NC}")

