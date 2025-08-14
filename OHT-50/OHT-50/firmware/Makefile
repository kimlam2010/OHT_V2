CC=gcc
CFLAGS=-I. -Ihal -Icontrol -Isafety -O2 -Wall -Wextra -Werror

all: test_sm test_safety loopback

test_sm: tests/test_state_machine.c control/state_machine.c
	$(CC) $(CFLAGS) -o $@ $^

test_safety: tests/test_safety_manager.c safety/safety_manager.c
	$(CC) $(CFLAGS) -o $@ $^

loopback: examples/rs485_loopback.c hal/hal_rs485.c hal/hal_uart_dma.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f test_sm test_safety loopback

.PHONY: all clean


