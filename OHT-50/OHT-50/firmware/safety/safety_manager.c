#include "safety_manager.h"

void safety_init(SafetyManager *sm, uint32_t now_ms, uint32_t max_interval_ms)
{
	sm->estop_input_active = false;
	sm->last_update_ms = now_ms;
	sm->max_update_interval_ms = max_interval_ms;
}

void safety_update(SafetyManager *sm, uint32_t now_ms, bool estop_input)
{
	sm->last_update_ms = now_ms;
	sm->estop_input_active = estop_input;
}

bool safety_estop_is_active(const SafetyManager *sm)
{
	return sm->estop_input_active;
}

bool safety_watchdog_violation(const SafetyManager *sm, uint32_t now_ms)
{
	return (now_ms - sm->last_update_ms) > sm->max_update_interval_ms;
}


