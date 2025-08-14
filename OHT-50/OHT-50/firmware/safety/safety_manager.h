#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct {
	bool estop_input_active;     // đọc từ GPIO an toàn
	uint32_t last_update_ms;     // thời điểm cuối cập nhật
	uint32_t max_update_interval_ms; // watchdog đơn giản cho safety loop
} SafetyManager;

void safety_init(SafetyManager *sm, uint32_t now_ms, uint32_t max_interval_ms);
void safety_update(SafetyManager *sm, uint32_t now_ms, bool estop_input);
bool safety_estop_is_active(const SafetyManager *sm);
bool safety_watchdog_violation(const SafetyManager *sm, uint32_t now_ms);


