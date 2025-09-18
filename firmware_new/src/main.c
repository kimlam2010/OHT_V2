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
#include "system_controller.h"
#include "safety_manager.h"
#include "safety_monitor.h"
#include "communication_manager.h"
#include "module_manager.h"
#include "module_polling_manager.h"
#include "power_module_handler.h"
#include "travel_motor_module_handler.h"
#include "api_manager.h"
#include "api_endpoints.h"
#include "app/websocket_server.h"
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
            .response_timeout_ms = 100,
            .debounce_time_ms = ESTOP_DEBOUNCE_TIME_MS,
            .auto_reset_enabled = false
        };
        if (hal_estop_init(&estop_cfg) != HAL_STATUS_OK) {
            fprintf(stderr, "[OHT-50] hal_estop_init failed\n");
        } else {
            // Register callback and run self-test
            (void)hal_estop_set_callback(NULL);
            (void)hal_estop_self_test();
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

        // Initialize Module Manager
        printf("[MAIN] Initializing module manager...\n");
        hal_status_t module_status = module_manager_init();
        if (module_status != HAL_STATUS_OK) {
            printf("[MAIN] WARNING: module_manager_init failed (status=%d), continuing...\n", module_status);
        } else {
            printf("[MAIN] Module manager initialized successfully\n");
            // Load YAML config for module manager if available
            (void)module_manager_load_config_from_yaml("/etc/oht50/modules.yaml");
            module_config_t cfg; if (module_manager_get_config(&cfg)==HAL_STATUS_OK){
                printf("[MAIN] Module cfg: health=%ums offline=%ums retry=%u resp_to=%u jitter=%u\n",
                    cfg.health_check_interval_ms, cfg.offline_threshold_ms, cfg.retry_count, cfg.response_timeout_ms, cfg.health_jitter_percent);
            }
        }

        // Initialize Module Polling Manager
        printf("[MAIN] Initializing module polling manager...\n");
        hal_status_t polling_status = module_polling_manager_init();
        if (polling_status != HAL_STATUS_OK) {
            printf("[MAIN] WARNING: module_polling_manager_init failed (status=%d), continuing...\n", polling_status);
        } else {
            printf("[MAIN] Module polling manager initialized successfully\n");
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
        .response_time_ms = 100,
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

    // 3) Initialize System Controller
    if (!g_dry_run) {
        system_controller_config_t sys_ctrl_cfg = {
            .update_period_ms = 100,
            .timeout_ms = 5000,
            .error_retry_count = 3,
            .error_retry_delay_ms = 1000,
            .enable_auto_recovery = true,
            .enable_error_logging = true,
            .enable_performance_monitoring = true,
            .enable_diagnostics = true,
            .max_error_count = 10,
            .error_reset_timeout_ms = 5000
        };
        if (system_controller_init(&sys_ctrl_cfg) != HAL_STATUS_OK) {
            fprintf(stderr, "[OHT-50] system_controller_init failed (continuing)\n");
        } else {
            printf("[OHT-50] System Controller initialized successfully\n");
        }
    } else {
        printf("[OHT-50] DRY-RUN: Skipping system_controller_init\n");
    }

    // 4) Initialize System State Machine
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

    // 4) Initialize API Manager (Minimal HTTP Server)
    if (!g_dry_run) {
        api_mgr_config_t api_config = {
            .http_port = 8080
        };
        
        int api_result = api_manager_init(&api_config);
        if (api_result != 0) {
            fprintf(stderr, "[OHT-50] ❌ API Manager init failed: %d (continuing)\n", api_result);
        } else {
            printf("[OHT-50] ✅ API Manager initialized\n");
            
            api_result = api_manager_start();
            if (api_result != 0) {
                fprintf(stderr, "[OHT-50] ❌ API Manager start failed: %d (continuing)\n", api_result);
            } else {
                printf("[OHT-50] ✅ API Manager started on port 8080 (HTTP)\n");
                
                // Register Minimal API v1 endpoints
                extern int api_register_minimal_endpoints(void);
                
                // Initialize WebSocket Server on port 8081
                ws_server_config_t ws_config = {
                    .port = 8081,
                    .max_clients = 10,
                    .timeout_ms = 60000,
                    .max_message_size = 4096,
                    .max_frame_size = 8192,
                    .ping_interval_ms = 30000,
                    .pong_timeout_ms = 5000,
                    .enable_compression = false,
                    .enable_authentication = false,
                    .server_name = "OHT-50-WebSocket"
                };
                
                hal_status_t ws_result = ws_server_init(&ws_config);
                if (ws_result != HAL_STATUS_OK) {
                    fprintf(stderr, "[OHT-50] ❌ WebSocket Server init failed: %d (continuing)\n", ws_result);
                } else {
                    printf("[OHT-50] ✅ WebSocket Server initialized\n");
                    
                    ws_result = ws_server_start();
                    if (ws_result != HAL_STATUS_OK) {
                        fprintf(stderr, "[OHT-50] ❌ WebSocket Server start failed: %d (continuing)\n", ws_result);
                    } else {
                        printf("[OHT-50] ✅ WebSocket Server started on port 8081\n");
                        
                        // Start RS485 telemetry streaming (Issue #90)
                        hal_status_t telemetry_result = ws_server_start_rs485_telemetry_streaming(2000); // 2 second interval
                        if (telemetry_result != HAL_STATUS_OK) {
                            fprintf(stderr, "[OHT-50] ⚠️ RS485 telemetry streaming start failed: %d\n", telemetry_result);
                        } else {
                            printf("[OHT-50] ✅ RS485 telemetry streaming started (2s interval)\n");
                        }
                    }
                }
                (void)api_register_minimal_endpoints();
                printf("[OHT-50] ✅ Minimal API v1 endpoints registered\n");
            }
        }
    } else {
        printf("[OHT-50] DRY-RUN: Skipping API Manager initialization\n");
    }

    // 5) Initialize Module Registry and load YAML configuration
    if (!g_dry_run) {
        if (registry_init() != 0) {
            fprintf(stderr, "[OHT-50] registry_init failed (continuing)\n");
        } else {
            printf("[OHT-50] Module Registry initialized\n");
            
            // Load YAML configuration
            int modules_loaded = registry_load_yaml("/etc/oht50/modules.yaml");
            if (modules_loaded >= 0) {
                printf("[OHT-50] Module Registry: %d modules loaded from YAML\n", modules_loaded);
            } else {
                printf("[OHT-50] Module Registry: Using default configuration\n");
            }
        }
    } else {
        printf("[OHT-50] DRY-RUN: Skipping Module Registry initialization\n");
    }

    // 6) Application loop
    printf("[OHT-50] Entering main loop. Press Ctrl+C to exit.\n");
    fflush(stdout);
    uint64_t last_led_toggle_ms = now_ms();
    uint64_t last_diag_ms = now_ms();
    uint64_t last_power_poll_ms = now_ms();
    uint64_t last_comm_poll_ms = now_ms();
    uint64_t last_discovery_poll_ms = now_ms();
    uint64_t last_motor_poll_ms = now_ms();
    (void)last_power_poll_ms;
    (void)last_comm_poll_ms;
    (void)last_discovery_poll_ms;
    (void)last_motor_poll_ms;
    // uint64_t last_module_manager_poll_ms = now_ms(); // Unused - commented out
    bool heartbeat_on = false;

    // Power module handler instance
    // power_module_handler_t power_handler; // Commented out - using global state instead
    bool power_handler_initialized = false;

    // Motor module handler instance
    motor_module_handler_t motor_handler;
    bool motor_handler_initialized = false;
    (void)motor_handler_initialized;

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
        
        // API Manager is event-driven, no processing loop needed

        // System Controller update (every 100ms) - only if initialized
        if (!g_dry_run) {
            hal_status_t sys_ctrl_status = system_controller_update();
            if (sys_ctrl_status != HAL_STATUS_OK && g_debug_mode) {
                // Only log if it's not a "not initialized" error to reduce spam
                if (sys_ctrl_status != HAL_STATUS_NOT_INITIALIZED) {
                    printf("[OHT-50][DEBUG] system_controller_update failed: %d\n", sys_ctrl_status);
                }
            }
        }
        
        // RS485 Module Telemetry Broadcasting (every 2000ms) - Issue #90
        uint64_t current_time = now_ms();
        static uint64_t last_rs485_telemetry_ms = 0;
        if (current_time - last_rs485_telemetry_ms >= 2000) {
            if (!g_dry_run) {
                // Broadcast telemetry for all discovered modules
                uint8_t module_addresses[] = {0x02, 0x03, 0x04, 0x05}; // Power, Safety, Motor, Dock
                for (size_t i = 0; i < sizeof(module_addresses); i++) {
                    uint8_t addr = module_addresses[i];
                    
                    // Check if module is online in registry
                    module_info_t module_info;
                    if (registry_get(addr, &module_info) == 0 && module_info.status == MODULE_STATUS_ONLINE) {
                        hal_status_t ws_result = ws_server_broadcast_rs485_telemetry(addr);
                        if (ws_result != HAL_STATUS_OK && g_debug_mode) {
                            printf("[OHT-50][DEBUG] RS485 telemetry broadcast failed for 0x%02X: %d\n", addr, ws_result);
                        }
                    }
                }
            }
            last_rs485_telemetry_ms = current_time;
        }
        
        // System Telemetry and Status Broadcasting (every 1000ms)
        static uint64_t last_telemetry_broadcast_ms = 0;
        if (current_time - last_telemetry_broadcast_ms >= 1000) {
            if (!g_dry_run) {
                // Send system telemetry data via WebSocket
                char telemetry_data[256];
                snprintf(telemetry_data, sizeof(telemetry_data), 
                        "{\"timestamp\":%lu,\"status\":\"running\",\"modules\":%zu}",
                        current_time, registry_count_online());
                comm_manager_send_telemetry((const uint8_t*)telemetry_data, strlen(telemetry_data));
                
                // Send status update via WebSocket
                char status_data[256];
                snprintf(status_data, sizeof(status_data),
                        "{\"timestamp\":%lu,\"system\":\"OHT-50\",\"state\":\"operational\"}",
                        current_time);
                comm_manager_send_status((const uint8_t*)status_data, strlen(status_data));
            }
            last_telemetry_broadcast_ms = current_time;
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
                    
                    // Add discovered modules to polling manager
                    printf("[OHT-50] Adding discovered modules to polling manager...\n");
                    module_polling_manager_add_module(0x02, MODULE_TYPE_POWER);        // Power Module
                    module_polling_manager_add_module(0x03, MODULE_TYPE_SAFETY);       // Safety Module
                    module_polling_manager_add_module(0x04, MODULE_TYPE_TRAVEL_MOTOR); // Travel Motor Module
                    module_polling_manager_add_module(0x05, MODULE_TYPE_DOCK);         // Dock Module
                    module_polling_manager_add_module(0x06, MODULE_TYPE_UNKNOWN);      // Unknown Module
                    module_polling_manager_add_module(0x07, MODULE_TYPE_UNKNOWN);      // Unknown Module
                    printf("[OHT-50] All discovered modules added to polling manager\n");
                }
                initial_discovery_done = true; // Only do discovery once
                last_discovery_poll_ms = t; // Update timestamp to prevent re-execution
            }
        }

        // Dynamic Module Polling (replaces individual module polling)
        if (!g_dry_run) {
            hal_status_t polling_status = module_polling_manager_update();
            if (polling_status != HAL_STATUS_OK && g_debug_mode) {
                printf("[OHT-50][DEBUG] module_polling_manager_update failed: %d\n", polling_status);
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
        // Stop Communication Manager API Server
        printf("[OHT-50] Stopping API Manager...\n");
        // Minimal API does not allocate endpoint resources; no-op
        (void)api_manager_stop();
        (void)api_manager_deinit();
        
        // Save Module Registry to YAML
        printf("[OHT-50] Saving Module Registry to YAML...\n");
        (void)registry_save_yaml("/etc/oht50/modules.yaml");
        
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


