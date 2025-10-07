/**
 * @file module_polling_manager.c
 * @brief Module Polling Manager Implementation for OHT-50
 * @version 1.0.0
 * @date 2025-01-28
 * @author FW Team
 * 
 * Implements dynamic polling of all discovered modules based on their types
 */

#include "module_polling_manager.h"
// Updated path for Domain-Driven Architecture v1.0.1
#include "../../infrastructure/communication/communication_manager.h"
// #include "power_module_handler.h"  // Not implemented yet
// #include "travel_motor_module_handler.h"  // Not implemented yet
#include "../../core/state_management/system_state_machine.h"
#include "../../validation/register_validation.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// Global polling manager instance
static module_polling_manager_t g_polling_manager = {0};

// Internal function prototypes
static uint64_t module_polling_get_timestamp_ms(void);
static hal_status_t module_polling_initialize_handler(uint8_t address, module_polling_type_t type);
static bool module_polling_should_poll(uint8_t address);
static hal_status_t module_polling_smart_read(uint8_t address, uint16_t start_reg, uint16_t count, uint16_t *data, const char *module_name);
static bool module_polling_validate_data(uint16_t *data, uint16_t count, const char *module_name);

/**
 * @brief Initialize module polling manager
 * @return HAL status
 */
hal_status_t module_polling_manager_init(void)
{
    printf("[POLLING-MGR] Initializing module polling manager...\n");
    
    // Clear manager state
    memset(&g_polling_manager, 0, sizeof(g_polling_manager));
    
    // Initialize all module slots as offline
    for (int i = 0; i <= 0xFF; i++) {  // Use 0xFF instead of MODULE_ADDR_MAX
        g_polling_manager.modules[i].address = i;
        g_polling_manager.modules[i].type = MODULE_TYPE_UNKNOWN;
        g_polling_manager.modules[i].is_online = false;
        g_polling_manager.modules[i].handler_initialized = false;
        g_polling_manager.modules[i].last_poll_ms = 0;
        g_polling_manager.modules[i].poll_interval_ms = POLLING_INTERVAL_UNKNOWN_MS;
    }
    
    g_polling_manager.initialized = true;
    g_polling_manager.total_modules = 0;
    
    printf("[POLLING-MGR] Module polling manager initialized successfully\n");
    return HAL_STATUS_OK;
}

/**
 * @brief Update module polling manager (call from main loop)
 * @return HAL status
 */
hal_status_t module_polling_manager_update(void)
{
    if (!g_polling_manager.initialized) {
        return HAL_STATUS_NOT_INITIALIZED;
    }
    
    // FIXED: Gate polling by system state for issue #135
    system_state_t current_state;
    hal_status_t state_status = system_state_machine_get_state(&current_state);
    if (state_status != HAL_STATUS_OK || current_state != SYSTEM_STATE_IDLE) {
        // Only poll when system is in IDLE state (READY)
        return HAL_STATUS_OK;
    }
    
    uint64_t current_time = module_polling_get_timestamp_ms();
    
    // Poll all online modules
    for (int i = 0; i <= 0xFF; i++) {  // Use 0xFF instead of MODULE_ADDR_MAX
        if (g_polling_manager.modules[i].is_online) {
            if (module_polling_should_poll(i)) {
                hal_status_t status = module_polling_manager_poll_module(i);
                if (status == HAL_STATUS_OK) {
                    g_polling_manager.modules[i].last_poll_ms = current_time;
                }
            }
        }
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Add module to polling manager
 * @param address Module address
 * @param type Module type
 * @return HAL status
 */
hal_status_t module_polling_manager_add_module(uint8_t address, module_polling_type_t type)
{
    // Address is already uint8_t, so no need to check > 0xFF
    printf("[POLLING-MGR] Adding module 0x%02X (type: %s)\n", address, module_polling_type_to_string(type));
    
    // Update module info
    g_polling_manager.modules[address].address = address;
    g_polling_manager.modules[address].type = type;
    g_polling_manager.modules[address].is_online = true;
    g_polling_manager.modules[address].poll_interval_ms = module_polling_get_interval(type);
    g_polling_manager.modules[address].last_poll_ms = 0;
    
    // Initialize handler for this module type
    hal_status_t status = module_polling_initialize_handler(address, type);
    if (status == HAL_STATUS_OK) {
        g_polling_manager.modules[address].handler_initialized = true;
        printf("[POLLING-MGR] Module 0x%02X handler initialized successfully\n", address);
    } else {
        printf("[POLLING-MGR] WARNING: Module 0x%02X handler initialization failed: %d\n", address, status);
        g_polling_manager.modules[address].handler_initialized = false;
    }
    
    g_polling_manager.total_modules++;
    
    printf("[POLLING-MGR] Module 0x%02X added successfully (total: %u)\n", address, g_polling_manager.total_modules);
    return HAL_STATUS_OK;
}

/**
 * @brief Remove module from polling manager
 * @param address Module address
 * @return HAL status
 */
hal_status_t module_polling_manager_remove_module(uint8_t address)
{
    // Address is already uint8_t, so no need to check > 0xFF
    
    if (g_polling_manager.modules[address].is_online) {
        printf("[POLLING-MGR] Removing module 0x%02X\n", address);
        
        g_polling_manager.modules[address].is_online = false;
        g_polling_manager.modules[address].handler_initialized = false;
        g_polling_manager.modules[address].type = MODULE_TYPE_UNKNOWN;
        
        if (g_polling_manager.total_modules > 0) {
            g_polling_manager.total_modules--;
        }
        
        printf("[POLLING-MGR] Module 0x%02X removed (total: %u)\n", address, g_polling_manager.total_modules);
    }
    
    return HAL_STATUS_OK;
}

/**
 * @brief Poll specific module
 * @param address Module address
 * @return HAL status
 */
hal_status_t module_polling_manager_poll_module(uint8_t address)
{
    // Check if module is online (address is already uint8_t)
    if (!g_polling_manager.modules[address].is_online) {
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    module_polling_type_t type = g_polling_manager.modules[address].type;
    
    switch (type) {
        case MODULE_TYPE_POWER:
            return module_polling_power_module(address);
            
        case MODULE_TYPE_TRAVEL_MOTOR:  // Use MODULE_TYPE_TRAVEL_MOTOR instead of MODULE_TYPE_MOTOR
            return module_polling_motor_module(address);
            
        case MODULE_TYPE_SAFETY:  // Use MODULE_TYPE_SAFETY instead of MODULE_TYPE_SENSOR
            return module_polling_sensor_module(address);
            
        case MODULE_TYPE_DOCK:  // Use MODULE_TYPE_DOCK instead of MODULE_TYPE_LIDAR
            return module_polling_dock_module(address);
            
        case MODULE_TYPE_UNKNOWN:
        default:
            return module_polling_unknown_module(address);
    }
}

/**
 * @brief Get module polling info
 * @param address Module address
 * @param info Output info structure
 * @return HAL status
 */
hal_status_t module_polling_manager_get_module_info(uint8_t address, module_polling_info_t *info)
{
    if (info == NULL) {  // Address is already uint8_t, so no need to check > 0xFF
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    *info = g_polling_manager.modules[address];
    return HAL_STATUS_OK;
}

/**
 * @brief Poll Power Module (Type 2) with fallback strategy
 * @param address Module address
 * @return HAL status
 */
hal_status_t module_polling_power_module(uint8_t address)
{
    printf("[POLLING-POWER] Polling Power Module 0x%02X (Full 51 registers)\n", address);
    
    // hal_status_t overall_status = HAL_STATUS_OK; // Unused - commented out
    uint32_t success_count = 0;
    uint32_t total_attempts = 0;
    
    // HIGH PRIORITY: Critical Battery Data (1s interval) - 21 registers
    printf("[POLLING-POWER] 0x%02X: Reading HIGH priority data...\n", address);
    
    // 1. Critical Battery Data (11 registers: 0x0000-0x000A)
    uint16_t battery_data[11];
    hal_status_t status = module_polling_smart_read(address, 0x0000, 11, battery_data, "POWER");
    total_attempts++;
    if (status == HAL_STATUS_OK) {
        success_count++;
        // Raw dump of registers 0x0000-0x000A to verify data integrity
        printf("[POLLING-POWER][RAW] 0x%02X: 0000-000A: %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X %04X\n",
               address,
               battery_data[0], battery_data[1], battery_data[2], battery_data[3], battery_data[4],
               battery_data[5], battery_data[6], battery_data[7], battery_data[8], battery_data[9], battery_data[10]);

        // Print with signed current formatted to one decimal place
        double current_a = ((int16_t)battery_data[1]) / 10.0;
        printf("[POLLING-POWER] 0x%02X: Battery=%d.%dV, Current=%.1fA, SOC=%d.%d%%, MaxCell=%dmV, MinCell=%dmV, Temp=%d°C, Conn=%d, Status=0x%04X\n",
               address, battery_data[0]/10, battery_data[0]%10,
               current_a,
               battery_data[2]/10, battery_data[2]%10,
               battery_data[3], battery_data[4], (int16_t)battery_data[8],
               battery_data[9], battery_data[10]);
    }
    
    // 2. Cell Voltages (6 registers: 0x0014-0x0019)
    uint16_t cell_data[6];
    status = module_polling_smart_read(address, 0x0014, 6, cell_data, "POWER");
    total_attempts++;
    if (status == HAL_STATUS_OK) {
        success_count++;
        printf("[POLLING-POWER] 0x%02X: Cell Voltages: [%d, %d, %d, %d, %d, %d] mV\n",
               address, cell_data[0], cell_data[1], cell_data[2], cell_data[3], cell_data[4], cell_data[5]);
    }
    
    // 3. Power Distribution Voltages (3 registers: 0x0040, 0x0043, 0x0046)
    uint16_t power_volt_data[3];
    status = module_polling_smart_read(address, 0x0040, 1, &power_volt_data[0], "POWER"); // 12V
    total_attempts++;
    if (status == HAL_STATUS_OK) {
        success_count++;
        status = module_polling_smart_read(address, 0x0043, 1, &power_volt_data[1], "POWER"); // 5V
        total_attempts++;
        if (status == HAL_STATUS_OK) {
            success_count++;
            status = module_polling_smart_read(address, 0x0046, 1, &power_volt_data[2], "POWER"); // 3.3V
            total_attempts++;
            if (status == HAL_STATUS_OK) {
                success_count++;
                printf("[POLLING-POWER] 0x%02X: Power Voltages: 12V=%d.%dV, 5V=%d.%dV, 3.3V=%d.%dV\n",
                       address, power_volt_data[0]/10, power_volt_data[0]%10,
                       power_volt_data[1]/10, power_volt_data[1]%10,
                       power_volt_data[2]/10, power_volt_data[2]%10);
            }
        }
    }
    
    // 4. Relay States (4 registers: 0x0049-0x004C)
    uint16_t relay_data[4];
    status = module_polling_smart_read(address, 0x0049, 4, relay_data, "POWER");
    total_attempts++;
    if (status == HAL_STATUS_OK) {
        success_count++;
        printf("[POLLING-POWER] 0x%02X: Relays: 12V=%d, 5V=%d, 3V3=%d, Fault=%d\n",
               address, relay_data[0], relay_data[1], relay_data[2], relay_data[3]);
    }
    
    // MEDIUM PRIORITY: Charging & Power Distribution (5s interval) - 20 registers
    printf("[POLLING-POWER] 0x%02X: Reading MEDIUM priority data...\n", address);
    
    // 5. SK60X Charging Control (8 registers: 0x0030-0x0037)
    uint16_t charging_data[8];
    status = module_polling_smart_read(address, 0x0030, 8, charging_data, "POWER");
    total_attempts++;
    if (status == HAL_STATUS_OK) {
        success_count++;
        printf("[POLLING-POWER] 0x%02X: Charging: VSet=%d.%dV, ISet=%d.%dA, VOut=%d.%dV, IOut=%d.%dA, POut=%d.%dW, VIn=%d.%dV, IIn=%d.%dA, Temp=%d°C\n",
               address, charging_data[0]/10, charging_data[0]%10,
               charging_data[1]/10, charging_data[1]%10,
               charging_data[2]/10, charging_data[2]%10,
               charging_data[3]/10, charging_data[3]%10,
               charging_data[4]/10, charging_data[4]%10,
               charging_data[5]/10, charging_data[5]%10,
               charging_data[6]/10, charging_data[6]%10,
               (int16_t)charging_data[7]);
    }
    
    // 6. Power Distribution Current/Power (6 registers: 0x0041, 0x0042, 0x0044, 0x0045, 0x0047, 0x0048)
    uint16_t power_curr_data[6];
    status = module_polling_smart_read(address, 0x0041, 2, &power_curr_data[0], "POWER"); // 12V I/P
    total_attempts++;
    if (status == HAL_STATUS_OK) {
        success_count++;
        status = module_polling_smart_read(address, 0x0044, 2, &power_curr_data[2], "POWER"); // 5V I/P
        total_attempts++;
        if (status == HAL_STATUS_OK) {
            success_count++;
            status = module_polling_smart_read(address, 0x0047, 2, &power_curr_data[4], "POWER"); // 3.3V I/P
            total_attempts++;
            if (status == HAL_STATUS_OK) {
                success_count++;
                printf("[POLLING-POWER] 0x%02X: Power Distribution: 12V=%d.%dA/%d.%dW, 5V=%d.%dA/%d.%dW, 3.3V=%d.%dA/%d.%dW\n",
                       address, power_curr_data[0]/10, power_curr_data[0]%10, power_curr_data[1]/10, power_curr_data[1]%10,
                       power_curr_data[2]/10, power_curr_data[2]%10, power_curr_data[3]/10, power_curr_data[3]%10,
                       power_curr_data[4]/10, power_curr_data[4]%10, power_curr_data[5]/10, power_curr_data[5]%10);
            }
        }
    }
    
    // 7. Cell Balancing Status (6 registers: 0x001C-0x0021)
    uint16_t balance_data[6];
    status = module_polling_smart_read(address, 0x001C, 6, balance_data, "POWER");
    total_attempts++;
    if (status == HAL_STATUS_OK) {
        success_count++;
        printf("[POLLING-POWER] 0x%02X: Cell Balance: [%d, %d, %d, %d, %d, %d]\n",
               address, balance_data[0], balance_data[1], balance_data[2], balance_data[3], balance_data[4], balance_data[5]);
    }
    
    // LOW PRIORITY: System Information (30s interval) - 10 registers
    printf("[POLLING-POWER] 0x%02X: Reading LOW priority data...\n", address);
    
    // 8. System Registers (8 registers: 0x0100-0x0107)
    uint16_t system_data[8];
    status = module_polling_smart_read(address, 0x0100, 8, system_data, "POWER");
    total_attempts++;
    if (status == HAL_STATUS_OK) {
        success_count++;
        printf("[POLLING-POWER] 0x%02X: System: DeviceID=0x%04X, FW=0x%04X, Status=0x%04X, Error=0x%04X, Type=0x%04X, HW=0x%04X, SN=0x%08X, Build=0x%04X\n",
               (unsigned int)address, (unsigned int)system_data[0], (unsigned int)system_data[1], (unsigned int)system_data[2], (unsigned int)system_data[3], 
               (unsigned int)system_data[7], 
               (unsigned int)((system_data[4] << 16) | system_data[5]), 
               (unsigned int)((system_data[6] << 16) | system_data[7]), (unsigned int)0x1234);
    }
    
    // 9. Configuration Registers (2 registers: 0x004D, 0x003E)
    uint16_t config_data[2];
    status = module_polling_smart_read(address, 0x004D, 1, &config_data[0], "POWER"); // Voltage threshold
    total_attempts++;
    if (status == HAL_STATUS_OK) {
        success_count++;
        status = module_polling_smart_read(address, 0x003E, 1, &config_data[1], "POWER"); // Charge state
        total_attempts++;
        if (status == HAL_STATUS_OK) {
            success_count++;
            printf("[POLLING-POWER] 0x%02X: Config: VThreshold=%d.%d%%, ChargeState=%d\n",
                   address, config_data[0]/100, config_data[0]%100, config_data[1]);
        }
    }
    
    // Summary
    printf("[POLLING-POWER] 0x%02X: Summary: %d/%d registers read successfully (%.1f%%)\n",
           address, success_count, total_attempts, (float)success_count * 100.0f / total_attempts);
    
    if (success_count >= total_attempts * 0.7f) { // 70% success rate
        return HAL_STATUS_OK;
    } else {
        return HAL_STATUS_ERROR;
    }
}

/**
 * @brief Poll Motor Module (Type 4)
 * @param address Module address
 * @return HAL status
 */
hal_status_t module_polling_motor_module(uint8_t address)
{
    printf("[POLLING-MOTOR] Polling Motor Module 0x%02X\n", address);
    
    // Strategy 1: Try to read system registers first (0x0100-0x0107)
    uint16_t system_data[8];
    hal_status_t status = module_polling_smart_read(address, 0x0100, 8, system_data, "MOTOR");
    
    if (status == HAL_STATUS_OK) {
        printf("[POLLING-MOTOR] 0x%02X: DeviceID=0x%04X, FW=0x%04X, HW=0x%04X, Type=0x%04X\n",
               address, system_data[0], system_data[1], system_data[2], system_data[5]);
        
        // Strategy 2: Try to read motor control registers (0x0000-0x000F)
        uint16_t control_data[16];
        hal_status_t control_status = module_polling_smart_read(address, 0x0000, 16, control_data, "MOTOR");
        
        if (control_status == HAL_STATUS_OK) {
            printf("[POLLING-MOTOR] 0x%02X: Enable=%d, Mode=%d, Speed=%d/%d, Pos=%d/%d, Temp=%d°C, V=%d.%dV, I=%d.%dA\n",
                   address, control_data[0], control_data[1], control_data[2], control_data[3],
                   control_data[4], control_data[5], control_data[11], 
                   control_data[12]/10, control_data[12]%10, control_data[13]/10, control_data[13]%10);
        } else {
            printf("[POLLING-MOTOR] 0x%02X: Control data read failed, using system data only\n", address);
        }
        
        // Strategy 3: Try to read motor status registers (0x0010-0x001F)
        uint16_t status_data[16];
        hal_status_t status_status = module_polling_smart_read(address, 0x0010, 16, status_data, "MOTOR");
        
        if (status_status == HAL_STATUS_OK) {
            printf("[POLLING-MOTOR] 0x%02X: Running=%d, Ready=%d, Fault=%d, E-Stop=%d, Home=%d, Limit=%d\n",
                   address, status_data[0], status_data[1], status_data[2], status_data[4],
                   status_data[5], status_data[6]);
        } else {
            printf("[POLLING-MOTOR] 0x%02X: Status data read failed, using system data only\n", address);
        }
        
        return HAL_STATUS_OK;
    } else {
        printf("[POLLING-MOTOR] 0x%02X: System registers read failed (status: %d)\n", address, status);
        return status;
    }
}

/**
 * @brief Poll Safety Module (Type 3) with fallback strategy
 * @param address Module address
 * @return HAL status
 */
hal_status_t module_polling_sensor_module(uint8_t address)
{
    printf("[POLLING-SAFETY] Polling Safety Module 0x%02X\n", address);
    
    // Strategy 1: Try to read system registers first (0x0100-0x0107)
    uint16_t system_data[8];
    hal_status_t status = comm_manager_modbus_read_holding_registers(address, 0x0100, 8, system_data);
    
    if (status == HAL_STATUS_OK) {
        printf("[POLLING-SAFETY] 0x%02X: DeviceID=0x%04X, Type=0x%04X, Status=0x%04X, Version=0x%04X\n",
               address, system_data[0], system_data[7], system_data[2], system_data[1]);
        
        // Strategy 2: Try to read safety-specific registers (0x0000-0x0053)
        uint16_t safety_data[8];
        hal_status_t safety_status = comm_manager_modbus_read_holding_registers(address, 0x0000, 8, safety_data);
        
        if (safety_status == HAL_STATUS_OK) {
            printf("[POLLING-SAFETY] 0x%02X: EStop=%d, Interlock=%d, Zone1=%d, Zone2=%d, Zone3=%d, Zone4=%d, Zone5=%d, Zone6=%d\n",
                   address, safety_data[0], safety_data[1], safety_data[2], safety_data[3], 
                   safety_data[4], safety_data[5], safety_data[6], safety_data[7]);
        } else {
            printf("[POLLING-SAFETY] 0x%02X: Safety data read failed, using system data only\n", address);
        }
        
        return HAL_STATUS_OK;
    } else {
        printf("[POLLING-SAFETY] 0x%02X: System registers read failed (status: %d)\n", address, status);
        return status;
    }
}

/**
 * @brief Poll Dock Module (Type 5) with real sensor data - IMPLEMENTATION FOR ISSUE #138
 * @param address Module address
 * @return HAL status
 */
hal_status_t module_polling_dock_module(uint8_t address)
{
    printf("[POLLING-DOCK] Polling Dock Module 0x%02X with real sensor data\n", address);
    
    // Strategy 1: Try to read system registers first (0x0100-0x0107)
    uint16_t system_data[8];
    hal_status_t status = comm_manager_modbus_read_holding_registers(address, 0x0100, 8, system_data);
    
    if (status == HAL_STATUS_OK) {
        printf("[POLLING-DOCK] 0x%02X: DeviceID=0x%04X, Type=0x%04X, Status=0x%04X, Version=0x%04X\n",
               address, system_data[0], system_data[7], system_data[2], system_data[1]);
        
        // Strategy 2: Poll RFID data every 100ms (registers 0x0108-0x010C) - REAL HARDWARE ADDRESSES
        uint16_t rfid_data[5];
        hal_status_t rfid_status = comm_manager_modbus_read_holding_registers(address, 0x0108, 5, rfid_data);
        
        if (rfid_status == HAL_STATUS_OK) {
            uint32_t tag_id = ((uint32_t)rfid_data[1] << 16) | rfid_data[0];
            printf("[POLLING-DOCK] 0x%02X: RFID TagID=0x%08X, Signal=%d%%, Status=%d, Time=%d\n",
                   address, tag_id, rfid_data[2], rfid_data[3], rfid_data[4]);
        } else {
            printf("[POLLING-DOCK] 0x%02X: RFID data read failed (status: %d)\n", address, rfid_status);
        }
        
        // Strategy 3: Poll accelerometer data every 50ms (registers 0x010D-0x0111) - REAL HARDWARE ADDRESSES
        uint16_t accel_data[5];
        hal_status_t accel_status = comm_manager_modbus_read_holding_registers(address, 0x010D, 5, accel_data);
        
        if (accel_status == HAL_STATUS_OK) {
            printf("[POLLING-DOCK] 0x%02X: Accel X=%d, Y=%d, Z=%d mg, Temp=%d°C, Status=%d\n",
                   address, (int16_t)accel_data[0], (int16_t)accel_data[1], (int16_t)accel_data[2], 
                   (int16_t)accel_data[3], accel_data[4]);
        } else {
            printf("[POLLING-DOCK] 0x%02X: Accelerometer data read failed (status: %d)\n", address, accel_status);
        }
        
        // Strategy 4: Poll proximity sensors every 50ms (registers 0x0112-0x0116) - REAL HARDWARE ADDRESSES
        uint16_t prox_data[5];
        hal_status_t prox_status = comm_manager_modbus_read_holding_registers(address, 0x0112, 5, prox_data);
        
        if (prox_status == HAL_STATUS_OK) {
            printf("[POLLING-DOCK] 0x%02X: Prox1=%d (digital), Prox2=%d (digital), Dist1=%dmm, Dist2=%dmm, DockConfirmed=%d\n",
                   address, prox_data[0], prox_data[1], prox_data[2], prox_data[3], prox_data[4]);
        } else {
            printf("[POLLING-DOCK] 0x%02X: Proximity sensors data read failed (status: %d)\n", address, prox_status);
        }
        
        // Strategy 5: Read dock status and position (registers 0x0104-0x0107)
        uint16_t dock_data[4];
        hal_status_t dock_status = comm_manager_modbus_read_holding_registers(address, 0x0104, 4, dock_data);
        
        if (dock_status == HAL_STATUS_OK) {
            printf("[POLLING-DOCK] 0x%02X: Position=%d, Target=%d, Status=%d, Accuracy=%d\n",
                   address, dock_data[0], dock_data[1], dock_data[2], dock_data[3]);
        } else {
            printf("[POLLING-DOCK] 0x%02X: Dock status data read failed (status: %d)\n", address, dock_status);
        }
        
        return HAL_STATUS_OK;
    } else {
        printf("[POLLING-DOCK] 0x%02X: System registers read failed (status: %d)\n", address, status);
        return status;
    }
}

/**
 * @brief Poll Unknown Module (Type 0)
 * @param address Module address
 * @return HAL status
 */
hal_status_t module_polling_unknown_module(uint8_t address)
{
    printf("[POLLING-UNKNOWN] Polling Unknown Module 0x%02X\n", address);
    
    // Read basic module data
    uint16_t basic_data[2];
    hal_status_t status = comm_manager_modbus_read_holding_registers(address, 0x0100, 2, basic_data);
    
    if (status == HAL_STATUS_OK) {
        printf("[POLLING-UNKNOWN] 0x%02X: DeviceID=0x%04X, Status=0x%04X\n",
               address, basic_data[0], basic_data[1]);
    } else {
        printf("[POLLING-UNKNOWN] 0x%02X: Read failed (status: %d)\n", address, status);
    }
    
    return status;
}

/**
 * @brief Convert module type to string
 * @param type Module type
 * @return String representation
 */
const char* module_polling_type_to_string(module_polling_type_t type)
{
    switch (type) {
        case MODULE_TYPE_POWER:        return "Power";
        case MODULE_TYPE_TRAVEL_MOTOR: return "Travel Motor";
        case MODULE_TYPE_SAFETY:       return "Safety";
        case MODULE_TYPE_DOCK:         return "Dock";
        case MODULE_TYPE_UNKNOWN:      return "Unknown";
        default:                       return "Invalid";
    }
}

/**
 * @brief Get polling interval for module type
 * @param type Module type
 * @return Polling interval in milliseconds
 */
uint32_t module_polling_get_interval(module_polling_type_t type)
{
    switch (type) {
        case MODULE_TYPE_POWER:        return POLLING_INTERVAL_POWER_MS;
        case MODULE_TYPE_TRAVEL_MOTOR: return POLLING_INTERVAL_MOTOR_MS;
        case MODULE_TYPE_SAFETY:       return POLLING_INTERVAL_SENSOR_MS;
        case MODULE_TYPE_DOCK:         return 50;  // 50ms for real-time sensor data
        case MODULE_TYPE_UNKNOWN:      return POLLING_INTERVAL_UNKNOWN_MS;
        default:                       return POLLING_INTERVAL_UNKNOWN_MS;
    }
}

// Internal functions

/**
 * @brief Get current timestamp in milliseconds
 * @return Timestamp in milliseconds
 */
static uint64_t module_polling_get_timestamp_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)(ts.tv_sec * 1000ULL + ts.tv_nsec / 1000000ULL);
}

/**
 * @brief Initialize handler for module type
 * @param address Module address
 * @param type Module type
 * @return HAL status
 */
static hal_status_t module_polling_initialize_handler(uint8_t address, module_polling_type_t type)
{
    // Suppress unused parameter warnings
    (void)address;
    (void)type;
    
    // For now, just return OK - handlers will be initialized separately
    // This can be expanded to initialize specific handlers based on module type
    return HAL_STATUS_OK;
}

/**
 * @brief Check if module should be polled
 * @param address Module address
 * @return true if should poll, false otherwise
 */
static bool module_polling_should_poll(uint8_t address)
{
    // Check if module is online (address is already uint8_t, so no need to check > 0xFF)
    if (!g_polling_manager.modules[address].is_online) {
        return false;
    }
    
    uint64_t current_time = module_polling_get_timestamp_ms();
    uint64_t last_poll = g_polling_manager.modules[address].last_poll_ms;
    uint32_t interval = g_polling_manager.modules[address].poll_interval_ms;
    
    return (current_time - last_poll) >= interval;
}

/**
 * @brief Smart read function with retry and validation
 * @param address Module address
 * @param start_reg Starting register address
 * @param count Number of registers to read
 * @param data Output data buffer
 * @param module_name Module name for logging
 * @return HAL status
 */
static hal_status_t module_polling_smart_read(uint8_t address, uint16_t start_reg, uint16_t count, uint16_t *data, const char *module_name)
{
    // FIXED: Add register validation for issue #135
    if (!register_validation_validate_read_request(address, start_reg, count)) {
        printf("[POLLING-%s] 0x%02X: Invalid register request (addr=0x%04X, qty=%u)\n", 
               module_name, address, start_reg, count);
        return HAL_STATUS_INVALID_PARAMETER;
    }
    
    const int max_retries = 3;
    hal_status_t status = HAL_STATUS_ERROR;
    
    for (int retry = 0; retry < max_retries; retry++) {
        status = comm_manager_modbus_read_holding_registers(address, start_reg, count, data);
        
        if (status == HAL_STATUS_OK) {
            // FIXED: Use enhanced validation for issue #135
            if (register_validation_validate_data(address, start_reg, data, count)) {
                if (retry > 0) {
                    printf("[POLLING-%s] 0x%02X: Success after %d retries\n", module_name, address, retry);
                }
                return HAL_STATUS_OK;
            } else {
                printf("[POLLING-%s] 0x%02X: Data validation failed (all zeros?) on retry %d\n", module_name, address, retry + 1);
                status = HAL_STATUS_ERROR;
            }
        } else {
            if (retry < max_retries - 1) {
                printf("[POLLING-%s] 0x%02X: Read failed, retrying... (%d/%d)\n", module_name, address, retry + 1, max_retries);
                usleep(100000); // Wait 100ms before retry
            }
        }
    }
    
    printf("[POLLING-%s] 0x%02X: All retries failed (status: %d)\n", module_name, address, status);
    return status;
}

/**
 * @brief Validate module data for reasonable values
 * @param data Data buffer
 * @param count Number of data points
 * @param module_name Module name for logging
 * @return true if data is valid, false otherwise
 */
static bool module_polling_validate_data(uint16_t *data __attribute__((unused)), uint16_t count __attribute__((unused)), const char *module_name __attribute__((unused)))
{
    if (data == NULL || count == 0) {
        return false;
    }
    
    // Basic validation: check for all zeros (might indicate communication error)
    bool all_zeros = true;
    for (uint16_t i = 0; i < count; i++) {
        if (data[i] != 0) {
            all_zeros = false;
            break;
        }
    }
    
    if (all_zeros) {
        printf("[POLLING-%s] WARNING: All data is zero - possible communication error\n", module_name);
        return false;
    }
    
    // Check for reasonable ranges based on module type
    if (strcmp(module_name, "POWER") == 0) {
        // Power module validation
        if (data[0] > 50000) { // Device ID should be reasonable
            printf("[POLLING-%s] WARNING: Device ID seems too high: 0x%04X\n", module_name, data[0]);
            return false;
        }
    } else if (strcmp(module_name, "SAFETY") == 0) {
        // Safety module validation
        if (data[0] > 50000) { // Device ID should be reasonable
            printf("[POLLING-%s] WARNING: Device ID seems too high: 0x%04X\n", module_name, data[0]);
            return false;
        }
    } else if (strcmp(module_name, "DOCK") == 0) {
        // Dock module validation
        if (data[0] > 50000) { // Device ID should be reasonable
            printf("[POLLING-%s] WARNING: Device ID seems too high: 0x%04X\n", module_name, data[0]);
            return false;
        }
    }
    
    return true;
}
