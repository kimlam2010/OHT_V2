# OHT-50 Managers Diagnostic Tools Guide

**Version:** 1.0  
**Date:** 2025-01-28  
**Status:** Active  

## Table of Contents

1. [Overview](#overview)
2. [Monitoring Tools](#monitoring-tools)
3. [Test Utilities](#test-utilities)
4. [Debug Tools](#debug-tools)
5. [Performance Analysis](#performance-analysis)
6. [Troubleshooting Scripts](#troubleshooting-scripts)
7. [Integration Testing](#integration-testing)

---

## Overview

This guide provides comprehensive diagnostic tools for monitoring, testing, and debugging the OHT-50 firmware managers. All tools are designed to work with the thread-safe, HAL-integrated manager architecture.

### Tool Categories

- **Monitoring Tools:** Real-time status and statistics monitoring
- **Test Utilities:** Automated testing and validation
- **Debug Tools:** Detailed debugging and analysis
- **Performance Analysis:** Performance metrics and optimization
- **Troubleshooting Scripts:** Common problem diagnosis and resolution

---

## Monitoring Tools

### 1. Manager Status Monitor

**File:** `tools/manager_monitor.c`

**Purpose:** Real-time monitoring of all manager states

**Usage:**
```bash
# Monitor all managers
./tools/manager_monitor --all

# Monitor specific manager
./tools/manager_monitor --comm
./tools/manager_monitor --module
./tools/manager_monitor --safety
./tools/manager_monitor --telemetry

# Continuous monitoring with refresh
./tools/manager_monitor --all --refresh=1s

# Export to file
./tools/manager_monitor --all --output=status.log
```

**Output Format:**
```
[2025-01-28 10:30:15] Communication Manager:
  Status: Initialized
  RS485 Port: /dev/ttyOHT485
  Baud Rate: 115200
  Statistics:
    Total Transmissions: 1250
    Successful: 1245
    Failed: 5
    Error Rate: 0.4%
    Avg Response Time: 45ms

[2025-01-28 10:30:15] Module Manager:
  Status: Operational
  Discovered Modules: 3
  Health Check Interval: 1000ms
  Modules:
    ID: 1 (Power Module) - Health: 95%
    ID: 2 (Safety Module) - Health: 98%
    ID: 3 (Motor Module) - Health: 92%

[2025-01-28 10:30:15] Safety Manager:
  Status: Normal
  E-Stop: Released
  Safety Circuit: OK
  Faults: None
  Response Time: 15ms

[2025-01-28 10:30:15] Telemetry Manager:
  Status: Active
  Collection Interval: 100ms
  JSON Buffer Size: 2048 bytes
  Buffer Usage: 45%
  Last Collection: 10ms ago
```

### 2. Communication Monitor

**File:** `tools/comm_monitor.c`

**Purpose:** Detailed RS485/Modbus communication monitoring

**Usage:**
```bash
# Monitor Modbus traffic
./tools/comm_monitor --modbus --verbose

# Monitor specific slave
./tools/comm_monitor --slave=1 --detailed

# Monitor error patterns
./tools/comm_monitor --errors --analyze

# Export communication logs
./tools/comm_monitor --export=comm.log --format=json
```

**Features:**
- Real-time Modbus frame analysis
- Error pattern detection
- Response time statistics
- CRC error tracking
- Retry analysis

### 3. Module Discovery Monitor

**File:** `tools/module_monitor.c`

**Purpose:** Module discovery and health monitoring

**Usage:**
```bash
# Monitor module discovery
./tools/module_monitor --discovery --verbose

# Monitor specific module health
./tools/module_monitor --id=1 --health --continuous

# Scan for new modules
./tools/module_monitor --scan --range=1-20

# Export module status
./tools/module_monitor --export=modules.json
```

**Features:**
- Module discovery tracking
- Health check monitoring
- Response time analysis
- Module type detection
- Status history

### 4. Safety Monitor

**File:** `tools/safety_monitor.c`

**Purpose:** Safety system monitoring and analysis

**Usage:**
```bash
# Monitor safety status
./tools/safety_monitor --status --continuous

# Monitor E-Stop events
./tools/safety_monitor --estop --events

# Monitor safety circuit
./tools/safety_monitor --circuit --detailed

# Export safety logs
./tools/safety_monitor --export=safety.log
```

**Features:**
- E-Stop event tracking
- Safety circuit monitoring
- Fault detection and analysis
- Response time measurement
- Safety state history

### 5. Telemetry Monitor

**File:** `tools/telemetry_monitor.c`

**Purpose:** Telemetry data monitoring and analysis

**Usage:**
```bash
# Monitor telemetry flow
./tools/telemetry_monitor --flow --verbose

# Monitor JSON serialization
./tools/telemetry_monitor --json --validate

# Monitor buffer usage
./tools/telemetry_monitor --buffer --threshold=80

# Export telemetry data
./tools/telemetry_monitor --export=telemetry.json
```

**Features:**
- Real-time telemetry monitoring
- JSON validation
- Buffer usage tracking
- Data flow analysis
- Performance metrics

---

## Test Utilities

### 1. Manager Integration Test

**File:** `tests/test_managers_integration.c`

**Purpose:** Comprehensive integration testing

**Usage:**
```bash
# Run all integration tests
./tests/test_managers_integration

# Run specific test
./tests/test_managers_integration --test=communication
./tests/test_managers_integration --test=module_discovery
./tests/test_managers_integration --test=safety
./tests/test_managers_integration --test=telemetry

# Run with verbose output
./tests/test_managers_integration --verbose

# Run performance tests
./tests/test_managers_integration --performance
```

**Test Coverage:**
- Communication manager RS485 integration
- Module manager discovery and health checks
- Safety manager E-Stop integration
- Telemetry manager JSON serialization
- Cross-manager integration flow
- Thread safety testing
- Error recovery testing

### 2. Communication Test Suite

**File:** `tools/comm_test.c`

**Purpose:** RS485/Modbus communication testing

**Usage:**
```bash
# Test basic communication
./tools/comm_test --basic

# Test specific slave
./tools/comm_test --slave=1 --read --address=0 --quantity=10

# Test write operations
./tools/comm_test --slave=1 --write --address=100 --data=1234

# Test error conditions
./tools/comm_test --error-injection

# Test performance
./tools/comm_test --performance --duration=60
```

**Test Scenarios:**
- Basic read/write operations
- Error injection testing
- Performance benchmarking
- Timeout testing
- CRC error testing

### 3. Module Test Suite

**File:** `tools/module_test.c`

**Purpose:** Module discovery and health testing

**Usage:**
```bash
# Test module discovery
./tools/module_test --discovery

# Test specific module
./tools/module_test --id=1 --health

# Test health monitoring
./tools/module_test --health --duration=300

# Test module communication
./tools/module_test --id=1 --communication

# Test error scenarios
./tools/module_test --error-injection
```

**Test Scenarios:**
- Module discovery testing
- Health check validation
- Communication testing
- Error condition testing
- Performance testing

### 4. Safety Test Suite

**File:** `tools/safety_test.c`

**Purpose:** Safety system testing and validation

**Usage:**
```bash
# Test E-Stop functionality
./tools/safety_test --estop

# Test safety circuit
./tools/safety_test --circuit

# Test fault detection
./tools/safety_test --fault-injection

# Test response time
./tools/safety_test --response-time

# Test recovery procedures
./tools/safety_test --recovery
```

**Test Scenarios:**
- E-Stop button testing
- Safety circuit validation
- Fault injection testing
- Response time measurement
- Recovery procedure testing

### 5. Telemetry Test Suite

**File:** `tools/telemetry_test.c`

**Purpose:** Telemetry system testing

**Usage:**
```bash
# Test data collection
./tools/telemetry_test --collection

# Test JSON serialization
./tools/telemetry_test --json --validate

# Test buffer handling
./tools/telemetry_test --buffer --overflow

# Test performance
./tools/telemetry_test --performance

# Test error conditions
./tools/telemetry_test --error-injection
```

**Test Scenarios:**
- Data collection testing
- JSON serialization validation
- Buffer overflow testing
- Performance benchmarking
- Error condition testing

---

## Debug Tools

### 1. Communication Debugger

**File:** `tools/comm_debug.c`

**Purpose:** Detailed communication debugging

**Usage:**
```bash
# Enable debug mode
export COMM_DEBUG=1

# Debug Modbus frames
./tools/comm_debug --frames --hex

# Debug specific transaction
./tools/comm_debug --transaction --id=123

# Debug error conditions
./tools/comm_debug --errors --detailed

# Debug timing
./tools/comm_debug --timing --verbose
```

**Debug Features:**
- Modbus frame analysis
- Transaction tracking
- Error condition analysis
- Timing analysis
- Memory usage tracking

### 2. Module Debugger

**File:** `tools/module_debug.c`

**Purpose:** Module system debugging

**Usage:**
```bash
# Enable module debug
export MODULE_DEBUG=1

# Debug discovery process
./tools/module_debug --discovery --step

# Debug health checks
./tools/module_debug --health --id=1

# Debug communication
./tools/module_debug --communication --id=1

# Debug state changes
./tools/module_debug --state --verbose
```

**Debug Features:**
- Discovery process debugging
- Health check analysis
- Communication debugging
- State change tracking
- Error condition analysis

### 3. Safety Debugger

**File:** `tools/safety_debug.c`

**Purpose:** Safety system debugging

**Usage:**
```bash
# Enable safety debug
export SAFETY_DEBUG=1

# Debug E-Stop events
./tools/safety_debug --estop --events

# Debug safety circuit
./tools/safety_debug --circuit --detailed

# Debug fault detection
./tools/safety_debug --faults --analysis

# Debug state transitions
./tools/safety_debug --state --transitions
```

**Debug Features:**
- E-Stop event analysis
- Safety circuit debugging
- Fault detection analysis
- State transition tracking
- Response time analysis

### 4. Telemetry Debugger

**File:** `tools/telemetry_debug.c`

**Purpose:** Telemetry system debugging

**Usage:**
```bash
# Enable telemetry debug
export TELEMETRY_DEBUG=1

# Debug data collection
./tools/telemetry_debug --collection --step

# Debug JSON serialization
./tools/telemetry_debug --json --validate

# Debug buffer usage
./tools/telemetry_debug --buffer --detailed

# Debug performance
./tools/telemetry_debug --performance --analysis
```

**Debug Features:**
- Data collection debugging
- JSON serialization analysis
- Buffer usage tracking
- Performance analysis
- Error condition debugging

---

## Performance Analysis

### 1. Performance Monitor

**File:** `tools/performance_monitor.c`

**Purpose:** Comprehensive performance analysis

**Usage:**
```bash
# Monitor all performance metrics
./tools/performance_monitor --all

# Monitor specific component
./tools/performance_monitor --comm
./tools/performance_monitor --modules
./tools/performance_monitor --safety
./tools/performance_monitor --telemetry

# Generate performance report
./tools/performance_monitor --report --output=performance.html

# Monitor with alerts
./tools/performance_monitor --alerts --threshold=90
```

**Metrics Tracked:**
- Response times (p50, p95, p99)
- Throughput (operations/second)
- Error rates
- Memory usage
- CPU utilization
- Thread activity

### 2. Performance Benchmark

**File:** `tools/performance_benchmark.c`

**Purpose:** Performance benchmarking

**Usage:**
```bash
# Run communication benchmark
./tools/performance_benchmark --comm --duration=60

# Run module benchmark
./tools/performance_benchmark --modules --duration=60

# Run safety benchmark
./tools/performance_benchmark --safety --duration=60

# Run telemetry benchmark
./tools/performance_benchmark --telemetry --duration=60

# Generate benchmark report
./tools/performance_benchmark --report --output=benchmark.html
```

**Benchmark Tests:**
- Communication throughput
- Module discovery speed
- Safety response time
- Telemetry serialization speed
- Memory efficiency
- CPU efficiency

### 3. Resource Monitor

**File:** `tools/resource_monitor.c`

**Purpose:** System resource monitoring

**Usage:**
```bash
# Monitor memory usage
./tools/resource_monitor --memory --detailed

# Monitor CPU usage
./tools/resource_monitor --cpu --per-thread

# Monitor file descriptors
./tools/resource_monitor --fds --limit=1000

# Monitor network usage
./tools/resource_monitor --network --interfaces

# Generate resource report
./tools/resource_monitor --report --output=resources.html
```

**Resource Metrics:**
- Memory usage (heap, stack, shared)
- CPU usage (per thread, per core)
- File descriptor usage
- Network interface usage
- Disk I/O usage

---

## Troubleshooting Scripts

### 1. System Health Check

**File:** `scripts/health_check.sh`

**Purpose:** Comprehensive system health check

**Usage:**
```bash
# Run full health check
./scripts/health_check.sh --full

# Run quick health check
./scripts/health_check.sh --quick

# Check specific component
./scripts/health_check.sh --comm
./scripts/health_check.sh --modules
./scripts/health_check.sh --safety
./scripts/health_check.sh --telemetry

# Generate health report
./scripts/health_check.sh --report --output=health.html
```

**Health Checks:**
- Hardware status
- Software status
- Communication status
- Module status
- Safety status
- Telemetry status

### 2. Problem Diagnosis

**File:** `scripts/diagnose.sh`

**Purpose:** Automated problem diagnosis

**Usage:**
```bash
# Diagnose communication issues
./scripts/diagnose.sh --comm

# Diagnose module issues
./scripts/diagnose.sh --modules

# Diagnose safety issues
./scripts/diagnose.sh --safety

# Diagnose telemetry issues
./scripts/diagnose.sh --telemetry

# Full system diagnosis
./scripts/diagnose.sh --full
```

**Diagnosis Features:**
- Symptom analysis
- Root cause identification
- Solution recommendations
- Automated fixes
- Report generation

### 3. Recovery Scripts

**File:** `scripts/recovery.sh`

**Purpose:** Automated recovery procedures

**Usage:**
```bash
# Recover communication
./scripts/recovery.sh --comm

# Recover modules
./scripts/recovery.sh --modules

# Recover safety system
./scripts/recovery.sh --safety

# Recover telemetry
./scripts/recovery.sh --telemetry

# Full system recovery
./scripts/recovery.sh --full
```

**Recovery Procedures:**
- Service restart
- Configuration reset
- State recovery
- Error clearing
- System restart

---

## Integration Testing

### 1. End-to-End Test

**File:** `tests/test_end_to_end.c`

**Purpose:** Complete system integration testing

**Usage:**
```bash
# Run end-to-end test
./tests/test_end_to_end

# Run with specific scenario
./tests/test_end_to_end --scenario=normal_operation
./tests/test_end_to_end --scenario=emergency_stop
./tests/test_end_to_end --scenario=module_failure
./tests/test_end_to_end --scenario=communication_failure

# Run with performance monitoring
./tests/test_end_to_end --performance --duration=300
```

**Test Scenarios:**
- Normal operation flow
- Emergency stop procedures
- Module failure handling
- Communication failure recovery
- Performance under load

### 2. Stress Test

**File:** `tests/test_stress.c`

**Purpose:** System stress testing

**Usage:**
```bash
# Run stress test
./tests/test_stress --duration=3600

# Run with specific load
./tests/test_stress --load=high --duration=1800

# Run with error injection
./tests/test_stress --errors --frequency=10

# Run with resource constraints
./tests/test_stress --memory=512 --cpu=50
```

**Stress Test Features:**
- High load testing
- Error injection
- Resource constraint testing
- Long duration testing
- Performance degradation analysis

### 3. Regression Test

**File:** `tests/test_regression.c`

**Purpose:** Regression testing

**Usage:**
```bash
# Run regression test suite
./tests/test_regression

# Run specific test category
./tests/test_regression --category=communication
./tests/test_regression --category=modules
./tests/test_regression --category=safety
./tests/test_regression --category=telemetry

# Run with baseline comparison
./tests/test_regression --baseline=previous_version
```

**Regression Test Features:**
- Automated test execution
- Baseline comparison
- Performance regression detection
- Functional regression detection
- Report generation

---

## Tool Installation and Setup

### Prerequisites

```bash
# Install required packages
sudo apt-get update
sudo apt-get install build-essential cmake libpthread-stubs0-dev

# Install development tools
sudo apt-get install valgrind gdb strace ltrace

# Install monitoring tools
sudo apt-get install htop iotop iftop
```

### Building Tools

```bash
# Build all tools
cd firmware_new
mkdir -p build
cd build
cmake ..
make tools

# Build specific tool
make comm_monitor
make module_monitor
make safety_monitor
make telemetry_monitor
```

### Configuration

```bash
# Set up environment variables
export OHT_DEBUG=1
export COMM_DEBUG=1
export MODULE_DEBUG=1
export SAFETY_DEBUG=1
export TELEMETRY_DEBUG=1

# Set up log directories
mkdir -p /var/log/oht
chmod 755 /var/log/oht
```

---

## Usage Examples

### Daily Monitoring

```bash
# Start daily monitoring
./tools/manager_monitor --all --refresh=5s --output=/var/log/oht/daily.log &

# Monitor performance
./tools/performance_monitor --all --alerts --threshold=90 &

# Check system health
./scripts/health_check.sh --full --report --output=/var/log/oht/health.html
```

### Problem Investigation

```bash
# When communication issues occur
./tools/comm_debug --frames --hex --output=comm_debug.log
./tools/comm_monitor --errors --analyze --output=comm_analysis.log
./scripts/diagnose.sh --comm --output=comm_diagnosis.html

# When module issues occur
./tools/module_debug --discovery --step --output=module_debug.log
./tools/module_monitor --health --id=1 --output=module_health.log
./scripts/diagnose.sh --modules --output=module_diagnosis.html
```

### Performance Optimization

```bash
# Run performance benchmark
./tools/performance_benchmark --all --duration=3600 --output=benchmark.html

# Analyze performance data
./tools/performance_monitor --analysis --input=benchmark.html --output=analysis.html

# Generate optimization recommendations
./tools/performance_monitor --recommendations --input=analysis.html --output=recommendations.html
```

---

## Changelog

### Version 1.0 (2025-01-28)
- ✅ Initial diagnostic tools guide
- ✅ Comprehensive monitoring tools
- ✅ Test utilities and suites
- ✅ Debug tools and analysis
- ✅ Performance analysis tools
- ✅ Troubleshooting scripts
- ✅ Integration testing framework
- ✅ Installation and setup guide

---

**Note:** This diagnostic tools guide is maintained by the firmware team and should be updated whenever new tools are added or existing tools are modified.
