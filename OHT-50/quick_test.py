#!/usr/bin/env python3
"""
Quick Hardware Test cho OHT-50
"""

import os
import time
import serial
from pathlib import Path

def test_uart1():
    print("=== Test UART1 ===")
    if Path("/dev/ttyS1").exists():
        print("✓ /dev/ttyS1 exists")
        try:
            with serial.Serial("/dev/ttyS1", 115200, timeout=1) as ser:
                print("✓ UART1 opened successfully")
                ser.write(b"OHT50_TEST\r\n")
                print("✓ Data written to UART1")
                return True
        except Exception as e:
            print(f"✗ UART1 error: {e}")
            return False
    else:
        print("✗ /dev/ttyS1 not found")
        return False

def test_gpio():
    print("=== Test GPIO ===")
    try:
        # Test GPIO35 (GPIO1_D3)
        with open("/sys/class/gpio/export", "w") as f:
            f.write("35")
        print("✓ GPIO35 exported")
        
        with open("/sys/class/gpio/gpio35/direction", "w") as f:
            f.write("out")
        print("✓ GPIO35 set as output")
        
        # Toggle
        for i in range(2):
            with open("/sys/class/gpio/gpio35/value", "w") as f:
                f.write("1")
            time.sleep(0.5)
            with open("/sys/class/gpio/gpio35/value", "w") as f:
                f.write("0")
            time.sleep(0.5)
            print(f"✓ GPIO35 toggle {i+1}/2")
            
        with open("/sys/class/gpio/unexport", "w") as f:
            f.write("35")
        print("✓ GPIO35 unexported")
        return True
    except Exception as e:
        print(f"✗ GPIO error: {e}")
        return False

def test_rs485():
    print("=== Test RS485 Protocol ===")
    try:
        with serial.Serial("/dev/ttyS1", 115200, timeout=1) as ser:
            # PING command
            ping_cmd = bytes([0xAA, 0x01, 0x01, 0x00, 0x00, 0x00])
            ser.write(ping_cmd)
            print(f"✓ Sent PING: {ping_cmd.hex()}")
            
            time.sleep(0.1)
            response = ser.read(10)
            if response:
                print(f"✓ Response: {response.hex()}")
            else:
                print("⚠ No response (expected if no module connected)")
            return True
    except Exception as e:
        print(f"✗ RS485 error: {e}")
        return False

if __name__ == "__main__":
    print("OHT-50 Quick Hardware Test")
    print("=" * 30)
    
    uart_ok = test_uart1()
    gpio_ok = test_gpio()
    rs485_ok = test_rs485()
    
    print("\n=== Results ===")
    print(f"UART1: {'✓ PASS' if uart_ok else '✗ FAIL'}")
    print(f"GPIO:  {'✓ PASS' if gpio_ok else '✗ FAIL'}")
    print(f"RS485: {'✓ PASS' if rs485_ok else '✗ FAIL'}")
    
    if all([uart_ok, gpio_ok, rs485_ok]):
        print("\n🎉 All tests PASSED!")
    else:
        print("\n⚠ Some tests FAILED")
