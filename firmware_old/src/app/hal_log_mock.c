/**
 * @file hal_log_mock.c
 * @brief Mock HAL logging functions for API testing
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 * @task FW-INT-01 (HTTP Server Implementation)
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// Mock logging functions
void hal_log_info(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[INFO] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

void hal_log_error(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[ERROR] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

void hal_log_warning(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[WARNING] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}

void hal_log_debug(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[DEBUG] ");
    vprintf(format, args);
    printf("\n");
    va_end(args);
}
