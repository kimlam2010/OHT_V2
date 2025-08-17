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
APP_SOURCES = $(wildcard $(SRC_DIR)/app/*.c)
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)

# Object files
HAL_OBJECTS = $(HAL_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
APP_OBJECTS = $(APP_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
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
TEST_SYSTEM_STATE_MACHINE_EXEC = $(BUILD_DIR)/test_system_state_machine
TEST_SAFETY_MANAGER_EXEC = $(BUILD_DIR)/test_safety_manager
TEST_LED_MANAGER_EXEC = $(BUILD_DIR)/test_led_manager
TEST_COMMUNICATION_MANAGER_EXEC = $(BUILD_DIR)/test_communication_manager
TEST_MODULE_MANAGER_EXEC = $(BUILD_DIR)/test_module_manager
TEST_NETWORK_MANAGER_EXEC = $(BUILD_DIR)/test_network_manager
TEST_WEEK3_MODULES_EXEC = $(BUILD_DIR)/test_week3_modules

# Default target
all: $(TEST_LIDAR_EXEC) $(TEST_RS485_EXEC) $(TEST_GPIO_EXEC) $(TEST_LED_EXEC) $(TEST_ESTOP_EXEC) $(TEST_RELAY_EXEC) $(TEST_NETWORK_EXEC) $(TEST_WIFI_SCAN_EXEC) $(TEST_SYSTEM_STATE_MACHINE_EXEC) $(TEST_SAFETY_MANAGER_EXEC) $(TEST_LED_MANAGER_EXEC) $(TEST_COMMUNICATION_MANAGER_EXEC) $(TEST_MODULE_MANAGER_EXEC) $(TEST_NETWORK_MANAGER_EXEC) $(TEST_WEEK3_MODULES_EXEC)

# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)/hal
	@mkdir -p $(OBJ_DIR)/app
	@mkdir -p $(OBJ_DIR)/tests

# Build HAL object files
$(OBJ_DIR)/hal/%.o: $(SRC_DIR)/hal/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Build APP object files
$(OBJ_DIR)/app/%.o: $(SRC_DIR)/app/%.c | $(BUILD_DIR)
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

$(TEST_SYSTEM_STATE_MACHINE_EXEC): $(OBJ_DIR)/app/system_state_machine.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/test_system_state_machine.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_SAFETY_MANAGER_EXEC): $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/test_safety_manager.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_LED_MANAGER_EXEC): $(OBJ_DIR)/app/led_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/test_led_manager.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_COMMUNICATION_MANAGER_EXEC): $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/tests/test_communication_manager.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread

$(TEST_MODULE_MANAGER_EXEC): $(OBJ_DIR)/app/module_manager.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/tests/test_module_manager.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_NETWORK_MANAGER_EXEC): $(OBJ_DIR)/app/network_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_network.o $(OBJ_DIR)/tests/test_network_manager.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_WEEK3_MODULES_EXEC): $(OBJ_DIR)/app/network_manager.o $(OBJ_DIR)/app/security_manager.o $(OBJ_DIR)/app/api_manager.o $(OBJ_DIR)/app/system_state_machine.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_network.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/tests/test_week3_modules.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Add Performance Manager and Diagnostics Manager to the build
$(BUILD_DIR)/firmware: $(OBJ_DIR)/app/network_manager.o $(OBJ_DIR)/app/security_manager.o $(OBJ_DIR)/app/api_manager.o $(OBJ_DIR)/app/performance_manager.o $(OBJ_DIR)/app/diagnostics_manager.o $(OBJ_DIR)/app/system_state_machine.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_network.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/hal/hal_estop.o
	@echo "Linking $@"
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Add Performance Manager object file
$(OBJ_DIR)/app/performance_manager.o: src/app/performance_manager.c include/performance_manager.h include/hal_common.h
	@echo "Compiling $<..."
	@mkdir -p $(OBJ_DIR)/app
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

# Add Diagnostics Manager object file
$(OBJ_DIR)/app/diagnostics_manager.o: src/app/diagnostics_manager.c include/diagnostics_manager.h include/hal_common.h
	@echo "Compiling $<..."
	@mkdir -p $(OBJ_DIR)/app
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

# Test targets
test: test-lidar test-rs485 test-gpio test-led test-estop test-relay test-network test-system-state-machine test-safety-manager test-led-manager test-communication-manager test-module-manager test-network-manager test-week3-modules test-week4-modules

# Week 4 modules test
TEST_WEEK4_MODULES_EXEC = $(BUILD_DIR)/test_week4_modules

$(OBJ_DIR)/tests/test_week4_modules.o: tests/test_week4_modules.c include/performance_manager.h include/diagnostics_manager.h include/hal_common.h
	@echo "Compiling $<..."
	@mkdir -p $(OBJ_DIR)/tests
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

$(TEST_WEEK4_MODULES_EXEC): $(OBJ_DIR)/app/performance_manager.o $(OBJ_DIR)/app/diagnostics_manager.o $(OBJ_DIR)/app/system_state_machine.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_network.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/tests/test_week4_modules.o
	@echo "Linking $@"
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

test-week4-modules: $(TEST_WEEK4_MODULES_EXEC)
	@echo "Running Week 4 Modules tests..."
	@$(TEST_WEEK4_MODULES_EXEC)

# Add test-week4-modules to .PHONY
.PHONY: all clean test test-lidar test-rs485 test-gpio test-led test-estop test-relay test-network test-system-state-machine test-safety-manager test-led-manager test-communication-manager test-module-manager test-network-manager test-week3-modules test-week4-modules
