#pragma once

#include <stdint.h>
#include <stdbool.h>

// RS485 DE/RE control
void rs485_init(void);
void rs485_set_tx_mode(bool enable); // true: TX (DE=1/RE=1), false: RX (DE=0/RE=0)

// Frame send/recv (blocking with timeout in ms)
int rs485_send_frame(const uint8_t* data, uint16_t len);
int rs485_recv_frame(uint8_t* buf, uint16_t buf_size, uint32_t timeout_ms);


