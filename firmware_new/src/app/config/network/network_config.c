/**
 * @file network_config.c
 * @brief Network Configuration Implementation - OHT-50 Robot Network Management Configuration
 * @version 1.0.0
 * @date 2025-01-28
 * @author Firmware Team - OHT-50
 * 
 * Implementation của Network Configuration cho OHT-50 robots
 * cung cấp quản lý cấu hình network và WiFi settings.
 */

#include "network_config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <time.h>

// Internal Functions
static int write_config_file(const char *file_path, const char *content);
static int read_config_file(const char *file_path, char *content, size_t max_size);
static int parse_config_line(const char *line, char *key, char *value);
static int format_config_content(const oht_network_config_t *config, char *content, size_t max_size);
static int parse_network_config(const char *content, oht_network_config_t *config);
static int format_wifi_config_content(const wifi_roaming_config_t *config, char *content, size_t max_size);
static int parse_wifi_config(const char *content, wifi_roaming_config_t *config);

// Validation Error Messages
static const char* validation_error_messages[] = {
    "Configuration is valid",
    "Invalid SSID format",
    "Invalid password format",
    "Invalid security type",
    "Invalid signal threshold",
    "Invalid timeout value",
    "Configuration file error"
};

/**
 * @brief Load network configuration from file
 */
int network_config_load(oht_network_config_t *config) {
    if (!config) {
        return CONFIG_FILE_ERROR;
    }
    
    // Create directory if not exists
    network_config_create_directory();
    
    // Check if config file exists
    if (!network_config_file_exists(NETWORK_CONFIG_FILE)) {
        printf("[NETWORK_CONFIG] Config file not found, creating default\n");
        network_config_create_default(config);
        network_config_save(config);
        return CONFIG_VALID;
    }
    
    // Read config file
    char content[2048];
    if (read_config_file(NETWORK_CONFIG_FILE, content, sizeof(content)) != 0) {
        printf("[NETWORK_CONFIG] Failed to read config file\n");
        return CONFIG_FILE_ERROR;
    }
    
    // Parse config content
    if (parse_network_config(content, config) != 0) {
        printf("[NETWORK_CONFIG] Failed to parse config file\n");
        return CONFIG_FILE_ERROR;
    }
    
    printf("[NETWORK_CONFIG] Network configuration loaded successfully\n");
    return CONFIG_VALID;
}

/**
 * @brief Save network configuration to file
 */
int network_config_save(const oht_network_config_t *config) {
    if (!config) {
        return CONFIG_FILE_ERROR;
    }
    
    // Validate configuration
    config_validation_result_t validation = network_config_validate(config);
    if (validation != CONFIG_VALID) {
        printf("[NETWORK_CONFIG] Configuration validation failed: %s\n", 
               network_config_get_validation_error(validation));
        return validation;
    }
    
    // Create directory if not exists
    network_config_create_directory();
    
    // Format config content
    char content[2048];
    if (format_config_content(config, content, sizeof(content)) != 0) {
        printf("[NETWORK_CONFIG] Failed to format config content\n");
        return CONFIG_FILE_ERROR;
    }
    
    // Write to file
    if (write_config_file(NETWORK_CONFIG_FILE, content) != 0) {
        printf("[NETWORK_CONFIG] Failed to write config file\n");
        return CONFIG_FILE_ERROR;
    }
    
    // Set file permissions
    network_config_set_permissions(NETWORK_CONFIG_FILE);
    
    printf("[NETWORK_CONFIG] Network configuration saved successfully\n");
    return CONFIG_VALID;
}

/**
 * @brief Reset network configuration to defaults
 */
int network_config_reset(void) {
    oht_network_config_t config;
    network_config_create_default(&config);
    
    int result = network_config_save(&config);
    if (result == CONFIG_VALID) {
        printf("[NETWORK_CONFIG] Network configuration reset to defaults\n");
    }
    
    return result;
}

/**
 * @brief Validate network configuration
 */
config_validation_result_t network_config_validate(const oht_network_config_t *config) {
    if (!config) {
        return CONFIG_FILE_ERROR;
    }
    
    // Validate SSID
    if (strlen(config->wifi_ssid) == 0 || strlen(config->wifi_ssid) > 32) {
        return CONFIG_INVALID_SSID;
    }
    
    // Validate password
    if (strlen(config->wifi_password) < 8 || strlen(config->wifi_password) > 64) {
        return CONFIG_INVALID_PASSWORD;
    }
    
    // Validate security type
    if (config->wifi_security_type < WIFI_SECURITY_OPEN || 
        config->wifi_security_type > WIFI_SECURITY_WPA3) {
        return CONFIG_INVALID_SECURITY;
    }
    
    // Validate signal threshold
    if (config->signal_strength < -100 || config->signal_strength > 0) {
        return CONFIG_INVALID_THRESHOLD;
    }
    
    return CONFIG_VALID;
}

/**
 * @brief Apply network configuration
 */
int network_config_apply(const oht_network_config_t *config) {
    if (!config) {
        return CONFIG_FILE_ERROR;
    }
    
    // Validate configuration
    config_validation_result_t validation = network_config_validate(config);
    if (validation != CONFIG_VALID) {
        return validation;
    }
    
    // Apply to Network Manager
    int result = network_manager_set_config(config);
    if (result != NETWORK_SUCCESS) {
        printf("[NETWORK_CONFIG] Failed to apply network configuration\n");
        return CONFIG_FILE_ERROR;
    }
    
    printf("[NETWORK_CONFIG] Network configuration applied successfully\n");
    return CONFIG_VALID;
}

/**
 * @brief Load WiFi configuration from file
 */
int wifi_config_load(wifi_roaming_config_t *config) {
    if (!config) {
        return CONFIG_FILE_ERROR;
    }
    
    // Create directory if not exists
    network_config_create_directory();
    
    // Check if config file exists
    if (!network_config_file_exists(ROAMING_CONFIG_FILE)) {
        printf("[NETWORK_CONFIG] WiFi config file not found, creating default\n");
        wifi_config_create_default(config);
        wifi_config_save(config);
        return CONFIG_VALID;
    }
    
    // Read config file
    char content[1024];
    if (read_config_file(ROAMING_CONFIG_FILE, content, sizeof(content)) != 0) {
        printf("[NETWORK_CONFIG] Failed to read WiFi config file\n");
        return CONFIG_FILE_ERROR;
    }
    
    // Parse config content
    if (parse_wifi_config(content, config) != 0) {
        printf("[NETWORK_CONFIG] Failed to parse WiFi config file\n");
        return CONFIG_FILE_ERROR;
    }
    
    printf("[NETWORK_CONFIG] WiFi configuration loaded successfully\n");
    return CONFIG_VALID;
}

/**
 * @brief Save WiFi configuration to file
 */
int wifi_config_save(const wifi_roaming_config_t *config) {
    if (!config) {
        return CONFIG_FILE_ERROR;
    }
    
    // Validate configuration
    config_validation_result_t validation = wifi_config_validate(config);
    if (validation != CONFIG_VALID) {
        printf("[NETWORK_CONFIG] WiFi configuration validation failed: %s\n", 
               network_config_get_validation_error(validation));
        return validation;
    }
    
    // Create directory if not exists
    network_config_create_directory();
    
    // Format config content
    char content[1024];
    if (format_wifi_config_content(config, content, sizeof(content)) != 0) {
        printf("[NETWORK_CONFIG] Failed to format WiFi config content\n");
        return CONFIG_FILE_ERROR;
    }
    
    // Write to file
    if (write_config_file(ROAMING_CONFIG_FILE, content) != 0) {
        printf("[NETWORK_CONFIG] Failed to write WiFi config file\n");
        return CONFIG_FILE_ERROR;
    }
    
    // Set file permissions
    network_config_set_permissions(ROAMING_CONFIG_FILE);
    
    printf("[NETWORK_CONFIG] WiFi configuration saved successfully\n");
    return CONFIG_VALID;
}

/**
 * @brief Reset WiFi configuration to defaults
 */
int wifi_config_reset(void) {
    wifi_roaming_config_t config;
    wifi_config_create_default(&config);
    
    int result = wifi_config_save(&config);
    if (result == CONFIG_VALID) {
        printf("[NETWORK_CONFIG] WiFi configuration reset to defaults\n");
    }
    
    return result;
}

/**
 * @brief Validate WiFi configuration
 */
config_validation_result_t wifi_config_validate(const wifi_roaming_config_t *config) {
    if (!config) {
        return CONFIG_FILE_ERROR;
    }
    
    // Validate signal threshold
    if (config->signal_threshold_dbm < -100 || config->signal_threshold_dbm > 0) {
        return CONFIG_INVALID_THRESHOLD;
    }
    
    // Validate timeouts
    if (config->scan_interval_ms < 1000 || config->scan_interval_ms > 60000) {
        return CONFIG_INVALID_TIMEOUT;
    }
    
    if (config->handover_timeout_ms < 500 || config->handover_timeout_ms > 10000) {
        return CONFIG_INVALID_TIMEOUT;
    }
    
    return CONFIG_VALID;
}

/**
 * @brief Apply WiFi configuration
 */
int wifi_config_apply(const wifi_roaming_config_t *config) {
    if (!config) {
        return CONFIG_FILE_ERROR;
    }
    
    // Validate configuration
    config_validation_result_t validation = wifi_config_validate(config);
    if (validation != CONFIG_VALID) {
        return validation;
    }
    
    // Apply to WiFi Manager
    int result = wifi_manager_enable_roaming(config);
    if (result != WIFI_SUCCESS) {
        printf("[NETWORK_CONFIG] Failed to apply WiFi configuration\n");
        return CONFIG_FILE_ERROR;
    }
    
    printf("[NETWORK_CONFIG] WiFi configuration applied successfully\n");
    return CONFIG_VALID;
}

/**
 * @brief Create default network configuration
 */
int network_config_create_default(oht_network_config_t *config) {
    if (!config) {
        return CONFIG_FILE_ERROR;
    }
    
    config->wifi_enabled = true;
    strcpy(config->wifi_ssid, DEFAULT_WIFI_SSID);
    strcpy(config->wifi_password, DEFAULT_WIFI_PASSWORD);
    config->wifi_security_type = OHT_WIFI_SECURITY_WPA2;
    config->signal_strength = DEFAULT_SIGNAL_THRESHOLD;
    config->roaming_enabled = DEFAULT_ROAMING_ENABLED;
    config->mobile_app_enabled = DEFAULT_MOBILE_APP_ENABLED;
    config->last_update_time = time(NULL);
    
    printf("[NETWORK_CONFIG] Default network configuration created\n");
    return CONFIG_VALID;
}

/**
 * @brief Create default WiFi configuration
 */
int wifi_config_create_default(wifi_roaming_config_t *config) {
    if (!config) {
        return CONFIG_FILE_ERROR;
    }
    
    config->enabled = DEFAULT_ROAMING_ENABLED;
    config->signal_threshold_dbm = DEFAULT_SIGNAL_THRESHOLD;
    config->scan_interval_ms = DEFAULT_SCAN_TIMEOUT_MS;
    config->handover_timeout_ms = DEFAULT_HANDOVER_TIMEOUT_MS;
    config->aggressive_roaming = false;
    
    printf("[NETWORK_CONFIG] Default WiFi configuration created\n");
    return CONFIG_VALID;
}

/**
 * @brief Backup current configuration
 */
int network_config_backup(const char *backup_path) {
    if (!backup_path) {
        return CONFIG_FILE_ERROR;
    }
    
    oht_network_config_t config;
    if (network_config_load(&config) != CONFIG_VALID) {
        return CONFIG_FILE_ERROR;
    }
    
    char content[2048];
    if (format_config_content(&config, content, sizeof(content)) != 0) {
        return CONFIG_FILE_ERROR;
    }
    
    if (write_config_file(backup_path, content) != 0) {
        return CONFIG_FILE_ERROR;
    }
    
    printf("[NETWORK_CONFIG] Configuration backed up to: %s\n", backup_path);
    return CONFIG_VALID;
}

/**
 * @brief Restore configuration from backup
 */
int network_config_restore(const char *backup_path) {
    if (!backup_path) {
        return CONFIG_FILE_ERROR;
    }
    
    if (!network_config_file_exists(backup_path)) {
        printf("[NETWORK_CONFIG] Backup file not found: %s\n", backup_path);
        return CONFIG_FILE_ERROR;
    }
    
    char content[2048];
    if (read_config_file(backup_path, content, sizeof(content)) != 0) {
        return CONFIG_FILE_ERROR;
    }
    
    oht_network_config_t config;
    if (parse_network_config(content, &config) != 0) {
        return CONFIG_FILE_ERROR;
    }
    
    int result = network_config_save(&config);
    if (result == CONFIG_VALID) {
        printf("[NETWORK_CONFIG] Configuration restored from: %s\n", backup_path);
    }
    
    return result;
}

/**
 * @brief Get configuration validation error message
 */
const char* network_config_get_validation_error(config_validation_result_t result) {
    int index = -result;
    if (index >= 0 && index < (int)(sizeof(validation_error_messages) / sizeof(validation_error_messages[0]))) {
        return validation_error_messages[index];
    }
    return "Unknown validation error";
}

/**
 * @brief Check if configuration file exists
 */
bool network_config_file_exists(const char *file_path) {
    if (!file_path) {
        return false;
    }
    
    return access(file_path, F_OK) == 0;
}

/**
 * @brief Create configuration directory if not exists
 */
int network_config_create_directory(void) {
    const char *dir_path = "/etc/oht50";
    
    // Check if directory exists
    if (access(dir_path, F_OK) == 0) {
        return CONFIG_VALID;
    }
    
    // Create directory
    if (mkdir(dir_path, 0755) != 0) {
        printf("[NETWORK_CONFIG] Failed to create directory: %s\n", dir_path);
        return CONFIG_FILE_ERROR;
    }
    
    printf("[NETWORK_CONFIG] Created directory: %s\n", dir_path);
    return CONFIG_VALID;
}

/**
 * @brief Set configuration file permissions
 */
int network_config_set_permissions(const char *file_path) {
    if (!file_path) {
        return CONFIG_FILE_ERROR;
    }
    
    // Set permissions: owner read/write, group read, others read
    if (chmod(file_path, 0644) != 0) {
        printf("[NETWORK_CONFIG] Failed to set permissions for: %s\n", file_path);
        return CONFIG_FILE_ERROR;
    }
    
    return CONFIG_VALID;
}

// Internal Functions Implementation

/**
 * @brief Write configuration file
 */
static int write_config_file(const char *file_path, const char *content) {
    if (!file_path || !content) {
        return -1;
    }
    
    FILE *file = fopen(file_path, "w");
    if (!file) {
        printf("[NETWORK_CONFIG] Failed to open file for writing: %s\n", file_path);
        return -1;
    }
    
    size_t written = fwrite(content, 1, strlen(content), file);
    fclose(file);
    
    if (written != strlen(content)) {
        printf("[NETWORK_CONFIG] Failed to write complete content to: %s\n", file_path);
        return -1;
    }
    
    return 0;
}

/**
 * @brief Read configuration file
 */
static int read_config_file(const char *file_path, char *content, size_t max_size) {
    if (!file_path || !content || max_size == 0) {
        return -1;
    }
    
    FILE *file = fopen(file_path, "r");
    if (!file) {
        printf("[NETWORK_CONFIG] Failed to open file for reading: %s\n", file_path);
        return -1;
    }
    
    size_t read_size = fread(content, 1, max_size - 1, file);
    fclose(file);
    
    content[read_size] = '\0';
    
    return 0;
}

/**
 * @brief Parse configuration line
 */
static int parse_config_line(const char *line, char *key, char *value) {
    if (!line || !key || !value) {
        return -1;
    }
    
    // Skip leading whitespace
    while (*line == ' ' || *line == '\t') {
        line++;
    }
    
    // Skip comments and empty lines
    if (*line == '#' || *line == '\0' || *line == '\n') {
        return -1;
    }
    
    // Find '=' character
    const char *equal_pos = strchr(line, '=');
    if (!equal_pos) {
        return -1;
    }
    
    // Extract key
    size_t key_len = equal_pos - line;
    if (key_len >= 64) {
        key_len = 63;
    }
    strncpy(key, line, key_len);
    key[key_len] = '\0';
    
    // Skip whitespace around '='
    equal_pos++;
    while (*equal_pos == ' ' || *equal_pos == '\t') {
        equal_pos++;
    }
    
    // Extract value
    strncpy(value, equal_pos, 255);
    value[255] = '\0';
    
    // Remove trailing whitespace and newline
    char *end = value + strlen(value) - 1;
    while (end > value && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        *end = '\0';
        end--;
    }
    
    return 0;
}

/**
 * @brief Format network configuration content
 */
static int format_config_content(const oht_network_config_t *config, char *content, size_t max_size) {
    if (!config || !content || max_size == 0) {
        return -1;
    }
    
    snprintf(content, max_size,
             "# OHT-50 Network Configuration\n"
             "# Generated on: %s\n\n"
             "wifi_enabled=%s\n"
             "wifi_ssid=%s\n"
             "wifi_password=%s\n"
             "wifi_security_type=%d\n"
             "signal_strength=%d\n"
             "roaming_enabled=%s\n"
             "mobile_app_enabled=%s\n"
             "last_update_time=%u\n",
             "2025-01-28",
             config->wifi_enabled ? "true" : "false",
             config->wifi_ssid,
             config->wifi_password,
             config->wifi_security_type,
             config->signal_strength,
             config->roaming_enabled ? "true" : "false",
             config->mobile_app_enabled ? "true" : "false",
             config->last_update_time);
    
    return 0;
}

/**
 * @brief Parse network configuration
 */
static int parse_network_config(const char *content, oht_network_config_t *config) {
    if (!content || !config) {
        return -1;
    }
    
    // Initialize with defaults
    network_config_create_default(config);
    
    char *line = strtok((char*)content, "\n");
    while (line) {
        char key[64], value[256];
        if (parse_config_line(line, key, value) == 0) {
            if (strcmp(key, "wifi_enabled") == 0) {
                config->wifi_enabled = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "wifi_ssid") == 0) {
                strncpy(config->wifi_ssid, value, sizeof(config->wifi_ssid) - 1);
                config->wifi_ssid[sizeof(config->wifi_ssid) - 1] = '\0';
            } else if (strcmp(key, "wifi_password") == 0) {
                strncpy(config->wifi_password, value, sizeof(config->wifi_password) - 1);
                config->wifi_password[sizeof(config->wifi_password) - 1] = '\0';
            } else if (strcmp(key, "wifi_security_type") == 0) {
                config->wifi_security_type = atoi(value);
            } else if (strcmp(key, "signal_strength") == 0) {
                config->signal_strength = atoi(value);
            } else if (strcmp(key, "roaming_enabled") == 0) {
                config->roaming_enabled = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "mobile_app_enabled") == 0) {
                config->mobile_app_enabled = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "last_update_time") == 0) {
                config->last_update_time = (uint32_t)atoll(value);
            }
        }
        line = strtok(NULL, "\n");
    }
    
    return 0;
}

/**
 * @brief Format WiFi configuration content
 */
static int format_wifi_config_content(const wifi_roaming_config_t *config, char *content, size_t max_size) {
    if (!config || !content || max_size == 0) {
        return -1;
    }
    
    snprintf(content, max_size,
             "# OHT-50 WiFi Roaming Configuration\n"
             "# Generated on: %s\n\n"
             "enabled=%s\n"
             "signal_threshold_dbm=%d\n"
             "scan_interval_ms=%u\n"
             "handover_timeout_ms=%u\n"
             "aggressive_roaming=%s\n",
             "2025-01-28",
             config->enabled ? "true" : "false",
             config->signal_threshold_dbm,
             config->scan_interval_ms,
             config->handover_timeout_ms,
             config->aggressive_roaming ? "true" : "false");
    
    return 0;
}

/**
 * @brief Parse WiFi configuration
 */
static int parse_wifi_config(const char *content, wifi_roaming_config_t *config) {
    if (!content || !config) {
        return -1;
    }
    
    // Initialize with defaults
    wifi_config_create_default(config);
    
    char *line = strtok((char*)content, "\n");
    while (line) {
        char key[64], value[256];
        if (parse_config_line(line, key, value) == 0) {
            if (strcmp(key, "enabled") == 0) {
                config->enabled = (strcmp(value, "true") == 0);
            } else if (strcmp(key, "signal_threshold_dbm") == 0) {
                config->signal_threshold_dbm = atoi(value);
            } else if (strcmp(key, "scan_interval_ms") == 0) {
                config->scan_interval_ms = (uint32_t)atoll(value);
            } else if (strcmp(key, "handover_timeout_ms") == 0) {
                config->handover_timeout_ms = (uint32_t)atoll(value);
            } else if (strcmp(key, "aggressive_roaming") == 0) {
                config->aggressive_roaming = (strcmp(value, "true") == 0);
            }
        }
        line = strtok(NULL, "\n");
    }
    
    return 0;
}
