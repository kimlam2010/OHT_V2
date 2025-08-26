#!/usr/bin/env python3
"""
Test Hall Sensor GPIO - OHT-50
Test hall sensor với GPIO đã được giải phóng (GPIO 131)
"""

import time
import os
import signal
import sys

class HallSensorTest:
    def __init__(self):
        # GPIO mapping cho Hall Sensor (sau khi giải phóng GPIO 131)
        self.hall_gpio_map = {
            'HALL_MOTOR1_A': 131,  # GPIO4_A3 - Motor 1 Hall A (đã giải phóng từ E-Stop)
            'HALL_MOTOR1_B': 132,  # GPIO4_A4 - Motor 1 Hall B (Relay 2)
            'HALL_MOTOR2_A': 28,   # GPIO0_D4 - Motor 2 Hall A (LED_COMM)
            'HALL_MOTOR2_B': 29,   # GPIO0_D5 - Motor 2 Hall B (LED_NETWORK)
        }
        
        self.running = True
        signal.signal(signal.SIGINT, self.signal_handler)
    
    def signal_handler(self, signum, frame):
        print("\n🛑 Dừng test Hall Sensor...")
        self.running = False
    
    def export_gpio(self, pin):
        """Export GPIO pin"""
        try:
            with open(f"/sys/class/gpio/export", "w") as f:
                f.write(str(pin))
            print(f"✅ GPIO{pin} exported")
            return True
        except FileExistsError:
            print(f"⚠️  GPIO{pin} already exported")
            return True
        except Exception as e:
            print(f"❌ Lỗi export GPIO{pin}: {e}")
            return False
    
    def set_gpio_direction(self, pin, direction):
        """Set GPIO direction"""
        try:
            with open(f"/sys/class/gpio/gpio{pin}/direction", "w") as f:
                f.write(direction)
            print(f"✅ GPIO{pin} direction set to {direction}")
            return True
        except Exception as e:
            print(f"❌ Lỗi set direction GPIO{pin}: {e}")
            return False
    
    def read_gpio(self, pin):
        """Đọc giá trị GPIO"""
        try:
            with open(f"/sys/class/gpio/gpio{pin}/value", "r") as f:
                return int(f.read().strip())
        except Exception as e:
            print(f"❌ Lỗi đọc GPIO{pin}: {e}")
            return None
    
    def setup_hall_sensors(self):
        """Setup tất cả hall sensors"""
        print("🔧 Setup Hall Sensors...")
        
        for name, pin in self.hall_gpio_map.items():
            print(f"\n📋 Setup {name} (GPIO{pin}):")
            
            # Export GPIO
            if not self.export_gpio(pin):
                return False
            
            # Set direction (INPUT cho hall sensor)
            if not self.set_gpio_direction(pin, "in"):
                return False
            
            # Test read
            value = self.read_gpio(pin)
            if value is not None:
                print(f"  ✅ {name} (GPIO{pin}): {value}")
            else:
                print(f"  ❌ {name} (GPIO{pin}): ERROR")
                return False
        
        return True
    
    def test_hall_sensor_reading(self):
        """Test đọc hall sensor"""
        print("\n📊 Test Hall Sensor Reading...")
        print("Nhấn Ctrl+C để dừng")
        
        while self.running:
            print("\n" + "="*50)
            print(f"Time: {time.strftime('%H:%M:%S')}")
            
            for name, pin in self.hall_gpio_map.items():
                value = self.read_gpio(pin)
                if value is not None:
                    status = "HIGH" if value == 1 else "LOW"
                    print(f"  {name} (GPIO{pin}): {status}")
                else:
                    print(f"  {name} (GPIO{pin}): ERROR")
            
            time.sleep(1)
    
    def test_hall_sensor_quadrature(self):
        """Test quadrature encoder simulation"""
        print("\n🔄 Test Quadrature Encoder Simulation...")
        print("Simulating motor rotation...")
        
        # Simulate quadrature signals
        quadrature_states = [
            (0, 0),  # State 00
            (0, 1),  # State 01
            (1, 1),  # State 11
            (1, 0),  # State 10
        ]
        
        for i in range(10):  # 10 cycles
            print(f"\nCycle {i+1}:")
            for state_a, state_b in quadrature_states:
                # Simulate hall sensor reading
                hall_a = self.read_gpio(self.hall_gpio_map['HALL_MOTOR1_A'])
                hall_b = self.read_gpio(self.hall_gpio_map['HALL_MOTOR1_B'])
                
                print(f"  Expected: A={state_a}, B={state_b} | Actual: A={hall_a}, B={hall_b}")
                time.sleep(0.5)
    
    def cleanup_gpio(self):
        """Cleanup GPIO pins"""
        print("\n🧹 Cleanup GPIO pins...")
        
        for name, pin in self.hall_gpio_map.items():
            try:
                with open(f"/sys/class/gpio/unexport", "w") as f:
                    f.write(str(pin))
                print(f"✅ GPIO{pin} unexported")
            except Exception as e:
                print(f"⚠️  GPIO{pin} unexport error: {e}")
    
    def run_all_tests(self):
        """Chạy tất cả tests"""
        print("🔧 OHT-50 Hall Sensor Test")
        print("=" * 50)
        print("GPIO Mapping:")
        for name, pin in self.hall_gpio_map.items():
            print(f"  {name}: GPIO{pin}")
        print()
        
        try:
            # Test 1: Setup hall sensors
            if not self.setup_hall_sensors():
                print("❌ Setup failed")
                return
            
            # Test 2: Continuous reading
            self.test_hall_sensor_reading()
            
        except KeyboardInterrupt:
            print("\n🛑 Test interrupted by user")
        finally:
            # Cleanup
            self.cleanup_gpio()
            print("\n✅ Test completed")

def main():
    """Main function"""
    print("🔧 OHT-50 Hall Sensor Test Menu")
    print("1. Run full test")
    print("2. Setup only")
    print("3. Exit")
    
    choice = input("\nChọn option (1-3): ").strip()
    
    test = HallSensorTest()
    
    if choice == "1":
        test.run_all_tests()
    elif choice == "2":
        if test.setup_hall_sensors():
            print("✅ Setup completed successfully")
        else:
            print("❌ Setup failed")
    elif choice == "3":
        print("👋 Goodbye!")
    else:
        print("❌ Invalid choice")

if __name__ == "__main__":
    main()
