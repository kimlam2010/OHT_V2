/**
 * @file mock_common.h
 * @brief Mock Common HAL functions for unit testing
 * @version 1.0.0
 * @date 2025-01-27
 * @team FW
 */

#ifndef MOCK_COMMON_H
#define MOCK_COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <sys/sysinfo.h> // For sysinfo struct
#include <sys/time.h> // For struct timeval, struct timezone
#include "hal_common.h" // For HAL types

// Mock Common HAL state
typedef struct {
    uint64_t timestamp_us;
    uint64_t timestamp_ms;
    hal_device_info_t device_info[10]; // Max 10 devices
    uint32_t device_count;
    hal_error_info_t last_error;
    hal_statistics_t statistics;
    struct sysinfo sys_info; // For mocking sysinfo
    int gettimeofday_return;
    int sysinfo_return;
} mock_common_state_t;

extern mock_common_state_t mock_common_state;

// Mock functions for HAL Common
void mock_common_reset(void);
void mock_common_set_timestamp_us(uint64_t us);
void mock_common_set_timestamp_ms(uint64_t ms);
void mock_common_set_device_info(uint32_t device_id, const hal_device_info_t *info);
void mock_common_set_system_uptime(uint64_t uptime_seconds);
void mock_common_set_free_memory(uint64_t free_mem_bytes);
void mock_common_set_cpu_usage(uint32_t cpu_usage_percent);
void mock_common_set_temperature(int16_t temperature_celsius);
void mock_common_set_last_error(hal_status_t error_code, const char *message);
void mock_common_set_build_info(uint32_t major, uint32_t minor, uint32_t patch);
void mock_common_set_performance_test_mode(bool enabled);

// Mock system calls
int mock_gettimeofday(struct timeval *tv, struct timezone *tz);
int mock_sysinfo(struct sysinfo *info);

#endif // MOCK_COMMON_H
