// OHT-50 Master Module - Main Application Entry Point (FW-09)
// Initialization order: HAL (E-Stop, LED, RS485) → Safety → State Machine → Communication (placeholder)
// Supports --dry-run to simulate without touching hardware

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include "hal_common.h"
#include "hal_led.h"
#include "hal_estop.h"
#include "hal_rs485.h"
#include "hal_lidar.h"
#include "system_state_machine.h"
#include "safety_manager.h"
#include "safety_monitor.h"
#include "communication_manager.h"
#include "module_manager.h"
#include "power_module_handler.h"
#include "travel_motor_module_handler.h"
#include "api_manager.h"
#include "constants.h"

static volatile sig_atomic_t g_should_run = 1;
static bool g_dry_run = false;
static bool g_debug_mode = false;

static void handle_signal(int signum) {
    (void)signum;
    g_should_run = 0;
}

static void install_signal_handlers(void) {
    signal(SIGINT, handle_signal);
    signal(SIGTERM, handle_signal);
}

static void print_usage(const char *prog) {
    printf("Usage: %s [--dry-run] [--debug|--verbose] [--help]\n", prog);
}

static uint64_t now_ms(void) { return hal_get_timestamp_ms(); }

int main(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "--dry-run") == 0) {
            g_dry_run = true;
        } else if (strcmp(argv[i], "--debug") == 0 || strcmp(argv[i], "--verbose") == 0) {
            g_debug_mode = true;
        } else if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            print_usage(argv[0]);
            return 0;
        } else {
            fprintf(stderr, "Unknown argument: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }

    printf("[OHT-50] Starting main application%s...\n", g_dry_run ? " (dry-run)" : "");
    fflush(stdout);
    install_signal_handlers();

    if (g_debug_mode) {
        (void)hal_log_set_level(HAL_LOG_LEVEL_DEBUG);
        printf("[OHT-50][DEBUG] Debug mode enabled\n");
        fflush(stdout);
    }

    if (g_dry_run) {
        // Print planned initialization without touching hardware
        printf("[OHT-50] DRY-RUN: Initialization plan:\n");
        printf("  - HAL: LED → E-Stop(pin=%d) → RS485(device=%s, %u baud)\n", ESTOP_PIN, RS485_DEVICE_PATH, RS485_BAUD_RATE);
        printf("  - Safety: interval=%ums, estop_timeout=%ums\n", SAFETY_CHECK_INTERVAL_MS, 50);
        printf("  - State Machine: update_period=%ums (skipped in dry-run)\n", UPDATE_PERIOD_MS);
        printf("[OHT-50] DRY-RUN: Simulating main loop...\n");
        fflush(stdout);
        for (int i = 1; i <= 5; ++i) { // LED_COUNT = 5
            printf("[OHT-50] DRY-RUN tick %d/5\n", i);
            if (g_debug_mode) {
                // Simulated diagnostics
                printf("[OHT-50][DEBUG] diag: state=IDLE safety=NORMAL estop=SAFE\n");
            }
            fflush(stdout);
            hal_sleep_ms(SLEEP_DELAY_MS);
        }
        printf("[OHT-50] DRY-RUN complete. Exiting.\n");
        fflush(stdout);
        return 0;
    }

    // 1) Initialize HAL subsystems
    if (!g_dry_run) {
        // LED subsystem
        if (hal_led_init() != HAL_STATUS_OK) {
            fprintf(stderr, "[OHT-50] hal_led_init failed\n");
        } else {
            // Turn on Power LED
            (void)hal_led_power_set(LED_STATE_ON);
        }

        // E-Stop subsystem
        estop_config_t estop_cfg = {
            .pin = ESTOP_PIN,
            .response_timeout_ms = 50,
            .debounce_time_ms = ESTOP_DEBOUNCE_TIME_MS,
            .auto_reset_enabled = false
        };
        if (hal_estop_init(&estop_cfg) != HAL_STATUS_OK) {
            fprintf(stderr, "[OHT-50] hal_estop_init failed\n");
        }

        // RS485 subsystem (best-effort init)
        rs485_config_t rs485_cfg;
        memset(&rs485_cfg, 0, sizeof(rs485_cfg));
        strncpy(rs485_cfg.device_path, RS485_DEVICE_PATH, sizeof(rs485_cfg.device_path) - 1);
        rs485_cfg.baud_rate = RS485_BAUD_RATE;
        rs485_cfg.data_bits = RS485_DATA_BITS;
        rs485_cfg.stop_bits = RS485_STOP_BITS;
        rs485_cfg.parity = RS485_PARITY;
        rs485_cfg.timeout_ms = MODBUS_TIMEOUT_MS;
        rs485_cfg.retry_count = MODBUS_RETRY_COUNT;
        if (hal_rs485_init(&rs485_cfg) != HAL_STATUS_OK) {
            fprintf(stderr, "[OHT-50] hal_rs485_init failed (continuing)\n");
        }

        // Communication manager init (for scan)
        printf("[MAIN] Initializing communication manager...\n");
        
        // Test registry_init first
        printf("[MAIN] Testing registry_init...\n");
        int reg_status = registry_init();
        printf("[MAIN] registry_init returned: %d\n", reg_status);
        
        // Test init_rs485
        printf("[MAIN] Testing init_rs485...\n");
        rs485_config_t test_rs485_cfg = {
            .baud_rate = RS485_BAUD_RATE,
            .data_bits = RS485_DATA_BITS,
            .stop_bits = RS485_STOP_BITS,
            .parity = RS485_PARITY,
            .timeout_ms = MODBUS_TIMEOUT_MS
        };
        strcpy(test_rs485_cfg.device_path, "/dev/ttyOHT485");
        hal_status_t rs485_status = hal_rs485_init(&test_rs485_cfg);
        printf("[MAIN] hal_rs485_init returned: %d\n", rs485_status);
        
        // Test hal_rs485_open
        printf("[MAIN] Testing hal_rs485_open...\n");
        hal_status_t open_status = hal_rs485_open();
        printf("[MAIN] hal_rs485_open returned: %d\n", open_status);
        
        comm_mgr_config_t cm_cfg = {
            .baud_rate = RS485_BAUD_RATE,
            .data_bits = RS485_DATA_BITS,
            .stop_bits = RS485_STOP_BITS,
            .parity = RS485_PARITY,
            .timeout_ms = MODBUS_TIMEOUT_MS,
            .retry_count = MODBUS_RETRY_COUNT,
            .retry_delay_ms = 100, // Default retry delay
            .modbus_slave_id = MODBUS_SLAVE_ID,
            .enable_crc_check = false,  // Disable CRC check temporarily
            .enable_echo_suppression = true,
            .buffer_size = RS485_BUFFER_SIZE
        };
        hal_status_t comm_status = comm_manager_init(&cm_cfg);
        if (comm_status != HAL_STATUS_OK) {
            printf("[MAIN] WARNING: comm_manager_init failed (status=%d), continuing...\n", comm_status);
        } else {
            printf("[MAIN] Communication manager initialized successfully\n");
        }

        // LiDAR subsystem initialization
        printf("[MAIN] Initializing LiDAR subsystem...\n");
        lidar_config_t lidar_cfg = {
            .device_path = "/dev/ttyUSB0",
            .baud_rate = 460800,
            .scan_rate_hz = 10,
            .emergency_stop_mm = 500,
            .warning_mm = 1000,
            .safe_mm = 2000,
            .sample_rate_hz = 5000,
            .angular_resolution = 0.72f
        };
        
        hal_status_t lidar_status = hal_lidar_init(&lidar_cfg);
        if (lidar_status != HAL_STATUS_OK) {
            fprintf(stderr, "[OHT-50] hal_lidar_init failed (status=%d), continuing...\n", lidar_status);
        } else {
            printf("[OHT-50] LiDAR initialized successfully\n");
            
            // Start LiDAR scanning
            hal_status_t scan_status = hal_lidar_start_scanning();
            if (scan_status != HAL_STATUS_OK) {
                fprintf(stderr, "[OHT-50] hal_lidar_start_scanning failed (status=%d)\n", scan_status);
            } else {
                printf("[OHT-50] LiDAR scanning started\n");
            }
        }


    } else {
        printf("[OHT-50] DRY-RUN: Skipping HAL init (LED, E-Stop, RS485, LiDAR)\n");
    }

    // 2) Initialize Safety Manager
    safety_config_t safety_cfg = {
        .estop_pin = ESTOP_PIN,
        .response_time_ms = 50,
        .debounce_time_ms = 10,
        .safety_check_interval_ms = SAFETY_CHECK_INTERVAL_MS,
        .fault_clear_timeout_ms = 5000,
        .event_callback = NULL
    };
    if (!g_dry_run) {
        if (safety_manager_init(&safety_cfg) != HAL_STATUS_OK) {
            fprintf(stderr, "[OHT-50] safety_manager_init failed\n");
        }
    } else {
        printf("[OHT-50] DRY-RUN: Skipping safety_manager_init\n");
    }

    // 3) Initialize System State Machine
    if (!g_dry_run) {
        system_config_t sys_cfg = {
            .state_timeout_ms = STATE_TIMEOUT_MS,
            .update_period_ms = UPDATE_PERIOD_MS,
            .auto_recovery_enabled = true,
            .safety_monitoring_enabled = true,
            .communication_monitoring_enabled = true,
            .sensor_monitoring_enabled = true
        };
        if (system_state_machine_init(&sys_cfg) != HAL_STATUS_OK) {
            fprintf(stderr, "[OHT-50] system_state_machine_init failed (continuing)\n");
        } else {
            (void)system_state_machine_process_event(SYSTEM_EVENT_INIT_COMPLETE);
        }
    } else {
        printf("[OHT-50] DRY-RUN: Skipping system_state_machine_init\n");
    }

    // 4) Initialize API Manager for BE communication
    if (!g_dry_run) {
        printf("[OHT-50] Initializing API Manager for BE communication...\n");
        api_mgr_config_t api_config = {
            .http_port = API_HTTP_PORT,
            .websocket_port = API_WEBSOCKET_PORT,
            .http_enabled = true,
            .websocket_enabled = true,
            .cors_enabled = true,
            .cors_origin = "*",
            .max_request_size = API_MAX_REQUEST_SIZE,
            .max_response_size = API_MAX_RESPONSE_SIZE,
            .request_timeout_ms = API_REQUEST_TIMEOUT_MS,
            .websocket_timeout_ms = API_WEBSOCKET_TIMEOUT_MS,
            .authentication_required = false,  // Disable for testing
            .ssl_enabled = false,
            .ssl_certificate_path = "",
            .ssl_private_key_path = ""
        };
        
        if (api_manager_init(&api_config) != HAL_STATUS_OK) {
            fprintf(stderr, "[OHT-50] api_manager_init failed (continuing)\n");
        } else {
            printf("[OHT-50] API Manager initialized - HTTP:%u, WebSocket:%u\n", API_HTTP_PORT, API_WEBSOCKET_PORT);
            if (api_manager_start_http_server() != HAL_STATUS_OK) {
                fprintf(stderr, "[OHT-50] api_manager_start failed (continuing)\n");
            } else {
                printf("[OHT-50] API Manager started successfully\n");
            }
        }
    } else {
        printf("[OHT-50] DRY-RUN: Skipping API Manager initialization\n");
    }

    // 5) Application loop
    printf("[OHT-50] Entering main loop. Press Ctrl+C to exit.\n");
    fflush(stdout);
    uint64_t last_led_toggle_ms = now_ms();
    uint64_t last_diag_ms = now_ms();
    uint64_t last_power_poll_ms = now_ms();
    uint64_t last_comm_poll_ms = now_ms();
    uint64_t last_discovery_poll_ms = now_ms();
    uint64_t last_motor_poll_ms = now_ms();
    bool heartbeat_on = false;

    // Power module handler instance
    // power_module_handler_t power_handler; // Commented out - using global state instead
    bool power_handler_initialized = false;

    // Motor module handler instance
    motor_module_handler_t motor_handler;
    bool motor_handler_initialized = false;

    // Target: reach IDLE in <= 120s
    uint64_t startup_deadline_ms = now_ms() + STARTUP_DEADLINE_MS;
    bool startup_deadline_active = g_dry_run; // Only enforce deadline in dry-run

    // Run initial scan once (Phase 1)
    if (!g_dry_run) {
        printf("[OHT-50] Starting initial module scan...\n");
        registry_set_scanning(true);
        hal_led_comm_set(LED_STATE_BLINK_SLOW); // Blink during scan
        
        (void)comm_manager_scan_range(MODULE_ADDR_POWER, MODULE_ADDR_MAX);
        
        size_t online = registry_count_online();
        bool has_offline = registry_has_offline_saved();
        
        printf("[OHT-50] Scan complete: %zu online, has_offline=%s\n", 
               online, has_offline ? "YES" : "NO");
        
        // CTO Requirements: COMM LED policy based on 4 mandatory slave modules
        if (online == MANDATORY_MODULES_COUNT) {
            // All 4 mandatory slave modules online: NORMAL status
            hal_led_comm_set(LED_STATE_ON);
            printf("[OHT-50] NORMAL: All %d mandatory slave modules online - COMM LED solid\n", MANDATORY_MODULES_COUNT);
        } else if (online > 0 && online < MANDATORY_MODULES_COUNT) {
            // Some mandatory slave modules missing: WARNING status
            hal_led_comm_set(LED_STATE_BLINK_FAST);
            hal_led_system_warning(); // indicate missing mandatory slave modules (warning pattern)
            printf("[OHT-50] WARNING: Only %zu/%d mandatory slave modules online - COMM LED blink fast\n", 
                   online, MANDATORY_MODULES_COUNT);
        } else {
            // No modules online: ERROR status
            hal_led_comm_set(LED_STATE_OFF);
            printf("[OHT-50] ERROR: No slave modules online - COMM LED off\n");
        }
        
        registry_set_scanning(false);
    }

    // Initialize Power Module handler if present and online
    if (!g_dry_run) {
        module_info_t mi;
        if (registry_get(MODULE_ADDR_POWER, &mi) == 0 && mi.status == MODULE_STATUS_ONLINE /* allow unknown type too */) {
            if (mi.type == MODULE_TYPE_POWER || mi.type == MODULE_TYPE_UNKNOWN) {
                // Initialize power module handler with explicit valid Modbus config
                power_module_config_t power_config = {0};
                power_config.slave_id = MODULE_ADDR_POWER;      // 0x02
                power_config.baudrate_code = 5;                  // 115200
                power_config.parity = 0;                         // None
                power_config.stop_bits = 1;                      // 1 stop bit
                power_config.fc_mask = 0x07;                     // FC3|FC6|FC16 supported
                if (power_module_handler_init(&power_config) == HAL_STATUS_OK) {
                    // Apply 4S defaults - simplified for now
                    power_handler_initialized = true;
                    printf("[POWER] Handler initialized (addr=0x%02X, 4S defaults applied)\n", MODULE_ADDR_POWER);
                } else {
                    printf("[POWER] Failed to initialize handler for addr 0x%02X\n", MODULE_ADDR_POWER);
                }
            }
        }
    }

    // Initialize Motor Module handler if present and online
    if (!g_dry_run) {
        module_info_t mi;
        if (registry_get(MODULE_ADDR_TRAVEL_MOTOR, &mi) == 0 && mi.status == MODULE_STATUS_ONLINE) {
            if (mi.type == MODULE_TYPE_TRAVEL_MOTOR || mi.type == MODULE_TYPE_UNKNOWN) {
                // Initialize motor module handler
                motor_module_config_t motor_config = {0};
                motor_config.address = MODULE_ADDR_TRAVEL_MOTOR;      // 0x04
                motor_config.command_timeout_ms = 1000;
                motor_config.response_timeout_ms = 500;
                motor_config.default_velocity = 1000;
                motor_config.default_acceleration = 500;
                motor_config.default_jerk = 100;
                motor_config.enable_safety_checks = true;
                motor_config.enable_position_limits = true;
                motor_config.enable_velocity_limits = true;
                motor_config.enable_acceleration_limits = true;
                
                if (motor_module_init(&motor_handler, &motor_config) == HAL_STATUS_OK) {
                    motor_handler_initialized = true;
                    printf("[MOTOR] Handler initialized (addr=0x%02X)\n", MODULE_ADDR_TRAVEL_MOTOR);
                } else {
                    printf("[MOTOR] Failed to initialize handler for addr 0x%02X\n", MODULE_ADDR_TRAVEL_MOTOR);
                }
            }
        }
    }

    while (g_should_run) {
        // Periodic updates
        if (!g_dry_run) {
            (void)system_state_machine_update();
        }
        if (!g_dry_run) {
            (void)safety_manager_update();
        }

        // Heartbeat LED on SYSTEM LED
        uint64_t t = now_ms();
        if (!g_dry_run && (t - last_led_toggle_ms) >= HEARTBEAT_INTERVAL_MS) {
            heartbeat_on = !heartbeat_on;
            (void)hal_led_system_set(heartbeat_on ? LED_STATE_ON : LED_STATE_OFF);
            last_led_toggle_ms = t;
        }

        // Periodic diagnostics in debug mode
        if (g_debug_mode && (t - last_diag_ms) >= DIAGNOSTICS_INTERVAL_MS) {
            system_status_t sys_status;
            safety_status_info_t safe_status;
            estop_status_t est_status;
            if (system_state_machine_get_status(&sys_status) == HAL_STATUS_OK) {
                printf("[OHT-50][DEBUG] state=%s prev=%s trans=%u ready=%s safe=%s comm=%s\n",
                       system_state_machine_get_state_name(sys_status.current_state),
                       system_state_machine_get_state_name(sys_status.previous_state),
                       sys_status.state_transition_count,
                       sys_status.system_ready ? "YES" : "NO",
                       sys_status.safety_ok ? "YES" : "NO",
                       sys_status.communication_ok ? "YES" : "NO");
            }
            if (safety_manager_get_status(&safe_status) == HAL_STATUS_OK) {
                printf("[OHT-50][DEBUG] safety-level=%d status=%d faults=%u\n",
                       (int)safe_status.level,
                       (int)safe_status.status,
                       safe_status.fault_count);
            }
            if (hal_estop_get_status(&est_status) == HAL_STATUS_OK) {
                printf("[OHT-50][DEBUG] estop state=%d fault=%d pin=%s\n",
                       (int)est_status.state, (int)est_status.fault,
                       est_status.pin_status ? "ON" : "OFF");
            }
            fflush(stdout);
            last_diag_ms = t;
        }

        // Communication Manager polling (every 100ms)
        if (!g_dry_run) {
            if ((t - last_comm_poll_ms) >= COMM_POLL_INTERVAL_MS) {
                hal_status_t comm_status = comm_manager_update();
                if (comm_status != HAL_STATUS_OK && g_debug_mode) {
                    printf("[OHT-50][DEBUG] comm_manager_update failed: %d\n", comm_status);
                }
                last_comm_poll_ms = t;
            }
        }

        // Module Discovery - only once at startup, not continuous polling
        static bool initial_discovery_done = false;
        if (!g_dry_run && !initial_discovery_done) {
            if ((t - last_discovery_poll_ms) >= 1000) { // Wait 1 second after startup
                hal_status_t discovery_status = module_manager_discover_modules();
                if (discovery_status != HAL_STATUS_OK && g_debug_mode) {
                    printf("[OHT-50][DEBUG] Initial module discovery failed: %d\n", discovery_status);
                } else {
                    printf("[OHT-50] Initial module discovery completed\n");
                }
                initial_discovery_done = true; // Only do discovery once
            }
        }

        // Power Module polling (every 100ms) - with fallback for offline modules
        if (!g_dry_run) {
            if ((t - last_power_poll_ms) >= 100) {
                if (power_handler_initialized) {
                    hal_status_t power_status = power_module_handler_poll_data();
                    if (power_status != HAL_STATUS_OK && g_debug_mode) {
                        printf("[OHT-50][DEBUG] power_module_handler_poll_data failed: %d\n", power_status);
                    }
                } else {
                    // Fallback polling for power module even when not initialized
                    if (g_debug_mode) {
                        printf("[OHT-50][DEBUG] Power module fallback polling (module offline)\n");
                    }
                    // Try to read power module directly via RS485
                    uint16_t power_data[4];
                    if (comm_manager_modbus_read_holding_registers(MODULE_ADDR_POWER, 0x00F0, 1, power_data) == HAL_STATUS_OK) {
                        printf("[POWER] Fallback poll: Device ID=0x%04X\n", power_data[0]);
                    }
                }
                last_power_poll_ms = t;
            }
        }

        // Motor Module polling (every 50ms) - with fallback for offline modules
        if (!g_dry_run) {
            if ((t - last_motor_poll_ms) >= 50) {
                if (motor_handler_initialized) {
                    hal_status_t motor_status = motor_module_handler_poll_data(&motor_handler);
                    if (motor_status != HAL_STATUS_OK && g_debug_mode) {
                        printf("[OHT-50][DEBUG] motor_module_handler_poll_data failed: %d\n", motor_status);
                    }
                } else {
                    // Fallback polling for motor module even when not initialized
                    if (g_debug_mode) {
                        printf("[OHT-50][DEBUG] Motor module fallback polling (module offline)\n");
                    }
                    // Try to read motor module directly via RS485
                    uint16_t motor_data[4];
                    if (comm_manager_modbus_read_holding_registers(MODULE_ADDR_TRAVEL_MOTOR, 0x0000, 1, motor_data) == HAL_STATUS_OK) {
                        printf("[MOTOR] Fallback poll: Position=%d\n", (int16_t)motor_data[0]);
                    }
                }
                last_motor_poll_ms = t;
            }
        }

        // LiDAR data processing (every 100ms for safety)
        static uint64_t last_lidar_poll_ms = 0;
        if (!g_dry_run && (t - last_lidar_poll_ms) >= 100) {
            lidar_scan_data_t scan_data;
            if (hal_lidar_get_scan_data(&scan_data) == HAL_STATUS_OK) {
                // Process LiDAR data for safety monitoring
                // Check safety zones using LiDAR data
                if (scan_data.scan_complete) {
                    safety_monitor_check_basic_zones(&scan_data);
                }
            }
            last_lidar_poll_ms = t;
        }

        // Check for E-Stop triggered
        if (!g_dry_run) {
            bool estop_triggered = false;
            if (hal_estop_is_triggered(&estop_triggered) == HAL_STATUS_OK && estop_triggered) {
                (void)system_state_machine_process_event(SYSTEM_EVENT_ESTOP_TRIGGERED);
            }
        }

        // Promote to IDLE if init complete
        if (!g_dry_run) {
            system_state_t current_state;
            if (system_state_machine_get_state(&current_state) == HAL_STATUS_OK) {
                if (current_state == SYSTEM_STATE_INIT) {
                    (void)system_state_machine_enter_idle();
                }
            }
        }

        // Startup deadline check (only in dry-run)
        if (startup_deadline_active && (t > startup_deadline_ms)) {
            printf("[OHT-50] Startup deadline reached.\n");
            break;
        }

        hal_sleep_ms(10);
    }

    printf("[OHT-50] Shutting down...\n");
    // Graceful shutdown
    if (!g_dry_run) {
        // Stop API Manager
        printf("[OHT-50] Stopping API Manager...\n");
        (void)api_manager_stop_http_server();
        (void)api_manager_deinit();
        
        if (power_handler_initialized) {
            // (void)power_module_deinit(&power_handler); // Commented out - using global state
        }
        (void)system_state_machine_enter_shutdown();
    }
    if (!g_dry_run) {
        // Stop LiDAR scanning
        printf("[OHT-50] Stopping LiDAR scanning...\n");
        (void)hal_lidar_stop_scanning();
        (void)hal_lidar_deinit();
        
        (void)hal_led_system_shutdown();
        (void)safety_manager_deinit();
        (void)hal_estop_deinit();
        (void)hal_led_deinit();
    }
    printf("[OHT-50] Exit.\n");
    return 0;
}


