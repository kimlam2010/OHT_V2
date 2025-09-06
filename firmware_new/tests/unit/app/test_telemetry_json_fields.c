/**
 * @file test_telemetry_json_fields.c
 * @brief Unit test to ensure telemetry JSON contains system/safety/control fields
 */

#include "unity.h"
#include "telemetry_manager.h"
#include <string.h>

void setUp(void) {}
void tearDown(void) { telemetry_manager_deinit(); }

void test_telemetry_json_contains_core_fields(void)
{
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, telemetry_manager_init(NULL));

    telemetry_data_t data;
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, telemetry_manager_get_data(&data));

    char json[4096];
    int n = telemetry_manager_serialize_json(&data, json, sizeof(json));
    TEST_ASSERT_GREATER_THAN(0, n);

    TEST_ASSERT_NOT_NULL(strstr(json, "\"status\":"));
    TEST_ASSERT_NOT_NULL(strstr(json, "\"safety\":"));
    TEST_ASSERT_NOT_NULL(strstr(json, "\"pos_mm\":"));
    TEST_ASSERT_NOT_NULL(strstr(json, "\"vel_mms\":"));
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_telemetry_json_contains_core_fields);
    UNITY_END();
    return 0;
}


