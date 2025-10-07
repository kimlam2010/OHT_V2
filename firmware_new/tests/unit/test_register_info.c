/**
 * @file test_register_info.c
 * @brief Unit tests for Register Info functionality (Issue #179)
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "register_info.h"
#include "register_map.h"

// Test counters
static int tests_passed = 0;
static int tests_failed = 0;

#define ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("✅ PASS: %s\n", message); \
            tests_passed++; \
        } else { \
            printf("❌ FAIL: %s\n", message); \
            tests_failed++; \
        } \
    } while(0)

// Test register info retrieval
void test_get_register_info(void) {
    printf("\n🔍 Testing get_register_info()...\n");
    
    // Test valid power module register
    const register_info_t* info = get_register_info(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE);
    ASSERT(info != NULL, "Power module battery voltage register info");
    ASSERT(info->address == POWER_REG_BATTERY_VOLTAGE, "Register address match");
    ASSERT(info->mode == REG_MODE_READ_ONLY, "Battery voltage is read-only");
    ASSERT(info->data_type == REG_DATA_TYPE_UINT16, "Battery voltage is UINT16");
    ASSERT(info->access_level == REG_ACCESS_USER, "Battery voltage is user accessible");
    
    // Test valid safety module register
    info = get_register_info(MODULE_ADDR_SAFETY, SAFETY_EMERGENCY_STOP_REG);
    ASSERT(info != NULL, "Safety module emergency stop register info");
    ASSERT(info->address == SAFETY_EMERGENCY_STOP_REG, "Safety register address match");
    ASSERT(info->is_safe_register == true, "Emergency stop is safety critical");
    
    // Test invalid module address
    info = get_register_info(0xFF, 0x0000);
    ASSERT(info == NULL, "Invalid module address returns NULL");
    
    // Test invalid register address
    info = get_register_info(MODULE_ADDR_POWER, 0xFFFF);
    ASSERT(info == NULL, "Invalid register address returns NULL");
}

// Test register access validation
void test_validate_register_access(void) {
    printf("\n🔒 Testing validate_register_access()...\n");
    
    // Test user read access to battery voltage (should succeed)
    bool result = validate_register_access(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE, REG_MODE_READ_ONLY, REG_ACCESS_USER);
    ASSERT(result == true, "User can read battery voltage");
    
    // Test user write access to battery voltage (should fail)
    result = validate_register_access(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE, REG_MODE_WRITE_ONLY, REG_ACCESS_USER);
    ASSERT(result == false, "User cannot write battery voltage");
    
    // Test admin write access to device ID (should succeed)
    result = validate_register_access(MODULE_ADDR_POWER, POWER_REG_DEVICE_ID, REG_MODE_WRITE_ONLY, REG_ACCESS_ADMIN);
    ASSERT(result == true, "Admin can write device ID");
    
    // Test user write access to device ID (should fail)
    result = validate_register_access(MODULE_ADDR_POWER, POWER_REG_DEVICE_ID, REG_MODE_WRITE_ONLY, REG_ACCESS_USER);
    ASSERT(result == false, "User cannot write device ID");
    
    // Test system access to safety critical register (should succeed)
    result = validate_register_access(MODULE_ADDR_SAFETY, SAFETY_EMERGENCY_STOP_REG, REG_MODE_READ_ONLY, REG_ACCESS_SYSTEM);
    ASSERT(result == true, "System can read emergency stop");
}

// Test register value validation
void test_validate_register_value(void) {
    printf("\n📊 Testing validate_register_value()...\n");
    
    // Test valid battery voltage value (in range)
    bool result = is_register_value_valid(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE, 5000); // 500.0V
    ASSERT(result == true, "Valid battery voltage value");
    
    // Test invalid battery voltage value (out of range)
    result = is_register_value_valid(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE, 0xFFFF);
    ASSERT(result == false, "Invalid battery voltage value");
    
    // Test valid device ID value (in range 1-247)
    result = is_register_value_valid(MODULE_ADDR_POWER, POWER_REG_DEVICE_ID, 100);
    ASSERT(result == true, "Valid device ID value");
    
    // Test invalid device ID value (out of range)
    result = is_register_value_valid(MODULE_ADDR_POWER, POWER_REG_DEVICE_ID, 0);
    ASSERT(result == false, "Invalid device ID value");
}

// Test register mode validation
void test_register_mode_validation(void) {
    printf("\n⚙️ Testing register mode validation...\n");
    
    // Test valid modes
    ASSERT(is_valid_register_mode(REG_MODE_READ_ONLY) == true, "Valid read-only mode");
    ASSERT(is_valid_register_mode(REG_MODE_WRITE_ONLY) == true, "Valid write-only mode");
    ASSERT(is_valid_register_mode(REG_MODE_READ_WRITE) == true, "Valid read-write mode");
    ASSERT(is_valid_register_mode(REG_MODE_WRITE_ONCE) == true, "Valid write-once mode");
    
    // Test invalid mode
    ASSERT(is_valid_register_mode(0xFF) == false, "Invalid register mode");
}

// Test access level validation
void test_access_level_validation(void) {
    printf("\n🔐 Testing access level validation...\n");
    
    // Test valid access levels
    ASSERT(is_valid_access_level(REG_ACCESS_USER) == true, "Valid user access level");
    ASSERT(is_valid_access_level(REG_ACCESS_ADMIN) == true, "Valid admin access level");
    ASSERT(is_valid_access_level(REG_ACCESS_SYSTEM) == true, "Valid system access level");
    
    // Test invalid access level
    ASSERT(is_valid_access_level(0) == false, "Invalid access level (too low)");
    ASSERT(is_valid_access_level(4) == false, "Invalid access level (too high)");
}

// Test data type validation
void test_data_type_validation(void) {
    printf("\n📝 Testing data type validation...\n");
    
    // Test valid data types
    ASSERT(is_valid_data_type(REG_DATA_TYPE_UINT8) == true, "Valid UINT8 data type");
    ASSERT(is_valid_data_type(REG_DATA_TYPE_UINT16) == true, "Valid UINT16 data type");
    ASSERT(is_valid_data_type(REG_DATA_TYPE_UINT32) == true, "Valid UINT32 data type");
    ASSERT(is_valid_data_type(REG_DATA_TYPE_INT16) == true, "Valid INT16 data type");
    ASSERT(is_valid_data_type(REG_DATA_TYPE_FLOAT) == true, "Valid FLOAT data type");
    
    // Test invalid data type
    ASSERT(is_valid_data_type(0xFF) == false, "Invalid data type");
}

// Test register read/write support
void test_register_support_validation(void) {
    printf("\n🔄 Testing register support validation...\n");
    
    // Test read support
    ASSERT(register_supports_read(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE) == true, "Battery voltage supports read");
    ASSERT(register_supports_read(MODULE_ADDR_POWER, POWER_REG_DEVICE_ID) == true, "Device ID supports read");
    
    // Test write support
    ASSERT(register_supports_write(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE) == false, "Battery voltage does not support write");
    ASSERT(register_supports_write(MODULE_ADDR_POWER, POWER_REG_DEVICE_ID) == true, "Device ID supports write");
    ASSERT(register_supports_write(MODULE_ADDR_POWER, POWER_REG_RESET_ERROR_CMD) == true, "Reset error command supports write");
}

// Test safety critical register detection
void test_safety_critical_detection(void) {
    printf("\n⚠️ Testing safety critical register detection...\n");
    
    // Test safety critical registers
    ASSERT(is_register_safe_critical(MODULE_ADDR_SAFETY, SAFETY_EMERGENCY_STOP_REG) == true, "Emergency stop is safety critical");
    ASSERT(is_register_safe_critical(MODULE_ADDR_POWER, POWER_REG_RESET_ERROR_CMD) == true, "Reset error command is safety critical");
    
    // Test non-safety critical registers
    ASSERT(is_register_safe_critical(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE) == false, "Battery voltage is not safety critical");
    ASSERT(is_register_safe_critical(MODULE_ADDR_POWER, POWER_REG_DEVICE_ID) == false, "Device ID is not safety critical");
}

// Test register info summary functions
void test_register_info_summary(void) {
    printf("\n📋 Testing register info summary functions...\n");
    
    // Test description retrieval
    const char* desc = get_register_description(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE);
    ASSERT(desc != NULL && strlen(desc) > 0, "Battery voltage description exists");
    
    // Test unit retrieval
    const char* unit = get_register_unit(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE);
    ASSERT(unit != NULL && strcmp(unit, "V") == 0, "Battery voltage unit is 'V'");
    
    // Test min/max value retrieval
    uint16_t min_val = get_register_min_value(MODULE_ADDR_POWER, POWER_REG_DEVICE_ID);
    uint16_t max_val = get_register_max_value(MODULE_ADDR_POWER, POWER_REG_DEVICE_ID);
    ASSERT(min_val == 1 && max_val == 247, "Device ID range is 1-247");
    
    // Test default value retrieval
    uint16_t default_val = get_register_default_value(MODULE_ADDR_POWER, POWER_REG_DEVICE_ID);
    ASSERT(default_val == 2, "Device ID default value is 2");
    
    // Test data type retrieval
    uint8_t data_type = get_register_data_type(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE);
    ASSERT(data_type == REG_DATA_TYPE_UINT16, "Battery voltage data type is UINT16");
}

// Main test function
int main(void) {
    printf("🧪 Register Info Unit Tests (Issue #179)\n");
    printf("========================================\n");
    
    test_get_register_info();
    test_validate_register_access();
    test_validate_register_value();
    test_register_mode_validation();
    test_access_level_validation();
    test_data_type_validation();
    test_register_support_validation();
    test_safety_critical_detection();
    test_register_info_summary();
    
    printf("\n📊 Test Results:\n");
    printf("✅ Passed: %d\n", tests_passed);
    printf("❌ Failed: %d\n", tests_failed);
    printf("📈 Total: %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed! Register Info implementation is working correctly.\n");
        return 0;
    } else {
        printf("\n💥 Some tests failed! Please check the implementation.\n");
        return 1;
    }
}
