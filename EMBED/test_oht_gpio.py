#!/usr/bin/env python3
"""
Test GPIO cho OHT-50
Test tất cả GPIO đã thiết lập
"""

import time
import os
import signal
import sys

class OHTGPIO:
    def __init__(self):
        # GPIO mapping cho OHT-50
        self.gpio_map = {
            'UART1_TX': 46,
            'UART1_RX': 47,
            'RELAY1': 51,
            'RELAY2': 50,
            'LED_POWER': 52,
            'LED_SYSTEM': 53,
            'LED_COMM': 54,
            'LED_NETWORK': 55,
            'LED_ERROR': 56,
            'ESTOP_CH1': 57,
            'ESTOP_CH2': 58
        }
        
        self.running = True
        signal.signal(signal.SIGINT, self.signal_handler)
    
    def signal_handler(self, signum, frame):
        print("\n🛑 Dừng test GPIO...")
        self.running = False
    
    def read_gpio(self, pin):
        """Đọc giá trị GPIO"""
        try:
            with open(f"/sys/class/gpio/gpio{pin}/value", "r") as f:
                return int(f.read().strip())
        except Exception as e:
            print(f"❌ Lỗi đọc GPIO{pin}: {e}")
            return None
    
    def write_gpio(self, pin, value):
        """Ghi giá trị GPIO"""
        try:
            with open(f"/sys/class/gpio/gpio{pin}/value", "w") as f:
                f.write(str(value))
            return True
        except Exception as e:
            print(f"❌ Lỗi ghi GPIO{pin}: {e}")
            return False
    
    def test_led_sequence(self):
        """Test LED sequence"""
        print("🔆 Test LED sequence...")
        
        leds = ['LED_POWER', 'LED_SYSTEM', 'LED_COMM', 'LED_NETWORK', 'LED_ERROR']
        
        for led in leds:
            pin = self.gpio_map[led]
            print(f"  {led} (GPIO{pin}) ON")
            self.write_gpio(pin, 1)
            time.sleep(0.5)
            
            print(f"  {led} (GPIO{pin}) OFF")
            self.write_gpio(pin, 0)
            time.sleep(0.2)
    
    def test_relay_sequence(self):
        """Test Relay sequence"""
        print("🔌 Test Relay sequence...")
        
        relays = ['RELAY1', 'RELAY2']
        
        for relay in relays:
            pin = self.gpio_map[relay]
            print(f"  {relay} (GPIO{pin}) ON")
            self.write_gpio(pin, 1)
            time.sleep(1)
            
            print(f"  {relay} (GPIO{pin}) OFF")
            self.write_gpio(pin, 0)
            time.sleep(0.5)
    
    def test_estop_monitoring(self):
        """Test E-Stop monitoring"""
        print("🛑 Test E-Stop monitoring...")
        print("Nhấn Ctrl+C để dừng")
        
        estop_pins = ['ESTOP_CH1', 'ESTOP_CH2']
        
        while self.running:
            for estop in estop_pins:
                pin = self.gpio_map[estop]
                value = self.read_gpio(pin)
                status = "PRESSED" if value == 0 else "RELEASED"
                print(f"  {estop} (GPIO{pin}): {status}", end=" | ")
            
            print()
            time.sleep(0.5)
    
    def test_all_gpio_status(self):
        """Test tất cả GPIO status"""
        print("📊 Test tất cả GPIO status...")
        
        for name, pin in self.gpio_map.items():
            value = self.read_gpio(pin)
            if value is not None:
                print(f"  {name} (GPIO{pin}): {value}")
            else:
                print(f"  {name} (GPIO{pin}): ERROR")
    
    def run_all_tests(self):
        """Chạy tất cả tests"""
        print("🔧 OHT-50 GPIO Test")
        print("=" * 40)
        
        # Test 1: Status hiện tại
        self.test_all_gpio_status()
        print()
        
        # Test 2: LED sequence
        self.test_led_sequence()
        print()
        
        # Test 3: Relay sequence
        self.test_relay_sequence()
        print()
        
        # Test 4: E-Stop monitoring
        self.test_estop_monitoring()

def main():
    """Main function"""
    print("🔧 OHT-50 GPIO Test Menu")
    print("=" * 30)
    print("1. Test LED sequence")
    print("2. Test Relay sequence")
    print("3. Test E-Stop monitoring")
    print("4. Test tất cả GPIO status")
    print("5. Chạy tất cả tests")
    print("0. Thoát")
    
    try:
        choice = input("\nChọn test (1-5, 0): ").strip()
        
        gpio = OHTGPIO()
        
        if choice == "1":
            gpio.test_led_sequence()
        elif choice == "2":
            gpio.test_relay_sequence()
        elif choice == "3":
            gpio.test_estop_monitoring()
        elif choice == "4":
            gpio.test_all_gpio_status()
        elif choice == "5":
            gpio.run_all_tests()
        elif choice == "0":
            print("Thoát")
        else:
            print("Lựa chọn không hợp lệ")
            
    except KeyboardInterrupt:
        print("\n🛑 Dừng test")
    except Exception as e:
        print(f"❌ Lỗi: {e}")

if __name__ == "__main__":
    main()
