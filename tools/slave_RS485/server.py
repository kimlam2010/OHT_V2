#!/usr/bin/env python3
import logging
import os
import random
import signal
import threading
import time
from typing import Dict

from pymodbus.datastore import ModbusServerContext, ModbusSlaveContext, ModbusSequentialDataBlock
from pymodbus.server import StartSerialServer
from pymodbus.transaction import ModbusRtuFramer

from modules.motor import build_motor_slave, update_motor_random
from modules.power import build_power_slave, update_power_random
from modules.safety import build_safety_slave, update_safety_random
from modules.dock import build_dock_slave, update_dock_random

logging.basicConfig(level=logging.INFO, format="[%(levelname)s] %(message)s")
logger = logging.getLogger("rs485-sim")
# Reduce pymodbus verbosity (DEBUG -> WARNING)
logging.getLogger("pymodbus").setLevel(logging.WARNING)

DEFAULT_TTY = os.environ.get("RS485_PORT", "/dev/ttyUSB1")
BAUD = int(os.environ.get("RS485_BAUD", "115200"))
PARITY = os.environ.get("RS485_PARITY", "N")
STOPBITS = int(os.environ.get("RS485_STOPBITS", "1"))
BYTESIZE = int(os.environ.get("RS485_BYTESIZE", "8"))

UNIT_MOTOR = int(os.environ.get("UNIT_MOTOR", "1"), 0)
UNIT_POWER = int(os.environ.get("UNIT_POWER", "2"), 0)
UNIT_SAFETY = int(os.environ.get("UNIT_SAFETY", "3"), 0)
UNIT_DOCK = int(os.environ.get("UNIT_DOCK", "5"), 0)

random.seed()


def make_context() -> ModbusServerContext:
    # Build each slave with its own address space
    motor = build_motor_slave()
    power = build_power_slave()
    safety = build_safety_slave()
    dock = build_dock_slave()

    slaves: Dict[int, ModbusSlaveContext] = {
        UNIT_MOTOR: motor,
        UNIT_POWER: power,
        UNIT_SAFETY: safety,
        UNIT_DOCK: dock,
    }
    return ModbusServerContext(slaves=slaves, single=False)


def updater_loop(context: ModbusServerContext, stop_evt: threading.Event):
    while not stop_evt.is_set():
        try:
            update_motor_random(context[UNIT_MOTOR])
            update_power_random(context[UNIT_POWER])
            update_safety_random(context[UNIT_SAFETY])
            update_dock_random(context[UNIT_DOCK])
        except Exception as e:
            logger.warning(f"Updater error: {e}")
        stop_evt.wait(0.2)


def main():
    port = DEFAULT_TTY
    logger.info(f"Starting RS485 Modbus RTU slave simulator on {port} @ {BAUD}bps")
    context = make_context()

    stop_evt = threading.Event()
    updater = threading.Thread(target=updater_loop, args=(context, stop_evt), daemon=True)
    updater.start()

    def _handle_signal(signum, frame):  # noqa: ARG001
        logger.info("Signal received, stopping...")
        stop_evt.set()

    signal.signal(signal.SIGINT, _handle_signal)
    signal.signal(signal.SIGTERM, _handle_signal)

    # Blocking call; returns on process signal
    StartSerialServer(
        context=context,
        framer=ModbusRtuFramer,
        port=port,
        timeout=1,
        baudrate=BAUD,
        parity=PARITY,
        stopbits=STOPBITS,
        bytesize=BYTESIZE,
    )

    # After server exits
    stop_evt.set()
    updater.join(timeout=1.0)


if __name__ == "__main__":
    main()
