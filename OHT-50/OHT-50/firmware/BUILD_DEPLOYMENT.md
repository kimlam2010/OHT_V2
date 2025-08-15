# Build & Deployment Guide - OHT-50 Firmware

## Tổng quan
Hướng dẫn build và deployment firmware khi thêm DI/DO hoặc các module khác.

## Build Commands

### 1. Build cơ bản
```bash
# Build tất cả
make all

# Build riêng từng module
make test_sm          # State machine test
make test_safety      # Safety manager test
make loopback         # RS485 loopback test
make test_dio         # DI/DO module test
make test_power       # Power module test
make test_motor       # Motor module test
make test_location    # Location module test
```

### 2. Build khi thêm DI/DO
```bash
# Clean và rebuild tất cả
make rebuild

# Hoặc build riêng DI/DO
make build_dio

# Test DI/DO
./test_dio
```

### 3. Build khi thêm module mới
```bash
# 1. Thêm target vào Makefile
# 2. Clean và rebuild
make clean
make all

# Hoặc build riêng module mới
make test_newmodule
```

## Khi nào cần build lại

### 1. Thêm DI/DO channels
- **Khi**: Thay đổi số lượng DI/DO (từ 8 lên 16)
- **Làm gì**: 
  - Cập nhật `board_config.h` (DI/DO pin mapping)
  - Cập nhật `hal_dio.h/.c` (API functions)
  - Build lại: `make rebuild`

### 2. Thêm module mới
- **Khi**: Thêm module RS485 mới (ví dụ: module 0x08)
- **Làm gì**:
  - Cập nhật `module_map.md` (địa chỉ module)
  - Cập nhật `rs485_opcode_payload_draft.md` (commands)
  - Thêm test file: `tests/test_newmodule.c`
  - Cập nhật Makefile
  - Build: `make rebuild`

### 3. Thay đổi protocol
- **Khi**: Thay đổi RS485 frame format hoặc commands
- **Làm gì**:
  - Cập nhật `hal_rs485.h/.c`
  - Cập nhật protocol documentation
  - Build: `make rebuild`

### 4. Thay đổi pin mapping
- **Khi**: Thay đổi GPIO pin assignment
- **Làm gì**:
  - Cập nhật `board_config.h`
  - Cập nhật `hal_gpio.h/.c`
  - Build: `make rebuild`

## Deployment Process

### 1. Development Environment
```bash
# Build và test trên máy dev
make all
./test_sm && ./test_safety
./test_dio
./test_power
./test_motor
./test_location
```

### 2. Target Hardware (Orange Pi 5B)
```bash
# Copy firmware lên target
scp test_* user@orangepi:/home/user/firmware/

# SSH vào target
ssh user@orangepi

# Test trên hardware
cd /home/user/firmware
./test_dio
./test_power
```

### 3. Production Deployment
```bash
# 1. Build production version
make clean
make all

# 2. Create deployment package
tar -czf oht50_firmware_$(date +%Y%m%d).tar.gz test_* *.md

# 3. Deploy to production
scp oht50_firmware_*.tar.gz user@prod:/opt/oht50/
ssh user@prod "cd /opt/oht50 && tar -xzf oht50_firmware_*.tar.gz"
```

## Configuration Management

### 1. Module Configuration
```bash
# Cập nhật module address
vim docs/specs/module_map.md

# Cập nhật protocol
vim docs/specs/rs485_opcode_payload_draft.md

# Cập nhật board config
vim hal/board_config.h
```

### 2. Environment Variables
```bash
# Set environment variables
export RS485_DEVICE="/dev/ttyS1"
export RS485_BAUD="115200"
export MODULE_ADDRESS="0x05"  # DI/DO module
```

### 3. Runtime Configuration
```bash
# Load configuration
source config/firmware.conf

# Test với config mới
./test_dio --config config/dio_config.json
```

## Testing Strategy

### 1. Unit Tests
```bash
# Test từng module riêng biệt
./test_sm          # State machine
./test_safety      # Safety features
./test_dio         # DI/DO functionality
```

### 2. Integration Tests
```bash
# Test communication giữa các module
./loopback         # RS485 communication
./test_all_modules # Test tất cả module
```

### 3. Hardware Tests
```bash
# Test trên hardware thật
./test_rs485.sh    # Hardware bring-up test
./test_dio --real  # Test DI/DO với hardware
```

## Troubleshooting

### 1. Build Errors
```bash
# Clean và rebuild
make clean
make all

# Check dependencies
ls -la hal/ control/ safety/ tests/
```

### 2. Runtime Errors
```bash
# Check hardware
./test_rs485.sh

# Check module communication
./test_dio --verbose

# Check logs
tail -f /var/log/oht50/firmware.log
```

### 3. Module Not Responding
```bash
# Test module address
./test_dio --addr 0x05

# Test RS485 communication
opwiring uart write 1 --hex "AA 05 01 00 00 00"
opwiring uart read 1 --hex
```

## Best Practices

### 1. Version Control
```bash
# Commit changes trước khi build
git add .
git commit -m "Add new DI/DO channels"

# Tag releases
git tag -a v1.1.0 -m "Add 16 DI/DO channels"
```

### 2. Documentation
```bash
# Cập nhật documentation
vim README.md
vim docs/specs/module_map.md

# Generate API docs
doxygen Doxyfile
```

### 3. Testing
```bash
# Test trước khi deploy
make test_all

# Test trên hardware
./test_hardware.sh

# Performance test
./test_performance.sh
```

## Quick Reference

### Build Commands
```bash
make all          # Build everything
make rebuild      # Clean and rebuild
make build_dio    # Build DI/DO only
make clean        # Clean build artifacts
```

### Test Commands
```bash
./test_dio        # Test DI/DO module
./test_power      # Test power module
./test_motor      # Test motor module
./test_location   # Test location module
./loopback        # Test RS485 communication
```

### Deployment Commands
```bash
make deploy       # Build and deploy
./deploy.sh       # Deploy script
./rollback.sh     # Rollback script
```
