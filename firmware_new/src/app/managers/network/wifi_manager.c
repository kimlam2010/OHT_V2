/**
 * @file wifi_manager.c
 * @brief WiFi Manager Implementation - OHT-50 Robot WiFi Connectivity
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Implementation của WiFi Manager cho OHT-50 robots
 * hỗ trợ scanning, connection, roaming, và signal monitoring.
 */

#include "wifi_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>

// WiFi Manager State
static bool wifi_manager_initialized = false;
static wifi_roaming_config_t roaming_config = {0};
static wifi_statistics_t wifi_stats = {0};
static wifi_connection_status_t current_status = WIFI_STATUS_DISCONNECTED;
static char current_ssid[32] = {0};
static int current_signal_strength = 0;
static pthread_mutex_t wifi_mutex = PTHREAD_MUTEX_INITIALIZER;

// Internal Functions
static wifi_signal_quality_t get_signal_quality(int signal_dbm);
static bool should_roam(int current_signal, int threshold);
static uint64_t get_timestamp_ms(void);
static void update_statistics(bool connection_success);
static void log_wifi_event(const char *event, const char *details);

// Error Messages
static const char* wifi_error_messages[] = {
    "Success",
    "Invalid parameter",
    "Initialization failed",
    "Connection failed",
    "Timeout",
    "Authentication failed",
    "Scan failed",
    "Roaming failed",
    "Signal too weak",
    "Not initialized"
};

/**
 * @brief Initialize WiFi Manager
 */
int wifi_manager_init(void) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_SUCCESS;
    }
    
    // Initialize default roaming configuration
    roaming_config.enabled = true;
    roaming_config.signal_threshold_dbm = -70;
    roaming_config.scan_interval_ms = 5000;
    roaming_config.handover_timeout_ms = 2000;
    roaming_config.aggressive_roaming = false;
    
    // Initialize statistics
    memset(&wifi_stats, 0, sizeof(wifi_statistics_t));
    wifi_stats.connection_success_rate = 100.0f;
    
    // Initialize current status
    current_status = WIFI_STATUS_DISCONNECTED;
    memset(current_ssid, 0, sizeof(current_ssid));
    current_signal_strength = 0;
    
    wifi_manager_initialized = true;
    
    printf("[WIFI_MANAGER] Initialized successfully\n");
    log_wifi_event("INIT", "WiFi Manager initialized");
    
    pthread_mutex_unlock(&wifi_mutex);
    return WIFI_SUCCESS;
}

/**
 * @brief Deinitialize WiFi Manager
 */
int wifi_manager_deinit(void) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (!wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_NOT_INITIALIZED;
    }
    
    // Disconnect if connected
    if (current_status == WIFI_STATUS_CONNECTED) {
        wifi_manager_disconnect();
    }
    
    wifi_manager_initialized = false;
    
    printf("[WIFI_MANAGER] Deinitialized successfully\n");
    log_wifi_event("DEINIT", "WiFi Manager deinitialized");
    
    pthread_mutex_unlock(&wifi_mutex);
    return WIFI_SUCCESS;
}

/**
 * @brief Scan for available WiFi networks
 */
int wifi_manager_scan_networks(wifi_scan_result_t *results, int max_count, uint32_t timeout_ms) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (!wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_NOT_INITIALIZED;
    }
    
    if (!results || max_count <= 0) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_INVALID_PARAM;
    }
    
    printf("[WIFI_MANAGER] Scanning for WiFi networks (timeout: %u ms)\n", timeout_ms);
    
    // Mock scan results for development
    int networks_found = 0;
    
    if (max_count >= 5) {
        // Network 1: OHT-50-Network (Primary)
        strcpy(results[0].ssid, "OHT-50-Network");
        strcpy(results[0].bssid, "00:11:22:33:44:55");
        results[0].signal_strength_dbm = -45;
        results[0].signal_quality = get_signal_quality(-45);
        results[0].frequency_mhz = 5000;
        results[0].channel = 36;
        results[0].security_type = OHT_WIFI_SECURITY_WPA2;
        results[0].band = WIFI_BAND_5G;
        results[0].hidden = false;
        results[0].connected = (strcmp(current_ssid, "OHT-50-Network") == 0);
        networks_found++;
        
        // Network 2: OHT-50-Backup
        strcpy(results[1].ssid, "OHT-50-Backup");
        strcpy(results[1].bssid, "00:11:22:33:44:66");
        results[1].signal_strength_dbm = -55;
        results[1].signal_quality = get_signal_quality(-55);
        results[1].frequency_mhz = 2400;
        results[1].channel = 6;
        results[1].security_type = OHT_WIFI_SECURITY_WPA3;
        results[1].band = WIFI_BAND_2_4G;
        results[1].hidden = false;
        results[1].connected = (strcmp(current_ssid, "OHT-50-Backup") == 0);
        networks_found++;
        
        // Network 3: OHT-50-Emergency
        strcpy(results[2].ssid, "OHT-50-Emergency");
        strcpy(results[2].bssid, "00:11:22:33:44:77");
        results[2].signal_strength_dbm = -65;
        results[2].signal_quality = get_signal_quality(-65);
        results[2].frequency_mhz = 5000;
        results[2].channel = 149;
        results[2].security_type = OHT_WIFI_SECURITY_WPA2;
        results[2].band = WIFI_BAND_5G;
        results[2].hidden = false;
        results[2].connected = (strcmp(current_ssid, "OHT-50-Emergency") == 0);
        networks_found++;
        
        // Network 4: OHT-50-Mobile
        strcpy(results[3].ssid, "OHT-50-Mobile");
        strcpy(results[3].bssid, "00:11:22:33:44:88");
        results[3].signal_strength_dbm = -60;
        results[3].signal_quality = get_signal_quality(-60);
        results[3].frequency_mhz = 2400;
        results[3].channel = 11;
        results[3].security_type = OHT_WIFI_SECURITY_WPA2;
        results[3].band = WIFI_BAND_2_4G;
        results[3].hidden = false;
        results[3].connected = (strcmp(current_ssid, "OHT-50-Mobile") == 0);
        networks_found++;
        
        // Network 5: OHT-50-Test
        strcpy(results[4].ssid, "OHT-50-Test");
        strcpy(results[4].bssid, "00:11:22:33:44:99");
        results[4].signal_strength_dbm = -75;
        results[4].signal_quality = get_signal_quality(-75);
        results[4].frequency_mhz = 5000;
        results[4].channel = 165;
        results[4].security_type = OHT_WIFI_SECURITY_WPA3;
        results[4].band = WIFI_BAND_5G;
        results[4].hidden = false;
        results[4].connected = (strcmp(current_ssid, "OHT-50-Test") == 0);
        networks_found++;
    }
    
    pthread_mutex_unlock(&wifi_mutex);
    
    printf("[WIFI_MANAGER] Found %d WiFi networks\n", networks_found);
    log_wifi_event("SCAN", "WiFi scan completed");
    
    return networks_found;
}

/**
 * @brief Connect to WiFi network
 */
int wifi_manager_connect(const wifi_connection_params_t *params) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (!wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_NOT_INITIALIZED;
    }
    
    if (!params) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_INVALID_PARAM;
    }
    
    // Validate parameters
    if (strlen(params->ssid) == 0 || strlen(params->ssid) > 32) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_INVALID_PARAM;
    }
    
    if (strlen(params->password) < 8 || strlen(params->password) > 64) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_INVALID_PARAM;
    }
    
    printf("[WIFI_MANAGER] Connecting to WiFi: %s\n", params->ssid);
    log_wifi_event("CONNECT_ATTEMPT", params->ssid);
    
    current_status = WIFI_STATUS_CONNECTING;
    
    // Mock connection process
    usleep(2000000); // 2 second delay
    
    // Simulate connection success/failure
    bool connection_success = true;
    
    if (connection_success) {
        current_status = WIFI_STATUS_CONNECTED;
        strncpy(current_ssid, params->ssid, sizeof(current_ssid) - 1);
        current_signal_strength = -45; // Mock signal strength
        
        update_statistics(true);
        
        printf("[WIFI_MANAGER] Connected to WiFi: %s (Signal: %d dBm)\n", 
               params->ssid, current_signal_strength);
        log_wifi_event("CONNECT_SUCCESS", params->ssid);
    } else {
        current_status = WIFI_STATUS_FAILED;
        memset(current_ssid, 0, sizeof(current_ssid));
        current_signal_strength = 0;
        
        update_statistics(false);
        
        printf("[WIFI_MANAGER] Failed to connect to WiFi: %s\n", params->ssid);
        log_wifi_event("CONNECT_FAILED", params->ssid);
    }
    
    pthread_mutex_unlock(&wifi_mutex);
    
    return connection_success ? WIFI_SUCCESS : WIFI_ERROR_CONNECTION_FAILED;
}

/**
 * @brief Disconnect from current WiFi network
 */
int wifi_manager_disconnect(void) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (!wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_NOT_INITIALIZED;
    }
    
    if (current_status != WIFI_STATUS_CONNECTED) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_SUCCESS; // Already disconnected
    }
    
    printf("[WIFI_MANAGER] Disconnecting from WiFi: %s\n", current_ssid);
    log_wifi_event("DISCONNECT", current_ssid);
    
    current_status = WIFI_STATUS_DISCONNECTED;
    wifi_stats.disconnection_events++;
    
    memset(current_ssid, 0, sizeof(current_ssid));
    current_signal_strength = 0;
    
    printf("[WIFI_MANAGER] Disconnected from WiFi\n");
    
    pthread_mutex_unlock(&wifi_mutex);
    return WIFI_SUCCESS;
}

/**
 * @brief Get current WiFi connection status
 */
wifi_connection_status_t wifi_manager_get_connection_status(void) {
    pthread_mutex_lock(&wifi_mutex);
    wifi_connection_status_t status = current_status;
    pthread_mutex_unlock(&wifi_mutex);
    return status;
}

/**
 * @brief Get current signal strength
 */
int wifi_manager_get_signal_strength(void) {
    pthread_mutex_lock(&wifi_mutex);
    int signal = current_signal_strength;
    pthread_mutex_unlock(&wifi_mutex);
    return signal;
}

/**
 * @brief Get signal quality level
 */
wifi_signal_quality_t wifi_manager_get_signal_quality(void) {
    pthread_mutex_lock(&wifi_mutex);
    wifi_signal_quality_t quality = get_signal_quality(current_signal_strength);
    pthread_mutex_unlock(&wifi_mutex);
    return quality;
}

/**
 * @brief Enable/disable WiFi roaming
 */
int wifi_manager_enable_roaming(const wifi_roaming_config_t *config) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (!wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_INVALID_PARAM;
    }
    
    roaming_config = *config;
    
    if (config->enabled) {
        printf("[WIFI_MANAGER] WiFi roaming enabled (threshold: %d dBm)\n", 
               config->signal_threshold_dbm);
        log_wifi_event("ROAMING_ENABLED", "WiFi roaming enabled");
    } else {
        printf("[WIFI_MANAGER] WiFi roaming disabled\n");
        log_wifi_event("ROAMING_DISABLED", "WiFi roaming disabled");
    }
    
    pthread_mutex_unlock(&wifi_mutex);
    return WIFI_SUCCESS;
}

/**
 * @brief Handle WiFi roaming (call periodically)
 */
int wifi_manager_handle_roaming(void) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (!wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_NOT_INITIALIZED;
    }
    
    if (!roaming_config.enabled || current_status != WIFI_STATUS_CONNECTED) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_SUCCESS;
    }
    
    // Check if roaming is needed
    if (should_roam(current_signal_strength, roaming_config.signal_threshold_dbm)) {
        printf("[WIFI_MANAGER] Signal weak (%d dBm), initiating roaming\n", current_signal_strength);
        log_wifi_event("ROAMING_INITIATED", "Signal too weak");
        
        current_status = WIFI_STATUS_ROAMING;
        wifi_stats.roaming_events++;
        
        // Mock roaming process
        usleep(roaming_config.handover_timeout_ms * 1000);
        
        // Simulate successful roaming
        current_status = WIFI_STATUS_CONNECTED;
        current_signal_strength = -50; // Improved signal
        
        printf("[WIFI_MANAGER] Roaming completed (new signal: %d dBm)\n", current_signal_strength);
        log_wifi_event("ROAMING_COMPLETED", "Roaming successful");
    }
    
    pthread_mutex_unlock(&wifi_mutex);
    return WIFI_SUCCESS;
}

/**
 * @brief Get WiFi statistics
 */
int wifi_manager_get_statistics(wifi_statistics_t *stats) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (!wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_NOT_INITIALIZED;
    }
    
    if (!stats) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_INVALID_PARAM;
    }
    
    *stats = wifi_stats;
    
    pthread_mutex_unlock(&wifi_mutex);
    return WIFI_SUCCESS;
}

/**
 * @brief Reset WiFi statistics
 */
int wifi_manager_reset_statistics(void) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (!wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_NOT_INITIALIZED;
    }
    
    memset(&wifi_stats, 0, sizeof(wifi_statistics_t));
    wifi_stats.connection_success_rate = 100.0f;
    
    printf("[WIFI_MANAGER] Statistics reset\n");
    log_wifi_event("STATS_RESET", "WiFi statistics reset");
    
    pthread_mutex_unlock(&wifi_mutex);
    return WIFI_SUCCESS;
}

/**
 * @brief Get WiFi configuration
 */
int wifi_manager_get_config(wifi_roaming_config_t *config) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (!wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_INVALID_PARAM;
    }
    
    *config = roaming_config;
    
    pthread_mutex_unlock(&wifi_mutex);
    return WIFI_SUCCESS;
}

/**
 * @brief Set WiFi configuration
 */
int wifi_manager_set_config(const wifi_roaming_config_t *config) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (!wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_INVALID_PARAM;
    }
    
    roaming_config = *config;
    
    pthread_mutex_unlock(&wifi_mutex);
    return WIFI_SUCCESS;
}

/**
 * @brief Get error message for error code
 */
const char* wifi_manager_get_error_message(int error_code) {
    int index = -error_code;
    if (index >= 0 && index < (int)(sizeof(wifi_error_messages) / sizeof(wifi_error_messages[0]))) {
        return wifi_error_messages[index];
    }
    return "Unknown error";
}

/**
 * @brief Check if WiFi is connected
 */
bool wifi_manager_is_connected(void) {
    pthread_mutex_lock(&wifi_mutex);
    bool connected = (current_status == WIFI_STATUS_CONNECTED);
    pthread_mutex_unlock(&wifi_mutex);
    return connected;
}

/**
 * @brief Get current SSID
 */
int wifi_manager_get_current_ssid(char *ssid) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (!wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_NOT_INITIALIZED;
    }
    
    if (!ssid) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_INVALID_PARAM;
    }
    
    strncpy(ssid, current_ssid, 31);
    ssid[31] = '\0';
    
    pthread_mutex_unlock(&wifi_mutex);
    return WIFI_SUCCESS;
}

/**
 * @brief Force WiFi reconnection
 */
int wifi_manager_reconnect(void) {
    pthread_mutex_lock(&wifi_mutex);
    
    if (!wifi_manager_initialized) {
        pthread_mutex_unlock(&wifi_mutex);
        return WIFI_ERROR_NOT_INITIALIZED;
    }
    
    if (current_status == WIFI_STATUS_CONNECTED) {
        printf("[WIFI_MANAGER] Forcing reconnection to: %s\n", current_ssid);
        log_wifi_event("RECONNECT", current_ssid);
        
        // Disconnect first
        current_status = WIFI_STATUS_DISCONNECTED;
        wifi_stats.disconnection_events++;
        
        // Reconnect
        current_status = WIFI_STATUS_CONNECTING;
        usleep(1000000); // 1 second delay
        
        current_status = WIFI_STATUS_CONNECTED;
        current_signal_strength = -40; // Improved signal
        
        update_statistics(true);
        
        printf("[WIFI_MANAGER] Reconnection successful (signal: %d dBm)\n", current_signal_strength);
    }
    
    pthread_mutex_unlock(&wifi_mutex);
    return WIFI_SUCCESS;
}

// Internal Functions Implementation

/**
 * @brief Get signal quality from dBm value
 */
static wifi_signal_quality_t get_signal_quality(int signal_dbm) {
    if (signal_dbm >= -30) {
        return WIFI_SIGNAL_EXCELLENT;
    } else if (signal_dbm >= -50) {
        return WIFI_SIGNAL_GOOD;
    } else if (signal_dbm >= -70) {
        return WIFI_SIGNAL_FAIR;
    } else if (signal_dbm >= -85) {
        return WIFI_SIGNAL_WEAK;
    } else {
        return WIFI_SIGNAL_NONE;
    }
}

/**
 * @brief Check if roaming should be initiated
 */
static bool should_roam(int current_signal, int threshold) {
    return current_signal < threshold;
}

/**
 * @brief Get current timestamp in milliseconds
 */
static uint64_t get_timestamp_ms(void) __attribute__((unused));
static uint64_t get_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/**
 * @brief Update WiFi statistics
 */
static void update_statistics(bool connection_success) {
    wifi_stats.connection_attempts++;
    
    if (connection_success) {
        wifi_stats.successful_connections++;
    } else {
        wifi_stats.failed_connections++;
    }
    
    wifi_stats.connection_success_rate = (float)wifi_stats.successful_connections * 100.0f / wifi_stats.connection_attempts;
}

/**
 * @brief Log WiFi event
 */
static void log_wifi_event(const char *event, const char *details) {
    printf("[WIFI_MANAGER] EVENT: %s - %s\n", event, details ? details : "");
}
