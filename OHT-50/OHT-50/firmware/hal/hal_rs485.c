#include "hal_rs485.h"
#include "hal_uart_dma.h"
#include "board_config.h"
#include <string.h>

// NOTE: Skeleton – thay bằng thao tác GPIO thực tế
static void gpio_set(uint32_t pin, int level) { (void)pin; (void)level; }
static void gpio_init_out(uint32_t pin) { (void)pin; }
static void delay_us(uint32_t us) { (void)us; /* TODO: implement platform delay */ }

void rs485_init(void)
{
    gpio_init_out(RS485_DE_PIN);
    gpio_init_out(RS485_RE_PIN);
    rs485_set_tx_mode(false); // RX mode mặc định
    uart_dma_init(RS485_BAUD_DEFAULT, RS485_PARITY_DEFAULT, RS485_STOP_DEFAULT);
}

void rs485_set_tx_mode(bool enable)
{
    if (enable) {
        gpio_set(RS485_DE_PIN, 1);
        gpio_set(RS485_RE_PIN, 1);
    } else {
        gpio_set(RS485_DE_PIN, 0);
        gpio_set(RS485_RE_PIN, 0);
    }
}

int rs485_send_frame(const uint8_t* data, uint16_t len)
{
    rs485_set_tx_mode(true);
    // thời gian chờ DE assert trước khi gửi
    delay_us(RS485_DE_ASSERT_US);
    int written = uart_tx_write(data, len);
    // chờ TX hoàn tất trước khi hạ DE
    (void)uart_tx_wait_complete(10);
    // giữ DE thêm một khoảng tail time để đảm bảo byte stop đã ra khỏi line
    delay_us(RS485_DE_TAIL_US);
    rs485_set_tx_mode(false);
    return written;
}

int rs485_recv_frame(uint8_t* buf, uint16_t buf_size, uint32_t timeout_ms)
{
    uint32_t elapsed = 0;
    const uint32_t step_ms = 1;
    uint16_t total = 0;
    while (elapsed < timeout_ms && total < buf_size) {
        uint16_t avail = uart_rx_available();
        if (avail) {
            uint16_t n = (avail > (buf_size - total)) ? (buf_size - total) : avail;
            total += uart_rx_read(buf + total, n);
            // Có thể thêm điều kiện dừng theo độ dài tối thiểu…
        } else {
            // sleep 1 ms – thay bằng delay phù hợp nền tảng
        }
        elapsed += step_ms;
    }
    return (int)total;
}


