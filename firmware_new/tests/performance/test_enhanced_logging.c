#include "unity.h"
#include "hal_common.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Test enhanced logging capabilities
void test_enhanced_logging_init(void) {
    hal_status_t status = hal_log_init(NULL); // Use stderr
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test setting log level
    status = hal_log_set_level(HAL_LOG_LEVEL_DEBUG);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test invalid log level
    status = hal_log_set_level((hal_log_level_t)99);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
}

void test_enhanced_logging_messages(void) {
    // Test basic logging
    hal_status_t status = hal_log_message(HAL_LOG_LEVEL_INFO, "Test info message");
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = hal_log_message(HAL_LOG_LEVEL_WARNING, "Test warning message");
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = hal_log_message(HAL_LOG_LEVEL_ERROR, "Test error message");
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = hal_log_message(HAL_LOG_LEVEL_DEBUG, "Test debug message");
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_enhanced_logging_with_context(void) {
    // Test structured logging with context
    hal_status_t status = hal_log_message_with_context(HAL_LOG_LEVEL_INFO, 
                                                       "TEST_COMPONENT", 
                                                       "test_function", 
                                                       123, 
                                                       "Test message with context");
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test with NULL component
    status = hal_log_message_with_context(HAL_LOG_LEVEL_WARNING, 
                                         NULL, 
                                         "test_function", 
                                         456, 
                                         "Test message with NULL component");
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_enhanced_error_logging(void) {
    // Test error logging with automatic tracking
    hal_status_t status = hal_log_error("TEST_COMPONENT", 
                                       "test_error_function", 
                                       789, 
                                       HAL_STATUS_ERROR, 
                                       "Test error with code %d", HAL_STATUS_ERROR);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test multiple errors
    status = hal_log_error("TEST_COMPONENT", 
                          "test_error_function2", 
                          999, 
                          HAL_STATUS_INVALID_PARAMETER, 
                          "Another test error");
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
}

void test_enhanced_logging_statistics(void) {
    uint32_t total_messages, error_count;
    uint64_t uptime_ms;
    
    // Get logging statistics
    hal_status_t status = hal_log_get_statistics(&total_messages, &error_count, &uptime_ms);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify we have some messages and errors
    TEST_ASSERT_GREATER_THAN(0, total_messages);
    TEST_ASSERT_GREATER_THAN(0, error_count);
    TEST_ASSERT_GREATER_THAN(0, uptime_ms);
    
    printf("Logging Statistics - Total Messages: %u, Errors: %u, Uptime: %lu ms\n", 
           total_messages, error_count, (unsigned long)uptime_ms);
}

void test_enhanced_logging_performance(void) {
    const int num_messages = 1000;
    uint64_t start_time = hal_get_timestamp_ms();
    
    // Test logging performance
    for (int i = 0; i < num_messages; i++) {
        hal_log_message(HAL_LOG_LEVEL_INFO, "Performance test message %d", i);
    }
    
    uint64_t end_time = hal_get_timestamp_ms();
    uint64_t duration = end_time - start_time;
    
    printf("Performance Test - %d messages in %lu ms (%.2f msgs/sec)\n", 
           num_messages, (unsigned long)duration, 
           (float)num_messages / (duration / 1000.0f));
    
    // Performance should be reasonable (at least 1000 msgs/sec)
    float msgs_per_sec = (float)num_messages / (duration / 1000.0f);
    TEST_ASSERT_GREATER_THAN(1000.0f, msgs_per_sec);
}

void test_enhanced_logging_close(void) {
    // Test logging close
    hal_status_t status = hal_log_close();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test closing again (should fail)
    status = hal_log_close();
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

int main(void) {
    UNITY_BEGIN();
    
    printf("🚀 OHT-50 ENHANCED LOGGING TEST SUITE\n");
    printf("=====================================\n\n");
    
    RUN_TEST(test_enhanced_logging_init);
    RUN_TEST(test_enhanced_logging_messages);
    RUN_TEST(test_enhanced_logging_with_context);
    RUN_TEST(test_enhanced_error_logging);
    RUN_TEST(test_enhanced_logging_statistics);
    RUN_TEST(test_enhanced_logging_performance);
    RUN_TEST(test_enhanced_logging_close);
    
    UNITY_END();
    return 0;
}
