# Master Module Firmware Makefile - Updated for new test structure
# EMBED Team - OHT-50 Project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_POSIX_C_SOURCE=200809L -O2 -g
LIBS = -lpthread

# Directories
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Test directories
TEST_DIR = tests
HAL_TEST_DIR = $(TEST_DIR)/hal
MANAGERS_TEST_DIR = $(TEST_DIR)/managers
CORE_TEST_DIR = $(TEST_DIR)/core
MODULES_TEST_DIR = $(TEST_DIR)/modules
INTEGRATION_TEST_DIR = $(TEST_DIR)/integration
APP_TEST_DIR = $(TEST_DIR)/app

# Source files
HAL_SOURCES = $(wildcard $(SRC_DIR)/hal/*.c)
APP_SOURCES = $(wildcard $(SRC_DIR)/app/*.c)

# Test sources by category
HAL_TEST_SOURCES = $(wildcard $(HAL_TEST_DIR)/*.c)
MANAGERS_TEST_SOURCES = $(wildcard $(MANAGERS_TEST_DIR)/*.c)
CORE_TEST_SOURCES = $(wildcard $(CORE_TEST_DIR)/*.c)
MODULES_TEST_SOURCES = $(wildcard $(MODULES_TEST_DIR)/*.c)
INTEGRATION_TEST_SOURCES = $(wildcard $(INTEGRATION_TEST_DIR)/*.c)
APP_TEST_SOURCES = $(wildcard $(APP_TEST_DIR)/*.c)

# Object files
HAL_OBJECTS = $(HAL_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
APP_OBJECTS = $(APP_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Test object files with proper paths
HAL_TEST_OBJECTS = $(HAL_TEST_SOURCES:$(HAL_TEST_DIR)/%.c=$(OBJ_DIR)/tests/hal/%.o)
MANAGERS_TEST_OBJECTS = $(MANAGERS_TEST_SOURCES:$(MANAGERS_TEST_DIR)/%.c=$(OBJ_DIR)/tests/managers/%.o)
CORE_TEST_OBJECTS = $(CORE_TEST_SOURCES:$(CORE_TEST_DIR)/%.c=$(OBJ_DIR)/tests/core/%.o)
MODULES_TEST_OBJECTS = $(MODULES_TEST_SOURCES:$(MODULES_TEST_DIR)/%.c=$(OBJ_DIR)/tests/modules/%.o)
INTEGRATION_TEST_OBJECTS = $(INTEGRATION_TEST_SOURCES:$(INTEGRATION_TEST_DIR)/%.c=$(OBJ_DIR)/tests/integration/%.o)
APP_TEST_OBJECTS = $(APP_TEST_SOURCES:$(APP_TEST_DIR)/%.c=$(OBJ_DIR)/tests/app/%.o)

# Target executables
TEST_LIDAR_EXEC = $(BUILD_DIR)/test_lidar
TEST_RS485_EXEC = $(BUILD_DIR)/test_rs485
TEST_GPIO_EXEC = $(BUILD_DIR)/test_gpio
TEST_LED_EXEC = $(BUILD_DIR)/test_led
TEST_ESTOP_EXEC = $(BUILD_DIR)/test_estop
TEST_RELAY_EXEC = $(BUILD_DIR)/test_relay
TEST_NETWORK_EXEC = $(BUILD_DIR)/test_network
TEST_USB_DEBUG_EXEC = $(BUILD_DIR)/test_usb_debug
TEST_CONFIG_PERSISTENCE_EXEC = $(BUILD_DIR)/test_config_persistence
TEST_OTA_UPDATE_EXEC = $(BUILD_DIR)/test_ota_update
TEST_SECURITY_MANAGER_EXEC = $(BUILD_DIR)/test_security_manager
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
TEST_MOTOR_MODULE_EXEC = $(BUILD_DIR)/test_motor_module
TEST_MOTOR_SIMPLE_EXEC = $(BUILD_DIR)/test_motor_simple
TEST_IO_MODULE_EXEC = $(BUILD_DIR)/test_io_module
TEST_DOCK_MODULE_EXEC = $(BUILD_DIR)/test_dock_module
TEST_DI_DO_MODULE_EXEC = $(BUILD_DIR)/test_di_do_module
TEST_HTTP_SERVER_EXEC = $(BUILD_DIR)/test_http_server
TEST_API_ENDPOINTS_EXEC = $(BUILD_DIR)/test_api_endpoints
TEST_WEBSOCKET_SERVER_EXEC = $(BUILD_DIR)/test_websocket_server

# Main application
MAIN_EXEC = $(BUILD_DIR)/oht50_main

# Tools
GPIO_VERIFY_TOOL = $(BUILD_DIR)/gpio_pin_verify

# Default target
all: $(TEST_LIDAR_EXEC) $(TEST_RS485_EXEC) $(TEST_GPIO_EXEC) $(TEST_LED_EXEC) $(TEST_ESTOP_EXEC) $(TEST_RELAY_EXEC) $(TEST_NETWORK_EXEC) $(TEST_USB_DEBUG_EXEC) $(TEST_CONFIG_PERSISTENCE_EXEC) $(TEST_OTA_UPDATE_EXEC) $(TEST_SECURITY_MANAGER_EXEC) $(TEST_SYSTEM_STATE_MACHINE_EXEC) $(TEST_SAFETY_MANAGER_EXEC) $(TEST_LED_MANAGER_EXEC) $(TEST_COMMUNICATION_MANAGER_EXEC) $(TEST_MODULE_MANAGER_EXEC) $(TEST_NETWORK_MANAGER_EXEC) $(TEST_MODULE_REGISTRY_EXEC) $(TEST_POWER_MODULE_EXEC) $(TEST_CONFIG_SYSTEM_EXEC) $(TEST_INTERLOCK_EXEC) $(TEST_SUITE_EXEC) $(TEST_MOTOR_MODULE_EXEC) $(TEST_MOTOR_SIMPLE_EXEC) $(TEST_IO_MODULE_EXEC) $(TEST_DOCK_MODULE_EXEC) $(TEST_DI_DO_MODULE_EXEC) $(TEST_HTTP_SERVER_EXEC) $(TEST_API_ENDPOINTS_EXEC) $(TEST_WEBSOCKET_SERVER_EXEC) $(MAIN_EXEC)

# Tools target
tools: $(GPIO_VERIFY_TOOL)

# Create build directories
$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(OBJ_DIR)/hal
	@mkdir -p $(OBJ_DIR)/app
	@mkdir -p $(OBJ_DIR)/tests/hal
	@mkdir -p $(OBJ_DIR)/tests/managers
	@mkdir -p $(OBJ_DIR)/tests/core
	@mkdir -p $(OBJ_DIR)/tests/modules
	@mkdir -p $(OBJ_DIR)/tests/integration
	@mkdir -p $(OBJ_DIR)/tests/app

# Build HAL object files
$(OBJ_DIR)/hal/%.o: $(SRC_DIR)/hal/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Build APP object files
$(OBJ_DIR)/app/%.o: $(SRC_DIR)/app/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Build main application object
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Build test object files by category
$(OBJ_DIR)/tests/hal/%.o: $(HAL_TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/tests/managers/%.o: $(MANAGERS_TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/tests/core/%.o: $(CORE_TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/tests/modules/%.o: $(MODULES_TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/tests/integration/%.o: $(INTEGRATION_TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

$(OBJ_DIR)/tests/app/%.o: $(APP_TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Build test executables
$(TEST_LIDAR_EXEC): $(OBJ_DIR)/hal/hal_lidar.o $(OBJ_DIR)/tests/hal/test_lidar.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_RS485_EXEC): $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/tests/hal/test_rs485.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_GPIO_EXEC): $(OBJ_DIR)/hal/hal_gpio.o $(OBJ_DIR)/tests/hal/test_gpio.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_LED_EXEC): $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/hal/test_led.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_ESTOP_EXEC): $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/tests/hal/test_estop.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_RELAY_EXEC): $(OBJ_DIR)/hal/hal_relay.o $(OBJ_DIR)/tests/hal/test_relay.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_NETWORK_EXEC): $(OBJ_DIR)/hal/hal_network.o $(OBJ_DIR)/tests/hal/test_network.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_USB_DEBUG_EXEC): $(OBJ_DIR)/hal/hal_usb_debug.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/tests/hal/test_usb_debug.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_CONFIG_PERSISTENCE_EXEC): $(OBJ_DIR)/hal/hal_config_persistence.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/tests/hal/test_config_persistence.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_OTA_UPDATE_EXEC): $(OBJ_DIR)/hal/hal_ota_update.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/tests/hal/test_ota_update.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_SECURITY_MANAGER_EXEC): $(OBJ_DIR)/app/security_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/tests/managers/test_security_manager.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_SYSTEM_STATE_MACHINE_EXEC): $(OBJ_DIR)/app/system_state_machine.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/core/test_system_state_machine.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_SAFETY_MANAGER_EXEC): $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/managers/test_safety_manager.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_LED_MANAGER_EXEC): $(OBJ_DIR)/app/led_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/managers/test_led_manager.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_COMMUNICATION_MANAGER_EXEC): $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/tests/managers/test_communication_manager.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_MODULE_MANAGER_EXEC): $(OBJ_DIR)/app/module_manager.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/tests/managers/test_module_manager.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_NETWORK_MANAGER_EXEC): $(OBJ_DIR)/app/network_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_network.o $(OBJ_DIR)/tests/managers/test_network_manager.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_MODULE_REGISTRY_EXEC): $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/tests/modules/test_module_registry.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_POWER_MODULE_EXEC): $(OBJ_DIR)/app/power_module_handler.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/modules/test_power_module.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_CONFIG_SYSTEM_EXEC): $(OBJ_DIR)/app/config_system.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/tests/core/test_config_system.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_INTERLOCK_EXEC): $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/integration/test_interlock.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_SUITE_EXEC): $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/app/system_state_machine.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/app/config_system.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/hal/hal_relay.o $(OBJ_DIR)/tests/integration/test_suite.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_MOTOR_MODULE_EXEC): $(OBJ_DIR)/app/motor_module_handler.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/modules/test_motor_module.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_MOTOR_SIMPLE_EXEC): $(OBJ_DIR)/app/motor_module_handler.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/modules/test_motor_simple.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_IO_MODULE_EXEC): $(OBJ_DIR)/app/io_module_handler.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/modules/test_io_module.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_DOCK_MODULE_EXEC): $(OBJ_DIR)/app/dock_module_handler.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/modules/test_dock_module.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_DI_DO_MODULE_EXEC): $(OBJ_DIR)/app/di_do_module_handler.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_rs485.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/tests/modules/test_di_do_module.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(TEST_HTTP_SERVER_EXEC): $(OBJ_DIR)/app/http_server.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/tests/app/test_http_server.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) -lpthread

$(TEST_API_ENDPOINTS_EXEC): $(OBJ_DIR)/app/api_endpoints.o $(OBJ_DIR)/app/http_server.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/tests/app/test_api_endpoints.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) -lpthread

$(TEST_WEBSOCKET_SERVER_EXEC): $(OBJ_DIR)/app/websocket_server.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/tests/app/test_websocket_server.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS) -lssl -lcrypto

# Link main application
$(MAIN_EXEC): $(OBJ_DIR)/main.o $(OBJ_DIR)/app/system_state_machine.o $(OBJ_DIR)/app/safety_manager.o $(OBJ_DIR)/app/communication_manager.o $(OBJ_DIR)/app/module_registry.o $(OBJ_DIR)/app/power_module_handler.o $(OBJ_DIR)/app/api_manager.o $(OBJ_DIR)/app/http_server.o $(OBJ_DIR)/app/api_endpoints.o $(OBJ_DIR)/hal/hal_common.o $(OBJ_DIR)/hal/hal_led.o $(OBJ_DIR)/hal/hal_estop.o $(OBJ_DIR)/hal/hal_rs485.o
	@echo "Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

# Build GPIO verification tool
$(GPIO_VERIFY_TOOL): tools/gpio_pin_verify.c | $(BUILD_DIR)
	@echo "Building GPIO Pin Verification Tool..."
	$(CC) $(CFLAGS) -D_POSIX_C_SOURCE=200809L $< -o $@

# Clean target
clean:
	@echo "Cleaning build directory..."
	rm -rf $(BUILD_DIR)

# Test targets
test: test-hal test-managers test-core test-modules test-integration test-app

test-hal: $(TEST_LIDAR_EXEC) $(TEST_RS485_EXEC) $(TEST_GPIO_EXEC) $(TEST_LED_EXEC) $(TEST_ESTOP_EXEC) $(TEST_RELAY_EXEC) $(TEST_NETWORK_EXEC)
	@echo "Running HAL tests..."

test-managers: $(TEST_SAFETY_MANAGER_EXEC) $(TEST_LED_MANAGER_EXEC) $(TEST_COMMUNICATION_MANAGER_EXEC) $(TEST_MODULE_MANAGER_EXEC) $(TEST_NETWORK_MANAGER_EXEC)
	@echo "Running Manager tests..."

test-core: $(TEST_SYSTEM_STATE_MACHINE_EXEC) $(TEST_CONFIG_SYSTEM_EXEC)
	@echo "Running Core tests..."

test-modules: $(TEST_MODULE_REGISTRY_EXEC) $(TEST_POWER_MODULE_EXEC) $(TEST_MOTOR_MODULE_EXEC) $(TEST_DOCK_MODULE_EXEC) $(TEST_DI_DO_MODULE_EXEC)
	@echo "Running Module tests..."

test-integration: $(TEST_INTERLOCK_EXEC) $(TEST_SUITE_EXEC)
	@echo "Running Integration tests..."

test-app: $(TEST_HTTP_SERVER_EXEC) $(TEST_API_ENDPOINTS_EXEC) $(TEST_WEBSOCKET_SERVER_EXEC)
	@echo "Running App tests..."

.PHONY: all clean test test-hal test-managers test-core test-modules test-integration test-app tools
