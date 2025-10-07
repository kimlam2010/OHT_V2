#!/usr/bin/env python3
"""
🔍 AUTO-DETECT CH340 USB-TO-SERIAL PORT
Tự động tìm và verify port CH340 cho RS485
"""

import serial.tools.list_ports
import serial
import sys
from typing import Optional, List

def find_ch340_ports() -> List[dict]:
    """
    Tìm tất cả CH340 ports
    Returns: List of port info dicts
    """
    ch340_ports = []
    
    # CH340 VID:PID combinations
    CH340_IDS = [
        (0x1a86, 0x7523),  # CH340
        (0x1a86, 0x5523),  # CH341
    ]
    
    ports = serial.tools.list_ports.comports()
    
    for port in ports:
        # Kiểm tra VID:PID
        if (port.vid, port.pid) in CH340_IDS:
            ch340_ports.append({
                'device': port.device,
                'description': port.description,
                'hwid': port.hwid,
                'vid': port.vid,
                'pid': port.pid,
                'serial_number': port.serial_number,
                'manufacturer': port.manufacturer if hasattr(port, 'manufacturer') else 'Unknown'
            })
        # Fallback: Kiểm tra description
        elif 'CH340' in port.description or 'CH341' in port.description:
            ch340_ports.append({
                'device': port.device,
                'description': port.description,
                'hwid': port.hwid,
                'vid': port.vid if port.vid else 0,
                'pid': port.pid if port.pid else 0,
                'serial_number': port.serial_number,
                'manufacturer': 'QinHeng Electronics (detected by name)'
            })
    
    return ch340_ports

def verify_port(port: str, baudrate: int = 115200) -> bool:
    """
    Verify port có thể mở được không
    """
    try:
        ser = serial.Serial(
            port=port,
            baudrate=baudrate,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=0.1
        )
        ser.close()
        return True
    except Exception as e:
        print(f"⚠️  Cannot open {port}: {e}", file=sys.stderr)
        return False

def get_best_ch340_port(baudrate: int = 115200) -> Optional[str]:
    """
    Tự động tìm và verify CH340 port tốt nhất
    """
    ch340_ports = find_ch340_ports()
    
    if not ch340_ports:
        return None
    
    # Thử verify từng port
    for port_info in ch340_ports:
        port = port_info['device']
        if verify_port(port, baudrate):
            return port
    
    # Nếu không verify được, return port đầu tiên
    return ch340_ports[0]['device'] if ch340_ports else None

def main():
    """CLI tool để detect CH340"""
    print("🔍 Đang tìm CH340 USB-to-Serial converters...\n")
    
    ch340_ports = find_ch340_ports()
    
    if not ch340_ports:
        print("❌ Không tìm thấy CH340!")
        print("\n💡 Kiểm tra:")
        print("   1. USB đã cắm chưa?")
        print("   2. Driver CH340 đã cài chưa? (lsmod | grep ch341)")
        print("   3. dmesg | tail -20")
        sys.exit(1)
    
    print(f"✅ Tìm thấy {len(ch340_ports)} CH340 port(s):\n")
    
    for i, port_info in enumerate(ch340_ports, 1):
        print(f"📌 Port #{i}:")
        print(f"   Device:       {port_info['device']}")
        print(f"   Description:  {port_info['description']}")
        print(f"   VID:PID:      {port_info['vid']:04x}:{port_info['pid']:04x}")
        print(f"   Serial:       {port_info['serial_number']}")
        print(f"   Manufacturer: {port_info['manufacturer']}")
        
        # Verify port
        if verify_port(port_info['device']):
            print(f"   Status:       ✅ OK - Có thể mở được")
        else:
            print(f"   Status:       ❌ FAILED - Không mở được")
        
        print()
    
    # Tìm best port
    best_port = get_best_ch340_port()
    if best_port:
        print(f"🎯 Recommended port: {best_port}")
        print(f"\n📋 Export để dùng:")
        print(f"   export RS485_PORT={best_port}")
    else:
        print("⚠️  Không có port nào verify được!")
        sys.exit(1)

if __name__ == "__main__":
    main()

