#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/hal_network.h"
#include "../include/hal_common.h"

int main(void) {
    printf("=== WiFi Scan Test Program ===\n");
    printf("Scanning available WiFi networks...\n\n");

    // Initialize network system
    hal_status_t status = hal_network_init(NULL);
    if (status != HAL_STATUS_OK) {
        printf("❌ Failed to initialize network system\n");
        return 1;
    }
    printf("✅ Network system initialized\n");

    // Initialize WiFi
    status = hal_wifi_init();
    if (status != HAL_STATUS_OK) {
        printf("❌ Failed to initialize WiFi\n");
        hal_network_deinit();
        return 1;
    }
    printf("✅ WiFi initialized\n");

    // Scan for networks
    printf("\n🔍 Scanning for WiFi networks...\n");
    char networks[20][32];  // Array to store SSIDs
    uint32_t count;
    
    status = hal_wifi_scan(networks, 20, &count);
    if (status != HAL_STATUS_OK) {
        printf("❌ WiFi scan failed\n");
        hal_network_deinit();
        return 1;
    }

    printf("✅ Scan completed. Found %d networks:\n\n", count);
    
    if (count == 0) {
        printf("No networks found\n");
    } else {
        printf("Available Networks:\n");
        printf("==================\n");
        for (uint32_t i = 0; i < count; i++) {
            printf("%2d. %s\n", i + 1, networks[i]);
        }
    }

    // Get WiFi status
    printf("\n📊 WiFi Status:\n");
    network_status_t wifi_status;
    status = hal_wifi_get_status(&wifi_status);
    if (status == HAL_STATUS_OK) {
        printf("State: %d\n", wifi_status.state);
        printf("IP: %s\n", wifi_status.ip_address);
        printf("MAC: %s\n", wifi_status.mac_address);
    }

    // Get signal strength
    uint32_t strength;
    status = hal_wifi_get_signal_strength(&strength);
    if (status == HAL_STATUS_OK) {
        printf("Signal Strength: %d%%\n", strength);
    }

    // Get channel
    uint32_t channel;
    status = hal_wifi_get_channel(&channel);
    if (status == HAL_STATUS_OK) {
        printf("Channel: %d\n", channel);
    }

    // Cleanup
    hal_network_deinit();
    printf("\n✅ Test completed\n");

    return 0;
}
