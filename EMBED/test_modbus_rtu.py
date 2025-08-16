#!/usr/bin/env python3
"""
Test Modbus RTU cho UART1 chân 46, 47
"""

import serial
import time
import struct
import signal
import sys

def signal_handler(sig, frame):
    print('\n🛑 Dừng test Modbus RTU...')
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

def crc16_modbus(data):
    """Tính CRC16 Modbus"""
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
            crc &= 0xFFFF
    return crc

def build_modbus_frame(slave_addr, function, start_addr, quantity):
    """Tạo Modbus RTU frame"""
    # Modbus frame: [Slave][Function][StartAddr_H][StartAddr_L][Quantity_H][Quantity_L][CRC_L][CRC_H]
    frame = bytearray([
        slave_addr,           # Slave address
        function,             # Function code
        (start_addr >> 8) & 0xFF,  # Start address high
        start_addr & 0xFF,         # Start address low
        (quantity >> 8) & 0xFF,    # Quantity high
        quantity & 0xFF            # Quantity low
    ])
    
    # Thêm CRC
    crc = crc16_modbus(frame)
    frame.append(crc & 0xFF)       # CRC low
    frame.append((crc >> 8) & 0xFF) # CRC high
    
    return bytes(frame)

def parse_modbus_response(data):
    """Parse Modbus response"""
    if len(data) < 3:
        return None
    
    slave_addr = data[0]
    function = data[1]
    
    if function == 0x03:  # Read Holding Registers
        byte_count = data[2]
        if len(data) >= 3 + byte_count + 2:  # +2 for CRC
            registers = []
            for i in range(byte_count // 2):
                reg_value = (data[3 + i*2] << 8) | data[4 + i*2]
                registers.append(reg_value)
            return {
                'slave': slave_addr,
                'function': function,
                'registers': registers
            }
    
    return None

def test_modbus_rtu():
    """Test Modbus RTU trên UART1"""
    print("🔧 Test Modbus RTU trên UART1 chân 46, 47")
    print("=" * 60)
    
    # Cấu hình UART1
    ser = serial.Serial(
        port='/dev/ttyS1',
        baudrate=115200,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=1
    )
    
    print(f"✅ UART1 mở thành công: {ser.name}")
    print(f"📋 Cấu hình: {ser.baudrate} baud, {ser.bytesize} bits")
    print("=" * 60)
    
    # Test parameters
    slave_addresses = [1, 2, 3]  # Test với slave 1, 2, 3
    functions = [0x03, 0x04]     # Read Holding Registers, Read Input Registers
    start_addresses = [0, 100, 200]  # Test với các địa chỉ khác nhau
    quantities = [1, 2, 4]       # Số lượng register
    
    counter = 0
    success_count = 0
    
    try:
        while True:
            for slave in slave_addresses:
                for func in functions:
                    for start_addr in start_addresses:
                        for qty in quantities:
                            counter += 1
                            
                            # Tạo Modbus frame
                            frame = build_modbus_frame(slave, func, start_addr, qty)
                            
                            # Hiển thị frame
                            timestamp = time.strftime("%H:%M:%S")
                            print(f"[{timestamp}] #{counter:04d} 📤 Slave {slave}, Func {func:02X}, Addr {start_addr}, Qty {qty}")
                            print(f"           Frame: {' '.join([f'{b:02X}' for b in frame])}")
                            
                            # Gửi frame
                            ser.write(frame)
                            ser.flush()
                            
                            # Đọc response
                            response = ser.read(100)
                            if response:
                                print(f"           📥 Response: {' '.join([f'{b:02X}' for b in response])}")
                                
                                # Parse response
                                parsed = parse_modbus_response(response)
                                if parsed:
                                    print(f"           ✅ Parsed: Slave {parsed['slave']}, Func {parsed['function']:02X}, Regs {parsed['registers']}")
                                    success_count += 1
                                else:
                                    print(f"           ⚠️  Response không phải Modbus hợp lệ")
                            else:
                                print(f"           ❌ Không có response")
                            
                            print("-" * 60)
                            time.sleep(0.5)  # Delay giữa các frame
            
            # Thống kê
            print(f"\n📊 Thống kê: {success_count}/{counter} frames thành công")
            print("=" * 60)
            
            # Hỏi có tiếp tục không
            try:
                user_input = input("Nhấn Enter để tiếp tục, Ctrl+C để dừng: ")
                if user_input.lower() in ['q', 'quit', 'exit']:
                    break
            except KeyboardInterrupt:
                break
    
    except KeyboardInterrupt:
        pass
    finally:
        ser.close()
        print("✅ Đã đóng UART1")

if __name__ == "__main__":
    test_modbus_rtu()
