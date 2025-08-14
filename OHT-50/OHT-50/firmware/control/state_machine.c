#include "state_machine.h"

static void reset_faults(OhtContext *ctx)
{
	ctx->fault_latched = false;
}

void oht_sm_init(OhtContext *ctx)
{
	ctx->state = OHT_STATE_IDLE;
	ctx->estop_active = false;
	ctx->fault_latched = false;
}

OhtState oht_sm_get_state(const OhtContext *ctx)
{
	return ctx->state;
}

int oht_sm_handle_event(OhtContext *ctx, OhtEvent event)
{
	// Luật an toàn: ESTOP khi bật thì override mọi trạng thái → vào ESTOP
	if (event == OHT_EVENT_ESTOP_ON) {
		ctx->estop_active = true;
		ctx->state = OHT_STATE_ESTOP;
		return 0;
	}
	// Khi đang ESTOP, chỉ chấp nhận E‑STOP OFF và sau đó về IDLE (không tự động tiếp tục MOVE/DOCK)
	if (ctx->estop_active) {
		if (event == OHT_EVENT_ESTOP_OFF) {
			ctx->estop_active = false;
			ctx->state = OHT_STATE_IDLE;
			return 0;
		}
		return -1;
	}

	// Fault sự cố: có thể xảy ra ở mọi trạng thái (trừ khi ESTOP đang active đã xử lý ở trên)
	if (event == OHT_EVENT_FAULT) {
		ctx->fault_latched = true;
		ctx->state = OHT_STATE_FAULT;
		return 0;
	}

	switch (ctx->state) {
	case OHT_STATE_IDLE:
		if (event == OHT_EVENT_START_MOVE) { ctx->state = OHT_STATE_MOVE; return 0; }
		if (event == OHT_EVENT_START_DOCK) { ctx->state = OHT_STATE_DOCK; return 0; }
		if (event == OHT_EVENT_STOP) { return 0; } // no-op
		if (event == OHT_EVENT_CLEAR_FAULT) { reset_faults(ctx); return 0; }
		break;
	case OHT_STATE_MOVE:
		if (event == OHT_EVENT_STOP) { ctx->state = OHT_STATE_IDLE; return 0; }
		if (event == OHT_EVENT_START_DOCK) { ctx->state = OHT_STATE_DOCK; return 0; }
		break;
	case OHT_STATE_DOCK:
		if (event == OHT_EVENT_STOP) { ctx->state = OHT_STATE_IDLE; return 0; }
		break;
	case OHT_STATE_FAULT:
		if (event == OHT_EVENT_CLEAR_FAULT) { reset_faults(ctx); ctx->state = OHT_STATE_IDLE; return 0; }
		break;
	case OHT_STATE_ESTOP:
		// đã xử lý ở đầu hàm; đến đây coi như invalid
		break;
	}
	return -1;
}


