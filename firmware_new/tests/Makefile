# Makefile for Issue #135 Validation Tests
# FW Team - Testing fixes for RS485/Modbus data invalid issues

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g
INCLUDES = -I../firmware_src -I../include -I../firmware_src/hal/communication -I../firmware_src/hal/common -I../firmware_src/app/validation -I../firmware_src/app/core
LIBS = -lpthread -lrt

# Source files
TEST_SOURCES = issue_135_simple_test.c
HAL_SOURCES = ../firmware_src/hal/communication/hal_rs485.c
VALIDATION_SOURCES = ../firmware_src/app/validation/register_validation.c
SYSTEM_SOURCES = ../firmware_src/app/core/system_state_machine.c

# Object files
TEST_OBJECTS = $(TEST_SOURCES:.c=.o)
HAL_OBJECTS = $(HAL_SOURCES:.c=.o)
VALIDATION_OBJECTS = $(VALIDATION_SOURCES:.c=.o)
SYSTEM_OBJECTS = $(SYSTEM_SOURCES:.c=.o)

# Target executable
TARGET = issue_135_simple_test

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(TEST_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

test: $(TARGET)
	@echo "Running Issue #135 validation tests..."
	@echo "=================================="
	./$(TARGET)

clean:
	rm -f $(TEST_OBJECTS) $(TARGET)
	@echo "Clean completed"

# Help target
help:
	@echo "Available targets:"
	@echo "  all     - Build the test executable"
	@echo "  test    - Build and run the validation tests"
	@echo "  clean   - Remove all generated files"
	@echo "  help    - Show this help message"

# Dependencies
issue_135_simple_test.o: issue_135_simple_test.c
