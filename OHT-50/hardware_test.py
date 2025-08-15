#!/usr/bin/env python3
"""
Hardware Test Script cho OHT-50
Platform: Orange Pi 5B (RK3588)
Test: UART1 (RS485), GPIO1_D3, GPIO1_D2
"""

import os
import sys
import time
import serial
import subprocess
from pathlib import Path

class OHT50HardwareTest:
    def __init__(self):
        self.uart1_device = "/dev/ttyS1"
        self.gpio_d3 = "GPIO1_D3"  # Relay1/DE-RE
        self.gpio_d2 = "GPIO1_D2"  # Relay2/DE-RE
        self.test_results = {}
        
    def log(self, message):
        """Log message với timestamp"""
        timestamp = time.strftime("%Y-%m-%d %H:%M:%S")
        print(f"[{timestamp}] {message}")
        
    def check_system_info(self):
        """Kiểm tra thông tin hệ thống"""
        self.log("=== Kiểm tra thông tin hệ thống ===")
        
        # Kernel version
        try:
            with open("/proc/version", "r") as f:
                kernel_info = f.read().strip()
                self.log(f"Kernel: {kernel_info}")
        except Exception as e:
            self.log(f"Lỗi đọc kernel info: {e}")
            
        # CPU info
        try:
            with open("/proc/cpuinfo", "r") as f:
                cpu_info = f.read()
                if "rk3588" in cpu_info.lower():
                    self.log("✓ CPU: RK3588 detected")
                else:
                    self.log("⚠ CPU: RK3588 not detected")
        except Exception as e:
            self.log(f"Lỗi đọc CPU info: {e}")
            
        # UART devices
        try:
            uart_devices = list(Path("/dev").glob("ttyS*"))
            self.log(f"UART devices: {[str(d) for d in uart_devices]}")
            
            if Path(self.uart1_device).exists():
                self.log(f"✓ {self.uart1_device} exists")
            else:
                self.log(f"⚠ {self.uart1_device} not found")
        except Exception as e:
            self.log(f"Lỗi kiểm tra UART devices: {e}")
            
    def test_gpio_sysfs(self):
        """Test GPIO qua sysfs"""
        self.log("=== Test GPIO qua sysfs ===")
        
        # Kiểm tra GPIO sysfs
        gpio_sysfs = "/sys/class/gpio"
        if not Path(gpio_sysfs).exists():
            self.log("⚠ GPIO sysfs không tồn tại")
            return False
            
        # Tìm GPIO chip
        gpio_chips = list(Path(gpio_sysfs).glob("gpiochip*"))
        self.log(f"GPIO chips: {[str(c) for c in gpio_chips]}")
        
        # Test GPIO1_D3 (bank 1, pin 3)
        try:
            # Export GPIO
            gpio_num = 1 * 32 + 3  # GPIO1_D3 = 35
            export_path = Path(gpio_sysfs) / "export"
            direction_path = Path(gpio_sysfs) / f"gpio{gpio_num}" / "direction"
            value_path = Path(gpio_sysfs) / f"gpio{gpio_num}" / "value"
            
            # Export GPIO
            with open(export_path, "w") as f:
                f.write(str(gpio_num))
            self.log(f"✓ Exported GPIO{gpio_num}")
            
            # Set direction
            with open(direction_path, "w") as f:
                f.write("out")
            self.log(f"✓ Set GPIO{gpio_num} as output")
            
            # Test toggle
            for i in range(3):
                with open(value_path, "w") as f:
                    f.write("1")
                time.sleep(0.5)
                with open(value_path, "w") as f:
                    f.write("0")
                time.sleep(0.5)
                self.log(f"✓ GPIO{gpio_num} toggle {i+1}/3")
                
            # Unexport
            with open(Path(gpio_sysfs) / "unexport", "w") as f:
                f.write(str(gpio_num))
            self.log(f"✓ Unexported GPIO{gpio_num}")
            
            return True
            
        except Exception as e:
            self.log(f"⚠ Lỗi test GPIO: {e}")
            return False
            
    def test_uart1_basic(self):
        """Test UART1 cơ bản"""
        self.log("=== Test UART1 cơ bản ===")
        
        if not Path(self.uart1_device).exists():
            self.log(f"⚠ {self.uart1_device} không tồn tại")
            return False
            
        try:
            # Kiểm tra quyền truy cập
            if not os.access(self.uart1_device, os.R_OK | os.W_OK):
                self.log(f"⚠ Không có quyền truy cập {self.uart1_device}")
                self.log("Thêm user vào group dialout: sudo usermod -a -G dialout $USER")
                return False
                
            # Test mở serial port
            with serial.Serial(
                port=self.uart1_device,
                baudrate=115200,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=1
            ) as ser:
                self.log(f"✓ Mở {self.uart1_device} thành công")
                
                # Test ghi dữ liệu
                test_data = b"OHT50_TEST\r\n"
                ser.write(test_data)
                self.log(f"✓ Ghi {len(test_data)} bytes")
                
                # Test đọc (có thể timeout nếu không có loopback)
                try:
                    response = ser.read(10)
                    if response:
                        self.log(f"✓ Đọc được: {response}")
                    else:
                        self.log("⚠ Không đọc được dữ liệu (có thể do không có loopback)")
                except Exception as e:
                    self.log(f"⚠ Lỗi đọc: {e}")
                    
                return True
                
        except Exception as e:
            self.log(f"⚠ Lỗi test UART1: {e}")
            return False
            
    def test_rs485_protocol(self):
        """Test RS485 protocol"""
        self.log("=== Test RS485 Protocol ===")
        
        if not Path(self.uart1_device).exists():
            self.log(f"⚠ {self.uart1_device} không tồn tại")
            return False
            
        try:
            with serial.Serial(
                port=self.uart1_device,
                baudrate=115200,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=1
            ) as ser:
                
                # Test PING command (0x01)
                ping_cmd = bytes([0xAA, 0x01, 0x01, 0x00, 0x00, 0x00])
                ser.write(ping_cmd)
                self.log(f"✓ Gửi PING: {ping_cmd.hex()}")
                
                time.sleep(0.1)
                
                # Đọc response
                try:
                    response = ser.read(10)
                    if response:
                        self.log(f"✓ Response: {response.hex()}")
                    else:
                        self.log("⚠ Không có response (có thể do không có module)")
                except Exception as e:
                    self.log(f"⚠ Lỗi đọc response: {e}")
                    
                # Test GET_INFO command (0x02)
                info_cmd = bytes([0xAA, 0x01, 0x02, 0x00, 0x00, 0x00])
                ser.write(info_cmd)
                self.log(f"✓ Gửi GET_INFO: {info_cmd.hex()}")
                
                time.sleep(0.1)
                
                try:
                    response = ser.read(10)
                    if response:
                        self.log(f"✓ Response: {response.hex()}")
                    else:
                        self.log("⚠ Không có response (có thể do không có module)")
                except Exception as e:
                    self.log(f"⚠ Lỗi đọc response: {e}")
                    
                return True
                
        except Exception as e:
            self.log(f"⚠ Lỗi test RS485 protocol: {e}")
            return False
            
    def create_udev_rules(self):
        """Tạo udev rules cho OHT-50"""
        self.log("=== Tạo udev rules ===")
        
        udev_rule = """# OHT-50 RS485 UART1 alias
KERNEL=="ttyS1", SYMLINK+="ttyOHT485", GROUP="dialout", MODE="0660"
"""
        
        try:
            rule_path = "/etc/udev/rules.d/99-ttyOHT485.rules"
            with open(rule_path, "w") as f:
                f.write(udev_rule)
            self.log(f"✓ Tạo udev rule: {rule_path}")
            
            # Reload udev
            subprocess.run(["sudo", "udevadm", "control", "--reload-rules"], check=True)
            subprocess.run(["sudo", "udevadm", "trigger"], check=True)
            self.log("✓ Reload udev rules")
            
            # Kiểm tra symlink
            if Path("/dev/ttyOHT485").exists():
                self.log("✓ Symlink /dev/ttyOHT485 created")
            else:
                self.log("⚠ Symlink /dev/ttyOHT485 not created")
                
            return True
            
        except Exception as e:
            self.log(f"⚠ Lỗi tạo udev rules: {e}")
            return False
            
    def run_all_tests(self):
        """Chạy tất cả test"""
        self.log("=== BẮT ĐẦU HARDWARE TEST OHT-50 ===")
        
        # System info
        self.check_system_info()
        
        # GPIO test
        gpio_result = self.test_gpio_sysfs()
        self.test_results['gpio'] = gpio_result
        
        # UART1 test
        uart_result = self.test_uart1_basic()
        self.test_results['uart'] = uart_result
        
        # RS485 protocol test
        rs485_result = self.test_rs485_protocol()
        self.test_results['rs485'] = rs485_result
        
        # Udev rules
        udev_result = self.create_udev_rules()
        self.test_results['udev'] = udev_result
        
        # Summary
        self.log("=== KẾT QUẢ TEST ===")
        for test, result in self.test_results.items():
            status = "✓ PASS" if result else "✗ FAIL"
            self.log(f"{test.upper()}: {status}")
            
        passed = sum(self.test_results.values())
        total = len(self.test_results)
        self.log(f"Tổng kết: {passed}/{total} tests passed")
        
        if passed == total:
            self.log("🎉 Tất cả test PASSED!")
        else:
            self.log("⚠ Một số test FAILED - cần kiểm tra hardware wiring")
            
        return self.test_results

if __name__ == "__main__":
    test = OHT50HardwareTest()
    results = test.run_all_tests()
    sys.exit(0 if all(results.values()) else 1)
