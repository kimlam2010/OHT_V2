# OHT-50 Firmware Test Suite

Comprehensive test suite for OHT-50 firmware including unit tests, integration tests, performance tests, and API tests.

## Directory Structure

```
tests/
├── unit/               # Unit tests for individual components
├── integration/        # Integration tests for system components
│   └── issues/         # Issue-specific regression tests
├── performance/        # Performance and load tests
├── network/            # Network communication tests
├── safety/             # Safety system tests
├── smoke/              # Basic smoke tests
├── hal/                # HAL layer tests
└── api/                # API endpoint tests (Python)
```

## Test Categories

### Unit Tests (`unit/`)
Test individual functions and modules in isolation.

**Files:**
- `test_hal_common.c` - HAL common functionality tests
- `test_hal_common_wrapper.c` - HAL wrapper tests
- `simple_test.c` - Basic unit test examples

**Run:**
```bash
cd build/tests/unit
ctest -V
```

### Integration Tests (`integration/`)
Test interaction between multiple components.

**Files:**
- `test_basic_integration.c` - Basic integration tests
- `test_simple_integration.c` - Simplified integration tests
- `test_end_to_end.c` - End-to-end system tests
- `test_enhanced_state_machine.c` - State machine integration
- `test_api_endpoints_single.c` - API endpoint integration
- `test_debug_hal.c` - HAL debugging tests
- `test_simple_shutdown.c` - Shutdown procedure tests
- `issues/issue_135_*.c` - Issue #135 regression tests

**Run:**
```bash
cd build/tests/integration
ctest -V
```

### Performance Tests (`performance/`)
Test system performance under various loads.

**Files:**
- `test_basic_performance.c` - Basic performance metrics
- `test_performance_benchmark.c` - Comprehensive benchmarks
- `test_soak.c` - Long-running soak tests

**Run:**
```bash
cd build/tests/performance
ctest -V
```

### Network Tests (`network/`)
Test network communication functionality.

**Files:**
- `network_tests.c` - General network tests
- `simple_network_test.c` - Basic network tests
- `basic_network_test.c` - Network fundamentals
- `test_wifi_ap_manager.c` - WiFi AP manager tests
- `test_wifi_ap_api.c` - WiFi AP API tests
- `test_fallback_manager.c` - Network fallback tests

**Run:**
```bash
cd build/tests/network
ctest -V
```

### Safety Tests (`safety/`)
Test safety system functionality and compliance.

**Files:**
- Safety monitor tests
- E-Stop tests
- Safety configuration tests

**Run:**
```bash
cd build/tests/safety
ctest -V
```

### HAL Tests (`hal/`)
Test Hardware Abstraction Layer.

**Files:**
- HAL peripheral tests
- HAL communication tests

**Run:**
```bash
cd build/tests/hal
ctest -V
```

### API Tests (`api/`)
Python-based API endpoint tests.

**Files:**
- `test_http_api.py` - HTTP API endpoint tests
- `test_websocket_simple.py` - Basic WebSocket tests
- `test_websocket_stress.py` - WebSocket stress tests

**Run:**
```bash
python3 tests/api/test_http_api.py
python3 tests/api/test_websocket_simple.py
python3 tests/api/test_websocket_stress.py
```

### Smoke Tests (`smoke/`)
Quick sanity checks for basic functionality.

**Run:**
```bash
cd build/tests/smoke
ctest -V
```

## Running All Tests

### Build and Run All Tests
```bash
cd firmware_new
mkdir -p build
cd build
cmake ..
make
ctest -V
```

### Run Specific Test Category
```bash
cd build/tests/<category>
ctest -V
```

### Run Single Test
```bash
cd build/tests/<category>
./<test_executable>
```

### Run Python API Tests
```bash
# Start firmware first
./oht50_main &

# Run API tests
python3 tests/api/test_http_api.py
python3 tests/api/test_websocket_simple.py
python3 tests/api/test_websocket_stress.py
```

## Test Scripts

### Main Test Script
```bash
./tests/run_tests.sh
```

### Setup Test Environment
```bash
./scripts/test/setup_tests.sh
```

### Run All Tests
```bash
./scripts/test/run_all_tests.sh
```

## Test Requirements

### C Tests
- **CMake:** >= 3.16
- **GCC:** C11 support
- **Unity:** Testing framework (included)

### Python Tests
- **Python:** >= 3.8
- **Requests:** HTTP client library
- **WebSocket:** WebSocket client library

Install Python dependencies:
```bash
pip3 install requests websocket-client
```

## Test Coverage

### Current Coverage
- **Unit Tests:** ~90% code coverage
- **Integration Tests:** Key system flows
- **Performance Tests:** Load and stress scenarios
- **API Tests:** All major endpoints

### Coverage Goals
- **Target:** > 95% code coverage
- **Safety-critical:** 100% coverage
- **Performance:** All critical paths benchmarked

## Test Naming Conventions

### C Test Files
- **Unit:** `test_module_name.c`
- **Integration:** `test_integration_scenario.c`
- **Performance:** `test_performance_aspect.c`

### Python Test Files
- **API:** `test_api_endpoint.py`
- **WebSocket:** `test_websocket_feature.py`

### Test Functions
- **Format:** `test_module_function_scenario()`
- **Example:** `test_hal_rs485_send_success()`

## Continuous Integration

### Pre-commit Checks
```bash
./scripts/test/run_all_tests.sh
```

### Build Verification
```bash
./scripts/build/build.sh
ctest -V
```

### Performance Regression
```bash
./tests/performance/test_performance_benchmark
```

## Troubleshooting

### Tests Fail to Build
1. Check CMake version: `cmake --version`
2. Clean build: `rm -rf build && mkdir build && cd build && cmake ..`
3. Check compiler: `gcc --version`

### Tests Fail to Run
1. Check permissions: `chmod +x tests/run_tests.sh`
2. Check dependencies: `ldd build/tests/unit/test_hal_common`
3. Check logs: Review test output in `build/Testing/Temporary/`

### Python Tests Fail
1. Check firmware is running: `ps aux | grep oht50_main`
2. Check network: `curl http://localhost:8080/api/v1/status`
3. Check Python packages: `pip3 list | grep -E "(requests|websocket)"`

## Best Practices

### Writing Tests
1. **Isolate:** Each test should be independent
2. **Clear:** Test names should describe what they test
3. **Fast:** Unit tests should run quickly
4. **Reliable:** No flaky tests

### Test Data
1. Use fixtures for common test data
2. Clean up test resources after each test
3. Don't depend on external state

### Assertions
1. Use descriptive assertion messages
2. One logical assertion per test
3. Test both success and failure cases

## Contributing

### Adding New Tests
1. Place in appropriate category directory
2. Follow naming conventions
3. Add to CMakeLists.txt
4. Update this README
5. Ensure tests pass locally

### Test Review Checklist
- [ ] Test name is descriptive
- [ ] Test is in correct category
- [ ] Test is independent
- [ ] Test has clear assertions
- [ ] Test documentation is updated
- [ ] All tests pass

---

**Last Updated:** 2025-10-07  
**Version:** 1.0


