#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "hal_rs485.h"
#include "hal_uart_dma.h"

#ifndef TEST_NUM_FRAMES
#define TEST_NUM_FRAMES 1000
#endif

#ifndef TEST_MIN_LEN
#define TEST_MIN_LEN 8
#endif

#ifndef TEST_MAX_LEN
#define TEST_MAX_LEN 64
#endif

static uint32_t popcount8(uint8_t v)
{
    // Kernighan's algorithm for 8-bit
    uint32_t c = 0;
    while (v) { v &= (uint8_t)(v - 1); c++; }
    return c;
}

static uint32_t count_bit_errors(const uint8_t* a, const uint8_t* b, uint16_t len)
{
    uint32_t bits = 0;
    for (uint16_t i = 0; i < len; ++i) {
        bits += popcount8((uint8_t)(a[i] ^ b[i]));
    }
    return bits;
}

int main(void)
{
    srand((unsigned int)time(NULL));

    rs485_init();
    uart_clear_errors();

    uint32_t total_bytes_sent = 0;
    uint32_t total_bytes_recv = 0;
    uint32_t total_bit_errors = 0;
    uint32_t total_overrun_before = 0, total_framing_before = 0;
    uint32_t total_overrun_after = 0, total_framing_after = 0;

    total_overrun_before = uart_get_overrun_errors();
    total_framing_before = uart_get_framing_errors();

    uint8_t tx_buf[TEST_MAX_LEN];
    uint8_t rx_buf[TEST_MAX_LEN];

    for (int f = 0; f < TEST_NUM_FRAMES; ++f) {
        const uint16_t len = (uint16_t)(TEST_MIN_LEN + (rand() % (TEST_MAX_LEN - TEST_MIN_LEN + 1)));
        for (uint16_t i = 0; i < len; ++i) {
            tx_buf[i] = (uint8_t)(rand() & 0xFF);
        }

        // Gửi frame (skeleton không thực sự đẩy ra UART; dùng feed để giả lập loopback)
        (void)rs485_send_frame(tx_buf, len);

        // Mô phỏng loopback: nạp lại dữ liệu vào RX ring buffer
        uart_sim_rx_feed(tx_buf, len);

        // Đọc lại
        memset(rx_buf, 0, sizeof(rx_buf));
        int rcv = rs485_recv_frame(rx_buf, len, 5 /*ms*/);
        if (rcv < 0) rcv = 0;

        total_bytes_sent += len;
        total_bytes_recv += (uint32_t)rcv;

        // So sánh và đếm bit errors (trên phần giao nhau)
        const uint16_t cmp_len = (rcv < (int)len) ? (uint16_t)rcv : len;
        total_bit_errors += count_bit_errors(tx_buf, rx_buf, cmp_len);
    }

    total_overrun_after = uart_get_overrun_errors();
    total_framing_after = uart_get_framing_errors();

    printf("RS485 Loopback (sim) completed\n");
    printf("Frames: %d\n", TEST_NUM_FRAMES);
    printf("Bytes sent: %u, received: %u\n", total_bytes_sent, total_bytes_recv);
    printf("Bit errors: %u\n", total_bit_errors);
    printf("Overrun: %u -> %u (delta %u)\n",
           total_overrun_before, total_overrun_after,
           (unsigned)(total_overrun_after - total_overrun_before));
    printf("Framing: %u -> %u (delta %u)\n",
           total_framing_before, total_framing_after,
           (unsigned)(total_framing_after - total_framing_before));

    return 0;
}


