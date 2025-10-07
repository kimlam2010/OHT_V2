# OHT-50 Firmware Code Structure

**Version:** 1.1.0  
**Last Updated:** 2025-10-07  
**Status:** Production Ready

## Complete Directory Structure

```
firmware_new/
├── 📄 Configuration Files (5)
│   ├── .clang-format          # Code formatting (LLVM-based, 100 col)
│   ├── .clang-format-ignore   # Format exclusions (third_party, build)
│   ├── .clang-tidy            # Static analysis (7 check categories)
│   ├── .editorconfig          # Editor consistency (UTF-8, LF, spaces)
│   └── .gitignore             # Git ignore rules (comprehensive)
│
├── 📚 Documentation (8 main docs + 12 READMEs)
│   ├── INDEX.md               # Quick navigation index
│   ├── README.md              # Project overview and quick start
│   ├── BUILD_GUIDE.md         # Complete build instructions
│   ├── CODE_STRUCTURE.md      # This file - Code organization
│   ├── CODE_QUALITY.md        # Code quality tools and practices
│   ├── DOCUMENTATION.md       # Documentation navigation
│   ├── CLEANUP_SUMMARY.md     # Detailed cleanup report
│   └── FINAL_CLEANUP_REPORT.md # Executive cleanup summary
│
├── 🔧 Build System
│   ├── CMakeLists.txt         # Main CMake configuration (modular)
│   ├── cmake/                 # CMake modules (3 files)
│   │   ├── CompilerOptions.cmake  # C11, warnings, optimizations
│   │   ├── BuildOptions.cmake     # Build/feature options
│   │   └── Dependencies.cmake     # External dependencies
│   ├── config/                # Build configuration
│   │   ├── build_config.h.in  # Build config template
│   │   └── safety/            # Safety configuration
│   └── modules.yaml           # Module registry (clean state)
│
├── 💻 Source Code (58,265 lines)
│   ├── src/                   # Source files (35 files)
│   │   ├── app/               # Application layer (12,000+ lines)
│   │   │   ├── api/           # HTTP API endpoints (2,494 lines)
│   │   │   ├── core/          # ⭐ Domain-Driven Core (6,337 lines)
│   │   │   │   ├── state_management/  # State machine & controller (1,714 lines)
│   │   │   │   ├── safety/            # Safety system (3,923 lines)
│   │   │   │   ├── control/           # Control loop & estimator (700 lines)
│   │   │   │   └── _backup/           # Historical backups
│   │   │   ├── managers/      # System managers (communication, telemetry)
│   │   │   ├── modules/       # Module handlers (Power, Motor, Dock, Safety)
│   │   │   ├── config/        # Configuration management
│   │   │   ├── storage/       # Data storage
│   │   │   └── validation/    # Input validation
│   │   ├── hal/               # Hardware Abstraction Layer (8,000+ lines)
│   │   │   ├── common/        # Common HAL utilities
│   │   │   ├── communication/ # RS485, Network, WiFi (2,917 lines)
│   │   │   ├── gpio/          # GPIO control
│   │   │   ├── peripherals/   # LED, LiDAR (3,438 lines), Relay
│   │   │   ├── register/      # Register management
│   │   │   ├── safety/        # E-Stop HAL
│   │   │   └── storage/       # Storage HAL
│   │   ├── utils/             # Utility functions
│   │   └── main.c             # Main application (1,091 lines)
│   │
│   └── include/               # Public headers (4 files)
│       ├── constants.h        # Global constants (7.3 KB)
│       ├── register_map.h     # Hardware registers (30 KB)
│       ├── safety_types.h     # Safety definitions (4.9 KB)
│       └── version.h          # Version information (272 bytes)
│
├── 🧪 Testing (47 test files)
│   └── tests/                 # Test suites (8 categories)
│       ├── unit/              # Unit tests (component-level)
│       ├── integration/       # Integration tests
│       │   └── issues/        # Issue-specific tests (issue_135_*)
│       ├── performance/       # Performance benchmarks
│       ├── network/           # Network tests (6 files, moved from src/)
│       ├── safety/            # Safety system validation
│       ├── smoke/             # Quick sanity checks
│       ├── hal/               # HAL layer tests
│       └── api/               # Python API tests (3 scripts)
│
├── 🔨 Utilities & Scripts
│   └── scripts/               # Organized by function (6 categories)
│       ├── build/             # Build scripts (6 scripts)
│       │   └── README.md
│       ├── deploy/            # Deployment scripts (8 scripts)
│       │   └── README.md
│       ├── test/              # Test scripts (13 scripts)
│       │   └── README.md
│       ├── rs485/             # RS485 utilities (2 scripts)
│       │   └── README.md
│       ├── lidar/             # LiDAR utilities (1 file)
│       │   └── README.md
│       ├── safety/            # Safety tests (1 script)
│       │   └── README.md
│       └── module_cli.py      # Module management CLI
│
├── 📦 Dependencies
│   └── third_party/           # External libraries
│       └── unity/             # Unit testing framework
│
└── 📁 Generated (excluded by .gitignore)
    └── build/                 # CMake build directory (empty after cleanup)
```

## Code Statistics

### Total Lines of Code
- **Total:** 58,265 lines
- **Source files:** ~35 files (C/H)

### Largest Components
1. `hal_lidar.c` - 3,438 lines (LiDAR peripheral driver)
2. `api_endpoints.c` - 2,494 lines (HTTP API endpoints)
3. **`safety_monitor.c`** - 1,762 lines (Safety domain - Main monitor)
4. `communication_manager.c` - 1,658 lines (Communication management)
5. `http_server.c` - 1,386 lines (HTTP server implementation)
6. `telemetry_manager.c` - 1,112 lines (Telemetry management)
7. `hal_wifi_ap.c` - 1,110 lines (WiFi Access Point HAL)
8. `main.c` - 1,091 lines (Main application)
9. `hal_network.c` - 1,047 lines (Network HAL)
10. **`critical_module_detector.c`** - 985 lines (Safety domain - Detection)
11. **`graduated_response_system.c`** - 936 lines (Safety domain - Response)
12. **`system_state_machine.c`** - 962 lines (State Management domain)
13. **`system_controller.c`** - 752 lines (State Management domain)
14. **`control_loop.c`** - 664 lines (Control domain)

## Architecture Layers

### Application Layer (app/)
- **Purpose:** High-level application logic
- **Architecture:** Domain-Driven Design (since v1.0.1)
- **Components:**
  - **Core Domains** (3 domains với modular libraries):
    - 🎛️ State Management - System state machine & controller
    - 🛡️ Safety System - Multi-level safety monitoring
    - ⚙️ Control System - Motion control & estimation
  - API endpoints and HTTP server
  - Communication management
  - Module management  
  - Configuration management
  - Data storage và validation

### HAL Layer (hal/)
- **Purpose:** Hardware abstraction
- **Components:**
  - RS485 communication (760 lines)
  - Network interfaces (1,047 lines)
  - WiFi Access Point (1,110 lines)
  - LiDAR peripheral (3,438 lines)
  - GPIO, LED, Relay control
  - E-Stop safety system

### Driver Layer (drivers/)
- **Purpose:** Platform-specific implementations
- **Components:**
  - Orange Pi 5B board support
  - RK3588 chipset drivers

## Key Features

### Communication
- **RS485:** Modbus RTU for slave modules
- **Ethernet:** 10/100/1000 Mbps for Center connection
- **WiFi:** 5G/2.4G for backup and mobile app
- **HTTP/WebSocket:** API for frontend

### Safety System
- **E-Stop:** Single-channel emergency stop
- **Safety Monitor:** Real-time safety monitoring
- **Graduated Response:** Multi-level safety response
- **Fault Detection:** Comprehensive error detection

### API Endpoints
- **Robot Control:** Movement, docking, emergency stop
- **Status Monitoring:** Real-time status and telemetry
- **Configuration:** System and safety configuration
- **Module Management:** Slave module discovery and control
- **Network Management:** WiFi AP, fallback, connectivity

## Testing Infrastructure

### Test Categories
- **Unit Tests:** Component-level testing
- **Integration Tests:** System integration testing
- **Performance Tests:** Load and stress testing
- **Network Tests:** Communication testing
- **Safety Tests:** Safety system validation
- **Smoke Tests:** Basic functionality verification

## Project Organization

### Configuration Files (5 files)
Professional development tooling:
- **`.clang-format`** - LLVM-based formatting (4 spaces, 100 columns)
- **`.clang-tidy`** - Static analysis (7 check categories enabled)
- **`.editorconfig`** - Cross-editor consistency
- **`.clang-format-ignore`** - Exclude third-party code
- **`.gitignore`** - Comprehensive ignore rules

### Documentation Structure (20 files)
**Main Documentation (8 files):**
1. `INDEX.md` - Quick navigation guide
2. `README.md` - Project overview
3. `BUILD_GUIDE.md` - Build instructions (457 lines)
4. `CODE_STRUCTURE.md` - This file
5. `CODE_QUALITY.md` - Quality tools guide (478 lines)
6. `DOCUMENTATION.md` - Doc navigation
7. `CLEANUP_SUMMARY.md` - Cleanup details
8. `FINAL_CLEANUP_REPORT.md` - Executive summary

**Component Documentation (12 READMEs):**
- `include/README.md` - Header organization
- `tests/README.md` - Test suite guide
- `cmake/README.md` - CMake configuration
- `scripts/*/README.md` - 6 script category guides

### Build System Organization
**Modular CMake (4 files):**
- `CMakeLists.txt` - Main configuration (139 lines)
- `cmake/CompilerOptions.cmake` - Compiler flags
- `cmake/BuildOptions.cmake` - Build options
- `cmake/Dependencies.cmake` - External deps

**Build Options Available:**
- `BUILD_TESTING` - Enable/disable tests
- `ENABLE_COVERAGE` - Code coverage
- `ENABLE_SANITIZERS` - ASan/UBSan
- `ENABLE_STATIC_ANALYSIS` - clang-tidy
- `ENABLE_LIDAR` - LiDAR support
- `ENABLE_WIFI_AP` - WiFi AP support
- `ENABLE_WEBSOCKET` - WebSocket support
- `ENABLE_HTTPS` - HTTPS support

### Test Organization (8 categories)
**Test Structure:**
```
tests/ (47 test files)
├── unit/              # Component-level tests
├── integration/       # System integration tests
│   └── issues/        # Issue-specific regression tests
├── performance/       # Performance benchmarks
├── network/           # Network communication tests (6 files)
├── safety/            # Safety system validation
├── smoke/             # Quick sanity checks
├── hal/               # HAL layer tests
└── api/               # Python API tests (3 scripts)
```

### Script Organization (6 categories)
**Scripts by Function:**
```
scripts/ (30+ scripts)
├── build/    (6)   # Build-related scripts
├── deploy/   (8)   # Deployment scripts
├── test/     (13)  # Testing scripts
├── rs485/    (2)   # RS485 utilities
├── lidar/    (1)   # LiDAR utilities
└── safety/   (1)   # Safety tests
```

## Deep Cleanup Summary

### Changes Made

**🏗️ Core Architecture Migration (2025-10-07 - v1.0.1):**
- ✅ **Restructured `src/app/core/`** thành Domain-Driven Architecture
- ✅ Created 3 domain subfolders: `state_management/`, `safety/`, `control/`
- ✅ Migrated 18 files vào domain-specific folders
- ✅ Created modular CMakeLists.txt cho mỗi domain
- ✅ Moved backup files vào `_backup/` folder
- ✅ Fixed 2 bugs: HAL constants và include placement
- ✅ Created README.md cho mỗi domain
- ✅ Backward compatible - KHÔNG phá vỡ existing code

**Build System Changes:**
- ✅ `app_core_state_management.a` - State management library
- ✅ `app_core_safety.a` - Safety system library
- ✅ `app_core_control.a` - Control system library
- ✅ `app_core` - INTERFACE library tổng hợp

**Deep Cleanup (2025-10-07 - v1.0.0):**

**1. Source Code Organization:**
- ✅ Moved 7 test files from `src/tests/network/` → `tests/network/`
- ✅ Moved `src/tests/test_register_info.c` → `tests/unit/`
- ✅ Removed duplicate `include/http_server.h`
- ✅ Removed unused `include/module_registry.h`
- ✅ Deleted `src/app/modules/` (empty, non-existent files)
- ✅ Deleted `src/drivers/` (completely empty)
- ✅ Deleted 4 empty subdirectories in `include/`
- ✅ Deleted empty `src/tests/` directory

**2. Build Artifacts Cleanup:**
- ✅ Cleaned entire `build/` directory
- ✅ Removed `*.o` files
- ✅ Removed old test executables
- ✅ Created comprehensive `.gitignore`

**3. Test Structure:**
- ✅ Organized into 8 categories
- ✅ Created `integration/issues/` subfolder
- ✅ Moved Python API tests to `tests/api/`
- ✅ Created `tests/README.md` (comprehensive guide)

**4. Scripts Reorganization:**
- ✅ Created 6 functional directories
- ✅ Moved 30+ scripts to appropriate categories
- ✅ Created README.md for each category

**5. CMake Optimization:**
- ✅ Modularized into 3 separate files
- ✅ Added 8+ build options
- ✅ Added feature toggles
- ✅ Improved dependency management
- ✅ Added build summary output

**6. Documentation Creation:**
- ✅ Created 8 main documentation files
- ✅ Created 12 component READMEs
- ✅ Total: 20 documentation files

**7. Code Quality Tools:**
- ✅ Created `.clang-format` configuration
- ✅ Optimized `.clang-tidy` settings
- ✅ Created `.editorconfig`
- ✅ Created `.clang-format-ignore`
- ✅ Created comprehensive `CODE_QUALITY.md`

### Files Summary
- **Created:** 20 files (docs + configs)
- **Moved:** 10 files (tests to proper location)
- **Deleted:** 13 items (1 header + 7 directories + 5 artifacts)
- **Directories Created:** 12 (test categories + script categories)

### Unused Files Removed (2025-10-07)
- ❌ `include/module_registry.h` - No corresponding .c file
- ❌ `src/app/modules/` - Directory with non-existent .c files
- ❌ `src/drivers/` - Completely empty directory
- ❌ `include/common/` - Empty subdirectory
- ❌ `include/drivers/` - Empty subdirectory
- ❌ `include/app/` - Empty subdirectory
- ❌ `include/hal/` - Empty subdirectory
- ✅ Updated `CMakeLists.txt` - Removed app_modules link
- ✅ Updated `src/app/CMakeLists.txt` - Removed modules subdirectory

## Naming Conventions

### Files
- **Source files:** `module_name.c`
- **Header files:** `module_name.h`
- **Test files:** `test_module_name.c`
- **Script files:** `action_name.sh`

### Functions
- **HAL functions:** `hal_module_action()`
- **API functions:** `api_resource_action()`
- **Manager functions:** `manager_name_action()`
- **Utility functions:** `util_category_action()`

### Constants
- **Macros:** `MODULE_CONSTANT_NAME`
- **Types:** `module_name_t`
- **Enums:** `MODULE_ENUM_VALUE`

## Dependencies

### External Libraries
- **Unity:** Unit testing framework
- **OpenSSL:** SSL/TLS support
- **pthread:** Multi-threading
- **math (libm):** Mathematical functions

### Build System
- **CMake:** Build configuration (version 3.16+)
- **C Standard:** C11
- **Compiler:** GCC with strict warnings

## Compliance

### Standards
- **ISO/IEC 12207:** Software lifecycle compliance
- **MISRA C:2012:** C coding standard (partial)
- **Safety Standards:** Basic safety compliance

### Code Quality
**Compiler Warnings (15+ flags):**
- `-Wall -Wextra -Wpedantic`
- `-Werror=implicit-function-declaration`
- `-Werror=return-type`
- `-Werror=uninitialized`
- `-Wformat=2 -Wformat-security`
- `-Wnull-dereference`
- `-Wstrict-prototypes -Wmissing-prototypes`
- `-Wold-style-definition`
- `-Wshadow -Wcast-align`
- `-Wconversion -Wsign-conversion`

**Static Analysis:**
- `clang-tidy` with 7 check categories
- `clang-format` for consistent formatting
- Mock detection guard (build-time)

**Quality Tools:**
- Code coverage: gcov/lcov
- Memory checking: AddressSanitizer
- Undefined behavior: UBSan
- Thread safety: ThreadSanitizer (available)

## Version Information

- **Firmware Version:** 1.0.0
- **HTTP Server Version:** 1.0.0
- **Schema Version:** 1

## Notes

### Performance Targets
- **HTTP API:** < 50ms response time
- **WebSocket:** < 20ms latency
- **Safety Response:** < 100ms E-Stop response

### Platform
- **Board:** Orange Pi 5B
- **Chipset:** RK3588
- **OS:** Linux (6.1.43-rockchip-rk3588)

### Key Interfaces
- **RS485:** `/dev/ttyOHT485` (UART1)
- **GPIO:** GPIO1_D3 (Relay1), GPIO1_D2 (Relay2)
- **LED:** 5x status LEDs
- **HTTP:** Port 8080 (default)

## Quick Reference

### Key Files
```
📄 Main Entry:          src/main.c (1,091 lines)
🌐 HTTP Server:         src/app/http_server.c (1,386 lines)
📡 API Endpoints:       src/app/api/api_endpoints.c (2,494 lines)

🎛️ State Management Domain:
   - system_state_machine.c (962 lines)
   - system_controller.c (752 lines)

🛡️ Safety System Domain:
   - safety_monitor.c (1,762 lines)
   - critical_module_detector.c (985 lines)
   - graduated_response_system.c (936 lines)
   - safety_rs485_integration.c (239 lines)

⚙️ Control System Domain:
   - control_loop.c (664 lines)
   - estimator_1d.c (36 lines)

🔧 HAL Layer:
   - hal_rs485.c (760 lines)
   - hal_network.c (1,047 lines)
   - hal_wifi_ap.c (1,110 lines)
   - hal_lidar.c (3,438 lines)

📊 Managers:
   - communication_manager.c (1,658 lines)
   - telemetry_manager.c (1,112 lines)
```

### Key Headers
```
🔢 Constants:          include/constants.h (7.3 KB)
📋 Register Map:       include/register_map.h (30 KB - largest)
🛡️ Safety Types:       include/safety_types.h (4.9 KB)
ℹ️ Version Info:       include/version.h (272 bytes)
```

### Build Commands
```bash
# Quick build
mkdir build && cd build && cmake .. && make -j$(nproc)

# Debug with coverage
cmake -DCMAKE_BUILD_TYPE=Debug -DENABLE_COVERAGE=ON ..

# Release with static analysis
cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_STATIC_ANALYSIS=ON ..

# With sanitizers
cmake -DENABLE_SANITIZERS=ON ..
```

### Documentation Map
```
📚 Start:              INDEX.md
📖 Overview:           README.md
🔨 Build:              BUILD_GUIDE.md
🏗️ Structure:          CODE_STRUCTURE.md (this file)
✨ Quality:            CODE_QUALITY.md
📁 Docs Index:         DOCUMENTATION.md
🧹 Cleanup:            CLEANUP_SUMMARY.md
📊 Final Report:       FINAL_CLEANUP_REPORT.md
```

### Project Metrics

| Metric | Value |
|--------|-------|
| **Total Code** | 58,265 lines |
| **Source Files** | 70 (C/H) |
| **Test Files** | 47 (C/Python) |
| **Documentation** | 20 files |
| **Config Files** | 5 quality tools |
| **CMake Modules** | 3 modules |
| **Build Options** | 8+ options |
| **Test Categories** | 8 categories |
| **Script Categories** | 6 categories |

### Component Sizes

| Component | Lines | Description |
|-----------|-------|-------------|
| LiDAR HAL | 3,438 | Largest component |
| API Endpoints | 2,494 | HTTP REST API |
| Safety Monitor | 1,762 | Safety system |
| Comm Manager | 1,658 | Communication |
| HTTP Server | 1,386 | Web server |
| Telemetry Mgr | 1,112 | Telemetry |
| WiFi AP HAL | 1,110 | WiFi Access Point |
| Main | 1,091 | Application entry |
| Network HAL | 1,047 | Network interface |

---

**Document Version:** 1.1.0  
**Last Updated:** 2025-10-07  
**Status:** ✅ Production Ready  
**Maintained By:** Firmware Team


