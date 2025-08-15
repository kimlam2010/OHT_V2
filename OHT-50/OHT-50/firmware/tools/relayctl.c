#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "../hal/hal_relay.h"

int main(int argc, char **argv) {
	if (argc < 3) {
		fprintf(stderr, "Usage: %s <channel:1|2> <on|off>\n", argv[0]);
		return 1;
	}

	int channel = atoi(argv[1]);
	bool on = strcmp(argv[2], "on") == 0;

	relay_hal_config_t cfg = {
		.gpiochip_index_channel1 = 0,
		.line_offset_channel1 = -1, // TODO: fill with actual offset
		.gpiochip_index_channel2 = 0,
		.line_offset_channel2 = -1  // TODO: fill with actual offset
	};

	if (!relay_hal_init(&cfg)) {
		fprintf(stderr, "relay_hal_init failed\n");
		return 2;
	}

	bool ok = relay_set(channel, on);
	relay_hal_shutdown();

	if (!ok) {
		fprintf(stderr, "relay_set failed\n");
		return 3;
	}

	printf("Relay %d -> %s\n", channel, on ? "ON" : "OFF");
	return 0;
}


