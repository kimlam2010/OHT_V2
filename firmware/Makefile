# Makefile for HAL LiDAR Implementation - OHT-50
# Version: 1.0.0
# Date: 2025-01-27
# Team: EMBED
# Task: EM-12 (LiDAR Driver & USB Integration)

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
INCLUDES = -I./include
LIBS = -lpthread -lm

# Directories
SRC_DIR = src
INCLUDE_DIR = include
TEST_DIR = tests
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Source files
HAL_SOURCES = $(SRC_DIR)/hal/hal_lidar.c $(SRC_DIR)/hal/hal_rs485.c $(SRC_DIR)/hal/hal_gpio.c
TEST_SOURCES = $(TEST_DIR)/test_lidar.c $(TEST_DIR)/test_rs485.c $(TEST_DIR)/test_gpio.c

# Object files
HAL_OBJECTS = $(HAL_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(OBJ_DIR)/tests/%.o)

# Target executables
TEST_LIDAR_EXEC = $(BUILD_DIR)/test_lidar
TEST_RS485_EXEC = $(BUILD_DIR)/test_rs485
TEST_GPIO_EXEC = $(BUILD_DIR)/test_gpio

# Default target
all: $(TEST_LIDAR_EXEC) $(TEST_RS485_EXEC) $(TEST_GPIO_EXEC)

# Create build directories
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)/hal
	mkdir -p $(OBJ_DIR)/tests

# Build HAL library
$(OBJ_DIR)/hal/%.o: $(SRC_DIR)/hal/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Build test objects
$(OBJ_DIR)/tests/%.o: $(TEST_DIR)/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

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

# Test targets
test: test-lidar test-rs485 test-gpio

test-lidar: $(TEST_LIDAR_EXEC)
	@echo "Running LiDAR HAL tests..."
	@echo "=========================================="
	./$(TEST_LIDAR_EXEC)

test-rs485: $(TEST_RS485_EXEC)
	@echo "Running RS485 HAL tests..."
	@echo "=========================================="
	./$(TEST_RS485_EXEC)

test-gpio: $(TEST_GPIO_EXEC)
	@echo "Running GPIO HAL tests..."
	@echo "=========================================="
	./$(TEST_GPIO_EXEC)

# Clean target
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)

# Install target (for development)
install: $(TEST_EXEC)
	@echo "Installing test executable..."
	sudo cp $(TEST_EXEC) /usr/local/bin/
	sudo chmod +x /usr/local/bin/test_lidar

# Uninstall target
uninstall:
	@echo "Uninstalling test executable..."
	sudo rm -f /usr/local/bin/test_lidar

# Help target
help:
	@echo "HAL LiDAR Makefile - OHT-50"
	@echo "============================"
	@echo "Targets:"
	@echo "  all       - Build all targets (default)"
	@echo "  test      - Build and run tests"
	@echo "  clean     - Remove build files"
	@echo "  install   - Install test executable"
	@echo "  uninstall - Remove installed executable"
	@echo "  help      - Show this help message"
	@echo ""
	@echo "Environment:"
	@echo "  CC        - C compiler (default: gcc)"
	@echo "  CFLAGS    - Compiler flags"
	@echo "  INCLUDES  - Include directories"
	@echo "  LIBS      - Linker libraries"

# Phony targets
.PHONY: all test clean install uninstall help

# Dependencies
$(OBJ_DIR)/tests/test_lidar.o: $(INCLUDE_DIR)/hal_lidar.h $(INCLUDE_DIR)/hal_common.h
$(OBJ_DIR)/tests/test_rs485.o: $(INCLUDE_DIR)/hal_rs485.h $(INCLUDE_DIR)/hal_common.h
$(OBJ_DIR)/tests/test_gpio.o: $(INCLUDE_DIR)/hal_gpio.h $(INCLUDE_DIR)/hal_common.h
$(OBJ_DIR)/hal/hal_lidar.o: $(INCLUDE_DIR)/hal_lidar.h $(INCLUDE_DIR)/hal_common.h
$(OBJ_DIR)/hal/hal_rs485.o: $(INCLUDE_DIR)/hal_rs485.h $(INCLUDE_DIR)/hal_common.h
$(OBJ_DIR)/hal/hal_gpio.o: $(INCLUDE_DIR)/hal_gpio.h $(INCLUDE_DIR)/hal_common.h
