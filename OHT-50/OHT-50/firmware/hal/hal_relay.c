#include "hal_relay.h"

// Note: This is a stub to be implemented with libgpiod.
// For now, it only records config and pretends to succeed.

static relay_hal_config_t s_cfg;
static bool s_initialized = false;

bool relay_hal_init(const relay_hal_config_t *config) {
	if (config == NULL) return false;
	s_cfg = *config;
	s_initialized = true;
	return true;
}

bool relay_set(int channel, bool on) {
	if (!s_initialized) return false;
	if (channel != 1 && channel != 2) return false;
	// TODO: implement with libgpiod gpioset
	(void)on;
	return true;
}

void relay_hal_shutdown(void) {
	s_initialized = false;
}


