# Plug-and-Play Module System - OHT-50

## Tổng quan
Hệ thống **plug-and-play** cho phép thêm module mới mà **KHÔNG CẦN** sửa source code, chỉ cần cấu hình file JSON.

## Cách hoạt động

### 1. Auto-Discovery
- Hệ thống tự động quét RS485 bus (địa chỉ 0x02-0x07)
- Phát hiện module mới khi khởi động
- Tự động đăng ký module với default operations

### 2. Configuration-Driven
- Module config lưu trong `/etc/oht50/modules.json`
- Thêm/sửa/xóa module chỉ cần edit file JSON
- Không cần rebuild firmware

### 3. Dynamic Loading
- Module được load runtime từ config file
- Có thể enable/disable module mà không restart
- Hot-plug support (thêm module khi đang chạy)

## Cách thêm module mới

### Bước 1: Kết nối hardware
```bash
# Kết nối module vào RS485 bus
# Ví dụ: thêm module DI/DO mới vào địa chỉ 0x08
```

### Bước 2: Cấu hình JSON
```bash
# Edit file config
sudo nano /etc/oht50/modules.json
```

Thêm module mới vào JSON:
```json
{
  "modules": [
    {
      "address": "0x08",
      "name": "New DI/DO Module",
      "type": 4,
      "enabled": true,
      "timeout_ms": 1000,
      "retry_count": 3
    }
  ]
}
```

### Bước 3: Reload config
```bash
# Reload configuration (không cần restart)
sudo systemctl reload oht50-firmware

# Hoặc dùng CLI tool
./module_cli reload
```

### Bước 4: Verify
```bash
# Kiểm tra module đã được nhận
./module_cli list

# Test communication
./module_cli ping 0x08
```

## Module Types

| Type | Name | Address Range | Description |
|------|------|---------------|-------------|
| 1 | Power | 0x02 | Quản lý pin, charger |
| 2 | Stepper | 0x03 | Động cơ stepper |
| 3 | DC Motor | 0x04 | Động cơ DC cargo |
| 4 | DI/DO | 0x05 | Digital I/O |
| 5 | AI | 0x06 | Analog Input |
| 6 | Location | 0x07 | RFID, encoder, Lidar |
| 7+ | Custom | 0x08+ | Module tùy chỉnh |

## Configuration File Format

### File: `/etc/oht50/modules.json`
```json
{
  "modules": [
    {
      "address": "0x02",
      "name": "Power Module",
      "type": 1,
      "version": 1,
      "enabled": true,
      "timeout_ms": 1000,
      "retry_count": 3,
      "custom_config": {
        "battery_capacity": 100,
        "charge_current": 2.5
      }
    },
    {
      "address": "0x05",
      "name": "DI/DO Module",
      "type": 4,
      "version": 1,
      "enabled": true,
      "timeout_ms": 500,
      "retry_count": 5,
      "custom_config": {
        "di_count": 8,
        "do_count": 8,
        "invert_logic": false
      }
    }
  ],
  "global_config": {
    "baud_rate": 115200,
    "parity": "none",
    "stop_bits": 1,
    "auto_discovery": true,
    "discovery_interval": 30
  }
}
```

## CLI Commands

### Module Management
```bash
# Liệt kê tất cả module
./module_cli list

# Thêm module mới
./module_cli add --addr 0x08 --name "New Module" --type 4

# Xóa module
./module_cli remove --addr 0x08

# Enable/disable module
./module_cli enable --addr 0x08
./module_cli disable --addr 0x08

# Ping module
./module_cli ping --addr 0x08

# Ping tất cả module
./module_cli ping-all
```

### Configuration
```bash
# Reload config
./module_cli reload

# Backup config
./module_cli backup

# Restore config
./module_cli restore --file backup.json

# Validate config
./module_cli validate
```

### Discovery
```bash
# Auto-discover modules
./module_cli discover

# Scan specific address range
./module_cli scan --start 0x02 --end 0x0F

# Test communication
./module_cli test --addr 0x08
```

## Khi nào cần build lại

### KHÔNG cần build lại khi:
- ✅ Thêm module mới (địa chỉ 0x08+)
- ✅ Thay đổi timeout, retry count
- ✅ Enable/disable module
- ✅ Thay đổi tên module
- ✅ Thêm custom config

### CẦN build lại khi:
- ❌ Thay đổi RS485 protocol
- ❌ Thay đổi pin mapping
- ❌ Thêm module type mới
- ❌ Thay đổi HAL layer

## Workflow thêm module

### 1. Hardware Setup
```bash
# 1. Kết nối module vào RS485 bus
# 2. Set địa chỉ module (dip switch hoặc config)
# 3. Cấp nguồn cho module
```

### 2. Software Configuration
```bash
# 1. Edit modules.json
sudo nano /etc/oht50/modules.json

# 2. Reload config
sudo systemctl reload oht50-firmware

# 3. Verify
./module_cli list
./module_cli ping --addr 0x08
```

### 3. Testing
```bash
# 1. Test basic communication
./module_cli test --addr 0x08

# 2. Test specific functions
./module_cli dio-read --addr 0x08
./module_cli dio-write --addr 0x08 --channel 1 --value 1

# 3. Monitor logs
tail -f /var/log/oht50/firmware.log
```

## Troubleshooting

### Module không respond
```bash
# 1. Check hardware connection
./test_rs485.sh

# 2. Check module address
./module_cli scan --start 0x02 --end 0x0F

# 3. Check config
./module_cli validate

# 4. Check logs
tail -f /var/log/oht50/firmware.log
```

### Config error
```bash
# 1. Validate JSON syntax
./module_cli validate

# 2. Restore backup
./module_cli restore --file backup.json

# 3. Reset to defaults
./module_cli reset-config
```

### Performance issues
```bash
# 1. Check timeout settings
./module_cli list --verbose

# 2. Adjust retry count
./module_cli config --addr 0x08 --retry 5

# 3. Monitor communication
./module_cli monitor --addr 0x08
```

## Best Practices

### 1. Configuration Management
```bash
# Backup config trước khi thay đổi
./module_cli backup

# Version control cho config
git add /etc/oht50/modules.json
git commit -m "Add new DI/DO module 0x08"
```

### 2. Testing
```bash
# Test trên development trước
./module_cli test --addr 0x08 --verbose

# Test integration
./module_cli integration-test

# Performance test
./module_cli perf-test --addr 0x08
```

### 3. Monitoring
```bash
# Monitor module health
./module_cli health --addr 0x08

# Monitor communication errors
./module_cli errors --addr 0x08

# Monitor performance
./module_cli stats --addr 0x08
```

## Examples

### Thêm DI/DO module mới
```bash
# 1. Edit config
sudo nano /etc/oht50/modules.json
# Thêm:
# {
#   "address": "0x08",
#   "name": "Extra DI/DO",
#   "type": 4,
#   "enabled": true
# }

# 2. Reload
sudo systemctl reload oht50-firmware

# 3. Test
./module_cli ping --addr 0x08
./module_cli dio-read --addr 0x08
```

### Thêm Power module backup
```bash
# 1. Edit config
sudo nano /etc/oht50/modules.json
# Thêm:
# {
#   "address": "0x09",
#   "name": "Backup Power",
#   "type": 1,
#   "enabled": true
# }

# 2. Reload và test
sudo systemctl reload oht50-firmware
./module_cli ping --addr 0x09
```

## Summary

Với hệ thống plug-and-play này:
- ✅ **Không cần sửa source code** khi thêm module
- ✅ **Chỉ cần edit file JSON** để cấu hình
- ✅ **Auto-discovery** module mới
- ✅ **Hot-plug** support
- ✅ **Configuration-driven** architecture
- ✅ **Easy troubleshooting** với CLI tools

Đây là cách **modern** và **scalable** để quản lý module trong hệ thống OHT-50!
