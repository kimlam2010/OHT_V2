/**
 * @file system_init.c
 * @brief system_init implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @author Firmware Team
 * @license MIT
 * @compliance MISRA C:2012, AUTOSAR
 * @safety SIL2
 */

/**
 * @file system_init.c
 * @brief System initialization implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @author Firmware Team
 * @license MIT
 * @compliance MISRA C:2012, AUTOSAR
 * @safety SIL2
 */

#include "system_init.h"
#include "hal_common.h"
#include "system_state_machine.h"
#include "config_system.h"

/* Module private variables */
static bool g_system_initialized = false;
static uint32_t g_init_error_count = 0U;

/* Module private functions */
static system_result_t System_Init_HAL(void);
static system_result_t System_Init_Modules(void);
static system_result_t System_Init_Services(void);

/**
 * @brief Initialize the entire system
 * @return SYSTEM_RESULT_SUCCESS on success, error code otherwise
 */
system_result_t System_Init(void)
{
    system_result_t result = SYSTEM_RESULT_SUCCESS;
    
    /* Check if already initialized */
    if (true == g_system_initialized)
    {
        result = SYSTEM_RESULT_ALREADY_INITIALIZED;
    }
    else
    {
        /* Initialize HAL layer */
        result = System_Init_HAL();
        
        if (SYSTEM_RESULT_SUCCESS == result)
        {
            /* Initialize modules */
            result = System_Init_Modules();
            
            if (SYSTEM_RESULT_SUCCESS == result)
            {
                /* Initialize services */
                result = System_Init_Services();
                
                if (SYSTEM_RESULT_SUCCESS == result)
                {
                    /* Mark system as initialized */
                    g_system_initialized = true;
                    g_init_error_count = 0U;
                }
                else
                {
                    g_init_error_count++;
                }
            }
            else
            {
                g_init_error_count++;
            }
        }
        else
        {
            g_init_error_count++;
        }
    }
    
    return result;
}

/**
 * @brief Initialize Hardware Abstraction Layer
 * @return SYSTEM_RESULT_SUCCESS on success, error code otherwise
 */
static system_result_t System_Init_HAL(void)
{
    system_result_t result = SYSTEM_RESULT_SUCCESS;
    
    /* Initialize common HAL */
    result = HAL_Common_Init();
    
    if (SYSTEM_RESULT_SUCCESS == result)
    {
        /* Initialize communication HAL */
        result = HAL_RS485_Init();
        
        if (SYSTEM_RESULT_SUCCESS == result)
        {
            result = HAL_Network_Init();
            
            if (SYSTEM_RESULT_SUCCESS == result)
            {
                result = HAL_USB_Debug_Init();
            }
        }
    }
    
    return result;
}

/**
 * @brief Initialize application modules
 * @return SYSTEM_RESULT_SUCCESS on success, error code otherwise
 */
static system_result_t System_Init_Modules(void)
{
    system_result_t result = SYSTEM_RESULT_SUCCESS;
    
    /* Initialize motor module */
    result = Motor_Module_Init();
    
    if (SYSTEM_RESULT_SUCCESS == result)
    {
        /* Initialize dock module */
        result = Dock_Module_Init();
        
        if (SYSTEM_RESULT_SUCCESS == result)
        {
            /* Initialize power module */
            result = Power_Module_Init();
            
            if (SYSTEM_RESULT_SUCCESS == result)
            {
                /* Initialize IO module */
                result = IO_Module_Init();
                
                if (SYSTEM_RESULT_SUCCESS == result)
                {
                    /* Initialize LED module */
                    result = LED_Module_Init();
                }
            }
        }
    }
    
    return result;
}

/**
 * @brief Initialize system services
 * @return SYSTEM_RESULT_SUCCESS on success, error code otherwise
 */
static system_result_t System_Init_Services(void)
{
    system_result_t result = SYSTEM_RESULT_SUCCESS;
    
    /* Initialize API services */
    result = API_Manager_Init();
    
    if (SYSTEM_RESULT_SUCCESS == result)
    {
        /* Initialize communication services */
        result = Communication_Manager_Init();
        
        if (SYSTEM_RESULT_SUCCESS == result)
        {
            /* Initialize safety services */
            result = Safety_Manager_Init();
            
            if (SYSTEM_RESULT_SUCCESS == result)
            {
                /* Initialize security services */
                result = Security_Manager_Init();
                
                if (SYSTEM_RESULT_SUCCESS == result)
                {
                    /* Initialize diagnostics services */
                    result = Diagnostics_Manager_Init();
                }
            }
        }
    }
    
    return result;
}

/**
 * @brief Get system initialization status
 * @return true if system is initialized, false otherwise
 */
bool System_Is_Initialized(void)
{
    return g_system_initialized;
}

/**
 * @brief Get initialization error count
 * @return Number of initialization errors
 */
uint32_t System_Get_Init_Error_Count(void)
{
    return g_init_error_count;
}

/**
 * @brief Shutdown the system
 * @return SYSTEM_RESULT_SUCCESS on success, error code otherwise
 */
system_result_t System_Shutdown(void)
{
    system_result_t result = SYSTEM_RESULT_SUCCESS;
    
    if (true == g_system_initialized)
    {
        /* Shutdown services */
        result = API_Manager_Shutdown();
        
        if (SYSTEM_RESULT_SUCCESS == result)
        {
            result = Communication_Manager_Shutdown();
            
            if (SYSTEM_RESULT_SUCCESS == result)
            {
                result = Safety_Manager_Shutdown();
                
                if (SYSTEM_RESULT_SUCCESS == result)
                {
                    /* Mark system as not initialized */
                    g_system_initialized = false;
                }
            }
        }
    }
    else
    {
        result = SYSTEM_RESULT_NOT_INITIALIZED;
    }
    
    return result;
}
