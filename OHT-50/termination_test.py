#!/usr/bin/env python3
"""
Termination Test Script cho OHT-50 RS485
Test termination resistors và bias setup
"""

import time
import serial
import struct
import threading
from pathlib import Path

class TerminationTest:
    def __init__(self):
        self.uart_device = "/dev/ttyS1"
        self.baudrate = 115200
        self.timeout = 1.0
        self.test_duration = 60  # 60 giây test
        
    def test_termination_voltage(self):
        """Test voltage levels trên RS485 bus"""
        print("=== Test Termination Voltage ===")
        print("Kết nối multimeter vào A-B lines")
        print("Đo voltage levels:")
        print("  - Idle state: ~0V")
        print("  - Active state: ±2V differential")
        print("  - Bias voltage: ~1.65V (nếu có bias)")
        
        input("Nhấn Enter khi đã kết nối multimeter...")
        
        try:
            with serial.Serial(
                port=self.uart_device,
                baudrate=self.baudrate,
                timeout=self.timeout
            ) as ser:
                print("✓ UART1 mở thành công")
                
                # Test idle state
                print("\n1. Test Idle State (5 giây)")
                print("   Đo voltage A-B (nên ~0V)")
                time.sleep(5)
                
                # Test active state
                print("\n2. Test Active State (5 giây)")
                print("   Đo voltage A-B (nên ±2V)")
                for i in range(10):
                    ser.write(b"TERMINATION_TEST_ACTIVE\r\n")
                    time.sleep(0.5)
                
                print("\n✓ Voltage test hoàn thành")
                return True
                
        except Exception as e:
            print(f"❌ Lỗi voltage test: {e}")
            return False
    
    def test_termination_impedance(self):
        """Test impedance của termination"""
        print("\n=== Test Termination Impedance ===")
        print("Kết nối multimeter vào A-B lines")
        print("Đo resistance:")
        print("  - Không termination: ~∞Ω")
        print("  - Có termination: ~120Ω")
        
        input("Nhấn Enter khi đã kết nối multimeter...")
        
        print("Đo resistance A-B:")
        print("  - Nếu ~120Ω: Termination đúng")
        print("  - Nếu ~∞Ω: Cần thêm termination")
        print("  - Nếu ~60Ω: Có 2 termination (đúng)")
        
        input("Nhấn Enter sau khi đo...")
        return True
    
    def test_bias_resistors(self):
        """Test bias resistors"""
        print("\n=== Test Bias Resistors ===")
        print("Kết nối multimeter vào A và B riêng biệt")
        print("Đo voltage A-GND và B-GND:")
        print("  - Không bias: ~0V")
        print("  - Có bias: ~1.65V (A), ~0V (B)")
        
        input("Nhấn Enter khi đã kết nối multimeter...")
        
        print("Đo voltage levels:")
        print("  - A-GND: ___V")
        print("  - B-GND: ___V")
        print("  - A-B differential: ___V")
        
        input("Nhấn Enter sau khi đo...")
        return True
    
    def test_signal_quality(self):
        """Test signal quality với oscilloscope"""
        print("\n=== Test Signal Quality ===")
        print("Kết nối oscilloscope vào A-B lines")
        print("Quan sát signal:")
        print("  - Clean square waves")
        print("  - No ringing/overshoot")
        print("  - Proper timing")
        
        input("Nhấn Enter khi đã kết nối oscilloscope...")
        
        try:
            with serial.Serial(
                port=self.uart_device,
                baudrate=self.baudrate,
                timeout=self.timeout
            ) as ser:
                print("Gửi test pattern...")
                
                # Test pattern 1: Alternating bits
                pattern1 = bytes([0x55, 0xAA, 0x55, 0xAA])
                ser.write(pattern1)
                print("Pattern 1: 0x55 0xAA 0x55 0xAA")
                time.sleep(2)
                
                # Test pattern 2: All ones
                pattern2 = bytes([0xFF, 0xFF, 0xFF, 0xFF])
                ser.write(pattern2)
                print("Pattern 2: 0xFF 0xFF 0xFF 0xFF")
                time.sleep(2)
                
                # Test pattern 3: All zeros
                pattern3 = bytes([0x00, 0x00, 0x00, 0x00])
                ser.write(pattern3)
                print("Pattern 3: 0x00 0x00 0x00 0x00")
                time.sleep(2)
                
                print("✓ Signal quality test hoàn thành")
                return True
                
        except Exception as e:
            print(f"❌ Lỗi signal quality test: {e}")
            return False
    
    def test_loopback_termination(self):
        """Test loopback với termination"""
        print("\n=== Test Loopback với Termination ===")
        print("Kết nối A→B để tạo loopback")
        print("Test data transmission...")
        
        input("Nhấn Enter khi đã kết nối loopback...")
        
        try:
            with serial.Serial(
                port=self.uart_device,
                baudrate=self.baudrate,
                timeout=self.timeout
            ) as ser:
                test_data = b"LOOPBACK_TERMINATION_TEST\r\n"
                ser.write(test_data)
                print(f"TX: {test_data}")
                
                # Đọc response
                response = ser.read(len(test_data))
                if response:
                    print(f"RX: {response}")
                    if response == test_data:
                        print("✓ Loopback test thành công")
                        return True
                    else:
                        print("⚠ Loopback test thất bại - data mismatch")
                        return False
                else:
                    print("⚠ Loopback test thất bại - no response")
                    return False
                    
        except Exception as e:
            print(f"❌ Lỗi loopback test: {e}")
            return False
    
    def test_ber_termination(self):
        """Test BER với termination"""
        print("\n=== Test BER với Termination ===")
        print("Test Bit Error Rate trong 30 giây...")
        
        try:
            with serial.Serial(
                port=self.uart_device,
                baudrate=self.baudrate,
                timeout=self.timeout
            ) as ser:
                start_time = time.time()
                total_bytes = 0
                error_bytes = 0
                
                while time.time() - start_time < 30:
                    # Gửi test pattern
                    test_pattern = bytes([0xAA, 0x55, 0xAA, 0x55])
                    ser.write(test_pattern)
                    total_bytes += len(test_pattern)
                    
                    # Đọc response (nếu có loopback)
                    response = ser.read(len(test_pattern))
                    if response and response != test_pattern:
                        error_bytes += len(response)
                    
                    time.sleep(0.1)
                
                # Tính BER
                if total_bytes > 0:
                    ber = error_bytes / total_bytes
                    print(f"Tổng bytes: {total_bytes}")
                    print(f"Error bytes: {error_bytes}")
                    print(f"BER: {ber:.6f} ({ber*100:.4f}%)")
                    
                    if ber < 0.001:  # < 0.1%
                        print("✓ BER test pass")
                        return True
                    else:
                        print("⚠ BER test fail - quá nhiều lỗi")
                        return False
                else:
                    print("⚠ BER test fail - không có data")
                    return False
                    
        except Exception as e:
            print(f"❌ Lỗi BER test: {e}")
            return False
    
    def run_all_tests(self):
        """Chạy tất cả tests"""
        print("🔧 Termination Test Suite cho OHT-50 RS485")
        print("=" * 50)
        
        tests = [
            ("Termination Voltage", self.test_termination_voltage),
            ("Termination Impedance", self.test_termination_impedance),
            ("Bias Resistors", self.test_bias_resistors),
            ("Signal Quality", self.test_signal_quality),
            ("Loopback Termination", self.test_loopback_termination),
            ("BER Termination", self.test_ber_termination)
        ]
        
        results = []
        
        for test_name, test_func in tests:
            print(f"\n{'='*20} {test_name} {'='*20}")
            try:
                result = test_func()
                results.append((test_name, result))
                status = "✓ PASS" if result else "❌ FAIL"
                print(f"{status}: {test_name}")
            except KeyboardInterrupt:
                print(f"\n🛑 Test bị dừng bởi người dùng")
                break
            except Exception as e:
                print(f"❌ ERROR: {test_name} - {e}")
                results.append((test_name, False))
        
        # Summary
        print(f"\n{'='*50}")
        print("📊 TEST SUMMARY")
        print("=" * 50)
        
        passed = sum(1 for _, result in results if result)
        total = len(results)
        
        for test_name, result in results:
            status = "✓ PASS" if result else "❌ FAIL"
            print(f"{status}: {test_name}")
        
        print(f"\nKết quả: {passed}/{total} tests passed")
        
        if passed == total:
            print("🎉 Tất cả tests passed! Termination setup OK")
        else:
            print("⚠ Một số tests failed. Kiểm tra lại wiring/termination")
        
        return passed == total

if __name__ == "__main__":
    test = TerminationTest()
    test.run_all_tests()
