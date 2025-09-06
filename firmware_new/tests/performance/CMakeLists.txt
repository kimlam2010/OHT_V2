# Performance Tests CMakeLists.txt
# ISO/IEC/IEEE 29119 Performance Tests

# Add performance benchmark test
add_executable(test_performance_benchmark test_performance_benchmark.c)

# Link with required libraries
target_link_libraries(test_performance_benchmark
    unity
    hal_common
    hal_gpio
    hal_led
)

# Add to test suite
add_test(NAME PerformanceBenchmarkTest COMMAND test_performance_benchmark)
