#!/usr/bin/env python3
"""
Test GPIO cho OHT-50 (CORRECTED)
Test theo đúng danh sách firmware đang sử dụng
"""

import time
import os
import signal
import sys

class OHTGPIO:
    def __init__(self):
        # GPIO mapping đúng theo firmware
        self.gpio_map = {
            # LED Status Indicators (5 LEDs)
            'LED_POWER': 54,      # GPIO1_D6 - Power LED (Green)
            'LED_SYSTEM': 35,     # GPIO1_A3 - System LED (Blue)
            'LED_COMM': 28,       # GPIO0_D4 - Communication LED (Yellow)
            'LED_NETWORK': 29,    # GPIO0_D5 - Network LED (Green)
            'LED_ERROR': 58,      # GPIO1_D2 - Error LED (Red)
            
            # E-Stop Safety System (Dual-channel)
            'ESTOP_CH1': 59,      # GPIO1_D3 - E-Stop Channel 1
            
            # Relay Output (2x)
            'RELAY1': 131,        # GPIO4_A3 - Relay 1 Output
            'RELAY2': 132,        # GPIO4_A4 - Relay 2 Output
            
            # RS485 Communication
            'UART1_TX': 46,       # UART1_TX - RS485 TX Control
            'UART1_RX': 47,       # UART1_RX - RS485 RX Control
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
        print("🔆 Test LED sequence (CORRECTED)...")
        
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
        print("🔌 Test Relay sequence (CORRECTED)...")
        
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
        print("🛑 Test E-Stop monitoring (CORRECTED)...")
        print("Nhấn Ctrl+C để dừng")
        
        estop_pins = ['ESTOP_CH1']
        
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
        print("📊 Test tất cả GPIO status (CORRECTED)...")
        
        for name, pin in self.gpio_map.items():
            value = self.read_gpio(pin)
            if value is not None:
                print(f"  {name} (GPIO{pin}): {value}")
            else:
                print(f"  {name} (GPIO{pin}): ERROR")
    
    def run_all_tests(self):
        """Chạy tất cả tests"""
        print("🔧 OHT-50 GPIO Test (CORRECTED)")
        print("=" * 50)
        
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
    print("🔧 OHT-50 GPIO Test Menu (CORRECTED)")
    print("=" * 40)
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
