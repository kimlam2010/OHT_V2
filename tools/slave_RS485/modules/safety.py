import random
from pymodbus.datastore import ModbusSlaveContext, ModbusSequentialDataBlock

SAFETY_DEVICE_ID_REG           = 0x0100
SAFETY_MODULE_TYPE_REG         = 0x0104
SAFETY_FIRMWARE_VERSION_REG    = 0x0105
SAFETY_HARDWARE_VERSION_REG    = 0x0106

SAFETY_EMERGENCY_STOP_REG      = 0x0001
SAFETY_SAFETY_ZONE_REG         = 0x0002
SAFETY_PROXIMITY_ALERT_REG     = 0x0003
SAFETY_RELAY_OUTPUT_REG        = 0x0004
SAFETY_SYSTEM_TEMP_REG         = 0x0006
SAFETY_SYSTEM_VOLTAGE_REG      = 0x0007
SAFETY_ANALOG_INPUT_1_REG      = 0x0010
SAFETY_ANALOG_INPUT_2_REG      = 0x0011
SAFETY_ANALOG_INPUT_3_REG      = 0x0012
SAFETY_ANALOG_INPUT_4_REG      = 0x0013
SAFETY_DIGITAL_INPUT_REG       = 0x0020
SAFETY_DI1_STATUS_REG          = 0x0021
SAFETY_DI2_STATUS_REG          = 0x0022
SAFETY_DI3_STATUS_REG          = 0x0023
SAFETY_DI4_STATUS_REG          = 0x0024
SAFETY_RELAY_CONTROL_REG       = 0x0030
SAFETY_RELAY_1_CONTROL_REG     = 0x0031
SAFETY_RELAY_2_CONTROL_REG     = 0x0032
SAFETY_RELAY_3_CONTROL_REG     = 0x0033
SAFETY_RELAY_4_CONTROL_REG     = 0x0034
SAFETY_ZONE_1_THRESHOLD_REG    = 0x0040
SAFETY_ZONE_2_THRESHOLD_REG    = 0x0041
SAFETY_ZONE_3_THRESHOLD_REG    = 0x0042
SAFETY_ZONE_4_THRESHOLD_REG    = 0x0043
SAFETY_PROXIMITY_THRESHOLD_REG = 0x0050
SAFETY_RESPONSE_TIME_REG       = 0x0051
SAFETY_AUTO_RESET_ENABLE_REG   = 0x0052
SAFETY_SAFETY_MODE_REG         = 0x0053
SAFETY_SYSTEM_STATUS_REG       = 0x0107
SAFETY_SYSTEM_ERROR_REG        = 0x0108
SAFETY_RESET_ERROR_CMD_REG     = 0x0109
SAFETY_MODULE_NAME_LOW_REG     = 0x00F8
SAFETY_MODULE_NAME_HIGH_REG    = 0x00F9


def build_safety_slave() -> ModbusSlaveContext:
    size = 0x0200
    hr = [0] * size

    hr[SAFETY_DEVICE_ID_REG] = 0x0003
    hr[SAFETY_MODULE_TYPE_REG] = 0x0003
    hr[SAFETY_FIRMWARE_VERSION_REG] = 0x0101
    hr[SAFETY_HARDWARE_VERSION_REG] = 0x0101
    # Defaults for thresholds
    hr[SAFETY_ZONE_1_THRESHOLD_REG] = 1000
    hr[SAFETY_ZONE_2_THRESHOLD_REG] = 500
    hr[SAFETY_ZONE_3_THRESHOLD_REG] = 200
    hr[SAFETY_ZONE_4_THRESHOLD_REG] = 100
    hr[SAFETY_PROXIMITY_THRESHOLD_REG] = 50

    return ModbusSlaveContext(
        di=ModbusSequentialDataBlock(0, [0]*size),
        co=ModbusSequentialDataBlock(0, [0]*size),
        hr=ModbusSequentialDataBlock(0, hr),
        ir=ModbusSequentialDataBlock(0, [0]*size),
        zero_mode=True,
    )


def _write(ctx: ModbusSlaveContext, addr: int, val: int):
    ctx.setValues(3, addr, [val & 0xFFFF])


def update_safety_random(ctx: ModbusSlaveContext):
    temp = max(150, min(600, 250 + random.randint(-5, 5)))   # 15.0..60.0C x10
    volt = max(180, min(300, 240 + random.randint(-5, 5)))
    analog = [random.randint(50, 5000) for _ in range(4)]  # distance mm per header
    digital = random.randint(0, 0x0F)
    zones = 0
    prox = 1 if any(v < 100 for v in analog) else 0
    estop = 0

    _write(ctx, SAFETY_SYSTEM_TEMP_REG, temp)
    _write(ctx, SAFETY_SYSTEM_VOLTAGE_REG, volt)
    _write(ctx, SAFETY_ANALOG_INPUT_1_REG, analog[0])
    _write(ctx, SAFETY_ANALOG_INPUT_2_REG, analog[1])
    _write(ctx, SAFETY_ANALOG_INPUT_3_REG, analog[2])
    _write(ctx, SAFETY_ANALOG_INPUT_4_REG, analog[3])
    _write(ctx, SAFETY_DIGITAL_INPUT_REG, digital)
    _write(ctx, SAFETY_DI1_STATUS_REG, (digital >> 0) & 1)
    _write(ctx, SAFETY_DI2_STATUS_REG, (digital >> 1) & 1)
    _write(ctx, SAFETY_DI3_STATUS_REG, (digital >> 2) & 1)
    _write(ctx, SAFETY_DI4_STATUS_REG, (digital >> 3) & 1)
    _write(ctx, SAFETY_SAFETY_ZONE_REG, zones)
    _write(ctx, SAFETY_PROXIMITY_ALERT_REG, prox)
    _write(ctx, SAFETY_EMERGENCY_STOP_REG, estop)
    # Relay
    relays = random.randint(0, 0x0F)
    _write(ctx, SAFETY_RELAY_CONTROL_REG, relays)
    _write(ctx, SAFETY_RELAY_1_CONTROL_REG, (relays >> 0) & 1)
    _write(ctx, SAFETY_RELAY_2_CONTROL_REG, (relays >> 1) & 1)
    _write(ctx, SAFETY_RELAY_3_CONTROL_REG, (relays >> 2) & 1)
    _write(ctx, SAFETY_RELAY_4_CONTROL_REG, (relays >> 3) & 1)
    # System
    _write(ctx, SAFETY_SYSTEM_STATUS_REG, 0)
    _write(ctx, SAFETY_SYSTEM_ERROR_REG, 0)
    if ctx.getValues(3, SAFETY_MODULE_NAME_LOW_REG, 1)[0] == 0:
        _write(ctx, SAFETY_MODULE_NAME_LOW_REG, 0x5930)
        _write(ctx, SAFETY_MODULE_NAME_HIGH_REG, 0x5330)
