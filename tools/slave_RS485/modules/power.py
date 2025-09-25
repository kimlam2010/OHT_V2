import random
from pymodbus.datastore import ModbusSlaveContext, ModbusSequentialDataBlock

# From power_module_handler.h
POWER_REG_BATTERY_VOLTAGE      = 0x0000
POWER_REG_BATTERY_CURRENT      = 0x0001
POWER_REG_BATTERY_SOC          = 0x0002
POWER_REG_TEMPERATURE          = 0x0008
POWER_REG_V_OUT_12V            = 0x0040
POWER_REG_I_OUT_12V            = 0x0041
POWER_REG_P_OUT_12V            = 0x0042
POWER_REG_RL_12V               = 0x0049
POWER_REG_FAULT_FLAGS          = 0x0023
POWER_REG_DEVICE_ID            = 0x0100
POWER_REG_MODULE_TYPE          = 0x0104
POWER_REG_FIRMWARE_VERSION     = 0x0105
POWER_REG_HARDWARE_VERSION     = 0x0106
# Additional subsets
POWER_REG_MAX_CELL_V           = 0x0003
POWER_REG_MIN_CELL_V           = 0x0004
POWER_REG_MAX_CELL_INDEX       = 0x0005
POWER_REG_MIN_CELL_INDEX       = 0x0006
POWER_REG_CELL_DIFF            = 0x0007
POWER_REG_CONNECTION_STATUS    = 0x0009
POWER_REG_CHARGE_STATUS        = 0x0011
POWER_REG_DISCHARGE_STATUS     = 0x0012
POWER_REG_V_SET                = 0x0030
POWER_REG_I_SET                = 0x0031
POWER_REG_V_OUT                = 0x0032
POWER_REG_I_OUT                = 0x0033
POWER_REG_P_OUT                = 0x0034
POWER_REG_STATUS               = 0x003B
POWER_REG_ON_OFF               = 0x003C
POWER_REG_CHARGE_RELAY         = 0x003D
POWER_REG_CHARGE_STATE         = 0x003E
POWER_REG_CHARGE_REQUEST       = 0x003F
POWER_REG_RL_5V                = 0x004A
POWER_REG_RL_3V3               = 0x004B
POWER_REG_RL_FAULT             = 0x004C
POWER_REG_USE_V_THS            = 0x004D
POWER_REG_SYSTEM_STATUS        = 0x0107
POWER_REG_SYSTEM_ERROR         = 0x0108
POWER_REG_RESET_ERROR_CMD      = 0x0109
POWER_REG_MODULE_NAME_LOW      = 0x00F8
POWER_REG_MODULE_NAME_HIGH     = 0x00F9


def build_power_slave() -> ModbusSlaveContext:
    size = 0x0200
    hr = [0] * size
    ir = [0] * size

    hr[POWER_REG_DEVICE_ID] = 0x0002
    hr[POWER_REG_MODULE_TYPE] = 0x0002
    hr[POWER_REG_FIRMWARE_VERSION] = 0x0101
    hr[POWER_REG_HARDWARE_VERSION] = 0x0101

    return ModbusSlaveContext(
        di=ModbusSequentialDataBlock(0, [0]*size),
        co=ModbusSequentialDataBlock(0, [0]*size),
        hr=ModbusSequentialDataBlock(0, hr),
        ir=ModbusSequentialDataBlock(0, ir),
        zero_mode=True,
    )


def _write_hr(ctx: ModbusSlaveContext, addr: int, val: int):
    ctx.setValues(3, addr, [val & 0xFFFF])

def _read_hr(ctx: ModbusSlaveContext, addr: int) -> int:
    return ctx.getValues(3, addr, count=1)[0]


def update_power_random(ctx: ModbusSlaveContext):
    # Battery voltage 18.0..30.0V (x10), current 0..100.0A (x10)
    vbat = max(180, min(300, 240 + random.randint(-5, 5)))
    ibat = max(0, min(1000, 50 + random.randint(-20, 20)))
    soc  = max(0, min(100, _read_hr(ctx, POWER_REG_BATTERY_SOC) + random.randint(-1, 1)))
    temp = max(150, min(600, 250 + random.randint(-3, 3)))   # 15.0..60.0C x10

    v12 = max(110, min(130, 120 + random.randint(-2, 2)))
    i12 = max(0, min(200, 10 + random.randint(-3, 3)))
    p12 = int(v12 * i12 / 10)

    fault = 0

    _write_hr(ctx, POWER_REG_BATTERY_VOLTAGE, vbat)
    _write_hr(ctx, POWER_REG_BATTERY_CURRENT, ibat)
    _write_hr(ctx, POWER_REG_BATTERY_SOC, soc)
    _write_hr(ctx, POWER_REG_TEMPERATURE, temp)
    # Identity fields (once)
    if _read_hr(ctx, POWER_REG_MODULE_NAME_LOW) == 0:
        _write_hr(ctx, POWER_REG_MODULE_NAME_LOW, 0x3052)   # dummy
        _write_hr(ctx, POWER_REG_MODULE_NAME_HIGH, 0x5730)  # dummy
    # Cell stats
    _write_hr(ctx, POWER_REG_MAX_CELL_V, random.randint(3400, 4200))
    _write_hr(ctx, POWER_REG_MIN_CELL_V, random.randint(3300, 4100))
    _write_hr(ctx, POWER_REG_MAX_CELL_INDEX, random.randint(0, 5))
    _write_hr(ctx, POWER_REG_MIN_CELL_INDEX, random.randint(0, 5))
    _write_hr(ctx, POWER_REG_CELL_DIFF, random.randint(0, 200))
    _write_hr(ctx, POWER_REG_CONNECTION_STATUS, 1)
    _write_hr(ctx, POWER_REG_CHARGE_STATUS, random.randint(0, 1))
    _write_hr(ctx, POWER_REG_DISCHARGE_STATUS, random.randint(0, 1))

    _write_hr(ctx, POWER_REG_V_OUT_12V, v12)
    _write_hr(ctx, POWER_REG_I_OUT_12V, i12)
    _write_hr(ctx, POWER_REG_P_OUT_12V, p12)
    _write_hr(ctx, POWER_REG_RL_12V, 1)
    _write_hr(ctx, POWER_REG_RL_5V, 1)
    _write_hr(ctx, POWER_REG_RL_3V3, 1)
    _write_hr(ctx, POWER_REG_RL_FAULT, 0)
    _write_hr(ctx, POWER_REG_USE_V_THS, 118)
    _write_hr(ctx, POWER_REG_FAULT_FLAGS, fault)
    # SK60X
    _write_hr(ctx, POWER_REG_V_SET, 126)
    _write_hr(ctx, POWER_REG_I_SET, 50)
    _write_hr(ctx, POWER_REG_V_OUT, v12)
    _write_hr(ctx, POWER_REG_I_OUT, i12)
    _write_hr(ctx, POWER_REG_P_OUT, p12)
    _write_hr(ctx, POWER_REG_STATUS, 1)
    _write_hr(ctx, POWER_REG_ON_OFF, 1)
    _write_hr(ctx, POWER_REG_CHARGE_RELAY, random.randint(0, 1))
    _write_hr(ctx, POWER_REG_CHARGE_STATE, random.randint(0, 2))
    _write_hr(ctx, POWER_REG_CHARGE_REQUEST, random.randint(0, 1))
    # System
    _write_hr(ctx, POWER_REG_SYSTEM_STATUS, 0)
    _write_hr(ctx, POWER_REG_SYSTEM_ERROR, 0)
