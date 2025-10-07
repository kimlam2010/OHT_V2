# OHT-50 Firmware

**Version:** 1.0.1 - Domain-Driven Architecture  
**Platform:** Orange Pi 5B (RK3588)  
**Standard:** ISO/IEC 12207 Compliant  
**Architecture:** Domain-Driven Design (DDD)

Firmware for OHT-50 (Overhead Hoist and Transfer) Master Module - A professionally organized and production-ready codebase with Domain-Driven Architecture.

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
├── build/              # Build directory (created during build)
├── config/             # Build and safety configuration
├── docs/               # Technical documentation
├── include/            # Public header files
├── scripts/            # Organized by functionality
│   ├── build/          # Build scripts
│   ├── deploy/         # Deployment scripts
│   ├── test/           # Test scripts
│   ├── rs485/          # RS485 scripts
│   ├── lidar/          # LiDAR scripts
│   └── safety/         # Safety scripts
├── src/                # Main source code
│   ├── app/            # ⭐ Application Layer (Domain-Driven Architecture v1.0.1)
│   │   ├── core/       # 🎛️ CORE LAYER (Independent, no external deps)
│   │   │   ├── state_management/  # State machine & controller
│   │   │   ├── safety/            # Safety monitoring (CRITICAL)
│   │   │   ├── control/           # Motion control & estimation
│   │   │   └── _backup/           # Historical backups
│   │   ├── infrastructure/  # 🔌 INFRASTRUCTURE LAYER (Technical services)
│   │   │   ├── communication/    # RS485/Modbus communication
│   │   │   ├── network/          # WiFi/LAN management
│   │   │   └── telemetry/        # Telemetry collection
│   │   ├── domain/     # 🏭 DOMAIN LAYER (Business logic)
│   │   │   ├── module_management/  # Module discovery & registry
│   │   │   ├── power/              # Power module (0x02)
│   │   │   ├── motion/             # Motor module (0x04)
│   │   │   ├── safety_module/      # Safety module (0x03)
│   │   │   └── dock/               # Dock module (0x05)
│   │   ├── application/  # 🔐 APPLICATION LAYER (Orchestration)
│   │   │   ├── safety_orchestrator/  # Multi-source safety coordination
│   │   │   └── system_orchestrator/  # System coordination (future)
│   │   ├── api/        # 🌐 API Layer (REST/WebSocket endpoints)
│   │   ├── validation/ # ✅ Validation (cross-cutting)
│   │   ├── storage/    # 💾 Storage (cross-cutting)
│   │   ├── managers/   # ⚠️ DEPRECATED (compatibility shim → infrastructure/application)
│   │   └── modules/    # ⚠️ DEPRECATED (compatibility shim → domain)
│   ├── hal/            # Hardware Abstraction Layer
│   │   ├── common/     # Common HAL definitions
│   │   ├── communication/  # RS485, Network communication
│   │   ├── peripherals/    # LED, Relay, LiDAR
│   │   ├── safety/     # E-Stop, Safety hardware
│   │   ├── gpio/       # GPIO operations
│   │   └── register/   # Register operations
│   └── main.c          # Entry point
├── tests/              # Test suites
│   ├── unit/           # Unit tests
│   └── integration/    # Integration tests
├── third_party/        # Third-party libraries
├── CMakeLists.txt      # Root CMake configuration
├── modules.yaml        # Module registry (runtime state)
├── MIGRATION_LOG_v1.0.1.md  # Architecture migration log
└── README.md           # This file
```

## Architecture (Domain-Driven Design v1.0.1)

Firmware đã được tổ chức lại theo **Domain-Driven Architecture** với 4 layers chính:

### 🎛️ **CORE LAYER** (`src/app/core/`)
Independent business logic layer - không depend on external systems.

**Domains:**
- **State Management** - System lifecycle (INIT → READY → RUNNING → FAULT → E-STOP)
- **Safety System** - Real-time safety monitoring (< 50ms response)
- **Control System** - Motion control & estimation

**Libraries:** `app_core` (includes: state_management, safety, control)

### 🔌 **INFRASTRUCTURE LAYER** (`src/app/infrastructure/`)
Technical services layer - handles I/O, networking, persistence.

**Services:**
- **Communication** - RS485/Modbus RTU communication
- **Network** - WiFi/LAN management with fallback
- **Telemetry** - Data collection & serialization

**Libraries:** `app_infrastructure` (includes: communication, network, telemetry)

### 🏭 **DOMAIN LAYER** (`src/app/domain/`)
Business domain layer - module-specific logic.

**Domains:**
- **Module Management** - Discovery, polling, registry
- **Power Domain** - Power module (0x02) handler
- **Motion Domain** - Travel motor (0x04) handler
- **Safety Module Domain** - Safety module (0x03) handler
- **Dock Domain** - Dock module (0x05) handler

**Libraries:** `app_domain` (includes: module_management, power, motion, safety_module, dock)

### 🔐 **APPLICATION LAYER** (`src/app/application/`)
Orchestration layer - coordinates multiple domains.

**Services:**
- **Safety Orchestrator** - Multi-source safety coordination
- **System Orchestrator** - System-wide coordination (future)

**Libraries:** `app_application` (includes: safety_orchestrator)

### 📊 **Dependency Flow**

```
┌─────────────────────────────────────────────┐
│           APPLICATION LAYER                 │
│  (Safety Orchestrator, System Orchestrator) │
└─────────────────┬───────────────────────────┘
                  │
┌─────────────────▼───────────────────────────┐
│             DOMAIN LAYER                     │
│   (Modules, Power, Motion, Safety, Dock)    │
└─────────────────┬───────────────────────────┘
                  │
┌─────────────────▼───────────────────────────┐
│         INFRASTRUCTURE LAYER                 │
│  (Communication, Network, Telemetry)         │
└─────────────────┬───────────────────────────┘
                  │
┌─────────────────▼───────────────────────────┐
│             CORE LAYER                       │
│    (State, Safety, Control)                  │
└─────────────────┬───────────────────────────┘
                  │
┌─────────────────▼───────────────────────────┐
│             HAL LAYER                        │
│    (Hardware Abstraction Layer)              │
└──────────────────────────────────────────────┘
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
**🏗️ Major Architecture Migration - Domain-Driven Design**

**Breaking Changes:** NONE (Backward compatible)

**Major Changes:**
- ✅ **Complete 4-layer architecture** - Core, Infrastructure, Domain, Application
- ✅ **Migrated 27 files** across 4 layers với git history preservation
- ✅ **Infrastructure layer** (12 files) - Communication, Network, Telemetry
- ✅ **Domain layer** (13 files) - Module Management, Power, Motion, Safety Module, Dock
- ✅ **Application layer** (2 files) - Safety Orchestrator
- ✅ **17 new CMakeLists.txt** - Modular build system
- ✅ **Legacy compatibility** - Old paths vẫn work via shims
- ✅ **Build successful** - oht50_main (473KB)

**Architecture Benefits:**
- 📦 Better organization & separation of concerns
- 🔧 Easier to maintain & extend
- 👥 Better team collaboration
- 🧪 Easier to test individual domains
- 📈 Improved scalability

**Migration Details:** See [MIGRATION_LOG_v1.0.1.md](MIGRATION_LOG_v1.0.1.md)  
**Architecture Guide:** See [src/app/ARCHITECTURE_v1.0.1.md](src/app/ARCHITECTURE_v1.0.1.md)

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
