"""
Firmware Integration Service for OHT-50
Handles communication between Center Client and Firmware
"""

import json
import time
import asyncio
import mmap
import os
import struct
from typing import Dict, Any, Optional
from datetime import datetime
import logging

logger = logging.getLogger(__name__)

class FirmwareIntegration:
    """Integration service for OHT-50 firmware communication"""
    
    def __init__(self, shared_memory_path: str = "/tmp/oht50_firmware"):
        self.shared_memory_path = shared_memory_path
        self.memory_size = 4096  # 4KB shared memory
        self.memory_handle = None
        self.memory_map = None
        
    async def initialize(self):
        """Initialize shared memory communication"""
        try:
            # Create shared memory file if not exists
            if not os.path.exists(self.shared_memory_path):
                with open(self.shared_memory_path, 'wb') as f:
                    f.write(b'\x00' * self.memory_size)
            
            # Open shared memory
            self.memory_handle = os.open(self.shared_memory_path, os.O_RDWR)
            self.memory_map = mmap.mmap(self.memory_handle, self.memory_size)
            
            logger.info(f"Firmware integration initialized: {self.shared_memory_path}")
            return True
            
        except Exception as e:
            logger.error(f"Failed to initialize firmware integration: {e}")
            return False
    
    async def get_system_status(self) -> Dict[str, Any]:
        """Get system status from firmware"""
        try:
            # Read status from shared memory
            self.memory_map.seek(0)
            status_data = self.memory_map.read(256)
            
            # Parse status data (example format)
            if len(status_data) >= 16:
                state_code = struct.unpack('I', status_data[0:4])[0]
                safety_code = struct.unpack('I', status_data[4:8])[0]
                modules_online = struct.unpack('I', status_data[8:12])[0]
                uptime = struct.unpack('I', status_data[12:16])[0]
                
                # Convert codes to strings
                states = {0: "IDLE", 1: "MOVING", 2: "DOCKING", 3: "FAULT", 4: "ESTOP"}
                safety_states = {0: "NORMAL", 1: "WARNING", 2: "CRITICAL", 3: "ESTOP"}
                
                return {
                    "state": states.get(state_code, "UNKNOWN"),
                    "safety": safety_states.get(safety_code, "UNKNOWN"),
                    "modules_online": modules_online,
                    "uptime": uptime
                }
            
            # Fallback to default values
            return {
                "state": "IDLE",
                "safety": "NORMAL", 
                "modules_online": 4,
                "uptime": int(time.time())
            }
            
        except Exception as e:
            logger.error(f"Failed to get system status: {e}")
            return {
                "state": "FAULT",
                "safety": "CRITICAL",
                "modules_online": 0,
                "uptime": 0
            }
    
    async def get_power_data(self) -> Dict[str, Any]:
        """Get power module data from firmware"""
        try:
            # Read power data from shared memory
            self.memory_map.seek(256)
            power_data = self.memory_map.read(128)
            
            # Parse power data (example format)
            if len(power_data) >= 12:
                voltage = struct.unpack('f', power_data[0:4])[0]
                current = struct.unpack('f', power_data[4:8])[0]
                temperature = struct.unpack('f', power_data[8:12])[0]
                
                return {
                    "voltage": voltage,
                    "current": current,
                    "temperature": temperature
                }
            
            # Fallback to default values
            return {
                "voltage": 24.5,
                "current": 3.2,
                "temperature": 42.1
            }
            
        except Exception as e:
            logger.error(f"Failed to get power data: {e}")
            return {
                "voltage": 0.0,
                "current": 0.0,
                "temperature": 0.0
            }
    
    async def send_command(self, command: str, params: Dict[str, Any]) -> bool:
        """Send command to firmware"""
        try:
            # Prepare command data
            cmd_data = {
                "command": command,
                "params": params,
                "timestamp": datetime.utcnow().isoformat()
            }
            
            # Write command to shared memory
            self.memory_map.seek(512)
            cmd_json = json.dumps(cmd_data).encode('utf-8')
            self.memory_map.write(cmd_json.ljust(512, b'\x00'))
            
            # Set command flag
            self.memory_map.seek(1024)
            self.memory_map.write(struct.pack('I', 1))  # Command ready flag
            
            logger.info(f"Command sent to firmware: {command}")
            return True
            
        except Exception as e:
            logger.error(f"Failed to send command: {e}")
            return False
    
    async def apply_configuration(self, config: Dict[str, Any]) -> bool:
        """Apply configuration to firmware"""
        try:
            # Prepare config data
            config_data = {
                "config": config,
                "timestamp": datetime.utcnow().isoformat()
            }
            
            # Write config to shared memory
            self.memory_map.seek(1536)
            config_json = json.dumps(config_data).encode('utf-8')
            self.memory_map.write(config_json.ljust(512, b'\x00'))
            
            # Set config flag
            self.memory_map.seek(1028)
            self.memory_map.write(struct.pack('I', 1))  # Config ready flag
            
            logger.info("Configuration sent to firmware")
            return True
            
        except Exception as e:
            logger.error(f"Failed to apply configuration: {e}")
            return False
    
    async def cleanup(self):
        """Cleanup shared memory resources"""
        try:
            if self.memory_map:
                self.memory_map.close()
            if self.memory_handle:
                os.close(self.memory_handle)
            logger.info("Firmware integration cleanup completed")
        except Exception as e:
            logger.error(f"Cleanup error: {e}")

# Global instance
firmware_integration = FirmwareIntegration()
