# CMake Configuration Modules

CMake configuration modules for OHT-50 firmware build system.

## Modules

### CompilerOptions.cmake
Compiler flags and options for building the firmware.

**Features:**
- C11 standard compliance
- Comprehensive warning flags
- Build type specific optimizations
- Platform-specific definitions
- Thread support detection

**Build Types:**
- **Debug:** `-g3 -O0 -DDEBUG`
- **Release:** `-O3 -DNDEBUG`
- **RelWithDebInfo:** `-g -O2 -DNDEBUG`
- **MinSizeRel:** `-Os -DNDEBUG`

### BuildOptions.cmake
Build configuration options and features.

**Options:**
- `BUILD_TESTING` - Build test suite (ON by default)
- `BUILD_DOCS` - Build documentation (OFF by default)
- `ENABLE_COVERAGE` - Enable code coverage (OFF by default)
- `ENABLE_SANITIZERS` - Enable ASan/UBSan (OFF by default)
- `ENABLE_STATIC_ANALYSIS` - Enable clang-tidy (OFF by default)
- `REBUILD_MIN_API` - Minimal API rebuild (OFF by default)

**Feature Options:**
- `ENABLE_LIDAR` - Enable LiDAR support (ON by default)
- `ENABLE_WIFI_AP` - Enable WiFi AP (ON by default)
- `ENABLE_WEBSOCKET` - Enable WebSocket (ON by default)
- `ENABLE_HTTPS` - Enable HTTPS (ON by default)

### Dependencies.cmake
External dependencies and system libraries.

**Required:**
- Threads (pthread)
- OpenSSL (SSL/TLS)

**Optional:**
- RPLidar SDK (if ENABLE_LIDAR)

**System Libraries:**
- pthread, m, dl, rt

## Usage

### Basic Build
```bash
mkdir build && cd build
cmake ..
make
```

### Debug Build
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Release Build
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

### With Code Coverage
```bash
cmake -DENABLE_COVERAGE=ON ..
make
make test
gcov ...
```

### With Sanitizers
```bash
cmake -DENABLE_SANITIZERS=ON -DCMAKE_BUILD_TYPE=Debug ..
make
./oht50_main
```

### Without Tests
```bash
cmake -DBUILD_TESTING=OFF ..
make
```

### Custom Install Prefix
```bash
cmake -DCMAKE_INSTALL_PREFIX=/opt/oht50 ..
make install
```

## CMake Variables

### Project Variables
- `PROJECT_VERSION_MAJOR` - Major version
- `PROJECT_VERSION_MINOR` - Minor version
- `PROJECT_VERSION_PATCH` - Patch version

### Build Variables
- `CMAKE_BUILD_TYPE` - Build type (Debug/Release/etc.)
- `CMAKE_C_COMPILER` - C compiler
- `CMAKE_INSTALL_PREFIX` - Install location

### Feature Variables
- `ENABLE_LIDAR` - LiDAR support
- `ENABLE_WIFI_AP` - WiFi AP support
- `ENABLE_WEBSOCKET` - WebSocket support
- `ENABLE_HTTPS` - HTTPS support

## Build Output

### Executables
- `build/oht50_main` - Main firmware executable

### Test Executables
- `build/tests/unit/*` - Unit tests
- `build/tests/integration/*` - Integration tests
- `build/tests/performance/*` - Performance tests

### Install Locations
- Binaries: `${CMAKE_INSTALL_PREFIX}/bin/`
- Libraries: `${CMAKE_INSTALL_PREFIX}/lib/`
- Headers: `${CMAKE_INSTALL_PREFIX}/include/`
- Data: `${CMAKE_INSTALL_PREFIX}/share/oht50/`

## Build Targets

### Main Targets
- `oht50_main` - Main executable
- `all` - Build everything
- `clean` - Clean build artifacts

### Test Targets
- `test` - Run all tests
- `<test_name>` - Build specific test

### Install Targets
- `install` - Install binaries and data
- `uninstall` - Remove installed files (if available)

## Configuration

### Compiler Selection
```bash
# GCC
cmake -DCMAKE_C_COMPILER=gcc ..

# Clang
cmake -DCMAKE_C_COMPILER=clang ..
```

### Cross-Compilation
```bash
# ARM
cmake -DCMAKE_TOOLCHAIN_FILE=arm-toolchain.cmake ..
```

### Build Options
```bash
# Multiple options
cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DENABLE_COVERAGE=ON \
    -DENABLE_LIDAR=ON \
    -DBUILD_TESTING=ON \
    ..
```

## Troubleshooting

### Missing Dependencies
```bash
# Check OpenSSL
pkg-config --modversion openssl

# Install on Ubuntu/Debian
sudo apt-get install libssl-dev

# Install on Fedora/RHEL
sudo dnf install openssl-devel
```

### CMake Version
```bash
# Check version
cmake --version

# Minimum required: 3.16
```

### Compiler Issues
```bash
# Check compiler
gcc --version
clang --version

# Verify C11 support
echo "int main() { return 0; }" | gcc -std=c11 -x c - -o /dev/null
```

## Best Practices

### Out-of-Source Builds
Always build in a separate directory:
```bash
mkdir build
cd build
cmake ..
```

### Build Type
Always specify build type:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

### Parallel Build
Use parallel make:
```bash
make -j$(nproc)
```

### Clean Rebuild
```bash
rm -rf build
mkdir build
cd build
cmake ..
make
```

## Advanced Usage

### Verbose Build
```bash
cmake --build . --verbose
# or
make VERBOSE=1
```

### Export Compile Commands
```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
```

### Static Analysis
```bash
cmake -DENABLE_STATIC_ANALYSIS=ON ..
make
```

### Code Coverage Report
```bash
cmake -DENABLE_COVERAGE=ON ..
make
make test
lcov --capture --directory . --output-file coverage.info
genhtml coverage.info --output-directory coverage_html
```

---

**Last Updated:** 2025-10-07  
**Version:** 1.0


