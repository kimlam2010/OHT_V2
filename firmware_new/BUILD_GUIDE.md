# OHT-50 Firmware Build Guide

Complete guide for building, testing, and deploying OHT-50 firmware.

## Prerequisites

### System Requirements
- **OS:** Linux (Ubuntu 20.04+ recommended)
- **RAM:** 4GB minimum, 8GB recommended
- **Disk:** 2GB free space
- **CPU:** Multi-core processor recommended

### Software Requirements

#### Required
- **CMake:** >= 3.16
- **GCC:** >= 9.0 (C11 support)
- **Make:** GNU Make
- **Git:** For version control

#### Optional
- **Clang:** For clang-tidy static analysis
- **Python3:** >= 3.8 (for API tests)
- **lcov/gcov:** For code coverage
- **Doxygen:** For API documentation

### Dependencies

#### System Libraries
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libssl-dev \
    libpthread-stubs0-dev \
    pkg-config

# Fedora/RHEL
sudo dnf install -y \
    gcc \
    make \
    cmake \
    git \
    openssl-devel \
    glibc-devel
```

#### Python Dependencies (for tests)
```bash
pip3 install requests websocket-client
```

## Quick Start

### 1. Clone Repository
```bash
cd /home/orangepi/Desktop/OHT_V2/firmware_new
```

### 2. Create Build Directory
```bash
mkdir -p build
cd build
```

### 3. Configure
```bash
cmake ..
```

### 4. Build
```bash
make -j$(nproc)
```

### 5. Run
```bash
./oht50_main
```

## Build Configurations

### Debug Build
For development with debug symbols:
```bash
mkdir -p build/debug
cd build/debug
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -j$(nproc)
```

### Release Build
Optimized for production:
```bash
mkdir -p build/release
cd build/release
cmake -DCMAKE_BUILD_TYPE=Release ../..
make -j$(nproc)
```

### RelWithDebInfo Build
Release with debug info:
```bash
mkdir -p build/relwithdebinfo
cd build/relwithdebinfo
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../..
make -j$(nproc)
```

### MinSizeRel Build
Optimized for size:
```bash
mkdir -p build/minsizerel
cd build/minsizerel
cmake -DCMAKE_BUILD_TYPE=MinSizeRel ../..
make -j$(nproc)
```

## Build Options

### Enable Code Coverage
```bash
cmake -DENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
make test
```

Generate coverage report:
```bash
lcov --capture --directory . --output-file coverage.info
lcov --remove coverage.info '/usr/*' --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

### Enable Sanitizers
AddressSanitizer and UndefinedBehaviorSanitizer:
```bash
cmake -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug ..
make -j$(nproc)
./oht50_main
```

### Enable Static Analysis
Using clang-tidy:
```bash
cmake -DENABLE_STATIC_ANALYSIS=ON ..
make -j$(nproc)
```

### Disable Tests
```bash
cmake -DBUILD_TESTING=OFF ..
make -j$(nproc)
```

### Feature Toggles
```bash
cmake \
    -DENABLE_LIDAR=ON \
    -DENABLE_WIFI_AP=ON \
    -DENABLE_WEBSOCKET=ON \
    -DENABLE_HTTPS=ON \
    ..
```

## Building Tests

### Build All Tests
```bash
cmake -DBUILD_TESTING=ON ..
make -j$(nproc)
```

### Build Specific Test Category
```bash
make test_hal_common        # Unit test
make test_basic_integration # Integration test
make test_performance       # Performance test
```

### Run Tests
```bash
# Run all tests
ctest -V

# Run specific test
ctest -R test_hal_common -V

# Run with parallel execution
ctest -j$(nproc)
```

## Cross-Compilation

### ARM (Orange Pi 5B)
```bash
# Set toolchain
export CC=aarch64-linux-gnu-gcc
export CXX=aarch64-linux-gnu-g++

# Configure for ARM
cmake \
    -DCMAKE_SYSTEM_NAME=Linux \
    -DCMAKE_SYSTEM_PROCESSOR=aarch64 \
    -DCMAKE_C_COMPILER=aarch64-linux-gnu-gcc \
    ..

# Build
make -j$(nproc)
```

## Installation

### Local Installation
```bash
cmake -DCMAKE_INSTALL_PREFIX=/usr/local ..
make -j$(nproc)
sudo make install
```

### Custom Installation
```bash
cmake -DCMAKE_INSTALL_PREFIX=/opt/oht50 ..
make -j$(nproc)
sudo make install
```

### Installation Structure
```
${CMAKE_INSTALL_PREFIX}/
├── bin/
│   └── oht50_main
└── share/oht50/
    └── config/
        └── modules.yaml
```

## Build Scripts

### Automated Build
```bash
./scripts/build/build.sh
```

### Clean Build
```bash
./scripts/build/build.sh clean
```

### Deploy Build
```bash
./scripts/deploy/deploy.sh
```

## Troubleshooting

### CMake Configuration Fails

**Problem:** CMake can't find OpenSSL
```bash
# Check OpenSSL installation
pkg-config --modversion openssl

# Install if missing
sudo apt-get install libssl-dev
```

**Problem:** CMake version too old
```bash
# Check version
cmake --version

# Install newer version
sudo apt-get install cmake
```

### Build Fails

**Problem:** Compiler errors
```bash
# Check compiler version
gcc --version

# Ensure C11 support
gcc -std=c11 --version
```

**Problem:** Missing headers
```bash
# Check include paths
echo | gcc -E -Wp,-v -

# Install development packages
sudo apt-get install build-essential
```

**Problem:** Linker errors
```bash
# Check library paths
ldconfig -p | grep ssl

# Install missing libraries
sudo apt-get install libssl-dev
```

### Runtime Issues

**Problem:** Shared library not found
```bash
# Check dependencies
ldd build/oht50_main

# Add library path
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH
```

**Problem:** Permission denied
```bash
# Check permissions
ls -la build/oht50_main

# Make executable
chmod +x build/oht50_main
```

## Build Performance

### Parallel Builds
```bash
# Use all cores
make -j$(nproc)

# Specific number of cores
make -j4
```

### Ccache
Speed up rebuilds with ccache:
```bash
# Install ccache
sudo apt-get install ccache

# Use ccache
cmake -DCMAKE_C_COMPILER_LAUNCHER=ccache ..
make -j$(nproc)
```

### Ninja Build System
Faster alternative to Make:
```bash
# Install ninja
sudo apt-get install ninja-build

# Use ninja
cmake -GNinja ..
ninja
```

## Best Practices

### Clean Builds
```bash
# Remove build directory
rm -rf build

# Recreate and rebuild
mkdir build && cd build
cmake ..
make -j$(nproc)
```

### Incremental Builds
```bash
# Just run make
cd build
make -j$(nproc)
```

### Build Type Selection
- **Development:** Use Debug
- **Testing:** Use Debug or RelWithDebInfo
- **Production:** Use Release
- **Embedded:** Use MinSizeRel

### Version Control
```bash
# Don't commit build artifacts
echo "build/" >> .gitignore
echo "*.o" >> .gitignore
```

## Advanced Topics

### Custom Compiler Flags
```bash
cmake -DCMAKE_C_FLAGS="-march=native -mtune=native" ..
```

### Static Linking
```bash
cmake -DBUILD_SHARED_LIBS=OFF ..
```

### LTO (Link Time Optimization)
```bash
cmake -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=ON ..
```

### Profile-Guided Optimization
```bash
# Step 1: Build with profiling
cmake -DCMAKE_C_FLAGS="-fprofile-generate" ..
make

# Step 2: Run to generate profile
./oht50_main

# Step 3: Build with profile
cmake -DCMAKE_C_FLAGS="-fprofile-use" ..
make
```

## CI/CD Integration

### GitHub Actions
```yaml
- name: Build
  run: |
    mkdir build
    cd build
    cmake -DCMAKE_BUILD_TYPE=Release ..
    make -j$(nproc)
```

### GitLab CI
```yaml
build:
  script:
    - mkdir build && cd build
    - cmake -DCMAKE_BUILD_TYPE=Release ..
    - make -j$(nproc)
```

## Related Documentation

- `README.md` - Project overview
- `CODE_STRUCTURE.md` - Code organization
- `cmake/README.md` - CMake configuration
- `tests/README.md` - Testing guide

---

**Last Updated:** 2025-10-07  
**Version:** 1.0


