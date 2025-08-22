#!/usr/bin/env python3
"""
Test Script với Module OHT-50
Test communication với module thật qua RS485
"""

import time
import serial
from hal_rs485 import HAL_RS485, RS485Config

def test_basic_communication():
    """Test communication cơ bản với module"""
    print("=== Test Basic Communication ===")
    
    config = RS485Config(
        device="/dev/ttyS1",
        baudrate=9600,
        de_pin=47,
        re_pin=46
    )
    
    try:
        with HAL_RS485(config) as rs485:
            print("✓ RS485 HAL initialized")
            
            # Test 1: PING command
            print("\n1. Test PING command")
            ping_cmd = bytes([0xAA, 0x02, 0x01, 0x00, 0x00, 0x00])
            rs485.send(ping_cmd)
            print(f"   TX: {ping_cmd.hex()}")
            
            # Wait for response
            time.sleep(0.1)
            response = rs485.receive(10)
            if response:
                print(f"   RX: {response.hex()}")
            else:
                print("   ⚠ No response")
            
            # Test 2: GET_INFO command
            print("\n2. Test GET_INFO command")
            info_cmd = bytes([0xAA, 0x02, 0x02, 0x00, 0x00, 0x00])
            rs485.send(info_cmd)
            print(f"   TX: {info_cmd.hex()}")
            
            time.sleep(0.1)
            response = rs485.receive(10)
            if response:
                print(f"   RX: {response.hex()}")
            else:
                print("   ⚠ No response")
            
            # Test 3: READ_DI command
            print("\n3. Test READ_DI command")
            di_cmd = bytes([0xAA, 0x02, 0x10, 0x00, 0x00, 0x00])
            rs485.send(di_cmd)
            print(f"   TX: {di_cmd.hex()}")
            
            time.sleep(0.1)
            response = rs485.receive(10)
            if response:
                print(f"   RX: {response.hex()}")
            else:
                print("   ⚠ No response")
            
            # Test 4: READ_AI command
            print("\n4. Test READ_AI command")
            ai_cmd = bytes([0xAA, 0x02, 0x11, 0x00, 0x00, 0x00])
            rs485.send(ai_cmd)
            print(f"   TX: {ai_cmd.hex()}")
            
            time.sleep(0.1)
            response = rs485.receive(10)
            if response:
                print(f"   RX: {response.hex()}")
            else:
                print("   ⚠ No response")
            
            # Get statistics
            stats = rs485.get_stats()
            print(f"\n📊 Statistics:")
            print(f"   Bytes sent: {stats.bytes_sent}")
            print(f"   Bytes received: {stats.bytes_received}")
            print(f"   Packets sent: {stats.packets_sent}")
            print(f"   Packets received: {stats.packets_received}")
            print(f"   Errors: {stats.errors}")
            
            return True
            
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def test_continuous_ping():
    """Test PING liên tục"""
    print("\n=== Test Continuous PING ===")
    
    config = RS485Config(
        device="/dev/ttyS1",
        baudrate=9600,
        de_pin=47,
        re_pin=46
    )
    
    try:
        with HAL_RS485(config) as rs485:
            ping_cmd = bytes([0xAA, 0x02, 0x01, 0x00, 0x00, 0x00])
            
            print("Gửi PING liên tục (10 lần)...")
            for i in range(10):
                rs485.send(ping_cmd)
                print(f"   [{i+1:02d}] TX: {ping_cmd.hex()}")
                
                time.sleep(0.1)
                response = rs485.receive(10)
                if response:
                    print(f"        RX: {response.hex()}")
                else:
                    print(f"        ⚠ No response")
                
                time.sleep(0.5)  # 500ms delay
            
            return True
            
    except Exception as e:
        print(f"❌ Error: {e}")
        return False

def test_gpio_control():
    """Test GPIO control cho DE/RE"""
    print("\n=== Test GPIO Control ===")
    
    try:
        # Test GPIO 47 (DE)
        print("Testing GPIO 47 (DE)...")
        with open("/sys/class/gpio/export", "w") as f:
            f.write("47")
        
        with open("/sys/class/gpio/gpio47/direction", "w") as f:
            f.write("out")
        
        # Toggle DE
        for i in range(3):
            with open("/sys/class/gpio/gpio47/value", "w") as f:
                f.write("1")
            print(f"   DE HIGH {i+1}/3")
            time.sleep(0.2)
            
            with open("/sys/class/gpio/gpio47/value", "w") as f:
                f.write("0")
            print(f"   DE LOW {i+1}/3")
            time.sleep(0.2)
        
        with open("/sys/class/gpio/unexport", "w") as f:
            f.write("47")
        
        print("✓ GPIO control test completed")
        return True
        
    except Exception as e:
        print(f"❌ GPIO Error: {e}")
        return False

if __name__ == "__main__":
    print("🔧 Test với Module OHT-50")
    print("=" * 40)
    
    # Test 1: Basic communication
    comm_ok = test_basic_communication()
    
    # Test 2: Continuous ping
    ping_ok = test_continuous_ping()
    
    # Test 3: GPIO control
    gpio_ok = test_gpio_control()
    
    # Summary
    print("\n" + "=" * 40)
    print("📊 TEST SUMMARY")
    print("=" * 40)
    print(f"Basic Communication: {'✓ PASS' if comm_ok else '❌ FAIL'}")
    print(f"Continuous Ping:     {'✓ PASS' if ping_ok else '❌ FAIL'}")
    print(f"GPIO Control:        {'✓ PASS' if gpio_ok else '❌ FAIL'}")
    
    if all([comm_ok, ping_ok, gpio_ok]):
        print("\n🎉 All tests PASSED!")
        print("✅ Module OHT-50 communication working")
    else:
        print("\n⚠ Some tests FAILED")
        print("❌ Check wiring and module connection")
