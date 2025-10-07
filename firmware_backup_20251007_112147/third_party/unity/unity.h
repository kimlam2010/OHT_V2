/**
 * @file unity.h
 * @brief Unity Test Framework Header
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-04 (Unit Testing)
 */

#ifndef UNITY_H
#define UNITY_H

#include <stdint.h>

// Unity test framework functions
void unity_begin(const char* filename);
void unity_end(void);
void unity_run_test(void (*test_func)(void), const char* test_name);
void unity_fail(const char* message, const char* file, int line);

// Unity assertion functions
void unity_assert_equal_int(int expected, int actual, const char* message, const char* file, int line);
void unity_assert_equal_uint(uint32_t expected, uint32_t actual, const char* message, const char* file, int line);
void unity_assert_equal_float(float expected, float actual, float tolerance, const char* message, const char* file, int line);
void unity_assert_not_null(const void* ptr, const char* message, const char* file, int line);
void unity_assert_null(const void* ptr, const char* message, const char* file, int line);
void unity_assert_true(int condition, const char* message, const char* file, int line);
void unity_assert_false(int condition, const char* message, const char* file, int line);
void unity_assert_greater_than(int threshold, int actual, const char* message, const char* file, int line);
void unity_assert_less_than(int threshold, int actual, const char* message, const char* file, int line);
void unity_assert_greater_or_equal(int threshold, int actual, const char* message, const char* file, int line);
void unity_assert_less_or_equal(int threshold, int actual, const char* message, const char* file, int line);
void unity_assert_equal_string(const char* expected, const char* actual, const char* message, const char* file, int line);

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

#endif // UNITY_H
