#!/usr/bin/env python3
"""
🔍 MODBUS RTU DEBUG TOOL
Hiển thị chi tiết Modbus frames
"""

import serial
import time
import sys
import os
from typing import List

# 🔍 AUTO-DETECT CH340 PORT
try:
    from port_detector import get_best_ch340_port
    
    PORT = os.environ.get('RS485_PORT')
    if not PORT:
        PORT = get_best_ch340_port()
        if PORT:
            print(f"✅ Tự động detect CH340: {PORT}\n")
        else:
            print(f"❌ Không tìm thấy CH340!")
            print(f"💡 Set thủ công: export RS485_PORT=/dev/ttyUSB0")
            sys.exit(1)
    else:
        print(f"📌 Dùng port đã set: {PORT}\n")
except ImportError:
    PORT = os.environ.get('RS485_PORT', '/dev/ttyUSB1')
    print(f"⚠️  port_detector.py not found, using: {PORT}\n")

BAUDRATE = int(os.environ.get('RS485_BAUD', '115200'))

def crc16_modbus(data: bytes) -> int:
    """Tính CRC16 Modbus"""
    crc = 0xFFFF
    for byte in data:
        crc ^= byte
        for _ in range(8):
            if crc & 0x0001:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc

def parse_modbus_frame(data: bytes) -> dict:
    """Parse Modbus RTU frame"""
    if len(data) < 4:
        return {"error": "Frame quá ngắn"}
    
    slave_id = data[0]
    function_code = data[1]
    
    # Tính CRC
    received_crc = (data[-1] << 8) | data[-2]
    calculated_crc = crc16_modbus(data[:-2])
    crc_ok = received_crc == calculated_crc
    
    result = {
        "slave_id": slave_id,
        "function_code": function_code,
        "data_length": len(data),
        "crc_received": f"0x{received_crc:04X}",
        "crc_calculated": f"0x{calculated_crc:04X}",
        "crc_ok": crc_ok
    }
    
    # Parse theo function code
    if function_code == 0x03:  # Read Holding Registers
        if len(data) >= 6:
            start_addr = (data[2] << 8) | data[3]
            num_regs = (data[4] << 8) | data[5]
            result["read_addr"] = f"0x{start_addr:04X}"
            result["num_regs"] = num_regs
            result["type"] = "READ_HOLDING_REGISTERS"
    
    elif function_code == 0x04:  # Read Input Registers
        if len(data) >= 6:
            start_addr = (data[2] << 8) | data[3]
            num_regs = (data[4] << 8) | data[5]
            result["read_addr"] = f"0x{start_addr:04X}"
            result["num_regs"] = num_regs
            result["type"] = "READ_INPUT_REGISTERS"
    
    elif function_code == 0x06:  # Write Single Register
        if len(data) >= 6:
            addr = (data[2] << 8) | data[3]
            value = (data[4] << 8) | data[5]
            result["write_addr"] = f"0x{addr:04X}"
            result["value"] = f"0x{value:04X}"
            result["type"] = "WRITE_SINGLE_REGISTER"
    
    elif function_code == 0x10:  # Write Multiple Registers
        if len(data) >= 7:
            start_addr = (data[2] << 8) | data[3]
            num_regs = (data[4] << 8) | data[5]
            byte_count = data[6]
            result["write_addr"] = f"0x{start_addr:04X}"
            result["num_regs"] = num_regs
            result["byte_count"] = byte_count
            result["type"] = "WRITE_MULTIPLE_REGISTERS"
    
    else:
        result["type"] = f"UNKNOWN_FUNCTION_0x{function_code:02X}"
    
    return result

def create_modbus_response(slave_id: int, function_code: int, data: bytes) -> bytes:
    """Tạo Modbus response đơn giản"""
    frame = bytes([slave_id, function_code]) + data
    crc = crc16_modbus(frame)
    frame += bytes([crc & 0xFF, (crc >> 8) & 0xFF])
    return frame

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
    print(f"\n{'='*70}")
    print(f"📡 MODBUS RTU SLAVE SIMULATOR - Đang lắng nghe...")
    print(f"{'='*70}\n")
    
    frame_count = 0
    response_count = 0
    error_count = 0
    
    buffer = bytearray()
    last_byte_time = time.time()
    
    while True:
        current_time = time.time()
        
        # Đọc data
        if ser.in_waiting > 0:
            data = ser.read(ser.in_waiting)
            buffer.extend(data)
            last_byte_time = current_time
        
        # Nếu đã hơn 10ms không nhận byte mới → xử lý frame
        if len(buffer) > 0 and (current_time - last_byte_time) > 0.01:
            frame_count += 1
            
            # Hiển thị raw data
            hex_str = ' '.join([f'{b:02X}' for b in buffer])
            print(f"\n📥 Frame #{frame_count} [{len(buffer)} bytes]")
            print(f"   RAW: {hex_str}")
            
            # Parse Modbus
            parsed = parse_modbus_frame(bytes(buffer))
            
            print(f"   📊 Parsed:")
            for key, value in parsed.items():
                if key == "crc_ok":
                    icon = "✅" if value else "❌"
                    print(f"      {icon} {key}: {value}")
                else:
                    print(f"      • {key}: {value}")
            
            # Tạo response nếu CRC OK
            if parsed.get("crc_ok", False):
                slave_id = parsed["slave_id"]
                func_code = parsed["function_code"]
                
                # Tạo response dummy
                if func_code == 0x03 or func_code == 0x04:  # Read
                    # Response: slave_id + func + byte_count + data + CRC
                    byte_count = 4  # 2 registers
                    response_data = bytes([byte_count, 0x12, 0x34, 0x56, 0x78])
                    response = create_modbus_response(slave_id, func_code, response_data)
                    
                elif func_code == 0x06:  # Write Single
                    # Echo back: slave_id + func + addr + value + CRC
                    response = buffer[:6]  # Lấy 6 bytes đầu
                    crc = crc16_modbus(response)
                    response = bytes(response) + bytes([crc & 0xFF, (crc >> 8) & 0xFF])
                    
                else:
                    # Exception response
                    response = create_modbus_response(slave_id, func_code | 0x80, bytes([0x01]))
                
                # Gửi response
                try:
                    time.sleep(0.002)  # Delay nhỏ trước khi gửi
                    ser.write(response)
                    response_count += 1
                    
                    hex_resp = ' '.join([f'{b:02X}' for b in response])
                    print(f"   📤 Response sent [{len(response)} bytes]:")
                    print(f"      {hex_resp}")
                    print(f"      ✅ TX LED should blink now!")
                    
                except Exception as e:
                    print(f"      ❌ Lỗi gửi response: {e}")
                    error_count += 1
            else:
                error_count += 1
                print(f"      ⚠️ CRC sai - không gửi response")
            
            print(f"   {'─'*66}")
            
            # Clear buffer
            buffer.clear()
        
        time.sleep(0.001)

except serial.SerialException as e:
    print(f"\n❌ Lỗi serial: {e}")
    print(f"\n💡 Kiểm tra:")
    print(f"   1. Port: ls -la /dev/ttyUSB* hoặc /dev/ttyS*")
    print(f"   2. Permission: sudo chmod 666 {PORT}")
    print(f"   3. Bị chiếm: sudo lsof {PORT}")
    sys.exit(1)

except KeyboardInterrupt:
    print(f"\n\n{'='*70}")
    print(f"📊 THỐNG KÊ:")
    print(f"   📥 Frames nhận: {frame_count}")
    print(f"   📤 Responses gửi: {response_count}")
    print(f"   ❌ Lỗi: {error_count}")
    print(f"{'='*70}")
    print(f"👋 Đã dừng!")
    sys.exit(0)

except Exception as e:
    print(f"\n❌ Lỗi: {e}")
    import traceback
    traceback.print_exc()
    sys.exit(1)

finally:
    if 'ser' in locals() and ser.is_open:
        ser.close()

