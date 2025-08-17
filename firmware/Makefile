# Master Module Firmware Makefile
# EMBED Team - OHT-50 Project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
LIBS = -lpthread

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
TEST_DIR = tests

# Source files
HAL_SOURCES = $(wildcard $(SRC_DIR)/hal/*.c)
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)

# Object files
HAL_OBJECTS = $(HAL_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(OBJ_DIR)/tests/%.o)

# Target executables
TEST_LIDAR_EXEC = $(BUILD_DIR)/test_lidar
TEST_RS485_EXEC = $(BUILD_DIR)/test_rs485
TEST_GPIO_EXEC = $(BUILD_DIR)/test_gpio
TEST_LED_EXEC = $(BUILD_DIR)/test_led
TEST_ESTOP_EXEC = $(BUILD_DIR)/test_estop
TEST_RELAY_EXEC = $(BUILD_DIR)/test_relay
TEST_NETWORK_EXEC = $(BUILD_DIR)/test_network
TEST_WIFI_SCAN_EXEC = $(BUILD_DIR)/test_wifi_scan

# Default target
all: $(TEST_LIDAR_EXEC) $(TEST_RS485_EXEC) $(TEST_GPIO_EXEC) $(TEST_LED_EXEC) $(TEST_ESTOP_EXEC) $(TEST_RELAY_EXEC) $(TEST_NETWORK_EXEC) $(TEST_WIFI_SCAN_EXEC)

# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)/hal
	@mkdir -p $(OBJ_DIR)/tests

# Build HAL object files
$(OBJ_DIR)/hal/%.o: $(SRC_DIR)/hal/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Build test object files
$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Build test executables
$(TEST_LIDAR_EXEC): $(OBJ_DIR)/hal/hal_lidar.o $(OBJ_DIR)/tests/test_lidar.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_RS485_EXEC): $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/tests/test_rs485.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_GPIO_EXEC): $(OBJ_DIR)/hal/hal_gpio.o $(OBJ_DIR)/tests/test_gpio.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_LED_EXEC): $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/test_led.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_ESTOP_EXEC): $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/tests/test_estop.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_RELAY_EXEC): $(OBJ_DIR)/hal/hal_relay.o $(OBJ_DIR)/tests/test_relay.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_NETWORK_EXEC): $(OBJ_DIR)/hal/hal_network.o $(OBJ_DIR)/tests/test_network.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_WIFI_SCAN_EXEC): $(OBJ_DIR)/hal/hal_network.o $(OBJ_DIR)/tests/test_wifi_scan.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Test targets
test: test-lidar test-rs485 test-gpio test-led test-estop test-relay test-network

test-lidar: $(TEST_LIDAR_EXEC)
	@echo "Running LiDAR tests..."
	@./$(TEST_LIDAR_EXEC)

test-rs485: $(TEST_RS485_EXEC)
	@echo "Running RS485 tests..."
	@./$(TEST_RS485_EXEC)

test-gpio: $(TEST_GPIO_EXEC)
	@echo "Running GPIO tests..."
	@./$(TEST_GPIO_EXEC)

test-led: $(TEST_LED_EXEC)
	@echo "Running LED tests..."
	@./$(TEST_LED_EXEC)

test-estop: $(TEST_ESTOP_EXEC)
	@echo "Running E-Stop tests..."
	@./$(TEST_ESTOP_EXEC)

test-relay: $(TEST_RELAY_EXEC)
	@echo "Running Relay tests..."
	@./$(TEST_RELAY_EXEC)

test-network: $(TEST_NETWORK_EXEC)
	@echo "Running Network tests..."
	@./$(TEST_NETWORK_EXEC)

test-wifi-scan: $(TEST_WIFI_SCAN_EXEC)
	@echo "Running WiFi scan..."
	@./$(TEST_WIFI_SCAN_EXEC)

# Clean target
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)

# Install target (for system installation)
install: all
	@echo "Installing Master Module firmware..."
	@sudo cp $(BUILD_DIR)/* /usr/local/bin/
	@sudo chmod +x /usr/local/bin/test_*

# Uninstall target
uninstall:
	@echo "Uninstalling Master Module firmware..."
	@sudo rm -f /usr/local/bin/test_*

# Help target
help:
	@echo "Master Module Firmware Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all        - Build all test executables"
	@echo "  test       - Run all tests"
	@echo "  test-*     - Run specific test (lidar, rs485, gpio, led, estop, relay)"
	@echo "  clean      - Clean build files"
	@echo "  install    - Install firmware to system"
	@echo "  uninstall  - Uninstall firmware from system"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make all           # Build all executables"
	@echo "  make test-led      # Run LED tests only"
	@echo "  make clean         # Clean build files"

.PHONY: all test test-lidar test-rs485 test-gpio test-led test-estop test-relay test-network test-wifi-scan clean install uninstall help
