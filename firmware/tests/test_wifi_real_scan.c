#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    printf("=== Real WiFi Scan Test ===\n");
    printf("Scanning actual WiFi networks...\n\n");

    // Method 1: Using iwlist
    printf("🔍 Method 1: Using iwlist\n");
    printf("========================\n");
    
    FILE *fp = popen("iwlist wlan0 scan 2>/dev/null | grep ESSID", "r");
    if (fp) {
        char line[256];
        int count = 0;
        while (fgets(line, sizeof(line), fp)) {
            char *essid_start = strstr(line, "ESSID:");
            if (essid_start) {
                char *quote_start = strchr(essid_start, '"');
                if (quote_start) {
                    quote_start++; // Skip the quote
                    char *quote_end = strchr(quote_start, '"');
                    if (quote_end) {
                        *quote_end = '\0'; // Null terminate
                        if (strlen(quote_start) > 0) {
                            printf("%2d. %s\n", ++count, quote_start);
                        }
                    }
                }
            }
        }
        pclose(fp);
        printf("Found %d networks with iwlist\n\n", count);
    } else {
        printf("iwlist command failed\n\n");
    }

    // Method 2: Using nmcli
    printf("🔍 Method 2: Using nmcli\n");
    printf("========================\n");
    
    fp = popen("nmcli -t -f SSID dev wifi list 2>/dev/null | grep -v '^$' | sort | uniq", "r");
    if (fp) {
        char line[256];
        int count = 0;
        while (fgets(line, sizeof(line), fp)) {
            line[strcspn(line, "\n")] = 0; // Remove newline
            if (strlen(line) > 0) {
                printf("%2d. %s\n", ++count, line);
            }
        }
        pclose(fp);
        printf("Found %d networks with nmcli\n\n", count);
    } else {
        printf("nmcli command failed\n\n");
    }

    // Method 3: Using iw dev scan
    printf("🔍 Method 3: Using iw dev scan\n");
    printf("==============================\n");
    
    // First trigger scan
    system("iw dev wlan0 scan 2>/dev/null > /dev/null");
    usleep(2000000); // Wait 2 seconds for scan to complete
    
    fp = popen("iw dev wlan0 scan 2>/dev/null | grep SSID", "r");
    if (fp) {
        char line[256];
        int count = 0;
        while (fgets(line, sizeof(line), fp)) {
            char *ssid_start = strstr(line, "SSID:");
            if (ssid_start) {
                ssid_start += 5; // Skip "SSID:"
                while (*ssid_start == ' ') ssid_start++; // Skip spaces
                line[strcspn(line, "\n")] = 0; // Remove newline
                if (strlen(ssid_start) > 0) {
                    printf("%2d. %s\n", ++count, ssid_start);
                }
            }
        }
        pclose(fp);
        printf("Found %d networks with iw dev scan\n\n", count);
    } else {
        printf("iw dev scan command failed\n\n");
    }

    // Check WiFi interface status
    printf("📊 WiFi Interface Status:\n");
    printf("=========================\n");
    
    fp = popen("ip addr show wlan0 2>/dev/null", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            line[strcspn(line, "\n")] = 0;
            if (strstr(line, "state") || strstr(line, "inet")) {
                printf("%s\n", line);
            }
        }
        pclose(fp);
    }

    printf("\n✅ WiFi scan completed!\n");
    return 0;
}
