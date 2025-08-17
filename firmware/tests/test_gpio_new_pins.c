#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

// New GPIO pin definitions
#define LED_POWER_PIN          54  // GPIO1_D6 - Power LED (Green)
#define LED_SYSTEM_PIN         35  // GPIO1_A3 - System LED (Blue)
#define LED_COMM_PIN           28  // GPIO0_D4 - Communication LED (Yellow)
#define LED_NETWORK_PIN        29  // GPIO0_D5 - Network LED (Green)
#define LED_ERROR_PIN          58  // GPIO1_D2 - Error LED (Red)
#define ESTOP_CHANNEL1_PIN     59  // GPIO1_D3 - E-Stop Channel 1
#define ESTOP_CHANNEL2_PIN     131 // GPIO4_A3 - E-Stop Channel 2
#define RELAY_OUTPUT_PIN       132 // GPIO4_A4 - Relay Output

int test_gpio_pin(int pin, const char *name) {
    printf("Testing %s (GPIO %d)...\n", name, pin);
    
    // Export GPIO
    FILE *fp = fopen("/sys/class/gpio/export", "w");
    if (!fp) {
        printf("❌ Failed to open export for GPIO %d\n", pin);
        return 0;
    }
    fprintf(fp, "%d", pin);
    fclose(fp);
    
    // Wait a moment
    usleep(100000);
    
    // Check if GPIO directory exists
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d", pin);
    if (access(path, F_OK) != 0) {
        printf("❌ GPIO %d directory not found\n", pin);
        return 0;
    }
    
    // Set direction to output
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    fp = fopen(path, "w");
    if (!fp) {
        printf("❌ Failed to set direction for GPIO %d\n", pin);
        return 0;
    }
    fprintf(fp, "out");
    fclose(fp);
    
    // Set value to HIGH
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    fp = fopen(path, "w");
    if (!fp) {
        printf("❌ Failed to set value for GPIO %d\n", pin);
        return 0;
    }
    fprintf(fp, "1");
    fclose(fp);
    
    printf("✅ GPIO %d set to HIGH\n", pin);
    
    // Wait 500ms
    usleep(500000);
    
    // Set value to LOW
    fp = fopen(path, "w");
    if (fp) {
        fprintf(fp, "0");
        fclose(fp);
        printf("✅ GPIO %d set to LOW\n", pin);
    }
    
    // Unexport
    fp = fopen("/sys/class/gpio/unexport", "w");
    if (fp) {
        fprintf(fp, "%d", pin);
        fclose(fp);
    }
    
    return 1;
}

int main(void) {
    printf("=== New GPIO Pins Test ===\n");
    printf("Testing available pins: 54, 35, 28, 29, 58, 59, 131, 132\n\n");
    
    int passed = 0;
    int total = 0;
    
    // Test LED pins
    if (test_gpio_pin(LED_POWER_PIN, "LED Power")) passed++;
    total++;
    
    if (test_gpio_pin(LED_SYSTEM_PIN, "LED System")) passed++;
    total++;
    
    if (test_gpio_pin(LED_COMM_PIN, "LED Communication")) passed++;
    total++;
    
    if (test_gpio_pin(LED_NETWORK_PIN, "LED Network")) passed++;
    total++;
    
    if (test_gpio_pin(LED_ERROR_PIN, "LED Error")) passed++;
    total++;
    
    // Test E-Stop pins
    if (test_gpio_pin(ESTOP_CHANNEL1_PIN, "E-Stop Channel 1")) passed++;
    total++;
    
    if (test_gpio_pin(ESTOP_CHANNEL2_PIN, "E-Stop Channel 2")) passed++;
    total++;
    
    // Test Relay pin
    if (test_gpio_pin(RELAY_OUTPUT_PIN, "Relay Output")) passed++;
    total++;
    
    printf("\n=== Test Summary ===\n");
    printf("Passed: %d/%d\n", passed, total);
    printf("Success rate: %.1f%%\n", (float)passed / total * 100.0);
    
    if (passed == total) {
        printf("✅ All GPIO pins working correctly!\n");
    } else {
        printf("❌ Some GPIO pins failed\n");
    }
    
    return (passed == total) ? 0 : 1;
}
