#include <stdio.h>
#include <assert.h>
#include "../safety/safety_manager.h"

int main(void)
{
	SafetyManager sm;
	safety_init(&sm, 1000, 50);
	assert(!safety_estop_is_active(&sm));
	assert(!safety_watchdog_violation(&sm, 1020));
	// Cập nhật với ESTOP bật
	safety_update(&sm, 1030, true);
	assert(safety_estop_is_active(&sm));
	// Watchdog vi phạm nếu quá hạn
	assert(safety_watchdog_violation(&sm, 1100) == true);
	printf("Safety manager tests passed.\n");
	return 0;
}


