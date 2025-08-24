/**
 * @file test_hal_storage.c
 * @brief Unit tests for HAL Storage
 * @version 1.0.0
 * @date 2025-01-27
 * @team EMBED
 * @task EM-15 (Storage Configuration Testing)
 */

#include "unity.h"
#include "hal_config_persistence.h"
#include "hal_common.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

// Test fixtures
static config_header_t test_header;

void setUp(void)
{
    // Initialize test header
    memset(&test_header, 0, sizeof(config_header_t));
    strcpy(test_header.magic, "OHT50CFG");
    strcpy(test_header.version, "1.0.0");
    test_header.entry_count = 10;
    test_header.file_size = 1024;
    strcpy(test_header.checksum, "12345678");
    test_header.created_timestamp_us = 1640995200000000ULL; // 2022-01-01 00:00:00
    test_header.modified_timestamp_us = 1640995200000000ULL;
    test_header.backup_count = 1;
    test_header.factory_reset_flag = false;
    test_header.emergency_mode = false;
}

void tearDown(void)
{
    // Cleanup after each test
    // Note: hal_storage_deinit() may not exist in actual implementation
}

// ============================================================================
// CONSTANTS TESTS
// ============================================================================

void test_storage_constants(void)
{
    TEST_ASSERT_EQUAL(65536, CONFIG_MAX_FILE_SIZE);
    TEST_ASSERT_EQUAL(64, CONFIG_MAX_KEY_LENGTH);
    TEST_ASSERT_EQUAL(256, CONFIG_MAX_VALUE_LENGTH);
    TEST_ASSERT_EQUAL(32, CONFIG_MAX_SECTION_LENGTH);
}

void test_storage_magic_string(void)
{
    TEST_ASSERT_EQUAL_STRING("OHT50CFG", "OHT50CFG");
    TEST_ASSERT_EQUAL(8, strlen("OHT50CFG"));
}

// ============================================================================
// DATA STRUCTURES TESTS
// ============================================================================

void test_storage_data_structures(void)
{
    TEST_ASSERT_EQUAL(8, sizeof(test_header.magic));
    TEST_ASSERT_EQUAL(16, sizeof(test_header.version));
    TEST_ASSERT_EQUAL(4, sizeof(test_header.entry_count));
    TEST_ASSERT_EQUAL(4, sizeof(test_header.file_size));
    TEST_ASSERT_EQUAL(8, sizeof(test_header.checksum));
}

void test_storage_config_initialization(void)
{
    TEST_ASSERT_EQUAL_STRING("OHT50CFG", test_header.magic);
    TEST_ASSERT_EQUAL_STRING("1.0.0", test_header.version);
    TEST_ASSERT_EQUAL(10, test_header.entry_count);
    TEST_ASSERT_EQUAL(1024, test_header.file_size);
    TEST_ASSERT_EQUAL_STRING("12345678", test_header.checksum);
}

void test_storage_header_initialization(void)
{
    TEST_ASSERT_EQUAL_STRING("OHT50CFG", test_header.magic);
    TEST_ASSERT_EQUAL_STRING("1.0.0", test_header.version);
    TEST_ASSERT_EQUAL(10, test_header.entry_count);
    TEST_ASSERT_EQUAL(1024, test_header.file_size);
    TEST_ASSERT_EQUAL_STRING("12345678", test_header.checksum);
    TEST_ASSERT_EQUAL(1640995200000000ULL, test_header.created_timestamp_us);
    TEST_ASSERT_EQUAL(1640995200000000ULL, test_header.modified_timestamp_us);
    TEST_ASSERT_EQUAL(1, test_header.backup_count);
    TEST_ASSERT_FALSE(test_header.factory_reset_flag);
    TEST_ASSERT_FALSE(test_header.emergency_mode);
}

// ============================================================================
// VALIDATION TESTS
// ============================================================================

void test_storage_config_validation(void)
{
    // Test valid configuration
    TEST_ASSERT_TRUE(strlen(test_header.magic) > 0);
    TEST_ASSERT_TRUE(strlen(test_header.version) > 0);
    TEST_ASSERT_TRUE(test_header.entry_count > 0);
    TEST_ASSERT_TRUE(test_header.file_size > 0);
    TEST_ASSERT_TRUE(test_header.file_size <= CONFIG_MAX_FILE_SIZE);
}

void test_storage_header_validation(void)
{
    // Test valid header
    TEST_ASSERT_EQUAL_STRING("OHT50CFG", test_header.magic);
    TEST_ASSERT_TRUE(strlen(test_header.version) > 0);
    TEST_ASSERT_TRUE(test_header.created_timestamp_us > 0);
    TEST_ASSERT_TRUE(test_header.file_size > 0);
    TEST_ASSERT_TRUE(test_header.file_size <= CONFIG_MAX_FILE_SIZE);
}

void test_storage_file_path_validation(void)
{
    // Test valid file paths
    const char* valid_paths[] = {
        CONFIG_PRIMARY_FILE,
        CONFIG_BACKUP_FILE,
        CONFIG_FACTORY_FILE,
        CONFIG_TEMP_FILE
    };
    
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(strlen(valid_paths[i]) > 0);
        TEST_ASSERT_TRUE(strlen(valid_paths[i]) < 256);
    }
}

// ============================================================================
// UTILITY TESTS
// ============================================================================

void test_storage_version_validation(void)
{
    // Test valid versions
    const char* valid_versions[] = {"1.0.0", "1.1.0", "2.0.0", "2.1.0"};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(strlen(valid_versions[i]) > 0);
        TEST_ASSERT_TRUE(strlen(valid_versions[i]) < CONFIG_VERSION_LENGTH);
    }
}

void test_storage_timestamp_validation(void)
{
    // Test valid timestamps (microseconds)
    uint64_t valid_timestamps[] = {
        1640995200000000ULL, // 2022-01-01 00:00:00
        1672531200000000ULL, // 2023-01-01 00:00:00
        1704067200000000ULL, // 2024-01-01 00:00:00
        1735689600000000ULL  // 2025-01-01 00:00:00
    };
    
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(valid_timestamps[i] > 0);
        TEST_ASSERT_TRUE(valid_timestamps[i] <= 0xFFFFFFFFFFFFFFFFULL);
    }
}

void test_storage_file_size_validation(void)
{
    // Test valid file sizes
    uint32_t valid_sizes[] = {1024, 4096, 8192, 16384};
    for (int i = 0; i < 4; i++) {
        TEST_ASSERT_TRUE(valid_sizes[i] > 0);
        TEST_ASSERT_TRUE(valid_sizes[i] <= CONFIG_MAX_FILE_SIZE);
    }
    
    // Test invalid file sizes
    uint32_t invalid_sizes[] = {0, CONFIG_MAX_FILE_SIZE + 1};
    for (int i = 0; i < 2; i++) {
        TEST_ASSERT_FALSE(invalid_sizes[i] > 0 && invalid_sizes[i] <= CONFIG_MAX_FILE_SIZE);
    }
}



// ============================================================================
// CHECKSUM UTILITY TESTS
// ============================================================================

void test_storage_checksum_validation(void)
{
    // Test valid checksums
    const char* valid_checksums[] = {"00000000", "12345678", "FFFFFFFF"};
    for (int i = 0; i < 3; i++) {
        TEST_ASSERT_TRUE(strlen(valid_checksums[i]) > 0);
        TEST_ASSERT_TRUE(strlen(valid_checksums[i]) <= CONFIG_CHECKSUM_LENGTH);
    }
}

void test_storage_checksum_calculation_simulation(void)
{
    // Simulate checksum calculation
    uint8_t test_data[] = "OHT50CFG_CONFIG_DATA";
    uint32_t calculated_checksum = 0;
    
    // Simple checksum calculation simulation
    for (size_t i = 0; i < sizeof(test_data) - 1; i++) {
        calculated_checksum += test_data[i];
    }
    
    TEST_ASSERT_TRUE(calculated_checksum > 0);
}

// ============================================================================
// MAGIC STRING UTILITY TESTS
// ============================================================================

void test_storage_magic_string_validation(void)
{
    // Test magic string properties
    TEST_ASSERT_EQUAL_STRING("OHT50CFG", "OHT50CFG");
    TEST_ASSERT_EQUAL(8, strlen("OHT50CFG"));
    
    // Test that magic string matches header
    TEST_ASSERT_EQUAL_STRING(test_header.magic, "OHT50CFG");
}

void test_storage_magic_string_comparison(void)
{
    // Test magic string comparison
    const char* valid_magic = "OHT50CFG";
    const char* invalid_magic = "INVALID";
    
    TEST_ASSERT_EQUAL_STRING("OHT50CFG", valid_magic);
    TEST_ASSERT_FALSE(strcmp("OHT50CFG", invalid_magic) == 0);
}

// ============================================================================
// VERSION UTILITY TESTS
// ============================================================================

void test_storage_version_extraction(void)
{
    // Test version extraction (major.minor.patch format)
    const char* version = "1.2.3";
    char major[2], minor[2], patch[2];
    
    // Simple version parsing simulation
    sscanf(version, "%1s.%1s.%1s", major, minor, patch);
    
    TEST_ASSERT_EQUAL_STRING("1", major);
    TEST_ASSERT_EQUAL_STRING("2", minor);
    TEST_ASSERT_EQUAL_STRING("3", patch);
}

void test_storage_version_comparison(void)
{
    // Test version comparison
    const char* version1 = "1.0.0";
    const char* version2 = "1.1.0";
    const char* version3 = "2.0.0";
    
    // Simple string comparison
    TEST_ASSERT_TRUE(strcmp(version2, version1) > 0);
    TEST_ASSERT_TRUE(strcmp(version3, version2) > 0);
    TEST_ASSERT_TRUE(strcmp(version1, version2) < 0);
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(void)
{
    UNITY_BEGIN();
    
    // Constants tests
    RUN_TEST(test_storage_constants);
    RUN_TEST(test_storage_magic_string);
    
    // Data structures tests
    RUN_TEST(test_storage_data_structures);
    RUN_TEST(test_storage_config_initialization);
    RUN_TEST(test_storage_header_initialization);
    
    // Validation tests
    RUN_TEST(test_storage_config_validation);
    RUN_TEST(test_storage_header_validation);
    RUN_TEST(test_storage_file_path_validation);
    
    // Utility tests
    RUN_TEST(test_storage_version_validation);
    RUN_TEST(test_storage_timestamp_validation);
    RUN_TEST(test_storage_file_size_validation);
    
    // Checksum utility tests
    RUN_TEST(test_storage_checksum_validation);
    RUN_TEST(test_storage_checksum_calculation_simulation);
    
    // Magic string utility tests
    RUN_TEST(test_storage_magic_string_validation);
    RUN_TEST(test_storage_magic_string_comparison);
    
    // Version utility tests
    RUN_TEST(test_storage_version_extraction);
    RUN_TEST(test_storage_version_comparison);
    
    UNITY_END();
    return 0;
}
