# Master Module Firmware Makefile - OHT-50 Project
# EMBED Team - Version 3.0.0
# Single Makefile for all firmware components

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200809L -O2 -g
CFLAGS += -DMISRA_C_2012 -DAUTOSAR -DSIL2
CFLAGS += -D_GNU_SOURCE
CFLAGS += -I$(INCLUDE_DIR) -I$(HAL_INCLUDE_DIR) -I$(CORE_INCLUDE_DIR)
CFLAGS += -I$(MODULES_INCLUDE_DIR) -I$(SERVICES_INCLUDE_DIR) -I$(CONTROL_INCLUDE_DIR)
CFLAGS += -I$(UTILITIES_INCLUDE_DIR)
LIBS = -lpthread -lm

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
TEST_BUILD_DIR = $(BUILD_DIR)/tests

# Source directories
CORE_DIR = $(SRC_DIR)/01-CORE
HAL_DIR = $(SRC_DIR)/02-HAL
MODULES_DIR = $(SRC_DIR)/03-MODULES
SERVICES_DIR = $(SRC_DIR)/04-SERVICES
CONTROL_DIR = $(SRC_DIR)/05-CONTROL
UTILITIES_DIR = $(SRC_DIR)/06-UTILITIES

# Include directories
CORE_INCLUDE_DIR = $(INCLUDE_DIR)/01-CORE
HAL_INCLUDE_DIR = $(INCLUDE_DIR)/02-HAL
MODULES_INCLUDE_DIR = $(INCLUDE_DIR)/03-MODULES
SERVICES_INCLUDE_DIR = $(INCLUDE_DIR)/04-SERVICES
CONTROL_INCLUDE_DIR = $(INCLUDE_DIR)/05-CONTROL
UTILITIES_INCLUDE_DIR = $(INCLUDE_DIR)/06-UTILITIES

# Test directories
TEST_DIR = tests
TEST_UTILS_DIR = $(TEST_DIR)/utils
HAL_TEST_DIR = $(TEST_DIR)/hal
MANAGERS_TEST_DIR = $(TEST_DIR)/managers
CORE_TEST_DIR = $(TEST_DIR)/core
MODULES_TEST_DIR = $(TEST_DIR)/modules
INTEGRATION_TEST_DIR = $(TEST_DIR)/integration
APP_TEST_DIR = $(TEST_DIR)/app

# Source files - Only existing files
CORE_SOURCES = $(wildcard $(CORE_DIR)/*/*.c)
HAL_SOURCES = $(wildcard $(HAL_DIR)/*/*.c)
MODULES_SOURCES = $(wildcard $(MODULES_DIR)/*/*.c)
SERVICES_SOURCES = $(wildcard $(SERVICES_DIR)/*/*.c)
CONTROL_SOURCES = $(wildcard $(CONTROL_DIR)/*/*.c)
UTILITIES_SOURCES = $(wildcard $(UTILITIES_DIR)/*/*.c)

# Test sources - Only existing files
TEST_UTILS_SOURCES = $(wildcard $(TEST_UTILS_DIR)/*.c)
HAL_TEST_SOURCES = $(wildcard $(HAL_TEST_DIR)/*.c)
MANAGERS_TEST_SOURCES = $(wildcard $(MANAGERS_TEST_DIR)/*.c)
CORE_TEST_SOURCES = $(wildcard $(CORE_TEST_DIR)/*.c)
MODULES_TEST_SOURCES = $(wildcard $(MODULES_TEST_DIR)/*.c)
INTEGRATION_TEST_SOURCES = $(wildcard $(INTEGRATION_TEST_DIR)/*.c)
APP_TEST_SOURCES = $(wildcard $(APP_TEST_DIR)/*.c)

# Object files
CORE_OBJECTS = $(CORE_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
HAL_OBJECTS = $(HAL_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
MODULES_OBJECTS = $(MODULES_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
SERVICES_OBJECTS = $(SERVICES_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
CONTROL_OBJECTS = $(CONTROL_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
UTILITIES_OBJECTS = $(UTILITIES_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Test object files
TEST_UTILS_OBJECTS = $(TEST_UTILS_SOURCES:$(TEST_UTILS_DIR)/%.c=$(OBJ_DIR)/tests/utils/%.o)
HAL_TEST_OBJECTS = $(HAL_TEST_SOURCES:$(HAL_TEST_DIR)/%.c=$(OBJ_DIR)/tests/hal/%.o)
MANAGERS_TEST_OBJECTS = $(MANAGERS_TEST_SOURCES:$(MANAGERS_TEST_DIR)/%.c=$(OBJ_DIR)/tests/managers/%.o)
CORE_TEST_OBJECTS = $(CORE_TEST_SOURCES:$(CORE_TEST_DIR)/%.c=$(OBJ_DIR)/tests/core/%.o)
MODULES_TEST_OBJECTS = $(MODULES_TEST_SOURCES:$(MODULES_TEST_DIR)/%.c=$(OBJ_DIR)/tests/modules/%.o)
INTEGRATION_TEST_OBJECTS = $(INTEGRATION_TEST_SOURCES:$(INTEGRATION_TEST_DIR)/%.c=$(OBJ_DIR)/tests/integration/%.o)
APP_TEST_OBJECTS = $(APP_TEST_SOURCES:$(APP_TEST_DIR)/%.c=$(OBJ_DIR)/tests/app/%.o)

# All HAL objects for testing
ALL_HAL_OBJECTS = $(HAL_OBJECTS)

# Target executables - Only for existing test files
TEST_HAL_COMMON_EXEC = $(TEST_BUILD_DIR)/test_hal_common
TEST_ESTOP_EXEC = $(TEST_BUILD_DIR)/test_estop
TEST_RS485_EXEC = $(TEST_BUILD_DIR)/test_rs485
TEST_RELAY_EXEC = $(TEST_BUILD_DIR)/test_relay
TEST_GPIO_EXEC = $(TEST_BUILD_DIR)/test_gpio
TEST_LED_EXEC = $(TEST_BUILD_DIR)/test_led
TEST_LIDAR_EXEC = $(TEST_BUILD_DIR)/test_lidar
TEST_NETWORK_EXEC = $(TEST_BUILD_DIR)/test_network
TEST_USB_DEBUG_EXEC = $(TEST_BUILD_DIR)/test_usb_debug
TEST_CONFIG_PERSISTENCE_EXEC = $(TEST_BUILD_DIR)/test_config_persistence
TEST_OTA_UPDATE_EXEC = $(TEST_BUILD_DIR)/test_ota_update

# Integration test executables
TEST_SAFETY_SYSTEM_EXEC = $(TEST_BUILD_DIR)/test_safety_system

# Main application
MAIN_EXEC = $(BUILD_DIR)/oht50_main

# Default target
all: hal_tests integration_tests $(MAIN_EXEC)

# Test targets - Only build existing tests
hal_tests: $(TEST_HAL_COMMON_EXEC) $(TEST_ESTOP_EXEC) $(TEST_RS485_EXEC) $(TEST_RELAY_EXEC) $(TEST_GPIO_EXEC) $(TEST_LED_EXEC) $(TEST_LIDAR_EXEC) $(TEST_NETWORK_EXEC) $(TEST_USB_DEBUG_EXEC) $(TEST_CONFIG_PERSISTENCE_EXEC) $(TEST_OTA_UPDATE_EXEC)

integration_tests: $(TEST_SAFETY_SYSTEM_EXEC)

# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(TEST_BUILD_DIR)
	@mkdir -p $(OBJ_DIR)/01-CORE/01-01-Main
	@mkdir -p $(OBJ_DIR)/01-CORE/01-02-State-Machine
	@mkdir -p $(OBJ_DIR)/01-CORE/01-03-Configuration
	@mkdir -p $(OBJ_DIR)/02-HAL/02-01-Communication
	@mkdir -p $(OBJ_DIR)/02-HAL/02-02-IO-Devices
	@mkdir -p $(OBJ_DIR)/02-HAL/02-03-Sensors
	@mkdir -p $(OBJ_DIR)/02-HAL/02-04-Safety
	@mkdir -p $(OBJ_DIR)/02-HAL/02-05-System
	@mkdir -p $(OBJ_DIR)/03-MODULES/03-01-Motor
	@mkdir -p $(OBJ_DIR)/03-MODULES/03-02-Dock
	@mkdir -p $(OBJ_DIR)/03-MODULES/03-03-Power
	@mkdir -p $(OBJ_DIR)/03-MODULES/03-04-IO
	@mkdir -p $(OBJ_DIR)/03-MODULES/03-05-LED
	@mkdir -p $(OBJ_DIR)/04-SERVICES/04-01-API
	@mkdir -p $(OBJ_DIR)/04-SERVICES/04-02-Communication
	@mkdir -p $(OBJ_DIR)/04-SERVICES/04-03-Safety
	@mkdir -p $(OBJ_DIR)/04-SERVICES/04-04-Security
	@mkdir -p $(OBJ_DIR)/04-SERVICES/04-05-Diagnostics
	@mkdir -p $(OBJ_DIR)/05-CONTROL/05-01-Control-Loop
	@mkdir -p $(OBJ_DIR)/05-CONTROL/05-02-Performance
	@mkdir -p $(OBJ_DIR)/05-CONTROL/05-03-Module-Management
	@mkdir -p $(OBJ_DIR)/06-UTILITIES/06-01-Mocks
	@mkdir -p $(OBJ_DIR)/06-UTILITIES/06-02-Backups
	@mkdir -p $(OBJ_DIR)/tests/utils
	@mkdir -p $(OBJ_DIR)/tests/hal
	@mkdir -p $(OBJ_DIR)/tests/managers
	@mkdir -p $(OBJ_DIR)/tests/core
	@mkdir -p $(OBJ_DIR)/tests/modules
	@mkdir -p $(OBJ_DIR)/tests/integration
	@mkdir -p $(OBJ_DIR)/tests/app

# Build rules for source files
$(OBJ_DIR)/01-CORE/%.o: $(CORE_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/02-HAL/%.o: $(HAL_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/03-MODULES/%.o: $(MODULES_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/04-SERVICES/%.o: $(SERVICES_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/05-CONTROL/%.o: $(CONTROL_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/06-UTILITIES/%.o: $(UTILITIES_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Build test utility objects
$(OBJ_DIR)/tests/utils/%.o: $(TEST_UTILS_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling test utility $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(TEST_UTILS_DIR) -c $< -o $@

# Build test object files by category
$(OBJ_DIR)/tests/hal/%.o: $(HAL_TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling HAL test $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(TEST_UTILS_DIR) -I$(TEST_DIR) -c $< -o $@

$(OBJ_DIR)/tests/integration/%.o: $(INTEGRATION_TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling integration test $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(TEST_UTILS_DIR) -I$(TEST_DIR) -c $< -o $@

$(OBJ_DIR)/tests/managers/%.o: $(MANAGERS_TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling manager test $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(TEST_UTILS_DIR) -I$(TEST_DIR) -c $< -o $@

$(OBJ_DIR)/tests/core/%.o: $(CORE_TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling core test $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(TEST_UTILS_DIR) -I$(TEST_DIR) -c $< -o $@

$(OBJ_DIR)/tests/modules/%.o: $(MODULES_TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling module test $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(TEST_UTILS_DIR) -I$(TEST_DIR) -c $< -o $@

$(OBJ_DIR)/tests/app/%.o: $(APP_TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling app test $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -I$(TEST_UTILS_DIR) -I$(TEST_DIR) -c $< -o $@

# Build test executables - Only for existing tests
$(TEST_HAL_COMMON_EXEC): $(OBJ_DIR)/tests/hal/test_hal_common.o $(TEST_UTILS_OBJECTS) $(ALL_HAL_OBJECTS) | $(BUILD_DIR)
	@echo "Building HAL common test..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(TEST_ESTOP_EXEC): $(OBJ_DIR)/tests/hal/test_estop.o $(TEST_UTILS_OBJECTS) $(ALL_HAL_OBJECTS) | $(BUILD_DIR)
	@echo "Building E-Stop test..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(TEST_RS485_EXEC): $(OBJ_DIR)/tests/hal/test_rs485.o $(TEST_UTILS_OBJECTS) $(ALL_HAL_OBJECTS) | $(BUILD_DIR)
	@echo "Building RS485 test..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(TEST_RELAY_EXEC): $(OBJ_DIR)/tests/hal/test_relay.o $(TEST_UTILS_OBJECTS) $(ALL_HAL_OBJECTS) | $(BUILD_DIR)
	@echo "Building relay test..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(TEST_GPIO_EXEC): $(OBJ_DIR)/tests/hal/test_gpio.o $(TEST_UTILS_OBJECTS) $(ALL_HAL_OBJECTS) | $(BUILD_DIR)
	@echo "Building GPIO test..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(TEST_LED_EXEC): $(OBJ_DIR)/tests/hal/test_led.o $(TEST_UTILS_OBJECTS) $(ALL_HAL_OBJECTS) | $(BUILD_DIR)
	@echo "Building LED test..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(TEST_LIDAR_EXEC): $(OBJ_DIR)/tests/hal/test_lidar.o $(TEST_UTILS_OBJECTS) $(ALL_HAL_OBJECTS) | $(BUILD_DIR)
	@echo "Building LiDAR test..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(TEST_NETWORK_EXEC): $(OBJ_DIR)/tests/hal/test_network.o $(TEST_UTILS_OBJECTS) $(ALL_HAL_OBJECTS) | $(BUILD_DIR)
	@echo "Building network test..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(TEST_USB_DEBUG_EXEC): $(OBJ_DIR)/tests/hal/test_usb_debug.o $(TEST_UTILS_OBJECTS) $(ALL_HAL_OBJECTS) | $(BUILD_DIR)
	@echo "Building USB debug test..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(TEST_CONFIG_PERSISTENCE_EXEC): $(OBJ_DIR)/tests/hal/test_config_persistence.o $(TEST_UTILS_OBJECTS) $(ALL_HAL_OBJECTS) | $(BUILD_DIR)
	@echo "Building config persistence test..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

$(TEST_OTA_UPDATE_EXEC): $(OBJ_DIR)/tests/hal/test_ota_update.o $(TEST_UTILS_OBJECTS) $(ALL_HAL_OBJECTS) | $(BUILD_DIR)
	@echo "Building OTA update test..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Build integration test executables
$(TEST_SAFETY_SYSTEM_EXEC): $(OBJ_DIR)/tests/integration/test_safety_system.o $(TEST_UTILS_OBJECTS) $(ALL_HAL_OBJECTS) | $(BUILD_DIR)
	@echo "Building safety system integration test..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Build main application
$(MAIN_EXEC): $(OBJ_DIR)/01-CORE/01-01-Main/main.o $(ALL_HAL_OBJECTS) $(CORE_OBJECTS) $(MODULES_OBJECTS) $(SERVICES_OBJECTS) $(CONTROL_OBJECTS) $(UTILITIES_OBJECTS) | $(BUILD_DIR)
	@echo "Building main application..."
	$(CC) $(CFLAGS) $^ $(LIBS) -o $@

# Test execution targets
test: hal_tests integration_tests
	@echo "Running HAL unit tests..."
	@for test in $(TEST_HAL_COMMON_EXEC) $(TEST_ESTOP_EXEC) $(TEST_RS485_EXEC) $(TEST_RELAY_EXEC) $(TEST_GPIO_EXEC) $(TEST_LED_EXEC) $(TEST_LIDAR_EXEC) $(TEST_NETWORK_EXEC) $(TEST_USB_DEBUG_EXEC) $(TEST_CONFIG_PERSISTENCE_EXEC) $(TEST_OTA_UPDATE_EXEC); do \
		if [ -f $$test ]; then \
			echo "Running $$(basename $$test)..."; \
			$$test || exit 1; \
		fi; \
	done
	@echo "Running integration tests..."
	@for test in $(TEST_SAFETY_SYSTEM_EXEC); do \
		if [ -f $$test ]; then \
			echo "Running $$(basename $$test)..."; \
			$$test || exit 1; \
		fi; \
	done
	@echo "All tests passed!"

# Individual test targets
test_hal_common: $(TEST_HAL_COMMON_EXEC)
	@echo "Running HAL common test..."
	$(TEST_HAL_COMMON_EXEC)

test_estop: $(TEST_ESTOP_EXEC)
	@echo "Running E-Stop test..."
	$(TEST_ESTOP_EXEC)

test_safety_system: $(TEST_SAFETY_SYSTEM_EXEC)
	@echo "Running safety system integration test..."
	$(TEST_SAFETY_SYSTEM_EXEC)

# Clean targets
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)

clean_tests:
	@echo "Cleaning test files..."
	rm -rf $(TEST_BUILD_DIR)

# Development targets
dev: clean all test

# Production build
release: CFLAGS += -DNDEBUG -O3
release: clean all test

# Debug build
debug: CFLAGS += -DDEBUG -O0 -g3
debug: clean all

# Show build info
info:
	@echo "Build Configuration:"
	@echo "  CC: $(CC)"
	@echo "  CFLAGS: $(CFLAGS)"
	@echo "  LIBS: $(LIBS)"
	@echo "  BUILD_DIR: $(BUILD_DIR)"
	@echo "  TEST_BUILD_DIR: $(TEST_BUILD_DIR)"
	@echo "  HAL_SOURCES: $(words $(HAL_SOURCES)) files"
	@echo "  TEST_SOURCES: $(words $(HAL_TEST_SOURCES)) files"
	@echo "  INTEGRATION_SOURCES: $(words $(INTEGRATION_TEST_SOURCES)) files"
	@echo ""
	@echo "Source files found:"
	@echo "  HAL: $(HAL_SOURCES)"
	@echo "  Tests: $(HAL_TEST_SOURCES)"

# Help
help:
	@echo "Available targets:"
	@echo "  all              - Build all tests and main application"
	@echo "  hal_tests        - Build HAL unit tests"
	@echo "  integration_tests - Build integration tests"
	@echo "  test             - Run all tests"
	@echo "  test_hal_common  - Run HAL common test"
	@echo "  test_estop       - Run E-Stop test"
	@echo "  test_safety_system - Run safety system test"
	@echo "  clean            - Clean all build files"
	@echo "  clean_tests      - Clean test files only"
	@echo "  dev              - Development build and test"
	@echo "  release          - Production build"
	@echo "  debug            - Debug build"
	@echo "  info             - Show build configuration"
	@echo "  help             - Show this help"

.PHONY: all hal_tests integration_tests test test_hal_common test_estop test_safety_system clean clean_tests dev release debug info help
