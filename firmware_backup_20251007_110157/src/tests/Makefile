# Makefile for Register Info Unit Tests (Issue #179)
# Version: 1.0.0
# Date: 2025-01-28

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g -O2
INCLUDES = -I../../include -I../hal/register -I../app/managers -I../app/modules
LDFLAGS = -lpthread

# Source files
SRCDIR = ../../
REGISTER_SRCS = $(SRCDIR)hal/register/register_info.c
COMM_MGR_SRCS = $(SRCDIR)app/managers/communication_manager.c
MODULE_SRCS = $(SRCDIR)app/modules/power_module_handler.c
TEST_SRCS = test_register_info.c

# Object files
REGISTER_OBJS = register_info.o
COMM_MGR_OBJS = communication_manager.o
MODULE_OBJS = power_module_handler.o
TEST_OBJS = test_register_info.o

# Target executable
TARGET = test_register_info

# Default target
all: $(TARGET)

# Build the test executable
$(TARGET): $(TEST_OBJS) $(REGISTER_OBJS) $(COMM_MGR_OBJS) $(MODULE_OBJS)
	@echo "🔗 Linking $@..."
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	@echo "✅ Build completed: $@"

# Compile register info source
register_info.o: $(REGISTER_SRCS)
	@echo "🔨 Compiling register_info.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile communication manager source
communication_manager.o: $(COMM_MGR_SRCS)
	@echo "🔨 Compiling communication_manager.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile power module handler source
power_module_handler.o: $(MODULE_SRCS)
	@echo "🔨 Compiling power_module_handler.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Compile test source
test_register_info.o: $(TEST_SRCS)
	@echo "🔨 Compiling test_register_info.c..."
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Run tests
test: $(TARGET)
	@echo "🧪 Running Register Info Unit Tests..."
	@echo "======================================"
	./$(TARGET)

# Clean build artifacts
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -f *.o $(TARGET)
	@echo "✅ Clean completed"

# Install dependencies (if needed)
install-deps:
	@echo "📦 Installing dependencies..."
	@echo "✅ Dependencies installed"

# Show help
help:
	@echo "🔧 Register Info Unit Tests Makefile"
	@echo "===================================="
	@echo "Available targets:"
	@echo "  all          - Build the test executable (default)"
	@echo "  test         - Build and run tests"
	@echo "  clean        - Clean build artifacts"
	@echo "  install-deps - Install dependencies"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make         - Build tests"
	@echo "  make test    - Build and run tests"
	@echo "  make clean   - Clean up"

# Phony targets
.PHONY: all test clean install-deps help
