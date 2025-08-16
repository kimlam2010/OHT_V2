#!/usr/bin/env python3
"""
Monitor UART1 để xem dữ liệu nhận được
"""

import serial
import time
import signal
import sys

def signal_handler(sig, frame):
    print('\n🛑 Dừng monitor UART1...')
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

def monitor_uart1():
    """Monitor UART1 liên tục"""
    print('📥 Bắt đầu monitor UART1...')
    print('Nhấn Ctrl+C để dừng')
    print('=' * 50)
    
    try:
        ser = serial.Serial(
            port='/dev/ttyS1',
            baudrate=115200,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1
        )
        
        print(f'✅ UART1 mở thành công: {ser.name}')
        print(f'📋 Cấu hình: {ser.baudrate} baud, {ser.bytesize} bits')
        print('=' * 50)
        
        counter = 0
        while True:
            try:
                # Đọc dữ liệu
                data = ser.read(100)
                if data:
                    counter += 1
                    timestamp = time.strftime("%H:%M:%S.%f")[:-3]
                    print(f'[{timestamp}] #{counter:04d} 📥 Received: {data}')
                    
                    # Hiển thị hex dump
                    hex_data = ' '.join([f'{b:02X}' for b in data])
                    print(f'           Hex: {hex_data}')
                    
                    # Hiển thị ASCII
                    ascii_data = ''.join([chr(b) if 32 <= b <= 126 else '.' for b in data])
                    print(f'         ASCII: {ascii_data}')
                    print('-' * 50)
                
                time.sleep(0.1)  # Đọc mỗi 100ms
                
            except KeyboardInterrupt:
                break
            except Exception as e:
                print(f'❌ Lỗi đọc: {e}')
                break
        
        ser.close()
        print('✅ Đã dừng monitor UART1')
        
    except Exception as e:
        print(f'❌ Lỗi mở UART1: {e}')

if __name__ == "__main__":
    monitor_uart1()
