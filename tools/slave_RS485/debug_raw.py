#!/usr/bin/env python3
"""
🔍 RS485 RAW DEBUG TOOL
Hiển thị mọi data nhận/gửi trên RS485
"""

import serial
import time
import sys
import os

# 🔍 AUTO-DETECT CH340 PORT
try:
    from port_detector import get_best_ch340_port
    
    PORT = os.environ.get('RS485_PORT')
    if not PORT:
        PORT = get_best_ch340_port()
        if PORT:
            print(f"✅ Tự động detect CH340: {PORT}")
        else:
            print(f"❌ Không tìm thấy CH340!")
            print(f"💡 Set thủ công: export RS485_PORT=/dev/ttyUSB0")
            sys.exit(1)
    else:
        print(f"📌 Dùng port đã set: {PORT}")
except ImportError:
    PORT = os.environ.get('RS485_PORT', '/dev/ttyUSB1')
    print(f"⚠️  port_detector.py not found, using: {PORT}")

BAUDRATE = int(os.environ.get('RS485_BAUD', '115200'))

print(f"🔌 Kết nối tới {PORT} @ {BAUDRATE}bps...")

try:
    ser = serial.Serial(
        port=PORT,
        baudrate=BAUDRATE,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE,
        timeout=0.1,
        write_timeout=1.0
    )
    print(f"✅ Đã kết nối thành công!")
    print(f"📊 Port info: {ser}")
    print(f"\n{'='*60}")
    print(f"📡 Đang lắng nghe... (Ctrl+C để thoát)")
    print(f"{'='*60}\n")
    
    rx_count = 0
    tx_count = 0
    
    while True:
        # Đọc data nhận được
        if ser.in_waiting > 0:
            data = ser.read(ser.in_waiting)
            rx_count += len(data)
            
            # Hiển thị HEX
            hex_str = ' '.join([f'{b:02X}' for b in data])
            
            # Hiển thị ASCII (nếu printable)
            ascii_str = ''.join([chr(b) if 32 <= b < 127 else '.' for b in data])
            
            print(f"📥 RX [{len(data):3d} bytes] Total: {rx_count:5d}")
            print(f"   HEX:   {hex_str}")
            print(f"   ASCII: {ascii_str}")
            
            # 🔄 TỰ ĐỘNG PHẢN HỒI (Echo test)
            # Gửi lại data nhận được để test TX
            try:
                ser.write(data)
                tx_count += len(data)
                print(f"📤 TX [{len(data):3d} bytes] Total: {tx_count:5d} (ECHO)")
                print(f"   → Đã gửi phản hồi!")
            except Exception as e:
                print(f"❌ Lỗi gửi: {e}")
            
            print(f"{'-'*60}\n")
        
        time.sleep(0.01)

except serial.SerialException as e:
    print(f"❌ Lỗi serial: {e}")
    print(f"\n💡 Kiểm tra:")
    print(f"   1. Port đúng không? Thử: ls -la /dev/ttyUSB* hoặc /dev/ttyS*")
    print(f"   2. Permission OK không? Thử: sudo chmod 666 {PORT}")
    print(f"   3. Port bị chiếm không? Thử: sudo lsof {PORT}")
    sys.exit(1)

except KeyboardInterrupt:
    print(f"\n\n{'='*60}")
    print(f"📊 THỐNG KÊ:")
    print(f"   📥 RX Total: {rx_count} bytes")
    print(f"   📤 TX Total: {tx_count} bytes")
    print(f"{'='*60}")
    print(f"👋 Đã dừng!")
    sys.exit(0)

except Exception as e:
    print(f"❌ Lỗi không mong đợi: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()

