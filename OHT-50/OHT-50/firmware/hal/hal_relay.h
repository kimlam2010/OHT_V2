#pragma once

#include <stdbool.h>

// HAL Relay control for Orange Pi 5B (draft)
// Channel 1 -> GPIO1_D3, Channel 2 -> GPIO1_D2 (mapping confirmed in docs)

typedef struct {
	int gpiochip_index_channel1; // e.g., 0 for gpiochip0
	int line_offset_channel1;    // e.g., 42
	int gpiochip_index_channel2;
	int line_offset_channel2;
} relay_hal_config_t;

// Initialize relay HAL with gpiochip/line offsets
bool relay_hal_init(const relay_hal_config_t *config);

// Set relay channel on/off (channel = 1 or 2)
bool relay_set(int channel, bool on);

// Release resources
void relay_hal_shutdown(void);


