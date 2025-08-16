/**
 * @file test_scheduler.c
 * @brief Unit tests cho scheduler
 * @version 1.0
 * @date 2024-12-19
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "../scheduler.h"

// Test task counters
static uint32_t test_task1_count = 0;
static uint32_t test_task2_count = 0;
static uint32_t test_task3_count = 0;

// Test task functions
static void test_task1(void* arg) {
    test_task1_count++;
    usleep(1000); // 1ms work
}

static void test_task2(void* arg) {
    test_task2_count++;
    usleep(500); // 0.5ms work
}

static void test_task3(void* arg) {
    test_task3_count++;
    usleep(2000); // 2ms work
}

// Test functions
static void test_scheduler_init(void) {
    printf("Testing scheduler initialization...\n");
    
    scheduler_context_t ctx;
    int result = scheduler_init(&ctx, 5);
    
    assert(result == 0);
    assert(ctx.max_tasks == 5);
    assert(ctx.task_count == 0);
    assert(ctx.running == false);
    
    scheduler_deinit(&ctx);
    printf("✓ Scheduler initialization test passed\n");
}

static void test_scheduler_add_task(void) {
    printf("Testing task addition...\n");
    
    scheduler_context_t ctx;
    scheduler_init(&ctx, 5);
    
    // Add tasks
    int task1_id = scheduler_add_task(&ctx, "Task1", test_task1, NULL, 
                                    PRIORITY_HIGH, 100, 200);
    int task2_id = scheduler_add_task(&ctx, "Task2", test_task2, NULL, 
                                    PRIORITY_MEDIUM, 200, 400);
    
    assert(task1_id == 0);
    assert(task2_id == 1);
    assert(ctx.task_count == 2);
    assert(strcmp(ctx.tasks[0].name, "Task1") == 0);
    assert(strcmp(ctx.tasks[1].name, "Task2") == 0);
    assert(ctx.tasks[0].priority == PRIORITY_HIGH);
    assert(ctx.tasks[1].priority == PRIORITY_MEDIUM);
    
    scheduler_deinit(&ctx);
    printf("✓ Task addition test passed\n");
}

static void test_scheduler_priority(void) {
    printf("Testing priority scheduling...\n");
    
    scheduler_context_t ctx;
    scheduler_init(&ctx, 5);
    
    // Add tasks with different priorities
    scheduler_add_task(&ctx, "Low", test_task1, NULL, PRIORITY_LOW, 100, 200);
    scheduler_add_task(&ctx, "High", test_task2, NULL, PRIORITY_HIGH, 100, 200);
    scheduler_add_task(&ctx, "Medium", test_task3, NULL, PRIORITY_MEDIUM, 100, 200);
    
    // Start scheduler
    scheduler_start(&ctx);
    
    // Let it run for a short time
    usleep(100000); // 100ms
    
    // Stop scheduler
    scheduler_stop(&ctx);
    
    // Check that high priority task ran more
    assert(test_task2_count > test_task1_count);
    assert(test_task2_count > test_task3_count);
    
    scheduler_deinit(&ctx);
    printf("✓ Priority scheduling test passed\n");
}

static void test_scheduler_timing(void) {
    printf("Testing timing accuracy...\n");
    
    scheduler_context_t ctx;
    scheduler_init(&ctx, 5);
    
    // Add a periodic task
    scheduler_add_task(&ctx, "Timing", test_task1, NULL, PRIORITY_HIGH, 10, 20);
    
    // Start scheduler
    scheduler_start(&ctx);
    
    // Let it run for 100ms (should be ~10 executions)
    usleep(100000);
    
    // Stop scheduler
    scheduler_stop(&ctx);
    
    // Check execution count (allow some tolerance)
    assert(test_task1_count >= 8 && test_task1_count <= 12);
    
    scheduler_deinit(&ctx);
    printf("✓ Timing accuracy test passed\n");
}

static void test_scheduler_deadline(void) {
    printf("Testing deadline monitoring...\n");
    
    scheduler_context_t ctx;
    scheduler_init(&ctx, 5);
    
    // Add a task that might miss deadline
    scheduler_add_task(&ctx, "Deadline", test_task3, NULL, PRIORITY_HIGH, 5, 5);
    
    // Start scheduler
    scheduler_start(&ctx);
    
    // Let it run for a short time
    usleep(50000); // 50ms
    
    // Stop scheduler
    scheduler_stop(&ctx);
    
    // Check that task was executed
    assert(test_task3_count > 0);
    
    scheduler_deinit(&ctx);
    printf("✓ Deadline monitoring test passed\n");
}

static void test_scheduler_remove_task(void) {
    printf("Testing task removal...\n");
    
    scheduler_context_t ctx;
    scheduler_init(&ctx, 5);
    
    // Add tasks
    int task1_id = scheduler_add_task(&ctx, "Task1", test_task1, NULL, 
                                    PRIORITY_HIGH, 100, 200);
    int task2_id = scheduler_add_task(&ctx, "Task2", test_task2, NULL, 
                                    PRIORITY_MEDIUM, 200, 400);
    
    assert(ctx.task_count == 2);
    
    // Remove first task
    int result = scheduler_remove_task(&ctx, task1_id);
    assert(result == 0);
    assert(ctx.task_count == 1);
    assert(strcmp(ctx.tasks[0].name, "Task2") == 0);
    
    scheduler_deinit(&ctx);
    printf("✓ Task removal test passed\n");
}

static void test_scheduler_time_functions(void) {
    printf("Testing time functions...\n");
    
    uint64_t time1_ms = scheduler_get_time_ms();
    uint64_t time1_us = scheduler_get_time_us();
    
    usleep(1000); // 1ms
    
    uint64_t time2_ms = scheduler_get_time_ms();
    uint64_t time2_us = scheduler_get_time_us();
    
    // Check that time increased
    assert(time2_ms > time1_ms);
    assert(time2_us > time1_us);
    
    // Check microsecond precision
    assert((time2_us - time1_us) >= 1000);
    
    printf("✓ Time functions test passed\n");
}

static void test_scheduler_stats(void) {
    printf("Testing statistics collection...\n");
    
    scheduler_context_t ctx;
    scheduler_init(&ctx, 5);
    
    // Add a task
    int task_id = scheduler_add_task(&ctx, "Stats", test_task1, NULL, 
                                   PRIORITY_HIGH, 10, 20);
    
    // Start scheduler
    scheduler_start(&ctx);
    
    // Let it run for a short time
    usleep(50000); // 50ms
    
    // Stop scheduler
    scheduler_stop(&ctx);
    
    // Get statistics
    uint32_t exec_count, missed_deadlines;
    scheduler_get_stats(&ctx, task_id, &exec_count, &missed_deadlines);
    
    assert(exec_count > 0);
    
    scheduler_deinit(&ctx);
    printf("✓ Statistics collection test passed\n");
}

// Main test function
int main(void) {
    printf("=== OHT-50 Scheduler Unit Tests ===\n");
    printf("Version: 1.0\n");
    printf("===================================\n\n");
    
    // Reset counters
    test_task1_count = 0;
    test_task2_count = 0;
    test_task3_count = 0;
    
    // Run tests
    test_scheduler_init();
    test_scheduler_add_task();
    test_scheduler_priority();
    test_scheduler_timing();
    test_scheduler_deadline();
    test_scheduler_remove_task();
    test_scheduler_time_functions();
    test_scheduler_stats();
    
    printf("\n=== All Tests Passed ===\n");
    printf("Total tests: 8\n");
    printf("Passed: 8\n");
    printf("Failed: 0\n");
    
    return 0;
}
