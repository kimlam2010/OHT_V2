import random
from pymodbus.datastore import ModbusSlaveContext, ModbusSequentialDataBlock

# Register map aliases (from travel_motor_module_handler.h)
MOTOR_ENABLE_REG                = 0x0000
MOTOR_MODE_REG                  = 0x0001
MOTOR_SPEED_TARGET_REG          = 0x0002
MOTOR_SPEED_ACTUAL_REG          = 0x0003
MOTOR_POSITION_TARGET_REG       = 0x0004
MOTOR_POSITION_ACTUAL_REG       = 0x0005
MOTOR_TEMPERATURE_REG           = 0x000B
MOTOR_VOLTAGE_REG               = 0x000C
MOTOR_CURRENT_REG               = 0x000D
MOTOR_FAULT_STATUS_REG          = 0x000E
MOTOR_OPERATION_STATUS_REG      = 0x000F

MOTOR_RUNNING_STATUS_REG        = 0x0010
MOTOR_READY_STATUS_REG          = 0x0011
MOTOR_EMERGENCY_STOP_REG        = 0x0014
MOTOR_POSITION_STATUS_REG       = 0x001E

MOTOR_DEVICE_ID_REG             = 0x0100
MOTOR_MODULE_TYPE_REG           = 0x0104
MOTOR_FIRMWARE_VERSION_REG      = 0x0105
MOTOR_HARDWARE_VERSION_REG      = 0x0106


def build_motor_slave() -> ModbusSlaveContext:
    # Allocate 0..0x0200 space for holding/input regs
    size = 0x0200
    hr = [0] * size
    ir = [0] * size

    # Static identity
    hr[MOTOR_DEVICE_ID_REG] = 0x0004
    hr[MOTOR_MODULE_TYPE_REG] = 0x0004
    hr[MOTOR_FIRMWARE_VERSION_REG] = 0x0101
    hr[MOTOR_HARDWARE_VERSION_REG] = 0x0101

    # Defaults
    hr[MOTOR_ENABLE_REG] = 0
    hr[MOTOR_MODE_REG] = 0
    hr[MOTOR_SPEED_TARGET_REG] = 1000
    hr[MOTOR_POSITION_TARGET_REG] = 0

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


def update_motor_random(ctx: ModbusSlaveContext):
    # Ranges per travel_motor_module_handler.h
    # speed 0..10000, position 0..65535, temp 0..1000 (x10C), voltage 0..500 (x10V), current 0..1000 (x10A)
    speed_prev = _read_hr(ctx, MOTOR_SPEED_ACTUAL_REG)
    speed = max(0, min(10000, speed_prev + random.randint(-50, 50)))

    pos_prev = _read_hr(ctx, MOTOR_POSITION_ACTUAL_REG)
    pos = (pos_prev + random.randint(-5, 5)) & 0xFFFF

    temp = max(200, min(900, 250 + random.randint(-5, 5)))  # 20.0..90.0 C
    volt = max(180, min(300, 240 + random.randint(-5, 5)))  # 18.0..30.0 V
    curr = max(0, min(500, 100 + random.randint(-10, 10)))  # 0..50.0 A
    fault = 0
    ready = 1
    running = 1 if speed > 0 else 0

    _write_hr(ctx, MOTOR_SPEED_ACTUAL_REG, speed)
    _write_hr(ctx, MOTOR_POSITION_ACTUAL_REG, pos)
    _write_hr(ctx, MOTOR_TEMPERATURE_REG, temp)
    _write_hr(ctx, MOTOR_VOLTAGE_REG, volt)
    _write_hr(ctx, MOTOR_CURRENT_REG, curr)
    _write_hr(ctx, MOTOR_FAULT_STATUS_REG, fault)
    _write_hr(ctx, MOTOR_OPERATION_STATUS_REG, 1)
    _write_hr(ctx, MOTOR_RUNNING_STATUS_REG, running)
    _write_hr(ctx, MOTOR_READY_STATUS_REG, ready)
    _write_hr(ctx, MOTOR_POSITION_STATUS_REG, 1 if pos == _read_hr(ctx, MOTOR_POSITION_TARGET_REG) else 0)
    _write_hr(ctx, MOTOR_EMERGENCY_STOP_REG, 0)
