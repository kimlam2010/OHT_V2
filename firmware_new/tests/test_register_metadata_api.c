/**
 * @file test_register_metadata_api.c
 * @brief Unit Tests for Register Metadata API (Issue #203)
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 * @task Issue #203 - Unit Testing
 */

#include "../src/hal/register/register_info.h"
#include "../src/app/utils/register_json_serializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

// ============================================================================
// TEST UTILITIES
// ============================================================================

static int g_test_count = 0;
static int g_test_passed = 0;
static int g_test_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        g_test_count++; \
        if (condition) { \
            g_test_passed++; \
            printf("  [✓] PASS: %s\n", message); \
        } else { \
            g_test_failed++; \
            printf("  [✗] FAIL: %s\n", message); \
        } \
    } while(0)

#define TEST_SECTION(name) \
    printf("\n🧪 TEST SECTION: %s\n", name)

// ============================================================================
// TEST CASES: Register Info API
// ============================================================================

void test_get_module_registers_array(void) {
    TEST_SECTION("get_module_registers_array()");
    
    // Test Power Module
    uint16_t power_count = 0;
    const register_info_t* power_regs = get_module_registers_array(MODULE_ADDR_POWER, &power_count);
    TEST_ASSERT(power_regs != NULL, "Power module registers should not be NULL");
    TEST_ASSERT(power_count > 0, "Power module should have > 0 registers");
    TEST_ASSERT(power_count >= 51, "Power module should have >= 51 registers");
    
    // Test Safety Module
    uint16_t safety_count = 0;
    const register_info_t* safety_regs = get_module_registers_array(MODULE_ADDR_SAFETY, &safety_count);
    TEST_ASSERT(safety_regs != NULL, "Safety module registers should not be NULL");
    TEST_ASSERT(safety_count > 0, "Safety module should have > 0 registers");
    TEST_ASSERT(safety_count >= 30, "Safety module should have >= 30 registers");
    
    // Test Motor Module
    uint16_t motor_count = 0;
    const register_info_t* motor_regs = get_module_registers_array(MODULE_ADDR_TRAVEL_MOTOR, &motor_count);
    TEST_ASSERT(motor_regs != NULL, "Motor module registers should not be NULL");
    TEST_ASSERT(motor_count > 0, "Motor module should have > 0 registers");
    TEST_ASSERT(motor_count >= 25, "Motor module should have >= 25 registers");
    
    // Test Dock Module
    uint16_t dock_count = 0;
    const register_info_t* dock_regs = get_module_registers_array(MODULE_ADDR_DOCK, &dock_count);
    TEST_ASSERT(dock_regs != NULL, "Dock module registers should not be NULL");
    TEST_ASSERT(dock_count > 0, "Dock module should have > 0 registers");
    TEST_ASSERT(dock_count >= 40, "Dock module should have >= 40 registers");
    
    // Test Invalid Module
    uint16_t invalid_count = 0;
    const register_info_t* invalid_regs = get_module_registers_array(0x99, &invalid_count);
    TEST_ASSERT(invalid_regs == NULL, "Invalid module should return NULL");
    TEST_ASSERT(invalid_count == 0, "Invalid module should have 0 registers");
}

void test_get_module_register_list_api(void) {
    TEST_SECTION("get_module_register_list_api()");
    
    // Test Power Module
    const register_list_response_t* power_list = get_module_register_list_api(MODULE_ADDR_POWER);
    TEST_ASSERT(power_list != NULL, "Power module list should not be NULL");
    TEST_ASSERT(power_list->valid == true, "Power module list should be valid");
    TEST_ASSERT(power_list->module_addr == MODULE_ADDR_POWER, "Power module address should match");
    TEST_ASSERT(power_list->module_name != NULL, "Power module name should not be NULL");
    TEST_ASSERT(strcmp(power_list->module_name, "Power Module") == 0, "Power module name should be correct");
    TEST_ASSERT(power_list->registers != NULL, "Power module registers array should not be NULL");
    TEST_ASSERT(power_list->count > 0, "Power module should have > 0 registers in list");
    
    // Test Safety Module
    const register_list_response_t* safety_list = get_module_register_list_api(MODULE_ADDR_SAFETY);
    TEST_ASSERT(safety_list != NULL, "Safety module list should not be NULL");
    TEST_ASSERT(safety_list->valid == true, "Safety module list should be valid");
    
    // Test Invalid Module
    const register_list_response_t* invalid_list = get_module_register_list_api(0x99);
    TEST_ASSERT(invalid_list == NULL, "Invalid module list should be NULL");
}

void test_get_register_info(void) {
    TEST_SECTION("get_register_info()");
    
    // Test valid Power Module register
    const register_info_t* reg = get_register_info(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE);
    TEST_ASSERT(reg != NULL, "Battery voltage register should exist");
    TEST_ASSERT(reg->address == POWER_REG_BATTERY_VOLTAGE, "Register address should match");
    TEST_ASSERT(reg->mode == REG_MODE_READ_ONLY, "Battery voltage should be READ_ONLY");
    TEST_ASSERT(reg->description != NULL, "Register description should exist");
    
    // Test valid Safety Module register
    reg = get_register_info(MODULE_ADDR_SAFETY, SAFETY_EMERGENCY_STOP_REG);
    TEST_ASSERT(reg != NULL, "E-Stop register should exist");
    TEST_ASSERT(reg->is_safe_register == true, "E-Stop should be safety-critical");
    
    // Test invalid register (use a register that doesn't exist but not end marker)
    reg = get_register_info(MODULE_ADDR_POWER, 0xFFFE);
    TEST_ASSERT(reg == NULL, "Invalid register should return NULL");
}

// ============================================================================
// TEST CASES: JSON Serializer
// ============================================================================

void test_enum_to_string_converters(void) {
    TEST_SECTION("Enum to String Converters");
    
    // Test register_mode_to_string
    TEST_ASSERT(strcmp(register_mode_to_string(REG_MODE_READ_ONLY), "READ") == 0, "READ_ONLY mode should be 'READ'");
    TEST_ASSERT(strcmp(register_mode_to_string(REG_MODE_WRITE_ONLY), "WRITE") == 0, "WRITE_ONLY mode should be 'WRITE'");
    TEST_ASSERT(strcmp(register_mode_to_string(REG_MODE_READ_WRITE), "READ_WRITE") == 0, "READ_WRITE mode should be 'READ_WRITE'");
    TEST_ASSERT(strcmp(register_mode_to_string(REG_MODE_WRITE_ONCE), "WRITE_ONCE") == 0, "WRITE_ONCE mode should be 'WRITE_ONCE'");
    
    // Test register_data_type_to_string
    TEST_ASSERT(strcmp(register_data_type_to_string(REG_DATA_TYPE_UINT8), "UINT8") == 0, "UINT8 type should be 'UINT8'");
    TEST_ASSERT(strcmp(register_data_type_to_string(REG_DATA_TYPE_UINT16), "UINT16") == 0, "UINT16 type should be 'UINT16'");
    TEST_ASSERT(strcmp(register_data_type_to_string(REG_DATA_TYPE_INT16), "INT16") == 0, "INT16 type should be 'INT16'");
    TEST_ASSERT(strcmp(register_data_type_to_string(REG_DATA_TYPE_FLOAT), "FLOAT") == 0, "FLOAT type should be 'FLOAT'");
    
    // Test register_access_level_to_string
    TEST_ASSERT(strcmp(register_access_level_to_string(REG_ACCESS_USER), "USER") == 0, "USER access should be 'USER'");
    TEST_ASSERT(strcmp(register_access_level_to_string(REG_ACCESS_ADMIN), "ADMIN") == 0, "ADMIN access should be 'ADMIN'");
    TEST_ASSERT(strcmp(register_access_level_to_string(REG_ACCESS_SYSTEM), "SYSTEM") == 0, "SYSTEM access should be 'SYSTEM'");
}

void test_serialize_register_to_json(void) {
    TEST_SECTION("serialize_register_to_json()");
    
    // Get a register
    const register_info_t* reg = get_register_info(MODULE_ADDR_POWER, POWER_REG_BATTERY_VOLTAGE);
    TEST_ASSERT(reg != NULL, "Register should exist for serialization test");
    
    if (reg) {
        // Serialize to JSON
        char* json = serialize_register_to_json(reg);
        TEST_ASSERT(json != NULL, "JSON should not be NULL");
        
        if (json) {
            // Basic validation - check for key fields
            TEST_ASSERT(strstr(json, "\"address\"") != NULL, "JSON should contain 'address' field");
            TEST_ASSERT(strstr(json, "\"mode\"") != NULL, "JSON should contain 'mode' field");
            TEST_ASSERT(strstr(json, "\"data_type\"") != NULL, "JSON should contain 'data_type' field");
            TEST_ASSERT(strstr(json, "\"description\"") != NULL, "JSON should contain 'description' field");
            TEST_ASSERT(strstr(json, "0x0000") != NULL, "JSON should contain hex address");
            
            free(json);
        }
    }
}

void test_serialize_register_list_to_json(void) {
    TEST_SECTION("serialize_register_list_to_json()");
    
    // Get register list
    const register_list_response_t* list = get_module_register_list_api(MODULE_ADDR_POWER);
    TEST_ASSERT(list != NULL, "Register list should exist for serialization test");
    
    if (list) {
        // Serialize to JSON
        char* json = serialize_register_list_to_json(list);
        TEST_ASSERT(json != NULL, "JSON should not be NULL");
        
        if (json) {
            // Basic validation
            TEST_ASSERT(strstr(json, "\"success\"") != NULL, "JSON should contain 'success' field");
            TEST_ASSERT(strstr(json, "\"data\"") != NULL, "JSON should contain 'data' field");
            TEST_ASSERT(strstr(json, "\"module_addr\"") != NULL, "JSON should contain 'module_addr' field");
            TEST_ASSERT(strstr(json, "\"module_name\"") != NULL, "JSON should contain 'module_name' field");
            TEST_ASSERT(strstr(json, "\"register_count\"") != NULL, "JSON should contain 'register_count' field");
            TEST_ASSERT(strstr(json, "\"registers\"") != NULL, "JSON should contain 'registers' array");
            TEST_ASSERT(strstr(json, "Power Module") != NULL, "JSON should contain module name");
            
            free(json);
        }
    }
}

void test_create_error_json(void) {
    TEST_SECTION("create_error_json()");
    
    char* json = create_error_json("Test error message");
    TEST_ASSERT(json != NULL, "Error JSON should not be NULL");
    
    if (json) {
        TEST_ASSERT(strstr(json, "\"success\"") != NULL, "Error JSON should contain 'success' field");
        TEST_ASSERT(strstr(json, "false") != NULL, "Error JSON should have success=false");
        TEST_ASSERT(strstr(json, "\"error\"") != NULL, "Error JSON should contain 'error' field");
        TEST_ASSERT(strstr(json, "Test error message") != NULL, "Error JSON should contain error message");
        
        free(json);
    }
}

// ============================================================================
// TEST RUNNER
// ============================================================================

int main(void) {
    printf("\n========================================\n");
    printf("🧪 REGISTER METADATA API - UNIT TESTS\n");
    printf("========================================\n");
    
    // Run tests
    test_get_module_registers_array();
    test_get_module_register_list_api();
    test_get_register_info();
    test_enum_to_string_converters();
    test_serialize_register_to_json();
    test_serialize_register_list_to_json();
    test_create_error_json();
    
    // Print results
    printf("\n========================================\n");
    printf("📊 TEST RESULTS\n");
    printf("========================================\n");
    printf("Total tests:  %d\n", g_test_count);
    printf("✓ Passed:     %d\n", g_test_passed);
    printf("✗ Failed:     %d\n", g_test_failed);
    printf("Success rate: %.1f%%\n", (g_test_passed * 100.0) / g_test_count);
    printf("========================================\n");
    
    if (g_test_failed > 0) {
        printf("\n⚠️  SOME TESTS FAILED!\n");
        return 1;
    } else {
        printf("\n✅ ALL TESTS PASSED!\n");
        return 0;
    }
}

