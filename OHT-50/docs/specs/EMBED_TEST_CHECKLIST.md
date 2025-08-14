# EMBED Test Checklist – RS485/ UART/EMI (v1.0)

Điền thông tin bộ đo kiểm và kết quả theo từng mục dưới đây.

## Thông tin chung
- Board/Adapter: 
- Firmware version: 
- Baud/Parity/Stop: 
- Ngày giờ test: 
- Người thực hiện: 

## Wiring RS485
- A/B đúng cực: [ ]
- Termination 120Ω hai đầu: [ ] (giá trị đo: ____ Ω)
- Bias resistor hiện diện: [ ] (giá trị: ____ Ω)
- Ảnh wiring đính kèm: [ ]

## DE/RE GPIO
- Mức logic đúng: [ ] (GPIO: ______)
- Quan sát trên scope (nếu có): [ ]

## UART + DMA Ring Buffer
- Cấu hình UART đúng (baud/parity/stop): [ ]
- DMA RX/TX hoạt động: [ ]
- Overflow/Framing error không xảy ra trong 15 phút: [ ]

## BER/Throughput/Latency (15 phút)
- Tổng byte nhận: ______
- Lỗi CRC (số khung): ______
- Timeout (số lần): ______
- Retry (số lần): ______
- BER ước lượng: ______
- Latency trung bình (ms): ______ | tối đa (ms): ______
- Log CSV đính kèm: [ ]

## udev rules – /dev/ttyOHT485
- File rules cài đặt: [ ]
- Thiết bị tồn tại sau reboot: [ ]
- Lệnh kiểm tra: `ls -l /dev/ttyOHT485` (ảnh/chụp màn hình): [ ]

## EMI/ESD (quick lab)
- TVS/bảo vệ lắp đúng: [ ]
- Test ESD nhanh (điều kiện lab): [ ] Kết quả: 
- Khuyến nghị cải thiện: 

## Kết luận
- Đạt yêu cầu bring-up: [ ]
- Vấn đề tồn tại & kế hoạch xử lý: 
