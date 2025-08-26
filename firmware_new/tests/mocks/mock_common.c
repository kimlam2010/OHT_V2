#include "mock_common.h"
#include <string.h>

static uint64_t g_mock_time_us = 1000; // start positive
static bool g_performance_test_mode = false;

void mock_common_reset(void) {
    g_mock_time_us = 1000;
    g_performance_test_mode = false;
}

void mock_common_set_time_us(uint64_t t) {
    g_mock_time_us = t;
}

void mock_common_set_performance_test_mode(bool enabled) {
    g_performance_test_mode = enabled;
}

uint64_t mock_common_get_time_us(void) {
    return g_mock_time_us;
}

// Override minimal subset used by tests
uint64_t hal_get_timestamp_us(void) {
    if (g_performance_test_mode) {
        // During performance test, return constant value
        return g_mock_time_us;
    } else {
        // Normal mode: increment by 1µs per call
        uint64_t current = g_mock_time_us;
        g_mock_time_us += 1;
        return current;
    }
}

uint64_t hal_get_timestamp_ms(void) {
    // Derived from microseconds
    return hal_get_timestamp_us() / 1000ULL;
}

void hal_sleep_ms(uint32_t milliseconds) {
    g_mock_time_us += (uint64_t)milliseconds * 1000ULL;
}

hal_status_t hal_get_device_info(uint32_t device_id, hal_device_info_t *device_info) {
    (void)device_id;
    if (device_info == NULL) return HAL_STATUS_INVALID_PARAMETER;
    memset(device_info, 0, sizeof(*device_info));
    device_info->device_type = HAL_DEVICE_TYPE_GPIO;
    device_info->status = HAL_DEVICE_STATUS_OK;
    device_info->device_id = 0;
    strcpy(device_info->device_name, "MockDevice");
    strcpy(device_info->device_version, HAL_VERSION_STRING);
    device_info->timestamp_us = hal_get_timestamp_us();
    device_info->error_count = 0;
    device_info->warning_count = 0;
    return HAL_STATUS_OK;
}
