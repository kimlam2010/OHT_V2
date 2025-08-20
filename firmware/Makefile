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

# Add configuration system sources
APP_SOURCES += $(SRC_DIR)/app/config_system.c

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
TEST_MODULE_REGISTRY_EXEC = $(BUILD_DIR)/test_module_registry
TEST_POWER_MODULE_EXEC = $(BUILD_DIR)/test_power_module
TEST_CONFIG_SYSTEM_EXEC = $(BUILD_DIR)/test_config_system
TEST_INTERLOCK_EXEC = $(BUILD_DIR)/test_interlock
TEST_SUITE_EXEC = $(BUILD_DIR)/test_suite

# Main application
MAIN_EXEC = $(BUILD_DIR)/oht50_main

# Default target
all: $(TEST_LIDAR_EXEC) $(TEST_RS485_EXEC) $(TEST_GPIO_EXEC) $(TEST_LED_EXEC) $(TEST_ESTOP_EXEC) $(TEST_RELAY_EXEC) $(TEST_NETWORK_EXEC) $(TEST_WIFI_SCAN_EXEC) $(TEST_SYSTEM_STATE_MACHINE_EXEC) $(TEST_SAFETY_MANAGER_EXEC) $(TEST_LED_MANAGER_EXEC) $(TEST_COMMUNICATION_MANAGER_EXEC) $(TEST_MODULE_MANAGER_EXEC) $(TEST_NETWORK_MANAGER_EXEC) $(TEST_MODULE_REGISTRY_EXEC) $(TEST_POWER_MODULE_EXEC) $(TEST_CONFIG_SYSTEM_EXEC) $(TEST_INTERLOCK_EXEC) $(TEST_SUITE_EXEC) $(MAIN_EXEC)

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

# Build main application object (src/main.c)
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c | $(BUILD_DIR)
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

$(TEST_COMMUNICATION_MANAGER_EXEC): $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/tests/test_communication_manager.o
	$(CC) $(CFLAGS) $^ -o $@ -lpthread


$(TEST_MODULE_MANAGER_EXEC): $(OBJ_DIR)/app/module_manager.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/tests/test_module_manager.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_NETWORK_MANAGER_EXEC): $(OBJ_DIR)/app/network_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_network.o $(OBJ_DIR)/tests/test_network_manager.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)



$(TEST_MODULE_REGISTRY_EXEC): $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/tests/test_module_registry.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

	# Include safety_manager since power_module_handler triggers safety faults
$(TEST_POWER_MODULE_EXEC): $(OBJ_DIR)/app/power_module_handler.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/test_power_module.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_CONFIG_SYSTEM_EXEC): $(OBJ_DIR)/app/config_system.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/tests/test_config_system.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_INTERLOCK_EXEC): $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/test_interlock.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_SUITE_EXEC): $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/app/system_state_machine.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/app/config_system.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/hal/hal_relay.o $(OBJ_DIR)/tests/test_suite.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Link main application
$(MAIN_EXEC): $(OBJ_DIR)/main.o $(OBJ_DIR)/app/system_state_machine.o $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/app/power_module_handler.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_rs485.o
	@echo "Linking $@..."
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

# Add Control Loop and Safety Mechanisms to the build
$(BUILD_DIR)/firmware: $(OBJ_DIR)/app/network_manager.o $(OBJ_DIR)/app/security_manager.o $(OBJ_DIR)/app/api_manager.o $(OBJ_DIR)/app/performance_manager.o $(OBJ_DIR)/app/diagnostics_manager.o $(OBJ_DIR)/app/system_state_machine.o $(OBJ_DIR)/app/control_loop.o $(OBJ_DIR)/app/safety_mechanisms.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_network.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/hal/hal_estop.o
	@echo "Linking $@"
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Add Control Loop object file
$(OBJ_DIR)/app/control_loop.o: src/app/control_loop.c include/control_loop.h include/hal_common.h include/system_state_machine.h
	@echo "Compiling $<..."
	@mkdir -p $(OBJ_DIR)/app
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

# Add Safety Mechanisms object file
$(OBJ_DIR)/app/safety_mechanisms.o: src/app/safety_mechanisms.c include/safety_mechanisms.h include/hal_common.h include/system_state_machine.h
	@echo "Compiling $<..."
	@mkdir -p $(OBJ_DIR)/app
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

# Test targets
test: test-lidar test-rs485 test-gpio test-led test-estop test-relay test-network test-system-state-machine test-safety-manager test-led-manager test-communication-manager test-module-manager test-network-manager test-control-loop test-safety-mechanisms test-module-registry test-power-module



# Control Loop test
TEST_CONTROL_LOOP_EXEC = $(BUILD_DIR)/test_control_loop

$(OBJ_DIR)/tests/test_control_loop.o: tests/test_control_loop.c include/control_loop.h include/hal_common.h
	@echo "Compiling $<..."
	@mkdir -p $(OBJ_DIR)/tests
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

$(TEST_CONTROL_LOOP_EXEC): $(OBJ_DIR)/app/control_loop.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/tests/test_control_loop.o
	@echo "Linking $@"
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

test-control-loop: $(TEST_CONTROL_LOOP_EXEC)
	@echo "Running Control Loop tests..."
	@$(TEST_CONTROL_LOOP_EXEC)

# Safety Mechanisms test
TEST_SAFETY_MECHANISMS_EXEC = $(BUILD_DIR)/test_safety_mechanisms

$(OBJ_DIR)/tests/test_safety_mechanisms.o: tests/test_safety_mechanisms.c include/safety_mechanisms.h include/hal_common.h
	@echo "Compiling $<..."
	@mkdir -p $(OBJ_DIR)/tests
	$(CC) $(CFLAGS) -Iinclude -c $< -o $@

$(TEST_SAFETY_MECHANISMS_EXEC): $(OBJ_DIR)/app/safety_mechanisms.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/tests/test_safety_mechanisms.o
	@echo "Linking $@"
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

test-safety-mechanisms: $(TEST_SAFETY_MECHANISMS_EXEC)
	@echo "Running Safety Mechanisms tests..."
	@$(TEST_SAFETY_MECHANISMS_EXEC)

test-module-registry: $(TEST_MODULE_REGISTRY_EXEC)
	@echo "Running Module Registry tests..."
	@$(TEST_MODULE_REGISTRY_EXEC)

test-power-module: $(TEST_POWER_MODULE_EXEC)
	@echo "Running Power Module tests..."
	@$(TEST_POWER_MODULE_EXEC)

# Add test-week4-modules to .PHONY
.PHONY: all clean test test-lidar test-rs485 test-gpio test-led test-estop test-relay test-network test-system-state-machine test-safety-manager test-led-manager test-communication-manager test-module-manager test-network-manager test-control-loop test-safety-mechanisms test-module-registry test-power-module
