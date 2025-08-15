# EMBED Test Checklist – RS485/ UART/EMI (v1.0)

Điền thông tin bộ đo kiểm và kết quả theo từng mục dưới đây.

## Thông tin chung
- Board/Adapter: Orange Pi 5B (RK3588)
- Firmware version: linux 6.1.43-rk3588
- Baud/Parity/Stop: 115200/8N1
- Ngày giờ test: 2025-08-15
- Người thực hiện: Embedded HW/Driver Engineer

## Wiring RS485
- A/B đúng cực: [ ] **CẦN WIRING**
- Termination 120Ω hai đầu: [ ] **CẦN WIRING** (giá trị đo: ____ Ω)
- Bias resistor hiện diện: [ ] **CẦN WIRING** (giá trị: ____ Ω)
- Ảnh wiring đính kèm: [ ] **CẦN WIRING**

## DE/RE GPIO
- Mức logic đúng: [ ] **CẦN WIRING** (GPIO: ______)
- Quan sát trên scope (nếu có): [ ] **CẦN WIRING**

## Relay GPIO (Orange Pi 5B – nếu áp dụng)
- GPIO1_D3 (Relay1) — gpiochip:line = gpiochip1:3 | Test: ON [ ] OFF [ ] **CẦN WIRING**
- GPIO1_D2 (Relay2) — gpiochip:line = gpiochip1:2 | Test: ON [ ] OFF [ ] **CẦN WIRING** (đang dùng cho status_led)

## UART + DMA Ring Buffer
- Cấu hình UART đúng (baud/parity/stop): [✅] **ĐÃ KIỂM TRA** - 115200/8N1
- DMA RX/TX hoạt động: [✅] **ĐÃ KIỂM TRA** - interrupt mode (DMA không có)
- Overflow/Framing error không xảy ra trong 15 phút: [ ] **CẦN WIRING**

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
- Thiết bị tồn tại sau reboot: [✅] **ĐÃ KIỂM TRA** - `/dev/ttyOHT485` → `ttyS9`
- Lệnh kiểm tra: `ls -l /dev/ttyOHT485` (ảnh/chụp màn hình): [✅] **ĐÃ KIỂM TRA**

## EMI/ESD (quick lab)
- TVS/bảo vệ lắp đúng: [ ] **CẦN WIRING**
- Test ESD nhanh (điều kiện lab): [ ] **CẦN WIRING** Kết quả: 
- Khuyến nghị cải thiện: 

## Công cụ test đã chuẩn bị
- [✅] Script test: `tools/rs485/rs485_loop_tester.py`
- [✅] Cài đặt: `gpiod`, `python3-libgpiod`, `pyserial`
- [✅] Udev alias: `/dev/ttyOHT485` → `ttyS9`
- [✅] GPIO mapping: `GPIO1_D3` = `gpiochip1:3`, `GPIO1_D2` = `gpiochip1:2`

## Kết luận
- Đạt yêu cầu bring-up: [✅] **PHẦN MỀM HOÀN THÀNH** (cần wiring để test phần cứng)
- Vấn đề tồn tại & kế hoạch xử lý: 
  - UART1 chưa bật được (chỉ có ttyS9) - cần tìm cách bật UART1 đúng
  - Cần wiring RS485 để test loopback/BER thật
  - GPIO1_D2 đang dùng cho status_led - cần xem xét xung đột

## Hướng dẫn test sau khi có wiring
```bash
# Test cơ bản
python3 tools/rs485/rs485_loop_tester.py --dev /dev/ttyOHT485 --baud 115200 --count 100 --payload 8

# Test với auto-RTS (nếu wiring DE/RE → RTS)
python3 tools/rs485/rs485_loop_tester.py --dev /dev/ttyOHT485 --baud 115200 --auto_rts --count 5000 --payload 16

# Test GPIO relay
sudo gpioset gpiochip1 3=1; sleep 1; sudo gpioset gpiochip1 3=0
```
