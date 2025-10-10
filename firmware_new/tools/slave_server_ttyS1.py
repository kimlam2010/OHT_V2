#!/usr/bin/env python3
"""
🔧 SLAVE SERVER TRÊN /dev/ttyS1 - Cùng bus với firmware
"""

import sys
sys.path.insert(0, '/home/orangepi/Desktop/OHT_V2/tools/slave_RS485')

from pymodbus.server import StartSerialServer
from pymodbus.device import ModbusDeviceIdentification
from pymodbus.datastore import ModbusSequentialDataBlock, ModbusSlaveContext, ModbusServerContext
import logging

# Setup logging
logging.basicConfig(level=logging.DEBUG)
log = logging.getLogger()

print("\n╔═══════════════════════════════════════════════════════════╗")
print("║   🔧 SLAVE SERVER ON /dev/ttyS1 (ttyOHT485)            ║")
print("╚═══════════════════════════════════════════════════════════╝\n")

# Create datastore with data at register 0x0100 (256)
store = ModbusSlaveContext(
    di=ModbusSequentialDataBlock(0, [0]*1000),
    co=ModbusSequentialDataBlock(0, [0]*1000),
    hr=ModbusSequentialDataBlock(0, [0]*1000),  # Holding registers
    ir=ModbusSequentialDataBlock(0, [0]*1000)
)

# Set data at register 0x0100 (Device ID)
store.setValues(3, 0x0100, [0x0002])  # Device ID = 2
store.setValues(3, 0x0101, [0x0001])  # Hardware Version
store.setValues(3, 0x0102, [0x0100])  # Firmware Version
store.setValues(3, 0x0103, [0x1234])  # Serial Number
store.setValues(3, 0x0104, [0x0001])  # Module Type (Power)
store.setValues(3, 0x0105, [0x0000])  # Status
store.setValues(3, 0x0106, [0x0000])  # Error Code
store.setValues(3, 0x0107, [0xFFFF])  # Capabilities

# Also set data at register 0x0000 for compatibility (Battery data 0x0000-0x000A)
store.setValues(3, 0x0000, [238, 65, 3, 3801, 3725, 4, 3, 140, 247, 1, 0])

# Set Cell Voltages (0x0014-0x0019) - 6 registers
store.setValues(3, 0x0014, [3420, 3435, 3448, 3462, 3471, 3489])

# Set Power Distribution registers (0x001C-0x0021) - 6 registers
store.setValues(3, 0x001C, [1200, 500, 1180, 495, 1195, 502])

# Set additional power registers (0x0040-0x004D)
store.setValues(3, 0x0040, [12000, 5000, 24000, 15000])  # 12V, 5V, 24V, 15V rails
store.setValues(3, 0x0044, [11950, 4980])  # Secondary measurements
store.setValues(3, 0x0046, [23800, 14950])  # Tertiary measurements
store.setValues(3, 0x0049, [1, 0, 0, 0])  # Control registers
store.setValues(3, 0x004D, [118])  # Voltage threshold

context = ModbusServerContext(slaves={2: store}, single=False)

# Identity
identity = ModbusDeviceIdentification()
identity.VendorName = 'OHT-50'
identity.ProductCode = 'POWER-MODULE'
identity.VendorUrl = 'http://oht50.local'
identity.ProductName = 'Power Module Simulator'
identity.ModelName = 'PM-001'
identity.MajorMinorRevision = '1.0.0'

PORT = "/dev/ttyS1"  # SAME AS FIRMWARE!
BAUDRATE = 115200

print(f"🔌 Port: {PORT}")
print(f"⚡ Baudrate: {BAUDRATE}")
print(f"🏷️  Slave Address: 2")
print(f"📊 Register 0x0100 = 0x0002 (Device ID)")
print(f"📊 Register 0x0000-0x0009 = [238, 65, 3, ...]")
print(f"\n✅ Starting Modbus RTU Slave Server...")
print(f"🔄 Firmware can now scan and discover this slave!\n")

try:
    StartSerialServer(
        context=context,
        identity=identity,
        port=PORT,
        baudrate=BAUDRATE,
        bytesize=8,
        parity='N',
        stopbits=1,
        timeout=1
    )
except KeyboardInterrupt:
    print("\n\n⚠️  Server stopped by user")
except Exception as e:
    print(f"\n❌ Error: {e}")
    import traceback
    traceback.print_exc()

