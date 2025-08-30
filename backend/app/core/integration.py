"""
RS485 and firmware integration for OHT-50 Backend
"""

import asyncio
import logging
from typing import Optional, Dict, Any
from pymodbus.client import ModbusSerialClient
from pymodbus.exceptions import ModbusException

from app.config import settings
from app.core.exceptions import RS485CommunicationException

logger = logging.getLogger(__name__)


class RS485Service:
    """RS485 communication service using Modbus RTU"""
    
    def __init__(self):
        self.client = ModbusSerialClient(
            port=settings.rs485_port,
            baudrate=settings.rs485_baud_rate,
            timeout=settings.rs485_timeout,
            retries=3
        )
        self.connected = False
        
    async def connect(self) -> bool:
        """Connect to RS485 device"""
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
        if self.client.connected:
            self.client.close()
            self.connected = False
            logger.info("Disconnected from RS485 device")
    
    async def read_register(self, address: int, register: int) -> int:
        """Read Modbus register"""
        try:
            if not await self.connect():
                raise RS485CommunicationException("Not connected to RS485 device")
            
            result = self.client.read_holding_registers(register, 1, slave=address)
            if result.isError():
                raise RS485CommunicationException(f"Read register error: {result}")
            
            return result.registers[0]
            
        except ModbusException as e:
            logger.error(f"Modbus read error: {e}")
            raise RS485CommunicationException(f"Modbus read failed: {e}")
        except Exception as e:
            logger.error(f"RS485 read error: {e}")
            raise RS485CommunicationException(f"Read failed: {e}")
    
    async def write_register(self, address: int, register: int, value: int) -> bool:
        """Write Modbus register"""
        try:
            if not await self.connect():
                raise RS485CommunicationException("Not connected to RS485 device")
            
            result = self.client.write_register(register, value, slave=address)
            if result.isError():
                raise RS485CommunicationException(f"Write register error: {result}")
            
            return True
            
        except ModbusException as e:
            logger.error(f"Modbus write error: {e}")
            raise RS485CommunicationException(f"Modbus write failed: {e}")
        except Exception as e:
            logger.error(f"RS485 write error: {e}")
            raise RS485CommunicationException(f"Write failed: {e}")


class FirmwareIntegrationService:
    """Firmware integration service"""
    
    def __init__(self):
        self.rs485_service = RS485Service()
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
                            "status": "online"
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
            # Read status registers
            status = await self.rs485_service.read_register(address, 0x0002)
            temperature = await self.rs485_service.read_register(address, 0x0003)
            
            return {
                "module_id": module_id,
                "address": address,
                "status": status,
                "temperature": temperature,
                "online": True
            }
            
        except Exception as e:
            logger.error(f"Failed to get status for module {module_id}: {e}")
            return {
                "module_id": module_id,
                "address": address,
                "status": 0,
                "temperature": 0,
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
            # Write command to command register
            command_value = self._encode_command(command)
            success = await self.rs485_service.write_register(address, 0x0100, command_value)
            
            if success:
                logger.info(f"Command sent to module {module_id}: {command}")
            
            return success
            
        except Exception as e:
            logger.error(f"Failed to send command to module {module_id}: {e}")
            raise RS485CommunicationException(f"Command failed: {e}")
    
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
