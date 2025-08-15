# Firmware Development Roadmap – OHT-50 (v1.0)

## Tổng quan
- **Platform**: Orange Pi 5B (RK3588)
- **Tools**: gcc, make, opwiring
- **Goal**: HAL RS485 + Control Loop + Safety + Telemetry

## Phase 1: Hardware Bring-up ✅

### 1.1 HAL Layer (Đã hoàn thành)
- [x] **hal_uart_dma.h/.c**: UART init, DMA RX/TX, ring buffer
- [x] **hal_rs485.h/.c**: RS485 DE/RE control, frame send/recv
- [x] **hal_relay.h/.c**: GPIO relay control (GPIO1_D3/D2)
- [x] **board_config.h**: Pin mapping Orange Pi 5B

### 1.2 Control Layer (Đã hoàn thành)
- [x] **state_machine.h/.c**: Idle/Move/Dock/Fault/E-Stop states
- [x] **safety_manager.h/.c**: E-Stop, watchdog, interlock
- [x] **Unit tests**: test_state_machine.c, test_safety_manager.c

### 1.3 Build System (Đã hoàn thành)
- [x] **Makefile**: Build system cơ bản
- [x] **Examples**: rs485_loopback.c
- [x] **Tools**: relayctl.c

## Phase 2: Software Development ⏳

### 2.1 RS485 Protocol Implementation
- [ ] **File**: `comms/rs485_protocol.h/.c`
- [ ] **Commands**: PING, GET_INFO, READ_DI, READ_AI, READ_FB, SET_POS, SET_VEL
- [ ] **Frame format**: `[0xAA][ADDR][CMD][LEN][PAYLOAD...][CRC16]`
- [ ] **CRC16-CCITT**: implementation + test
- [ ] **Timeout/Retry**: configurable parameters
- [ ] **Error handling**: TIMEOUT, CRC_FAIL, BUSY, NACK

### 2.2 Motor Control Interface
- [ ] **File**: `motor/motor_interface.h/.c`
- [ ] **Functions**: enable, disable, set_velocity, set_position, read_feedback
- [ ] **Safety**: overcurrent protection, temperature monitoring
- [ ] **Feedback**: position, velocity, current, temperature, fault status
- [ ] **Integration**: với RS485 protocol

### 2.3 Encoder/Position Fusion
- [ ] **File**: `motor/encoder.h/.c`
- [ ] **Encoder reading**: count, velocity calculation
- [ ] **Position fusion**: RFID + encoder integration
- [ ] **Calibration**: offset, scale, dead reckoning
- [ ] **Location tracking**: s_on_rail calculation

### 2.4 Trajectory Planning
- [ ] **File**: `control/profiles/`
- [ ] **Trapezoid profile**: velocity planning
- [ ] **S-curve profile**: smooth acceleration
- [ ] **File**: `control/planner.h/.c`
- [ ] **Motion planning**: setpoint generation
- [ ] **Limits**: velocity, acceleration, jerk limits

## Phase 3: Integration & Testing ⏳

### 3.1 Control Loop Integration
- [ ] **File**: `control/loop.h/.c`
- [ ] **20ms tick**: jitter < 2ms
- [ ] **Async processing**: HAL + State Machine + Planner
- [ ] **Real-time constraints**: priority scheduling
- [ ] **Performance monitoring**: latency, throughput

### 3.2 Telemetry System
- [ ] **File**: `comms/telemetry.h/.c`
- [ ] **Data packaging**: theo telemetry_schema.md
- [ ] **Streaming**: real-time data transmission
- [ ] **Backpressure**: khi quá tải
- [ ] **Rate limiting**: 10-50 Hz configurable

### 3.3 Configuration System
- [ ] **File**: `config/config_manager.h/.c`
- [ ] **JSON config**: system parameters
- [ ] **Runtime config**: dynamic parameter changes
- [ ] **Validation**: parameter bounds checking
- [ ] **Persistence**: config file storage

### 3.4 Backend Integration
- [ ] **File**: `comms/backend_interface.h/.c`
- [ ] **HTTP API**: health, status, command endpoints
- [ ] **WebSocket**: telemetry stream
- [ ] **Authentication**: token-based security
- [ ] **Error handling**: proper HTTP status codes

## Phase 4: Production Readiness ⏳

### 4.1 System Integration
- [ ] **Systemd service**: `oht50-firmware.service`
- [ ] **Logging system**: JSON format, rotation
- [ ] **User permissions**: oht user, dialout group
- [ ] **Startup scripts**: init.d/systemd

### 4.2 Performance Optimization
- [ ] **Memory optimization**: buffer management
- [ ] **CPU optimization**: critical path profiling
- [ ] **Power management**: sleep modes
- [ ] **Real-time tuning**: scheduler optimization

### 4.3 Testing & Validation
- [ ] **Unit tests**: pytest coverage > 80%
- [ ] **Integration tests**: end-to-end scenarios
- [ ] **Hardware tests**: real module communication
- [ ] **Performance tests**: latency, throughput
- [ ] **Stress tests**: fault injection, error recovery

### 4.4 Documentation
- [ ] **API docs**: OpenAPI/Swagger
- [ ] **Architecture diagrams**: Mermaid
- [ ] **Wiring diagrams**: hardware connections
- [ ] **Troubleshooting guide**: common issues
- [ ] **Deployment guide**: installation instructions

## Task Dependencies

```
Hardware Bring-up (Phase 1)
    ↓
RS485 Protocol (Phase 2.1)
    ↓
Motor Control (Phase 2.2)
    ↓
Encoder/Position (Phase 2.3)
    ↓
Trajectory Planning (Phase 2.4)
    ↓
Control Loop (Phase 3.1)
    ↓
Telemetry (Phase 3.2)
    ↓
Backend Integration (Phase 3.4)
    ↓
Production Readiness (Phase 4)
```

## Success Criteria

### Sprint 0 (Hardware + Basic Software) ✅
- [x] HAL skeleton hoạt động
- [x] State machine hoạt động
- [x] Safety manager hoạt động
- [x] Basic tests pass

### Sprint 1 (Protocol + Motor Control)
- [ ] RS485 protocol hoạt động
- [ ] Motor control interface hoạt động
- [ ] Encoder reading hoạt động
- [ ] Unit tests pass

### Sprint 2 (Control + Telemetry)
- [ ] Control loop hoạt động
- [ ] Trajectory planning hoạt động
- [ ] Telemetry streaming hoạt động
- [ ] Integration tests pass

### Sprint 3 (Production)
- [ ] Backend integration hoạt động
- [ ] Systemd service hoạt động
- [ ] Performance metrics OK
- [ ] Documentation complete

## Risk Mitigation

### Hardware Risks
- **UART1 không hoạt động**: Test với opwiring, check device tree overlay
- **GPIO conflict**: Check pin mapping, resolve conflicts
- **RS485 wiring sai**: Follow wiring diagram, test loopback
- **Module không respond**: Check address mapping, protocol compliance

### Software Risks
- **CRC errors**: Implement proper CRC16, test thoroughly
- **Timeout issues**: Tune timeout/retry parameters
- **State machine bugs**: Comprehensive unit tests
- **Performance issues**: Profile and optimize critical paths

### Integration Risks
- **Real-time constraints**: Monitor jitter, optimize async code
- **Memory leaks**: Use proper resource management
- **Network issues**: Implement robust reconnection
- **Safety violations**: Comprehensive E-Stop testing

## Tools & Commands Reference

### Build Commands
```bash
# Build all
make all

# Build specific target
make test_sm
make test_safety
make loopback

# Clean
make clean
```

### Test Commands
```bash
# Run unit tests
./test_sm && ./test_safety

# Test RS485 loopback
./loopback

# Test relay control
./tools/relayctl 1 on
./tools/relayctl 2 off
```

### Debug Commands
```bash
# Debug with gdb
gdb ./loopback

# Check hardware
./test_rs485.sh

# Monitor system
systemctl status oht50-firmware
journalctl -u oht50-firmware -f
```

## Next Steps

1. **Immediate**: Complete RS485 protocol implementation
2. **Short-term**: Implement motor control interface
3. **Medium-term**: Build control loop and telemetry
4. **Long-term**: Production deployment and optimization

## Resources

- **Hardware docs**: `docs/dev_radxa/platform_orangepi_5b.md`
- **Protocol docs**: `docs/specs/rs485_opcode_payload_draft.md`
- **Test checklist**: `docs/specs/EMBED_TEST_CHECKLIST.md`
- **Embedded roadmap**: `docs/dev_radxa/EMBEDDED_ROADMAP.md`
- **API specs**: `docs/specs/api_minimal.md`
