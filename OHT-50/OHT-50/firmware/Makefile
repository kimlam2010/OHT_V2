CC=gcc
CFLAGS=-I. -Ihal -Icontrol -Isafety -O2 -Wall -Wextra -Werror

# Build targets
all: test_sm test_safety loopback test_dio test_power test_motor test_location

# State machine tests
test_sm: tests/test_state_machine.c control/state_machine.c
	$(CC) $(CFLAGS) -o $@ $^

# Safety tests
test_safety: tests/test_safety_manager.c safety/safety_manager.c
	$(CC) $(CFLAGS) -o $@ $^

# RS485 loopback test
loopback: examples/rs485_loopback.c hal/hal_rs485.c hal/hal_uart_dma.c
	$(CC) $(CFLAGS) -o $@ $^

# DI/DO module test
test_dio: tests/test_dio.c hal/hal_rs485.c hal/hal_uart_dma.c
	$(CC) $(CFLAGS) -o $@ $^

# Power module test
test_power: tests/test_power.c hal/hal_rs485.c hal/hal_uart_dma.c
	$(CC) $(CFLAGS) -o $@ $^

# Motor module test
test_motor: tests/test_motor.c hal/hal_rs485.c hal/hal_uart_dma.c
	$(CC) $(CFLAGS) -o $@ $^

# Location module test
test_location: tests/test_location.c hal/hal_rs485.c hal/hal_uart_dma.c
	$(CC) $(CFLAGS) -o $@ $^

# Clean all build artifacts
clean:
	rm -f test_sm test_safety loopback test_dio test_power test_motor test_location

# Clean and rebuild everything
rebuild: clean all

# Build specific module test
build_dio: test_dio
build_power: test_power
build_motor: test_motor
build_location: test_location

.PHONY: all clean rebuild build_dio build_power build_motor build_location


