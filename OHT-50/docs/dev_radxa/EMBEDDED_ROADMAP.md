# Embedded Development Roadmap – OHT-50 (v1.0)

## Tổng quan
- **Platform**: Orange Pi 5B (RK3588)
- **Tools**: opwiring, pyserial, FastAPI
- **Goal**: HAL RS485 + Control Loop + Local API + Telemetry

## Phase 1: Hardware Bring-up (Ưu tiên 1)

### 1.1 Cài đặt và cấu hình cơ bản
- [ ] **Cài đặt opwiring**: `sudo apt install opwiring`
- [ ] **Test UART1**: `opwiring uart enable 1`, `opwiring uart status 1`
- [ ] **Test GPIO**: `opwiring gpio set 1 3 1`, `opwiring gpio set 1 3 0`
- [ ] **Cấu hình RS485**: baud 115200, auto-RTS mode
- [ ] **Chạy test script**: `./test_rs485.sh`

### 1.2 Wiring RS485
- [ ] **Kết nối UART1**: TX (GPIO0_A2) → RS485 Transceiver TX
- [ ] **Kết nối UART1**: RX (GPIO0_A3) → RS485 Transceiver RX
- [ ] **Kết nối GND**: GND → RS485 Transceiver GND
- [ ] **Kết nối module**: RS485 A/B → Module RS485 A/B
- [ ] **Termination**: 120Ω hai đầu
- [ ] **Bias resistor**: 680Ω-10kΩ

### 1.3 Test protocol RS485
- [ ] **Test PING**: `opwiring uart write 1 --hex "AA 01 01 00 00 00"`
- [ ] **Test GET_INFO**: `opwiring uart write 1 --hex "AA 01 02 00 00 00"`
- [ ] **Test READ_DI**: `opwiring uart write 1 --hex "AA 01 10 00 00 00"`
- [ ] **Test READ_AI**: `opwiring uart write 1 --hex "AA 01 11 00 00 00"`
- [ ] **Response parsing**: `opwiring uart read 1 --hex`

## Phase 2: Software Development (Ưu tiên 2)

### 2.1 RS485 Bus Driver
- [ ] **File**: `drivers/bus_rs485.py`
- [ ] **Frame format**: `[0xAA][ADDR][CMD][LEN][PAYLOAD...][CRC16]`
- [ ] **CRC16-CCITT**: implementation + test
- [ ] **Timeout/Retry**: configurable parameters
- [ ] **Test với opwiring**: integration test

### 2.2 HAL Interfaces
- [ ] **File**: `drivers/hal.py`
- [ ] **MotorInterface**: enable, disable, set_velocity, set_position, read_feedback
- [ ] **IOInterface**: read_di, write_do, read_ai, write_ao
- [ ] **SafetyInterface**: read_status, latch_reset
- [ ] **LocationInterface**: read_tag_event, read_encoder, reset_encoder
- [ ] **Mock HAL**: `drivers/hal_mock.py` cho testing

### 2.3 Control System
- [ ] **File**: `control/profiles/`
- [ ] **Trapezoid profile**: velocity planning
- [ ] **S-curve profile**: smooth acceleration
- [ ] **File**: `control/planner.py`
- [ ] **Trajectory planning**: setpoint generation
- [ ] **File**: `control/state_machine.py`
- [ ] **States**: Idle, Move, Dock, Fault, E-Stop
- [ ] **Transitions**: event-driven state changes

### 2.4 Local API
- [ ] **File**: `services/local_api/api.py`
- [ ] **FastAPI**: health, status, command endpoints
- [ ] **WebSocket**: telemetry stream
- [ ] **Rate limiting**: 10-50 Hz telemetry
- [ ] **Error handling**: proper HTTP status codes

## Phase 3: Integration & Testing (Ưu tiên 3)

### 3.1 Control Loop
- [ ] **File**: `control/loop.py`
- [ ] **20ms tick**: jitter < 2ms
- [ ] **Async processing**: HAL + State Machine + Planner
- [ ] **Telemetry**: real-time data streaming
- [ ] **Logging**: JSON format, rotation

### 3.2 Simulation
- [ ] **File**: `sim/rail_1d.py`
- [ ] **1D motion**: inertia + friction
- [ ] **Position accuracy**: ±1-2mm
- [ ] **Integration test**: HAL mock + simulation

### 3.3 Center Client (Optional)
- [ ] **File**: `services/center_client/ws_client.py`
- [ ] **WebSocket client**: mission/permit reception
- [ ] **Reconnection**: backoff strategy
- [ ] **Message queue**: outbound events

## Phase 4: Production Readiness (Ưu tiên 4)

### 4.1 System Integration
- [ ] **Systemd service**: `oht50.service`
- [ ] **Configuration**: `config/system.yaml`
- [ ] **Logging**: `/var/log/oht50/`
- [ ] **User permissions**: `oht` user, dialout group

### 4.2 Testing & Validation
- [ ] **Unit tests**: pytest coverage > 80%
- [ ] **Integration tests**: end-to-end scenarios
- [ ] **Hardware tests**: real module communication
- [ ] **Performance tests**: latency, throughput

### 4.3 Documentation
- [ ] **API docs**: OpenAPI/Swagger
- [ ] **Architecture diagrams**: Mermaid
- [ ] **Wiring diagrams**: hardware connections
- [ ] **Troubleshooting guide**: common issues

## Task Dependencies

```
Hardware Bring-up
    ↓
RS485 Bus Driver
    ↓
HAL Interfaces
    ↓
Control System
    ↓
Local API
    ↓
Integration & Testing
    ↓
Production Readiness
```

## Success Criteria

### Sprint 0 (Hardware + Basic Software)
- [ ] UART1 hoạt động với opwiring
- [ ] GPIO DE/RE test OK
- [ ] RS485 protocol test OK với module thật
- [ ] Basic HAL mock hoạt động
- [ ] Simple control loop chạy được

### Sprint 1 (Control System)
- [ ] State machine hoạt động
- [ ] Trajectory planning hoạt động
- [ ] Local API endpoints hoạt động
- [ ] WebSocket telemetry stream hoạt động
- [ ] Unit tests pass

### Sprint 2 (Integration)
- [ ] End-to-end control loop hoạt động
- [ ] Real hardware integration
- [ ] Performance metrics OK
- [ ] Error handling robust
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

### opwiring Commands
```bash
# UART
opwiring uart list
opwiring uart enable 1
opwiring uart status 1
opwiring uart config 1 --baud 115200 --rs485 --rs485-auto-rts
opwiring uart test 1
opwiring uart write 1 --hex "AA 01 01 00 00 00"
opwiring uart read 1 --hex

# GPIO
opwiring gpio list
opwiring gpio info 1 3
opwiring gpio set 1 3 1
opwiring gpio set 1 3 0
```

### Test Commands
```bash
# Hardware test
./test_rs485.sh

# Software test
pytest tests/
pytest tests/ -v --cov=drivers --cov=control --cov=services

# Performance test
python -m pytest tests/test_performance.py -v
```

### Debug Commands
```bash
# System status
systemctl status oht50
journalctl -u oht50 -f

# Hardware debug
dmesg | grep -i uart
dmesg | grep -i gpio
```

## Next Steps

1. **Immediate**: Complete hardware bring-up with opwiring
2. **Short-term**: Implement RS485 bus driver with tests
3. **Medium-term**: Build control system and local API
4. **Long-term**: Production deployment and optimization

## Resources

- **Hardware docs**: `docs/dev_radxa/platform_orangepi_5b.md`
- **Protocol docs**: `docs/specs/rs485_opcode_payload_draft.md`
- **Test checklist**: `docs/specs/EMBED_TEST_CHECKLIST.md`
- **API specs**: `docs/specs/api_minimal.md`
- **Architecture**: `docs/specs/architecture.md`
