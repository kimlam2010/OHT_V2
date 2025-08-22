/**
 * @file gpio_pin_verify.c
 * @brief GPIO Pin Verification Tool for OHT-50 Master Module
 * @version 1.0.0
 * @date 2025-01-27
 * @team FIRMWARE
 * @task Task 2.1 (Hardware Pin Mapping Verification)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <stdbool.h>

// Pin definitions from HAL headers
#define ESTOP_PIN              59  // GPIO1_D3 - E-Stop (Single channel)
#define LED_POWER_PIN          54  // GPIO1_D6 - Power LED (Green)
#define RELAY1_OUTPUT_PIN      131 // GPIO4_A3 - Relay 1 output
#define RELAY2_OUTPUT_PIN      132 // GPIO4_A4 - Relay 2 output

// GPIO paths
#define GPIO_BASE_PATH         "/sys/class/gpio"
#define GPIO_EXPORT_PATH       "/sys/class/gpio/export"
#define GPIO_UNEXPORT_PATH     "/sys/class/gpio/unexport"

// Test status
static bool test_running = true;

// Signal handler for graceful shutdown
static void signal_handler(int sig) {
    printf("\nReceived signal %d, shutting down...\n", sig);
    test_running = false;
}

// GPIO utility functions
static int gpio_export(int pin) {
    int fd;
    char buffer[16];
    
    fd = open(GPIO_EXPORT_PATH, O_WRONLY);
    if (fd < 0) {
        printf("Failed to open export for writing: %s\n", strerror(errno));
        return -1;
    }
    
    snprintf(buffer, sizeof(buffer), "%d", pin);
    if (write(fd, buffer, strlen(buffer)) != (ssize_t)strlen(buffer)) {
        printf("Failed to export GPIO pin %d: %s\n", pin, strerror(errno));
        close(fd);
        return -1;
    }
    
    close(fd);
    usleep(100000); // Wait 100ms for export to complete
    return 0;
}

static int gpio_unexport(int pin) {
    int fd;
    char buffer[16];
    
    fd = open(GPIO_UNEXPORT_PATH, O_WRONLY);
    if (fd < 0) {
        printf("Failed to open unexport for writing: %s\n", strerror(errno));
        return -1;
    }
    
    snprintf(buffer, sizeof(buffer), "%d", pin);
    if (write(fd, buffer, strlen(buffer)) != (ssize_t)strlen(buffer)) {
        printf("Failed to unexport GPIO pin %d: %s\n", pin, strerror(errno));
        close(fd);
        return -1;
    }
    
    close(fd);
    return 0;
}

static int gpio_set_direction(int pin, const char *direction) {
    int fd;
    char path[64];
    
    snprintf(path, sizeof(path), "%s/gpio%d/direction", GPIO_BASE_PATH, pin);
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        printf("Failed to open direction for GPIO pin %d: %s\n", pin, strerror(errno));
        return -1;
    }
    
    if (write(fd, direction, strlen(direction)) != (ssize_t)strlen(direction)) {
        printf("Failed to set direction for GPIO pin %d: %s\n", pin, strerror(errno));
        close(fd);
        return -1;
    }
    
    close(fd);
    return 0;
}

static int gpio_set_value(int pin, int value) {
    int fd;
    char path[64];
    char buffer[2];
    
    snprintf(path, sizeof(path), "%s/gpio%d/value", GPIO_BASE_PATH, pin);
    fd = open(path, O_WRONLY);
    if (fd < 0) {
        printf("Failed to open value for GPIO pin %d: %s\n", pin, strerror(errno));
        return -1;
    }
    
    snprintf(buffer, sizeof(buffer), "%d", value);
    if (write(fd, buffer, 1) != 1) {
        printf("Failed to set value for GPIO pin %d: %s\n", pin, strerror(errno));
        close(fd);
        return -1;
    }
    
    close(fd);
    return 0;
}

static int gpio_get_value(int pin) {
    int fd;
    char path[64];
    char buffer[2];
    
    snprintf(path, sizeof(path), "%s/gpio%d/value", GPIO_BASE_PATH, pin);
    fd = open(path, O_RDONLY);
    if (fd < 0) {
        printf("Failed to open value for reading GPIO pin %d: %s\n", pin, strerror(errno));
        return -1;
    }
    
    if (read(fd, buffer, 1) != 1) {
        printf("Failed to read value from GPIO pin %d: %s\n", pin, strerror(errno));
        close(fd);
        return -1;
    }
    
    close(fd);
    return buffer[0] - '0';
}

static int check_gpio_exists(int pin) {
    char path[64];
    struct stat st;
    
    snprintf(path, sizeof(path), "%s/gpio%d", GPIO_BASE_PATH, pin);
    return (stat(path, &st) == 0) ? 1 : 0;
}

// Test functions
static int test_pin_export_unexport(int pin, const char *name) {
    printf("Testing %s (GPIO %d):\n", name, pin);
    
    // Check if already exported
    if (check_gpio_exists(pin)) {
        printf("  - GPIO %d already exported\n", pin);
    } else {
        // Export pin
        if (gpio_export(pin) != 0) {
            printf("  ❌ FAIL: Cannot export GPIO %d\n", pin);
            return -1;
        }
        printf("  ✅ SUCCESS: GPIO %d exported\n", pin);
    }
    
    // Check if GPIO directory exists
    if (!check_gpio_exists(pin)) {
        printf("  ❌ FAIL: GPIO %d directory not found after export\n", pin);
        return -1;
    }
    printf("  ✅ SUCCESS: GPIO %d directory exists\n", pin);
    
    // Test direction setting
    if (gpio_set_direction(pin, "out") != 0) {
        printf("  ❌ FAIL: Cannot set GPIO %d direction to output\n", pin);
        return -1;
    }
    printf("  ✅ SUCCESS: GPIO %d direction set to output\n", pin);
    
    // Test value setting
    if (gpio_set_value(pin, 0) != 0) {
        printf("  ❌ FAIL: Cannot set GPIO %d value to 0\n", pin);
        return -1;
    }
    printf("  ✅ SUCCESS: GPIO %d value set to 0\n", pin);
    
    // Test value reading
    int value = gpio_get_value(pin);
    if (value < 0) {
        printf("  ❌ FAIL: Cannot read GPIO %d value\n", pin);
        return -1;
    }
    printf("  ✅ SUCCESS: GPIO %d value read: %d\n", pin, value);
    
    // Test value toggle
    if (gpio_set_value(pin, 1) != 0) {
        printf("  ❌ FAIL: Cannot set GPIO %d value to 1\n", pin);
        return -1;
    }
    
    value = gpio_get_value(pin);
    if (value != 1) {
        printf("  ❌ FAIL: GPIO %d value not set correctly (expected 1, got %d)\n", pin, value);
        return -1;
    }
    printf("  ✅ SUCCESS: GPIO %d toggle test passed\n", pin);
    
    // Set back to 0
    gpio_set_value(pin, 0);
    
    printf("  ✅ ALL TESTS PASSED for %s (GPIO %d)\n\n", name, pin);
    return 0;
}

static void test_rs485_device(void) {
    printf("Testing RS485 Device Path:\n");
    
    // Check if /dev/ttyOHT485 exists
    struct stat st;
    if (stat("/dev/ttyOHT485", &st) == 0) {
        printf("  ✅ SUCCESS: /dev/ttyOHT485 exists\n");
        
        // Check if it's a character device
        if (S_ISCHR(st.st_mode)) {
            printf("  ✅ SUCCESS: /dev/ttyOHT485 is a character device\n");
        } else {
            printf("  ❌ WARNING: /dev/ttyOHT485 exists but is not a character device\n");
        }
    } else {
        printf("  ❌ FAIL: /dev/ttyOHT485 not found\n");
        printf("  📝 NOTE: Verify udev rules for UART1 → /dev/ttyOHT485 mapping\n");
    }
    
    // Check if /dev/ttyS1 (UART1) exists
    if (stat("/dev/ttyS1", &st) == 0) {
        printf("  ✅ SUCCESS: /dev/ttyS1 (UART1) exists\n");
    } else {
        printf("  ❌ FAIL: /dev/ttyS1 (UART1) not found\n");
    }
    
    printf("\n");
}

static void cleanup_pins(void) {
    printf("Cleaning up GPIO pins...\n");
    gpio_unexport(ESTOP_PIN);
    gpio_unexport(LED_POWER_PIN);
    gpio_unexport(RELAY1_OUTPUT_PIN);
    gpio_unexport(RELAY2_OUTPUT_PIN);
}

int main(int argc, char *argv[]) {
    (void)argc; // Unused parameter
    (void)argv; // Unused parameter
    
    printf("=== OHT-50 Hardware Pin Verification Tool ===\n");
    printf("Firmware Team - Task 2.1\n");
    printf("Testing GPIO pin assignments for Master Module\n\n");
    
    // Set up signal handler
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    int tests_passed = 0;
    int tests_total = 4;
    
    // Test each pin
    if (test_pin_export_unexport(ESTOP_PIN, "E-Stop Input") == 0) {
        tests_passed++;
    }
    
    if (test_pin_export_unexport(LED_POWER_PIN, "Power LED") == 0) {
        tests_passed++;
    }
    
    if (test_pin_export_unexport(RELAY1_OUTPUT_PIN, "Relay 1 Output") == 0) {
        tests_passed++;
    }
    
    if (test_pin_export_unexport(RELAY2_OUTPUT_PIN, "Relay 2 Output") == 0) {
        tests_passed++;
    }
    
    // Test RS485 device
    test_rs485_device();
    
    // Cleanup
    cleanup_pins();
    
    // Summary
    printf("=== Hardware Pin Verification Summary ===\n");
    printf("Tests passed: %d/%d\n", tests_passed, tests_total);
    printf("Success rate: %.1f%%\n", (float)tests_passed / tests_total * 100.0f);
    
    if (tests_passed == tests_total) {
        printf("🎉 All GPIO pins verified successfully!\n");
        printf("📝 Ready for coordination with EMBED team.\n");
        return 0;
    } else {
        printf("❌ Some GPIO pins failed verification.\n");
        printf("📝 Coordinate with EMBED team to fix pin mapping issues.\n");
        return 1;
    }
}
