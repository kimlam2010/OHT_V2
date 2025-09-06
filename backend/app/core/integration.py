"""
Firmware Integration Service - Real HTTP API Communication
"""

import httpx
import logging
from typing import Dict, Any
from datetime import datetime, timezone

from app.config import settings

logger = logging.getLogger(__name__)


class FirmwareIntegrationService:
    """Real Firmware Integration Service via HTTP API"""
    
    def __init__(self, firmware_url: str = None):
        self.firmware_url = firmware_url or settings.firmware_url
        self.http_client = httpx.AsyncClient(
            base_url=self.firmware_url,
            timeout=10.0,  # 10 second timeout
            headers={
                "Content-Type": "application/json",
                "User-Agent": "OHT-50-Backend/1.0"
            }
        )
        
        logger.info(f"Firmware Integration Service initialized for: {self.firmware_url}")
    
    async def get_robot_position(self) -> Dict[str, Any]:
        """Get robot position from Firmware via HTTP API"""
        try:
            response = await self.http_client.get("/api/v1/robot/position")
            
            if response.status_code == 200:
                data = response.json()
                logger.debug(f"Firmware position response: {data}")
                return data
            else:
                logger.error(f"Firmware position request failed: HTTP {response.status_code}")
                raise FirmwareCommunicationException(f"Failed to get robot position: {response.status_code}")
                
        except httpx.TimeoutException:
            logger.error("Firmware communication timeout")
            raise FirmwareCommunicationException("Firmware communication timeout")
        except httpx.ConnectError:
            logger.error("Firmware connection failed")
            raise FirmwareCommunicationException("Firmware connection failed")
        except Exception as e:
            logger.error(f"Firmware position request failed: {e}")
            raise FirmwareCommunicationException(f"Robot position request failed: {e}")

    async def get_robot_configuration(self) -> Dict[str, Any]:
        """Get robot configuration from Firmware via HTTP API"""
        try:
            response = await self.http_client.get("/api/v1/robot/configuration")
            
            if response.status_code == 200:
                data = response.json()
                logger.debug(f"Firmware configuration response: {data}")
                return data
            else:
                logger.error(f"Firmware configuration request failed: HTTP {response.status_code}")
                raise FirmwareCommunicationException(f"Failed to get robot configuration: {response.status_code}")
                
        except httpx.TimeoutException:
            logger.error("Firmware communication timeout")
            raise FirmwareCommunicationException("Firmware communication timeout")
        except httpx.ConnectError:
            logger.error("Firmware connection failed")
            raise FirmwareCommunicationException("Firmware connection failed")
        except Exception as e:
            logger.error(f"Firmware configuration request failed: {e}")
            raise FirmwareCommunicationException(f"Robot configuration request failed: {e}")

    async def get_robot_status(self) -> Dict[str, Any]:
        """Get robot status from Firmware via HTTP API"""
        try:
            response = await self.http_client.get("/api/v1/robot/status")
            
            if response.status_code == 200:
                data = response.json()
                logger.debug(f"Firmware status response: {data}")
                return data
            else:
                logger.error(f"Firmware status request failed: HTTP {response.status_code}")
                raise FirmwareCommunicationException(f"Failed to get robot status: {response.status_code}")
                
        except httpx.TimeoutException:
            logger.error("Firmware communication timeout")
            raise FirmwareCommunicationException("Firmware communication timeout")
        except httpx.ConnectError:
            logger.error("Firmware connection failed")
            raise FirmwareCommunicationException("Firmware connection failed")
        except Exception as e:
            logger.error(f"Firmware communication error: {e}")
            raise FirmwareCommunicationException(f"Firmware communication error: {e}")
    
    async def send_robot_command(self, command: Dict[str, Any]) -> bool:
        """Send robot command to Firmware via HTTP API"""
        try:
            # Validate command structure
            self._validate_command(command)
            
            response = await self.http_client.post(
                "/api/v1/robot/command",
                json=command
            )
            
            if response.status_code == 200:
                result = response.json()
                success = result.get("success", False)
                
                if success:
                    logger.info(f"Command sent successfully: {command.get('command_type')}")
                else:
                    logger.warning(f"Command failed: {result.get('message', 'Unknown error')}")
                
                return success
            else:
                logger.error(f"Command request failed: HTTP {response.status_code}")
                return False
                
        except httpx.TimeoutException:
            logger.error("Command timeout")
            return False
        except httpx.ConnectError:
            logger.error("Command connection failed")
            return False
        except Exception as e:
            logger.error(f"Command communication error: {e}")
            return False
    
    async def get_telemetry_data(self) -> Dict[str, Any]:
        """Get telemetry data from Firmware via HTTP API"""
        try:
            response = await self.http_client.get("/api/v1/telemetry/current")
            
            if response.status_code == 200:
                data = response.json()
                logger.debug(f"Firmware telemetry response: {data}")
                return data
            else:
                logger.error(f"Firmware telemetry request failed: HTTP {response.status_code}")
                raise FirmwareCommunicationException(f"Failed to get telemetry: {response.status_code}")
                
        except httpx.TimeoutException:
            logger.error("Firmware telemetry timeout")
            raise FirmwareCommunicationException("Firmware telemetry timeout")
        except httpx.ConnectError:
            logger.error("Firmware telemetry connection failed")
            raise FirmwareCommunicationException("Firmware telemetry connection failed")
        except Exception as e:
            logger.error(f"Firmware telemetry error: {e}")
            raise FirmwareCommunicationException(f"Firmware telemetry error: {e}")
    
    async def emergency_stop(self) -> bool:
        """Send emergency stop command to Firmware via HTTP API"""
        try:
            response = await self.http_client.post(
                "/api/v1/robot/emergency-stop",
                json={"command": "emergency_stop", "timestamp": datetime.now(timezone.utc).isoformat()}
            )
            
            if response.status_code == 200:
                result = response.json()
                success = result.get("success", False)
                
                if success:
                    logger.info("Emergency stop command sent successfully")
                else:
                    logger.warning(f"Emergency stop failed: {result.get('message', 'Unknown error')}")
                
                return success
            else:
                logger.error(f"Emergency stop request failed: HTTP {response.status_code}")
                return False
                
        except httpx.TimeoutException:
            logger.error("Emergency stop timeout")
            return False
        except httpx.ConnectError:
            logger.error("Emergency stop connection failed")
            return False
        except Exception as e:
            logger.error(f"Emergency stop communication error: {e}")
            return False
    
    async def get_safety_status(self) -> Dict[str, Any]:
        """Get safety status from Firmware via HTTP API"""
        try:
            response = await self.http_client.get("/api/v1/safety/status")
            
            if response.status_code == 200:
                data = response.json()
                logger.debug(f"Firmware safety response: {data}")
                return data
            else:
                logger.error(f"Firmware safety request failed: HTTP {response.status_code}")
                raise FirmwareCommunicationException(f"Failed to get safety status: {response.status_code}")
                
        except httpx.TimeoutException:
            logger.error("Firmware safety timeout")
            raise FirmwareCommunicationException("Firmware safety timeout")
        except httpx.ConnectError:
            logger.error("Firmware safety connection failed")
            raise FirmwareCommunicationException("Firmware safety connection failed")
        except Exception as e:
            logger.error(f"Firmware safety error: {e}")
            raise FirmwareCommunicationException(f"Firmware safety error: {e}")
    
    def _validate_command(self, command: Dict[str, Any]):
        """Validate command structure"""
        # Handle both formats: {"type": "move", "parameters": {...}} and {"command_type": "move", "parameters": {...}}
        command_type = command.get("command_type") or command.get("type")
        parameters = command.get("parameters", {})
        
        if not command_type:
            raise ValueError("Command missing required fields: command_type or type")
        
        if not isinstance(parameters, dict):
            raise ValueError("Parameters must be a dictionary")
        
        valid_commands = ["move", "stop", "pause", "resume", "emergency_stop"]
        if command_type not in valid_commands:
            raise ValueError(f"Invalid command type: {command_type}")
    
    async def health_check(self) -> bool:
        """Check Firmware health via HTTP API"""
        try:
            response = await self.http_client.get("/health")
            return response.status_code == 200
        except Exception as e:
            logger.error(f"Firmware health check failed: {e}")
            return False
    
    async def close(self):
        """Close HTTP client"""
        await self.http_client.aclose()


class FirmwareCommunicationException(Exception):
    """Exception for Firmware communication errors"""
    pass


# Global service instance
firmware_integration_service = FirmwareIntegrationService()


class MockFirmwareService:
    """Mock Firmware service for testing - FOR UNIT TESTING ONLY"""
    
    def __init__(self):
        # WARNING: This is MOCK data - NOT for production use
        logger.warning("🧪 MOCK Firmware Service: Using simulated data - NOT real Firmware!")
        self.mock_data = self._initialize_mock_data()
        
    def _initialize_mock_data(self) -> Dict[str, Any]:
        # MOCK DATA - ONLY FOR UNIT TESTING
        # DO NOT USE IN PRODUCTION
        return {
            "robot_id": "OHT-50-001",
            "status": "idle",
            "position": {"x": 150.5, "y": 200.3},
            "battery_level": 87,
            "temperature": 42.5,
            "motor_speed": 1500,
            "motor_temperature": 45,
            "dock_status": "ready",
            "safety_status": "normal",
            "created_at": datetime.now(timezone.utc).isoformat(),
            "updated_at": datetime.now(timezone.utc).isoformat()
        }
        
    async def get_robot_status(self) -> Dict[str, Any]:
        """Mock robot status"""
        try:
            logger.info("🧪 MOCK: Returning robot status")
            # Add timestamp to mock data
            mock_data = self.mock_data.copy()
            mock_data["timestamp"] = datetime.now(timezone.utc).isoformat()
            return mock_data
        except Exception as e:
            logger.error(f"🧪 MOCK: Error in get_robot_status: {e}")
            # Return safe mock data
            return {
                "robot_id": "OHT-50-001",
                "status": "idle",
                "position": {"x": 150.5, "y": 200.3},
                "battery_level": 87,
                "temperature": 42.5,
                "motor_speed": 1500,
                "motor_temperature": 45,
                "dock_status": "ready",
                "safety_status": "normal",
                "created_at": datetime.now(timezone.utc).isoformat(),
                "updated_at": datetime.now(timezone.utc).isoformat(),
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
        
    async def send_robot_command(self, command: Dict[str, Any]) -> bool:
        """Mock robot command - ALWAYS SUCCESS for testing"""
        # Handle both command formats
        command_type = command.get("command_type") or command.get("type")
        parameters = command.get("parameters", {})
        
        logger.warning("🧪 MOCK: Simulating robot command: %s with parameters: %s", command_type, parameters)
        
        # Simulate command execution
        if command_type == "move":
            self.mock_data["status"] = "moving"
        elif command_type == "stop":
            self.mock_data["status"] = "idle"
        elif command_type == "emergency_stop":
            self.mock_data["status"] = "emergency_stop"
            
        return True  # Always return success for testing
        
    async def send_command(self, command: Dict[str, Any]) -> bool:
        """Mock send command - ALWAYS SUCCESS for testing (alias for send_robot_command)"""
        return await self.send_robot_command(command)
        
    async def get_telemetry_data(self) -> Dict[str, Any]:
        """Mock telemetry data"""
        return {
            "motor_speed": self.mock_data["motor_speed"],
            "motor_temperature": self.mock_data["motor_temperature"],
            "dock_status": self.mock_data["dock_status"],
            "safety_status": self.mock_data["safety_status"]
        }
        
    async def get_safety_status(self) -> Dict[str, Any]:
        """Mock safety status"""
        return {
            "status": "normal",
            "emergency_stop": False,
            "safety_zones": ["zone1", "zone2"],
            "last_check": datetime.now(timezone.utc).isoformat()
        }
        
    async def health_check(self) -> bool:
        """Mock health check"""
        return True
        
    async def emergency_stop(self) -> bool:
        """Mock emergency stop"""
        logger.warning("🧪 MOCK: Simulating emergency stop")
        self.mock_data["status"] = "emergency_stop"
        return True

    async def get_module_status(self, module_id: str) -> Dict[str, Any]:
        """Mock module status to unblock tests"""
        logger.warning("🧪 MOCK: Returning module status for %s", module_id)
        return {
            "module_id": module_id,
            "status": "online",
            "last_seen": datetime.now(timezone.utc).isoformat(),
            "errors": []
        }

    async def discover_modules(self) -> Any:
        """Mock module discovery to unblock tests"""
        logger.warning("🧪 MOCK: Discovering modules")
        return [
            {"module_id": "mod-001", "type": "drive", "status": "online"},
            {"module_id": "mod-002", "type": "io", "status": "online"}
        ]
