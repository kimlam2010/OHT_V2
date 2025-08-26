#!/usr/bin/env python3
"""
Script gửi "hello" liên tục ra tty0
Sử dụng: python3 send_hello_tty0.py
"""

import serial
import time
import sys

def send_hello_continuous():
    """Gửi 'hello' liên tục ra tty0"""
    try:
        # Mở kết nối serial với ttyS0 (UART1)
        ser = serial.Serial(
            port='/dev/ttyS0',
            baudrate=115200,
            timeout=1,
            write_timeout=1
        )
        
        print("Đã kết nối ttyS0 (UART1). Bắt đầu gửi 'hello'...")
        print("Nhấn Ctrl+C để dừng")
        
        count = 0
        while True:
            try:
                # Gửi "hello" với số thứ tự
                message = f"hello {count}\n"
                ser.write(message.encode('utf-8'))
                ser.flush()  # Đảm bảo dữ liệu được gửi ngay
                
                print(f"Đã gửi: {message.strip()}")
                count += 1
                
                # Delay 1 giây
                time.sleep(1)
                
            except serial.SerialTimeoutException:
                print("Timeout khi gửi dữ liệu")
                continue
                
    except serial.SerialException as e:
        print(f"Lỗi kết nối serial: {e}")
        print("Kiểm tra quyền truy cập ttyS0 hoặc thiết bị có tồn tại không")
        sys.exit(1)
        
    except KeyboardInterrupt:
        print("\nĐã dừng bởi người dùng")
        if 'ser' in locals():
            ser.close()
            print("Đã đóng kết nối serial")
        sys.exit(0)
        
    except Exception as e:
        print(f"Lỗi không xác định: {e}")
        if 'ser' in locals():
            ser.close()
        sys.exit(1)

if __name__ == "__main__":
    send_hello_continuous()
