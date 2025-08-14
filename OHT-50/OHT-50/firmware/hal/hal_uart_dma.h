#pragma once

#include <stdint.h>
#include <stdbool.h>

void uart_dma_init(uint32_t baud, uint8_t parity, uint8_t stop_bits);
void uart_reconfigure(uint32_t baud, uint8_t parity, uint8_t stop_bits);
// RX ring buffer API
uint16_t uart_rx_available(void);
uint16_t uart_rx_read(uint8_t* dst, uint16_t max_len);
// TX API
int uart_tx_write(const uint8_t* data, uint16_t len);
bool uart_tx_is_busy(void);
int uart_tx_wait_complete(uint32_t timeout_ms);
// Errors/metrics
uint32_t uart_get_overrun_errors(void);
uint32_t uart_get_framing_errors(void);
void uart_clear_errors(void);

// Test/mô phỏng (tùy chọn): nạp dữ liệu vào RX buffer từ phần mềm
void uart_sim_rx_feed(const uint8_t* data, uint16_t len);


