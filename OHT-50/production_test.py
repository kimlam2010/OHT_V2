#!/usr/bin/env python3
"""
Production Test Script cho OHT-50 RS485
BER/Latency measurement và production validation
"""

import time
import serial
import struct
import csv
import threading
from datetime import datetime
from pathlib import Path

class ProductionTest:
    def __init__(self):
        self.uart_device = "/dev/ttyS1"
        self.baudrate = 115200
        self.timeout = 1.0
        self.test_duration = 900  # 15 phút
        self.log_file = f"production_test_{datetime.now().strftime('%Y%m%d_%H%M%S')}.csv"
        
    def measure_latency(self, test_duration=60):
        """Đo latency của RS485 communication"""
        print(f"=== Latency Measurement ({test_duration}s) ===")
        
        try:
            with serial.Serial(
                port=self.uart_device,
                baudrate=self.baudrate,
                timeout=self.timeout
            ) as ser:
                latencies = []
                start_time = time.time()
                
                while time.time() - start_time < test_duration:
                    # Gửi timestamp
                    tx_time = time.time()
                    timestamp_data = struct.pack('d', tx_time)
                    ser.write(timestamp_data)
                    
                    # Đọc response
                    response = ser.read(len(timestamp_data))
                    if response:
                        rx_time = time.time()
                        rx_timestamp = struct.unpack('d', response)[0]
                        
                        # Tính latency
                        latency = (rx_time - tx_time) * 1000  # ms
                        latencies.append(latency)
                        
                        print(f"Latency: {latency:.2f}ms")
                    
                    time.sleep(0.1)
                
                # Tính thống kê
                if latencies:
                    avg_latency = sum(latencies) / len(latencies)
                    min_latency = min(latencies)
                    max_latency = max(latencies)
                    
                    print(f"\nLatency Statistics:")
                    print(f"  Average: {avg_latency:.2f}ms")
                    print(f"  Min: {min_latency:.2f}ms")
                    print(f"  Max: {max_latency:.2f}ms")
                    print(f"  Samples: {len(latencies)}")
                    
                    return {
                        'avg_latency': avg_latency,
                        'min_latency': min_latency,
                        'max_latency': max_latency,
                        'samples': len(latencies)
                    }
                else:
                    print("⚠ Không có data latency")
                    return None
                    
        except Exception as e:
            print(f"❌ Lỗi latency measurement: {e}")
            return None
    
    def measure_ber(self, test_duration=900):
        """Đo Bit Error Rate trong 15 phút"""
        print(f"=== BER Measurement ({test_duration}s) ===")
        
        try:
            with serial.Serial(
                port=self.uart_device,
                baudrate=self.baudrate,
                timeout=self.timeout
            ) as ser:
                total_bytes = 0
                error_bytes = 0
                start_time = time.time()
                
                # Test patterns
                patterns = [
                    bytes([0x00, 0x00, 0x00, 0x00]),  # All zeros
                    bytes([0xFF, 0xFF, 0xFF, 0xFF]),  # All ones
                    bytes([0x55, 0x55, 0x55, 0x55]),  # Alternating 0x55
                    bytes([0xAA, 0xAA, 0xAA, 0xAA]),  # Alternating 0xAA
                    bytes([0x01, 0x02, 0x03, 0x04]),  # Sequential
                    bytes([0xFE, 0xFD, 0xFC, 0xFB])   # Reverse sequential
                ]
                
                pattern_index = 0
                
                while time.time() - start_time < test_duration:
                    # Gửi test pattern
                    pattern = patterns[pattern_index]
                    ser.write(pattern)
                    total_bytes += len(pattern)
                    
                    # Đọc response
                    response = ser.read(len(pattern))
                    if response:
                        # So sánh pattern
                        if response != pattern:
                            error_bytes += len(response)
                        
                        # Log progress
                        elapsed = time.time() - start_time
                        if int(elapsed) % 60 == 0:  # Log mỗi phút
                            current_ber = error_bytes / total_bytes if total_bytes > 0 else 0
                            print(f"Progress: {elapsed:.0f}s, BER: {current_ber:.6f} ({current_ber*100:.4f}%)")
                    
                    # Chuyển pattern
                    pattern_index = (pattern_index + 1) % len(patterns)
                    time.sleep(0.01)  # 10ms interval
                
                # Tính BER cuối cùng
                final_ber = error_bytes / total_bytes if total_bytes > 0 else 0
                
                print(f"\nBER Test Results:")
                print(f"  Total bytes: {total_bytes:,}")
                print(f"  Error bytes: {error_bytes:,}")
                print(f"  BER: {final_ber:.6f} ({final_ber*100:.4f}%)")
                print(f"  Duration: {test_duration}s")
                
                return {
                    'total_bytes': total_bytes,
                    'error_bytes': error_bytes,
                    'ber': final_ber,
                    'duration': test_duration
                }
                
        except Exception as e:
            print(f"❌ Lỗi BER measurement: {e}")
            return None
    
    def stress_test(self, test_duration=300):
        """Stress test với high data rate"""
        print(f"=== Stress Test ({test_duration}s) ===")
        
        try:
            with serial.Serial(
                port=self.uart_device,
                baudrate=self.baudrate,
                timeout=self.timeout
            ) as ser:
                total_packets = 0
                failed_packets = 0
                start_time = time.time()
                
                # High-speed data transmission
                while time.time() - start_time < test_duration:
                    # Gửi packet với sequence number
                    packet = struct.pack('I', total_packets) + b'STRESS_TEST_DATA'
                    ser.write(packet)
                    total_packets += 1
                    
                    # Đọc response
                    response = ser.read(len(packet))
                    if response:
                        try:
                            rx_seq = struct.unpack('I', response[:4])[0]
                            if rx_seq != total_packets - 1:
                                failed_packets += 1
                        except:
                            failed_packets += 1
                    
                    # High speed: minimal delay
                    time.sleep(0.001)  # 1ms
                
                # Tính thống kê
                success_rate = (total_packets - failed_packets) / total_packets if total_packets > 0 else 0
                
                print(f"\nStress Test Results:")
                print(f"  Total packets: {total_packets:,}")
                print(f"  Failed packets: {failed_packets:,}")
                print(f"  Success rate: {success_rate:.4f} ({success_rate*100:.2f}%)")
                print(f"  Duration: {test_duration}s")
                
                return {
                    'total_packets': total_packets,
                    'failed_packets': failed_packets,
                    'success_rate': success_rate,
                    'duration': test_duration
                }
                
        except Exception as e:
            print(f"❌ Lỗi stress test: {e}")
            return None
    
    def temperature_test(self, test_duration=600):
        """Test performance ở nhiệt độ khác nhau"""
        print(f"=== Temperature Test ({test_duration}s) ===")
        print("Chạy test ở nhiệt độ phòng và ghi chú nhiệt độ")
        
        try:
            # Đọc nhiệt độ CPU
            temp = self.get_cpu_temperature()
            print(f"CPU Temperature: {temp}°C")
            
            # Chạy BER test
            ber_result = self.measure_ber(test_duration // 2)
            
            # Chạy latency test
            latency_result = self.measure_latency(test_duration // 2)
            
            return {
                'temperature': temp,
                'ber_result': ber_result,
                'latency_result': latency_result
            }
            
        except Exception as e:
            print(f"❌ Lỗi temperature test: {e}")
            return None
    
    def get_cpu_temperature(self):
        """Đọc nhiệt độ CPU"""
        try:
            with open('/sys/class/thermal/thermal_zone0/temp', 'r') as f:
                temp = int(f.read().strip()) / 1000.0
                return temp
        except:
            return None
    
    def save_results(self, results):
        """Lưu kết quả vào CSV file"""
        try:
            with open(self.log_file, 'w', newline='') as csvfile:
                writer = csv.writer(csvfile)
                
                # Header
                writer.writerow(['Test', 'Parameter', 'Value', 'Unit', 'Timestamp'])
                
                # BER results
                if 'ber_result' in results and results['ber_result']:
                    ber = results['ber_result']
                    writer.writerow(['BER', 'Total Bytes', ber['total_bytes'], 'bytes', datetime.now()])
                    writer.writerow(['BER', 'Error Bytes', ber['error_bytes'], 'bytes', datetime.now()])
                    writer.writerow(['BER', 'BER', ber['ber'], 'ratio', datetime.now()])
                    writer.writerow(['BER', 'Duration', ber['duration'], 'seconds', datetime.now()])
                
                # Latency results
                if 'latency_result' in results and results['latency_result']:
                    lat = results['latency_result']
                    writer.writerow(['Latency', 'Average', lat['avg_latency'], 'ms', datetime.now()])
                    writer.writerow(['Latency', 'Min', lat['min_latency'], 'ms', datetime.now()])
                    writer.writerow(['Latency', 'Max', lat['max_latency'], 'ms', datetime.now()])
                    writer.writerow(['Latency', 'Samples', lat['samples'], 'count', datetime.now()])
                
                # Stress test results
                if 'stress_result' in results and results['stress_result']:
                    stress = results['stress_result']
                    writer.writerow(['Stress', 'Total Packets', stress['total_packets'], 'count', datetime.now()])
                    writer.writerow(['Stress', 'Failed Packets', stress['failed_packets'], 'count', datetime.now()])
                    writer.writerow(['Stress', 'Success Rate', stress['success_rate'], 'ratio', datetime.now()])
                
                # Temperature
                if 'temperature' in results and results['temperature']:
                    writer.writerow(['Temperature', 'CPU', results['temperature'], '°C', datetime.now()])
            
            print(f"✓ Kết quả đã lưu vào: {self.log_file}")
            
        except Exception as e:
            print(f"❌ Lỗi lưu kết quả: {e}")
    
    def run_production_test(self):
        """Chạy toàn bộ production test suite"""
        print("🏭 Production Test Suite cho OHT-50 RS485")
        print("=" * 60)
        print(f"Thời gian: {datetime.now()}")
        print(f"Device: {self.uart_device}")
        print(f"Baudrate: {self.baudrate}")
        print("=" * 60)
        
        results = {}
        
        # 1. Latency test (1 phút)
        print("\n1️⃣ Latency Test (60s)")
        results['latency_result'] = self.measure_latency(60)
        
        # 2. BER test (15 phút)
        print("\n2️⃣ BER Test (15 phút)")
        results['ber_result'] = self.measure_ber(900)
        
        # 3. Stress test (5 phút)
        print("\n3️⃣ Stress Test (5 phút)")
        results['stress_result'] = self.stress_test(300)
        
        # 4. Temperature test (10 phút)
        print("\n4️⃣ Temperature Test (10 phút)")
        results['temperature_result'] = self.temperature_test(600)
        
        # 5. Lưu kết quả
        print("\n5️⃣ Lưu kết quả")
        self.save_results(results)
        
        # 6. Summary
        print("\n" + "=" * 60)
        print("📊 PRODUCTION TEST SUMMARY")
        print("=" * 60)
        
        # BER summary
        if results.get('ber_result'):
            ber = results['ber_result']
            ber_status = "✓ PASS" if ber['ber'] < 0.001 else "❌ FAIL"
            print(f"{ber_status} BER: {ber['ber']:.6f} ({ber['ber']*100:.4f}%)")
        
        # Latency summary
        if results.get('latency_result'):
            lat = results['latency_result']
            lat_status = "✓ PASS" if lat['avg_latency'] < 10 else "❌ FAIL"
            print(f"{lat_status} Latency: {lat['avg_latency']:.2f}ms avg")
        
        # Stress summary
        if results.get('stress_result'):
            stress = results['stress_result']
            stress_status = "✓ PASS" if stress['success_rate'] > 0.99 else "❌ FAIL"
            print(f"{stress_status} Stress: {stress['success_rate']:.4f} success rate")
        
        # Overall result
        passed_tests = sum([
            results.get('ber_result', {}).get('ber', 1) < 0.001,
            results.get('latency_result', {}).get('avg_latency', 100) < 10,
            results.get('stress_result', {}).get('success_rate', 0) > 0.99
        ])
        
        total_tests = 3
        print(f"\nOverall: {passed_tests}/{total_tests} tests passed")
        
        if passed_tests == total_tests:
            print("🎉 PRODUCTION TEST PASSED!")
            print("✅ OHT-50 RS485 ready for production")
        else:
            print("⚠ PRODUCTION TEST FAILED!")
            print("❌ Cần kiểm tra lại hardware/configuration")
        
        return passed_tests == total_tests

if __name__ == "__main__":
    test = ProductionTest()
    test.run_production_test()
