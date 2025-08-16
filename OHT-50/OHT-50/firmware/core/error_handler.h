/**
 * @file error_handler.h
 * @brief Error handling framework với graceful degradation cho OHT-50
 * @version 1.0
 * @date 2024-12-19
 */

#ifndef OHT_ERROR_HANDLER_H
#define OHT_ERROR_HANDLER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Error severity levels
typedef enum {
    ERROR_LEVEL_INFO = 0,     // Informational
    ERROR_LEVEL_WARNING = 1,  // Warning - system can continue
    ERROR_LEVEL_ERROR = 2,    // Error - degraded operation
    ERROR_LEVEL_CRITICAL = 3, // Critical - system shutdown
    ERROR_LEVEL_FATAL = 4     // Fatal - immediate shutdown
} error_level_t;

// Error categories
typedef enum {
    ERROR_CAT_SYSTEM = 0,     // System errors
    ERROR_CAT_HARDWARE = 1,   // Hardware errors
    ERROR_CAT_COMMUNICATION = 2, // Communication errors
    ERROR_CAT_SAFETY = 3,     // Safety errors
    ERROR_CAT_CONTROL = 4,    // Control errors
    ERROR_CAT_SCHEDULER = 5,  // Scheduler errors
    ERROR_CAT_MEMORY = 6,     // Memory errors
    ERROR_CAT_TIMING = 7      // Timing errors
} error_category_t;

// Error codes
typedef enum {
    // System errors (0x0000-0x00FF)
    ERROR_SYS_INIT_FAILED = 0x0001,
    ERROR_SYS_SHUTDOWN = 0x0002,
    ERROR_SYS_CONFIG_INVALID = 0x0003,
    
    // Hardware errors (0x0100-0x01FF)
    ERROR_HW_GPIO_FAILED = 0x0101,
    ERROR_HW_UART_FAILED = 0x0102,
    ERROR_HW_PWM_FAILED = 0x0103,
    ERROR_HW_ADC_FAILED = 0x0104,
    ERROR_HW_TIMER_FAILED = 0x0105,
    
    // Communication errors (0x0200-0x02FF)
    ERROR_COMM_RS485_TIMEOUT = 0x0201,
    ERROR_COMM_RS485_CRC = 0x0202,
    ERROR_COMM_RS485_FRAME = 0x0203,
    ERROR_COMM_WS_DISCONNECTED = 0x0204,
    
    // Safety errors (0x0300-0x03FF)
    ERROR_SAFETY_ESTOP_ACTIVE = 0x0301,
    ERROR_SAFETY_WATCHDOG_TIMEOUT = 0x0302,
    ERROR_SAFETY_LIMIT_EXCEEDED = 0x0303,
    ERROR_SAFETY_INTERLOCK_FAILED = 0x0304,
    
    // Control errors (0x0400-0x04FF)
    ERROR_CTRL_DEADLINE_MISSED = 0x0401,
    ERROR_CTRL_LATENCY_HIGH = 0x0402,
    ERROR_CTRL_STATE_INVALID = 0x0403,
    ERROR_CTRL_SETPOINT_INVALID = 0x0404,
    
    // Scheduler errors (0x0500-0x05FF)
    ERROR_SCHED_TASK_OVERFLOW = 0x0501,
    ERROR_SCHED_PRIORITY_CONFLICT = 0x0502,
    ERROR_SCHED_DEADLOCK = 0x0503,
    
    // Memory errors (0x0600-0x06FF)
    ERROR_MEM_ALLOC_FAILED = 0x0601,
    ERROR_MEM_BUFFER_OVERFLOW = 0x0602,
    ERROR_MEM_STACK_OVERFLOW = 0x0603,
    
    // Timing errors (0x0700-0x07FF)
    ERROR_TIME_DRIFT = 0x0701,
    ERROR_TIME_SYNC_FAILED = 0x0702,
    ERROR_TIME_OVERFLOW = 0x0703
} error_code_t;

// Error context structure
typedef struct {
    error_code_t code;
    error_level_t level;
    error_category_t category;
    uint32_t timestamp;
    uint32_t line;
    const char* file;
    const char* function;
    const char* message;
    void* context;
} error_context_t;

// Error handler function type
typedef void (*error_handler_t)(const error_context_t* error);

// Error recovery action
typedef enum {
    ERROR_ACTION_NONE = 0,
    ERROR_ACTION_RETRY = 1,
    ERROR_ACTION_RESET = 2,
    ERROR_ACTION_DEGRADE = 3,
    ERROR_ACTION_SHUTDOWN = 4,
    ERROR_ACTION_EMERGENCY_STOP = 5
} error_action_t;

// Error statistics
typedef struct {
    uint32_t total_errors;
    uint32_t errors_by_level[5];  // Indexed by error_level_t
    uint32_t errors_by_category[8]; // Indexed by error_category_t
    uint32_t recovery_attempts;
    uint32_t successful_recoveries;
    uint32_t last_error_timestamp;
} error_stats_t;

// Function prototypes
int error_handler_init(void);
void error_handler_deinit(void);

// Error reporting
void error_report(error_code_t code, error_level_t level, 
                 const char* file, uint32_t line, const char* function,
                 const char* message, void* context);

// Error handling
void error_handler_set_handler(error_handler_t handler);
error_action_t error_handler_get_recommended_action(error_code_t code);
int error_handler_execute_recovery(error_code_t code);

// Error statistics
void error_handler_get_stats(error_stats_t* stats);
void error_handler_reset_stats(void);
void error_handler_print_stats(void);

// Error checking utilities
bool error_handler_is_system_healthy(void);
bool error_handler_has_critical_errors(void);
uint32_t error_handler_get_error_count(error_level_t level);

// Macro for easy error reporting
#define ERROR_REPORT(code, level, message) \
    error_report(code, level, __FILE__, __LINE__, __FUNCTION__, message, NULL)

#define ERROR_REPORT_WITH_CONTEXT(code, level, message, context) \
    error_report(code, level, __FILE__, __LINE__, __FUNCTION__, message, context)

#ifdef __cplusplus
}
#endif

#endif // OHT_ERROR_HANDLER_H
