# Pinout Radxa & Mapping UART/CAN/RS485

Phiên bản: v1.1 (cập nhật EM-02/EM-03)
Phạm vi: Bảng ánh xạ chân Radxa và bo mở rộng: UART/CAN/RS485, mức điện áp, lưu ý ESD/EMC.

> Lưu ý: Model Radxa cụ thể (TBD). Nội dung dưới đây là khuôn mẫu; thay thế theo datasheet board/hat thực tế.  
> Đối với dự án OHT-50, platform đã chốt là Orange Pi 5B. Xem chi tiết tại `docs/dev_radxa/platform_orangepi_5b.md`.

## 1) Mức điện áp & bảo vệ
- Logic GPIO: 3.3V (xác nhận theo model). Không cắm thiết bị 5V trực tiếp.
- Bảo vệ: TVS diodes, series resistor cho đường tín hiệu ra ngoài.

## 2) UART mapping (cập nhật EM-02/EM-03)
| Chức năng | UART# | TX Pin | RX Pin | Mức | Ghi chú |
|---|---:|---|---|---|---|
| RS485    | UART1 | GPIOx  | GPIOy  | 3.3V| Qua transceiver RS485 → A/B; DE/RE = RTS hoặc GPIO rời (xem `platform_orangepi_5b.md`) |
| Console  | UART0 | GPIOx  | GPIOy  | 3.3V| Dành debug, không dùng cho điều khiển |
| MotorDrv | UART3 | GPIOx  | GPIOy  | 3.3V| Modbus RTU (tùy chọn) |

**Trạng thái hiện tại:**
- UART1: chưa bật được (chỉ có ttyS9 hoạt động)
- RS485: tạm thời dùng `/dev/ttyOHT485` → `ttyS9`
- Cần wiring để test loopback/BER

## 3) CAN mapping (nếu có tích hợp)
| Chức năng | IFACE | CAN_TX | CAN_RX | Mức | Ghi chú |
|---|---|---|---|---|---|
| CAN0 | MCAN0 | GPIOx | GPIOy | 3.3V | Qua transceiver CAN (H/L) |

## 4) RS485 transceiver kết nối
- IC điển hình: MAX485/ADM485/THVD… (TBD theo thiết kế)
- Kết nối A/B: xoắn đôi, có bias và termination (chi tiết: `14_rs485_can_transceiver.md`).
- DE/RE (Driver Enable/Receive Enable): nối GPIO điều khiển nửa song công.

## 5) Nguồn và mát
- 5V/12V/24V theo sơ đồ nguồn; bảo vệ quá áp/quá dòng.
- GND chung giữa Radxa, driver, và cảm biến; tránh vòng mát.

## 6) Chống nhiễu & layout
- Dây tín hiệu dài: cặp xoắn, che chắn, nối đất một đầu.
- Khoảng cách giữa đường công suất và tín hiệu ≥ 5 mm; tách plane.

## 7) Bảng tham chiếu nhanh
- RS485: A/B, termination 120Ω cuối tuyến, bias 680Ω–10kΩ (tùy chiều dài/thiết bị).
- CAN: CANH/CANL, termination 120Ω hai đầu, bias trung tính (theo transceiver).

## 8) Kiểm tra trước cấp nguồn
- Đo liên tục A/B và CANH/CANL: không chạm nhau/đất.
- Xác nhận DE/RE điều khiển đúng mức.

## 9) GPIO mapping (EM-02/EM-03)
| Tên logic | gpiochip | line | Chức năng | Trạng thái |
|---|---|---:|---|---|
| GPIO1_D3 | gpiochip1 | 3 | Relay1 hoặc DE/RE | Sẵn sàng test |
| GPIO1_D2 | gpiochip1 | 2 | Relay2 hoặc DE/RE | Đang dùng cho status_led |

Cập nhật lại bảng pin khi chốt model Radxa & HAT mở rộng.
