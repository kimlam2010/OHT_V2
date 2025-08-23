# Embedded Development Roadmap – OHT-50 (v2.1)

## Tổng quan
- **Platform**: Orange Pi 5B (RK3588)
- **Tools**: opwiring, gcc, make
- **Goal**: Hardware bring-up + RS485 communication + Plug-and-Play modules
- **Timeline**: 4 phases, 2-3 weeks total

## Phase 1: Hardware Bring-up (Sprint 0) - 1 tuần

### Tasks
- [x] Cài đặt opwiring: `sudo apt install opwiring`
- [x] Test UART1: `opwiring uart enable 1`
- [x] Wiring RS485: UART1 → RS485 transceiver → module
- [x] Test protocol: PING, GET_INFO với module thật
- [ ] **NEW**: Implement basic module discovery
- [ ] **NEW**: Create initial modules.json config

### Success Criteria
- ✅ UART1 hoạt động (115200 baud)
- ✅ RS485 communication thành công
- ✅ PING/GET_INFO response từ module
- **NEW**: Auto-discovery phát hiện được module
- **NEW**: Config file được tạo và load

### Commands
```bash
# Hardware test
./test_rs485.sh

# Module discovery
./module_cli discover

# Initial config
./module_cli init-config
```

## Phase 2: Plug-and-Play System (Sprint 1) - 1 tuần

### Tasks
- [ ] **NEW**: Implement module manager (module_manager.h/.c)
- [ ] **NEW**: Create CLI tool (module_cli)
- [ ] **NEW**: JSON config parser
- [ ] **NEW**: Auto-discovery engine
- [ ] **NEW**: Dynamic module loading
- [ ] **NEW**: Hot-plug support

### Success Criteria
- **NEW**: Thêm module mới chỉ cần edit JSON
- **NEW**: CLI tool hoạt động đầy đủ
- **NEW**: Auto-discovery phát hiện module mới
- **NEW**: Hot-plug thêm/xóa module runtime

### Commands
```bash
# Add new module
./module_cli add --addr 0x08 --name "New DI/DO" --type 4

# List modules
./module_cli list

# Test communication
./module_cli ping --addr 0x08
```

## Phase 3: Module Integration (Sprint 2) - 1 tuần

### Tasks
- [ ] **NEW**: Implement module-specific operations
- [ ] **NEW**: DI/DO module support (0x05)
- [ ] **NEW**: Power module support (0x02)
- [ ] **NEW**: Motor module support (0x03, 0x04)
- [ ] **NEW**: AI module support (0x06)
- [ ] **NEW**: Location module support (0x07)

### Success Criteria
- **NEW**: Tất cả module types được support
- **NEW**: Module-specific commands hoạt động
- **NEW**: Error handling và retry logic
- **NEW**: Performance monitoring

### Commands
```bash
# DI/DO operations
./module_cli dio-read --addr 0x05
./module_cli dio-write --addr 0x05 --channel 1 --value 1

# Power operations
./module_cli power-status --addr 0x02
./module_cli battery-status --addr 0x02

# Motor operations
./module_cli motor-set-position --addr 0x03 --position 1000
./module_cli motor-get-position --addr 0x03
```

## Phase 4: Production Readiness (Sprint 3) - 1 tuần

### Tasks
- [ ] **NEW**: Systemd service integration
- [ ] **NEW**: Configuration management
- [ ] **NEW**: Logging và monitoring
- [ ] **NEW**: Performance optimization
- [ ] **NEW**: Documentation và testing
- [ ] **NEW**: Deployment scripts

### Success Criteria
- **NEW**: Systemd service tự động start
- **NEW**: Config backup/restore
- **NEW**: Comprehensive logging
- **NEW**: Performance metrics
- **NEW**: Complete documentation

### Commands
```bash
# Service management
sudo systemctl enable oht50-firmware
sudo systemctl start oht50-firmware
sudo systemctl status oht50-firmware

# Config management
./module_cli backup
./module_cli restore --file backup.json

# Monitoring
./module_cli health --addr 0x05
./module_cli stats --addr 0x05
```

## Plug-and-Play Architecture

### Core Components
```
┌─────────────────────────────────────┐
│           Module Manager            │
│  ┌─────────────────────────────────┐ │
│  │      Auto-Discovery Engine      │ │
│  └─────────────────────────────────┘ │
│  ┌─────────────────────────────────┐ │
│  │     Configuration Parser        │ │
│  └─────────────────────────────────┘ │
│  ┌─────────────────────────────────┐ │
│  │      Dynamic Loader             │ │
│  └─────────────────────────────────┘ │
└─────────────────────────────────────┘
           │
           ▼
┌─────────────────────────────────────┐
│         RS485 Bus Driver            │
└─────────────────────────────────────┘
           │
           ▼
┌─────────────────────────────────────┐
│        Hardware Modules             │
│  ┌─────┐ ┌─────┐ ┌─────┐ ┌─────┐   │
│  │0x02 │ │0x03 │ │0x04 │ │0x05 │   │
│  │Power│ │Step │ │DC   │ │DI/DO│   │
│  └─────┘ └─────┘ └─────┘ └─────┘   │
└─────────────────────────────────────┘
```

### Configuration Flow
```
1. System Boot
   ↓
2. Load modules.json
   ↓
3. Auto-discover modules
   ↓
4. Register modules
   ↓
5. Start communication
   ↓
6. Monitor health
```

### Hot-Plug Flow
```
1. New module connected
   ↓
2. Auto-discovery detects
   ↓
3. Add to modules.json
   ↓
4. Reload config
   ↓
5. Start communication
   ↓
6. Verify operation
```

## Risk Mitigation

### Technical Risks
- **RS485 communication issues**: Use opwiring for testing
- **Module discovery failures**: Implement retry logic
- **Config file corruption**: Backup/restore mechanism
- **Performance degradation**: Monitor và optimize

### Mitigation Strategies
- **Hardware testing**: Comprehensive test_rs485.sh
- **Error handling**: Graceful degradation
- **Configuration validation**: JSON schema validation
- **Performance monitoring**: Real-time metrics

## Dependencies

### Hardware
- Orange Pi 5B với UART1 enabled
- RS485 transceiver module
- Module hardware (0x02-0x07)

### Software
- opwiring tool
- gcc compiler
- make build system
- JSON parser library

### Documentation
- Platform setup guide
- Wiring diagrams
- Protocol specifications
- CLI tool documentation

## Success Metrics

### Phase 1
- ✅ UART1 communication: 100% success rate
- ✅ RS485 protocol: PING/GET_INFO working
- **NEW**: Module discovery: >90% success rate

### Phase 2
- **NEW**: Config-driven module loading: 100%
- **NEW**: CLI tool functionality: All commands working
- **NEW**: Hot-plug support: Add/remove modules runtime

### Phase 3
- **NEW**: Module-specific operations: All types supported
- **NEW**: Error handling: Graceful degradation
- **NEW**: Performance: <100ms response time

### Phase 4
- **NEW**: Production deployment: Systemd service
- **NEW**: Monitoring: Real-time health checks
- **NEW**: Documentation: Complete user guide

## Reference Commands

### Hardware Testing
```bash
# Check opwiring installation
which opwiring

# Test UART1
opwiring uart enable 1
opwiring uart status 1

# Test RS485 communication
./test_rs485.sh
```

### Module Management
```bash
# Initialize system
./module_cli init

# Discover modules
./module_cli discover

# List modules
./module_cli list

# Add module
./module_cli add --addr 0x08 --name "New Module" --type 4

# Test module
./module_cli ping --addr 0x08
```

### Configuration
```bash
# Backup config
./module_cli backup

# Restore config
./module_cli restore --file backup.json

# Validate config
./module_cli validate

# Reload config
./module_cli reload
```

### Monitoring
```bash
# Health check
./module_cli health --addr 0x05

# Performance stats
./module_cli stats --addr 0x05

# Error logs
./module_cli errors --addr 0x05
```

## Next Steps

1. **Immediate**: Complete hardware bring-up với opwiring
2. **Short-term**: Implement plug-and-play system
3. **Medium-term**: Module-specific operations
4. **Long-term**: Production deployment và monitoring

Với plug-and-play system, việc thêm module mới sẽ trở nên **đơn giản** và **không cần sửa source code**!
