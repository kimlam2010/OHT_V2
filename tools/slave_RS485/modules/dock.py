import random
from pymodbus.datastore import ModbusSlaveContext, ModbusSequentialDataBlock

DOCK_DEVICE_ID_REG                = 0x0100
DOCK_MODULE_TYPE_REG              = 0x0104
DOCK_FIRMWARE_VERSION_REG         = 0x0105
DOCK_HARDWARE_VERSION_REG         = 0x0106

DOCK_POSITION_TARGET_REG          = 0x7000
DOCK_CURRENT_POSITION_REG         = 0x7001
DOCK_STATUS_REG                   = 0x7005
DOCK_DISTANCE_TO_DOCK_REG         = 0x7006
DOCK_ALIGNMENT_ANGLE_REG          = 0x7007

DOCK_RFID_TAG_ID_LOW_REG          = 0x7100
DOCK_RFID_TAG_ID_HIGH_REG         = 0x7101
DOCK_RFID_SIGNAL_STRENGTH_REG     = 0x7102
DOCK_RFID_READ_STATUS_REG         = 0x7103

DOCK_ACCEL_X_REG                  = 0x7200
DOCK_ACCEL_Y_REG                  = 0x7201
DOCK_ACCEL_Z_REG                  = 0x7202
DOCK_ACCEL_TEMPERATURE_REG        = 0x7203
DOCK_ACCEL_STATUS_REG             = 0x7204

DOCK_PROX_SENSOR_1_REG            = 0x7300
DOCK_PROX_SENSOR_2_REG            = 0x7301
DOCK_PROX_SENSOR_1_DISTANCE_REG   = 0x7302
DOCK_PROX_SENSOR_2_DISTANCE_REG   = 0x7303
DOCK_DOCK_CONFIRMED_REG           = 0x7304
DOCK_DEVICE_ID_REG                = 0x0100
DOCK_CONFIG_BAUDRATE_REG          = 0x0101
DOCK_CONFIG_PARITY_REG            = 0x0102
DOCK_CONFIG_STOP_BITS_REG         = 0x0103
DOCK_MODULE_TYPE_REG              = 0x0104
DOCK_FIRMWARE_VERSION_REG         = 0x0105
DOCK_HARDWARE_VERSION_REG         = 0x0106
DOCK_SYSTEM_STATUS_REG            = 0x0107
DOCK_SYSTEM_ERROR_REG             = 0x0108
DOCK_RESET_ERROR_CMD_REG          = 0x0109
DOCK_CONFIG_APPROACH_DISTANCE_REG = 0xA000
DOCK_CONFIG_FINAL_SPEED_REG       = 0xA001
DOCK_CONFIG_ACCURACY_REG          = 0xA002
DOCK_CONFIG_TIMEOUT_REG           = 0xA003
DOCK_CONFIG_RETRY_COUNT_REG       = 0xA004
DOCK_CONFIG_DEBOUNCE_TIME_REG     = 0xA005
DOCK_CONFIG_ALIGNMENT_TOLERANCE_REG = 0xA006


def build_dock_slave() -> ModbusSlaveContext:
    size = 0xA100  # cover up to 0xA0xx
    hr = [0] * size

    hr[DOCK_DEVICE_ID_REG] = 0x0005
    hr[DOCK_MODULE_TYPE_REG] = 0x0005
    hr[DOCK_FIRMWARE_VERSION_REG] = 0x0101
    hr[DOCK_HARDWARE_VERSION_REG] = 0x0101
    # Provide non-zero module name/version markers at 0x00F8.. to satisfy FW readers
    # Two 16-bit words could encode ASCII like 'Y0' 'S0' or any non-zero signature
    hr[0x00F8] = 0x5930  # 'Y0'
    hr[0x00F9] = 0x5330  # 'S0'
    # Defaults for config
    hr[DOCK_CONFIG_APPROACH_DISTANCE_REG] = 500
    hr[DOCK_CONFIG_FINAL_SPEED_REG] = 50
    hr[DOCK_CONFIG_ACCURACY_REG] = 2
    hr[DOCK_CONFIG_TIMEOUT_REG] = 30
    hr[DOCK_CONFIG_RETRY_COUNT_REG] = 3
    hr[DOCK_CONFIG_DEBOUNCE_TIME_REG] = 100
    hr[DOCK_CONFIG_ALIGNMENT_TOLERANCE_REG] = 50

    return ModbusSlaveContext(
        di=ModbusSequentialDataBlock(0, [0]*size),
        co=ModbusSequentialDataBlock(0, [0]*size),
        hr=ModbusSequentialDataBlock(0, hr),
        ir=ModbusSequentialDataBlock(0, [0]*size),
        zero_mode=True,
    )


def _w(ctx: ModbusSlaveContext, a: int, v: int):
    ctx.setValues(3, a, [v & 0xFFFF])


def update_dock_random(ctx: ModbusSlaveContext):
    pos = (ctx.getValues(3, DOCK_CURRENT_POSITION_REG, count=1)[0] + random.randint(-2, 3)) & 0xFFFF
    dist = max(0, min(2000, ctx.getValues(3, DOCK_DISTANCE_TO_DOCK_REG, count=1)[0] + random.randint(-10, 10)))
    angle = max(0, min(200, ctx.getValues(3, DOCK_ALIGNMENT_ANGLE_REG, count=1)[0] + random.randint(-2, 2)))  # 0..20.0 deg x10

    tag_low = random.randint(0, 0xFFFF)
    tag_high = random.randint(0, 0xFFFF)
    rssi = random.randint(0, 100)
    read_ok = random.choice([0, 1])

    ax = random.randint(-2000, 2000) & 0xFFFF  # mg
    ay = random.randint(-2000, 2000) & 0xFFFF
    az = random.randint(-2000, 2000) & 0xFFFF
    at = max(150, min(600, 250 + random.randint(-5, 5)))
    ast = 1

    s1 = random.choice([0, 1])
    s2 = random.choice([0, 1])
    d1 = random.randint(0, 1000)
    d2 = random.randint(0, 1000)
    docked = 1 if s1 and s2 and dist < 50 else 0
    status = 0x08 if docked else 0x01

    _w(ctx, DOCK_CURRENT_POSITION_REG, pos)
    _w(ctx, DOCK_DISTANCE_TO_DOCK_REG, dist)
    _w(ctx, DOCK_ALIGNMENT_ANGLE_REG, angle)

    _w(ctx, DOCK_RFID_TAG_ID_LOW_REG, tag_low)
    _w(ctx, DOCK_RFID_TAG_ID_HIGH_REG, tag_high)
    _w(ctx, DOCK_RFID_SIGNAL_STRENGTH_REG, rssi)
    _w(ctx, DOCK_RFID_READ_STATUS_REG, read_ok)

    _w(ctx, DOCK_ACCEL_X_REG, ax)
    _w(ctx, DOCK_ACCEL_Y_REG, ay)
    _w(ctx, DOCK_ACCEL_Z_REG, az)
    _w(ctx, DOCK_ACCEL_TEMPERATURE_REG, at)
    _w(ctx, DOCK_ACCEL_STATUS_REG, ast)

    _w(ctx, DOCK_PROX_SENSOR_1_REG, s1)
    _w(ctx, DOCK_PROX_SENSOR_2_REG, s2)
    _w(ctx, DOCK_PROX_SENSOR_1_DISTANCE_REG, d1)
    _w(ctx, DOCK_PROX_SENSOR_2_DISTANCE_REG, d2)
    _w(ctx, DOCK_DOCK_CONFIRMED_REG, docked)
    _w(ctx, DOCK_STATUS_REG, status)
    _w(ctx, DOCK_SYSTEM_STATUS_REG, 0)
    _w(ctx, DOCK_SYSTEM_ERROR_REG, 0)
