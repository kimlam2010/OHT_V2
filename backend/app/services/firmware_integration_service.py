"""
Firmware Integration Service - OHT-50 Backend

This service provides high-level integration with the OHT-50 Firmware,
handling HTTP API communication, WebSocket real-time updates, and
connection management.

WARNING: This service MUST connect to real Firmware via HTTP API
DO NOT use mock data in production
"""

import asyncio
import logging
import time
from datetime import datetime, timezone, timedelta
from typing import Dict, Any, Optional, List, AsyncGenerator
from enum import Enum
from dataclasses import dataclass

from app.lib.fw_client import FWClient, FWConfig, FWConnectionStatus, FWClientError
from app.config import settings

logger = logging.getLogger(__name__)


class FirmwareStatus(Enum):
    """Firmware connection status"""
    DISCONNECTED = "disconnected"
    CONNECTING = "connecting"
    CONNECTED = "connected"
    ERROR = "error"
    RECONNECTING = "reconnecting"


@dataclass
class SensorReading:
    """Sensor reading data structure"""
    sensor_id: str
    value: float
    unit: str
    quality: float
    timestamp: datetime
    is_valid: bool = True


class FirmwareIntegrationService:
    """
    Firmware Integration Service
    
    WARNING: This service MUST connect to real Firmware via HTTP API
    DO NOT use mock data in production
    """
    
    def __init__(self, firmware_url: str = None):
        """
        Initialize firmware integration service
        
        Args:
            firmware_url: Firmware HTTP API URL
        """
        self.firmware_url = firmware_url or getattr(settings, 'firmware_url', 'http://localhost:8080')
        self.firmware_ws_url = getattr(settings, 'firmware_websocket_url', 'ws://localhost:8081/ws')
        
        # Initialize FW client
        self.fw_client: Optional[FWClient] = None
        self.fw_config = FWConfig(
            host="localhost",
            http_port=8080,
            ws_port=8081,
            timeout=settings.firmware_timeout,
            max_retries=settings.firmware_retry_count
        )
        
        # Connection status
        self.status = FirmwareStatus.DISCONNECTED
        self.last_heartbeat = None
        self.connection_errors = 0
        self.max_connection_errors = 5
        
        # Sensor data cache
        self.sensor_cache: Dict[str, SensorReading] = {}
        self.cache_timeout = timedelta(seconds=1)  # 1 second cache timeout
        
        # Background tasks
        self._health_monitor_task: Optional[asyncio.Task] = None
        self._telemetry_task: Optional[asyncio.Task] = None
        
        # WARNING: This service MUST connect to real Firmware
        # DO NOT use mock data in production
        logger.warning("🔌 Firmware Integration: Connecting to REAL Firmware at %s", self.firmware_url)
        logger.warning("⚠️  WARNING: This service MUST use real Firmware - NO mock data in production!")
        
    async def initialize(self) -> bool:
        """
        Initialize firmware integration service
        
        Returns:
            True if initialization successful, False otherwise
        """
        try:
            logger.info("🚀 Initializing Firmware Integration Service...")
            
            # Create FW client
            self.fw_client = FWClient(self.fw_config)
            
            # Connect to firmware
            connected = await self.fw_client.connect()
            if not connected:
                logger.error("❌ Failed to connect to firmware during initialization")
                self.status = FirmwareStatus.ERROR
                return False
            
            self.status = FirmwareStatus.CONNECTED
            self.last_heartbeat = datetime.now(timezone.utc)
            
            # Start background monitoring tasks
            await self._start_background_tasks()
            
            logger.info("✅ Firmware Integration Service initialized successfully")
            return True
            
        except Exception as e:
            logger.error(f"❌ Firmware Integration Service initialization failed: {e}")
            self.status = FirmwareStatus.ERROR
            return False
    
    async def shutdown(self) -> None:
        """Shutdown firmware integration service"""
        logger.info("🛑 Shutting down Firmware Integration Service...")
        
        # Stop background tasks
        if self._health_monitor_task:
            self._health_monitor_task.cancel()
        if self._telemetry_task:
            self._telemetry_task.cancel()
        
        # Disconnect from firmware
        if self.fw_client:
            await self.fw_client.disconnect()
        
        self.status = FirmwareStatus.DISCONNECTED
        logger.info("✅ Firmware Integration Service shutdown complete")
    
    async def _start_background_tasks(self) -> None:
        """Start background monitoring tasks"""
        # Health monitoring task
        self._health_monitor_task = asyncio.create_task(self._health_monitor_loop())
        
        # Telemetry streaming task
        self._telemetry_task = asyncio.create_task(self._telemetry_loop())
        
        logger.info("✅ Background monitoring tasks started")
    
    async def _health_monitor_loop(self) -> None:
        """Background health monitoring loop"""
        while True:
            try:
                if self.fw_client:
                    is_healthy = await self.fw_client.check_connection_health()
                    
                    if is_healthy:
                        self.status = FirmwareStatus.CONNECTED
                        self.connection_errors = 0
                    else:
                        self.status = FirmwareStatus.ERROR
                        self.connection_errors += 1
                        
                        # Attempt reconnection if too many errors
                        if self.connection_errors >= self.max_connection_errors:
                            await self._attempt_reconnection()
                
                # Wait before next check
                await asyncio.sleep(10)  # Check every 10 seconds
                
            except asyncio.CancelledError:
                logger.info("🛑 Health monitor task cancelled")
                break
            except Exception as e:
                logger.error(f"❌ Health monitor error: {e}")
                await asyncio.sleep(5)  # Wait before retry
    
    async def _telemetry_loop(self) -> None:
        """Background telemetry streaming loop"""
        while True:
            try:
                if self.fw_client and self.status == FirmwareStatus.CONNECTED:
                    async for telemetry_data in self.fw_client.get_telemetry_stream():
                        await self._process_telemetry_data(telemetry_data)
                else:
                    # Wait if not connected
                    await asyncio.sleep(1)
                    
            except asyncio.CancelledError:
                logger.info("🛑 Telemetry task cancelled")
                break
            except Exception as e:
                logger.error(f"❌ Telemetry streaming error: {e}")
                await asyncio.sleep(1)
    
    async def _process_telemetry_data(self, data: Dict[str, Any]) -> None:
        """Process incoming telemetry data"""
        try:
            # Update sensor cache with new data
            timestamp = datetime.now(timezone.utc)
            
            # Process robot position data
            if "robot_position" in data:
                position = data["robot_position"]
                self.sensor_cache["robot_x"] = SensorReading(
                    sensor_id="robot_x",
                    value=position.get("x", 0.0),
                    unit="mm",
                    quality=1.0,
                    timestamp=timestamp
                )
                self.sensor_cache["robot_y"] = SensorReading(
                    sensor_id="robot_y",
                    value=position.get("y", 0.0),
                    unit="mm",
                    quality=1.0,
                    timestamp=timestamp
                )
            
            # Process battery data
            if "battery_level" in data:
                self.sensor_cache["battery"] = SensorReading(
                    sensor_id="battery",
                    value=float(data["battery_level"]),
                    unit="%",
                    quality=1.0,
                    timestamp=timestamp
                )
            
            # Process temperature data
            if "temperature" in data:
                self.sensor_cache["temperature"] = SensorReading(
                    sensor_id="temperature",
                    value=float(data["temperature"]),
                    unit="°C",
                    quality=1.0,
                    timestamp=timestamp
                )
            
            logger.debug(f"📊 Telemetry processed: {len(data)} data points")
            
        except Exception as e:
            logger.error(f"❌ Error processing telemetry data: {e}")
    
    async def _attempt_reconnection(self) -> None:
        """Attempt to reconnect to firmware"""
        try:
            logger.warning("🔄 Attempting firmware reconnection...")
            self.status = FirmwareStatus.RECONNECTING
            
            # Disconnect current client
            if self.fw_client:
                await self.fw_client.disconnect()
            
            # Create new client and connect
            self.fw_client = FWClient(self.fw_config)
            connected = await self.fw_client.connect()
            
            if connected:
                self.status = FirmwareStatus.CONNECTED
                self.connection_errors = 0
                logger.info("✅ Firmware reconnection successful")
                
                # Restart background tasks
                await self._start_background_tasks()
            else:
                self.status = FirmwareStatus.ERROR
                logger.error("❌ Firmware reconnection failed")
                
        except Exception as e:
            logger.error(f"❌ Reconnection error: {e}")
            self.status = FirmwareStatus.ERROR
    
    async def validate_firmware_connection(self) -> bool:
        """
        Validate firmware connection and warn if using mock data
        
        Returns:
            True if real firmware connected, False if using mock
        """
        try:
            # Check if this is a mock service
            if isinstance(self, MockFirmwareService):
                logger.warning("🚨 MOCK DATA WARNING: Using MockFirmwareService - NOT connected to real firmware!")
                logger.warning("🚨 This should NEVER happen in production!")
                return False
            
            # Test real firmware connection
            if not self.fw_client:
                logger.error("❌ REAL FIRMWARE: Client not initialized")
                return False
            
            # Test connection with health check
            health_response = await self.fw_client.get_system_health()
            if health_response.get("success", False):
                logger.info("✅ REAL FIRMWARE: Connection validated successfully")
                return True
            else:
                logger.error("❌ REAL FIRMWARE: Connection failed - %s", response.error)
                logger.warning("🚨 WARNING: Backend may fallback to mock data if firmware unavailable!")
                return False
                
        except Exception as e:
            logger.error("❌ FIRMWARE VALIDATION: Connection validation failed - %s", e)
            logger.warning("🚨 WARNING: Backend may fallback to mock data if firmware unavailable!")
            return False
    
    # System API Methods
    
    async def get_system_status(self) -> Dict[str, Any]:
        """Get firmware system status"""
        try:
            if not self.fw_client:
                return {"success": False, "error": "FW client not initialized"}
            
            status = await self.fw_client.get_system_status()
            return status
            
        except Exception as e:
            logger.error(f"❌ Failed to get system status: {e}")
            return {"success": False, "error": str(e)}
    
    async def get_system_health(self) -> Dict[str, Any]:
        """Get firmware system health"""
        try:
            if not self.fw_client:
                return {"success": False, "error": "FW client not initialized"}
            
            health = await self.fw_client.get_system_health()
            return health
            
        except Exception as e:
            logger.error(f"❌ Failed to get system health: {e}")
            return {"success": False, "error": str(e)}
    
    # Module API Methods
    
    async def get_modules(self) -> List[Dict[str, Any]]:
        """Get list of firmware modules"""
        try:
            if not self.fw_client:
                return []
            
            modules = await self.fw_client.get_modules()
            return modules
            
        except Exception as e:
            logger.error(f"❌ Failed to get modules: {e}")
            return []
    
    async def get_module_info(self, module_id: int) -> Dict[str, Any]:
        """Get module information"""
        try:
            if not self.fw_client:
                return {"success": False, "error": "FW client not initialized"}
            
            info = await self.fw_client.get_module_info(module_id)
            return info
            
        except Exception as e:
            logger.error(f"❌ Failed to get module {module_id} info: {e}")
            return {"success": False, "error": str(e)}
    
    async def send_module_command(self, module_id: int, command: str, parameters: Dict[str, Any]) -> bool:
        """Send command to module"""
        try:
            if not self.fw_client:
                logger.error("❌ FW client not initialized")
                return False
            
            success = await self.fw_client.send_module_command(module_id, command, parameters)
            return success
            
        except Exception as e:
            logger.error(f"❌ Failed to send command to module {module_id}: {e}")
            return False
    
    # Safety API Methods
    
    async def get_safety_status(self) -> Dict[str, Any]:
        """Get safety status"""
        try:
            if not self.fw_client:
                return {"success": False, "error": "FW client not initialized"}
            
            status = await self.fw_client.get_safety_status()
            return status
            
        except Exception as e:
            logger.error(f"❌ Failed to get safety status: {e}")
            return {"success": False, "error": str(e)}
    
    async def emergency_stop(self) -> bool:
        """Trigger emergency stop"""
        try:
            if not self.fw_client:
                logger.error("❌ FW client not initialized")
                return False
            
            success = await self.fw_client.emergency_stop()
            return success
            
        except Exception as e:
            logger.error(f"❌ Failed to trigger emergency stop: {e}")
            return False
    
    # Configuration API Methods
    
    async def get_configuration(self) -> Dict[str, Any]:
        """Get current configuration"""
        try:
            if not self.fw_client:
                return {"success": False, "error": "FW client not initialized"}
            
            config = await self.fw_client.get_configuration()
            return config
            
        except Exception as e:
            logger.error(f"❌ Failed to get configuration: {e}")
            return {"success": False, "error": str(e)}
    
    async def update_configuration(self, config: Dict[str, Any]) -> bool:
        """Update configuration"""
        try:
            if not self.fw_client:
                logger.error("❌ FW client not initialized")
                return False
            
            success = await self.fw_client.update_configuration(config)
            return success
            
        except Exception as e:
            logger.error(f"❌ Failed to update configuration: {e}")
            return False
    
    # Diagnostics API Methods
    
    async def get_diagnostics(self) -> Dict[str, Any]:
        """Get system diagnostics"""
        try:
            if not self.fw_client:
                return {"success": False, "error": "FW client not initialized"}
            
            diagnostics = await self.fw_client.get_diagnostics()
            return diagnostics
            
        except Exception as e:
            logger.error(f"❌ Failed to get diagnostics: {e}")
            return {"success": False, "error": str(e)}
    
    # Sensor Data Methods
    
    async def get_sensor_data(self, sensor_type: str, sensor_id: str = None) -> Optional[Dict[str, Any]]:
        """Get sensor data from cache"""
        try:
            cache_key = sensor_id or sensor_type
            sensor_reading = self.sensor_cache.get(cache_key)
            
            if sensor_reading:
                # Check if data is still fresh
                age = datetime.now(timezone.utc) - sensor_reading.timestamp
                if age <= self.cache_timeout:
                    return {
                        "sensor_id": sensor_reading.sensor_id,
                        "value": sensor_reading.value,
                        "unit": sensor_reading.unit,
                        "quality": sensor_reading.quality,
                        "timestamp": sensor_reading.timestamp.isoformat(),
                        "is_valid": sensor_reading.is_valid
                    }
                else:
                    logger.warning(f"⚠️ Sensor data for {cache_key} is stale (age: {age})")
            
            return None
            
        except Exception as e:
            logger.error(f"❌ Failed to get sensor data for {sensor_type}: {e}")
            return None
    
    async def get_telemetry_data(self) -> Dict[str, Any]:
        """Get current telemetry data"""
        try:
            telemetry = {
                "success": True,
                "data": {
                    "timestamp": datetime.now(timezone.utc).isoformat(),
                    "connection_status": self.status.value,
                    "sensors": {}
                }
            }
            
            # Add sensor data from cache
            for sensor_id, sensor_reading in self.sensor_cache.items():
                age = datetime.now(timezone.utc) - sensor_reading.timestamp
                if age <= self.cache_timeout:
                    telemetry["data"]["sensors"][sensor_id] = {
                        "value": sensor_reading.value,
                        "unit": sensor_reading.unit,
                        "quality": sensor_reading.quality,
                        "timestamp": sensor_reading.timestamp.isoformat()
                    }
            
            return telemetry
            
        except Exception as e:
            logger.error(f"❌ Failed to get telemetry data: {e}")
            return {"success": False, "error": str(e)}
    
    # Connection Status Methods
    
    def get_connection_status(self) -> Dict[str, Any]:
        """Get connection status"""
        return {
            "status": self.status.value,
            "firmware_url": self.firmware_url,
            "last_heartbeat": self.last_heartbeat.isoformat() if self.last_heartbeat else None,
            "connection_errors": self.connection_errors,
            "sensor_cache_size": len(self.sensor_cache)
        }
    
    async def get_robot_status(self) -> Optional[Dict[str, Any]]:
        """Get robot status"""
        try:
            # Try to get from firmware first
            if self.fw_client and self.status == FirmwareStatus.CONNECTED:
                system_status = await self.fw_client.get_system_status()
                if system_status.get("success"):
                    return system_status
            
            # Fallback to cached sensor data
            return await self.get_telemetry_data()
            
        except Exception as e:
            logger.error(f"❌ Failed to get robot status: {e}")
            return None
    
    async def send_robot_command(self, command: Dict[str, Any]) -> bool:
        """Send robot command"""
        try:
            if not self.fw_client:
                logger.error("❌ FW client not initialized")
                return False
            
            command_type = command.get("command_type")
            parameters = command.get("parameters", {})
            
            # Route command to appropriate module
            if command_type == "move":
                # Send to movement module (assuming module ID 1)
                success = await self.fw_client.send_module_command(1, "move", parameters)
            elif command_type == "stop":
                # Send emergency stop
                success = await self.fw_client.emergency_stop()
            else:
                logger.warning(f"⚠️ Unknown command type: {command_type}")
                success = False
            
            return success
            
        except Exception as e:
            logger.error(f"❌ Failed to send robot command: {e}")
            return False


# Mock Firmware Service for Development/Testing ONLY
class MockFirmwareService:
    """
    Mock Firmware Service - FOR DEVELOPMENT/TESTING ONLY
    
    WARNING: This is MOCK data - NOT for production use
    """
    
    def __init__(self):
        # WARNING: This is MOCK data - NOT for production use
        logger.warning("🧪 MOCK Firmware Service: Using simulated data - NOT real Firmware!")
        logger.warning("⚠️  WARNING: This is for development/testing ONLY - DO NOT use in production!")
        
        self.mock_data = self._initialize_mock_data()
        
    def _initialize_mock_data(self) -> Dict[str, Any]:
        """Initialize mock sensor data"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        # DO NOT USE IN PRODUCTION
        return {
            "robot_status": {
                "robot_id": "OHT-50-001",
                "status": "idle",
                "position": {"x": 150.5, "y": 200.3},
                "battery_level": 87,
                "temperature": 42.5
            },
            "rfid": {
                "sensor_id": "rfid_001",
                "value": 0.85,
                "unit": "signal_strength",
                "quality": 0.9,
                "timestamp": datetime.now(timezone.utc).isoformat()
            },
            "accelerometer": {
                "sensor_id": "accel_001",
                "value": 0.1,
                "unit": "g",
                "quality": 0.95,
                "timestamp": datetime.now(timezone.utc).isoformat()
            },
            "proximity": {
                "sensor_id": "prox_001",
                "value": 25.5,
                "unit": "cm",
                "quality": 0.8,
                "timestamp": datetime.now(timezone.utc).isoformat()
            },
            "lidar": {
                "sensor_id": "lidar_001",
                "value": 180.0,
                "unit": "degrees",
                "quality": 0.85,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
        }
    
    async def get_sensor_data(self, sensor_type: str, sensor_id: str = None) -> Optional[Dict[str, Any]]:
        """Get mock sensor data"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        return self.mock_data.get(sensor_type)
    
    async def get_robot_status(self) -> Optional[Dict[str, Any]]:
        """Get mock robot status"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        return self.mock_data.get("robot_status")
    
    async def send_robot_command(self, command: Dict[str, Any]) -> bool:
        """Mock robot command"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        logger.info("🧪 MOCK: Robot command received: %s", command)
        return True
    
    async def get_telemetry_data(self) -> Dict[str, Any]:
        """Get mock telemetry data"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        return {
            "success": True,
            "data": {
                "robot_status": self.mock_data.get("robot_status"),
                "sensors": {
                    "rfid": self.mock_data.get("rfid"),
                    "accelerometer": self.mock_data.get("accelerometer"),
                    "proximity": self.mock_data.get("proximity"),
                    "lidar": self.mock_data.get("lidar")
                },
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
        }
    
    async def emergency_stop(self) -> bool:
        """Mock emergency stop"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        logger.info("🧪 MOCK: Emergency stop executed")
        return True


# Factory function to get firmware service
def get_firmware_service(use_mock: bool = False) -> FirmwareIntegrationService:
    """
    Get firmware service instance
    
    Args:
        use_mock: Whether to use mock service (development only)
        
    Returns:
        Firmware service instance
    """
    firmware_service = FirmwareIntegrationService()
    
    # Check environment variables
    import os
    testing_mode = os.getenv("TESTING", "false").lower() == "true"
    use_mock_env = os.getenv("USE_MOCK_FIRMWARE", "false").lower() == "true"
    is_production = os.getenv("ENVIRONMENT", "development").lower() == "production"
    
    # PRODUCTION: NEVER use mock
    if is_production:
        logger.info("🔌 PRODUCTION MODE: Using REAL Firmware Service ONLY")
        return firmware_service
    
    # TESTING MODE: Always use mock
    if testing_mode:
        logger.warning("🧪 TESTING MODE: Using MOCK Firmware Service")
        # In tests some code expects a Mock instance; provide compatibility
        try:
            from unittest.mock import Mock as _Mock
            mock_instance = _Mock(spec=MockFirmwareService)
            # Add required methods to mock
            mock_instance.get_robot_status.return_value = {
                "robot_id": "OHT-50-001",
                "status": "idle",
                "position": {"x": 150.5, "y": 200.3},
                "battery_level": 87,
                "temperature": 42.5
            }
            mock_instance.get_telemetry_data.return_value = {
                "timestamp": "2025-01-28T10:30:00Z",
                "motor_speed": 1500.0,
                "motor_temperature": 45.0,
                "dock_status": "ready",
                "safety_status": "normal"
            }
            mock_instance.emergency_stop.return_value = True
            mock_instance.get_module_status.return_value = {"status": "active"}
            mock_instance.discover_modules.return_value = []
            mock_instance.initialize.return_value = True
            mock_instance.connection_errors = 0
            mock_instance.status = "CONNECTED"
            return mock_instance  # type: ignore[return-value]
        except Exception:
            pass
        return MockFirmwareService()
    
    # DEVELOPMENT: Check environment flags
    if use_mock and use_mock_env:
        logger.warning("🧪 DEVELOPMENT MODE: Using MOCK Firmware Service (use_mock=True and USE_MOCK_FIRMWARE=true)")
        return MockFirmwareService()
    else:
        logger.info("🔌 DEVELOPMENT MODE: Using REAL Firmware Service - connecting to actual Firmware")
        return firmware_service


# Global firmware service instance
firmware_service = get_firmware_service()