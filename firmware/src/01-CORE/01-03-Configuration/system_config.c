/**
 * @file system_config.c
 * @brief system_config implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @author Firmware Team
 * @license MIT
 * @compliance MISRA C:2012, AUTOSAR
 * @safety SIL2
 */

/**
 * @file system_config.c
 * @brief System configuration interface implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @author Firmware Team
 * @license MIT
 * @compliance MISRA C:2012, AUTOSAR
 * @safety SIL2
 */

#include "system_config.h"
#include "hal_config_persistence.h"
#include "config_system.h"

/* Module private variables */
static system_config_t g_system_config;
static bool g_config_loaded = false;
static uint32_t g_config_version = 0U;

/* Module private functions */
static config_result_t System_Config_Validate(const system_config_t* config);
static config_result_t System_Config_Apply(const system_config_t* config);

/**
 * @brief Initialize system configuration
 * @return CONFIG_RESULT_SUCCESS on success, error code otherwise
 */
config_result_t System_Config_Init(void)
{
    config_result_t result = CONFIG_RESULT_SUCCESS;
    
    /* Load configuration from persistent storage */
    result = HAL_Config_Load(&g_system_config, sizeof(system_config_t));
    
    if (CONFIG_RESULT_SUCCESS == result)
    {
        /* Validate loaded configuration */
        result = System_Config_Validate(&g_system_config);
        
        if (CONFIG_RESULT_SUCCESS == result)
        {
            /* Apply configuration */
            result = System_Config_Apply(&g_system_config);
            
            if (CONFIG_RESULT_SUCCESS == result)
            {
                g_config_loaded = true;
                g_config_version = g_system_config.version;
            }
        }
    }
    else
    {
        /* Load default configuration if persistent storage fails */
        result = System_Config_Load_Default();
    }
    
    return result;
}

/**
 * @brief Load default system configuration
 * @return CONFIG_RESULT_SUCCESS on success, error code otherwise
 */
config_result_t System_Config_Load_Default(void)
{
    config_result_t result = CONFIG_RESULT_SUCCESS;
    
    /* Initialize with default values */
    g_system_config.version = 1U;
    g_system_config.motor_max_speed = 1000U;
    g_system_config.motor_acceleration = 500U;
    g_system_config.motor_deceleration = 500U;
    g_system_config.safety_timeout_ms = 5000U;
    g_system_config.communication_timeout_ms = 1000U;
    g_system_config.led_brightness = 50U;
    g_system_config.debug_level = DEBUG_LEVEL_INFO;
    
    /* Validate default configuration */
    result = System_Config_Validate(&g_system_config);
    
    if (CONFIG_RESULT_SUCCESS == result)
    {
        /* Apply default configuration */
        result = System_Config_Apply(&g_system_config);
        
        if (CONFIG_RESULT_SUCCESS == result)
        {
            g_config_loaded = true;
            g_config_version = g_system_config.version;
        }
    }
    
    return result;
}

/**
 * @brief Get system configuration
 * @param config Pointer to configuration structure to fill
 * @return CONFIG_RESULT_SUCCESS on success, error code otherwise
 */
config_result_t System_Config_Get(system_config_t* config)
{
    config_result_t result = CONFIG_RESULT_SUCCESS;
    
    /* Validate parameters */
    if (NULL == config)
    {
        result = CONFIG_RESULT_INVALID_PARAMETER;
    }
    else if (false == g_config_loaded)
    {
        result = CONFIG_RESULT_NOT_LOADED;
    }
    else
    {
        /* Copy configuration */
        *config = g_system_config;
    }
    
    return result;
}

/**
 * @brief Set system configuration
 * @param config Pointer to new configuration
 * @return CONFIG_RESULT_SUCCESS on success, error code otherwise
 */
config_result_t System_Config_Set(const system_config_t* config)
{
    config_result_t result = CONFIG_RESULT_SUCCESS;
    
    /* Validate parameters */
    if (NULL == config)
    {
        result = CONFIG_RESULT_INVALID_PARAMETER;
    }
    else
    {
        /* Validate new configuration */
        result = System_Config_Validate(config);
        
        if (CONFIG_RESULT_SUCCESS == result)
        {
            /* Apply new configuration */
            result = System_Config_Apply(config);
            
            if (CONFIG_RESULT_SUCCESS == result)
            {
                /* Update internal configuration */
                g_system_config = *config;
                g_config_version = config->version;
                
                /* Save to persistent storage */
                result = HAL_Config_Save(&g_system_config, sizeof(system_config_t));
            }
        }
    }
    
    return result;
}

/**
 * @brief Validate system configuration
 * @param config Pointer to configuration to validate
 * @return CONFIG_RESULT_SUCCESS on success, error code otherwise
 */
static config_result_t System_Config_Validate(const system_config_t* config)
{
    config_result_t result = CONFIG_RESULT_SUCCESS;
    
    /* Validate version */
    if (0U == config->version)
    {
        result = CONFIG_RESULT_INVALID_VERSION;
    }
    /* Validate motor parameters */
    else if ((0U == config->motor_max_speed) || (config->motor_max_speed > 10000U))
    {
        result = CONFIG_RESULT_INVALID_MOTOR_SPEED;
    }
    else if ((0U == config->motor_acceleration) || (config->motor_acceleration > 5000U))
    {
        result = CONFIG_RESULT_INVALID_MOTOR_ACCELERATION;
    }
    else if ((0U == config->motor_deceleration) || (config->motor_deceleration > 5000U))
    {
        result = CONFIG_RESULT_INVALID_MOTOR_DECELERATION;
    }
    /* Validate safety parameters */
    else if ((100U > config->safety_timeout_ms) || (config->safety_timeout_ms > 30000U))
    {
        result = CONFIG_RESULT_INVALID_SAFETY_TIMEOUT;
    }
    /* Validate communication parameters */
    else if ((100U > config->communication_timeout_ms) || (config->communication_timeout_ms > 10000U))
    {
        result = CONFIG_RESULT_INVALID_COMMUNICATION_TIMEOUT;
    }
    /* Validate LED parameters */
    else if (config->led_brightness > 100U)
    {
        result = CONFIG_RESULT_INVALID_LED_BRIGHTNESS;
    }
    /* Validate debug level */
    else if (config->debug_level > DEBUG_LEVEL_MAX)
    {
        result = CONFIG_RESULT_INVALID_DEBUG_LEVEL;
    }
    
    return result;
}

/**
 * @brief Apply system configuration
 * @param config Pointer to configuration to apply
 * @return CONFIG_RESULT_SUCCESS on success, error code otherwise
 */
static config_result_t System_Config_Apply(const system_config_t* config)
{
    config_result_t result = CONFIG_RESULT_SUCCESS;
    
    /* Apply motor configuration */
    result = Config_System_Set_Motor_Parameters(config->motor_max_speed, 
                                               config->motor_acceleration, 
                                               config->motor_deceleration);
    
    if (CONFIG_RESULT_SUCCESS == result)
    {
        /* Apply safety configuration */
        result = Config_System_Set_Safety_Parameters(config->safety_timeout_ms);
        
        if (CONFIG_RESULT_SUCCESS == result)
        {
            /* Apply communication configuration */
            result = Config_System_Set_Communication_Parameters(config->communication_timeout_ms);
            
            if (CONFIG_RESULT_SUCCESS == result)
            {
                /* Apply LED configuration */
                result = Config_System_Set_LED_Parameters(config->led_brightness);
                
                if (CONFIG_RESULT_SUCCESS == result)
                {
                    /* Apply debug configuration */
                    result = Config_System_Set_Debug_Parameters(config->debug_level);
                }
            }
        }
    }
    
    return result;
}

/**
 * @brief Get configuration version
 * @return Configuration version number
 */
uint32_t System_Config_Get_Version(void)
{
    return g_config_version;
}

/**
 * @brief Check if configuration is loaded
 * @return true if configuration is loaded, false otherwise
 */
bool System_Config_Is_Loaded(void)
{
    return g_config_loaded;
}

/**
 * @brief Reset configuration to defaults
 * @return CONFIG_RESULT_SUCCESS on success, error code otherwise
 */
config_result_t System_Config_Reset(void)
{
    config_result_t result = CONFIG_RESULT_SUCCESS;
    
    /* Load default configuration */
    result = System_Config_Load_Default();
    
    if (CONFIG_RESULT_SUCCESS == result)
    {
        /* Save to persistent storage */
        result = HAL_Config_Save(&g_system_config, sizeof(system_config_t));
    }
    
    return result;
}
