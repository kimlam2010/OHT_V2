/**
 * @file test_telemetry_manager.c
 * @brief Unit tests for Telemetry Manager
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 */

#include <string.h>
#include <stdio.h>
#include "unity.h"
#include "telemetry_manager.h"
#include "hal_common.h"
#include "system_state_machine.h"
#include "module_manager.h"

// Test data structures
static telemetry_config_t test_config;
static telemetry_location_t test_location;
static telemetry_navigation_t test_navigation;
static telemetry_dock_t test_dock;
static telemetry_safety_t test_safety;
static telemetry_status_t test_status;

// Event tracking
static telemetry_event_t g_last_event;
static telemetry_data_t g_last_event_data;

// ============================================================================
// SETUP AND TEARDOWN
// ============================================================================

void setUp(void)
{
    // Initialize test configuration
    test_config.update_rate_ms = 100;
    test_config.buffer_size = 50;
    test_config.enable_location = true;
    test_config.enable_navigation = true;
    test_config.enable_dock = true;
    test_config.enable_system = true;
    test_config.enable_events = true;

    // Initialize test location data
    test_location.position.x = 1000.0f;
    test_location.position.y = 2000.0f;
    test_location.position.z = 0.0f;
    test_location.orientation.pitch = 0.0f;
    test_location.orientation.roll = 0.0f;
    test_location.orientation.yaw = 45.0f;
    test_location.velocity.x = 100.0f;
    test_location.velocity.y = 0.0f;
    test_location.velocity.z = 0.0f;
    test_location.acceleration.x = 0.0f;
    test_location.acceleration.y = 0.0f;
    test_location.acceleration.z = 0.0f;
    test_location.accuracy = 10.0f;
    test_location.system_status = LOCATION_SYSTEM_OK;
    test_location.imu_status = IMU_STATUS_OK;
    test_location.magnetic_status = MAGNETIC_STATUS_OK;

    // Initialize test navigation data
    test_navigation.status = NAV_STATUS_NAVIGATING;
    test_navigation.target.x = 5000.0f;
    test_navigation.target.y = 3000.0f;
    test_navigation.target.z = 0.0f;
    test_navigation.progress = 25.0f;
    test_navigation.distance_to_target = 3000.0f;
    test_navigation.estimated_time = 30.0f;
    test_navigation.path_clear = true;
    test_navigation.obstacle_detected = false;
    test_navigation.obstacle_distance = 0.0f;
    test_navigation.obstacle_angle = 0.0f;
    test_navigation.speed_limit = 200.0f;
    test_navigation.zone_type = ZONE_TYPE_SAFE;

    // Initialize test dock data
    strcpy(test_dock.station_id, "STATION_001");
    strcpy(test_dock.target_station_id, "STATION_001");
    test_dock.status = DOCK_STATUS_APPROACHING;
    test_dock.alignment_status = ALIGNMENT_ALIGNING;
    test_dock.distance = 500.0f;
    test_dock.angle = 5.0f;
    test_dock.charging_status = CHARGING_NOT_CHARGING;
    test_dock.charging_current = 0.0f;
    test_dock.charging_voltage = 0.0f;
    strcpy(test_dock.rfid_tag_id, "TAG_001");
    test_dock.rfid_signal_strength = -45;
    test_dock.safety_status = DOCK_SAFETY_SAFE;

    // Initialize test safety data
    test_safety.estop = false;
    test_safety.zone_blocked = false;
    test_safety.interlock_active = false;
    test_safety.location_safe = true;
    test_safety.obstacle_detected = false;
    test_safety.safety_zone_clear = true;
    test_safety.emergency_stop = false;

    // Initialize test status data
    test_status.state = SYSTEM_STATE_IDLE;
    test_status.pos_mm = 1000.0f;
    test_status.vel_mms = 100.0f;
    test_status.acc_mms2 = 0.0f;
    test_status.target.pos_mm = 5000.0f;
    test_status.target.vel_mms = 200.0f;
    memcpy(&test_status.safety, &test_safety, sizeof(telemetry_safety_t));
    test_status.enc.health = ENCODER_HEALTH_OK;
    strcpy(test_status.enc.fault, "");
    test_status.enc.count = 12345;
    strcpy(test_status.errors, "");
    strcpy(test_status.warnings, "");

    // Reset event tracking
    g_last_event = TELEMETRY_EVENT_NONE;
    memset(&g_last_event_data, 0, sizeof(telemetry_data_t));
}

void tearDown(void)
{
    // Cleanup if needed
    telemetry_manager_deinit();
}

// Event callback for testing
static void test_event_callback(telemetry_event_t event, const telemetry_data_t *data) {
    g_last_event = event;
    if (data != NULL) {
        memcpy(&g_last_event_data, data, sizeof(telemetry_data_t));
    }
}

// ============================================================================
// CONSTANTS TESTS
// ============================================================================

void test_telemetry_status_constants(void)
{
    // Test telemetry status constants
    TEST_ASSERT_EQUAL(0, TELEMETRY_STATUS_OK);
    TEST_ASSERT_EQUAL(1, TELEMETRY_STATUS_ERROR);
    TEST_ASSERT_EQUAL(2, TELEMETRY_STATUS_BUFFER_FULL);
    TEST_ASSERT_EQUAL(3, TELEMETRY_STATUS_INVALID_DATA);
}

void test_telemetry_event_constants(void)
{
    // Test telemetry event constants
    TEST_ASSERT_EQUAL(0, TELEMETRY_EVENT_NONE);
    TEST_ASSERT_EQUAL(1, TELEMETRY_EVENT_LOCATION_UPDATE);
    TEST_ASSERT_EQUAL(2, TELEMETRY_EVENT_NAVIGATION_UPDATE);
    TEST_ASSERT_EQUAL(3, TELEMETRY_EVENT_DOCK_UPDATE);
    TEST_ASSERT_EQUAL(4, TELEMETRY_EVENT_SAFETY_ALERT);
    TEST_ASSERT_EQUAL(5, TELEMETRY_EVENT_SYSTEM_STATUS);
    TEST_ASSERT_EQUAL(6, TELEMETRY_EVENT_ERROR);
}

void test_location_status_constants(void)
{
    // Test location status constants
    TEST_ASSERT_EQUAL(0, LOCATION_SYSTEM_OK);
    TEST_ASSERT_EQUAL(1, LOCATION_SYSTEM_ERROR);
    TEST_ASSERT_EQUAL(2, LOCATION_SYSTEM_CALIBRATING);
    
    TEST_ASSERT_EQUAL(0, IMU_STATUS_OK);
    TEST_ASSERT_EQUAL(1, IMU_STATUS_ERROR);
    
    TEST_ASSERT_EQUAL(0, MAGNETIC_STATUS_OK);
    TEST_ASSERT_EQUAL(1, MAGNETIC_STATUS_ERROR);
}

void test_navigation_status_constants(void)
{
    // Test navigation status constants
    TEST_ASSERT_EQUAL(0, NAV_STATUS_IDLE);
    TEST_ASSERT_EQUAL(1, NAV_STATUS_NAVIGATING);
    TEST_ASSERT_EQUAL(2, NAV_STATUS_POSITIONING);
    TEST_ASSERT_EQUAL(3, NAV_STATUS_ARRIVED);
    TEST_ASSERT_EQUAL(4, NAV_STATUS_ERROR);
    
    TEST_ASSERT_EQUAL(0, ZONE_TYPE_SAFE);
    TEST_ASSERT_EQUAL(1, ZONE_TYPE_WARNING);
    TEST_ASSERT_EQUAL(2, ZONE_TYPE_RESTRICTED);
    TEST_ASSERT_EQUAL(3, ZONE_TYPE_EMERGENCY);
}

void test_dock_status_constants(void)
{
    // Test dock status constants
    TEST_ASSERT_EQUAL(0, DOCK_STATUS_NOT_DOCKING);
    TEST_ASSERT_EQUAL(1, DOCK_STATUS_APPROACHING);
    TEST_ASSERT_EQUAL(2, DOCK_STATUS_ALIGNING);
    TEST_ASSERT_EQUAL(3, DOCK_STATUS_DOCKED);
    TEST_ASSERT_EQUAL(4, DOCK_STATUS_UNDOCKING);
    TEST_ASSERT_EQUAL(5, DOCK_STATUS_ERROR);
    
    TEST_ASSERT_EQUAL(0, ALIGNMENT_NOT_ALIGNED);
    TEST_ASSERT_EQUAL(1, ALIGNMENT_ALIGNING);
    TEST_ASSERT_EQUAL(2, ALIGNMENT_ALIGNED);
    TEST_ASSERT_EQUAL(3, ALIGNMENT_ERROR);
    
    TEST_ASSERT_EQUAL(0, CHARGING_NOT_CHARGING);
    TEST_ASSERT_EQUAL(1, CHARGING_CHARGING);
    TEST_ASSERT_EQUAL(2, CHARGING_FULL);
    TEST_ASSERT_EQUAL(3, CHARGING_ERROR);
    
    TEST_ASSERT_EQUAL(0, DOCK_SAFETY_SAFE);
    TEST_ASSERT_EQUAL(1, DOCK_SAFETY_WARNING);
    TEST_ASSERT_EQUAL(2, DOCK_SAFETY_DANGER);
}

void test_encoder_health_constants(void)
{
    // Test encoder health constants
    TEST_ASSERT_EQUAL(0, ENCODER_HEALTH_OK);
    TEST_ASSERT_EQUAL(1, ENCODER_HEALTH_WARNING);
    TEST_ASSERT_EQUAL(2, ENCODER_HEALTH_ERROR);
}

// ============================================================================
// DATA STRUCTURES TESTS
// ============================================================================

void test_telemetry_data_structures(void)
{
    setUp();
    
    // Test configuration structure
    TEST_ASSERT_EQUAL(100, test_config.update_rate_ms);
    TEST_ASSERT_EQUAL(50, test_config.buffer_size);
    TEST_ASSERT_TRUE(test_config.enable_location);
    TEST_ASSERT_TRUE(test_config.enable_navigation);
    TEST_ASSERT_TRUE(test_config.enable_dock);
    TEST_ASSERT_TRUE(test_config.enable_system);
    TEST_ASSERT_TRUE(test_config.enable_events);

    // Test location structure
    TEST_ASSERT_EQUAL_FLOAT(1000.0f, test_location.position.x, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(2000.0f, test_location.position.y, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_location.position.z, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(45.0f, test_location.orientation.yaw, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, test_location.velocity.x, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(10.0f, test_location.accuracy, 0.001f);
    TEST_ASSERT_EQUAL(LOCATION_SYSTEM_OK, test_location.system_status);

    // Test navigation structure
    TEST_ASSERT_EQUAL(NAV_STATUS_NAVIGATING, test_navigation.status);
    TEST_ASSERT_EQUAL_FLOAT(5000.0f, test_navigation.target.x, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(25.0f, test_navigation.progress, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(3000.0f, test_navigation.distance_to_target, 0.001f);
    TEST_ASSERT_TRUE(test_navigation.path_clear);
    TEST_ASSERT_FALSE(test_navigation.obstacle_detected);
    TEST_ASSERT_EQUAL(ZONE_TYPE_SAFE, test_navigation.zone_type);

    // Test dock structure
    TEST_ASSERT_EQUAL(DOCK_STATUS_APPROACHING, test_dock.status);
    TEST_ASSERT_EQUAL_STRING("STATION_001", test_dock.station_id);
    TEST_ASSERT_EQUAL_FLOAT(500.0f, test_dock.distance, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(5.0f, test_dock.angle, 0.001f);
    TEST_ASSERT_EQUAL(CHARGING_NOT_CHARGING, test_dock.charging_status);
    TEST_ASSERT_EQUAL_STRING("TAG_001", test_dock.rfid_tag_id);
    TEST_ASSERT_EQUAL(-45, test_dock.rfid_signal_strength);

    // Test safety structure
    TEST_ASSERT_FALSE(test_safety.estop);
    TEST_ASSERT_FALSE(test_safety.zone_blocked);
    TEST_ASSERT_FALSE(test_safety.interlock_active);
    TEST_ASSERT_TRUE(test_safety.location_safe);
    TEST_ASSERT_FALSE(test_safety.obstacle_detected);
    TEST_ASSERT_TRUE(test_safety.safety_zone_clear);
    TEST_ASSERT_FALSE(test_safety.emergency_stop);

    // Test status structure
    TEST_ASSERT_EQUAL(SYSTEM_STATE_IDLE, test_status.state);
    TEST_ASSERT_EQUAL_FLOAT(1000.0f, test_status.pos_mm, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(100.0f, test_status.vel_mms, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(0.0f, test_status.acc_mms2, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(5000.0f, test_status.target.pos_mm, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(200.0f, test_status.target.vel_mms, 0.001f);
    TEST_ASSERT_EQUAL(ENCODER_HEALTH_OK, test_status.enc.health);
    TEST_ASSERT_EQUAL(12345, test_status.enc.count);
}

// ============================================================================
// INITIALIZATION TESTS
// ============================================================================

void test_telemetry_manager_initialization(void)
{
    setUp();
    
    // Test initialization with default config
    hal_status_t status = telemetry_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test double initialization
    status = telemetry_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_ALREADY_INITIALIZED, status);
    
    // Test deinitialization
    status = telemetry_manager_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Test deinitialization when not initialized
    status = telemetry_manager_deinit();
    TEST_ASSERT_EQUAL(HAL_STATUS_NOT_INITIALIZED, status);
}

void test_telemetry_manager_custom_config(void)
{
    setUp();
    
    // Test initialization with custom config
    telemetry_config_t custom_config = {
        .update_rate_ms = 200,
        .buffer_size = 25,
        .enable_location = false,
        .enable_navigation = true,
        .enable_dock = false,
        .enable_system = true,
        .enable_events = false
    };
    
    hal_status_t status = telemetry_manager_init(&custom_config);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify configuration was applied
    telemetry_data_t data;
    status = telemetry_manager_get_data(&data);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    telemetry_manager_deinit();
}

// ============================================================================
// DATA UPDATE TESTS
// ============================================================================

void test_telemetry_manager_update_location(void)
{
    setUp();
    
    hal_status_t status = telemetry_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Update location data
    status = telemetry_manager_update_location(&test_location);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify location data was updated
    telemetry_data_t data;
    status = telemetry_manager_get_data(&data);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    TEST_ASSERT_EQUAL_FLOAT(test_location.position.x, data.location.position.x, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_location.position.y, data.location.position.y, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_location.position.z, data.location.position.z, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_location.orientation.yaw, data.location.orientation.yaw, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_location.velocity.x, data.location.velocity.x, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_location.accuracy, data.location.accuracy, 0.001f);
    TEST_ASSERT_EQUAL(test_location.system_status, data.location.system_status);
    
    telemetry_manager_deinit();
}

void test_telemetry_manager_update_navigation(void)
{
    setUp();
    
    hal_status_t status = telemetry_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Update navigation data
    status = telemetry_manager_update_navigation(&test_navigation);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify navigation data was updated
    telemetry_data_t data;
    status = telemetry_manager_get_data(&data);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    TEST_ASSERT_EQUAL(test_navigation.status, data.navigation.status);
    TEST_ASSERT_EQUAL_FLOAT(test_navigation.target.x, data.navigation.target.x, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_navigation.target.y, data.navigation.target.y, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_navigation.progress, data.navigation.progress, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_navigation.distance_to_target, data.navigation.distance_to_target, 0.001f);
    TEST_ASSERT_EQUAL(test_navigation.path_clear, data.navigation.path_clear);
    TEST_ASSERT_EQUAL(test_navigation.obstacle_detected, data.navigation.obstacle_detected);
    TEST_ASSERT_EQUAL(test_navigation.zone_type, data.navigation.zone_type);
    
    telemetry_manager_deinit();
}

void test_telemetry_manager_update_dock(void)
{
    setUp();
    
    hal_status_t status = telemetry_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Update dock data
    status = telemetry_manager_update_dock(&test_dock);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify dock data was updated
    telemetry_data_t data;
    status = telemetry_manager_get_data(&data);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    TEST_ASSERT_EQUAL(test_dock.status, data.dock.status);
    TEST_ASSERT_EQUAL_STRING(test_dock.station_id, data.dock.station_id);
    TEST_ASSERT_EQUAL_STRING(test_dock.target_station_id, data.dock.target_station_id);
    TEST_ASSERT_EQUAL(test_dock.alignment_status, data.dock.alignment_status);
    TEST_ASSERT_EQUAL_FLOAT(test_dock.distance, data.dock.distance, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_dock.angle, data.dock.angle, 0.001f);
    TEST_ASSERT_EQUAL(test_dock.charging_status, data.dock.charging_status);
    TEST_ASSERT_EQUAL_STRING(test_dock.rfid_tag_id, data.dock.rfid_tag_id);
    TEST_ASSERT_EQUAL(test_dock.rfid_signal_strength, data.dock.rfid_signal_strength);
    TEST_ASSERT_EQUAL(test_dock.safety_status, data.dock.safety_status);
    
    telemetry_manager_deinit();
}

void test_telemetry_manager_update_safety(void)
{
    setUp();
    
    hal_status_t status = telemetry_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Update safety data
    status = telemetry_manager_update_safety(&test_safety);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify safety data was updated
    telemetry_data_t data;
    status = telemetry_manager_get_data(&data);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    TEST_ASSERT_EQUAL(test_safety.estop, data.status.safety.estop);
    TEST_ASSERT_EQUAL(test_safety.zone_blocked, data.status.safety.zone_blocked);
    TEST_ASSERT_EQUAL(test_safety.interlock_active, data.status.safety.interlock_active);
    TEST_ASSERT_EQUAL(test_safety.location_safe, data.status.safety.location_safe);
    TEST_ASSERT_EQUAL(test_safety.obstacle_detected, data.status.safety.obstacle_detected);
    TEST_ASSERT_EQUAL(test_safety.safety_zone_clear, data.status.safety.safety_zone_clear);
    TEST_ASSERT_EQUAL(test_safety.emergency_stop, data.status.safety.emergency_stop);
    
    telemetry_manager_deinit();
}

void test_telemetry_manager_update_status(void)
{
    setUp();
    
    hal_status_t status = telemetry_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Update status data
    status = telemetry_manager_update_status(&test_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify status data was updated
    telemetry_data_t data;
    status = telemetry_manager_get_data(&data);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    TEST_ASSERT_EQUAL(test_status.state, data.status.state);
    TEST_ASSERT_EQUAL_FLOAT(test_status.pos_mm, data.status.pos_mm, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_status.vel_mms, data.status.vel_mms, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_status.acc_mms2, data.status.acc_mms2, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_status.target.pos_mm, data.status.target.pos_mm, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_status.target.vel_mms, data.status.target.vel_mms, 0.001f);
    TEST_ASSERT_EQUAL(test_status.enc.health, data.status.enc.health);
    TEST_ASSERT_EQUAL(test_status.enc.count, data.status.enc.count);
    
    telemetry_manager_deinit();
}

// ============================================================================
// EVENT CALLBACK TESTS
// ============================================================================

void test_telemetry_manager_event_callback(void)
{
    setUp();
    
    hal_status_t status = telemetry_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Set event callback
    status = telemetry_manager_set_callback(test_event_callback);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Update location data to trigger event
    status = telemetry_manager_update_location(&test_location);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify event was triggered
    TEST_ASSERT_EQUAL(TELEMETRY_EVENT_LOCATION_UPDATE, g_last_event);
    
    // Verify event data
    TEST_ASSERT_EQUAL_FLOAT(test_location.position.x, g_last_event_data.location.position.x, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_location.position.y, g_last_event_data.location.position.y, 0.001f);
    TEST_ASSERT_EQUAL_FLOAT(test_location.position.z, g_last_event_data.location.position.z, 0.001f);
    
    telemetry_manager_deinit();
}

void test_telemetry_manager_safety_alert_event(void)
{
    setUp();
    
    hal_status_t status = telemetry_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Set event callback
    status = telemetry_manager_set_callback(test_event_callback);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Create safety alert condition
    telemetry_safety_t alert_safety = test_safety;
    alert_safety.estop = true;
    
    // Update safety data to trigger alert
    status = telemetry_manager_update_safety(&alert_safety);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify safety alert event was triggered
    TEST_ASSERT_EQUAL(TELEMETRY_EVENT_SAFETY_ALERT, g_last_event);
    
    telemetry_manager_deinit();
}

// ============================================================================
// JSON SERIALIZATION TESTS
// ============================================================================

void test_telemetry_manager_json_serialization(void)
{
    setUp();
    
    hal_status_t status = telemetry_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Update all data
    status = telemetry_manager_update_location(&test_location);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = telemetry_manager_update_navigation(&test_navigation);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = telemetry_manager_update_dock(&test_dock);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = telemetry_manager_update_safety(&test_safety);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = telemetry_manager_update_status(&test_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Get the current telemetry data
    telemetry_data_t current_data;
    status = telemetry_manager_get_data(&current_data);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Serialize to JSON
    char json_buffer[4096];
    int result = telemetry_manager_serialize_json(&current_data, json_buffer, sizeof(json_buffer));
    TEST_ASSERT_GREATER_THAN(0, result);
    TEST_ASSERT_LESS_THAN((int)sizeof(json_buffer), result);
    
    // Verify JSON contains expected fields
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"ts\":"));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"status\":"));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"location\":"));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"navigation\":"));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"dock\":"));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"mission_id\":"));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"station_id\":"));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"cargo_present\":"));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"cpu_usage\":"));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"memory_usage\":"));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"temperature\":"));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"center_connected\":"));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"rs485_connected\":"));
    
    // Verify specific values
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"state\": \"idle\""));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"status\": \"navigating\""));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"status\": \"approaching\""));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"mission_id\": \"NONE\""));
    TEST_ASSERT_NOT_NULL(strstr(json_buffer, "\"station_id\": \"NONE\""));
    
    telemetry_manager_deinit();
}

// ============================================================================
// STATISTICS TESTS
// ============================================================================

void test_telemetry_manager_statistics(void)
{
    setUp();
    
    hal_status_t status = telemetry_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Perform some updates
    status = telemetry_manager_update_location(&test_location);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = telemetry_manager_update_navigation(&test_navigation);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = telemetry_manager_update_dock(&test_dock);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = telemetry_manager_update_safety(&test_safety);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = telemetry_manager_update_status(&test_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Get statistics
    telemetry_stats_t stats;
    status = telemetry_manager_get_statistics(&stats);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    // Verify statistics
    TEST_ASSERT_GREATER_THAN(0, stats.total_updates);
    TEST_ASSERT_EQUAL(1, stats.location_updates);
    TEST_ASSERT_EQUAL(1, stats.navigation_updates);
    TEST_ASSERT_EQUAL(1, stats.dock_updates);
    TEST_ASSERT_EQUAL(1, stats.safety_updates);
    TEST_ASSERT_EQUAL(1, stats.system_updates);
    TEST_ASSERT_GREATER_THAN_OR_EQUAL(0, stats.events_sent);
    TEST_ASSERT_GREATER_THAN_OR_EQUAL(0, stats.json_serializations);
    TEST_ASSERT_GREATER_THAN(0, stats.last_update_time);
    
    // Test reset statistics
    status = telemetry_manager_reset_statistics();
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = telemetry_manager_get_statistics(&stats);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    TEST_ASSERT_EQUAL(0, stats.total_updates);
    TEST_ASSERT_EQUAL(0, stats.location_updates);
    TEST_ASSERT_EQUAL(0, stats.navigation_updates);
    TEST_ASSERT_EQUAL(0, stats.dock_updates);
    TEST_ASSERT_EQUAL(0, stats.safety_updates);
    TEST_ASSERT_EQUAL(0, stats.system_updates);
    TEST_ASSERT_EQUAL(0, stats.events_sent);
    TEST_ASSERT_EQUAL(0, stats.json_serializations);
    
    telemetry_manager_deinit();
}

// ============================================================================
// ERROR HANDLING TESTS
// ============================================================================

void test_telemetry_manager_error_handling(void)
{
    setUp();
    
    // Test operations without initialization
    telemetry_data_t data;
    hal_status_t status = telemetry_manager_get_data(&data);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_update_location(&test_location);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_update_navigation(&test_navigation);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_update_dock(&test_dock);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_update_safety(&test_safety);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_update_status(&test_status);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_set_callback(test_event_callback);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    telemetry_stats_t stats;
    status = telemetry_manager_get_statistics(&stats);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_reset_statistics();
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    // Test with NULL parameters
    status = telemetry_manager_init(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_OK, status);
    
    status = telemetry_manager_get_data(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_update_location(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_update_navigation(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_update_dock(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_update_safety(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_update_status(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    status = telemetry_manager_get_statistics(NULL);
    TEST_ASSERT_EQUAL(HAL_STATUS_INVALID_PARAMETER, status);
    
    // Test JSON serialization with NULL parameters
    char json_buffer[1024];
    int result = telemetry_manager_serialize_json(NULL, json_buffer, sizeof(json_buffer));
    TEST_ASSERT_EQUAL(-1, result);
    
    telemetry_data_t test_data;
    result = telemetry_manager_serialize_json(&test_data, NULL, sizeof(json_buffer));
    TEST_ASSERT_EQUAL(-1, result);
    
    result = telemetry_manager_serialize_json(&test_data, json_buffer, 0);
    TEST_ASSERT_EQUAL(-1, result);
    
    telemetry_manager_deinit();
}

// ============================================================================
// MAIN TEST RUNNER
// ============================================================================

int main(void)
{
    UNITY_BEGIN();
    
    // Constants tests
    RUN_TEST(test_telemetry_status_constants);
    RUN_TEST(test_telemetry_event_constants);
    RUN_TEST(test_location_status_constants);
    RUN_TEST(test_navigation_status_constants);
    RUN_TEST(test_dock_status_constants);
    RUN_TEST(test_encoder_health_constants);
    
    // Data structures tests
    RUN_TEST(test_telemetry_data_structures);
    
    // Initialization tests
    RUN_TEST(test_telemetry_manager_initialization);
    RUN_TEST(test_telemetry_manager_custom_config);
    
    // Data update tests
    RUN_TEST(test_telemetry_manager_update_location);
    RUN_TEST(test_telemetry_manager_update_navigation);
    RUN_TEST(test_telemetry_manager_update_dock);
    RUN_TEST(test_telemetry_manager_update_safety);
    RUN_TEST(test_telemetry_manager_update_status);
    
    // Event callback tests
    RUN_TEST(test_telemetry_manager_event_callback);
    RUN_TEST(test_telemetry_manager_safety_alert_event);
    
    // JSON serialization tests
    RUN_TEST(test_telemetry_manager_json_serialization);
    
    // Statistics tests
    RUN_TEST(test_telemetry_manager_statistics);
    
    // Error handling tests
    RUN_TEST(test_telemetry_manager_error_handling);
    
    UNITY_END();
    return 0;
}
