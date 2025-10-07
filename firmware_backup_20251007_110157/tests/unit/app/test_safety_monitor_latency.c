/**
 * @file test_safety_monitor_latency.c
 * @brief Unit test for Safety Monitor E-Stop latency path
 */

#include "unity.h"
#include "safety_monitor.h"
#include "hal_common.h"
#include <string.h>

static safety_monitor_config_t cfg;

void setUp(void)
{
    memset(&cfg, 0, sizeof(cfg));
    cfg.update_period_ms = 5;
    cfg.estop_timeout_ms = 100; // Must be >= 100ms per HAL requirements
    cfg.zone_check_period_ms = 50;
    cfg.interlock_check_period_ms = 20;
    cfg.sensor_check_period_ms = 50;
    cfg.watchdog_timeout_ms = 1000;
    cfg.enable_zone_monitoring = false;
    cfg.enable_interlock_monitoring = false;
    cfg.enable_sensor_monitoring = false;
    cfg.enable_watchdog_monitoring = false;
    cfg.enable_emergency_procedures = false;
    cfg.max_retry_count = 0;
    cfg.retry_delay_ms = 0;
}

void tearDown(void)
{
    safety_monitor_deinit();
}

void test_estop_latency_under_50ms_on_event(void)
{
    // Initialize
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_monitor_init(&cfg));

    // Trigger E-Stop event path
    TEST_ASSERT_EQUAL(HAL_STATUS_OK,
        safety_monitor_process_event(SAFETY_MONITOR_EVENT_ESTOP_TRIGGERED, "unit-test"));

    // Read back latency metric
    uint32_t latency_ms = 0;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, safety_monitor_get_last_estop_latency(&latency_ms));

    // Validate < 50 ms target
    TEST_ASSERT_TRUE(latency_ms < 50);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_estop_latency_under_50ms_on_event);
    UNITY_END();
    return 0;
}


