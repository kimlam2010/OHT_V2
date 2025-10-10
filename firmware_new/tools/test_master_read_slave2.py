#!/usr/bin/env python3
"""
🔍 TEST MASTER ĐỌC SLAVE ADDRESS 2
Test xem master /dev/ttyOHT485 có đọc được slave address 2 không
"""

import serial
import time
import sys
from typing import Optional

# ANSI Colors
RED = '\033[0;31m'
GREEN = '\033[0;32m'
YELLOW = '\033[1;33m'
BLUE = '\033[0;34m'
CYAN = '\033[0;36m'
NC = '\033[0m'  # No Color

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

def build_modbus_read_request(slave_addr: int, function: int, start_addr: int, count: int) -> bytes:
    """Xây dựng Modbus Read request"""
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
    """In hex với format đẹp"""
    hex_str = ' '.join(f'{b:02X}' for b in data)
    print(f"{prefix}{hex_str}")

def test_read_slave(port: str, slave_addr: int, baudrate: int = 115200):
    """Test đọc slave"""
    
    print(f"\n{BLUE}{'='*60}{NC}")
    print(f"{BLUE}🔍 TEST MASTER ĐỌC SLAVE ADDRESS {slave_addr}{NC}")
    print(f"{BLUE}{'='*60}{NC}\n")
    
    # Mở port
    print(f"{YELLOW}📡 Kết nối tới {port} @ {baudrate}bps...{NC}")
    try:
        ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=2.0,
            write_timeout=1.0
        )
        print(f"{GREEN}✅ Đã kết nối thành công!{NC}")
        print(f"{CYAN}📊 Port info: {ser}{NC}\n")
    except Exception as e:
        print(f"{RED}❌ Lỗi kết nối: {e}{NC}")
        return False
    
    try:
        # Flush buffer
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        time.sleep(0.1)
        
        # Test cases - Đọc nhiều ô nhớ hơn
        test_cases = [
            {
                'name': 'Đọc Holding Register 0 (1 register)',
                'function': 0x03,
                'start_addr': 0,
                'count': 1
            },
            {
                'name': 'Đọc Holding Register 0-9 (10 registers)',
                'function': 0x03,
                'start_addr': 0,
                'count': 10
            },
            {
                'name': 'Đọc Holding Register 0-19 (20 registers)',
                'function': 0x03,
                'start_addr': 0,
                'count': 20
            },
            {
                'name': 'Đọc Holding Register 0-49 (50 registers)',
                'function': 0x03,
                'start_addr': 0,
                'count': 50
            },
            {
                'name': 'Đọc Holding Register 100-109 (10 registers)',
                'function': 0x03,
                'start_addr': 100,
                'count': 10
            },
            {
                'name': 'Đọc Input Register 0-19 (20 registers)',
                'function': 0x04,
                'start_addr': 0,
                'count': 20
            },
        ]
        
        success_count = 0
        
        for i, test in enumerate(test_cases, 1):
            print(f"\n{YELLOW}{'='*60}{NC}")
            print(f"{YELLOW}📝 TEST {i}/{len(test_cases)}: {test['name']}{NC}")
            print(f"{YELLOW}{'='*60}{NC}")
            
            # Build request
            request = build_modbus_read_request(
                slave_addr,
                test['function'],
                test['start_addr'],
                test['count']
            )
            
            print(f"\n{CYAN}📤 Gửi request:{NC}")
            print_hex(request, f"   {CYAN}Hex: {NC}")
            print(f"   {CYAN}Slave: {NC}{slave_addr}")
            print(f"   {CYAN}Function: {NC}0x{test['function']:02X} ({test['name'].split()[1]} {test['name'].split()[2]})")
            print(f"   {CYAN}Address: {NC}{test['start_addr']}")
            print(f"   {CYAN}Count: {NC}{test['count']} register(s)")
            
            # Gửi
            ser.reset_input_buffer()
            bytes_sent = ser.write(request)
            ser.flush()
            print(f"\n{GREEN}✅ Đã gửi {bytes_sent} bytes{NC}")
            
            # Đợi response
            print(f"\n{YELLOW}⏳ Đợi response (timeout 2s)...{NC}")
            time.sleep(0.2)  # Cho slave xử lý
            
            response = ser.read(1000)  # Đọc tất cả có sẵn
            
            # ⚠️ KIỂM TRA ECHO/LOOPBACK
            if response == request:
                print(f"\n{RED}⚠️⚠️⚠️  CẢNH BÁO: PHÁT HIỆN ECHO/LOOPBACK!  ⚠️⚠️⚠️{NC}")
                print(f"{RED}Response giống hệt request → Đang nhận lại chính frame đã gửi!{NC}")
                print(f"{YELLOW}Nguyên nhân có thể:{NC}")
                print(f"  {YELLOW}1. DE/RE không được control đúng (transceiver ở chế độ receive khi transmit){NC}")
                print(f"  {YELLOW}2. Hardware loopback (A-B nối ngắn mạch){NC}")
                print(f"  {YELLOW}3. Full-duplex mode thay vì half-duplex{NC}")
                print(f"{RED}→ KHÔNG CÓ PHẢN HỒI THỰC SỰ TỪ SLAVE!{NC}\n")
                continue  # Bỏ qua test này
            
            if len(response) > 0:
                print(f"\n{GREEN}✅ CÓ PHẢN HỒI! ({len(response)} bytes){NC}")
                print(f"\n{CYAN}📥 Response:{NC}")
                print_hex(response, f"   {CYAN}Hex: {NC}")
                
                # Parse response
                if len(response) >= 3:
                    resp_slave = response[0]
                    resp_func = response[1]
                    
                    print(f"\n{CYAN}📊 Parse response:{NC}")
                    print(f"   {CYAN}Slave ID: {NC}{resp_slave} {'✅' if resp_slave == slave_addr else '❌ SAI!'}")
                    
                    # Check if error response
                    if resp_func & 0x80:
                        error_code = response[2] if len(response) > 2 else 0
                        print(f"   {RED}❌ LỖI MODBUS!{NC}")
                        print(f"   {RED}Function: {NC}0x{resp_func:02X} (Error)")
                        print(f"   {RED}Error Code: {NC}{error_code}")
                    else:
                        print(f"   {GREEN}Function: {NC}0x{resp_func:02X} ✅")
                        
                        if resp_func in [0x03, 0x04]:  # Read Holding/Input Registers
                            byte_count = response[2]
                            print(f"   {CYAN}Byte Count: {NC}{byte_count}")
                            
                            if len(response) >= 3 + byte_count:
                                data_bytes = response[3:3+byte_count]
                                print(f"   {GREEN}Data: {NC}", end='')
                                print_hex(data_bytes)
                                
                                # Parse thành register values (16-bit)
                                registers = []
                                for j in range(0, len(data_bytes), 2):
                                    if j+1 < len(data_bytes):
                                        reg_val = (data_bytes[j] << 8) | data_bytes[j+1]
                                        registers.append(reg_val)
                                
                                print(f"   {GREEN}Registers: {NC}{registers}")
                
                success_count += 1
                
            else:
                print(f"\n{RED}❌ TIMEOUT - Không có phản hồi!{NC}")
            
            # Đợi trước test tiếp theo
            time.sleep(0.5)
        
        # Tổng kết
        print(f"\n{BLUE}{'='*60}{NC}")
        print(f"{BLUE}📊 KẾT QUẢ TỔNG KẾT{NC}")
        print(f"{BLUE}{'='*60}{NC}\n")
        
        print(f"   {CYAN}Tổng số test: {NC}{len(test_cases)}")
        print(f"   {GREEN}Thành công: {NC}{success_count}")
        print(f"   {RED}Thất bại: {NC}{len(test_cases) - success_count}")
        
        if success_count == len(test_cases):
            print(f"\n{GREEN}✅ TẤT CẢ TESTS PASS!{NC}")
            print(f"{GREEN}Master có thể giao tiếp với slave address {slave_addr} thành công!{NC}\n")
            return True
        elif success_count > 0:
            print(f"\n{YELLOW}⚠️  MỘT SỐ TESTS PASS{NC}")
            print(f"{YELLOW}Slave hoạt động nhưng có vấn đề với một số functions.{NC}\n")
            return True
        else:
            print(f"\n{RED}❌ TẤT CẢ TESTS FAIL!{NC}")
            print(f"{RED}Master KHÔNG thể giao tiếp với slave address {slave_addr}!{NC}\n")
            return False
            
    except Exception as e:
        print(f"\n{RED}❌ Lỗi: {e}{NC}")
        import traceback
        traceback.print_exc()
        return False
    finally:
        ser.close()
        print(f"{YELLOW}🔌 Đã đóng kết nối{NC}\n")

def main():
    """Main function"""
    
    print(f"\n{BLUE}╔═══════════════════════════════════════════════════════════╗{NC}")
    print(f"{BLUE}║   🔍 TEST MASTER ĐỌC SLAVE - OHT-50                     ║{NC}")
    print(f"{BLUE}╚═══════════════════════════════════════════════════════════╝{NC}\n")
    
    # Cấu hình
    master_port = "/dev/ttyOHT485"  # Hoặc /dev/ttyS1
    slave_addr = 2
    baudrate = 115200
    
    print(f"{CYAN}📋 Cấu hình:{NC}")
    print(f"   {CYAN}Master Port: {NC}{master_port}")
    print(f"   {CYAN}Slave Address: {NC}{slave_addr}")
    print(f"   {CYAN}Baudrate: {NC}{baudrate}")
    
    # Kiểm tra port tồn tại
    import os
    if not os.path.exists(master_port):
        print(f"\n{RED}❌ Port {master_port} không tồn tại!{NC}")
        
        # Thử /dev/ttyS1
        alt_port = "/dev/ttyS1"
        if os.path.exists(alt_port):
            print(f"{YELLOW}⚠️  Thử {alt_port} thay thế...{NC}")
            master_port = alt_port
        else:
            print(f"{RED}❌ {alt_port} cũng không tồn tại!{NC}")
            sys.exit(1)
    
    # Chạy test
    success = test_read_slave(master_port, slave_addr, baudrate)
    
    if not success:
        print(f"\n{YELLOW}💡 GỢI Ý KHẮC PHỤC:{NC}")
        print(f"{YELLOW}1. Kiểm tra slave có đang chạy không?{NC}")
        print(f"{YELLOW}2. Kiểm tra wiring RS485: A-A, B-B{NC}")
        print(f"{YELLOW}3. Kiểm tra termination 120Ω ở 2 đầu{NC}")
        print(f"{YELLOW}4. Kiểm tra GND chung{NC}")
        print(f"{YELLOW}5. Kiểm tra baudrate master = slave (115200){NC}")
        print(f"{YELLOW}6. Kiểm tra slave address đúng (2){NC}")
        print(f"{YELLOW}7. Kiểm tra DE/RE GPIO control{NC}\n")
        
        sys.exit(1)
    
    sys.exit(0)

if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print(f"\n\n{YELLOW}⚠️  Đã bị ngắt bởi người dùng{NC}\n")
        sys.exit(0)

