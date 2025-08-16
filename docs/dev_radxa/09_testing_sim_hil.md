# 09) Kiểm thử (Unit/Integration/Sim/HIL)

## Unit
- CRC16, parser frame, planner S‑curve biên, state machine đường đi hạnh phúc & lỗi.

## Integration
- HAL RS485 giả lập → lệnh/feedback cho motor/io/safety/location.
- Center client cục bộ (mock WS/MQTT).

## Simulation
- `sim/rail_1d`: chạy profile đến station; đo sai số dừng.

## HIL (Hardware-in-the-Loop)
- Nối tối thiểu 1 driver, 1 DIO, Safety; đo timeout/retry và khoảng cách dừng.

## Tiêu chí chấp nhận
- Tất cả test xanh; không vi phạm giới hạn; E‑Stop đúng trình tự.
