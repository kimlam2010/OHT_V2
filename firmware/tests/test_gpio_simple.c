#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(void) {
    printf("=== Simple GPIO Test ===\n");
    
    // Test GPIO export
    printf("Testing GPIO export...\n");
    
    // Try to export GPIO1_A0 (Pin 32)
    FILE *fp = fopen("/sys/class/gpio/export", "w");
    if (fp) {
        fprintf(fp, "32");
        fclose(fp);
        printf("✅ GPIO 32 export successful\n");
    } else {
        printf("❌ GPIO 32 export failed\n");
        return 1;
    }
    
    // Check if GPIO directory exists
    if (access("/sys/class/gpio/gpio32", F_OK) == 0) {
        printf("✅ GPIO 32 directory exists\n");
    } else {
        printf("❌ GPIO 32 directory not found\n");
        return 1;
    }
    
    // Set direction
    fp = fopen("/sys/class/gpio/gpio32/direction", "w");
    if (fp) {
        fprintf(fp, "out");
        fclose(fp);
        printf("✅ GPIO 32 direction set to output\n");
    } else {
        printf("❌ GPIO 32 direction set failed\n");
        return 1;
    }
    
    // Test GPIO value
    fp = fopen("/sys/class/gpio/gpio32/value", "w");
    if (fp) {
        fprintf(fp, "1");
        fclose(fp);
        printf("✅ GPIO 32 set to HIGH\n");
        
        sleep(1);
        
        fp = fopen("/sys/class/gpio/gpio32/value", "w");
        if (fp) {
            fprintf(fp, "0");
            fclose(fp);
            printf("✅ GPIO 32 set to LOW\n");
        }
    } else {
        printf("❌ GPIO 32 value set failed\n");
        return 1;
    }
    
    // Unexport
    fp = fopen("/sys/class/gpio/unexport", "w");
    if (fp) {
        fprintf(fp, "32");
        fclose(fp);
        printf("✅ GPIO 32 unexport successful\n");
    }
    
    printf("✅ Simple GPIO test completed successfully!\n");
    return 0;
}
