#!/usr/bin/env python3
"""
Gửi UART1 liên tục từ terminal
"""

import serial
import time
import signal
import sys

def signal_handler(sig, frame):
    print('\n🛑 Dừng gửi UART1...')
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

def send_uart1():
    """Gửi UART1 liên tục"""
    print('📤 Bắt đầu gửi UART1 liên tục...')
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
                # Tạo message test
                timestamp = time.strftime("%H:%M:%S")
                message = f'UART1 Test #{counter:04d} - {timestamp}\n'
                
                # Gửi dữ liệu
                ser.write(message.encode())
                print(f'[{timestamp}] #{counter:04d} 📤 Sent: {message.strip()}')
                
                # Đọc response nếu có
                response = ser.read(100)
                if response:
                    print(f'           📥 Received: {response}')
                
                counter += 1
                time.sleep(1)  # Gửi mỗi giây
                
            except KeyboardInterrupt:
                break
            except Exception as e:
                print(f'❌ Lỗi: {e}')
                break
        
        ser.close()
        print('✅ Đã dừng gửi UART1')
        
    except Exception as e:
        print(f'❌ Lỗi mở UART1: {e}')

if __name__ == "__main__":
    send_uart1()
