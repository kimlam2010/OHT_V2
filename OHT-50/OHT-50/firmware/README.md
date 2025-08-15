# OHT-50 Firmware HAL Skeleton (Orange Pi 5B)

Mục tiêu: EM-02/EM-03 (DE/RE + UART DMA ring buffer) và loopback/BER logging cho Orange Pi 5B.

## Platform & Tools
- **Platform**: Orange Pi 5B (RK3588)
- **Tools**: opwiring, gcc, make
- **OS**: Linux 6.1.43-rk3588

## Cấu trúc thư mục
```
firmware/
├── hal/                    # Hardware Abstraction Layer
│   ├── board_config.h      # Pin mapping Orange Pi 5B
│   ├── hal_uart_dma.h/.c   # UART init, DMA RX/TX, ring buffer
│   ├── hal_rs485.h/.c      # RS485 DE/RE control, frame send/recv
│   └── hal_relay.h/.c      # GPIO relay control (GPIO1_D3/D2)
├── control/                # Control System
│   ├── state_machine.h/.c  # Idle/Move/Dock/Fault/E-Stop states
│   ├── profiles/           # Trajectory planning (TBD)
│   └── planner.h/.c        # Motion planning (TBD)
├── safety/                 # Safety System
│   └── safety_manager.h/.c # E-Stop, watchdog, interlock
├── comms/                  # Communication (TBD)
│   ├── rs485_protocol.h/.c # PING, GET_INFO, READ_FB, etc.
│   └── telemetry.h/.c      # Telemetry packaging
├── motor/                  # Motor Control (TBD)
│   ├── motor_interface.h/.c
│   └── encoder.h/.c
├── tests/                  # Unit Tests
│   ├── test_state_machine.c
│   └── test_safety_manager.c
├── examples/               # Examples
│   └── rs485_loopback.c
├── tools/                  # Development Tools
│   └── relayctl.c          # CLI relay control
├── config/                 # Configuration (TBD)
│   └── gpio_orangepi5b.json
├── Makefile
└── README.md
```

## Pin Mapping Orange Pi 5B

### UART Mapping
- **UART1**: RS485 bus (GPIO0_A2/A3) - **CHÍNH**
- **UART0**: Console debug (không dùng cho điều khiển)
- **UART2**: Motor driver (GPIO0_A4/A5)
- **UART3**: CAN bus (GPIO0_A6/A7)

### GPIO Mapping
- **GPIO1_D3**: Relay1/DE-RE control
- **GPIO1_D2**: Relay2/DE-RE control (đang dùng cho status_led)

## API Overview

### HAL Layer
```c
// RS485 Communication
void rs485_init(void);
int rs485_send_frame(const uint8_t* data, uint16_t len);
int rs485_recv_frame(uint8_t* buf, uint16_t buf_size, uint32_t timeout_ms);

// Relay Control
void relay_init(void);
void relay_set(int channel, bool on);  // channel: 1 or 2

// UART DMA
void uart_dma_init(uint32_t baud, uint8_t parity, uint8_t stop_bits);
int uart_tx_write(const uint8_t* data, uint16_t len);
uint16_t uart_rx_available(void);
uint16_t uart_rx_read(uint8_t* buf, uint16_t len);
```

### Control Layer
```c
// State Machine
void oht_sm_init(OhtContext *ctx);
OhtState oht_sm_get_state(const OhtContext *ctx);
int oht_sm_handle_event(OhtContext *ctx, OhtEvent event);

// Safety Manager
void safety_init(SafetyManager *sm, uint32_t now_ms, uint32_t max_interval_ms);
bool safety_estop_is_active(const SafetyManager *sm);
bool safety_watchdog_violation(const SafetyManager *sm, uint32_t now_ms);
```

## Build & Test

### Prerequisites
```bash
# Install opwiring
sudo apt install opwiring

# Test hardware
./test_rs485.sh
```

### Build
```bash
make all
```

Sinh ra 3 binary:
- `test_sm`: unit test máy trạng thái Idle/Move/Dock/Fault/E‑Stop
- `test_safety`: unit test safety (E‑Stop input + watchdog)
- `loopback`: mô phỏng loopback RS485

### Test
```bash
# Run unit tests
./test_sm && ./test_safety

# Test RS485 loopback
./loopback

# Test relay control
./tools/relayctl 1 on
./tools/relayctl 2 off
```

## Configuration

### GPIO Configuration
File: `config/gpio_orangepi5b.json`
```json
{
  "relay1": {
    "gpiochip": 1,
    "line": 3,
    "description": "Relay1/DE-RE control"
  },
  "relay2": {
    "gpiochip": 1,
    "line": 2,
    "description": "Relay2/DE-RE control"
  }
}
```

### RS485 Configuration
```c
// board_config.h
#define RS485_UART_INSTANCE   UART1
#define RS485_TX_PIN          GPIO0_A2
#define RS485_RX_PIN          GPIO0_A3
#define RS485_DE_PIN          GPIO1_D3
#define RS485_RE_PIN          GPIO1_D3
#define RS485_BAUD_DEFAULT    115200
```

## Development Workflow

### 1. Hardware Bring-up
```bash
# Test UART1 with opwiring
opwiring uart enable 1
opwiring uart status 1

# Test GPIO
opwiring gpio set 1 3 1
opwiring gpio set 1 3 0

# Test RS485 protocol
opwiring uart write 1 --hex "AA 01 01 00 00 00"  # PING
opwiring uart read 1 --hex
```

### 2. Software Development
```bash
# Build and test
make clean && make all
./test_sm && ./test_safety

# Debug with gdb
gdb ./loopback
```

### 3. Integration Testing
```bash
# Test with real hardware
./loopback --real-hardware

# Test relay control
./tools/relayctl 1 on
sleep 1
./tools/relayctl 1 off
```

## Roadmap Integration

### Phase 1: Hardware Bring-up ✅
- [x] HAL skeleton
- [x] State machine
- [x] Safety manager
- [x] Basic tests

### Phase 2: Software Development ⏳
- [ ] RS485 protocol implementation
- [ ] Motor control interface
- [ ] Encoder/position fusion
- [ ] Telemetry packaging

### Phase 3: Integration & Testing ⏳
- [ ] Backend integration
- [ ] Center communication
- [ ] Configuration system
- [ ] End-to-end testing

### Phase 4: Production Readiness ⏳
- [ ] Systemd service
- [ ] Logging system
- [ ] Performance optimization
- [ ] Documentation

## Troubleshooting

### UART1 Issues
```bash
# Check UART1 status
opwiring uart status 1

# Check device tree overlay
opwiring overlay list
opwiring overlay enable rk3588-uart1-m1

# Check device nodes
ls -l /dev/ttyS*
```

### GPIO Issues
```bash
# Check GPIO mapping
opwiring gpio info 1 3
opwiring gpio info 1 2

# Test GPIO manually
opwiring gpio set 1 3 1
opwiring gpio set 1 3 0
```

### RS485 Issues
```bash
# Test RS485 loopback
opwiring uart test 1

# Test protocol
opwiring uart write 1 --hex "AA 01 01 00 00 00"
opwiring uart read 1 --hex
```

## References
- **Hardware docs**: `docs/dev_radxa/platform_orangepi_5b.md`
- **Protocol docs**: `docs/specs/rs485_opcode_payload_draft.md`
- **Test checklist**: `docs/specs/EMBED_TEST_CHECKLIST.md`
- **Roadmap**: `docs/dev_radxa/EMBEDDED_ROADMAP.md`
