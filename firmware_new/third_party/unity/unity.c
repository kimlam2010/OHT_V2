/**
 * @file unity.c
 * @brief Unity Test Framework Implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-04 (Unit Testing)
 */

#include "unity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// Unity test framework state
static struct {
    int test_failures;
    int test_count;
    int test_ignored;
    char current_test_name[256];
    char failure_message[512];
    int line_number;
    const char* file_name;
} unity_state = {0};

// Unity test framework functions

void unity_begin(const char* filename) {
    printf("\nUnity Test Framework\n");
    printf("====================\n");
    printf("Running tests from: %s\n\n", filename);
    
    unity_state.test_failures = 0;
    unity_state.test_count = 0;
    unity_state.test_ignored = 0;
}

void unity_end(void) {
    printf("\n====================\n");
    printf("Test Results Summary\n");
    printf("====================\n");
    printf("Tests Run: %d\n", unity_state.test_count);
    printf("Failures: %d\n", unity_state.test_failures);
    printf("Ignored: %d\n", unity_state.test_ignored);
    printf("Success Rate: %.1f%%\n", 
           unity_state.test_count > 0 ? 
           ((float)(unity_state.test_count - unity_state.test_failures) / unity_state.test_count) * 100.0f : 0.0f);
    
    if (unity_state.test_failures > 0) {
        printf("\n❌ Some tests failed!\n");
        exit(1);
    } else {
        printf("\n✅ All tests passed!\n");
        exit(0);
    }
}

void unity_run_test(void (*test_func)(void), const char* test_name) {
    unity_state.test_count++;
    strncpy(unity_state.current_test_name, test_name, sizeof(unity_state.current_test_name) - 1);
    unity_state.current_test_name[sizeof(unity_state.current_test_name) - 1] = '\0';
    
    printf("Running: %s", test_name);
    fflush(stdout);
    
    // Reset failure state for this test
    unity_state.test_failures = 0;
    
    // Run the test
    test_func();
    
    if (unity_state.test_failures > 0) {
        printf(" ❌ FAILED\n");
        printf("   File: %s:%d\n", unity_state.file_name, unity_state.line_number);
        printf("   Message: %s\n", unity_state.failure_message);
        // Don't reset test_failures here - let unity_end() handle the total count
    } else {
        printf(" ✅ PASSED\n");
    }
}

void unity_fail(const char* message, const char* file, int line) {
    unity_state.test_failures++;
    unity_state.line_number = line;
    unity_state.file_name = file;
    strncpy(unity_state.failure_message, message, sizeof(unity_state.failure_message) - 1);
    unity_state.failure_message[sizeof(unity_state.failure_message) - 1] = '\0';
}

// Unity assertion macros

void unity_assert_equal_int(int expected, int actual, const char* message, const char* file, int line) {
    if (expected != actual) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: expected %d, got %d", message, expected, actual);
        unity_fail(buffer, file, line);
    }
}

void unity_assert_equal_uint(uint32_t expected, uint32_t actual, const char* message, const char* file, int line) {
    if (expected != actual) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: expected %u, got %u", message, expected, actual);
        unity_fail(buffer, file, line);
    }
}

void unity_assert_equal_float(float expected, float actual, float tolerance, const char* message, const char* file, int line) {
    float diff = expected - actual;
    if (diff < 0) diff = -diff;
    if (diff > tolerance) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: expected %.6f, got %.6f (tolerance %.6f)", 
                message, expected, actual, tolerance);
        unity_fail(buffer, file, line);
    }
}

void unity_assert_not_null(const void* ptr, const char* message, const char* file, int line) {
    if (ptr == NULL) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: pointer is NULL", message);
        unity_fail(buffer, file, line);
    }
}

void unity_assert_null(const void* ptr, const char* message, const char* file, int line) {
    if (ptr != NULL) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: pointer is not NULL", message);
        unity_fail(buffer, file, line);
    }
}

void unity_assert_true(int condition, const char* message, const char* file, int line) {
    if (!condition) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: condition is false", message);
        unity_fail(buffer, file, line);
    }
}

void unity_assert_false(int condition, const char* message, const char* file, int line) {
    if (condition) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: condition is true", message);
        unity_fail(buffer, file, line);
    }
}

void unity_assert_greater_than(int threshold, int actual, const char* message, const char* file, int line) {
    if (actual <= threshold) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: %d is not greater than %d", message, actual, threshold);
        unity_fail(buffer, file, line);
    }
}

void unity_assert_less_than(int threshold, int actual, const char* message, const char* file, int line) {
    if (actual >= threshold) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: %d is not less than %d", message, actual, threshold);
        unity_fail(buffer, file, line);
    }
}

void unity_assert_greater_or_equal(int threshold, int actual, const char* message, const char* file, int line) {
    if (actual < threshold) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: %d is not greater than or equal to %d", message, actual, threshold);
        unity_fail(buffer, file, line);
    }
}

void unity_assert_less_or_equal(int threshold, int actual, const char* message, const char* file, int line) {
    if (actual > threshold) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: %d is not less than or equal to %d", message, actual, threshold);
        unity_fail(buffer, file, line);
    }
}

void unity_assert_equal_string(const char* expected, const char* actual, const char* message, const char* file, int line) {
    if (strcmp(expected, actual) != 0) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: expected '%s', got '%s'", message, expected, actual);
        unity_fail(buffer, file, line);
    }
}

// Unity test runner macros

#define UNITY_BEGIN() unity_begin(__FILE__)
#define UNITY_END() unity_end()

#define RUN_TEST(test_func) unity_run_test(test_func, #test_func)

// Unity assertion macros
#define TEST_ASSERT_EQUAL(expected, actual) \
    unity_assert_equal_int((int)(expected), (int)(actual), "Values not equal", __FILE__, __LINE__)

#define TEST_ASSERT_EQUAL_UINT(expected, actual) \
    unity_assert_equal_uint((uint32_t)(expected), (uint32_t)(actual), "Values not equal", __FILE__, __LINE__)

#define TEST_ASSERT_EQUAL_FLOAT(expected, actual, tolerance) \
    unity_assert_equal_float((float)(expected), (float)(actual), (float)(tolerance), "Values not equal", __FILE__, __LINE__)

#define TEST_ASSERT_NOT_NULL(ptr) \
    unity_assert_not_null((ptr), "Pointer is NULL", __FILE__, __LINE__)

#define TEST_ASSERT_NULL(ptr) \
    unity_assert_null((ptr), "Pointer is not NULL", __FILE__, __LINE__)

#define TEST_ASSERT_TRUE(condition) \
    unity_assert_true((condition), "Condition is false", __FILE__, __LINE__)

#define TEST_ASSERT_FALSE(condition) \
    unity_assert_false((condition), "Condition is true", __FILE__, __LINE__)

#define TEST_ASSERT_GREATER_THAN(threshold, actual) \
    unity_assert_greater_than((int)(threshold), (int)(actual), "Value not greater than threshold", __FILE__, __LINE__)

#define TEST_ASSERT_LESS_THAN(threshold, actual) \
    unity_assert_less_than((int)(threshold), (int)(actual), "Value not less than threshold", __FILE__, __LINE__)

#define TEST_ASSERT_GREATER_THAN_OR_EQUAL(threshold, actual) \
    unity_assert_greater_or_equal((int)(threshold), (int)(actual), "Value not greater than or equal to threshold", __FILE__, __LINE__)

#define TEST_ASSERT_LESS_THAN_OR_EQUAL(threshold, actual) \
    unity_assert_less_or_equal((int)(threshold), (int)(actual), "Value not less than or equal to threshold", __FILE__, __LINE__)

#define TEST_ASSERT_EQUAL_STRING(expected, actual) \
    unity_assert_equal_string((expected), (actual), "Strings not equal", __FILE__, __LINE__)
