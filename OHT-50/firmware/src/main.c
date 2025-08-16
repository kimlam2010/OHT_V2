/**
 * OHT-50 Firmware - Main Entry Point
 * 
 * @file main.c
 * @version 1.0.0
 * @date 2025-01-27
 * @author FW Team
 */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <errno.h>

// HAL includes
#include "hal.h"
#include "scheduler.h"
#include "state_machine.h"
#include "rs485_protocol.h"

// Global variables
static volatile int g_running = 1;
static pthread_t g_scheduler_thread;
static pthread_t g_control_thread;
static pthread_t g_comms_thread;

// Signal handler for graceful shutdown
static void signal_handler(int sig) {
    printf("[MAIN] Received signal %d, shutting down...\n", sig);
    g_running = 0;
}

// Scheduler thread function
static void* scheduler_thread(void* arg) {
    printf("[SCHEDULER] Thread started\n");
    
    while (g_running) {
        // Execute scheduler tick
        scheduler_tick();
        
        // Sleep for 1ms (1000Hz)
        usleep(1000);
    }
    
    printf("[SCHEDULER] Thread stopped\n");
    return NULL;
}

// Control thread function
static void* control_thread(void* arg) {
    printf("[CONTROL] Thread started\n");
    
    while (g_running) {
        // Execute control loop
        control_loop_tick();
        
        // Sleep for 10ms (100Hz)
        usleep(10000);
    }
    
    printf("[CONTROL] Thread stopped\n");
    return NULL;
}

// Communications thread function
static void* comms_thread(void* arg) {
    printf("[COMMS] Thread started\n");
    
    while (g_running) {
        // Execute RS485 communication
        rs485_tick();
        
        // Sleep for 20ms (50Hz)
        usleep(20000);
    }
    
    printf("[COMMS] Thread stopped\n");
    return NULL;
}

// Initialize system
static int system_init(void) {
    printf("[MAIN] Initializing OHT-50 Firmware v1.0.0\n");
    
    // Initialize HAL
    if (hal_init() != 0) {
        printf("[MAIN] ERROR: HAL initialization failed\n");
        return -1;
    }
    printf("[MAIN] HAL initialized successfully\n");
    
    // Initialize scheduler
    if (scheduler_init() != 0) {
        printf("[MAIN] ERROR: Scheduler initialization failed\n");
        return -1;
    }
    printf("[MAIN] Scheduler initialized successfully\n");
    
    // Initialize state machine
    if (state_machine_init() != 0) {
        printf("[MAIN] ERROR: State machine initialization failed\n");
        return -1;
    }
    printf("[MAIN] State machine initialized successfully\n");
    
    // Initialize RS485 protocol
    if (rs485_init() != 0) {
        printf("[MAIN] ERROR: RS485 protocol initialization failed\n");
        return -1;
    }
    printf("[MAIN] RS485 protocol initialized successfully\n");
    
    return 0;
}

// Cleanup system
static void system_cleanup(void) {
    printf("[MAIN] Cleaning up system...\n");
    
    // Stop threads
    g_running = 0;
    
    // Wait for threads to finish
    pthread_join(g_scheduler_thread, NULL);
    pthread_join(g_control_thread, NULL);
    pthread_join(g_comms_thread, NULL);
    
    // Cleanup modules
    rs485_cleanup();
    state_machine_cleanup();
    scheduler_cleanup();
    hal_cleanup();
    
    printf("[MAIN] System cleanup completed\n");
}

// Main function
int main(int argc, char* argv[]) {
    printf("[MAIN] OHT-50 Firmware starting...\n");
    
    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    // Initialize system
    if (system_init() != 0) {
        printf("[MAIN] ERROR: System initialization failed\n");
        return -1;
    }
    
    printf("[MAIN] System initialized successfully\n");
    
    // Create threads
    if (pthread_create(&g_scheduler_thread, NULL, scheduler_thread, NULL) != 0) {
        printf("[MAIN] ERROR: Failed to create scheduler thread\n");
        return -1;
    }
    
    if (pthread_create(&g_control_thread, NULL, control_thread, NULL) != 0) {
        printf("[MAIN] ERROR: Failed to create control thread\n");
        return -1;
    }
    
    if (pthread_create(&g_comms_thread, NULL, comms_thread, NULL) != 0) {
        printf("[MAIN] ERROR: Failed to create comms thread\n");
        return -1;
    }
    
    printf("[MAIN] All threads created successfully\n");
    printf("[MAIN] Firmware running... (Press Ctrl+C to stop)\n");
    
    // Main loop - just wait for shutdown signal
    while (g_running) {
        sleep(1);
    }
    
    // Cleanup
    system_cleanup();
    
    printf("[MAIN] Firmware shutdown completed\n");
    return 0;
}
