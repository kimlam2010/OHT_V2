#!/usr/bin/env python3
"""
Test UART1 với chân 46 (TX) và 47 (RX)
"""

import serial
import time
import os

def test_uart1_pins():
    """Test UART1 với chân 46 và 47"""
    print("🔧 Test UART1 với chân 46 (TX) và 47 (RX)")
    print("=" * 50)
    
    # Kiểm tra GPIO 46 và 47
    print("📋 Kiểm tra GPIO 46 và 47:")
    try:
        with open('/sys/class/gpio/gpio46/direction', 'r') as f:
            gpio46_mode = f.read().strip()
        with open('/sys/class/gpio/gpio47/direction', 'r') as f:
            gpio47_mode = f.read().strip()
        print(f"  GPIO 46 (TX): {gpio46_mode}")
        print(f"  GPIO 47 (RX): {gpio47_mode}")
    except Exception as e:
        print(f"  ❌ Lỗi đọc GPIO: {e}")
        return False
    
    # Kiểm tra UART1 device
    print("\n📋 Kiểm tra UART1 device:")
    if os.path.exists('/dev/ttyS1'):
        print("  ✅ /dev/ttyS1 tồn tại")
    else:
        print("  ❌ /dev/ttyS1 không tồn tại")
        return False
    
    # Test UART1 communication
    print("\n📋 Test UART1 communication:")
    try:
        ser = serial.Serial(
            port='/dev/ttyS1',
            baudrate=115200,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1
        )
        print("  ✅ UART1 mở thành công")
        
        # Gửi test data
        test_data = b'Hello UART1 Test\n'
        ser.write(test_data)
        print(f"  📤 Sent: {test_data}")
        
        # Đọc response
        response = ser.read(100)
        if response:
            print(f"  📥 Received: {response}")
        else:
            print("  ⚠️  Không có response (expected - no loopback)")
        
        ser.close()
        print("  ✅ UART1 đóng thành công")
        
    except Exception as e:
        print(f"  ❌ Lỗi UART1: {e}")
        return False
    
    # Test GPIO control
    print("\n📋 Test GPIO control:")
    try:
        # Set GPIO 46 (TX) to 1
        with open('/sys/class/gpio/gpio46/value', 'w') as f:
            f.write('1')
        
        # Đọc giá trị
        with open('/sys/class/gpio/gpio46/value', 'r') as f:
            gpio46_value = f.read().strip()
        with open('/sys/class/gpio/gpio47/value', 'r') as f:
            gpio47_value = f.read().strip()
        
        print(f"  GPIO 46 (TX) = {gpio46_value}")
        print(f"  GPIO 47 (RX) = {gpio47_value}")
        
        # Set GPIO 46 (TX) back to 0
        with open('/sys/class/gpio/gpio46/value', 'w') as f:
            f.write('0')
        
        print("  ✅ GPIO control test passed")
        
    except Exception as e:
        print(f"  ❌ Lỗi GPIO control: {e}")
        return False
    
    print("\n✅ UART1 với chân 46 và 47 test completed successfully!")
    return True

if __name__ == "__main__":
    if not test_uart1_pins():
        print("\n❌ Test failed!")
        exit(1)
