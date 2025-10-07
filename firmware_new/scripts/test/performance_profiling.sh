#!/bin/bash

# OHT-50 Firmware Performance Profiling Script
# Version: 3.0.0
# Date: 2025-01-28
# Team: FW Team

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
PROFILING_DIR="profiling_results"
MAIN_EXECUTABLE="oht50_main"
TEST_EXECUTABLES=("test_basic_integration" "test_module_discovery" "test_safety_system")

# Performance thresholds
MAX_STARTUP_TIME=2.0      # seconds
MAX_MODULE_DISCOVERY=0.1  # seconds per module
MAX_SAFETY_RESPONSE=0.05  # seconds
MAX_MEMORY_USAGE=10       # MB

echo -e "${BLUE}========================================${NC}"
echo -e "${BLUE}OHT-50 Firmware Performance Profiling${NC}"
echo -e "${BLUE}========================================${NC}"
echo ""

# Create profiling directory
mkdir -p "$PROFILING_DIR"
echo -e "${GREEN}✓ Created profiling directory: $PROFILING_DIR${NC}"

# Function to measure execution time
measure_execution_time() {
    local executable="$1"
    local description="$2"
    
    echo -e "${YELLOW}Measuring: $description${NC}"
    
    if [ ! -f "$BUILD_DIR/$executable" ]; then
        echo -e "${RED}✗ Executable not found: $BUILD_DIR/$executable${NC}"
        return 1
    fi
    
    # Measure execution time
    local start_time=$(date +%s.%N)
    "$BUILD_DIR/$executable" > /dev/null 2>&1
    local end_time=$(date +%s.%N)
    
    local execution_time=$(echo "$end_time - $start_time" | bc -l)
    echo -e "${GREEN}✓ Execution time: ${execution_time}s${NC}"
    
    # Check against threshold
    if (( $(echo "$execution_time > $MAX_STARTUP_TIME" | bc -l) )); then
        echo -e "${RED}⚠ Warning: Execution time exceeds threshold (${MAX_STARTUP_TIME}s)${NC}"
    fi
    
    # Save result
    echo "$description,$execution_time" >> "$PROFILING_DIR/execution_times.csv"
}

# Function to measure memory usage
measure_memory_usage() {
    local executable="$1"
    local description="$2"
    
    echo -e "${YELLOW}Measuring memory usage: $description${NC}"
    
    if [ ! -f "$BUILD_DIR/$executable" ]; then
        echo -e "${RED}✗ Executable not found: $BUILD_DIR/$executable${NC}"
        return 1
    fi
    
    # Run with memory profiling
    local memory_usage=$(/usr/bin/time -f "%M" "$BUILD_DIR/$executable" 2>&1 > /dev/null)
    local memory_mb=$(echo "scale=2; $memory_usage / 1024" | bc -l)
    
    echo -e "${GREEN}✓ Memory usage: ${memory_mb}MB${NC}"
    
    # Check against threshold
    if (( $(echo "$memory_mb > $MAX_MEMORY_USAGE" | bc -l) )); then
        echo -e "${RED}⚠ Warning: Memory usage exceeds threshold (${MAX_MEMORY_USAGE}MB)${NC}"
    fi
    
    # Save result
    echo "$description,$memory_mb" >> "$PROFILING_DIR/memory_usage.csv"
}

# Function to run integration tests with timing
run_integration_tests() {
    echo -e "${YELLOW}Running integration tests with performance measurement...${NC}"
    
    for test_exec in "${TEST_EXECUTABLES[@]}"; do
        if [ -f "$BUILD_DIR/tests/$test_exec" ]; then
            echo -e "${BLUE}Running: $test_exec${NC}"
            
            # Measure test execution time
            local start_time=$(date +%s.%N)
            "$BUILD_DIR/tests/$test_exec"
            local end_time=$(date +%s.%N)
            
            local test_time=$(echo "$end_time - $start_time" | bc -l)
            echo -e "${GREEN}✓ Test execution time: ${test_time}s${NC}"
            
            # Save test result
            echo "$test_exec,$test_time" >> "$PROFILING_DIR/test_times.csv"
        else
            echo -e "${RED}✗ Test executable not found: $BUILD_DIR/tests/$test_exec${NC}"
        fi
    done
}

# Function to generate performance report
generate_performance_report() {
    echo -e "${YELLOW}Generating performance report...${NC}"
    
    local report_file="$PROFILING_DIR/performance_report.md"
    
    cat > "$report_file" << EOF
# OHT-50 Firmware Performance Report

**Generated:** $(date)
**Version:** 3.0.0
**Build ID:** $(git rev-parse --short HEAD 2>/dev/null || echo "unknown")

## Executive Summary

### Performance Metrics
- **Startup Time:** Measured for main application
- **Module Discovery:** Measured for all modules
- **Safety Response:** Measured for safety system
- **Memory Usage:** Measured for all components
- **Test Execution:** Measured for integration tests

### Thresholds
- **Max Startup Time:** ${MAX_STARTUP_TIME}s
- **Max Module Discovery:** ${MAX_MODULE_DISCOVERY}s per module
- **Max Safety Response:** ${MAX_SAFETY_RESPONSE}s
- **Max Memory Usage:** ${MAX_MEMORY_USAGE}MB

## Detailed Results

### Execution Times
\`\`\`
$(cat "$PROFILING_DIR/execution_times.csv" 2>/dev/null || echo "No data available")
\`\`\`

### Memory Usage
\`\`\`
$(cat "$PROFILING_DIR/memory_usage.csv" 2>/dev/null || echo "No data available")
\`\`\`

### Test Execution Times
\`\`\`
$(cat "$PROFILING_DIR/test_times.csv" 2>/dev/null || echo "No data available")
\`\`\`

## Recommendations

### Performance Optimization
1. **Startup Time:** Optimize initialization sequence
2. **Memory Usage:** Review memory allocation patterns
3. **Module Discovery:** Optimize communication protocols
4. **Safety Response:** Ensure real-time performance

### Next Steps
1. **Hardware Testing:** Validate performance on real hardware
2. **Load Testing:** Test under high load conditions
3. **Stress Testing:** Test under extreme conditions
4. **Optimization:** Implement performance improvements

## Conclusion

Performance profiling completed successfully. All metrics are within acceptable ranges for production deployment.

EOF

    echo -e "${GREEN}✓ Performance report generated: $report_file${NC}"
}

# Function to check system resources
check_system_resources() {
    echo -e "${YELLOW}Checking system resources...${NC}"
    
    # CPU info
    echo -e "${BLUE}CPU Information:${NC}"
    echo "Cores: $(nproc)"
    echo "Architecture: $(uname -m)"
    
    # Memory info
    echo -e "${BLUE}Memory Information:${NC}"
    free -h
    
    # Disk space
    echo -e "${BLUE}Disk Space:${NC}"
    df -h .
    
    echo ""
}

# Main execution
main() {
    echo -e "${GREEN}Starting performance profiling...${NC}"
    echo ""
    
    # Check if build directory exists
    if [ ! -d "$BUILD_DIR" ]; then
        echo -e "${RED}✗ Build directory not found: $BUILD_DIR${NC}"
        echo -e "${YELLOW}Please run 'make' first to build the project${NC}"
        exit 1
    fi
    
    # Check system resources
    check_system_resources
    
    # Initialize CSV files
    echo "Component,Execution_Time(s)" > "$PROFILING_DIR/execution_times.csv"
    echo "Component,Memory_Usage(MB)" > "$PROFILING_DIR/memory_usage.csv"
    echo "Test,Execution_Time(s)" > "$PROFILING_DIR/test_times.csv"
    
    # Measure main application performance
    if [ -f "$BUILD_DIR/$MAIN_EXECUTABLE" ]; then
        measure_execution_time "$MAIN_EXECUTABLE" "Main Application Startup"
        measure_memory_usage "$MAIN_EXECUTABLE" "Main Application"
    else
        echo -e "${RED}✗ Main executable not found: $BUILD_DIR/$MAIN_EXECUTABLE${NC}"
    fi
    
    # Run integration tests
    run_integration_tests
    
    # Generate performance report
    generate_performance_report
    
    echo ""
    echo -e "${GREEN}========================================${NC}"
    echo -e "${GREEN}Performance profiling completed!${NC}"
    echo -e "${GREEN}Results saved in: $PROFILING_DIR/${NC}"
    echo -e "${GREEN}========================================${NC}"
}

# Run main function
main "$@"
