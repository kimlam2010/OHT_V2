/**
 * @file register_validation.c
 * @brief Register validation for issue #135 fix
 * @version 1.0.0
 * @date 2025-01-28
 * @team FW
 * @task Fix RS485/Modbus data validation issues
 */

#include "register_validation.h"
#include "register_map.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Internal function prototypes
static bool validate_power_module_registers(uint16_t start_addr, uint16_t quantity);
static bool validate_motor_module_registers(uint16_t start_addr, uint16_t quantity);
static bool validate_safety_module_registers(uint16_t start_addr, uint16_t quantity);
static bool validate_dock_module_registers(uint16_t start_addr, uint16_t quantity);

/**
 * @brief Validate register read request for specific module
 * @param module_addr Module address (0x02-0x05)
 * @param start_addr Starting register address
 * @param quantity Number of registers to read
 * @return true if valid, false if invalid
 */
bool register_validation_validate_read_request(uint8_t module_addr, uint16_t start_addr, uint16_t quantity)
{
    // Check if quantity is reasonable (max 125 registers per Modbus spec)
    if (quantity == 0 || quantity > 125) {
        printf("[REG-VALID] Invalid quantity: %u (must be 1-125)\n", quantity);
        return false;
    }
    
    // Check if start address + quantity doesn't overflow
    if (start_addr + quantity > 0xFFFF) {
        printf("[REG-VALID] Address overflow: start=0x%04X, qty=%u\n", start_addr, quantity);
        return false;
    }
    
    // Validate based on module type
    switch (module_addr) {
        case MODULE_ADDR_POWER:
            return validate_power_module_registers(start_addr, quantity);
            
        case MODULE_ADDR_TRAVEL_MOTOR:
            return validate_motor_module_registers(start_addr, quantity);
            
        case MODULE_ADDR_SAFETY:
            return validate_safety_module_registers(start_addr, quantity);
            
        case MODULE_ADDR_DOCK:
            return validate_dock_module_registers(start_addr, quantity);
            
        default:
            printf("[REG-VALID] Unknown module address: 0x%02X\n", module_addr);
            return false;
    }
}

/**
 * @brief Validate received register data for all-zero payloads
 * @param module_addr Module address
 * @param start_addr Starting register address
 * @param data Register data
 * @param quantity Number of registers
 * @return true if data is valid (not all zeros), false if invalid
 */
bool register_validation_validate_data(uint8_t module_addr, uint16_t start_addr, 
                                      const uint16_t *data, uint16_t quantity)
{
    if (!data) {
        return false;
    }
    
    // Check for all-zero payload (common issue #135 symptom)
    bool all_zeros = true;
    for (uint16_t i = 0; i < quantity; i++) {
        if (data[i] != 0x0000) {
            all_zeros = false;
            break;
        }
    }
    
    if (all_zeros) {
        printf("[REG-VALID] All-zero payload detected for module 0x%02X, addr=0x%04X, qty=%u\n", 
               module_addr, start_addr, quantity);
        return false;
    }
    
    // Additional validation based on module type and register ranges
    switch (module_addr) {
        case MODULE_ADDR_POWER:
            return validate_power_data_ranges(start_addr, data, quantity);
            
        case MODULE_ADDR_TRAVEL_MOTOR:
            return validate_motor_data_ranges(start_addr, data, quantity);
            
        case MODULE_ADDR_SAFETY:
            return validate_safety_data_ranges(start_addr, data, quantity);
            
        case MODULE_ADDR_DOCK:
            return validate_dock_data_ranges(start_addr, data, quantity);
            
        default:
            // Unknown module - accept any non-zero data
            return true;
    }
}

/**
 * @brief Get recommended register read parameters for module
 * @param module_addr Module address
 * @param start_addr Pointer to recommended start address
 * @param quantity Pointer to recommended quantity
 * @return true if module supported, false otherwise
 */
bool register_validation_get_recommended_params(uint8_t module_addr, uint16_t *start_addr, uint16_t *quantity)
{
    if (!start_addr || !quantity) {
        return false;
    }
    
    switch (module_addr) {
        case MODULE_ADDR_POWER:
            // Start with basic battery data (4 registers)
            *start_addr = POWER_REG_BATTERY_VOLTAGE;  // 0x0000
            *quantity = 4;  // Voltage, Current, SOC, Max Cell V
            break;
            
        case MODULE_ADDR_TRAVEL_MOTOR:
            // Start with basic motor status (4 registers)
            *start_addr = DC_MOTOR_REG_MOTOR1_CONTROL_MODE;  // 0x0000
            *quantity = 4;  // Control mode, speed, position, status
            break;
            
        case MODULE_ADDR_SAFETY:
            // Start with basic safety status (4 registers)
            *start_addr = SAFETY_ANALOG_INPUT_1_REG;  // 0x0010
            *quantity = 4;  // Analog inputs
            break;
            
        case MODULE_ADDR_DOCK:
            // Start with basic position data (4 registers)
            *start_addr = DOCK_REG_POSITION_X;  // 0x0030
            *quantity = 4;  // Position X, Y, Z, status
            break;
            
        default:
            return false;
    }
    
    return true;
}

// Internal validation functions
static bool validate_power_module_registers(uint16_t start_addr, uint16_t quantity)
{
    // Power module valid ranges: 0x0000-0x00FF
    uint16_t end_addr = start_addr + quantity - 1;
    
    if (end_addr > 0x00FF) {
        printf("[REG-VALID] Power module address out of range: start=0x%04X, end=0x%04X\n", 
               start_addr, end_addr);
        return false;
    }
    
    // Check for problematic ranges that might return zeros
    if (start_addr >= 0x0080 && start_addr < 0x00F0) {
        printf("[REG-VALID] Warning: Power module reserved range: 0x%04X\n", start_addr);
        // Don't reject, but warn
    }
    
    return true;
}

static bool validate_motor_module_registers(uint16_t start_addr, uint16_t quantity)
{
    // Motor module valid ranges: 0x0000-0x0106
    uint16_t end_addr = start_addr + quantity - 1;
    
    if (end_addr > 0x0106) {
        printf("[REG-VALID] Motor module address out of range: start=0x%04X, end=0x%04X\n", 
               start_addr, end_addr);
        return false;
    }
    
    return true;
}

static bool validate_safety_module_registers(uint16_t start_addr, uint16_t quantity)
{
    // Safety module valid ranges: 0x0000-0x00FF
    uint16_t end_addr = start_addr + quantity - 1;
    
    if (end_addr > 0x00FF) {
        printf("[REG-VALID] Safety module address out of range: start=0x%04X, end=0x%04X\n", 
               start_addr, end_addr);
        return false;
    }
    
    return true;
}

static bool validate_dock_module_registers(uint16_t start_addr, uint16_t quantity)
{
    // Dock module valid ranges: 0x0000-0x00FF
    uint16_t end_addr = start_addr + quantity - 1;
    
    if (end_addr > 0x00FF) {
        printf("[REG-VALID] Dock module address out of range: start=0x%04X, end=0x%04X\n", 
               start_addr, end_addr);
        return false;
    }
    
    return true;
}

// Data range validation functions
bool validate_power_data_ranges(uint16_t start_addr, const uint16_t *data, uint16_t quantity)
{
    for (uint16_t i = 0; i < quantity; i++) {
        uint16_t reg_addr = start_addr + i;
        uint16_t value = data[i];
        
        switch (reg_addr) {
            case POWER_REG_BATTERY_VOLTAGE:  // 0x0000
                if (value == 0 && value > 50000) {  // 0-5000V range
                    printf("[REG-VALID] Power voltage out of range: %u\n", value);
                    return false;
                }
                break;
                
            case POWER_REG_BATTERY_CURRENT:  // 0x0001
                if (value > 20000) {  // 0-2000A range
                    printf("[REG-VALID] Power current out of range: %u\n", value);
                    return false;
                }
                break;
                
            case POWER_REG_BATTERY_SOC:  // 0x0002
                if (value > 1000) {  // 0-100% range
                    printf("[REG-VALID] Power SOC out of range: %u\n", value);
                    return false;
                }
                break;
        }
    }
    
    return true;
}

bool validate_motor_data_ranges(uint16_t start_addr, const uint16_t *data, uint16_t quantity)
{
    // Motor data validation - basic range checks
    for (uint16_t i = 0; i < quantity; i++) {
        uint16_t value = data[i];
        
        // Basic sanity checks for motor data
        if (start_addr + i == DC_MOTOR_REG_MOTOR1_CONTROL_MODE && value > 3) {
            printf("[REG-VALID] Motor control mode out of range: %u\n", value);
            return false;
        }
    }
    
    return true;
}

bool validate_safety_data_ranges(uint16_t start_addr, const uint16_t *data, uint16_t quantity)
{
    // Safety data validation - basic range checks
    for (uint16_t i = 0; i < quantity; i++) {
        uint16_t value = data[i];
        
        // Basic sanity checks for safety data
        if (start_addr + i >= SAFETY_ANALOG_INPUT_1_REG && start_addr + i <= SAFETY_ANALOG_INPUT_4_REG) {
            if (value > 5000) {  // Distance sensors typically 0-5000mm
                printf("[REG-VALID] Safety analog input out of range: %u\n", value);
                return false;
            }
        }
    }
    
    return true;
}

bool validate_dock_data_ranges(uint16_t start_addr, const uint16_t *data, uint16_t quantity)
{
    // Dock data validation - basic range checks
    for (uint16_t i = 0; i < quantity; i++) {
        uint16_t value = data[i];
        
        // Basic sanity checks for dock data
        if (start_addr + i == DOCK_REG_POSITION_X || start_addr + i == DOCK_REG_POSITION_Y) {
            if (value > 10000) {  // Position typically 0-10000mm
                printf("[REG-VALID] Dock position out of range: %u\n", value);
                return false;
            }
        }
    }
    
    return true;
}
