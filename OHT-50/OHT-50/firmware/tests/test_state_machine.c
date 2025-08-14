#include <stdio.h>
#include <assert.h>
#include "../control/state_machine.h"

static void test_basic_flow()
{
	OhtContext ctx; oht_sm_init(&ctx);
	assert(oht_sm_get_state(&ctx) == OHT_STATE_IDLE);
	assert(oht_sm_handle_event(&ctx, OHT_EVENT_START_MOVE) == 0);
	assert(oht_sm_get_state(&ctx) == OHT_STATE_MOVE);
	assert(oht_sm_handle_event(&ctx, OHT_EVENT_START_DOCK) == 0);
	assert(oht_sm_get_state(&ctx) == OHT_STATE_DOCK);
	assert(oht_sm_handle_event(&ctx, OHT_EVENT_STOP) == 0);
	assert(oht_sm_get_state(&ctx) == OHT_STATE_IDLE);
}

static void test_fault_latch_and_clear()
{
	OhtContext ctx; oht_sm_init(&ctx);
	assert(oht_sm_handle_event(&ctx, OHT_EVENT_FAULT) == 0);
	assert(oht_sm_get_state(&ctx) == OHT_STATE_FAULT);
	assert(oht_sm_handle_event(&ctx, OHT_EVENT_CLEAR_FAULT) == 0);
	assert(oht_sm_get_state(&ctx) == OHT_STATE_IDLE);
}

static void test_estop_override()
{
	OhtContext ctx; oht_sm_init(&ctx);
	assert(oht_sm_handle_event(&ctx, OHT_EVENT_START_MOVE) == 0);
	assert(oht_sm_get_state(&ctx) == OHT_STATE_MOVE);
	assert(oht_sm_handle_event(&ctx, OHT_EVENT_ESTOP_ON) == 0);
	assert(oht_sm_get_state(&ctx) == OHT_STATE_ESTOP);
	// Khi đang ESTOP, các event khác không có hiệu lực trừ ESTOP_OFF
	assert(oht_sm_handle_event(&ctx, OHT_EVENT_STOP) < 0);
	assert(oht_sm_handle_event(&ctx, OHT_EVENT_START_MOVE) < 0);
	assert(oht_sm_handle_event(&ctx, OHT_EVENT_ESTOP_OFF) == 0);
	assert(oht_sm_get_state(&ctx) == OHT_STATE_IDLE);
}

int main(void)
{
	test_basic_flow();
	test_fault_latch_and_clear();
	test_estop_override();
	printf("All state machine tests passed.\n");
	return 0;
}


