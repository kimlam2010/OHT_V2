/**
 * @file wifi_ap_manager.c
 * @brief WiFi AP Manager Implementation - OHT-50 Robot WiFi Access Point Management
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Implementation của WiFi AP Manager cho OHT-50 robots
 * cung cấp quản lý WiFi Access Point mode.
 */

#include "wifi_ap_manager.h"
#include "hal_wifi_ap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

// WiFi AP Manager State
static bool wifi_ap_manager_initialized = false;
static wifi_ap_config_t current_config = {0};
static wifi_ap_status_t current_status = {0};
static wifi_ap_statistics_t statistics = {0};
static time_t ap_start_time = 0;
static bool auto_fallback_enabled = false;
static uint32_t fallback_timeout_ms = 30000; // 30 seconds

// Default AP Configuration
static const wifi_ap_config_t default_config = {
    .ap_enabled = false,
    .ap_ssid = "OHT-50-Hotspot",
    .ap_password = "oht50_secure_2025",
    .ap_security_type = AP_SECURITY_WPA2,
    .ap_ip = "192.168.4.1",
    .ap_netmask = "255.255.255.0",
    .ap_channel = 6,
    .auto_fallback = true,
    .fallback_timeout_ms = 30000,
    .max_clients = 10
};

// Internal Functions
static void update_ap_status(void);
static void update_statistics(void);
static int validate_ap_config(const wifi_ap_config_t *config);

/**
 * @brief Initialize WiFi AP Manager
 */
int wifi_ap_manager_init(void) {
    if (wifi_ap_manager_initialized) {
        return WIFI_AP_SUCCESS;
    }
    
    printf("[WIFI_AP_MANAGER] Initializing WiFi AP Manager...\n");
    
    // Initialize HAL WiFi AP
    if (hal_wifi_ap_init() != HAL_STATUS_OK) {
        printf("[WIFI_AP_MANAGER] Failed to initialize HAL WiFi AP\n");
        return WIFI_AP_ERROR_HAL_FAILED;
    }
    
    // Initialize with default configuration
    memcpy(&current_config, &default_config, sizeof(wifi_ap_config_t));
    
    // Initialize status
    current_status.status = AP_STATUS_DISABLED;
    current_status.ap_enabled = false;
    current_status.connected_clients = 0;
    current_status.max_clients = current_config.max_clients;
    current_status.uptime_seconds = 0;
    current_status.total_bytes_sent = 0;
    current_status.total_bytes_received = 0;
    current_status.cpu_usage_percent = 0.0f;
    current_status.memory_usage_percent = 0.0f;
    
    // Initialize statistics
    memset(&statistics, 0, sizeof(wifi_ap_statistics_t));
    
    wifi_ap_manager_initialized = true;
    
    printf("[WIFI_AP_MANAGER] Initialized successfully\n");
    return WIFI_AP_SUCCESS;
}

/**
 * @brief Deinitialize WiFi AP Manager
 */
int wifi_ap_manager_deinit(void) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_SUCCESS;
    }
    
    printf("[WIFI_AP_MANAGER] Deinitializing WiFi AP Manager...\n");
    
    // Stop AP if running
    if (current_status.status == AP_STATUS_RUNNING) {
        wifi_ap_manager_stop();
    }
    
    // Deinitialize HAL
    hal_wifi_ap_deinit();
    
    wifi_ap_manager_initialized = false;
    
    printf("[WIFI_AP_MANAGER] Deinitialized successfully\n");
    return WIFI_AP_SUCCESS;
}

/**
 * @brief Start WiFi Access Point
 */
int wifi_ap_manager_start(const wifi_ap_config_t *config) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    // Validate configuration
    int validation_result = validate_ap_config(config);
    if (validation_result != WIFI_AP_SUCCESS) {
        return validation_result;
    }
    
    // Check if already running
    if (current_status.status == AP_STATUS_RUNNING) {
        return WIFI_AP_ERROR_ALREADY_RUNNING;
    }
    
    printf("[WIFI_AP_MANAGER] Starting WiFi AP: SSID=%s, Channel=%d\n", 
           config->ap_ssid, config->ap_channel);
    
    // Update status to starting
    current_status.status = AP_STATUS_STARTING;
    update_ap_status();
    
    // Start AP via HAL
    hal_wifi_ap_config_t hal_config;
    strncpy(hal_config.ssid, config->ap_ssid, sizeof(hal_config.ssid) - 1);
    hal_config.ssid[sizeof(hal_config.ssid) - 1] = '\0';
    strncpy(hal_config.password, config->ap_password, sizeof(hal_config.password) - 1);
    hal_config.password[sizeof(hal_config.password) - 1] = '\0';
    hal_config.security_type = (hal_ap_security_type_t)config->ap_security_type;
    strncpy(hal_config.ip_address, config->ap_ip, sizeof(hal_config.ip_address) - 1);
    hal_config.ip_address[sizeof(hal_config.ip_address) - 1] = '\0';
    strncpy(hal_config.netmask, config->ap_netmask, sizeof(hal_config.netmask) - 1);
    hal_config.netmask[sizeof(hal_config.netmask) - 1] = '\0';
    hal_config.channel = config->ap_channel;
    hal_config.max_clients = config->max_clients;
    
    if (hal_wifi_ap_start(&hal_config) != HAL_STATUS_OK) {
        current_status.status = AP_STATUS_ERROR;
        update_ap_status();
        printf("[WIFI_AP_MANAGER] Failed to start AP via HAL\n");
        return WIFI_AP_ERROR_HAL_FAILED;
    }
    
    // Update configuration
    memcpy(&current_config, config, sizeof(wifi_ap_config_t));
    
    // Update status to running
    current_status.status = AP_STATUS_RUNNING;
    current_status.ap_enabled = true;
    strncpy(current_status.ap_ssid, config->ap_ssid, sizeof(current_status.ap_ssid) - 1);
    strncpy(current_status.ap_ip, config->ap_ip, sizeof(current_status.ap_ip) - 1);
    current_status.ap_channel = config->ap_channel;
    current_status.max_clients = config->max_clients;
    ap_start_time = time(NULL);
    
    // Update statistics
    statistics.ap_start_count++;
    
    update_ap_status();
    update_statistics();
    
    printf("[WIFI_AP_MANAGER] WiFi AP started successfully\n");
    return WIFI_AP_SUCCESS;
}

/**
 * @brief Stop WiFi Access Point
 */
int wifi_ap_manager_stop(void) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    if (current_status.status != AP_STATUS_RUNNING) {
        return WIFI_AP_ERROR_NOT_RUNNING;
    }
    
    printf("[WIFI_AP_MANAGER] Stopping WiFi AP...\n");
    
    // Update status to stopping
    current_status.status = AP_STATUS_STOPPING;
    update_ap_status();
    
    // Stop AP via HAL
    if (hal_wifi_ap_stop() != HAL_STATUS_OK) {
        current_status.status = AP_STATUS_ERROR;
        update_ap_status();
        printf("[WIFI_AP_MANAGER] Failed to stop AP via HAL\n");
        return WIFI_AP_ERROR_HAL_FAILED;
    }
    
    // Update status to disabled
    current_status.status = AP_STATUS_DISABLED;
    current_status.ap_enabled = false;
    current_status.connected_clients = 0;
    current_status.uptime_seconds = 0;
    
    // Update statistics
    statistics.ap_stop_count++;
    if (ap_start_time > 0) {
        statistics.total_uptime_seconds += (time(NULL) - ap_start_time);
        ap_start_time = 0;
    }
    
    update_ap_status();
    update_statistics();
    
    printf("[WIFI_AP_MANAGER] WiFi AP stopped successfully\n");
    return WIFI_AP_SUCCESS;
}

/**
 * @brief Get WiFi AP status
 */
int wifi_ap_manager_get_status(wifi_ap_status_t *status) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    if (!status) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    // Update current status
    update_ap_status();
    
    // Copy status
    memcpy(status, &current_status, sizeof(wifi_ap_status_t));
    
    return WIFI_AP_SUCCESS;
}

/**
 * @brief Get real-time AP status (with live updates)
 */
int wifi_ap_manager_get_realtime_status(wifi_ap_status_t *status) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    if (!status) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    // Force update all metrics
    update_ap_status();
    
    // Get additional real-time metrics
    FILE *fp;
    char buffer[128];
    
    // Get signal strength (if available)
    fp = popen("iw dev wlan0 link | grep 'signal:' | awk '{print $2}'", "r");
    if (fp != NULL) {
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            // Signal strength is already in status structure
        }
        pclose(fp);
    }
    
    // Get connected clients with details
    if (current_status.status == AP_STATUS_RUNNING) {
        hal_wifi_ap_client_t hal_clients[20];
        int count = hal_wifi_ap_get_clients(hal_clients, 20);
        if (count >= 0) {
            current_status.connected_clients = count;
        }
    }
    
    memcpy(status, &current_status, sizeof(wifi_ap_status_t));
    
    return WIFI_AP_SUCCESS;
}

/**
 * @brief Get connected clients
 */
int wifi_ap_manager_get_clients(wifi_ap_client_t *clients, int max_count) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    if (!clients || max_count <= 0) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    if (current_status.status != AP_STATUS_RUNNING) {
        return WIFI_AP_ERROR_NOT_RUNNING;
    }
    
    // Get clients from HAL
    hal_wifi_ap_client_t hal_clients[max_count];
    int count = hal_wifi_ap_get_clients(hal_clients, max_count);
    
    if (count < 0) {
        return WIFI_AP_ERROR_HAL_FAILED;
    }
    
    // Convert HAL clients to manager clients
    for (int i = 0; i < count; i++) {
        strncpy(clients[i].mac_address, hal_clients[i].mac_address, sizeof(clients[i].mac_address) - 1);
        strncpy(clients[i].ip_address, hal_clients[i].ip_address, sizeof(clients[i].ip_address) - 1);
        strncpy(clients[i].hostname, hal_clients[i].hostname, sizeof(clients[i].hostname) - 1);
        clients[i].signal_strength_dbm = hal_clients[i].signal_strength_dbm;
        clients[i].connected_time_seconds = hal_clients[i].connected_time_seconds;
        clients[i].bytes_sent = hal_clients[i].bytes_sent;
        clients[i].bytes_received = hal_clients[i].bytes_received;
        clients[i].authenticated = hal_clients[i].authenticated;
    }
    
    return count;
}

/**
 * @brief Kick client from AP
 */
int wifi_ap_manager_kick_client(const char *mac_address) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    if (!mac_address) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    if (current_status.status != AP_STATUS_RUNNING) {
        return WIFI_AP_ERROR_NOT_RUNNING;
    }
    
    printf("[WIFI_AP_MANAGER] Kicking client: %s\n", mac_address);
    
    // Kick client via HAL
    if (hal_wifi_ap_kick_client(mac_address) != HAL_STATUS_OK) {
        printf("[WIFI_AP_MANAGER] Failed to kick client via HAL\n");
        return WIFI_AP_ERROR_HAL_FAILED;
    }
    
    printf("[WIFI_AP_MANAGER] Client kicked successfully\n");
    // statistics.clients_kicked++; // TODO: Add to statistics structure
    return WIFI_AP_SUCCESS;
}

/**
 * @brief Get client information by MAC address
 */
int wifi_ap_manager_get_client_info(const char *mac_address, wifi_ap_client_t *client) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    if (!mac_address || !client) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    if (current_status.status != AP_STATUS_RUNNING) {
        return WIFI_AP_ERROR_NOT_RUNNING;
    }
    
    // Validate MAC address format
    if (strlen(mac_address) != 17) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    // Get all clients and find the one with matching MAC
    wifi_ap_client_t clients[20];
    int client_count = wifi_ap_manager_get_clients(clients, 20);
    
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].mac_address, mac_address) == 0) {
            memcpy(client, &clients[i], sizeof(wifi_ap_client_t));
            return WIFI_AP_SUCCESS;
        }
    }
    
    return WIFI_AP_ERROR_CLIENT_NOT_FOUND;
}

/**
 * @brief Get client count
 */
int wifi_ap_manager_get_client_count(void) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    if (current_status.status != AP_STATUS_RUNNING) {
        return 0;
    }
    
    return current_status.connected_clients;
}

/**
 * @brief Check if client is connected
 */
bool wifi_ap_manager_is_client_connected(const char *mac_address) {
    if (!wifi_ap_manager_initialized || !mac_address) {
        return false;
    }
    
    if (current_status.status != AP_STATUS_RUNNING) {
        return false;
    }
    
    // Validate MAC address format
    if (strlen(mac_address) != 17) {
        return false;
    }
    
    // Get all clients and check if MAC exists
    wifi_ap_client_t clients[20];
    int client_count = wifi_ap_manager_get_clients(clients, 20);
    
    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].mac_address, mac_address) == 0) {
            return true;
        }
    }
    
    return false;
}

/**
 * @brief Get AP configuration
 */
int wifi_ap_manager_get_config(wifi_ap_config_t *config) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    memcpy(config, &current_config, sizeof(wifi_ap_config_t));
    return WIFI_AP_SUCCESS;
}

/**
 * @brief Set AP configuration
 */
int wifi_ap_manager_set_config(const wifi_ap_config_t *config) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    // Validate configuration
    int validation_result = validate_ap_config(config);
    if (validation_result != WIFI_AP_SUCCESS) {
        return validation_result;
    }
    
    // Update configuration
    memcpy(&current_config, config, sizeof(wifi_ap_config_t));
    
    printf("[WIFI_AP_MANAGER] AP configuration updated\n");
    return WIFI_AP_SUCCESS;
}

/**
 * @brief Get AP statistics
 */
int wifi_ap_manager_get_statistics(wifi_ap_statistics_t *stats) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    if (!stats) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    // Update statistics
    update_statistics();
    
    memcpy(stats, &statistics, sizeof(wifi_ap_statistics_t));
    return WIFI_AP_SUCCESS;
}

/**
 * @brief Reset AP statistics
 */
int wifi_ap_manager_reset_statistics(void) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    memset(&statistics, 0, sizeof(wifi_ap_statistics_t));
    
    printf("[WIFI_AP_MANAGER] Statistics reset\n");
    return WIFI_AP_SUCCESS;
}

/**
 * @brief Check if AP is running
 */
bool wifi_ap_manager_is_running(void) {
    if (!wifi_ap_manager_initialized) {
        return false;
    }
    
    return (current_status.status == AP_STATUS_RUNNING);
}

/**
 * @brief Get error message for error code
 */
const char* wifi_ap_manager_get_error_message(int error_code) {
    switch (error_code) {
        case WIFI_AP_SUCCESS:
            return "Success";
        case WIFI_AP_ERROR_INVALID_PARAM:
            return "Invalid parameter";
        case WIFI_AP_ERROR_INIT_FAILED:
            return "Initialization failed";
        case WIFI_AP_ERROR_START_FAILED:
            return "AP start failed";
        case WIFI_AP_ERROR_STOP_FAILED:
            return "AP stop failed";
        case WIFI_AP_ERROR_NOT_INITIALIZED:
            return "Manager not initialized";
        case WIFI_AP_ERROR_ALREADY_RUNNING:
            return "AP already running";
        case WIFI_AP_ERROR_NOT_RUNNING:
            return "AP not running";
        case WIFI_AP_ERROR_INVALID_SSID:
            return "Invalid SSID";
        case WIFI_AP_ERROR_WEAK_PASSWORD:
            return "Weak password";
        case WIFI_AP_ERROR_HAL_FAILED:
            return "HAL operation failed";
        case WIFI_AP_ERROR_MAX_CLIENTS_REACHED:
            return "Maximum clients reached";
        case WIFI_AP_ERROR_CLIENT_NOT_FOUND:
            return "Client not found";
        case WIFI_AP_ERROR_CONFIG_FAILED:
            return "Configuration failed";
        default:
            return "Unknown error";
    }
}

/**
 * @brief Validate AP configuration
 */
int wifi_ap_manager_validate_config(const wifi_ap_config_t *config) {
    return validate_ap_config(config);
}

/**
 * @brief Enable/disable auto-fallback mode
 */
int wifi_ap_manager_set_auto_fallback(bool enabled, uint32_t timeout_ms) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    auto_fallback_enabled = enabled;
    fallback_timeout_ms = timeout_ms;
    
    printf("[WIFI_AP_MANAGER] Auto-fallback %s, timeout=%u ms\n", 
           enabled ? "enabled" : "disabled", timeout_ms);
    
    return WIFI_AP_SUCCESS;
}

/**
 * @brief Trigger fallback to AP mode
 */
int wifi_ap_manager_trigger_fallback(void) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    printf("[WIFI_AP_MANAGER] Triggering fallback to AP mode...\n");
    
    // Start AP with current configuration
    int result = wifi_ap_manager_start(&current_config);
    
    if (result == WIFI_AP_SUCCESS) {
        statistics.fallback_triggers++;
        printf("[WIFI_AP_MANAGER] Fallback triggered successfully\n");
    } else {
        printf("[WIFI_AP_MANAGER] Fallback trigger failed: %s\n", 
               wifi_ap_manager_get_error_message(result));
    }
    
    return result;
}

/**
 * @brief Get fallback status
 */
int wifi_ap_manager_get_fallback_status(bool *enabled, uint32_t *timeout_ms) {
    if (!wifi_ap_manager_initialized) {
        return WIFI_AP_ERROR_NOT_INITIALIZED;
    }
    
    if (!enabled || !timeout_ms) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    *enabled = auto_fallback_enabled;
    *timeout_ms = fallback_timeout_ms;
    
    return WIFI_AP_SUCCESS;
}

// Internal Functions Implementation

/**
 * @brief Update AP status
 */
static void update_ap_status(void) {
    if (current_status.status == AP_STATUS_RUNNING && ap_start_time > 0) {
        current_status.uptime_seconds = time(NULL) - ap_start_time;
    }
    
    // Get client count from HAL
    if (current_status.status == AP_STATUS_RUNNING) {
        hal_wifi_ap_client_t hal_clients[10];
        int count = hal_wifi_ap_get_clients(hal_clients, 10);
        if (count >= 0) {
            current_status.connected_clients = count;
        }
    }
    
    // Get real system metrics
    FILE *fp;
    char buffer[128];
    
    // Get CPU usage
    fp = popen("top -bn1 | grep 'Cpu(s)' | awk '{print $2}' | awk -F'%' '{print $1}'", "r");
    if (fp != NULL) {
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            current_status.cpu_usage_percent = atof(buffer);
        }
        pclose(fp);
    } else {
        current_status.cpu_usage_percent = 15.0f; // Fallback
    }
    
    // Get memory usage
    fp = popen("free | grep Mem | awk '{printf \"%.1f\", $3/$2 * 100.0}'", "r");
    if (fp != NULL) {
        if (fgets(buffer, sizeof(buffer), fp) != NULL) {
            current_status.memory_usage_percent = atof(buffer);
        }
        pclose(fp);
    } else {
        current_status.memory_usage_percent = 25.0f; // Fallback
    }
    
    // Update network statistics
    if (current_status.status == AP_STATUS_RUNNING) {
        // Get bytes sent/received from /proc/net/dev
        fp = fopen("/proc/net/dev", "r");
        if (fp != NULL) {
            char line[256];
            while (fgets(line, sizeof(line), fp)) {
                if (strstr(line, "wlan0:") != NULL) {
                    unsigned long long bytes_received, bytes_sent;
                    sscanf(line, "%*s %llu %*u %*u %*u %*u %*u %*u %*u %llu", 
                           &bytes_received, &bytes_sent);
                    current_status.total_bytes_received = (uint32_t)bytes_received;
                    current_status.total_bytes_sent = (uint32_t)bytes_sent;
                    break;
                }
            }
            fclose(fp);
        }
    }
}

/**
 * @brief Update statistics
 */
static void update_statistics(void) {
    if (current_status.status == AP_STATUS_RUNNING && ap_start_time > 0) {
        statistics.total_uptime_seconds += (time(NULL) - ap_start_time);
        ap_start_time = time(NULL);
    }
    
    // Update average metrics (simplified)
    statistics.average_cpu_usage = current_status.cpu_usage_percent;
    statistics.average_memory_usage = current_status.memory_usage_percent;
}

/**
 * @brief Validate IP address format
 */
static bool is_valid_ip_address(const char *ip) {
    if (!ip) return false;
    
    int a, b, c, d;
    if (sscanf(ip, "%d.%d.%d.%d", &a, &b, &c, &d) != 4) {
        return false;
    }
    
    return (a >= 0 && a <= 255) && (b >= 0 && b <= 255) && 
           (c >= 0 && c <= 255) && (d >= 0 && d <= 255);
}

/**
 * @brief Validate AP configuration
 */
static int validate_ap_config(const wifi_ap_config_t *config) {
    if (!config) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    // Validate SSID
    if (strlen(config->ap_ssid) < 1 || strlen(config->ap_ssid) > 32) {
        return WIFI_AP_ERROR_INVALID_SSID;
    }
    
    // Validate SSID characters (alphanumeric, underscore, hyphen)
    for (int i = 0; i < strlen(config->ap_ssid); i++) {
        char c = config->ap_ssid[i];
        if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || 
              (c >= '0' && c <= '9') || c == '_' || c == '-')) {
            return WIFI_AP_ERROR_INVALID_SSID;
        }
    }
    
    // Validate password based on security type
    if (config->ap_security_type != AP_SECURITY_OPEN) {
        if (strlen(config->ap_password) < 8 || strlen(config->ap_password) > 64) {
            return WIFI_AP_ERROR_WEAK_PASSWORD;
        }
        
        // Validate password complexity for WPA2/WPA3
        if (config->ap_security_type == AP_SECURITY_WPA2 || 
            config->ap_security_type == AP_SECURITY_WPA3) {
            bool has_upper = false, has_lower = false, has_digit = false, has_special = false;
            
            for (int i = 0; i < strlen(config->ap_password); i++) {
                char c = config->ap_password[i];
                if (c >= 'A' && c <= 'Z') has_upper = true;
                else if (c >= 'a' && c <= 'z') has_lower = true;
                else if (c >= '0' && c <= '9') has_digit = true;
                else if (strchr("!@#$%^&*()_+-=[]{}|;:,.<>?", c)) has_special = true;
            }
            
            // WPA2/WPA3 requires at least 3 of 4 character types
            int complexity_count = (has_upper ? 1 : 0) + (has_lower ? 1 : 0) + 
                                  (has_digit ? 1 : 0) + (has_special ? 1 : 0);
            if (complexity_count < 3) {
                return WIFI_AP_ERROR_WEAK_PASSWORD;
            }
        }
    }
    
    // Validate IP address format
    if (!is_valid_ip_address(config->ap_ip)) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    // Validate netmask format
    if (!is_valid_ip_address(config->ap_netmask)) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    // Validate channel (1-13 for 2.4GHz)
    if (config->ap_channel < 1 || config->ap_channel > 13) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    // Validate max clients
    if (config->max_clients < 1 || config->max_clients > 50) {
        return WIFI_AP_ERROR_INVALID_PARAM;
    }
    
    return WIFI_AP_SUCCESS;
}
