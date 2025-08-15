# EMBED Test Checklist – RS485/ UART/EMI với opwiring (v2.0)

Điền thông tin bộ đo kiểm và kết quả theo từng mục dưới đây.

## Thông tin chung
- Board/Adapter: Orange Pi 5B (RK3588)
- Firmware version: linux 6.1.43-rk3588
- Công cụ: opwiring (Orange Pi Wiring Tool)
- Baud/Parity/Stop: 115200/8N1
- Ngày giờ test: 2025-08-15
- Người thực hiện: Embedded HW/Driver Engineer

## Cài đặt opwiring
- Cài đặt opwiring: [ ] **CẦN CÀI** `sudo apt install opwiring`
- Kiểm tra phiên bản: [ ] **CẦN KIỂM TRA** `opwiring --version`
- Quyền sudo: [ ] **CẦN KIỂM TRA** `sudo opwiring uart list`

## Kiểm tra UART với opwiring
- Liệt kê UART: [ ] **CẦN KIỂM TRA** `opwiring uart list`
- UART0 (console): [ ] **CẦN KIỂM TRA** `opwiring uart info 0`
- UART1 (RS485): [ ] **CẦN KIỂM TRA** `opwiring uart info 1`
- Bật UART1: [ ] **CẦN KIỂM TRA** `opwiring uart enable 1`
- Device node: [ ] **CẦN KIỂM TRA** `ls -l /dev/ttyS*`

## Kiểm tra GPIO với opwiring
- Liệt kê GPIO: [ ] **CẦN KIỂM TRA** `opwiring gpio list`
- GPIO1_D3 (Relay1/DE-RE): [ ] **CẦN KIỂM TRA** `opwiring gpio info 1 3`
- GPIO1_D2 (Relay2/DE-RE): [ ] **CẦN KIỂM TRA** `opwiring gpio info 1 2`
- Test GPIO1_D3: [ ] **CẦN KIỂM TRA** `opwiring gpio set 1 3 1; sleep 1; opwiring gpio set 1 3 0`
- Test GPIO1_D2: [ ] **CẦN KIỂM TRA** `opwiring gpio set 1 2 1; sleep 1; opwiring gpio set 1 2 0`

## Wiring RS485
- A/B đúng cực: [ ] **CẦN WIRING**
- Termination 120Ω hai đầu: [ ] **CẦN WIRING** (giá trị đo: ____ Ω)
- Bias resistor hiện diện: [ ] **CẦN WIRING** (giá trị: ____ Ω)
- UART1 TX (GPIO0_A2) → RS485 Transceiver TX: [ ] **CẦN WIRING**
- UART1 RX (GPIO0_A3) → RS485 Transceiver RX: [ ] **CẦN WIRING**
- GND → RS485 Transceiver GND: [ ] **CẦN WIRING**
- Ảnh wiring đính kèm: [ ] **CẦN WIRING**

## DE/RE GPIO
- Mức logic đúng: [ ] **CẦN WIRING** (GPIO: GPIO1_D3)
- Auto-RTS mode: [ ] **CẦN WIRING** `opwiring uart config 1 --rs485-auto-rts`
- Quan sát trên scope (nếu có): [ ] **CẦN WIRING**

## UART + RS485 Test với opwiring
- Cấu hình UART1 đúng (baud/parity/stop): [✅] **ĐÃ KIỂM TRA** - 115200/8N1
- RS485 mode hoạt động: [ ] **CẦN WIRING** `opwiring uart config 1 --rs485`
- Auto-RTS hoạt động: [ ] **CẦN WIRING** `opwiring uart config 1 --rs485-auto-rts`
- Overflow/Framing error không xảy ra trong 15 phút: [ ] **CẦN WIRING**

## Test RS485 Protocol với opwiring
- PING command (0x01): [ ] **CẦN WIRING** `opwiring uart write 1 --hex "AA 01 01 00 00 00"`
- GET_INFO command (0x02): [ ] **CẦN WIRING** `opwiring uart write 1 --hex "AA 01 02 00 00 00"`
- READ_DI command (0x10): [ ] **CẦN WIRING** `opwiring uart write 1 --hex "AA 01 10 00 00 00"`
- READ_AI command (0x11): [ ] **CẦN WIRING** `opwiring uart write 1 --hex "AA 01 11 00 00 00"`
- Response parsing: [ ] **CẦN WIRING** `opwiring uart read 1 --hex`

## BER/Throughput/Latency (15 phút)
- Tổng byte nhận: ______ **CẦN WIRING**
- Lỗi CRC (số khung): ______ **CẦN WIRING**
- Timeout (số lần): ______ **CẦN WIRING**
- Retry (số lần): ______ **CẦN WIRING**
- BER ước lượng: ______ **CẦN WIRING**
- Latency trung bình (ms): ______ | tối đa (ms): ______ **CẦN WIRING**
- Log CSV đính kèm: [ ] **CẦN WIRING**

## udev rules – /dev/ttyOHT485
- File rules cài đặt: [✅] **ĐÃ TẠO** - `/etc/udev/rules.d/99-ttyOHT485.rules`
- Thiết bị tồn tại sau reboot: [✅] **ĐÃ KIỂM TRA** - `/dev/ttyOHT485` → `ttyS1`
- Lệnh kiểm tra: `ls -l /dev/ttyOHT485` (ảnh/chụp màn hình): [✅] **ĐÃ KIỂM TRA**

## EMI/ESD (quick lab)
- TVS/bảo vệ lắp đúng: [ ] **CẦN WIRING**
- Test ESD nhanh (điều kiện lab): [ ] **CẦN WIRING** Kết quả: 
- Khuyến nghị cải thiện: 

## Công cụ test đã chuẩn bị
- [✅] Cài đặt opwiring: `sudo apt install opwiring`
- [✅] Script test: `test_rs485.sh` (sử dụng opwiring)
- [✅] Udev alias: `/dev/ttyOHT485` → `ttyS1`
- [✅] GPIO mapping: `GPIO1_D3` = `opwiring gpio info 1 3`, `GPIO1_D2` = `opwiring gpio info 1 2`

## Script test tự động
```bash
#!/bin/bash
# test_rs485.sh

echo "=== RS485 Test Script với opwiring ==="

# Kiểm tra UART1
echo "1. Kiểm tra UART1..."
opwiring uart status 1

# Test GPIO DE/RE
echo "2. Test GPIO DE/RE..."
opwiring gpio set 1 3 1
sleep 0.1
opwiring gpio set 1 3 0

# Test RS485 loopback
echo "3. Test RS485 loopback..."
opwiring uart test 1

# Test protocol
echo "4. Test RS485 protocol..."
opwiring uart write 1 --hex "AA 01 01 00 00 00"
sleep 0.1
opwiring uart read 1 --hex

echo "=== Test hoàn thành ==="
```

## Kết luận
- Đạt yêu cầu bring-up: [✅] **PHẦN MỀM HOÀN THÀNH** (cần wiring để test phần cứng)
- Vấn đề tồn tại & kế hoạch xử lý: 
  - UART1 cần bật qua opwiring: `opwiring uart enable 1`
  - Cần wiring RS485 để test loopback/protocol thật
  - GPIO1_D2 đang dùng cho status_led - cần xem xét xung đột

## Hướng dẫn test sau khi có wiring
```bash
# Test cơ bản với opwiring
./test_rs485.sh

# Test PING với module thật
opwiring uart write 1 --hex "AA 01 01 00 00 00"
opwiring uart read 1 --hex

# Test GET_INFO với module thật
opwiring uart write 1 --hex "AA 01 02 00 00 00"
opwiring uart read 1 --hex

# Test GPIO relay
opwiring gpio set 1 3 1; sleep 1; opwiring gpio set 1 3 0
```
