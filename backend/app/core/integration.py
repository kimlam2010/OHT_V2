"""
RS485 and firmware integration for OHT-50 Backend
"""

import asyncio
import logging
import time
from typing import Optional, Dict, Any, List
from pymodbus.client import ModbusSerialClient
from pymodbus.exceptions import ModbusException

from app.config import settings
from app.core.exceptions import RS485CommunicationException
from app.core.monitoring import metrics_collector

logger = logging.getLogger(__name__)


class RS485Service:
    """RS485 communication service using Modbus RTU"""
    
    def __init__(self, use_mock: bool = False):
        self.use_mock = use_mock
        self.client = None
        self.connected = False
        self.mock_data = self._initialize_mock_data()
        
        if not self.use_mock:
            self.client = ModbusSerialClient(
                port=settings.rs485_port,
                baudrate=settings.rs485_baud_rate,
                timeout=settings.rs485_timeout,
                retries=3
            )
        
    def _initialize_mock_data(self) -> Dict[int, Dict[int, int]]:
        """Initialize mock data for testing"""
        return {
            # Travel Motor Module (Address 1)
            1: {
                0x0001: 1001,  # module_id: Travel Motor
                30001: 1,  # motor_status: Running
                30002: 1500,  # motor_speed_feedback: 1500 RPM
                30003: 2,  # motor_current: 2A
                30004: 24,  # motor_voltage: 24V
                30005: 45,  # motor_temperature: 45°C
                30006: 1000,  # motor_position: 1000mm
                30007: 0,  # motor_error_code: No error
                30008: 0,  # motor_error_message: No error
            },
            # Dock Location Module (Address 2)
            2: {
                0x0001: 1002,  # module_id: Dock Location
                30001: 0,  # dock_status: Idle
                30002: 150,  # dock_position_x: 150mm
                30003: 200,  # dock_position_y: 200mm
                30004: 0,  # dock_orientation: 0°
                30005: 5,  # dock_accuracy: 5mm
                30006: 0,  # dock_error_code: No error
                30007: 12345,  # nfc_tag_id: 12345
                30008: 15,  # dock_sensor_status: All sensors active
            },
            # Safety Module (Address 3)
            3: {
                0x0001: 1003,  # module_id: Safety Module
                30001: 0,  # safety_status: Safe
                30002: 0,  # emergency_stop_status: Released
                30003: 1,  # safety_zone_1: Clear
                30004: 1,  # safety_zone_2: Clear
                30005: 1,  # safety_zone_3: Clear
                30006: 0,  # obstacle_detected: No obstacles
                30007: 0,  # safety_error_code: No error
                30008: 0,  # safety_error_message: No error
            }
        }
        
    async def connect(self) -> bool:
        """Connect to RS485 device"""
        if self.use_mock:
            self.connected = True
            logger.info("Mock RS485 connection established")
            return True
            
        try:
            if not self.client.connected:
                self.connected = self.client.connect()
                if self.connected:
                    logger.info(f"Connected to RS485 device: {settings.rs485_port}")
                else:
                    logger.error(f"Failed to connect to RS485 device: {settings.rs485_port}")
            return self.connected
        except Exception as e:
            logger.error(f"RS485 connection error: {e}")
            raise RS485CommunicationException(f"Connection failed: {e}")
    
    async def disconnect(self):
        """Disconnect from RS485 device"""
        if self.use_mock:
            self.connected = False
            logger.info("Mock RS485 connection closed")
            return
            
        if self.client and self.client.connected:
            self.client.close()
            self.connected = False
            logger.info("Disconnected from RS485 device")
    
    async def read_register(self, address: int, register: int) -> int:
        """Read Modbus register"""
        start_time = time.time()
        
        try:
            if self.use_mock:
                # Return mock data
                if address in self.mock_data and register in self.mock_data[address]:
                    # Simulate communication delay
                    await asyncio.sleep(0.01)
                    return self.mock_data[address][register]
                else:
                    raise RS485CommunicationException(f"Mock register not found: {address}:{register}")
            
            if not await self.connect():
                raise RS485CommunicationException("Not connected to RS485 device")
            
            result = self.client.read_holding_registers(register, 1, slave=address)
            if result.isError():
                raise RS485CommunicationException(f"Read register error: {result}")
            
            # Record successful communication
            duration = time.time() - start_time
            metrics_collector.record_metric("rs485_read_duration", duration)
            
            return result.registers[0]
            
        except ModbusException as e:
            # Record communication error
            metrics_collector.record_rs485_error(address, "modbus_error")
            logger.error(f"Modbus read error: {e}")
            raise RS485CommunicationException(f"Modbus read failed: {e}")
        except Exception as e:
            # Record communication error
            metrics_collector.record_rs485_error(address, "general_error")
            logger.error(f"RS485 read error: {e}")
            raise RS485CommunicationException(f"Read failed: {e}")
    
    async def write_register(self, address: int, register: int, value: int) -> bool:
        """Write Modbus register"""
        start_time = time.time()
        
        try:
            if self.use_mock:
                # Update mock data
                if address not in self.mock_data:
                    self.mock_data[address] = {}
                self.mock_data[address][register] = value
                
                # Simulate communication delay
                await asyncio.sleep(0.01)
                return True
            
            if not await self.connect():
                raise RS485CommunicationException("Not connected to RS485 device")
            
            result = self.client.write_register(register, value, slave=address)
            if result.isError():
                raise RS485CommunicationException(f"Write register error: {result}")
            
            # Record successful communication
            duration = time.time() - start_time
            metrics_collector.record_metric("rs485_write_duration", duration)
            
            return True
            
        except ModbusException as e:
            # Record communication error
            metrics_collector.record_rs485_error(address, "modbus_error")
            logger.error(f"Modbus write error: {e}")
            raise RS485CommunicationException(f"Modbus write failed: {e}")
        except Exception as e:
            # Record communication error
            metrics_collector.record_rs485_error(address, "general_error")
            logger.error(f"RS485 write error: {e}")
            raise RS485CommunicationException(f"Write failed: {e}")
    
    async def read_multiple_registers(self, address: int, start_register: int, count: int) -> List[int]:
        """Read multiple registers"""
        try:
            if self.use_mock:
                # Return mock data for multiple registers
                registers = []
                for i in range(count):
                    register_addr = start_register + i
                    if address in self.mock_data and register_addr in self.mock_data[address]:
                        registers.append(self.mock_data[address][register_addr])
                    else:
                        registers.append(0)
                
                # Simulate communication delay
                await asyncio.sleep(0.02)
                return registers
            
            if not await self.connect():
                raise RS485CommunicationException("Not connected to RS485 device")
            
            result = self.client.read_holding_registers(start_register, count, slave=address)
            if result.isError():
                raise RS485CommunicationException(f"Read multiple registers error: {result}")
            
            return result.registers
            
        except Exception as e:
            logger.error(f"RS485 read multiple registers error: {e}")
            raise RS485CommunicationException(f"Read multiple registers failed: {e}")


class FirmwareIntegrationService:
    """Firmware integration service"""
    
    def __init__(self, use_mock: bool = False):
        self.rs485_service = RS485Service(use_mock=use_mock)
        self.modules = {}  # Cache for discovered modules
        
    async def discover_modules(self) -> Dict[str, Dict[str, Any]]:
        """Discover connected modules via RS485"""
        try:
            modules = {}
            # Scan for modules on addresses 1-10
            for address in range(1, 11):
                try:
                    # Try to read module ID register
                    module_id = await self.rs485_service.read_register(address, 0x0001)
                    if module_id != 0:
                        modules[f"module_{address}"] = {
                            "address": address,
                            "module_id": module_id,
                            "status": "online",
                            "last_seen": time.time()
                        }
                        logger.info(f"Discovered module {address} with ID: {module_id}")
                except RS485CommunicationException:
                    # Module not present at this address
                    continue
            
            self.modules = modules
            return modules
            
        except Exception as e:
            logger.error(f"Module discovery failed: {e}")
            raise RS485CommunicationException(f"Discovery failed: {e}")
    
    async def get_module_status(self, module_id: str) -> Dict[str, Any]:
        """Get status of specific module"""
        if module_id not in self.modules:
            raise RS485CommunicationException(f"Module {module_id} not found")
        
        module = self.modules[module_id]
        address = module["address"]
        
        try:
            # Read status registers based on module type
            if address == 1:  # Travel Motor
                status = await self.rs485_service.read_register(address, 30001)
                speed = await self.rs485_service.read_register(address, 30002)
                current = await self.rs485_service.read_register(address, 30003)
                temperature = await self.rs485_service.read_register(address, 30005)
                
                return {
                    "module_id": module_id,
                    "address": address,
                    "type": "travel_motor",
                    "status": status,
                    "speed": speed,
                    "current": current,
                    "temperature": temperature,
                    "online": True
                }
            elif address == 2:  # Dock Location
                status = await self.rs485_service.read_register(address, 30001)
                pos_x = await self.rs485_service.read_register(address, 30002)
                pos_y = await self.rs485_service.read_register(address, 30003)
                nfc_tag = await self.rs485_service.read_register(address, 30007)
                
                return {
                    "module_id": module_id,
                    "address": address,
                    "type": "dock_location",
                    "status": status,
                    "position_x": pos_x,
                    "position_y": pos_y,
                    "nfc_tag": nfc_tag,
                    "online": True
                }
            elif address == 3:  # Safety Module
                status = await self.rs485_service.read_register(address, 30001)
                estop = await self.rs485_service.read_register(address, 30002)
                obstacles = await self.rs485_service.read_register(address, 30006)
                
                return {
                    "module_id": module_id,
                    "address": address,
                    "type": "safety_module",
                    "status": status,
                    "emergency_stop": estop,
                    "obstacles_detected": obstacles,
                    "online": True
                }
            else:
                # Generic module status
                status = await self.rs485_service.read_register(address, 30001)
                return {
                    "module_id": module_id,
                    "address": address,
                    "type": "unknown",
                    "status": status,
                    "online": True
                }
            
        except Exception as e:
            logger.error(f"Failed to get status for module {module_id}: {e}")
            return {
                "module_id": module_id,
                "address": address,
                "status": 0,
                "online": False,
                "error": str(e)
            }
    
    async def send_command(self, module_id: str, command: Dict[str, Any]) -> bool:
        """Send command to module"""
        if module_id not in self.modules:
            raise RS485CommunicationException(f"Module {module_id} not found")
        
        module = self.modules[module_id]
        address = module["address"]
        
        try:
            # Validate command
            self._validate_command(command)
            
            # Send via RS485 based on module type
            if address == 1:  # Travel Motor
                success = await self._send_motor_command(address, command)
            elif address == 2:  # Dock Location
                success = await self._send_dock_command(address, command)
            elif address == 3:  # Safety Module
                success = await self._send_safety_command(address, command)
            else:
                # Generic command
                command_value = self._encode_command(command)
                success = await self.rs485_service.write_register(address, 0x0100, command_value)
            
            if success:
                logger.info(f"Command sent to module {module_id}: {command}")
                # Record command metric
                metrics_collector.record_robot_command(
                    command.get("type", "unknown"),
                    "sent"
                )
            
            return success
            
        except Exception as e:
            logger.error(f"Failed to send command to module {module_id}: {e}")
            # Record command error
            metrics_collector.record_robot_command(
                command.get("type", "unknown"),
                "failed"
            )
            raise RS485CommunicationException(f"Command failed: {e}")
    
    def _validate_command(self, command: Dict[str, Any]) -> None:
        """Validate command structure"""
        if "type" not in command:
            raise ValueError("Command must have 'type' field")
        
        command_type = command["type"]
        valid_commands = [
            "move", "stop", "pause", "resume", "home", "set_mode",
            "dock", "undock", "emergency_stop", "reset", "enable", "disable"
        ]
        if command_type not in valid_commands:
            raise ValueError(f"Invalid command type: {command_type}")
    
    async def _send_motor_command(self, address: int, command: Dict[str, Any]) -> bool:
        """Send command to motor module"""
        command_type = command["type"]
        
        if command_type == "move":
            # Set motor speed and direction
            speed = command.get("parameters", {}).get("speed", 50)
            direction = command.get("parameters", {}).get("direction", 0)
            
            await self.rs485_service.write_register(address, 40002, direction)  # direction
            await self.rs485_service.write_register(address, 40003, speed)  # speed
            await self.rs485_service.write_register(address, 40001, 1)  # enable
            
        elif command_type == "stop":
            await self.rs485_service.write_register(address, 40001, 0)  # disable
            
        elif command_type == "emergency_stop":
            await self.rs485_service.write_register(address, 40006, 1)  # emergency stop
            
        return True
    
    async def _send_dock_command(self, address: int, command: Dict[str, Any]) -> bool:
        """Send command to dock module"""
        command_type = command["type"]
        
        if command_type == "dock":
            await self.rs485_service.write_register(address, 40003, 1)  # dock command
        elif command_type == "undock":
            await self.rs485_service.write_register(address, 40003, 2)  # undock command
        elif command_type == "stop":
            await self.rs485_service.write_register(address, 40003, 0)  # stop command
            
        return True
    
    async def _send_safety_command(self, address: int, command: Dict[str, Any]) -> bool:
        """Send command to safety module"""
        command_type = command["type"]
        
        if command_type == "reset":
            await self.rs485_service.write_register(address, 40003, 1)  # safety reset
        elif command_type == "enable":
            await self.rs485_service.write_register(address, 40001, 1)  # enable safety
        elif command_type == "disable":
            await self.rs485_service.write_register(address, 40001, 0)  # disable safety
            
        return True
    
    def _encode_command(self, command: Dict[str, Any]) -> int:
        """Encode command dictionary to integer value"""
        # Simple command encoding - can be enhanced based on protocol
        command_type = command.get("type", 0)
        parameters = command.get("parameters", {})
        
        # Encode command type and parameters
        encoded = command_type << 8
        if "speed" in parameters:
            encoded |= (parameters["speed"] & 0xFF)
        
        return encoded
