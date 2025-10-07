# 📅 Changelog - OHT-50 Firmware

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [Unreleased]

### 🔮 Planned Features
- [ ] WebSocket support (optional re-enable)
- [ ] HTTPS/TLS support
- [ ] Multi-language support
- [ ] Advanced telemetry analytics
- [ ] Predictive maintenance
- [ ] Cloud integration

---

## [1.0.1] - 2025-10-07

### ✨ Added
- **Domain-Driven Architecture**: Restructured codebase into 4 layers (Core, Infrastructure, Domain, Application)
- **Modular CMake**: Separated build configuration into modular files
- **Performance Monitoring**: Real-time loop performance tracking with adaptive timing
- **Auto Cleanup**: Automatic process and port cleanup on startup
- **Module Polling Manager**: Dynamic polling for slave modules
- **Module Data Storage**: Persistent storage for module configurations
- **Network HAL**: WiFi and LAN management with failover
- **LiDAR Integration**: 360° obstacle detection and safety zones
- **API Documentation**: Comprehensive API reference
- **Code Quality Tools**: clang-format, clang-tidy, editorconfig

### 🔄 Changed
- **Boot Sequence**: Optimized to < 20ms (was ~200ms)
- **State Machine**: Improved transition logic BOOT → INIT → IDLE
- **Module Discovery**: Moved to background to avoid blocking boot
- **Communication Manager**: Refactored for better error handling
- **Safety System**: Enhanced multi-level graduated response
- **Telemetry**: Separated RS485 telemetry (2s) and system telemetry (1s)
- **LED Indicators**: Updated COMM LED logic for 4 mandatory modules

### 🐛 Fixed
- **Issue #135**: Fast boot sequence (< 10s to READY state)
- **Issue #90**: RS485 telemetry broadcasting
- **Issue #168**: Network API initialization
- **Port Conflicts**: Auto cleanup for port 8080/8081
- **Process Conflicts**: Kill old processes before start
- **Memory Leaks**: Fixed several memory leaks in communication layer
- **Race Conditions**: Fixed state machine race conditions
- **RS485 Timeouts**: Improved retry and timeout handling

### 🚫 Removed
- **WebSocket Support**: Removed per CTO decision (firmware only uses HTTP/REST)
- **Old Module Handlers**: Deprecated managers/ and modules/ folders (compatibility shim remains)
- **Duplicate Code**: Removed redundant implementations
- **Empty Directories**: Cleaned up empty src/drivers/, src/tests/

### 📚 Documentation
- Added `INSTALLATION.md` - Complete installation guide
- Added `USAGE.md` - Comprehensive usage guide
- Added `CONTRIBUTING.md` - Contribution guidelines
- Added `CHANGELOG.md` - This file
- Added `LICENSE.md` - Project license
- Added `DEVELOPMENT.md` - Development guide
- Added `API_REFERENCE.md` - API documentation
- Added `TROUBLESHOOTING.md` - Troubleshooting guide
- Added `SECURITY.md` - Security policy
- Updated `README.md` - Refreshed with new architecture
- Updated `BUILD_GUIDE.md` - Enhanced build instructions
- Added `ARCHITECTURE_QUICK_REFERENCE.md` - Quick reference card
- Added `DOMAIN_DRIVEN_MIGRATION_SUMMARY.md` - Migration details

### 🏗️ Architecture
- **CORE LAYER**: State Management, Safety System, Control System
- **INFRASTRUCTURE LAYER**: Communication, Network, Telemetry
- **DOMAIN LAYER**: Module Management, Power, Motion, Safety Module, Dock
- **APPLICATION LAYER**: Safety Orchestrator, System Orchestrator
- **Dependency Flow**: Application → Domain → Infrastructure → Core → HAL

### ⚡ Performance
- **Boot Time**: < 20ms (target met)
- **Loop Time**: Average 8-15μs, Max 25ms
- **API Response**: < 50ms (HTTP)
- **Safety Response**: < 100ms (E-Stop)
- **CPU Usage**: Target 70%, adaptive sleep 5-50ms

### 🧪 Testing
- 47 test files across 8 categories
- Unit tests for all core components
- Integration tests for system workflows
- Performance benchmarks
- Safety validation tests
- Network communication tests

---

## [1.0.0] - 2025-10-05

### ✨ Added
- **Initial Release**: Production-ready firmware for OHT-50 Master Module
- **Multi-protocol Communication**: RS485, Ethernet, WiFi
- **Safety System**: E-Stop hardware integration, safety monitoring
- **Module Management**: Auto-discovery and control of slave modules
- **HTTP API**: RESTful API on port 8080
- **State Machine**: System lifecycle management
- **HAL Layer**: Hardware abstraction for Orange Pi 5B
- **RS485 HAL**: Modbus RTU communication
- **GPIO HAL**: Relay control (GPIO1_D3, GPIO1_D2)
- **LED HAL**: 5x status indicators
- **Module Registry**: YAML-based module configuration
- **Safety Monitor**: Real-time safety monitoring
- **Communication Manager**: RS485/Modbus management
- **Telemetry Manager**: Data collection and streaming

### 🏗️ Initial Architecture
- Application Layer (app/)
- HAL Layer (hal/)
- Managers (communication, telemetry, module)
- Module Handlers (power, motor, dock, safety)

### 📚 Initial Documentation
- README.md
- BUILD_GUIDE.md
- CODE_STRUCTURE.md
- CODE_QUALITY.md

---

## [0.9.0] - 2025-09-20 (Beta)

### ✨ Added
- **Beta Release**: Initial beta testing
- **Core Features**: State machine, safety, communication
- **Hardware Integration**: Orange Pi 5B bring-up
- **RS485 Communication**: Basic Modbus RTU support
- **Module Discovery**: Auto-scan for slave modules

### 🐛 Fixed
- UART1 configuration issues
- GPIO permission problems
- RS485 CRC validation
- State machine deadlocks

### 🧪 Testing
- Basic unit tests
- Hardware-in-the-loop tests
- Manual testing procedures

---

## [0.1.0] - 2025-08-15 (Alpha)

### ✨ Added
- **Alpha Release**: Initial development version
- **Project Setup**: CMake build system
- **Basic HAL**: UART, GPIO stubs
- **Proof of Concept**: Basic communication test

### 📝 Notes
- Internal development only
- Not production-ready
- Experimental features

---

## 📋 **Version Naming Convention**

- **Major** (X.0.0): Breaking changes, major architecture changes
- **Minor** (1.X.0): New features, backward-compatible changes
- **Patch** (1.0.X): Bug fixes, minor improvements

---

## 🏷️ **Change Types**

- **✨ Added**: New features
- **🔄 Changed**: Changes in existing functionality
- **🚫 Deprecated**: Soon-to-be removed features
- **🗑️ Removed**: Removed features
- **🐛 Fixed**: Bug fixes
- **🔒 Security**: Security fixes
- **📚 Documentation**: Documentation changes
- **⚡ Performance**: Performance improvements
- **🧪 Testing**: Testing additions/changes

---

## 🔗 **Links**

- [GitHub Repository](https://github.com/your-org/OHT-50-firmware)
- [Issue Tracker](https://github.com/your-org/OHT-50-firmware/issues)
- [Releases](https://github.com/your-org/OHT-50-firmware/releases)
- [Documentation](https://docs.oht50.com)

---

## 📞 **Contact**

- **Email:** support@oht50.com
- **Slack:** #oht50-firmware
- **Website:** https://oht50.com

---

**Maintained By:** OHT-50 Firmware Team  
**Last Updated:** 2025-10-07

