/**
 * @file network_manager.c
 * @brief Network Manager Implementation - OHT-50 Robot WiFi Connectivity
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Implementation của Network Manager cho OHT-50 robots
 * hỗ trợ WiFi connectivity, roaming, và mobile app support.
 */

#include "network_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>

// Network Manager State
static bool network_manager_initialized = false;
static oht_network_config_t current_config = {0};
static oht_network_status_t current_status = {0};
static performance_metrics_t performance_metrics = {0};
static pthread_mutex_t network_mutex = PTHREAD_MUTEX_INITIALIZER;

// Internal Functions
static int validate_wifi_credentials(const char *ssid, const char *password);
static int validate_security_type(int security_type);
static int validate_signal_threshold(int threshold);
static uint64_t get_timestamp_ms(void);
static void record_performance_metric(const char *operation, uint32_t response_time_ms, bool success);
static void update_network_status(void);
static void get_real_network_info(void);
static void get_real_network_stats(void);

// Error Messages
static const char* error_messages[] = {
    "Success",
    "Invalid parameter",
    "Initialization failed",
    "Connection failed",
    "Timeout",
    "Authentication failed",
    "Scan failed",
    "Configuration failed",
    "Not initialized",
    "Invalid SSID",
    "Weak password",
    "Invalid security type",
    "Roaming failed",
    "Mobile app failed"
};

/**
 * @brief Initialize Network Manager
 */
int network_manager_init(void) {
    pthread_mutex_lock(&network_mutex);
    
    if (network_manager_initialized) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_SUCCESS;
    }
    
    // Initialize Network Manager (mock for now)
    printf("[NETWORK_MANAGER] Initializing Network Manager...\n");
    
    // Load default configuration
    current_config.wifi_enabled = true;
    strcpy(current_config.wifi_ssid, "OHT-50-Network");
    strcpy(current_config.wifi_password, "secure_password_2025");
    current_config.wifi_security_type = OHT_WIFI_SECURITY_WPA2;
    current_config.signal_strength = -70;
    current_config.roaming_enabled = true;
    current_config.mobile_app_enabled = true;
    current_config.last_update_time = get_timestamp_ms();
    
    // Initialize status
    memset(&current_status, 0, sizeof(oht_network_status_t));
    current_status.connected = false;
    
    // Initialize performance metrics
    memset(&performance_metrics, 0, sizeof(performance_metrics_t));
    performance_metrics.success_rate = 100.0f;
    
    network_manager_initialized = true;
    
    printf("[NETWORK_MANAGER] Initialized successfully\n");
    
    pthread_mutex_unlock(&network_mutex);
    return NETWORK_SUCCESS;
}

/**
 * @brief Deinitialize Network Manager
 */
int network_manager_deinit(void) {
    pthread_mutex_lock(&network_mutex);
    
    if (!network_manager_initialized) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_NOT_INITIALIZED;
    }
    
    // Disconnect WiFi if connected
    if (current_status.connected) {
        network_manager_disconnect_wifi();
    }
    
    // Deinitialize Network Manager (mock for now)
    printf("[NETWORK_MANAGER] Deinitializing Network Manager...\n");
    
    network_manager_initialized = false;
    
    printf("[NETWORK_MANAGER] Deinitialized successfully\n");
    
    pthread_mutex_unlock(&network_mutex);
    return NETWORK_SUCCESS;
}

/**
 * @brief Connect to WiFi network
 */
int network_manager_connect_wifi(const char *ssid, const char *password) {
    uint64_t start_time = get_timestamp_ms();
    bool success = false;
    
    pthread_mutex_lock(&network_mutex);
    
    if (!network_manager_initialized) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_NOT_INITIALIZED;
    }
    
    // Validate parameters
    if (!ssid || !password) {
        pthread_mutex_unlock(&network_mutex);
        record_performance_metric("connect_wifi", get_timestamp_ms() - start_time, false);
        return NETWORK_ERROR_INVALID_PARAM;
    }
    
    // Validate WiFi credentials
    int validation_result = validate_wifi_credentials(ssid, password);
    if (validation_result != NETWORK_SUCCESS) {
        pthread_mutex_unlock(&network_mutex);
        record_performance_metric("connect_wifi", get_timestamp_ms() - start_time, false);
        return validation_result;
    }
    
    // Update configuration
    strncpy(current_config.wifi_ssid, ssid, sizeof(current_config.wifi_ssid) - 1);
    strncpy(current_config.wifi_password, password, sizeof(current_config.wifi_password) - 1);
    current_config.last_update_time = get_timestamp_ms();
    
    // Connect via mock implementation
    printf("[NETWORK_MANAGER] Connecting to WiFi: %s\n", ssid);
    
    // Get REAL network information from system
    if (strlen(ssid) > 0 && strlen(password) >= 8) {
        current_status.connected = true;
        strncpy(current_status.current_ssid, ssid, sizeof(current_status.current_ssid) - 1);
        
        // Get REAL network information from system interfaces
        get_real_network_info();
        
        success = true;
        printf("[NETWORK_MANAGER] Connected to WiFi: %s\n", ssid);
    } else {
        current_status.connected = false;
        printf("[NETWORK_MANAGER] Failed to connect to WiFi: %s\n", ssid);
    }
    
    pthread_mutex_unlock(&network_mutex);
    
    uint64_t response_time = get_timestamp_ms() - start_time;
    record_performance_metric("connect_wifi", response_time, success);
    
    return success ? NETWORK_SUCCESS : NETWORK_ERROR_CONNECTION_FAILED;
}

/**
 * @brief Disconnect from current WiFi network
 */
int network_manager_disconnect_wifi(void) {
    uint64_t start_time = get_timestamp_ms();
    bool success = false;
    
    pthread_mutex_lock(&network_mutex);
    
    if (!network_manager_initialized) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_NOT_INITIALIZED;
    }
    
    printf("[NETWORK_MANAGER] Disconnecting from WiFi\n");
    
    // Mock disconnection success
    if (true) {
        current_status.connected = false;
        memset(current_status.current_ssid, 0, sizeof(current_status.current_ssid));
        success = true;
        printf("[NETWORK_MANAGER] Disconnected from WiFi\n");
    } else {
        printf("[NETWORK_MANAGER] Failed to disconnect from WiFi\n");
    }
    
    pthread_mutex_unlock(&network_mutex);
    
    uint64_t response_time = get_timestamp_ms() - start_time;
    record_performance_metric("disconnect_wifi", response_time, success);
    
    return success ? NETWORK_SUCCESS : NETWORK_ERROR_CONNECTION_FAILED;
}

/**
 * @brief Scan for available WiFi networks
 */
int network_manager_scan_networks(wifi_network_t *networks, int max_count) {
    uint64_t start_time = get_timestamp_ms();
    bool success = false;
    int networks_found = 0;
    
    pthread_mutex_lock(&network_mutex);
    
    if (!network_manager_initialized) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_NOT_INITIALIZED;
    }
    
    if (!networks || max_count <= 0) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_INVALID_PARAM;
    }
    
    // Mock scan results for development
    if (max_count >= 3) {
        // Network 1: OHT-50-Network
        strcpy(networks[0].ssid, "OHT-50-Network");
        strcpy(networks[0].bssid, "00:11:22:33:44:55");
        networks[0].signal_strength = -45;
        networks[0].frequency = 5000;
        networks[0].channel = 36;
        strcpy(networks[0].security, "WPA2");
        networks[0].hidden = false;
        
        // Network 2: OHT-50-Backup
        strcpy(networks[1].ssid, "OHT-50-Backup");
        strcpy(networks[1].bssid, "00:11:22:33:44:66");
        networks[1].signal_strength = -55;
        networks[1].frequency = 2400;
        networks[1].channel = 6;
        strcpy(networks[1].security, "WPA3");
        networks[1].hidden = false;
        
        // Network 3: OHT-50-Emergency
        strcpy(networks[2].ssid, "OHT-50-Emergency");
        strcpy(networks[2].bssid, "00:11:22:33:44:77");
        networks[2].signal_strength = -65;
        networks[2].frequency = 5000;
        networks[2].channel = 149;
        strcpy(networks[2].security, "WPA2");
        networks[2].hidden = false;
        
        networks_found = 3;
        success = true;
    }
    
    pthread_mutex_unlock(&network_mutex);
    
    uint64_t response_time = get_timestamp_ms() - start_time;
    record_performance_metric("scan_networks", response_time, success);
    
    printf("[NETWORK_MANAGER] Scanned %d networks\n", networks_found);
    
    return networks_found;
}

/**
 * @brief Get current network status
 */
int network_manager_get_status(oht_network_status_t *status) {
    uint64_t start_time = get_timestamp_ms();
    bool success = false;
    
    pthread_mutex_lock(&network_mutex);
    
    if (!network_manager_initialized) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_NOT_INITIALIZED;
    }
    
    if (!status) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_INVALID_PARAM;
    }
    
    // Update network status
    update_network_status();
    
    // Copy current status
    *status = current_status;
    success = true;
    
    pthread_mutex_unlock(&network_mutex);
    
    uint64_t response_time = get_timestamp_ms() - start_time;
    record_performance_metric("get_status", response_time, success);
    
    return success ? NETWORK_SUCCESS : NETWORK_ERROR_INVALID_PARAM;
}

/**
 * @brief Enable/disable WiFi roaming
 */
int network_manager_enable_roaming(bool enable) {
    uint64_t start_time = get_timestamp_ms();
    bool success = false;
    
    pthread_mutex_lock(&network_mutex);
    
    if (!network_manager_initialized) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_NOT_INITIALIZED;
    }
    
    current_config.roaming_enabled = enable;
    current_config.last_update_time = get_timestamp_ms();
    
    if (enable) {
        printf("[NETWORK_MANAGER] WiFi roaming enabled\n");
    } else {
        printf("[NETWORK_MANAGER] WiFi roaming disabled\n");
    }
    
    success = true;
    
    pthread_mutex_unlock(&network_mutex);
    
    uint64_t response_time = get_timestamp_ms() - start_time;
    record_performance_metric("enable_roaming", response_time, success);
    
    return success ? NETWORK_SUCCESS : NETWORK_ERROR_ROAMING_FAILED;
}

/**
 * @brief Enable/disable mobile app support
 */
int network_manager_enable_mobile_app(bool enable) {
    uint64_t start_time = get_timestamp_ms();
    bool success = false;
    
    pthread_mutex_lock(&network_mutex);
    
    if (!network_manager_initialized) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_NOT_INITIALIZED;
    }
    
    current_config.mobile_app_enabled = enable;
    current_config.last_update_time = get_timestamp_ms();
    
    if (enable) {
        printf("[NETWORK_MANAGER] Mobile app support enabled\n");
    } else {
        printf("[NETWORK_MANAGER] Mobile app support disabled\n");
    }
    
    success = true;
    
    pthread_mutex_unlock(&network_mutex);
    
    uint64_t response_time = get_timestamp_ms() - start_time;
    record_performance_metric("enable_mobile_app", response_time, success);
    
    return success ? NETWORK_SUCCESS : NETWORK_ERROR_MOBILE_APP_FAILED;
}

/**
 * @brief Get network configuration
 */
int network_manager_get_config(oht_network_config_t *config) {
    pthread_mutex_lock(&network_mutex);
    
    if (!network_manager_initialized) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_INVALID_PARAM;
    }
    
    *config = current_config;
    
    pthread_mutex_unlock(&network_mutex);
    return NETWORK_SUCCESS;
}

/**
 * @brief Set network configuration
 */
int network_manager_set_config(const oht_network_config_t *config) {
    pthread_mutex_lock(&network_mutex);
    
    if (!network_manager_initialized) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_NOT_INITIALIZED;
    }
    
    if (!config) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_INVALID_PARAM;
    }
    
    // Validate configuration
    if (!network_manager_validate_config(config)) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_CONFIG_FAILED;
    }
    
    current_config = *config;
    current_config.last_update_time = get_timestamp_ms();
    
    pthread_mutex_unlock(&network_mutex);
    return NETWORK_SUCCESS;
}

/**
 * @brief Get performance metrics
 */
int network_manager_get_performance(performance_metrics_t *metrics) {
    pthread_mutex_lock(&network_mutex);
    
    if (!network_manager_initialized) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_NOT_INITIALIZED;
    }
    
    if (!metrics) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_INVALID_PARAM;
    }
    
    *metrics = performance_metrics;
    
    pthread_mutex_unlock(&network_mutex);
    return NETWORK_SUCCESS;
}

/**
 * @brief Reset network configuration to defaults
 */
int network_manager_reset_config(void) {
    pthread_mutex_lock(&network_mutex);
    
    if (!network_manager_initialized) {
        pthread_mutex_unlock(&network_mutex);
        return NETWORK_ERROR_NOT_INITIALIZED;
    }
    
    // Reset to default configuration
    current_config.wifi_enabled = true;
    strcpy(current_config.wifi_ssid, "OHT-50-Network");
    strcpy(current_config.wifi_password, "secure_password_2025");
    current_config.wifi_security_type = OHT_WIFI_SECURITY_WPA2;
    current_config.signal_strength = -70;
    current_config.roaming_enabled = true;
    current_config.mobile_app_enabled = true;
    current_config.last_update_time = get_timestamp_ms();
    
    printf("[NETWORK_MANAGER] Configuration reset to defaults\n");
    
    pthread_mutex_unlock(&network_mutex);
    return NETWORK_SUCCESS;
}

/**
 * @brief Validate network configuration
 */
bool network_manager_validate_config(const oht_network_config_t *config) {
    if (!config) {
        return false;
    }
    
    // Validate SSID
    if (strlen(config->wifi_ssid) == 0 || strlen(config->wifi_ssid) > 32) {
        return false;
    }
    
    // Validate password
    if (strlen(config->wifi_password) < 8 || strlen(config->wifi_password) > 64) {
        return false;
    }
    
    // Validate security type
    if (config->wifi_security_type < OHT_WIFI_SECURITY_OPEN || 
        config->wifi_security_type > OHT_WIFI_SECURITY_WPA3) {
        return false;
    }
    
    return true;
}

/**
 * @brief Get error message for error code
 */
const char* network_manager_get_error_message(int error_code) {
    int index = -error_code;
    if (index >= 0 && index < (int)(sizeof(error_messages) / sizeof(error_messages[0]))) {
        return error_messages[index];
    }
    return "Unknown error";
}

// Internal Functions Implementation

/**
 * @brief Validate WiFi credentials
 */
static int validate_wifi_credentials(const char *ssid, const char *password) {
    if (!ssid || strlen(ssid) == 0 || strlen(ssid) > 32) {
        return NETWORK_ERROR_INVALID_SSID;
    }
    
    if (!password || strlen(password) < 8 || strlen(password) > 64) {
        return NETWORK_ERROR_WEAK_PASSWORD;
    }
    
    return NETWORK_SUCCESS;
}

/**
 * @brief Validate security type
 */
static int validate_security_type(int security_type) {
    if (security_type < OHT_WIFI_SECURITY_OPEN || security_type > OHT_WIFI_SECURITY_WPA3) {
        return NETWORK_ERROR_INVALID_SECURITY;
    }
    return NETWORK_SUCCESS;
}

/**
 * @brief Validate signal threshold
 */
static int validate_signal_threshold(int threshold) {
    if (threshold < -100 || threshold > 0) {
        return NETWORK_ERROR_INVALID_PARAM;
    }
    return NETWORK_SUCCESS;
}

/**
 * @brief Get current timestamp in milliseconds
 */
static uint64_t get_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

/**
 * @brief Record performance metric
 */
static void record_performance_metric(const char *operation, uint32_t response_time_ms, bool success) {
    performance_metrics.request_count++;
    if (!success) {
        performance_metrics.error_count++;
    }
    
    performance_metrics.success_rate = (float)(performance_metrics.request_count - performance_metrics.error_count) * 100.0f / performance_metrics.request_count;
    
    if (response_time_ms > performance_metrics.response_time_ms) {
        performance_metrics.response_time_ms = response_time_ms;
    }
    
    printf("[NETWORK_MANAGER] %s: %s (%u ms)\n", 
           operation, success ? "SUCCESS" : "FAILED", response_time_ms);
}

/**
 * @brief Update network status
 */
static void update_network_status(void) {
    // Get REAL network information from system
    get_real_network_info();
    
    // Update bytes if connected - get REAL network statistics
    if (current_status.connected) {
        get_real_network_stats();
    }
}

/**
 * @brief Get REAL network information from system interfaces
 */
static void get_real_network_info(void) {
    FILE *fp;
    char line[256];
    char interface[16];
    char ip_address[16];
    char gateway[16];
    char dns[16];
    int signal_strength = -100;
    
    // Get IP address from system interfaces - PRIORITIZE wlan0 if connected to WiFi
    if (strlen(current_status.current_ssid) > 0) {
        // If connected to WiFi, get wlan0 IP first
        fp = popen("ip addr show wlan0 | grep 'inet ' | awk '{print $2}' | cut -d'/' -f1", "r");
    } else {
        // If not connected to WiFi, get eth0 IP
        fp = popen("ip addr show eth0 | grep 'inet ' | awk '{print $2}' | cut -d'/' -f1", "r");
    }
    if (fp != NULL) {
        if (fgets(ip_address, sizeof(ip_address), fp) != NULL) {
            // Remove newline
            ip_address[strcspn(ip_address, "\n")] = 0;
            strcpy(current_status.ip_address, ip_address);
            printf("[NETWORK_MANAGER] Real IP address: %s\n", ip_address);
        }
        pclose(fp);
    }
    
    // Get gateway from system
    fp = popen("ip route | grep default | awk '{print $3}' | head -1", "r");
    if (fp != NULL) {
        if (fgets(gateway, sizeof(gateway), fp) != NULL) {
            gateway[strcspn(gateway, "\n")] = 0;
            strcpy(current_status.gateway, gateway);
            printf("[NETWORK_MANAGER] Real gateway: %s\n", gateway);
        }
        pclose(fp);
    }
    
    // Get DNS from system
    fp = popen("cat /etc/resolv.conf | grep nameserver | awk '{print $2}' | head -1", "r");
    if (fp != NULL) {
        if (fgets(dns, sizeof(dns), fp) != NULL) {
            dns[strcspn(dns, "\n")] = 0;
            strcpy(current_status.dns, dns);
            printf("[NETWORK_MANAGER] Real DNS: %s\n", dns);
        }
        pclose(fp);
    }
    
    // Get WiFi signal strength (if connected to WiFi)
    if (strlen(current_status.current_ssid) > 0) {
        fp = popen("iwconfig wlan0 2>/dev/null | grep 'Signal level' | awk '{print $4}' | cut -d'=' -f2", "r");
        if (fp != NULL) {
            if (fgets(line, sizeof(line), fp) != NULL) {
                signal_strength = atoi(line);
                current_status.signal_strength = signal_strength;
                printf("[NETWORK_MANAGER] Real signal strength: %d dBm\n", signal_strength);
            }
            pclose(fp);
        }
    }
}

/**
 * @brief Get REAL network statistics from system
 */
static void get_real_network_stats(void) {
    FILE *fp;
    char line[256];
    unsigned long long bytes_sent = 0, bytes_received = 0;
    
    // Get network statistics from /proc/net/dev
    fp = popen("cat /proc/net/dev | grep -E 'eth0|wlan0' | awk '{print $2, $10}' | head -1", "r");
    if (fp != NULL) {
        if (fgets(line, sizeof(line), fp) != NULL) {
            sscanf(line, "%llu %llu", &bytes_received, &bytes_sent);
            current_status.bytes_received = bytes_received;
            current_status.bytes_sent = bytes_sent;
            printf("[NETWORK_MANAGER] Real network stats: RX=%llu, TX=%llu\n", bytes_received, bytes_sent);
        }
        pclose(fp);
    }
    
    // Get real latency (ping to gateway)
    fp = popen("ping -c 1 -W 1 $(ip route | grep default | awk '{print $3}' | head -1) 2>/dev/null | grep 'time=' | awk '{print $7}' | cut -d'=' -f2", "r");
    if (fp != NULL) {
        if (fgets(line, sizeof(line), fp) != NULL) {
            float latency = atof(line);
            current_status.latency_ms = latency;
            printf("[NETWORK_MANAGER] Real latency: %.2f ms\n", latency);
        }
        pclose(fp);
    }
}
