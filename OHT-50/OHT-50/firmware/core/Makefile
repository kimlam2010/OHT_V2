# Makefile cho OHT-50 Firmware Framework
# Version: 1.0
# Platform: Orange Pi 5B (RK3588)

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -g -D_GNU_SOURCE
LDFLAGS = -lpthread -lm -lrt

# Directories
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin
TEST_DIR = tests

# Source files
CORE_SOURCES = $(wildcard $(SRC_DIR)/*.c)
CORE_OBJECTS = $(CORE_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Test sources
TEST_SOURCES = $(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS = $(TEST_SOURCES:$(TEST_DIR)/%.c=$(OBJ_DIR)/%.o)

# Executables
MAIN_EXEC = $(BIN_DIR)/oht50_firmware
TEST_EXEC = $(BIN_DIR)/oht50_tests

# Default target
all: directories $(MAIN_EXEC)

# Create directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Main firmware executable
$(MAIN_EXEC): $(CORE_OBJECTS)
	@echo "Linking $@..."
	$(CC) $(CORE_OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Compile core source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Test target
test: directories $(TEST_EXEC)
	@echo "Running tests..."
	./$(TEST_EXEC)

# Test executable
$(TEST_EXEC): $(TEST_OBJECTS) $(filter-out $(OBJ_DIR)/main.o, $(CORE_OBJECTS))
	@echo "Linking test executable..."
	$(CC) $(TEST_OBJECTS) $(filter-out $(OBJ_DIR)/main.o, $(CORE_OBJECTS)) -o $@ $(LDFLAGS)

# Compile test files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.c
	@echo "Compiling test $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	@echo "Cleaning build files..."
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Clean complete"

# Install target (for development)
install: $(MAIN_EXEC)
	@echo "Installing firmware..."
	sudo cp $(MAIN_EXEC) /usr/local/bin/
	sudo chmod +x /usr/local/bin/oht50_firmware
	@echo "Installation complete"

# Uninstall target
uninstall:
	@echo "Uninstalling firmware..."
	sudo rm -f /usr/local/bin/oht50_firmware
	@echo "Uninstallation complete"

# Debug target
debug: CFLAGS += -DDEBUG -g3 -O0
debug: all

# Release target
release: CFLAGS += -DNDEBUG -O3
release: all

# Run target
run: $(MAIN_EXEC)
	@echo "Running OHT-50 firmware..."
	./$(MAIN_EXEC)

# Valgrind target for memory checking
valgrind: $(MAIN_EXEC)
	@echo "Running with Valgrind..."
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./$(MAIN_EXEC)

# Static analysis with cppcheck
static-analysis:
	@echo "Running static analysis..."
	cppcheck --enable=all --std=c99 $(SRC_DIR)/

# Format code with clang-format
format:
	@echo "Formatting code..."
	clang-format -i $(SRC_DIR)/*.c $(SRC_DIR)/*.h $(TEST_DIR)/*.c

# Documentation target
docs:
	@echo "Generating documentation..."
	doxygen Doxyfile

# Package target
package: clean release
	@echo "Creating package..."
	tar -czf oht50_firmware_v1.0.tar.gz --exclude='.git' --exclude='obj' --exclude='bin' .

# Help target
help:
	@echo "OHT-50 Firmware Framework Makefile"
	@echo "=================================="
	@echo "Available targets:"
	@echo "  all              - Build firmware (default)"
	@echo "  test             - Build and run tests"
	@echo "  clean            - Clean build files"
	@echo "  install          - Install firmware to system"
	@echo "  uninstall        - Remove firmware from system"
	@echo "  debug            - Build with debug symbols"
	@echo "  release          - Build optimized release version"
	@echo "  run              - Build and run firmware"
	@echo "  valgrind         - Run with memory checking"
	@echo "  static-analysis  - Run static code analysis"
	@echo "  format           - Format code with clang-format"
	@echo "  docs             - Generate documentation"
	@echo "  package          - Create distribution package"
	@echo "  help             - Show this help message"

# Dependencies
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/scheduler.h $(SRC_DIR)/control_loop.h $(SRC_DIR)/error_handler.h
$(OBJ_DIR)/scheduler.o: $(SRC_DIR)/scheduler.c $(SRC_DIR)/scheduler.h
$(OBJ_DIR)/control_loop.o: $(SRC_DIR)/control_loop.c $(SRC_DIR)/control_loop.h $(SRC_DIR)/scheduler.h
$(OBJ_DIR)/error_handler.o: $(SRC_DIR)/error_handler.c $(SRC_DIR)/error_handler.h

# Phony targets
.PHONY: all clean install uninstall debug release run valgrind static-analysis format docs package help directories test
