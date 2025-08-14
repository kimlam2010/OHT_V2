# OHT-50 Firmware HAL Skeleton

Mục tiêu: EM-02/EM-03 (DE/RE + UART DMA ring buffer) và loopback/BER logging.

Thư mục dự kiến:
- `hal/hal_uart_dma.h/.c`: UART init, DMA RX/TX, ring buffer, timeout.
- `hal/hal_rs485.h/.c`: Điều khiển DE/RE (half-duplex), API gửi/nhận khung.

API bổ sung:
- `uart_reconfigure(baud, parity, stop_bits)` – đổi tham số UART động.
- `uart_tx_wait_complete(timeout_ms)` – chờ TX xong để an toàn hạ DE.
- `uart_sim_rx_feed(data,len)` – nạp dữ liệu mô phỏng vào RX.
- `uart_clear_errors()` – reset bộ đếm lỗi.

## Ví dụ loopback (dev)

File: `firmware/examples/rs485_loopback.c`

Chạy thử trên môi trường dev (skeleton, giả lập loopback bằng `uart_sim_rx_feed()`):

1) Biên dịch nhanh bằng gcc (máy phát triển):
```
gcc -Ihal examples/rs485_loopback.c hal/hal_rs485.c hal/hal_uart_dma.c -o rs485_loopback
```

2) Chạy:
```
./rs485_loopback
```

Tham số có thể thay qua macro:
- `TEST_NUM_FRAMES` (mặc định 1000)
- `TEST_MIN_LEN` (mặc định 8), `TEST_MAX_LEN` (mặc định 64)

Kết quả in: số frame, byte gửi/nhận, bit error (BER), overrun/framing delta.

Ghi chú: Ở nền tảng thật, thay thế `uart_tx_write()`/DMA TX và ISR RX để đo thực tế; cập nhật `RS485_DE_ASSERT_US/TAIL_US` theo transceiver.

Lưu ý:
- Map chân UART (TX/RX) và GPIO DE/RE theo `docs/dev_radxa/pinout_radxa.md`.
- ISR tối thiểu; xử lý nền qua ring buffer.
- Timeout mục tiêu: 50 ms; retry ở lớp trên.
