#!/bin/bash

# OHT-50 Firmware Test Setup Script
# Unity Framework Setup

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== OHT-50 Firmware Test Setup ===${NC}"

# Create test directory structure
echo -e "${GREEN}Creating test directory structure...${NC}"
mkdir -p tests/unit/hal
mkdir -p tests/unit/app
mkdir -p tests/integration
mkdir -p tests/system
mkdir -p tests/mocks

# Create Unity test runner
echo -e "${GREEN}Creating Unity test runner...${NC}"
cat > tests/unity_test_runner.c << 'EOF'
#include "unity.h"
#include <stdio.h>
#include <stdlib.h>

// Test function declarations
extern void test_hal_common(void);
extern void test_hal_gpio(void);
extern void test_hal_communication(void);
extern void test_hal_safety(void);
extern void test_hal_peripherals(void);
extern void test_app_core(void);
extern void test_app_managers(void);
extern void test_app_modules(void);
extern void test_app_api(void);

int main(void) {
    UNITY_BEGIN();
    
    printf("Running HAL Tests...\n");
    RUN_TEST(test_hal_common);
    RUN_TEST(test_hal_gpio);
    RUN_TEST(test_hal_communication);
    RUN_TEST(test_hal_safety);
    RUN_TEST(test_hal_peripherals);
    
    printf("Running Application Tests...\n");
    RUN_TEST(test_app_core);
    RUN_TEST(test_app_managers);
    RUN_TEST(test_app_modules);
    RUN_TEST(test_app_api);
    
    return UNITY_END();
}
EOF

# Create HAL unit tests
echo -e "${GREEN}Creating HAL unit tests...${NC}"

# HAL Common tests
cat > tests/unit/hal/test_hal_common.c << 'EOF'
#include "unity.h"
#include "hal_common.h"

void setUp(void) {
    // Setup code before each test
}

void tearDown(void) {
    // Cleanup code after each test
}

void test_hal_get_timestamp_us(void) {
    uint64_t timestamp1 = hal_get_timestamp_us();
    uint64_t timestamp2 = hal_get_timestamp_us();
    
    TEST_ASSERT_GREATER_THAN(0, timestamp1);
    TEST_ASSERT_GREATER_OR_EQUAL(timestamp1, timestamp2);
}

void test_hal_get_timestamp_ms(void) {
    uint64_t timestamp1 = hal_get_timestamp_ms();
    uint64_t timestamp2 = hal_get_timestamp_ms();
    
    TEST_ASSERT_GREATER_THAN(0, timestamp1);
    TEST_ASSERT_GREATER_OR_EQUAL(timestamp1, timestamp2);
}

void test_hal_status_to_string(void) {
    const char* status_str = hal_status_to_string(HAL_STATUS_OK);
    TEST_ASSERT_NOT_NULL(status_str);
    TEST_ASSERT_EQUAL_STRING("OK", status_str);
    
    status_str = hal_status_to_string(HAL_STATUS_ERROR);
    TEST_ASSERT_NOT_NULL(status_str);
    TEST_ASSERT_EQUAL_STRING("ERROR", status_str);
}
EOF

# HAL GPIO tests
cat > tests/unit/hal/test_hal_gpio.c << 'EOF'
#include "unity.h"
#include "hal_gpio.h"

void setUp(void) {
    // Setup code before each test
}

void tearDown(void) {
    // Cleanup code after each test
}

void test_hal_gpio_init(void) {
    hal_status_t status = hal_gpio_init();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_gpio_deinit(void) {
    hal_status_t status = hal_gpio_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}
EOF

# HAL Communication tests
cat > tests/unit/hal/test_hal_communication.c << 'EOF'
#include "unity.h"
#include "hal_rs485.h"

void setUp(void) {
    // Setup code before each test
}

void tearDown(void) {
    // Cleanup code after each test
}

void test_hal_rs485_init(void) {
    rs485_config_t config = {
        .device_path = "/dev/ttyOHT485",
        .baud_rate = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 0,
        .timeout_ms = 1000,
        .retry_count = 3
    };
    
    hal_status_t status = hal_rs485_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_rs485_deinit(void) {
    hal_status_t status = hal_rs485_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}
EOF

# HAL Safety tests
cat > tests/unit/hal/test_hal_safety.c << 'EOF'
#include "unity.h"
#include "hal_estop.h"

void setUp(void) {
    // Setup code before each test
}

void tearDown(void) {
    // Cleanup code after each test
}

void test_hal_estop_init(void) {
    estop_config_t config = {
        .channel1_pin = 6,
        .channel2_pin = 0,
        .response_timeout_ms = 100,
        .debounce_time_ms = 50,
        .dual_channel_required = false,
        .auto_reset_enabled = false
    };
    
    hal_status_t status = hal_estop_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_estop_deinit(void) {
    hal_status_t status = hal_estop_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}
EOF

# HAL Peripherals tests
cat > tests/unit/hal/test_hal_peripherals.c << 'EOF'
#include "unity.h"
#include "hal_led.h"
#include "hal_relay.h"

void setUp(void) {
    // Setup code before each test
}

void tearDown(void) {
    // Cleanup code after each test
}

void test_hal_led_init(void) {
    hal_status_t status = hal_led_init();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_led_deinit(void) {
    hal_status_t status = hal_led_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_relay_init(void) {
    relay_config_t config = {
        .output_pin = 1,
        .voltage_v = 24,
        .current_max_a = 2,
        .pulse_duration_ms = 100,
        .pulse_interval_ms = 1000,
        .overcurrent_protection = true,
        .overtemperature_protection = true
    };
    
    hal_status_t status = hal_relay_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_hal_relay_deinit(void) {
    hal_status_t status = hal_relay_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}
EOF

# Application tests
echo -e "${GREEN}Creating Application unit tests...${NC}"

# App Core tests
cat > tests/unit/app/test_app_core.c << 'EOF'
#include "unity.h"
#include "system_state_machine.h"

void setUp(void) {
    // Setup code before each test
}

void tearDown(void) {
    // Cleanup code after each test
}

void test_system_state_machine_init(void) {
    system_config_t config = {
        .state_timeout_ms = 5000,
        .update_period_ms = 50,
        .auto_recovery_enabled = true,
        .safety_monitoring_enabled = true,
        .communication_monitoring_enabled = true,
        .sensor_monitoring_enabled = true
    };
    
    hal_status_t status = system_state_machine_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_system_state_machine_deinit(void) {
    hal_status_t status = system_state_machine_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}
EOF

# App Managers tests
cat > tests/unit/app/test_app_managers.c << 'EOF'
#include "unity.h"
#include "safety_manager.h"
#include "communication_manager.h"
#include "module_manager.h"

void setUp(void) {
    // Setup code before each test
}

void tearDown(void) {
    // Cleanup code after each test
}

void test_safety_manager_init(void) {
    safety_config_t config = {
        .safety_check_interval_ms = 50,
        .estop_response_timeout_ms = 100,
        .safety_circuit_timeout_ms = 100,
        .sensor_timeout_ms = 100,
        .enable_auto_recovery = true,
        .enable_safety_monitoring = true,
        .enable_estop_monitoring = true,
        .enable_sensor_monitoring = true
    };
    
    hal_status_t status = safety_manager_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_communication_manager_init(void) {
    comm_mgr_config_t config = {
        .baud_rate = 115200,
        .data_bits = 8,
        .stop_bits = 1,
        .parity = 0,
        .timeout_ms = 1500,
        .retry_count = 2,
        .retry_delay_ms = 100,
        .modbus_slave_id = 1,
        .enable_crc_check = false,
        .enable_echo_suppression = true,
        .buffer_size = 256
    };
    
    hal_status_t status = comm_manager_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_module_manager_init(void) {
    hal_status_t status = module_manager_init();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}
EOF

# App Modules tests
cat > tests/unit/app/test_app_modules.c << 'EOF'
#include "unity.h"
#include "power_module_handler.h"
#include "motor_module_handler.h"
#include "dock_module_handler.h"

void setUp(void) {
    // Setup code before each test
}

void tearDown(void) {
    // Cleanup code after each test
}

void test_power_module_handler_init(void) {
    power_module_config_t config = {0};
    hal_status_t status = power_module_handler_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_motor_module_handler_init(void) {
    motor_module_config_t config = {
        .address = 0x03,
        .command_timeout_ms = 1000,
        .response_timeout_ms = 500,
        .default_velocity = 100,
        .default_acceleration = 50,
        .default_jerk = 10,
        .enable_safety_checks = true,
        .enable_position_limits = true,
        .enable_velocity_limits = true,
        .enable_acceleration_limits = true
    };
    
    motor_module_handler_t handler;
    hal_status_t status = motor_module_init(&handler, &config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_dock_module_handler_init(void) {
    dock_module_handler_t handler;
    hal_status_t status = dock_module_init(&handler, 0x05);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}
EOF

# App API tests
cat > tests/unit/app/test_app_api.c << 'EOF'
#include "unity.h"
#include "api_manager.h"

void setUp(void) {
    // Setup code before each test
}

void tearDown(void) {
    // Cleanup code after each test
}

void test_api_manager_init(void) {
    api_mgr_config_t config = {
        .http_port = 8080,
        .websocket_port = 8081,
        .http_enabled = true,
        .websocket_enabled = true,
        .cors_enabled = true,
        .cors_origin = "*",
        .max_request_size = 8192,
        .max_response_size = 16384,
        .request_timeout_ms = 30000,
        .websocket_timeout_ms = 60000,
        .authentication_required = false,
        .ssl_enabled = false,
        .ssl_certificate_path = "",
        .ssl_private_key_path = ""
    };
    
    hal_status_t status = api_manager_init(&config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}
EOF

# Create test CMakeLists.txt
echo -e "${GREEN}Creating test CMakeLists.txt...${NC}"
cat > tests/CMakeLists.txt << 'EOF'
# Test Configuration
# Unity Framework Setup

# Find Unity framework
find_package(Unity REQUIRED)

# Create test executable
add_executable(oht50_tests
    unity_test_runner.c
    unit/hal/test_hal_common.c
    unit/hal/test_hal_gpio.c
    unit/hal/test_hal_communication.c
    unit/hal/test_hal_safety.c
    unit/hal/test_hal_peripherals.c
    unit/app/test_app_core.c
    unit/app/test_app_managers.c
    unit/app/test_app_modules.c
    unit/app/test_app_api.c
)

# Link libraries
target_link_libraries(oht50_tests
    Unity::Unity
    hal_common
    hal_gpio
    hal_communication
    hal_safety
    hal_peripherals
    app_core
    app_managers
    app_modules
    app_api
)

# Include directories
target_include_directories(oht50_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/common
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/gpio
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/communication
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/safety
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/hal/peripherals
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/core
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/managers
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/modules
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/app/api
)

# Add test
add_test(NAME OHT50_Unit_Tests COMMAND oht50_tests)
EOF

# Create test runner script
echo -e "${GREEN}Creating test runner script...${NC}"
cat > tests/run_tests.sh << 'EOF'
#!/bin/bash

# OHT-50 Firmware Test Runner

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}=== OHT-50 Firmware Test Runner ===${NC}"

# Build tests
echo -e "${GREEN}Building tests...${NC}"
cd ../build
make oht50_tests

# Run tests
echo -e "${GREEN}Running tests...${NC}"
./oht50_tests

echo -e "${GREEN}Tests completed successfully!${NC}"
EOF

chmod +x tests/run_tests.sh

echo -e "${GREEN}=== Test Setup Complete ===${NC}"
echo -e "Test structure created in: ${YELLOW}tests/${NC}"
echo -e "Run tests with: ${YELLOW}cd tests && ./run_tests.sh${NC}"
echo -e "Or build and run: ${YELLOW}cd build && make oht50_tests && ./oht50_tests${NC}"
