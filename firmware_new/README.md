# OHT-50 Firmware

**Version:** 1.0.0  
**Platform:** Orange Pi 5B (RK3588)  
**Standard:** ISO/IEC 12207 Compliant

Firmware for OHT-50 (Overhead Hoist and Transfer) Master Module - A professionally organized and production-ready codebase.

## Overview

OHT-50 firmware provides comprehensive control and monitoring for automated material handling systems. It features:

- **Multi-protocol Communication:** RS485, Ethernet, WiFi
- **Safety System:** Real-time monitoring with E-Stop support
- **LiDAR Integration:** 360° obstacle detection
- **HTTP/WebSocket API:** RESTful API with real-time telemetry
- **Module Management:** Auto-discovery and control of slave modules

## Quick Start

```bash
# Build
mkdir build && cd build
cmake ..
make -j$(nproc)

# Run
./oht50_main
```

For detailed build instructions, see [BUILD_GUIDE.md](BUILD_GUIDE.md).

## Directory Structure

```
firmware_new/
├── build/              # Thư mục build (trống, sẽ được tạo khi build)
├── config/             # Cấu hình build và safety
├── docs/               # Tài liệu kỹ thuật
├── include/            # Header files
├── scripts/            # Scripts được tổ chức theo chức năng
│   ├── build/          # Scripts build
│   ├── deploy/         # Scripts deploy
│   ├── test/           # Scripts test
│   ├── rs485/          # Scripts RS485
│   ├── lidar/          # Scripts LiDAR
│   └── safety/         # Scripts safety
├── src/                # Source code chính
│   ├── app/            # Application layer
│   │   ├── core/       # ⭐ Domain-Driven Core Components
│   │   │   ├── state_management/  # State machine & system controller
│   │   │   ├── safety/            # Safety monitor & critical detection
│   │   │   ├── control/           # Control loop & estimator
│   │   │   └── _backup/           # Backup files (git history)
│   │   ├── managers/   # System managers
│   │   ├── modules/    # Module handlers
│   │   ├── api/        # API endpoints
│   │   ├── validation/ # Input validation
│   │   └── storage/    # Data storage
│   ├── hal/            # Hardware Abstraction Layer
│   │   ├── common/     # Common HAL definitions
│   │   ├── communication/ # RS485, Network communication
│   │   ├── peripherals/   # LED, Relay, LiDAR
│   │   ├── safety/     # E-Stop, Safety hardware
│   │   ├── gpio/       # GPIO operations
│   │   └── register/   # Register operations
│   └── main.c          # Entry point
├── tests/              # Test suites
│   ├── unit/           # Unit tests
│   └── integration/    # Integration tests
├── third_party/        # Third-party libraries
├── CMakeLists.txt      # CMake configuration
├── modules.yaml        # Module registry (clean state)
└── README.md           # File này
```

## Core Architecture (Domain-Driven Design)

Firmware core đã được tổ chức lại theo **Domain-Driven Architecture** để dễ maintain và scale:

### 🎛️ **State Management** (`src/app/core/state_management/`)
Quản lý trạng thái và lifecycle của hệ thống.

**Components:**
- `system_state_machine.c/h` - State machine chính (INIT → READY → RUNNING → FAULT → E-STOP)
- `system_controller.c/h` - System coordination và event handling

**Responsibilities:**
- State transitions và validation
- System lifecycle management
- Event coordination giữa các modules

### 🛡️ **Safety System** (`src/app/core/safety/`)
Hệ thống an toàn real-time với multi-level response.

**Components:**
- `safety_monitor.c/h` - Main safety monitoring (1,763 lines)
- `critical_module_detector.c/h` - Critical module detection (985 lines)
- `graduated_response_system.c/h` - Graduated safety response (936 lines)
- `safety_rs485_integration.c/h` - RS485 safety integration (239 lines)

**Responsibilities:**
- E-Stop monitoring và response (< 50ms)
- Critical module failure detection
- Graduated safety response (NORMAL → WARNING → CRITICAL → EMERGENCY)
- Safety-critical RS485 communication

### ⚙️ **Control System** (`src/app/core/control/`)
Motion control và position estimation.

**Components:**
- `control_loop.c/h` - PID control loop (664 lines)
- `estimator_1d.c/h` - 1D position estimator (36 lines)

**Responsibilities:**
- Velocity control (velocity-only architecture)
- Position estimation
- Motion profile execution
- Safety-aware control

### 📦 **Dependencies**

```
State Management  (no dependencies)
       ↑
Safety System  (depends on State Management)
       ↑
Control System  (depends on Safety & State Management)
```

**Design Principles:**
- ✅ Clear separation of concerns
- ✅ Single responsibility per domain
- ✅ Minimal coupling between domains
- ✅ Easy to test và maintain

## Key Features

### Communication
- **RS485 (Modbus RTU):** Slave module communication
- **Ethernet:** 10/100/1000 Mbps for Center connection
- **WiFi (5G/2.4G):** Backup connection and mobile app support
- **HTTP/REST API:** RESTful endpoints on port 8080
- **WebSocket:** Real-time telemetry streaming

### Safety System
- **E-Stop:** Single-channel emergency stop
- **Safety Monitor:** Real-time safety monitoring
- **Graduated Response:** Multi-level safety response
- **Fault Detection:** Comprehensive error detection and recovery

### Hardware Interface
- **GPIO Control:** 2x Relay outputs (GPIO1_D3, GPIO1_D2)
- **LED Status:** 5x status indicators (Power, System, Comm, Network, Error)
- **LiDAR:** RPLiDAR integration for 360° scanning

### API Endpoints
- Robot control and status
- Module management
- Configuration management
- Telemetry streaming
- Safety monitoring

## Documentation

### Getting Started
- **[INDEX.md](INDEX.md)** - Quick navigation index
- **[README.md](README.md)** - This file (project overview)
- **[BUILD_GUIDE.md](BUILD_GUIDE.md)** - Complete build instructions
- **[CODE_STRUCTURE.md](CODE_STRUCTURE.md)** - Code organization
- **[CODE_QUALITY.md](CODE_QUALITY.md)** - Code quality tools and practices

### Component Documentation
- **[src/README.md](src/README.md)** - Source code structure (TBD)
- **[include/README.md](include/README.md)** - Header files
- **[tests/README.md](tests/README.md)** - Test suite
- **[cmake/README.md](cmake/README.md)** - CMake configuration

### Comprehensive Docs
- **[DOCUMENTATION.md](DOCUMENTATION.md)** - Documentation index
- **[docs/](docs/)** - Full technical documentation

## Scripts

Scripts are organized by function in `scripts/`:

- **build/** - Build scripts (`build.sh`, `build_phase1.sh`)
- **deploy/** - Deployment scripts (`deploy.sh`, `start_firmware.sh`)
- **test/** - Test scripts (`run_all_tests.sh`, `setup_tests.sh`)
- **rs485/** - RS485 utilities (`rs485_quick_test.py`)
- **lidar/** - LiDAR utilities (`99-rplidar.rules`)
- **safety/** - Safety tests (`safety_test.sh`)

See `scripts/*/README.md` for details on each category.

## Testing

```bash
# Build with tests
cmake -DBUILD_TESTING=ON ..
make -j$(nproc)

# Run all tests
ctest -V

# Run specific category
cd tests/unit && ctest -V
cd tests/integration && ctest -V
```

For detailed testing guide, see [tests/README.md](tests/README.md).

## Code Quality

### Standards Compliance
- **ISO/IEC 12207:** Software lifecycle compliance
- **MISRA C:2012:** Partial compliance
- **C11 Standard:** Full compliance

### Build Quality
- **Warnings:** Comprehensive warning flags enabled
- **Static Analysis:** clang-tidy support
- **Code Coverage:** gcov/lcov integration
- **Sanitizers:** ASan and UBSan support

### Code Statistics
- **Total Lines:** 58,265 lines
- **Source Files:** 35 files (C/H)
- **Test Coverage:** >90% (target)

## Platform

### Hardware
- **Board:** Orange Pi 5B
- **Chipset:** RK3588 (ARM Cortex-A76/A55)
- **RAM:** 4GB+ recommended
- **Storage:** 16GB+ SD card

### Software
- **OS:** Linux (Ubuntu 20.04+ or Armbian)
- **Kernel:** 6.1.43-rockchip-rk3588
- **Compiler:** GCC 9.0+ with C11 support

### Interfaces
- **RS485:** `/dev/ttyOHT485` (UART1)
- **GPIO:** GPIO1_D3 (Relay1), GPIO1_D2 (Relay2)
- **Network:** Ethernet + WiFi

## Project Status

### ✅ Completed (Deep Cleanup)
- [x] Cleaned build artifacts
- [x] Organized test structure
- [x] Removed duplicate headers
- [x] Structured scripts by function
- [x] Created comprehensive documentation
- [x] Optimized CMake configuration
- [x] Added .gitignore
- [x] Created README for each directory

### Architecture
- [x] Multi-layer architecture (App, HAL, Drivers)
- [x] Modular component design
- [x] Clear separation of concerns
- [x] Hardware abstraction layer

### Features
- [x] HTTP/WebSocket server
- [x] RS485 communication
- [x] Safety system
- [x] Module management
- [x] LiDAR integration
- [x] Network management

## Contributing

### Code Style
- Follow existing code style
- Use C11 standard features
- Add comprehensive error handling
- Document all public APIs

### Testing
- Write unit tests for new features
- Ensure integration tests pass
- Test on target hardware
- Add performance benchmarks

### Documentation
- Update relevant README files
- Document API changes
- Add code examples
- Update changelog

## License

[Specify your license here]

## Contact

**Project:** OHT-50 Master Module  
**Team:** Firmware Development Team  

## Changelog

### v1.0.1 (2025-10-07)
**🏗️ Core Architecture Migration - Domain-Driven Design**

**Major Changes:**
- ✅ **Restructured `src/app/core/`** với Domain-Driven Architecture
- ✅ **Organized 3 domains:** State Management, Safety System, Control System
- ✅ **Modular build system** - Mỗi domain có CMakeLists.txt riêng
- ✅ **Backward compatible** - Không phá vỡ existing code

**Technical Details:**
- Migrated 18 files vào domain-specific folders
- Created 3 subdomain libraries (state_management, safety, control)
- Unified INTERFACE library cho backward compatibility
- Fixed 2 bugs: HAL constants và include placement
- Moved backup files vào `_backup/` folder

**Build System:**
- `app_core_state_management.a` - State management library
- `app_core_safety.a` - Safety system library  
- `app_core_control.a` - Control system library
- `app_core` - INTERFACE library aggregating all domains

**Benefits:**
- 🎯 Clear separation of concerns
- 📦 Better modularity và reusability
- 👥 Easier team collaboration
- 📈 Better scalability cho future features

### v1.0.0 (2025-10-07)
- Initial release with deep cleanup
- Comprehensive documentation structure
- Optimized build system
- Professional code organization

---

**Last Updated:** 2025-10-07  
**Status:** Production Ready  
**Architecture:** Domain-Driven Design (v1.0.1+)
