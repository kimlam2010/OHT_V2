/**
 * @file main.c
 * @brief Main application cho OHT-50 firmware framework
 * @version 1.0
 * @date 2024-12-19
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "scheduler.h"
#include "control_loop.h"
#include "error_handler.h"

// Global variables
static scheduler_context_t g_scheduler;
static control_loop_context_t g_control_loop;
static bool g_running = false;

// Function prototypes
static void signal_handler(int sig);
static void demo_control_function(void* arg);
static void demo_safety_function(void* arg);
static void demo_telemetry_function(void* arg);
static void demo_background_task(void* arg);
static void demo_communication_task(void* arg);
static void print_system_status(void);

/**
 * Main function
 */
int main(int argc, char* argv[]) {
    printf("=== OHT-50 Firmware Framework Demo ===\n");
    printf("Version: 1.0\n");
    printf("Platform: Orange Pi 5B (RK3588)\n");
    printf("=====================================\n\n");

    // Setup signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Initialize error handler
    if (error_handler_init() != 0) {
        printf("ERROR: Failed to initialize error handler\n");
        return -1;
    }
    printf("✓ Error handler initialized\n");

    // Initialize scheduler
    if (scheduler_init(&g_scheduler, 10) != 0) {
        ERROR_REPORT(ERROR_SYS_INIT_FAILED, ERROR_LEVEL_CRITICAL, "Failed to initialize scheduler");
        return -1;
    }
    printf("✓ Scheduler initialized\n");

    // Initialize control loop
    if (control_loop_init(&g_control_loop) != 0) {
        ERROR_REPORT(ERROR_SYS_INIT_FAILED, ERROR_LEVEL_CRITICAL, "Failed to initialize control loop");
        return -1;
    }
    printf("✓ Control loop initialized\n");

    // Setup control loop functions
    control_loop_set_control_function(&g_control_loop, demo_control_function, NULL);
    control_loop_set_safety_function(&g_control_loop, demo_safety_function, NULL);
    control_loop_set_telemetry_function(&g_control_loop, demo_telemetry_function, NULL);

    // Add demo tasks to scheduler
    scheduler_add_task(&g_scheduler, "Background", demo_background_task, NULL, 
                      PRIORITY_BACKGROUND, 5000, 10000); // Every 5 seconds
    
    scheduler_add_task(&g_scheduler, "Communication", demo_communication_task, NULL, 
                      PRIORITY_MEDIUM, 100, 200); // Every 100ms

    // Start scheduler
    if (scheduler_start(&g_scheduler) != 0) {
        ERROR_REPORT(ERROR_SYS_INIT_FAILED, ERROR_LEVEL_CRITICAL, "Failed to start scheduler");
        return -1;
    }
    printf("✓ Scheduler started\n");

    // Start control loop
    if (control_loop_start(&g_scheduler, &g_control_loop) != 0) {
        ERROR_REPORT(ERROR_SYS_INIT_FAILED, ERROR_LEVEL_CRITICAL, "Failed to start control loop");
        return -1;
    }
    printf("✓ Control loop started\n");

    g_running = true;
    printf("\n=== System Running ===\n");
    printf("Press Ctrl+C to stop\n\n");

    // Main loop - print status every 10 seconds
    uint32_t status_counter = 0;
    while (g_running) {
        sleep(10);
        status_counter++;
        
        if (status_counter % 6 == 0) { // Every minute
            print_system_status();
        }
        
        // Check for critical errors
        if (error_handler_has_critical_errors()) {
            printf("CRITICAL: System has critical errors, initiating shutdown\n");
            break;
        }
    }

    // Cleanup
    printf("\n=== Shutting Down ===\n");
    
    control_loop_stop(&g_control_loop);
    printf("✓ Control loop stopped\n");
    
    scheduler_stop(&g_scheduler);
    printf("✓ Scheduler stopped\n");
    
    control_loop_deinit(&g_control_loop);
    scheduler_deinit(&g_scheduler);
    error_handler_deinit();
    
    printf("✓ System shutdown complete\n");
    return 0;
}

/**
 * Signal handler for graceful shutdown
 */
static void signal_handler(int sig) {
    printf("\nReceived signal %d, shutting down gracefully...\n", sig);
    g_running = false;
}

/**
 * Demo control function - called every 1ms
 */
static void demo_control_function(void* arg) {
    static uint32_t counter = 0;
    counter++;
    
    // Simulate some control work
    usleep(100); // 100us of work
    
    // Occasionally report a control error for testing
    if (counter % 1000 == 0) {
        ERROR_REPORT(ERROR_CTRL_DEADLINE_MISSED, ERROR_LEVEL_WARNING, "Demo control deadline missed");
    }
}

/**
 * Demo safety function - called every 1ms
 */
static void demo_safety_function(void* arg) {
    static uint32_t counter = 0;
    counter++;
    
    // Simulate safety checks
    usleep(50); // 50us of safety work
    
    // Occasionally report a safety warning for testing
    if (counter % 5000 == 0) {
        ERROR_REPORT(ERROR_SAFETY_LIMIT_EXCEEDED, ERROR_LEVEL_WARNING, "Demo safety limit exceeded");
    }
}

/**
 * Demo telemetry function - called every 1ms
 */
static void demo_telemetry_function(void* arg) {
    static uint32_t counter = 0;
    counter++;
    
    // Simulate telemetry collection
    usleep(25); // 25us of telemetry work
    
    // Occasionally report a communication error for testing
    if (counter % 3000 == 0) {
        ERROR_REPORT(ERROR_COMM_RS485_TIMEOUT, ERROR_LEVEL_WARNING, "Demo RS485 timeout");
    }
}

/**
 * Demo background task - called every 5 seconds
 */
static void demo_background_task(void* arg) {
    static uint32_t counter = 0;
    counter++;
    
    printf("Background task %u: System maintenance check\n", counter);
    
    // Simulate background maintenance work
    usleep(100000); // 100ms of work
    
    // Occasionally report a system info message
    if (counter % 12 == 0) { // Every minute
        ERROR_REPORT(ERROR_SYS_CONFIG_INVALID, ERROR_LEVEL_INFO, "Demo system info message");
    }
}

/**
 * Demo communication task - called every 100ms
 */
static void demo_communication_task(void* arg) {
    static uint32_t counter = 0;
    counter++;
    
    // Simulate communication work
    usleep(1000); // 1ms of work
    
    // Occasionally report a communication warning
    if (counter % 50 == 0) { // Every 5 seconds
        ERROR_REPORT(ERROR_COMM_WS_DISCONNECTED, ERROR_LEVEL_WARNING, "Demo WebSocket disconnected");
    }
}

/**
 * Print system status
 */
static void print_system_status(void) {
    printf("\n=== System Status ===\n");
    
    // Print scheduler statistics
    printf("Scheduler:\n");
    printf("  Tasks: %u\n", g_scheduler.task_count);
    printf("  Ticks: %u\n", g_scheduler.tick_count);
    
    // Print control loop statistics
    control_loop_stats_t ctrl_stats;
    control_loop_get_stats(&g_control_loop, &ctrl_stats);
    printf("Control Loop:\n");
    printf("  Total cycles: %u\n", ctrl_stats.total_cycles);
    printf("  Missed deadlines: %u\n", ctrl_stats.missed_deadlines);
    printf("  Max latency: %u us\n", ctrl_stats.max_latency_us);
    printf("  Avg latency: %u us\n", ctrl_stats.avg_latency_us);
    printf("  Max jitter: %u us\n", ctrl_stats.max_jitter_us);
    
    // Print error statistics
    error_stats_t error_stats;
    error_handler_get_stats(&error_stats);
    printf("Errors:\n");
    printf("  Total: %u\n", error_stats.total_errors);
    printf("  Critical: %u\n", error_stats.errors_by_level[ERROR_LEVEL_CRITICAL]);
    printf("  Fatal: %u\n", error_stats.errors_by_level[ERROR_LEVEL_FATAL]);
    printf("  Recovery attempts: %u\n", error_stats.recovery_attempts);
    printf("  Successful recoveries: %u\n", error_stats.successful_recoveries);
    
    // Print system health
    printf("System Health: %s\n", error_handler_is_system_healthy() ? "HEALTHY" : "DEGRADED");
    printf("===================\n\n");
}
