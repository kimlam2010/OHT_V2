"""
Firmware Integration Service for OHT-50 Backend
Handles communication with Firmware via HTTP API (NOT RS485)
"""

import logging
import asyncio
import httpx
from typing import Dict, Any, List, Optional, Tuple
from datetime import datetime, timedelta
from dataclasses import dataclass
from enum import Enum

from app.config import settings
from app.models.telemetry import SensorData
from app.models.sensors import SensorConfiguration

logger = logging.getLogger(__name__)


class SensorType(Enum):
    """Sensor types supported by firmware"""
    RFID = "rfid"
    ACCELEROMETER = "accelerometer"
    PROXIMITY = "proximity"
    LIDAR = "lidar"


class FirmwareStatus(Enum):
    """Firmware connection status"""
    CONNECTED = "connected"
    DISCONNECTED = "disconnected"
    ERROR = "error"
    TIMEOUT = "timeout"


@dataclass
class FirmwareResponse:
    """Firmware API response"""
    success: bool
    data: Optional[Dict[str, Any]] = None
    error: Optional[str] = None
    timestamp: Optional[datetime] = None


@dataclass
class SensorReading:
    """Sensor reading from firmware"""
    sensor_type: SensorType
    sensor_id: str
    data: Dict[str, Any]
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
        self.firmware_url = firmware_url or getattr(settings, 'firmware_url', 'http://localhost:8081')
        self.http_client = httpx.AsyncClient(
            base_url=self.firmware_url,
            timeout=httpx.Timeout(5.0),  # 5 second timeout
            limits=httpx.Limits(max_keepalive_connections=5, max_connections=10)
        )
        
        # Connection status
        self.status = FirmwareStatus.DISCONNECTED
        self.last_heartbeat = None
        self.connection_errors = 0
        self.max_connection_errors = 5
        
        # Sensor data cache
        self.sensor_cache: Dict[str, SensorReading] = {}
        self.cache_timeout = timedelta(seconds=1)  # 1 second cache timeout
        
        # WARNING: This service MUST connect to real Firmware
        # DO NOT use mock data in production
        logger.warning("🔌 Firmware Integration: Connecting to REAL Firmware at %s", self.firmware_url)
        logger.warning("⚠️  WARNING: This service MUST use real Firmware - NO mock data in production!")
        
    async def initialize(self) -> bool:
        """
        Initialize connection to firmware
        
        Returns:
            True if initialization successful
        """
        try:
            logger.info("🔌 Initializing firmware connection to %s", self.firmware_url)
            
            # Test connection with system status (health endpoint doesn't exist)
            response = await self._send_request("GET", "/api/v1/system/status")
            
            if response.success:
                self.status = FirmwareStatus.CONNECTED
                self.last_heartbeat = datetime.utcnow()
                self.connection_errors = 0
                logger.info("✅ Firmware connection established successfully")
                return True
            else:
                self.status = FirmwareStatus.ERROR
                self.connection_errors += 1
                logger.error("❌ Firmware connection failed: %s", response.error)
                return False
                
        except Exception as e:
            self.status = FirmwareStatus.ERROR
            self.connection_errors += 1
            logger.error("❌ Firmware initialization failed: %s", e)
            return False
    
    async def get_sensor_data(self, sensor_type: SensorType, sensor_id: str = None) -> Optional[SensorReading]:
        """
        Get sensor data from firmware
        
        Args:
            sensor_type: Type of sensor
            sensor_id: Specific sensor ID (optional)
            
        Returns:
            Sensor reading or None if failed
        """
        try:
            # Check cache first
            cache_key = f"{sensor_type.value}_{sensor_id or 'default'}"
            if cache_key in self.sensor_cache:
                cached_reading = self.sensor_cache[cache_key]
                if datetime.utcnow() - cached_reading.timestamp < self.cache_timeout:
                    return cached_reading
            
            # Get data from firmware
            endpoint = f"/api/v1/sensors/{sensor_type.value}"
            if sensor_id:
                endpoint += f"/{sensor_id}"
            
            response = await self._send_request("GET", endpoint)
            
            if response.success and response.data:
                # Parse sensor data
                reading = self._parse_sensor_data(sensor_type, sensor_id, response.data)
                
                if reading:
                    # Cache the reading
                    self.sensor_cache[cache_key] = reading
                    return reading
            
            return None
            
        except Exception as e:
            logger.error("❌ Failed to get sensor data for %s: %s", sensor_type.value, e)
            return None
    
    async def get_rfid_data(self, sensor_id: str = None) -> Optional[SensorReading]:
        """
        Get RFID sensor data
        
        Args:
            sensor_id: RFID sensor ID
            
        Returns:
            RFID reading or None
        """
        return await self.get_sensor_data(SensorType.RFID, sensor_id)
    
    async def get_accelerometer_data(self, sensor_id: str = None) -> Optional[SensorReading]:
        """
        Get accelerometer sensor data
        
        Args:
            sensor_id: Accelerometer sensor ID
            
        Returns:
            Accelerometer reading or None
        """
        return await self.get_sensor_data(SensorType.ACCELEROMETER, sensor_id)
    
    async def get_proximity_data(self, sensor_id: str = None) -> Optional[SensorReading]:
        """
        Get proximity sensor data
        
        Args:
            sensor_id: Proximity sensor ID
            
        Returns:
            Proximity reading or None
        """
        return await self.get_sensor_data(SensorType.PROXIMITY, sensor_id)
    
    async def get_lidar_data(self, sensor_id: str = None) -> Optional[SensorReading]:
        """
        Get LiDAR sensor data
        
        Args:
            sensor_id: LiDAR sensor ID
            
        Returns:
            LiDAR reading or None
        """
        return await self.get_sensor_data(SensorType.LIDAR, sensor_id)
    
    async def get_all_sensor_data(self) -> Dict[str, SensorReading]:
        """
        Get data from all sensors
        
        Returns:
            Dictionary of sensor readings
        """
        readings = {}
        
        try:
            # Get data from all sensor types
            tasks = [
                self.get_rfid_data(),
                self.get_accelerometer_data(),
                self.get_proximity_data(),
                self.get_lidar_data()
            ]
            
            results = await asyncio.gather(*tasks, return_exceptions=True)
            
            for i, result in enumerate(results):
                if isinstance(result, SensorReading):
                    sensor_type = list(SensorType)[i]
                    readings[sensor_type.value] = result
                elif isinstance(result, Exception):
                    logger.error("❌ Error getting sensor data: %s", result)
            
            return readings
            
        except Exception as e:
            logger.error("❌ Failed to get all sensor data: %s", e)
            return {}
    
    async def send_robot_command(self, command: Dict[str, Any]) -> bool:
        """
        Send robot command to firmware
        
        Args:
            command: Robot command data
            
        Returns:
            True if command sent successfully
        """
        try:
            logger.info("🤖 Sending robot command to firmware: %s", command.get("type", "unknown"))
            
            response = await self._send_request("POST", "/api/v1/robot/command", data=command)
            
            if response.success:
                logger.info("✅ Robot command sent successfully")
                return True
            else:
                logger.error("❌ Robot command failed: %s", response.error)
                return False
                
        except Exception as e:
            logger.error("❌ Failed to send robot command: %s", e)
            return False
    
    async def get_robot_status(self) -> Optional[Dict[str, Any]]:
        """
        Get robot status from firmware
        
        Returns:
            Robot status data or None
        """
        try:
            logger.info("🔍 Getting robot status from Firmware...")
            
            # Get system status from firmware
            logger.debug("📡 Calling /api/v1/system/status")
            system_response = await self._send_request("GET", "/api/v1/system/status")
            logger.debug(f"📡 System response: success={system_response.success}, data={system_response.data}")
            
            logger.debug("📡 Calling /api/v1/motion/state")
            motion_response = await self._send_request("GET", "/api/v1/motion/state")
            logger.debug(f"📡 Motion response: success={motion_response.success}, data={motion_response.data}")
            
            if system_response.success and motion_response.success:
                # Validate response data
                if not self._validate_firmware_response(system_response.data, motion_response.data):
                    logger.error("❌ Invalid Firmware response data")
                    return None
                
                # Combine system and motion data into robot status format
                robot_status = {
                    "robot_id": "OHT-50-001",
                    "status": "idle" if motion_response.data.get("v", 0) == 0 else "moving",
                    "mode": "auto",
                    "position": {
                        "x": motion_response.data.get("x_est", 0.0),
                        "y": 0.0,  # Y position not available in motion state
                        "z": 0.0
                    },
                    "speed": motion_response.data.get("v", 0.0),
                    "target_speed": motion_response.data.get("target_v", 0.0),
                    "remaining_distance": motion_response.data.get("remaining", 0.0),
                    "battery_level": None,  # MISSING: Firmware doesn't provide battery level
                    "temperature": None,     # MISSING: Firmware doesn't provide temperature
                    "safety": {
                        "estop": motion_response.data.get("safety", {}).get("estop", False),
                        "p95": motion_response.data.get("safety", {}).get("p95", 0)
                    },
                    "docking": motion_response.data.get("docking", "IDLE"),
                    "health": motion_response.data.get("health", False),
                    "freshness_ms": motion_response.data.get("freshness_ms", 0),
                    "system": system_response.data.get("system", "OHT-50"),
                    "system_status": system_response.data.get("status", "ok"),
                    "timestamp": datetime.utcnow().isoformat()
                }
                logger.info(f"✅ Successfully got robot status: {robot_status}")
                return robot_status
            else:
                logger.warning(f"⚠️ Failed to get complete robot status: system_success={system_response.success}, motion_success={motion_response.success}")
                return None
            
        except Exception as e:
            logger.error("❌ Failed to get robot status: %s", e)
            return None
    
    async def configure_sensor(self, sensor_id: str, config: Dict[str, Any]) -> bool:
        """
        Configure sensor via firmware
        
        Args:
            sensor_id: Sensor identifier
            config: Configuration data
            
        Returns:
            True if configuration successful
        """
        try:
            logger.info("⚙️ Configuring sensor %s via firmware", sensor_id)
            
            response = await self._send_request("POST", f"/api/v1/sensors/{sensor_id}/configure", data=config)
            
            if response.success:
                logger.info("✅ Sensor %s configured successfully", sensor_id)
                return True
            else:
                logger.error("❌ Sensor configuration failed: %s", response.error)
                return False
                
        except Exception as e:
            logger.error("❌ Failed to configure sensor %s: %s", sensor_id, e)
            return False
    
    async def calibrate_sensor(self, sensor_id: str, calibration_data: Dict[str, Any]) -> bool:
        """
        Calibrate sensor via firmware
        
        Args:
            sensor_id: Sensor identifier
            calibration_data: Calibration parameters
            
        Returns:
            True if calibration successful
        """
        try:
            logger.info("🔧 Calibrating sensor %s via firmware", sensor_id)
            
            response = await self._send_request("POST", f"/api/v1/sensors/{sensor_id}/calibrate", data=calibration_data)
            
            if response.success:
                logger.info("✅ Sensor %s calibrated successfully", sensor_id)
                return True
            else:
                logger.error("❌ Sensor calibration failed: %s", response.error)
                return False
                
        except Exception as e:
            logger.error("❌ Failed to calibrate sensor %s: %s", sensor_id, e)
            return False
    
    async def heartbeat(self) -> bool:
        """
        Send heartbeat to firmware
        
        Returns:
            True if firmware is responsive
        """
        try:
            response = await self._send_request("GET", "/api/v1/system/status")
            
            if response.success:
                self.status = FirmwareStatus.CONNECTED
                self.last_heartbeat = datetime.utcnow()
                self.connection_errors = 0
                return True
            else:
                self.status = FirmwareStatus.ERROR
                self.connection_errors += 1
                return False
                
        except Exception as e:
            self.status = FirmwareStatus.ERROR
            self.connection_errors += 1
            logger.error("❌ Firmware heartbeat failed: %s", e)
            return False
    
    async def get_telemetry_data(self) -> Optional[Dict[str, Any]]:
        """
        Get telemetry data from firmware
        
        Returns:
            Telemetry data or None
        """
        try:
            # Get robot status and modules data
            robot_status = await self.get_robot_status()
            modules_response = await self._send_request("GET", "/api/v1/modules")
            
            if robot_status and modules_response.success:
                telemetry_data = {
                    "timestamp": datetime.utcnow().isoformat(),
                    "robot_status": robot_status,
                    "modules": modules_response.data.get("modules", []),
                    "system_health": {
                        "firmware_connected": True,
                        "modules_count": len(modules_response.data.get("modules", [])),
                        "active_modules": len([m for m in modules_response.data.get("modules", []) if m.get("status", 0) > 0])
                    }
                }
                return telemetry_data
            
            return None
            
        except Exception as e:
            logger.error("❌ Failed to get telemetry data: %s", e)
            return None
    
    async def get_connection_status(self) -> Dict[str, Any]:
        """
        Get firmware connection status
        
        Returns:
            Connection status information
        """
        return {
            "status": self.status.value,
            "firmware_url": self.firmware_url,
            "last_heartbeat": self.last_heartbeat.isoformat() if self.last_heartbeat else None,
            "connection_errors": self.connection_errors,
            "is_healthy": self.connection_errors < self.max_connection_errors
        }
    
    async def _send_request(self, method: str, endpoint: str, data: Dict[str, Any] = None) -> FirmwareResponse:
        """
        Send HTTP request to firmware
        
        Args:
            method: HTTP method
            endpoint: API endpoint
            data: Request data
            
        Returns:
            Firmware response
        """
        try:
            # Prepare request
            kwargs = {}
            if data:
                kwargs["json"] = data
            
            # Send request
            response = await self.http_client.request(method, endpoint, **kwargs)
            
            # Parse response
            if response.status_code == 200:
                try:
                    response_data = response.json()
                    return FirmwareResponse(
                        success=True,
                        data=response_data,
                        timestamp=datetime.utcnow()
                    )
                except Exception as e:
                    return FirmwareResponse(
                        success=False,
                        error=f"Invalid JSON response: {e}",
                        timestamp=datetime.utcnow()
                    )
            else:
                return FirmwareResponse(
                    success=False,
                    error=f"HTTP {response.status_code}: {response.text}",
                    timestamp=datetime.utcnow()
                )
                
        except httpx.TimeoutException:
            self.status = FirmwareStatus.TIMEOUT
            return FirmwareResponse(
                success=False,
                error="Request timeout",
                timestamp=datetime.utcnow()
            )
        except httpx.ConnectError:
            self.status = FirmwareStatus.DISCONNECTED
            return FirmwareResponse(
                success=False,
                error="Connection error",
                timestamp=datetime.utcnow()
            )
        except Exception as e:
            self.status = FirmwareStatus.ERROR
            return FirmwareResponse(
                success=False,
                error=str(e),
                timestamp=datetime.utcnow()
            )
    
    def _parse_sensor_data(self, sensor_type: SensorType, sensor_id: str, data: Dict[str, Any]) -> Optional[SensorReading]:
        """
        Parse sensor data from firmware response
        
        Args:
            sensor_type: Type of sensor
            sensor_id: Sensor identifier
            data: Raw sensor data
            
        Returns:
            Parsed sensor reading
        """
        try:
            # Extract common fields
            quality = data.get("quality", 1.0)
            timestamp_str = data.get("timestamp")
            
            if timestamp_str:
                try:
                    timestamp = datetime.fromisoformat(timestamp_str.replace('Z', '+00:00'))
                except:
                    timestamp = datetime.utcnow()
            else:
                timestamp = datetime.utcnow()
            
            # Validate data quality
            is_valid = quality > 0.0 and data.get("is_valid", True)
            
            return SensorReading(
                sensor_type=sensor_type,
                sensor_id=sensor_id or f"{sensor_type.value}_default",
                data=data,
                quality=quality,
                timestamp=timestamp,
                is_valid=is_valid
            )
            
        except Exception as e:
            logger.error("❌ Failed to parse sensor data: %s", e)
            return None
    
    async def cleanup(self):
        """Cleanup resources"""
        try:
            await self.http_client.aclose()
            logger.info("✅ Firmware integration service cleaned up")
        except Exception as e:
            logger.error("❌ Error during cleanup: %s", e)
    
    def _validate_firmware_response(self, system_data: Dict[str, Any], motion_data: Dict[str, Any]) -> bool:
        """
        Validate Firmware response data
        
        Args:
            system_data: System status data
            motion_data: Motion state data
            
        Returns:
            True if data is valid
        """
        try:
            # Validate system data
            if not system_data or not isinstance(system_data, dict):
                logger.error("❌ Invalid system data: not a dict or empty")
                return False
            
            # System data is nested in "data" field
            actual_system_data = system_data.get("data", system_data)
            if "system" not in actual_system_data or "status" not in actual_system_data:
                logger.error("❌ Invalid system data: missing required fields")
                return False
            
            # Validate motion data
            if not motion_data or not isinstance(motion_data, dict):
                logger.error("❌ Invalid motion data: not a dict or empty")
                return False
            
            # Motion data is nested in "data" field
            actual_motion_data = motion_data.get("data", motion_data)
            
            # Check for required motion fields
            required_fields = ["x_est", "v", "safety"]
            for field in required_fields:
                if field not in actual_motion_data:
                    logger.error(f"❌ Invalid motion data: missing field '{field}'")
                    return False
            
            # Validate data types
            if not isinstance(actual_motion_data.get("x_est"), (int, float)):
                logger.error("❌ Invalid motion data: x_est must be numeric")
                return False
            
            if not isinstance(actual_motion_data.get("v"), (int, float)):
                logger.error("❌ Invalid motion data: v must be numeric")
                return False
            
            # Validate safety data
            safety = actual_motion_data.get("safety", {})
            if not isinstance(safety, dict):
                logger.error("❌ Invalid motion data: safety must be a dict")
                return False
            
            logger.debug("✅ Firmware response data validation passed")
            return True
            
        except Exception as e:
            logger.error(f"❌ Data validation error: {e}")
            return False


# Global firmware integration service instance
firmware_service = FirmwareIntegrationService()


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
            "rfid": {
                "sensor_id": "RFID_001",
                "data": {
                    "rfid_id": "TAG_001",
                    "signal_strength": 0.8,
                    "distance": 150.5,
                    "angle": 45.0
                },
                "quality": 0.9,
                "timestamp": datetime.utcnow().isoformat(),
                "is_valid": True
            },
            "accelerometer": {
                "sensor_id": "ACCEL_001",
                "data": {
                    "x": 0.1,
                    "y": 0.2,
                    "z": 9.8,
                    "magnitude": 9.81
                },
                "quality": 0.95,
                "timestamp": datetime.utcnow().isoformat(),
                "is_valid": True
            },
            "proximity": {
                "sensor_id": "PROX_001",
                "data": {
                    "distance": 25.0,
                    "obstacle_detected": False,
                    "confidence": 0.85
                },
                "quality": 0.85,
                "timestamp": datetime.utcnow().isoformat(),
                "is_valid": True
            },
            "lidar": {
                "sensor_id": "LIDAR_001",
                "data": {
                    "scan_data": [100, 95, 90, 85, 80] * 72,  # 360 points
                    "resolution": 5.0,
                    "max_range": 500.0,
                    "min_range": 10.0
                },
                "quality": 0.9,
                "timestamp": datetime.utcnow().isoformat(),
                "is_valid": True
            },
            "robot_status": {
                "status": "idle",
                "position": {"x": 150.5, "y": 200.3, "theta": 1.57},
                "battery_level": 87,
                "temperature": 42.5,
                "speed": 0.0
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
                "timestamp": datetime.utcnow().isoformat()
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
    import os
    testing_mode = os.getenv("TESTING", "false").lower() == "true"
    
    if use_mock or testing_mode:
        logger.warning("🧪 Using MOCK Firmware Service - NOT for production!")
        # In tests some code expects a Mock instance; provide compatibility
        try:
            from unittest.mock import Mock as _Mock
            if testing_mode:
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
                    "motor_temperature": 42.5,
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
    else:
        logger.info("🔌 Using REAL Firmware Service - connecting to actual Firmware")
        # For testing mode, still return mock even when use_mock=False
        if testing_mode:
            return MockFirmwareService()
        return firmware_service
