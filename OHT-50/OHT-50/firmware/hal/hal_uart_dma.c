#include "hal_uart_dma.h"
#include "board_config.h"
#include <string.h>

// NOTE: Đây là skeleton – thay bằng HAL thực tế của nền tảng (STM32 HAL, Zephyr, v.v.).

// RX ring buffer giả lập (thay bằng DMA circular buffer trên nền tảng thực)
#define UART_RX_BUF_SIZE 1024
static volatile uint8_t s_rx_buf[UART_RX_BUF_SIZE];
static volatile uint16_t s_rx_head = 0;
static volatile uint16_t s_rx_tail = 0;

// Lỗi/metrics
static volatile uint32_t s_overrun_errors = 0;
static volatile uint32_t s_framing_errors = 0;
static volatile uint8_t s_tx_busy = 0;
static uint32_t s_baud = 0;
static uint8_t s_parity = 0;
static uint8_t s_stop_bits = 1;

void uart_dma_init(uint32_t baud, uint8_t parity, uint8_t stop_bits)
{
    (void)parity; (void)stop_bits;
    // TODO: enable clock cho UART/GPIO, cấu hình TX/RX pin theo RS485_TX_PIN/RS485_RX_PIN
    // TODO: cấu hình UART (RS485_UART_INSTANCE) với baud; bật DMA cho RX/TX, RX circular
    // Reset buffer
    s_rx_head = s_rx_tail = 0;
    memset((void*)s_rx_buf, 0, sizeof(s_rx_buf));
    s_tx_busy = 0;
    s_baud = baud;
    s_parity = parity;
    s_stop_bits = stop_bits;
}

void uart_reconfigure(uint32_t baud, uint8_t parity, uint8_t stop_bits)
{
    // TODO: cấu hình lại UART thật; ở skeleton chỉ lưu tham số
    s_baud = baud;
    s_parity = parity;
    s_stop_bits = stop_bits;
}

// Trả số byte có sẵn trong RX buffer
uint16_t uart_rx_available(void)
{
    uint16_t head = s_rx_head;
    uint16_t tail = s_rx_tail;
    if (head >= tail) return (uint16_t)(head - tail);
    return (uint16_t)(UART_RX_BUF_SIZE - tail + head);
}

// Đọc tối đa max_len byte từ RX buffer
uint16_t uart_rx_read(uint8_t* dst, uint16_t max_len)
{
    uint16_t cnt = 0;
    while (cnt < max_len && s_rx_tail != s_rx_head) {
        dst[cnt++] = s_rx_buf[s_rx_tail];
        s_rx_tail = (uint16_t)((s_rx_tail + 1) % UART_RX_BUF_SIZE);
    }
    return cnt;
}

// Ghi TX (blocking) – skeleton: giả sử ghi thành công
int uart_tx_write(const uint8_t* data, uint16_t len)
{
    // TODO: đẩy data vào DMA TX / FIFO TX của UART
    (void)data;
    s_tx_busy = 1;
    // Trong skeleton, giả lập truyền xong tức thì
    s_tx_busy = 0;
    return (int)len;
}

bool uart_tx_is_busy(void) { return s_tx_busy != 0; }

int uart_tx_wait_complete(uint32_t timeout_ms)
{
    uint32_t elapsed = 0;
    const uint32_t step_ms = 1;
    while (elapsed < timeout_ms) {
        if (!uart_tx_is_busy()) return 0; // OK
        // TODO: sleep/delay 1ms
        elapsed += step_ms;
    }
    return -1; // timeout
}

uint32_t uart_get_overrun_errors(void) { return s_overrun_errors; }
uint32_t uart_get_framing_errors(void) { return s_framing_errors; }
void uart_clear_errors(void) { s_overrun_errors = 0; s_framing_errors = 0; }

void uart_sim_rx_feed(const uint8_t* data, uint16_t len)
{
    // Nạp dữ liệu vào RX ring buffer (phục vụ mô phỏng/dev)
    for (uint16_t i = 0; i < len; ++i) {
        uint16_t next_head = (uint16_t)((s_rx_head + 1) % UART_RX_BUF_SIZE);
        if (next_head == s_rx_tail) {
            // buffer đầy → tăng overrun
            s_overrun_errors++;
            break;
        }
        s_rx_buf[s_rx_head] = data[i];
        s_rx_head = next_head;
    }
}


