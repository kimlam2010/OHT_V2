#pragma once

/*
 Pin mapping cho Orange Pi 5B (RK3588) theo tài liệu dev_radxa/platform_orangepi_5b.md
 UART1 cho RS485 bus (GPIO0_A2/A3)
 GPIO1_D3 cho DE/RE control
*/

// UART instance/port (Orange Pi 5B) – dùng UART1 cho RS485
#define RS485_UART_INSTANCE   UART1

// GPIO definitions cho Orange Pi 5B
#ifndef GPIO0_A2
#define GPIO0_A2 ((uint32_t)0x0A2)  // UART1_TX
#endif
#ifndef GPIO0_A3
#define GPIO0_A3 ((uint32_t)0x0A3)  // UART1_RX
#endif
#ifndef GPIO1_D3
#define GPIO1_D3 ((uint32_t)0x1D3)  // Relay1/DE-RE control
#endif
#ifndef GPIO1_D2
#define GPIO1_D2 ((uint32_t)0x1D2)  // Relay2/DE-RE control (đang dùng cho status_led)
#endif

// TX/RX pins theo sơ đồ Orange Pi 5B (UART1_TX_M1 / UART1_RX_M1)
#define RS485_TX_PIN          GPIO0_A2
#define RS485_RX_PIN          GPIO0_A3

// DE/RE control pin (active-high) – dùng chung GPIO1_D3
#define RS485_DE_PIN          GPIO1_D3
#define RS485_RE_PIN          GPIO1_D3

// Relay control pins
#define RELAY1_PIN            GPIO1_D3
#define RELAY2_PIN            GPIO1_D2

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

// Device paths
#define RS485_DEVICE_PATH     "/dev/ttyS1"  // UART1 device
#define RS485_DEVICE_ALIAS    "/dev/ttyOHT485"  // udev alias

// GPIO chip mapping (libgpiod)
#define GPIO_CHIP1            "gpiochip1"
#define GPIO_CHIP0            "gpiochip0"

// GPIO line mapping (libgpiod)
#define GPIO1_D3_LINE         3  // gpiochip1:3
#define GPIO1_D2_LINE         2  // gpiochip1:2

// Timeout values
#define RS485_TIMEOUT_MS      50
#define RS485_RETRY_COUNT     2
#define RS485_RETRY_DELAY_MS  10

// Buffer sizes
#define RS485_RX_BUFFER_SIZE  1024
#define RS485_TX_BUFFER_SIZE  512
#define UART_DMA_BUFFER_SIZE  2048

// Safety timeouts
#define SAFETY_WATCHDOG_MS    1000
#define ESTOP_DEBOUNCE_MS     50


