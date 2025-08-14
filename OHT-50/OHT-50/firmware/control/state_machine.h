#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef enum {
	OHT_STATE_IDLE = 0,
	OHT_STATE_MOVE,
	OHT_STATE_DOCK,
	OHT_STATE_FAULT,
	OHT_STATE_ESTOP
} OhtState;

typedef enum {
	OHT_EVENT_NONE = 0,
	OHT_EVENT_START_MOVE,
	OHT_EVENT_START_DOCK,
	OHT_EVENT_STOP,
	OHT_EVENT_FAULT,
	OHT_EVENT_CLEAR_FAULT,
	OHT_EVENT_ESTOP_ON,
	OHT_EVENT_ESTOP_OFF
} OhtEvent;

typedef struct {
	OhtState state;
	bool estop_active;
	bool fault_latched;
} OhtContext;

void oht_sm_init(OhtContext *ctx);
OhtState oht_sm_get_state(const OhtContext *ctx);

// Trả 0 nếu xử lý thành công; <0 nếu event không hợp lệ cho trạng thái hiện tại
int oht_sm_handle_event(OhtContext *ctx, OhtEvent event);


