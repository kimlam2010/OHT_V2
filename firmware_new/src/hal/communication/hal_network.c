#include "hal_network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

// Network configuration
static network_config_t network_config = {
    .type = NETWORK_TYPE_BOTH,
    .dhcp_enabled = true,
    .ip_address = "192.168.1.100",
    .gateway = "192.168.1.1",
    .netmask = "255.255.255.0",
    .dns1 = "8.8.8.8",
    .dns2 = "8.8.4.4",
    .timeout_ms = 10000,
    .auto_reconnect = true
};

// WiFi configuration
static wifi_config_t wifi_config = {
    .ssid = "OHT50-Network",
    .password = "secure_password",
    .security = WIFI_SECURITY_WPA3,
    .band = NETWORK_WIFI_BAND_5G,
    .backup_only = true,
    .scan_timeout_ms = 5000
};

// Network status
static network_status_t network_status = {0};
static bool network_initialized = false;
static pthread_t network_monitor_thread;
static bool network_thread_running = false;
static network_event_callback_t network_callback = NULL;

// Failover configuration
static bool failover_enabled = false;
static network_type_t primary_network = NETWORK_TYPE_ETHERNET;
static network_type_t backup_network = NETWORK_TYPE_WIFI;
static network_type_t active_network = NETWORK_TYPE_ETHERNET;

// Internal functions
static uint64_t get_timestamp_ms(void);
static void* network_monitor_thread_func(void *arg);
static hal_status_t execute_command(const char *command, char *output, size_t output_size);
static hal_status_t get_interface_status(const char *interface, bool *up, char *ip_address);
static hal_status_t get_mac_address(const char *interface, char *mac_address);
static hal_status_t ping_host(const char *host, uint32_t timeout_ms, uint32_t *latency_ms);

hal_status_t hal_network_init(const network_config_t *config) {
    if (network_initialized) {
        return HAL_STATUS_OK;
    }

    printf("Initializing network system...\n");

    // Copy configuration
    if (config != NULL) {
        network_config = *config;
    }

    // Initialize status
    memset(&network_status, 0, sizeof(network_status));
    network_status.state = NETWORK_STATE_DISCONNECTED;
    network_status.active_type = NETWORK_TYPE_ETHERNET;
    strcpy(network_status.ip_address, "0.0.0.0");
    strcpy(network_status.mac_address, "00:00:00:00:00:00");
    network_status.speed_mbps = 0;
    network_status.signal_strength = 0;
    network_status.packet_loss = 0;
    network_status.latency_ms = 0;
    network_status.bytes_sent = 0;
    network_status.bytes_received = 0;
    network_status.connection_time = 0;
    network_status.disconnect_count = 0;

    // Initialize Ethernet interface
    printf("Ethernet interface check...\n");
    bool eth_up;
    char eth_ip[16];
    hal_status_t status = get_interface_status(NETWORK_ETH_INTERFACE, &eth_up, eth_ip);
    if (status != HAL_STATUS_OK) {
        printf("Ethernet interface not found\n");
        return status;
    }
    printf("Ethernet interface found: %s\n", eth_up ? "UP" : "DOWN");

    // Initialize WiFi interface
    printf("WiFi interface check...\n");
    bool wifi_up;
    char wifi_ip[16];
    status = get_interface_status(NETWORK_WIFI_INTERFACE, &wifi_up, wifi_ip);
    if (status != HAL_STATUS_OK) {
        printf("WiFi interface not found\n");
        return status;
    }
    printf("WiFi interface found: %s\n", wifi_up ? "UP" : "DOWN");

    // Start network monitor thread
    network_thread_running = true;
    if (pthread_create(&network_monitor_thread, NULL, network_monitor_thread_func, NULL) != 0) {
        printf("Failed to create network monitor thread\n");
        return HAL_STATUS_ERROR;
    }

    network_initialized = true;
    printf("Network system initialized successfully\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_network_deinit(void) {
    if (!network_initialized) {
        return HAL_STATUS_OK;
    }

    printf("Deinitializing network system...\n");

    // Stop network monitor thread
    network_thread_running = false;
    if (network_monitor_thread != 0) {
        pthread_join(network_monitor_thread, NULL);
        network_monitor_thread = 0;
    }

    // Disconnect networks
    if (network_status.state == NETWORK_STATE_CONNECTED) {
        if (network_status.active_type == NETWORK_TYPE_ETHERNET) {
            hal_ethernet_disconnect();
        } else if (network_status.active_type == NETWORK_TYPE_WIFI) {
            hal_wifi_disconnect();
        }
        network_status.state = NETWORK_STATE_DISCONNECTED;
        strcpy(network_status.ip_address, "0.0.0.0");
    }

    network_initialized = false;
    printf("Network system deinitialized\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_network_connect(network_type_t type) {
    if (!network_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Connecting to network type: %d\n", type);

    hal_status_t status;
    switch (type) {
        case NETWORK_TYPE_ETHERNET:
            status = hal_ethernet_connect();
            break;
        case NETWORK_TYPE_WIFI:
            status = hal_wifi_connect();
            break;
        case NETWORK_TYPE_BOTH:
            status = hal_ethernet_connect();
            if (status == HAL_STATUS_OK) {
                status = hal_wifi_connect();
            }
            break;
        default:
            return HAL_STATUS_INVALID_PARAMETER;
    }

    if (status == HAL_STATUS_OK) {
        network_status.state = NETWORK_STATE_CONNECTED;
        network_status.active_type = type;
        network_status.connection_time = get_timestamp_ms();

        // Call callback if set
        if (network_callback != NULL) {
            network_callback(NETWORK_STATE_CONNECTED, type);
        }
    } else {
        network_status.state = NETWORK_STATE_FAILED;
        network_status.disconnect_count++;

        // Call callback if set
        if (network_callback != NULL) {
            network_callback(NETWORK_STATE_FAILED, type);
        }
    }

    return status;
}

hal_status_t hal_network_disconnect(network_type_t type) {
    if (!network_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    printf("Disconnecting from network type: %d\n", type);

    hal_status_t status;
    switch (type) {
        case NETWORK_TYPE_ETHERNET:
            status = hal_ethernet_disconnect();
            break;
        case NETWORK_TYPE_WIFI:
            status = hal_wifi_disconnect();
            break;
        case NETWORK_TYPE_BOTH:
            hal_ethernet_disconnect();
            status = hal_wifi_disconnect();
            break;
        default:
            return HAL_STATUS_INVALID_PARAMETER;
    }

    if (status == HAL_STATUS_OK) {
        network_status.state = NETWORK_STATE_DISCONNECTED;
        network_status.disconnect_count++;

        // Call callback if set
        if (network_callback != NULL) {
            network_callback(NETWORK_STATE_DISCONNECTED, type);
        }
    }

    return status;
}

hal_status_t hal_network_get_status(network_status_t *status) {
    if (!network_initialized || status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *status = network_status;
    return HAL_STATUS_OK;
}

hal_status_t hal_network_is_connected(network_type_t type, bool *connected) {
    if (!network_initialized || connected == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    switch (type) {
        case NETWORK_TYPE_ETHERNET:
            return hal_ethernet_get_status(&network_status);
        case NETWORK_TYPE_WIFI:
            return hal_wifi_get_status(&network_status);
        case NETWORK_TYPE_BOTH:
            *connected = (network_status.state == NETWORK_STATE_CONNECTED);
            return HAL_STATUS_OK;
        default:
            return HAL_STATUS_INVALID_PARAMETER;
    }
}

hal_status_t hal_network_set_config(const network_config_t *config) {
    if (!network_initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    network_config = *config;
    return HAL_STATUS_OK;
}

hal_status_t hal_network_get_config(network_config_t *config) {
    if (!network_initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *config = network_config;
    return HAL_STATUS_OK;
}

hal_status_t hal_network_set_wifi_config(const wifi_config_t *config) {
    if (!network_initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    wifi_config = *config;
    return HAL_STATUS_OK;
}

hal_status_t hal_network_get_wifi_config(wifi_config_t *config) {
    if (!network_initialized || config == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *config = wifi_config;
    return HAL_STATUS_OK;
}

hal_status_t hal_network_set_callback(network_event_callback_t callback) {
    // Allow setting callback even if not initialized (for testing)
    network_callback = callback;
    return HAL_STATUS_OK;
}

hal_status_t hal_network_update(void) {
    if (!network_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }

    // Update network status based on active type
    switch (network_status.active_type) {
        case NETWORK_TYPE_ETHERNET:
            hal_ethernet_get_status(&network_status);
            break;
        case NETWORK_TYPE_WIFI:
            hal_wifi_get_status(&network_status);
            break;
        case NETWORK_TYPE_BOTH: {
            // Check both interfaces
            network_status_t eth_status, wifi_status;
            hal_ethernet_get_status(&eth_status);
            hal_wifi_get_status(&wifi_status);
            
            // Use the one that's connected
            if (eth_status.state == NETWORK_STATE_CONNECTED) {
                network_status = eth_status;
                network_status.active_type = NETWORK_TYPE_ETHERNET;
            } else if (wifi_status.state == NETWORK_STATE_CONNECTED) {
                network_status = wifi_status;
                network_status.active_type = NETWORK_TYPE_WIFI;
            }
            break;
        }
    }

    return HAL_STATUS_OK;
}

// Ethernet Functions
hal_status_t hal_ethernet_init(void) {
    printf("Initializing Ethernet interface...\n");
    
    // Check if Ethernet interface exists
    bool up;
    char ip_address[16];
    hal_status_t status = get_interface_status(NETWORK_ETH_INTERFACE, &up, ip_address);
    if (status != HAL_STATUS_OK) {
        printf("Ethernet interface not found\n");
        return status;
    }

    printf("Ethernet interface initialized\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_ethernet_connect(void) {
    printf("Connecting Ethernet...\n");

    // Real Ethernet connection implementation
    printf("Connecting Ethernet...\n");
    
    // Bring interface up
    char command[256];
    snprintf(command, sizeof(command), "ifconfig %s up", NETWORK_ETH_INTERFACE);
    char output[512];
    hal_status_t status = execute_command(command, output, sizeof(output));
    if (status != HAL_STATUS_OK) {
        printf("Failed to bring Ethernet interface up\n");
        return status;
    }
    
    // Get IP address via DHCP
    if (network_config.dhcp_enabled) {
        snprintf(command, sizeof(command), "dhclient %s", NETWORK_ETH_INTERFACE);
        status = execute_command(command, output, sizeof(output));
        if (status != HAL_STATUS_OK) {
            printf("Failed to get DHCP lease for Ethernet\n");
            return status;
        }
    }
    
    // Get actual IP and MAC address
    bool up;
    char ip_address[16];
    status = get_interface_status(NETWORK_ETH_INTERFACE, &up, ip_address);
    if (status == HAL_STATUS_OK && up) {
        strcpy(network_status.ip_address, ip_address);
        get_mac_address(NETWORK_ETH_INTERFACE, network_status.mac_address);
        network_status.state = NETWORK_STATE_CONNECTED;
        network_status.active_type = NETWORK_TYPE_ETHERNET;
        hal_ethernet_get_speed(&network_status.speed_mbps);
        
        printf("Ethernet connected: %s\n", network_status.ip_address);
        return HAL_STATUS_OK;
    }
    
    printf("Ethernet connection failed\n");
    return HAL_STATUS_ERROR;
}

hal_status_t hal_ethernet_disconnect(void) {
    printf("Disconnecting Ethernet...\n");

    // Real Ethernet disconnect implementation
    char command[256];
    snprintf(command, sizeof(command), "ifconfig %s down", NETWORK_ETH_INTERFACE);
    char output[512];
    hal_status_t status = execute_command(command, output, sizeof(output));
    
    network_status.state = NETWORK_STATE_DISCONNECTED;
    strcpy(network_status.ip_address, "0.0.0.0");
    printf("Ethernet disconnected\n");

    return status;
}

hal_status_t hal_ethernet_get_status(network_status_t *status) {
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    bool up;
    char ip_address[16];
    hal_status_t hal_status = get_interface_status(NETWORK_ETH_INTERFACE, &up, ip_address);
    
    if (hal_status == HAL_STATUS_OK && up) {
        status->state = NETWORK_STATE_CONNECTED;
        strcpy(status->ip_address, ip_address);
        get_mac_address(NETWORK_ETH_INTERFACE, status->mac_address);
        hal_ethernet_get_speed(&status->speed_mbps);
    } else {
        status->state = NETWORK_STATE_DISCONNECTED;
        strcpy(status->ip_address, "0.0.0.0");
    }

    return HAL_STATUS_OK;
}

hal_status_t hal_ethernet_get_speed(uint32_t *speed_mbps) {
    if (speed_mbps == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Read Ethernet speed from /sys/class/net/eth0/speed
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/net/%s/speed", NETWORK_ETH_INTERFACE);
    
    FILE *fp = fopen(path, "r");
    if (!fp) {
        *speed_mbps = 0;
        return HAL_STATUS_ERROR;
    }
    
    if (fscanf(fp, "%u", speed_mbps) != 1) {
        *speed_mbps = 0;
    }
    fclose(fp);
    
    return HAL_STATUS_OK;
}

hal_status_t hal_ethernet_test_connectivity(const char *target_ip, uint32_t timeout_ms) {
    if (target_ip == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    uint32_t latency_ms;
    return ping_host(target_ip, timeout_ms, &latency_ms);
}

// WiFi Functions
hal_status_t hal_wifi_init(void) {
    printf("Initializing WiFi interface...\n");
    
    // Check if WiFi interface exists
    bool up;
    char ip_address[16];
    hal_status_t status = get_interface_status(NETWORK_WIFI_INTERFACE, &up, ip_address);
    if (status != HAL_STATUS_OK) {
        printf("WiFi interface not found\n");
        return status;
    }

    printf("WiFi interface initialized\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_wifi_connect(void) {
    printf("Connecting WiFi to SSID: %s\n", wifi_config.ssid);

    // Create wpa_supplicant configuration
    char config_file[256];
    snprintf(config_file, sizeof(config_file), "/tmp/wpa_supplicant_%s.conf", NETWORK_WIFI_INTERFACE);
    
    FILE *fp = fopen(config_file, "w");
    if (!fp) {
        printf("Failed to create wpa_supplicant config\n");
        return HAL_STATUS_ERROR;
    }

    fprintf(fp, "network={\n");
    fprintf(fp, "    ssid=\"%s\"\n", wifi_config.ssid);
    fprintf(fp, "    psk=\"%s\"\n", wifi_config.password);
    
    switch (wifi_config.security) {
        case WIFI_SECURITY_WPA3:
            fprintf(fp, "    key_mgmt=WPA-PSK\n");
            fprintf(fp, "    proto=RSN\n");
            fprintf(fp, "    pairwise=CCMP\n");
            fprintf(fp, "    group=CCMP\n");
            break;
        case WIFI_SECURITY_WPA2:
            fprintf(fp, "    key_mgmt=WPA-PSK\n");
            fprintf(fp, "    proto=RSN\n");
            fprintf(fp, "    pairwise=CCMP\n");
            break;
        case WIFI_SECURITY_WPA:
            fprintf(fp, "    key_mgmt=WPA-PSK\n");
            fprintf(fp, "    proto=WPA\n");
            fprintf(fp, "    pairwise=TKIP\n");
            break;
        case WIFI_SECURITY_WEP:
            fprintf(fp, "    key_mgmt=NONE\n");
            fprintf(fp, "    wep_key0=\"%s\"\n", wifi_config.password);
            break;
        case WIFI_SECURITY_NONE:
            fprintf(fp, "    key_mgmt=NONE\n");
            break;
    }
    
    fprintf(fp, "}\n");
    fclose(fp);

    // Start wpa_supplicant
    char command[512];
    snprintf(command, sizeof(command), 
            "wpa_supplicant -B -i %s -c %s",
            NETWORK_WIFI_INTERFACE, config_file);
    
    char output[512];
    hal_status_t status = execute_command(command, output, sizeof(output));
    if (status != HAL_STATUS_OK) {
        printf("Failed to start wpa_supplicant\n");
        return status;
    }

    // Wait for connection
    usleep(2000000); // 2 seconds

    // Get IP address
    if (network_config.dhcp_enabled) {
        snprintf(command, sizeof(command), "dhclient %s", NETWORK_WIFI_INTERFACE);
        status = execute_command(command, output, sizeof(output));
        if (status != HAL_STATUS_OK) {
            printf("Failed to get DHCP lease for WiFi\n");
            return status;
        }
    }

    // Update status
    bool up;
    char ip_address[16];
    status = get_interface_status(NETWORK_WIFI_INTERFACE, &up, ip_address);
    if (status == HAL_STATUS_OK && up) {
        strcpy(network_status.ip_address, ip_address);
        get_mac_address(NETWORK_WIFI_INTERFACE, network_status.mac_address);
        network_status.state = NETWORK_STATE_CONNECTED;
        network_status.active_type = NETWORK_TYPE_WIFI;
        printf("WiFi connected: %s\n", ip_address);
        return HAL_STATUS_OK;
    }

    printf("WiFi connection failed\n");
    return HAL_STATUS_ERROR;
}

hal_status_t hal_wifi_disconnect(void) {
    printf("Disconnecting WiFi...\n");

    // Kill wpa_supplicant
    char command[256];
    snprintf(command, sizeof(command), "pkill -f wpa_supplicant");
    
    char output[512];
    hal_status_t status = execute_command(command, output, sizeof(output));

    // Bring interface down
    snprintf(command, sizeof(command), "ifconfig %s down", NETWORK_WIFI_INTERFACE);
    status = execute_command(command, output, sizeof(output));
    
    if (status == HAL_STATUS_OK) {
        network_status.state = NETWORK_STATE_DISCONNECTED;
        printf("WiFi disconnected\n");
    }

    return status;
}

hal_status_t hal_wifi_get_status(network_status_t *status) {
    if (status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    bool up;
    char ip_address[16];
    hal_status_t hal_status = get_interface_status(NETWORK_WIFI_INTERFACE, &up, ip_address);
    
    if (hal_status == HAL_STATUS_OK && up) {
        status->state = NETWORK_STATE_CONNECTED;
        strcpy(status->ip_address, ip_address);
        get_mac_address(NETWORK_WIFI_INTERFACE, status->mac_address);
        hal_wifi_get_signal_strength(&status->signal_strength);
        status->speed_mbps = 54; // Typical WiFi speed
    } else {
        status->state = NETWORK_STATE_DISCONNECTED;
        strcpy(status->ip_address, "0.0.0.0");
        status->signal_strength = 0;
    }

    return HAL_STATUS_OK;
}

hal_status_t hal_wifi_scan(char (*networks)[32], uint32_t max_networks, uint32_t *count) {
    printf("[HAL-NETWORK] Scanning for WiFi networks...\n");
    
    if (networks == NULL || count == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *count = 0;
    
    // Use nmcli to scan for networks
    char command[128] = "nmcli -t -f SSID dev wifi list";
    char output[2048];
    hal_status_t status = execute_command(command, output, sizeof(output));
    
    if (status != HAL_STATUS_OK) {
        printf("[HAL-NETWORK] WiFi scan command failed\n");
        return HAL_STATUS_ERROR;
    }
    
    // Parse SSIDs from output
    char *line = strtok(output, "\n");
    while (line != NULL && *count < max_networks) {
        // Remove any trailing whitespace
        char *end = line + strlen(line) - 1;
        while (end > line && (*end == '\r' || *end == '\n' || *end == ' ')) {
            *end = '\0';
            end--;
        }
        
        // Skip empty lines and hidden networks
        if (strlen(line) > 0 && strcmp(line, "--") != 0) {
            strncpy(networks[*count], line, 31);
            networks[*count][31] = '\0';
            (*count)++;
        }
        
        line = strtok(NULL, "\n");
    }
    
    printf("[HAL-NETWORK] ✅ Found %u WiFi networks\n", *count);
    return HAL_STATUS_OK;
}

hal_status_t hal_wifi_get_signal_strength(uint32_t *strength) {
    if (strength == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Read signal strength from /proc/net/wireless
    char command[256];
    snprintf(command, sizeof(command), "cat /proc/net/wireless | grep %s", NETWORK_WIFI_INTERFACE);
    
    char output[512];
    hal_status_t status = execute_command(command, output, sizeof(output));
    if (status != HAL_STATUS_OK) {
        *strength = 0;
        return HAL_STATUS_ERROR;
    }

    // Parse signal strength (simplified)
    int signal_db;
    if (sscanf(output, "%*s %*d %d", &signal_db) == 1) {
        // Convert dBm to percentage (rough approximation)
        if (signal_db >= -50) {
            *strength = 100;
        } else if (signal_db >= -60) {
            *strength = 80;
        } else if (signal_db >= -70) {
            *strength = 60;
        } else if (signal_db >= -80) {
            *strength = 40;
        } else if (signal_db >= -90) {
            *strength = 20;
        } else {
            *strength = 0;
        }
    } else {
        *strength = 0;
    }

    return HAL_STATUS_OK;
}

hal_status_t hal_wifi_get_channel(uint32_t *channel) {
    // Real WiFi channel implementation
    if (channel == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    // Get current channel using iwgetid
    char command[256];
    snprintf(command, sizeof(command), "iwgetid %s --channel", NETWORK_WIFI_INTERFACE);
    
    char output[512];
    hal_status_t status = execute_command(command, output, sizeof(output));
    if (status != HAL_STATUS_OK) {
        *channel = 0;
        return HAL_STATUS_ERROR;
    }

    // Parse channel number
    if (sscanf(output, "%u", channel) != 1) {
        *channel = 0;
        return HAL_STATUS_ERROR;
    }

    return HAL_STATUS_OK;
}

hal_status_t hal_wifi_test_connectivity(const char *target_ip, uint32_t timeout_ms) {
    if (target_ip == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    uint32_t latency_ms;
    return ping_host(target_ip, timeout_ms, &latency_ms);
}

// Network Utility Functions
hal_status_t hal_network_ping(const char *host, uint32_t timeout_ms, uint32_t *latency_ms) {
    if (host == NULL || latency_ms == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    return ping_host(host, timeout_ms, latency_ms);
}

hal_status_t hal_network_get_statistics(uint64_t *bytes_sent, uint64_t *bytes_received) {
    if (bytes_sent == NULL || bytes_received == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    *bytes_sent = network_status.bytes_sent;
    *bytes_received = network_status.bytes_received;
    return HAL_STATUS_OK;
}

hal_status_t hal_network_reset_statistics(void) {
    network_status.bytes_sent = 0;
    network_status.bytes_received = 0;
    network_status.disconnect_count = 0;
    return HAL_STATUS_OK;
}

hal_status_t hal_network_get_diagnostics(char *info, size_t max_len) {
    if (info == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }

    snprintf(info, max_len,
             "Network Diagnostics:\n"
             "State: %d\n"
             "Active Type: %d\n"
             "IP Address: %s\n"
             "MAC Address: %s\n"
             "Speed: %u Mbps\n"
             "Signal Strength: %u%%\n"
             "Latency: %u ms\n"
             "Bytes Sent: %lu\n"
             "Bytes Received: %lu\n"
             "Disconnect Count: %u\n"
             "Failover Enabled: %s\n",
             network_status.state,
             network_status.active_type,
             network_status.ip_address,
             network_status.mac_address,
             network_status.speed_mbps,
             network_status.signal_strength,
             network_status.latency_ms,
             network_status.bytes_sent,
             network_status.bytes_received,
             network_status.disconnect_count,
             failover_enabled ? "Yes" : "No");

    return HAL_STATUS_OK;
}

hal_status_t hal_network_self_test(void) {
    printf("Running network self-test...\n");

    // Test Ethernet
    hal_status_t status = hal_ethernet_test_connectivity("8.8.8.8", 5000);
    if (status != HAL_STATUS_OK) {
        printf("Ethernet self-test failed\n");
        return status;
    }

    // Test WiFi
    status = hal_wifi_test_connectivity("8.8.8.8", 5000);
    if (status != HAL_STATUS_OK) {
        printf("WiFi self-test failed\n");
        return status;
    }

    printf("Network self-test passed\n");
    return HAL_STATUS_OK;
}

hal_status_t hal_network_validate_hardware(void) {
    printf("Validating network hardware...\n");

    // Check Ethernet interface
    bool eth_up;
    char eth_ip[16];
    hal_status_t status = get_interface_status(NETWORK_ETH_INTERFACE, &eth_up, eth_ip);
    if (status != HAL_STATUS_OK) {
        printf("Ethernet hardware validation failed\n");
        return status;
    }

    // Check WiFi interface
    bool wifi_up;
    char wifi_ip[16];
    status = get_interface_status(NETWORK_WIFI_INTERFACE, &wifi_up, wifi_ip);
    if (status != HAL_STATUS_OK) {
        printf("WiFi hardware validation failed\n");
        return status;
    }

    printf("Network hardware validation passed\n");
    return HAL_STATUS_OK;
}

// Failover Functions - Simplified for OHT-50 Master Module
hal_status_t hal_network_enable_failover(network_type_t primary_type, network_type_t backup_type) {
    // Simplified - basic failover only
    if (!network_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    failover_enabled = true;
    primary_network = primary_type;
    backup_network = backup_type;
    active_network = primary_type;
    
    return HAL_STATUS_OK;
}

hal_status_t hal_network_disable_failover(void) {
    if (!network_initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    failover_enabled = false;
    return HAL_STATUS_OK;
}

hal_status_t hal_network_get_failover_status(network_type_t *active_type, bool *failover_enabled_status) {
    if (!network_initialized || active_type == NULL || failover_enabled_status == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *active_type = active_network;
    *failover_enabled_status = failover_enabled;
    return HAL_STATUS_OK;
}

hal_status_t hal_network_switch_to_backup(void) {
    if (!network_initialized || !failover_enabled) {
        return HAL_STATUS_ERROR;
    }
    
    active_network = backup_network;
    return hal_network_connect(backup_network);
}

hal_status_t hal_network_switch_to_primary(void) {
    if (!network_initialized || !failover_enabled) {
        return HAL_STATUS_ERROR;
    }
    
    active_network = primary_network;
    return hal_network_connect(primary_network);
}

// Internal helper functions
static uint64_t get_timestamp_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec / 1000;
}

__attribute__((unused))
static void* network_monitor_thread_func(void *arg) {
    (void)arg; // Unused parameter
    
    while (network_thread_running) {
        hal_network_update();
        
        // Check failover if enabled
        if (failover_enabled) {
            bool connected = false; // Initialize to prevent uninitialized warning
            hal_status_t status = hal_network_is_connected(active_network, &connected);
            if (status == HAL_STATUS_OK && !connected) {
                printf("Network failover: switching to backup\n");
                hal_network_switch_to_backup();
            }
        }
        
        usleep(1000000); // 1 second update rate
    }
    
    return NULL;
}

static hal_status_t execute_command(const char *command, char *output, size_t output_size) {
    FILE *fp = popen(command, "r");
    if (!fp) {
        return HAL_STATUS_ERROR;
    }
    
    size_t bytes_read = fread(output, 1, output_size - 1, fp);
    output[bytes_read] = '\0';
    
    int status = pclose(fp);
    return (status == 0) ? HAL_STATUS_OK : HAL_STATUS_ERROR;
}

static hal_status_t get_interface_status(const char *interface, bool *up, char *ip_address) {
    // Real implementation using socket ioctl calls
    
    if (interface == NULL || up == NULL || ip_address == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    // Check if interface exists
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/net/%s", interface);
    
    if (access(path, F_OK) != 0) {
        *up = false;
        strcpy(ip_address, "0.0.0.0");
        return HAL_STATUS_ERROR;
    }
    
    // Check if interface is up
    snprintf(path, sizeof(path), "/sys/class/net/%s/operstate", interface);
    FILE *fp = fopen(path, "r");
    if (fp) {
        char state[16];
        if (fgets(state, sizeof(state), fp) != NULL) {
            *up = (strstr(state, "up") != NULL);
        } else {
            *up = false;
        }
        fclose(fp);
    } else {
        *up = false;
    }
    
    // Get actual IP address using ioctl
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        strcpy(ip_address, "0.0.0.0");
        return HAL_STATUS_ERROR;
    }
    
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interface, IFNAMSIZ - 1);
    
    if (ioctl(sock, SIOCGIFADDR, &ifr) == 0) {
        struct sockaddr_in *addr = (struct sockaddr_in*)&ifr.ifr_addr;
        inet_ntop(AF_INET, &addr->sin_addr, ip_address, 16);
    } else {
        strcpy(ip_address, "0.0.0.0");
    }
    
    close(sock);
    return HAL_STATUS_OK;
}

static hal_status_t get_mac_address(const char *interface, char *mac_address) {
    // Simplified implementation for testing
    // Read MAC address from /sys/class/net/interface/address
    
    if (interface == NULL || mac_address == NULL) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/net/%s/address", interface);
    
    FILE *fp = fopen(path, "r");
    if (!fp) {
        strcpy(mac_address, "00:00:00:00:00:00");
        return HAL_STATUS_ERROR;
    }
    
    if (fgets(mac_address, 18, fp) != NULL) {
        // Remove newline
        mac_address[strcspn(mac_address, "\n")] = 0;
    } else {
        strcpy(mac_address, "00:00:00:00:00:00");
    }
    
    fclose(fp);
    return HAL_STATUS_OK;
}

static hal_status_t ping_host(const char *host, uint32_t timeout_ms, uint32_t *latency_ms) {
    char command[256];
    snprintf(command, sizeof(command), "ping -c 1 -W %u %s", timeout_ms / 1000, host);
    
    char output[512];
    hal_status_t status = execute_command(command, output, sizeof(output));
    if (status != HAL_STATUS_OK) {
        *latency_ms = 0;
        return HAL_STATUS_ERROR;
    }
    
    // Parse ping output for time
    char *time_str = strstr(output, "time=");
    if (time_str != NULL) {
        float time_ms;
        if (sscanf(time_str, "time=%f ms", &time_ms) == 1) {
            *latency_ms = (uint32_t)time_ms;
            return HAL_STATUS_OK;
        }
    }
    
    *latency_ms = 0;
    return HAL_STATUS_ERROR;
}

