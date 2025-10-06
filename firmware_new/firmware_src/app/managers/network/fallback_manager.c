/**
 * @file fallback_manager.c
 * @brief Fallback Manager Implementation - OHT-50 Robot Network Fallback Management
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Implementation của Fallback Manager cho OHT-50 robots
 * cung cấp quản lý fallback connectivity.
 */

#include "fallback_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

// Fallback Manager State
static bool fallback_manager_initialized = false;
static bool fallback_monitoring_active = false;
static fallback_config_t current_config = {0};
static fallback_status_info_t current_status = {0};
static fallback_statistics_t statistics = {0};
static time_t last_connection_check = 0;
static time_t last_fallback_time = 0;
static time_t last_recovery_time = 0;
static uint32_t connection_attempts = 0;
static uint32_t recovery_attempts = 0;

// WiFi and AP parameters for recovery
static char wifi_ssid[32] = "OHT-50-Network";
static char wifi_password[64] = "secure_password_2025";
static char ap_ssid[32] = "OHT-50-Hotspot";
static char ap_password[64] = "oht50_secure_2025";

// Default Configuration
static const fallback_config_t default_config = {
    .auto_fallback_enabled = true,
    .connection_timeout_ms = 30000,      // 30 seconds
    .retry_interval_ms = 5000,           // 5 seconds
    .max_retry_attempts = 3,
    .ap_mode_enabled = true,
    .ap_startup_timeout_ms = 10000,      // 10 seconds
    .recovery_check_interval_ms = 10000, // 10 seconds
    .signal_strength_threshold_dbm = -70,
    .aggressive_fallback = false
};

// Internal Functions
static void update_fallback_status(void);
static void update_statistics(void);
static int check_wifi_connection_quality(void);
static int attempt_wifi_reconnection(void);
static int start_ap_mode(void);
static int stop_ap_mode(void);
static bool should_trigger_fallback(void);
static bool should_attempt_recovery(void);

/**
 * @brief Initialize Fallback Manager
 */
int fallback_manager_init(void) {
    if (fallback_manager_initialized) {
        return FALLBACK_SUCCESS;
    }
    
    printf("[FALLBACK_MANAGER] Initializing Fallback Manager...\n");
    
    // Initialize with default configuration
    memcpy(&current_config, &default_config, sizeof(fallback_config_t));
    
    // Initialize status
    current_status.status = FALLBACK_STATUS_DISABLED;
    current_status.auto_fallback_enabled = current_config.auto_fallback_enabled;
    current_status.wifi_connected = false;
    current_status.ap_running = false;
    current_status.wifi_signal_strength_dbm = 0;
    current_status.connection_attempts = 0;
    current_status.fallback_triggers = 0;
    current_status.recovery_attempts = 0;
    current_status.last_fallback_time = 0;
    current_status.last_recovery_time = 0;
    strncpy(current_status.current_ssid, "", sizeof(current_status.current_ssid) - 1);
    strncpy(current_status.ap_ssid, ap_ssid, sizeof(current_status.ap_ssid) - 1);
    
    // Initialize statistics
    memset(&statistics, 0, sizeof(fallback_statistics_t));
    
    // Initialize timers
    last_connection_check = time(NULL);
    last_fallback_time = 0;
    last_recovery_time = 0;
    connection_attempts = 0;
    recovery_attempts = 0;
    
    fallback_manager_initialized = true;
    
    printf("[FALLBACK_MANAGER] Initialized successfully\n");
    return FALLBACK_SUCCESS;
}

/**
 * @brief Deinitialize Fallback Manager
 */
int fallback_manager_deinit(void) {
    if (!fallback_manager_initialized) {
        return FALLBACK_SUCCESS;
    }
    
    printf("[FALLBACK_MANAGER] Deinitializing Fallback Manager...\n");
    
    // Stop monitoring if active
    if (fallback_monitoring_active) {
        fallback_manager_stop_monitoring();
    }
    
    fallback_manager_initialized = false;
    
    printf("[FALLBACK_MANAGER] Deinitialized successfully\n");
    return FALLBACK_SUCCESS;
}

/**
 * @brief Start fallback monitoring
 */
int fallback_manager_start_monitoring(void) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    if (fallback_monitoring_active) {
        return FALLBACK_ERROR_ALREADY_RUNNING;
    }
    
    printf("[FALLBACK_MANAGER] Starting fallback monitoring...\n");
    
    fallback_monitoring_active = true;
    current_status.status = FALLBACK_STATUS_MONITORING;
    
    update_fallback_status();
    update_statistics();
    
    printf("[FALLBACK_MANAGER] Fallback monitoring started\n");
    return FALLBACK_SUCCESS;
}

/**
 * @brief Stop fallback monitoring
 */
int fallback_manager_stop_monitoring(void) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    if (!fallback_monitoring_active) {
        return FALLBACK_ERROR_NOT_RUNNING;
    }
    
    printf("[FALLBACK_MANAGER] Stopping fallback monitoring...\n");
    
    fallback_monitoring_active = false;
    current_status.status = FALLBACK_STATUS_DISABLED;
    
    update_fallback_status();
    update_statistics();
    
    printf("[FALLBACK_MANAGER] Fallback monitoring stopped\n");
    return FALLBACK_SUCCESS;
}

/**
 * @brief Enable/disable auto-fallback
 */
int fallback_manager_enable_auto_fallback(bool enabled) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    current_config.auto_fallback_enabled = enabled;
    current_status.auto_fallback_enabled = enabled;
    
    printf("[FALLBACK_MANAGER] Auto-fallback %s\n", enabled ? "enabled" : "disabled");
    return FALLBACK_SUCCESS;
}

/**
 * @brief Check WiFi connection status
 */
int fallback_manager_check_connection(void) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    // Check WiFi connection status
    bool wifi_connected = wifi_manager_is_connected();
    int signal_strength = wifi_manager_get_signal_strength();
    
    current_status.wifi_connected = wifi_connected;
    current_status.wifi_signal_strength_dbm = signal_strength;
    
    if (wifi_connected) {
        wifi_manager_get_current_ssid(current_status.current_ssid);
        return FALLBACK_SUCCESS;
    } else {
        return FALLBACK_ERROR_WIFI_MANAGER_FAILED;
    }
}

/**
 * @brief Trigger fallback to AP mode
 */
int fallback_manager_trigger_fallback(void) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    printf("[FALLBACK_MANAGER] Triggering fallback to AP mode...\n");
    
    current_status.status = FALLBACK_STATUS_TRIGGERED;
    last_fallback_time = time(NULL);
    current_status.last_fallback_time = last_fallback_time;
    current_status.fallback_triggers++;
    statistics.total_fallback_triggers++;
    
    // Start AP mode
    int result = start_ap_mode();
    if (result != FALLBACK_SUCCESS) {
        current_status.status = FALLBACK_STATUS_ERROR;
        printf("[FALLBACK_MANAGER] Failed to start AP mode: %s\n", 
               fallback_manager_get_error_message(result));
        return result;
    }
    
    current_status.ap_running = true;
    update_fallback_status();
    update_statistics();
    
    printf("[FALLBACK_MANAGER] Fallback to AP mode successful\n");
    return FALLBACK_SUCCESS;
}

/**
 * @brief Attempt recovery to WiFi
 */
int fallback_manager_attempt_recovery(void) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    printf("[FALLBACK_MANAGER] Attempting recovery to WiFi...\n");
    
    current_status.status = FALLBACK_STATUS_RECOVERING;
    last_recovery_time = time(NULL);
    current_status.last_recovery_time = last_recovery_time;
    current_status.recovery_attempts++;
    statistics.total_recovery_attempts++;
    
    // Stop AP mode
    int result = stop_ap_mode();
    if (result != FALLBACK_SUCCESS) {
        printf("[FALLBACK_MANAGER] Warning: Failed to stop AP mode: %s\n", 
               fallback_manager_get_error_message(result));
    }
    
    // Attempt WiFi reconnection
    result = attempt_wifi_reconnection();
    if (result == FALLBACK_SUCCESS) {
        current_status.wifi_connected = true;
        current_status.ap_running = false;
        current_status.status = FALLBACK_STATUS_MONITORING;
        statistics.successful_recoveries++;
        
        printf("[FALLBACK_MANAGER] Recovery to WiFi successful\n");
    } else {
        current_status.status = FALLBACK_STATUS_TRIGGERED;
        statistics.failed_recoveries++;
        
        // Restart AP mode if recovery failed
        start_ap_mode();
        current_status.ap_running = true;
        
        printf("[FALLBACK_MANAGER] Recovery to WiFi failed, staying in AP mode\n");
    }
    
    update_fallback_status();
    update_statistics();
    
    return result;
}

/**
 * @brief Get fallback status
 */
int fallback_manager_get_status(fallback_status_info_t *status) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    if (!status) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    // Update current status
    update_fallback_status();
    
    // Copy status
    memcpy(status, &current_status, sizeof(fallback_status_info_t));
    
    return FALLBACK_SUCCESS;
}

/**
 * @brief Get fallback configuration
 */
int fallback_manager_get_config(fallback_config_t *config) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    memcpy(config, &current_config, sizeof(fallback_config_t));
    return FALLBACK_SUCCESS;
}

/**
 * @brief Set fallback configuration
 */
int fallback_manager_set_config(const fallback_config_t *config) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    // Validate configuration
    int validation_result = fallback_manager_validate_config(config);
    if (validation_result != FALLBACK_SUCCESS) {
        return validation_result;
    }
    
    // Update configuration
    memcpy(&current_config, config, sizeof(fallback_config_t));
    current_status.auto_fallback_enabled = config->auto_fallback_enabled;
    
    printf("[FALLBACK_MANAGER] Fallback configuration updated\n");
    return FALLBACK_SUCCESS;
}

/**
 * @brief Get fallback statistics
 */
int fallback_manager_get_statistics(fallback_statistics_t *stats) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    if (!stats) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    // Update statistics
    update_statistics();
    
    memcpy(stats, &statistics, sizeof(fallback_statistics_t));
    return FALLBACK_SUCCESS;
}

/**
 * @brief Reset fallback statistics
 */
int fallback_manager_reset_statistics(void) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    memset(&statistics, 0, sizeof(fallback_statistics_t));
    
    printf("[FALLBACK_MANAGER] Statistics reset\n");
    return FALLBACK_SUCCESS;
}

/**
 * @brief Check if fallback is enabled
 */
bool fallback_manager_is_enabled(void) {
    if (!fallback_manager_initialized) {
        return false;
    }
    
    return current_config.auto_fallback_enabled;
}

/**
 * @brief Check if monitoring is active
 */
bool fallback_manager_is_monitoring(void) {
    if (!fallback_manager_initialized) {
        return false;
    }
    
    return fallback_monitoring_active;
}

/**
 * @brief Get error message for error code
 */
const char* fallback_manager_get_error_message(int error_code) {
    switch (error_code) {
        case FALLBACK_SUCCESS:
            return "Success";
        case FALLBACK_ERROR_INVALID_PARAM:
            return "Invalid parameter";
        case FALLBACK_ERROR_INIT_FAILED:
            return "Initialization failed";
        case FALLBACK_ERROR_NOT_INITIALIZED:
            return "Manager not initialized";
        case FALLBACK_ERROR_ALREADY_RUNNING:
            return "Already running";
        case FALLBACK_ERROR_NOT_RUNNING:
            return "Not running";
        case FALLBACK_ERROR_WIFI_MANAGER_FAILED:
            return "WiFi manager operation failed";
        case FALLBACK_ERROR_AP_MANAGER_FAILED:
            return "AP manager operation failed";
        case FALLBACK_ERROR_TIMEOUT:
            return "Operation timeout";
        default:
            return "Unknown error";
    }
}

/**
 * @brief Validate fallback configuration
 */
int fallback_manager_validate_config(const fallback_config_t *config) {
    if (!config) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    // Validate timeout values
    if (config->connection_timeout_ms < 1000 || config->connection_timeout_ms > 300000) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    if (config->retry_interval_ms < 1000 || config->retry_interval_ms > 60000) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    if (config->max_retry_attempts < 1 || config->max_retry_attempts > 10) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    if (config->ap_startup_timeout_ms < 5000 || config->ap_startup_timeout_ms > 60000) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    if (config->recovery_check_interval_ms < 5000 || config->recovery_check_interval_ms > 300000) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    if (config->signal_strength_threshold_dbm > -30 || config->signal_strength_threshold_dbm < -100) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    return FALLBACK_SUCCESS;
}

/**
 * @brief Set WiFi connection parameters for recovery
 */
int fallback_manager_set_wifi_params(const char *ssid, const char *password) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    if (!ssid || !password) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    strncpy(wifi_ssid, ssid, sizeof(wifi_ssid) - 1);
    strncpy(wifi_password, password, sizeof(wifi_password) - 1);
    
    printf("[FALLBACK_MANAGER] WiFi parameters updated: SSID=%s\n", ssid);
    return FALLBACK_SUCCESS;
}

/**
 * @brief Set AP configuration for fallback
 */
int fallback_manager_set_ap_params(const char *ssid, const char *password) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    if (!ssid || !password) {
        return FALLBACK_ERROR_INVALID_PARAM;
    }
    
    strncpy(ap_ssid, ssid, sizeof(ap_ssid) - 1);
    strncpy(ap_password, password, sizeof(ap_password) - 1);
    strncpy(current_status.ap_ssid, ssid, sizeof(current_status.ap_ssid) - 1);
    
    printf("[FALLBACK_MANAGER] AP parameters updated: SSID=%s\n", ssid);
    return FALLBACK_SUCCESS;
}

/**
 * @brief Get current connection type
 */
const char* fallback_manager_get_connection_type(void) {
    if (!fallback_manager_initialized) {
        return "Unknown";
    }
    
    if (current_status.ap_running) {
        return "AP Mode";
    } else if (current_status.wifi_connected) {
        return "WiFi";
    } else {
        return "Disconnected";
    }
}

/**
 * @brief Force switch to WiFi
 */
int fallback_manager_force_wifi(void) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    printf("[FALLBACK_MANAGER] Force switching to WiFi...\n");
    
    // Stop AP mode
    stop_ap_mode();
    current_status.ap_running = false;
    
    // Attempt WiFi connection
    int result = attempt_wifi_reconnection();
    if (result == FALLBACK_SUCCESS) {
        current_status.wifi_connected = true;
        current_status.status = FALLBACK_STATUS_MONITORING;
        printf("[FALLBACK_MANAGER] Force switch to WiFi successful\n");
    } else {
        current_status.wifi_connected = false;
        current_status.status = FALLBACK_STATUS_ERROR;
        printf("[FALLBACK_MANAGER] Force switch to WiFi failed\n");
    }
    
    update_fallback_status();
    return result;
}

/**
 * @brief Force switch to AP mode
 */
int fallback_manager_force_ap(void) {
    if (!fallback_manager_initialized) {
        return FALLBACK_ERROR_NOT_INITIALIZED;
    }
    
    printf("[FALLBACK_MANAGER] Force switching to AP mode...\n");
    
    // Disconnect WiFi
    wifi_manager_disconnect();
    current_status.wifi_connected = false;
    
    // Start AP mode
    int result = start_ap_mode();
    if (result == FALLBACK_SUCCESS) {
        current_status.ap_running = true;
        current_status.status = FALLBACK_STATUS_TRIGGERED;
        printf("[FALLBACK_MANAGER] Force switch to AP mode successful\n");
    } else {
        current_status.ap_running = false;
        current_status.status = FALLBACK_STATUS_ERROR;
        printf("[FALLBACK_MANAGER] Force switch to AP mode failed\n");
    }
    
    update_fallback_status();
    return result;
}

/**
 * @brief Get connection quality score
 */
int fallback_manager_get_connection_quality(void) {
    if (!fallback_manager_initialized) {
        return 0;
    }
    
    if (current_status.ap_running) {
        return 60; // AP mode quality score
    } else if (current_status.wifi_connected) {
        int signal = current_status.wifi_signal_strength_dbm;
        if (signal > -50) return 100;
        else if (signal > -60) return 90;
        else if (signal > -70) return 80;
        else if (signal > -80) return 70;
        else if (signal > -90) return 60;
        else return 50;
    } else {
        return 0;
    }
}

/**
 * @brief Handle fallback monitoring (call periodically)
 */
int fallback_manager_handle_monitoring(void) {
    if (!fallback_manager_initialized || !fallback_monitoring_active) {
        return FALLBACK_ERROR_NOT_RUNNING;
    }
    
    time_t current_time = time(NULL);
    
    // Check if it's time for connection check
    if (current_time - last_connection_check >= (current_config.retry_interval_ms / 1000)) {
        last_connection_check = current_time;
        
        // Check WiFi connection
        int connection_result = fallback_manager_check_connection();
        
        if (connection_result != FALLBACK_SUCCESS) {
            // WiFi disconnected, check if should trigger fallback
            if (should_trigger_fallback()) {
                fallback_manager_trigger_fallback();
            }
        } else {
            // WiFi connected, check if should attempt recovery from AP mode
            if (current_status.ap_running && should_attempt_recovery()) {
                fallback_manager_attempt_recovery();
            }
        }
    }
    
    update_fallback_status();
    update_statistics();
    
    return FALLBACK_SUCCESS;
}

// Internal Functions Implementation

/**
 * @brief Update fallback status
 */
static void update_fallback_status(void) {
    // Update connection status
    current_status.wifi_connected = wifi_manager_is_connected();
    current_status.ap_running = wifi_ap_manager_is_running();
    
    if (current_status.wifi_connected) {
        current_status.wifi_signal_strength_dbm = wifi_manager_get_signal_strength();
        wifi_manager_get_current_ssid(current_status.current_ssid);
    }
    
    // Update connection attempts
    current_status.connection_attempts = connection_attempts;
    current_status.recovery_attempts = recovery_attempts;
}

/**
 * @brief Update statistics
 */
static void update_statistics(void) {
    // Calculate success rates
    if (statistics.total_fallback_triggers > 0) {
        statistics.fallback_success_rate = 100.0f; // Assume all fallbacks are successful for now
    }
    
    if (statistics.total_recovery_attempts > 0) {
        statistics.recovery_success_rate = (float)statistics.successful_recoveries / 
                                          statistics.total_recovery_attempts * 100.0f;
    }
    
    // Update uptime statistics (simplified)
    if (current_status.wifi_connected) {
        statistics.total_wifi_uptime_seconds++;
    }
    if (current_status.ap_running) {
        statistics.total_ap_uptime_seconds++;
    }
}

/**
 * @brief Check WiFi connection quality
 */
static int check_wifi_connection_quality(void) __attribute__((unused));
static int check_wifi_connection_quality(void) {
    if (!wifi_manager_is_connected()) {
        return 0;
    }
    
    int signal_strength = wifi_manager_get_signal_strength();
    return signal_strength;
}

/**
 * @brief Attempt WiFi reconnection
 */
static int attempt_wifi_reconnection(void) {
    printf("[FALLBACK_MANAGER] Attempting WiFi reconnection...\n");
    
    connection_attempts++;
    
    // Disconnect first
    wifi_manager_disconnect();
    usleep(1000000); // Wait 1 second
    
    // Attempt reconnection
    wifi_connection_params_t params = {
        .auto_connect = true,
        .connection_timeout_ms = current_config.connection_timeout_ms
    };
    strncpy(params.ssid, wifi_ssid, sizeof(params.ssid) - 1);
    strncpy(params.password, wifi_password, sizeof(params.password) - 1);
    params.security_type = AP_SECURITY_WPA2;
    
    int result = wifi_manager_connect(&params);
    if (result == WIFI_SUCCESS) {
        printf("[FALLBACK_MANAGER] WiFi reconnection successful\n");
        return FALLBACK_SUCCESS;
    } else {
        printf("[FALLBACK_MANAGER] WiFi reconnection failed: %s\n", 
               wifi_manager_get_error_message(result));
        return FALLBACK_ERROR_WIFI_MANAGER_FAILED;
    }
}

/**
 * @brief Start AP mode
 */
static int start_ap_mode(void) {
    printf("[FALLBACK_MANAGER] Starting AP mode...\n");
    
    wifi_ap_config_t ap_config = {
        .ap_enabled = true,
        .ap_channel = 6,
        .max_clients = 10,
        .auto_fallback = false,
        .fallback_timeout_ms = 0
    };
    strncpy(ap_config.ap_ssid, ap_ssid, sizeof(ap_config.ap_ssid) - 1);
    strncpy(ap_config.ap_password, ap_password, sizeof(ap_config.ap_password) - 1);
    strncpy(ap_config.ap_ip, "192.168.4.1", sizeof(ap_config.ap_ip) - 1);
    strncpy(ap_config.ap_netmask, "255.255.255.0", sizeof(ap_config.ap_netmask) - 1);
    ap_config.ap_security_type = AP_SECURITY_WPA2;
    
    int result = wifi_ap_manager_start(&ap_config);
    if (result == WIFI_AP_SUCCESS) {
        printf("[FALLBACK_MANAGER] AP mode started successfully\n");
        return FALLBACK_SUCCESS;
    } else {
        printf("[FALLBACK_MANAGER] AP mode start failed: %s\n", 
               wifi_ap_manager_get_error_message(result));
        return FALLBACK_ERROR_AP_MANAGER_FAILED;
    }
}

/**
 * @brief Stop AP mode
 */
static int stop_ap_mode(void) {
    printf("[FALLBACK_MANAGER] Stopping AP mode...\n");
    
    int result = wifi_ap_manager_stop();
    if (result == WIFI_AP_SUCCESS) {
        printf("[FALLBACK_MANAGER] AP mode stopped successfully\n");
        return FALLBACK_SUCCESS;
    } else {
        printf("[FALLBACK_MANAGER] AP mode stop failed: %s\n", 
               wifi_ap_manager_get_error_message(result));
        return FALLBACK_ERROR_AP_MANAGER_FAILED;
    }
}

/**
 * @brief Check if should trigger fallback
 */
static bool should_trigger_fallback(void) {
    if (!current_config.auto_fallback_enabled) {
        return false;
    }
    
    // Check if AP mode is enabled
    if (!current_config.ap_mode_enabled) {
        return false;
    }
    
    // Check if we've exceeded max retry attempts
    if (connection_attempts >= (uint32_t)current_config.max_retry_attempts) {
        return true;
    }
    
    // Check signal strength threshold
    if (current_status.wifi_signal_strength_dbm < current_config.signal_strength_threshold_dbm) {
        return true;
    }
    
    return false;
}

/**
 * @brief Check if should attempt recovery
 */
static bool should_attempt_recovery(void) {
    if (!current_config.auto_fallback_enabled) {
        return false;
    }
    
    // Check if enough time has passed since last fallback
    time_t current_time = time(NULL);
    if (current_time - last_fallback_time < (current_config.recovery_check_interval_ms / 1000)) {
        return false;
    }
    
    // Check if we haven't exceeded max recovery attempts
    if (recovery_attempts >= (uint32_t)current_config.max_retry_attempts) {
        return false;
    }
    
    return true;
}
