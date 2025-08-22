/**
 * @file state_transitions.c
 * @brief state_transitions implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @author Firmware Team
 * @license MIT
 * @compliance MISRA C:2012, AUTOSAR
 * @safety SIL2
 */

/**
 * @file state_transitions.c
 * @brief State transition logic implementation
 * @version 1.0.0
 * @date 2025-01-27
 * @author Firmware Team
 * @license MIT
 * @compliance MISRA C:2012, AUTOSAR
 * @safety SIL2
 */

#include "state_transitions.h"
#include "system_state_machine.h"
#include "safety_manager.h"

/* Module private variables */
static state_transition_table_t g_transition_table[SYSTEM_STATE_MAX][SYSTEM_EVENT_MAX];

/* Module private functions */
static bool State_Transition_Is_Valid(system_state_t current_state, system_event_t event);
static void State_Transition_Execute(system_state_t current_state, system_event_t new_state);

/**
 * @brief Initialize state transition table
 * @return STATE_RESULT_SUCCESS on success, error code otherwise
 */
state_result_t State_Transitions_Init(void)
{
    state_result_t result = STATE_RESULT_SUCCESS;
    uint32_t i = 0U;
    uint32_t j = 0U;
    
    /* Initialize transition table with invalid transitions */
    for (i = 0U; i < SYSTEM_STATE_MAX; i++)
    {
        for (j = 0U; j < SYSTEM_EVENT_MAX; j++)
        {
            g_transition_table[i][j].is_valid = false;
            g_transition_table[i][j].target_state = SYSTEM_STATE_IDLE;
            g_transition_table[i][j].guard_function = NULL;
            g_transition_table[i][j].action_function = NULL;
        }
    }
    
    /* Define valid state transitions */
    /* IDLE state transitions */
    g_transition_table[SYSTEM_STATE_IDLE][SYSTEM_EVENT_START].is_valid = true;
    g_transition_table[SYSTEM_STATE_IDLE][SYSTEM_EVENT_START].target_state = SYSTEM_STATE_READY;
    g_transition_table[SYSTEM_STATE_IDLE][SYSTEM_EVENT_START].guard_function = Safety_Check_System_Ready;
    g_transition_table[SYSTEM_STATE_IDLE][SYSTEM_EVENT_START].action_function = State_Action_Enter_Ready;
    
    g_transition_table[SYSTEM_STATE_IDLE][SYSTEM_EVENT_FAULT].is_valid = true;
    g_transition_table[SYSTEM_STATE_IDLE][SYSTEM_EVENT_FAULT].target_state = SYSTEM_STATE_FAULT;
    g_transition_table[SYSTEM_STATE_IDLE][SYSTEM_EVENT_FAULT].guard_function = NULL;
    g_transition_table[SYSTEM_STATE_IDLE][SYSTEM_EVENT_FAULT].action_function = State_Action_Enter_Fault;
    
    /* READY state transitions */
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_MOVE].is_valid = true;
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_MOVE].target_state = SYSTEM_STATE_MOVING;
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_MOVE].guard_function = Safety_Check_Move_Allowed;
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_MOVE].action_function = State_Action_Enter_Moving;
    
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_DOCK].is_valid = true;
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_DOCK].target_state = SYSTEM_STATE_DOCKING;
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_DOCK].guard_function = Safety_Check_Dock_Allowed;
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_DOCK].action_function = State_Action_Enter_Docking;
    
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_STOP].is_valid = true;
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_STOP].target_state = SYSTEM_STATE_IDLE;
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_STOP].guard_function = NULL;
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_STOP].action_function = State_Action_Enter_Idle;
    
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_FAULT].is_valid = true;
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_FAULT].target_state = SYSTEM_STATE_FAULT;
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_FAULT].guard_function = NULL;
    g_transition_table[SYSTEM_STATE_READY][SYSTEM_EVENT_FAULT].action_function = State_Action_Enter_Fault;
    
    /* MOVING state transitions */
    g_transition_table[SYSTEM_STATE_MOVING][SYSTEM_EVENT_STOP].is_valid = true;
    g_transition_table[SYSTEM_STATE_MOVING][SYSTEM_EVENT_STOP].target_state = SYSTEM_STATE_READY;
    g_transition_table[SYSTEM_STATE_MOVING][SYSTEM_EVENT_STOP].guard_function = NULL;
    g_transition_table[SYSTEM_STATE_MOVING][SYSTEM_EVENT_STOP].action_function = State_Action_Enter_Ready;
    
    g_transition_table[SYSTEM_STATE_MOVING][SYSTEM_EVENT_ESTOP].is_valid = true;
    g_transition_table[SYSTEM_STATE_MOVING][SYSTEM_EVENT_ESTOP].target_state = SYSTEM_STATE_ESTOP;
    g_transition_table[SYSTEM_STATE_MOVING][SYSTEM_EVENT_ESTOP].guard_function = NULL;
    g_transition_table[SYSTEM_STATE_MOVING][SYSTEM_EVENT_ESTOP].action_function = State_Action_Enter_EStop;
    
    g_transition_table[SYSTEM_STATE_MOVING][SYSTEM_EVENT_FAULT].is_valid = true;
    g_transition_table[SYSTEM_STATE_MOVING][SYSTEM_EVENT_FAULT].target_state = SYSTEM_STATE_FAULT;
    g_transition_table[SYSTEM_STATE_MOVING][SYSTEM_EVENT_FAULT].guard_function = NULL;
    g_transition_table[SYSTEM_STATE_MOVING][SYSTEM_EVENT_FAULT].action_function = State_Action_Enter_Fault;
    
    /* DOCKING state transitions */
    g_transition_table[SYSTEM_STATE_DOCKING][SYSTEM_EVENT_DOCKED].is_valid = true;
    g_transition_table[SYSTEM_STATE_DOCKING][SYSTEM_EVENT_DOCKED].target_state = SYSTEM_STATE_DOCKED;
    g_transition_table[SYSTEM_STATE_DOCKING][SYSTEM_EVENT_DOCKED].guard_function = NULL;
    g_transition_table[SYSTEM_STATE_DOCKING][SYSTEM_EVENT_DOCKED].action_function = State_Action_Enter_Docked;
    
    g_transition_table[SYSTEM_STATE_DOCKING][SYSTEM_EVENT_ESTOP].is_valid = true;
    g_transition_table[SYSTEM_STATE_DOCKING][SYSTEM_EVENT_ESTOP].target_state = SYSTEM_STATE_ESTOP;
    g_transition_table[SYSTEM_STATE_DOCKING][SYSTEM_EVENT_ESTOP].guard_function = NULL;
    g_transition_table[SYSTEM_STATE_DOCKING][SYSTEM_EVENT_ESTOP].action_function = State_Action_Enter_EStop;
    
    g_transition_table[SYSTEM_STATE_DOCKING][SYSTEM_EVENT_FAULT].is_valid = true;
    g_transition_table[SYSTEM_STATE_DOCKING][SYSTEM_EVENT_FAULT].target_state = SYSTEM_STATE_FAULT;
    g_transition_table[SYSTEM_STATE_DOCKING][SYSTEM_EVENT_FAULT].guard_function = NULL;
    g_transition_table[SYSTEM_STATE_DOCKING][SYSTEM_EVENT_FAULT].action_function = State_Action_Enter_Fault;
    
    /* DOCKED state transitions */
    g_transition_table[SYSTEM_STATE_DOCKED][SYSTEM_EVENT_UNDOCK].is_valid = true;
    g_transition_table[SYSTEM_STATE_DOCKED][SYSTEM_EVENT_UNDOCK].target_state = SYSTEM_STATE_READY;
    g_transition_table[SYSTEM_STATE_DOCKED][SYSTEM_EVENT_UNDOCK].guard_function = Safety_Check_Undock_Allowed;
    g_transition_table[SYSTEM_STATE_DOCKED][SYSTEM_EVENT_UNDOCK].action_function = State_Action_Enter_Ready;
    
    g_transition_table[SYSTEM_STATE_DOCKED][SYSTEM_EVENT_ESTOP].is_valid = true;
    g_transition_table[SYSTEM_STATE_DOCKED][SYSTEM_EVENT_ESTOP].target_state = SYSTEM_STATE_ESTOP;
    g_transition_table[SYSTEM_STATE_DOCKED][SYSTEM_EVENT_ESTOP].guard_function = NULL;
    g_transition_table[SYSTEM_STATE_DOCKED][SYSTEM_EVENT_ESTOP].action_function = State_Action_Enter_EStop;
    
    g_transition_table[SYSTEM_STATE_DOCKED][SYSTEM_EVENT_FAULT].is_valid = true;
    g_transition_table[SYSTEM_STATE_DOCKED][SYSTEM_EVENT_FAULT].target_state = SYSTEM_STATE_FAULT;
    g_transition_table[SYSTEM_STATE_DOCKED][SYSTEM_EVENT_FAULT].guard_function = NULL;
    g_transition_table[SYSTEM_STATE_DOCKED][SYSTEM_EVENT_FAULT].action_function = State_Action_Enter_Fault;
    
    /* ESTOP state transitions */
    g_transition_table[SYSTEM_STATE_ESTOP][SYSTEM_EVENT_RESET].is_valid = true;
    g_transition_table[SYSTEM_STATE_ESTOP][SYSTEM_EVENT_RESET].target_state = SYSTEM_STATE_IDLE;
    g_transition_table[SYSTEM_STATE_ESTOP][SYSTEM_EVENT_RESET].guard_function = Safety_Check_Reset_Allowed;
    g_transition_table[SYSTEM_STATE_ESTOP][SYSTEM_EVENT_RESET].action_function = State_Action_Enter_Idle;
    
    /* FAULT state transitions */
    g_transition_table[SYSTEM_STATE_FAULT][SYSTEM_EVENT_RESET].is_valid = true;
    g_transition_table[SYSTEM_STATE_FAULT][SYSTEM_EVENT_RESET].target_state = SYSTEM_STATE_IDLE;
    g_transition_table[SYSTEM_STATE_FAULT][SYSTEM_EVENT_RESET].guard_function = Safety_Check_Reset_Allowed;
    g_transition_table[SYSTEM_STATE_FAULT][SYSTEM_EVENT_RESET].action_function = State_Action_Enter_Idle;
    
    return result;
}

/**
 * @brief Process state transition
 * @param current_state Current system state
 * @param event Event that triggered the transition
 * @return STATE_RESULT_SUCCESS on success, error code otherwise
 */
state_result_t State_Transition_Process(system_state_t current_state, system_event_t event)
{
    state_result_t result = STATE_RESULT_SUCCESS;
    
    /* Validate parameters */
    if ((current_state >= SYSTEM_STATE_MAX) || (event >= SYSTEM_EVENT_MAX))
    {
        result = STATE_RESULT_INVALID_PARAMETER;
    }
    else
    {
        /* Check if transition is valid */
        if (true == State_Transition_Is_Valid(current_state, event))
        {
            /* Execute transition */
            State_Transition_Execute(current_state, event);
        }
        else
        {
            result = STATE_RESULT_INVALID_TRANSITION;
        }
    }
    
    return result;
}

/**
 * @brief Check if state transition is valid
 * @param current_state Current system state
 * @param event Event that triggered the transition
 * @return true if transition is valid, false otherwise
 */
static bool State_Transition_Is_Valid(system_state_t current_state, system_event_t event)
{
    bool is_valid = false;
    
    if ((current_state < SYSTEM_STATE_MAX) && (event < SYSTEM_EVENT_MAX))
    {
        is_valid = g_transition_table[current_state][event].is_valid;
        
        /* Check guard function if exists */
        if ((true == is_valid) && (NULL != g_transition_table[current_state][event].guard_function))
        {
            is_valid = g_transition_table[current_state][event].guard_function();
        }
    }
    
    return is_valid;
}

/**
 * @brief Execute state transition
 * @param current_state Current system state
 * @param event Event that triggered the transition
 */
static void State_Transition_Execute(system_state_t current_state, system_event_t event)
{
    system_state_t new_state = g_transition_table[current_state][event].target_state;
    
    /* Execute action function if exists */
    if (NULL != g_transition_table[current_state][event].action_function)
    {
        g_transition_table[current_state][event].action_function();
    }
    
    /* Update system state */
    System_State_Set(new_state);
}

/**
 * @brief Get transition table entry
 * @param state System state
 * @param event System event
 * @return Pointer to transition table entry, NULL if invalid
 */
const state_transition_table_t* State_Transition_Get_Entry(system_state_t state, system_event_t event)
{
    const state_transition_table_t* entry = NULL;
    
    if ((state < SYSTEM_STATE_MAX) && (event < SYSTEM_EVENT_MAX))
    {
        entry = &g_transition_table[state][event];
    }
    
    return entry;
}
