# Plug-and-Play Module Tasks - OHT-50

## Tổng quan
Task list cụ thể để implement hệ thống plug-and-play cho module management.

## Sprint 1: Core Infrastructure (1 tuần)

### Task 1.1: Module Manager Core
**Priority**: High  
**Effort**: 2 days  
**Dependencies**: None

#### Subtasks
- [ ] Create `hal/module_manager.h` (interface)
- [ ] Create `hal/module_manager.c` (implementation)
- [ ] Implement module registry (static array)
- [ ] Implement basic CRUD operations
- [ ] Add unit tests

#### Success Criteria
- Module registration/unregistration works
- Module lookup by address works
- Memory management is safe
- Unit tests pass

#### Commands
```bash
# Build module manager
make test_module_manager

# Run tests
./test_module_manager
```

### Task 1.2: Configuration Parser
**Priority**: High  
**Effort**: 2 days  
**Dependencies**: Task 1.1

#### Subtasks
- [ ] Implement JSON parser (simple)
- [ ] Create config file format spec
- [ ] Implement load/save functions
- [ ] Add config validation
- [ ] Add error handling

#### Success Criteria
- JSON config file can be loaded
- Config can be saved to file
- Invalid config is rejected
- Error messages are clear

#### Commands
```bash
# Test config parser
./test_config_parser

# Validate config
./module_cli validate
```

### Task 1.3: Auto-Discovery Engine
**Priority**: Medium  
**Effort**: 1 day  
**Dependencies**: Task 1.1, Task 1.2

#### Subtasks
- [ ] Implement RS485 bus scanning
- [ ] Add PING command for discovery
- [ ] Implement GET_INFO parsing
- [ ] Add auto-registration logic
- [ ] Add discovery logging

#### Success Criteria
- Can scan address range 0x02-0x07
- Detects responding modules
- Auto-registers discovered modules
- Logs discovery results

#### Commands
```bash
# Run discovery
./module_cli discover

# Check discovered modules
./module_cli list
```

### Task 1.4: CLI Tool Foundation
**Priority**: Medium  
**Effort**: 2 days  
**Dependencies**: Task 1.1, Task 1.2, Task 1.3

#### Subtasks
- [ ] Create CLI framework
- [ ] Implement basic commands (list, ping, add, remove)
- [ ] Add help system
- [ ] Add error handling
- [ ] Add logging

#### Success Criteria
- CLI tool compiles and runs
- Basic commands work
- Help system is comprehensive
- Error handling is robust

#### Commands
```bash
# Build CLI tool
make module_cli

# Test CLI
./module_cli --help
./module_cli list
```

## Sprint 2: Module Operations (1 tuần)

### Task 2.1: DI/DO Module Support
**Priority**: High  
**Effort**: 2 days  
**Dependencies**: Sprint 1 complete

#### Subtasks
- [ ] Implement DI/DO operations structure
- [ ] Add READ_DI command (0x40)
- [ ] Add WRITE_DO command (0x41)
- [ ] Add GET_DI_STATUS command (0x42)
- [ ] Add GET_DO_STATUS command (0x43)
- [ ] Add CLI commands for DI/DO

#### Success Criteria
- Can read DI channels
- Can write DO channels
- Can get DI/DO status
- CLI commands work for DI/DO

#### Commands
```bash
# DI/DO operations
./module_cli dio-read --addr 0x05
./module_cli dio-write --addr 0x05 --channel 1 --value 1
./module_cli dio-status --addr 0x05
```

### Task 2.2: Power Module Support
**Priority**: High  
**Effort**: 1 day  
**Dependencies**: Sprint 1 complete

#### Subtasks
- [ ] Implement power operations structure
- [ ] Add GET_BATTERY_STATUS command (0x10)
- [ ] Add GET_CHARGER_STATUS command (0x11)
- [ ] Add GET_POWER_STATUS command (0x13)
- [ ] Add CLI commands for power

#### Success Criteria
- Can read battery status
- Can read charger status
- Can read power status
- CLI commands work for power

#### Commands
```bash
# Power operations
./module_cli power-battery --addr 0x02
./module_cli power-charger --addr 0x02
./module_cli power-status --addr 0x02
```

### Task 2.3: Motor Module Support
**Priority**: Medium  
**Effort**: 2 days  
**Dependencies**: Sprint 1 complete

#### Subtasks
- [ ] Implement motor operations structure
- [ ] Add SET_POSITION command (0x20)
- [ ] Add SET_VELOCITY command (0x21)
- [ ] Add GET_POSITION command (0x22)
- [ ] Add ENABLE_MOTOR command (0x23)
- [ ] Add STOP_MOTOR command (0x24)
- [ ] Add CLI commands for motors

#### Success Criteria
- Can set motor position
- Can set motor velocity
- Can read motor position
- Can enable/disable motors
- CLI commands work for motors

#### Commands
```bash
# Motor operations
./module_cli motor-set-position --addr 0x03 --position 1000
./module_cli motor-set-velocity --addr 0x03 --velocity 100
./module_cli motor-get-position --addr 0x03
./module_cli motor-enable --addr 0x03
./module_cli motor-stop --addr 0x03
```

### Task 2.4: AI Module Support
**Priority**: Low  
**Effort**: 1 day  
**Dependencies**: Sprint 1 complete

#### Subtasks
- [ ] Implement AI operations structure
- [ ] Add READ_AI command (0x50)
- [ ] Add GET_DISTANCE command (0x51)
- [ ] Add CLI commands for AI

#### Success Criteria
- Can read analog inputs
- Can read distance sensors
- CLI commands work for AI

#### Commands
```bash
# AI operations
./module_cli ai-read --addr 0x06 --channel 1
./module_cli ai-distance --addr 0x06 --sensor 1
```

## Sprint 3: Advanced Features (1 tuần)

### Task 3.1: Hot-Plug Support
**Priority**: Medium  
**Effort**: 2 days  
**Dependencies**: Sprint 2 complete

#### Subtasks
- [ ] Implement module monitoring thread
- [ ] Add module health checking
- [ ] Add automatic reconnection
- [ ] Add hot-plug event handling
- [ ] Add CLI commands for hot-plug

#### Success Criteria
- Can detect module disconnection
- Can detect module reconnection
- Automatically reconnects to modules
- CLI shows hot-plug events

#### Commands
```bash
# Hot-plug monitoring
./module_cli monitor --addr 0x05
./module_cli health --addr 0x05
./module_cli events
```

### Task 3.2: Performance Monitoring
**Priority**: Low  
**Effort**: 1 day  
**Dependencies**: Sprint 2 complete

#### Subtasks
- [ ] Implement performance metrics
- [ ] Add response time measurement
- [ ] Add error rate tracking
- [ ] Add throughput measurement
- [ ] Add CLI commands for monitoring

#### Success Criteria
- Can measure response times
- Can track error rates
- Can measure throughput
- CLI shows performance stats

#### Commands
```bash
# Performance monitoring
./module_cli stats --addr 0x05
./module_cli perf-test --addr 0x05
./module_cli errors --addr 0x05
```

### Task 3.3: Configuration Management
**Priority**: Medium  
**Effort**: 2 days  
**Dependencies**: Sprint 1 complete

#### Subtasks
- [ ] Implement config backup/restore
- [ ] Add config validation
- [ ] Add config versioning
- [ ] Add config migration
- [ ] Add CLI commands for config

#### Success Criteria
- Can backup configuration
- Can restore configuration
- Can validate configuration
- Can migrate old configs
- CLI commands work for config

#### Commands
```bash
# Configuration management
./module_cli backup
./module_cli restore --file backup.json
./module_cli validate
./module_cli migrate
```

## Sprint 4: Production Readiness (1 tuần)

### Task 4.1: Systemd Integration
**Priority**: High  
**Effort**: 1 day  
**Dependencies**: Sprint 3 complete

#### Subtasks
- [ ] Create systemd service file
- [ ] Add service dependencies
- [ ] Add service configuration
- [ ] Add service logging
- [ ] Test service startup/shutdown

#### Success Criteria
- Service starts automatically
- Service stops gracefully
- Service logs properly
- Service restarts on failure

#### Commands
```bash
# Service management
sudo systemctl enable oht50-firmware
sudo systemctl start oht50-firmware
sudo systemctl status oht50-firmware
```

### Task 4.2: Logging System
**Priority**: Medium  
**Effort**: 1 day  
**Dependencies**: Sprint 3 complete

#### Subtasks
- [ ] Implement structured logging
- [ ] Add log levels (DEBUG, INFO, WARN, ERROR)
- [ ] Add log rotation
- [ ] Add log filtering
- [ ] Add CLI commands for logs

#### Success Criteria
- Logs are structured and readable
- Log levels work correctly
- Log rotation works
- CLI can show logs

#### Commands
```bash
# Logging
./module_cli logs --addr 0x05
./module_cli logs --level ERROR
./module_cli logs --follow
```

### Task 4.3: Documentation
**Priority**: Medium  
**Effort**: 1 day  
**Dependencies**: Sprint 3 complete

#### Subtasks
- [ ] Write user manual
- [ ] Write API documentation
- [ ] Write troubleshooting guide
- [ ] Write examples
- [ ] Create diagrams

#### Success Criteria
- Documentation is complete
- Examples work correctly
- Troubleshooting guide is helpful
- Diagrams are clear

#### Files
- `docs/PLUG_AND_PLAY_MODULES.md`
- `docs/CLI_REFERENCE.md`
- `docs/TROUBLESHOOTING.md`
- `docs/EXAMPLES.md`

### Task 4.4: Testing & Validation
**Priority**: High  
**Effort**: 2 days  
**Dependencies**: Sprint 3 complete

#### Subtasks
- [ ] Write integration tests
- [ ] Write performance tests
- [ ] Write stress tests
- [ ] Write regression tests
- [ ] Validate with real hardware

#### Success Criteria
- All tests pass
- Performance meets requirements
- Stress tests don't crash
- Works with real hardware

#### Commands
```bash
# Testing
make test-all
./test_integration
./test_performance
./test_stress
```

## Dependencies Graph

```
Task 1.1 (Module Manager Core)
    ↓
Task 1.2 (Configuration Parser)
    ↓
Task 1.3 (Auto-Discovery Engine)
    ↓
Task 1.4 (CLI Tool Foundation)
    ↓
Task 2.1 (DI/DO Module Support)
Task 2.2 (Power Module Support)
Task 2.3 (Motor Module Support)
Task 2.4 (AI Module Support)
    ↓
Task 3.1 (Hot-Plug Support)
Task 3.2 (Performance Monitoring)
Task 3.3 (Configuration Management)
    ↓
Task 4.1 (Systemd Integration)
Task 4.2 (Logging System)
Task 4.3 (Documentation)
Task 4.4 (Testing & Validation)
```

## Success Metrics

### Sprint 1
- ✅ Module manager compiles and runs
- ✅ Configuration parser works
- ✅ Auto-discovery finds modules
- ✅ CLI tool is functional

### Sprint 2
- ✅ DI/DO operations work
- ✅ Power operations work
- ✅ Motor operations work
- ✅ AI operations work

### Sprint 3
- ✅ Hot-plug detection works
- ✅ Performance monitoring works
- ✅ Configuration management works

### Sprint 4
- ✅ Systemd service works
- ✅ Logging system works
- ✅ Documentation is complete
- ✅ All tests pass

## Risk Mitigation

### Technical Risks
- **JSON parser complexity**: Use simple parser initially
- **Memory management**: Use static allocation
- **Thread safety**: Use proper synchronization
- **Error handling**: Comprehensive error checking

### Mitigation Strategies
- **Start simple**: Basic functionality first
- **Test frequently**: Unit tests for each component
- **Document well**: Clear documentation
- **Validate early**: Test with real hardware

## Next Steps

1. **Start with Task 1.1**: Module Manager Core
2. **Build incrementally**: One task at a time
3. **Test continuously**: After each task
4. **Document as you go**: Keep docs updated

Với task list này, việc implement plug-and-play system sẽ được **chia nhỏ** và **dễ quản lý**!
