#!/usr/bin/env python3
"""
Test Modbus RTU với loopback cho UART1
"""

import serial
import time
import signal
import sys

def signal_handler(sig, frame):
    print('\n🛑 Dừng test Modbus RTU loopback...')
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

def test_modbus_loopback():
    """Test Modbus RTU với loopback"""
    print("🔧 Test Modbus RTU loopback trên UART1")
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
    print("=" * 60)
    
    # Test loopback - gửi và nhận lại chính frame đó
    test_frames = [
        # [Slave, Function, StartAddr, Quantity, Description]
        [1, 0x03, 0, 1, "Read Holding Register 0"],
        [2, 0x03, 0, 1, "Read Holding Register 0"],
        [1, 0x04, 0, 1, "Read Input Register 0"],
        [2, 0x04, 0, 1, "Read Input Register 0"],
        [1, 0x06, 0, 1234, "Write Single Register 0 = 1234"],
        [2, 0x06, 0, 5678, "Write Single Register 0 = 5678"],
    ]
    
    counter = 0
    loopback_count = 0
    
    try:
        for slave, func, addr, qty, desc in test_frames:
            counter += 1
            
            # Tạo frame
            if func == 0x06:  # Write Single Register
                frame = bytearray([
                    slave,           # Slave address
                    func,            # Function code
                    (addr >> 8) & 0xFF,  # Register address high
                    addr & 0xFF,          # Register address low
                    (qty >> 8) & 0xFF,    # Value high
                    qty & 0xFF            # Value low
                ])
            else:  # Read functions
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
                
                # Kiểm tra loopback (response giống frame gửi)
                if response == frame:
                    print(f"           ✅ Loopback detected!")
                    loopback_count += 1
                else:
                    # Kiểm tra CRC
                    if len(response) >= 4:
                        received_crc = response[-2] | (response[-1] << 8)
                        calculated_crc = crc16_modbus(response[:-2])
                        if received_crc == calculated_crc:
                            print(f"           ✅ Valid Modbus response")
                        else:
                            print(f"           ❌ CRC Error")
                    else:
                        print(f"           ⚠️  Invalid response length")
            else:
                print(f"           ❌ Không có response")
            
            print("-" * 60)
            time.sleep(1)  # Delay 1 giây
        
        # Thống kê
        print(f"\n📊 Thống kê:")
        print(f"   Tổng frames gửi: {counter}")
        print(f"   Loopback detected: {loopback_count}")
        print(f"   Response rate: {loopback_count/counter*100:.1f}%")
        print("=" * 60)
    
    except KeyboardInterrupt:
        pass
    finally:
        ser.close()
        print("✅ Đã đóng UART1")

if __name__ == "__main__":
    test_modbus_loopback()
