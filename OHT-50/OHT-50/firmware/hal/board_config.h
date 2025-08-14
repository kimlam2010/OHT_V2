#pragma once

/*
 Mapping chân cho RS485/UART theo tài liệu dev_radxa/pinout_radxa.md
 Gợi ý (điền theo model Radxa thực tế):
 - Ưu tiên UART2 cho RS485 (theo pinout mẫu trong docs), nếu khác model thì chọn UART có DMA.
 - Chọn 1 GPIO rảnh cho DE/RE (có thể dùng chung: DE=RE, mức active-high).
 - Mức logic: TX/RX 3.3V; DE/RE 3.3V.
*/

// UART instance/port (Orange Pi 5B) – dùng UART1 cho RS485
#define RS485_UART_INSTANCE   UART1

// Định danh GPIO minh họa (mã hóa bank+index), thay bằng macro HAL thực tế khi tích hợp
#ifndef GPIO1_B6
#define GPIO1_B6 ((uint32_t)0x1B6)
#endif
#ifndef GPIO1_B7
#define GPIO1_B7 ((uint32_t)0x1B7)
#endif
#ifndef GPIO1_C6
#define GPIO1_C6 ((uint32_t)0x1C6)
#endif

// TX/RX pins theo sơ đồ Orange Pi 5B (UART1_TX_M1 / UART1_RX_M1)
#define RS485_TX_PIN          GPIO1_B6
#define RS485_RX_PIN          GPIO1_B7

// DE/RE control pin (active-high) – dùng chung GPIO1_C6
#define RS485_DE_PIN          GPIO1_C6
#define RS485_RE_PIN          GPIO1_C6

// Cấu hình UART mặc định
#define RS485_BAUD_DEFAULT    115200
#define RS485_PARITY_DEFAULT  0       // 0=None, 1=Even, 2=Odd (tuỳ HAL)
#define RS485_STOP_DEFAULT    1       // Stop bits

// Thời gian chờ cho DE/RE (micro giây) – tinh chỉnh theo transceiver thực tế
#ifndef RS485_DE_ASSERT_US
#define RS485_DE_ASSERT_US    2
#endif
#ifndef RS485_DE_TAIL_US
#define RS485_DE_TAIL_US      2
#endif


