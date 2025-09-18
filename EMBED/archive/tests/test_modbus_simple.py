#!/usr/bin/env python3
"""
Test Modbus RTU đơn giản cho UART1
"""

import serial
import time
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

def test_modbus_simple():
    """Test Modbus RTU đơn giản"""
    print("🔧 Test Modbus RTU đơn giản trên UART1")
    print("=" * 50)
    
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
    print("=" * 50)
    
    # Test cases
    test_cases = [
        # [Slave, Function, StartAddr, Quantity, Description]
        [1, 0x03, 0, 1, "Read Holding Register 0"],
        [2, 0x03, 0, 1, "Read Holding Register 0"],
        [1, 0x04, 0, 1, "Read Input Register 0"],
        [2, 0x04, 0, 1, "Read Input Register 0"],
        [1, 0x03, 100, 2, "Read Holding Registers 100-101"],
        [2, 0x03, 100, 2, "Read Holding Registers 100-101"],
    ]
    
    counter = 0
    
    try:
        for slave, func, addr, qty, desc in test_cases:
            counter += 1
            
            # Tạo frame
            frame = bytearray([
                slave,           # Slave address
                func,            # Function code
                (addr >> 8) & 0xFF,  # Start address high
                addr & 0xFF,          # Start address low
                (qty >> 8) & 0xFF,    # Quantity high
                qty & 0xFF            # Quantity low
            ])
            
            # Thêm CRC
            crc = crc16_modbus(frame)
            frame.append(crc & 0xFF)
            frame.append((crc >> 8) & 0xFF)
            
            # Hiển thị
            timestamp = time.strftime("%H:%M:%S")
            print(f"[{timestamp}] #{counter} 📤 {desc}")
            print(f"           Slave: {slave}, Func: {func:02X}, Addr: {addr}, Qty: {qty}")
            print(f"           Frame: {' '.join([f'{b:02X}' for b in frame])}")
            
            # Gửi
            ser.write(frame)
            ser.flush()
            
            # Đọc response
            response = ser.read(100)
            if response:
                print(f"           📥 Response: {' '.join([f'{b:02X}' for b in response])}")
                
                # Kiểm tra CRC
                if len(response) >= 4:
                    received_crc = response[-2] | (response[-1] << 8)
                    calculated_crc = crc16_modbus(response[:-2])
                    if received_crc == calculated_crc:
                        print(f"           ✅ CRC OK")
                    else:
                        print(f"           ❌ CRC Error")
            else:
                print(f"           ❌ Không có response")
            
            print("-" * 50)
            time.sleep(1)  # Delay 1 giây
    
    except KeyboardInterrupt:
        pass
    finally:
        ser.close()
        print("✅ Đã đóng UART1")

if __name__ == "__main__":
    test_modbus_simple()
