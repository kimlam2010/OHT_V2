#!/usr/bin/env python3
"""
Continuous TX Test cho UART1
Truyền dữ liệu liên tục để test
"""

import serial
import time
import signal
import sys

class ContinuousTXTest:
    def __init__(self):
        self.uart_device = "/dev/ttyS1"
        self.baudrate = 9600
        self.running = True
        
    def signal_handler(self, signum, frame):
        print("\n🛑 Dừng truyền...")
        self.running = False
        
    def test_pattern_1(self):
        """Test pattern 1: Dữ liệu đơn giản"""
        pattern = b"OHT50_TEST_PATTERN_1\r\n"
        return pattern
        
    def test_pattern_2(self):
        """Test pattern 2: RS485 protocol"""
        pattern = bytes([0xAA, 0x02, 0x01, 0x00, 0x00, 0x00])  # PING
        return pattern
        
    def test_pattern_3(self):
        """Test pattern 3: Dữ liệu nhị phân"""
        pattern = bytes([0x55, 0xAA, 0x55, 0xAA, 0x00, 0xFF])
        return pattern
        
    def test_pattern_4(self):
        """Test pattern 4: Dữ liệu text dài"""
        pattern = b"OHT50_CONTINUOUS_TRANSMISSION_TEST_" + str(int(time.time())).encode() + b"\r\n"
        return pattern
        
    def run_continuous_tx(self, pattern_type=1, interval=1.0):
        """Chạy TX liên tục"""
        print(f"=== Continuous TX Test - Pattern {pattern_type} ===")
        print(f"Device: {self.uart_device}")
        print(f"Baudrate: {self.baudrate}")
        print(f"Interval: {interval}s")
        print("Nhấn Ctrl+C để dừng")
        print("=" * 50)
        
        try:
            with serial.Serial(
                port=self.uart_device,
                baudrate=self.baudrate,
                bytesize=serial.EIGHTBITS,
                parity=serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=1
            ) as ser:
                print(f"✓ Mở {self.uart_device} thành công")
                
                packet_count = 0
                start_time = time.time()
                
                while self.running:
                    # Chọn pattern
                    if pattern_type == 1:
                        data = self.test_pattern_1()
                    elif pattern_type == 2:
                        data = self.test_pattern_2()
                    elif pattern_type == 3:
                        data = self.test_pattern_3()
                    elif pattern_type == 4:
                        data = self.test_pattern_4()
                    else:
                        data = self.test_pattern_1()
                    
                    # Gửi dữ liệu
                    ser.write(data)
                    packet_count += 1
                    
                    # Hiển thị thông tin
                    elapsed = time.time() - start_time
                    rate = packet_count / elapsed if elapsed > 0 else 0
                    
                    print(f"[{packet_count:04d}] TX: {data.hex()} | Rate: {rate:.1f} pkt/s | Elapsed: {elapsed:.1f}s")
                    
                    time.sleep(interval)
                    
        except KeyboardInterrupt:
            print("\n🛑 Dừng bởi người dùng")
        except Exception as e:
            print(f"❌ Lỗi: {e}")
        finally:
            elapsed = time.time() - start_time
            print(f"\n📊 Thống kê:")
            print(f"   Tổng packet: {packet_count}")
            print(f"   Thời gian: {elapsed:.1f}s")
            print(f"   Tốc độ TB: {packet_count/elapsed:.1f} pkt/s" if elapsed > 0 else "   Tốc độ TB: 0 pkt/s")
            
    def run_all_patterns(self, interval=2.0):
        """Chạy tất cả patterns"""
        patterns = [
            ("Pattern 1 - Text đơn giản", 1),
            ("Pattern 2 - RS485 Protocol", 2),
            ("Pattern 3 - Binary data", 3),
            ("Pattern 4 - Text dài", 4)
        ]
        
        for name, pattern_type in patterns:
            print(f"\n🔄 {name}")
            print("Nhấn Enter để tiếp tục, Ctrl+C để dừng...")
            try:
                input()
                self.run_continuous_tx(pattern_type, interval)
            except KeyboardInterrupt:
                print("\n🛑 Dừng test")
                break
            except EOFError:
                break

if __name__ == "__main__":
    # Đăng ký signal handler
    signal.signal(signal.SIGINT, lambda s, f: sys.exit(0))
    
    test = ContinuousTXTest()
    
    print("OHT-50 Continuous TX Test")
    print("=" * 30)
    print("1. Pattern 1 - Text đơn giản")
    print("2. Pattern 2 - RS485 Protocol") 
    print("3. Pattern 3 - Binary data")
    print("4. Pattern 4 - Text dài")
    print("5. Tất cả patterns")
    print("0. Thoát")
    
    try:
        choice = input("\nChọn pattern (1-5, 0): ").strip()
        
        if choice == "1":
            test.run_continuous_tx(1, 1.0)
        elif choice == "2":
            test.run_continuous_tx(2, 0.5)
        elif choice == "3":
            test.run_continuous_tx(3, 0.2)
        elif choice == "4":
            test.run_continuous_tx(4, 2.0)
        elif choice == "5":
            test.run_all_patterns(2.0)
        elif choice == "0":
            print("Thoát")
        else:
            print("Lựa chọn không hợp lệ, chạy pattern 1")
            test.run_continuous_tx(1, 1.0)
            
    except KeyboardInterrupt:
        print("\n🛑 Dừng test")
    except EOFError:
        print("\nThoát")
