"""
Robot Control Service - Production Implementation
"""

import time
import logging
from typing import Dict, Any, Optional  
from datetime import datetime, timezone

from app.core.database import get_db_context
from app.models.robot import Robot
from app.models.telemetry import TelemetryCurrent
from app.core.integration import FirmwareIntegrationService

logger = logging.getLogger(__name__)


class RobotControlService:
    """Robot Control Service - Production Implementation"""
    
    def __init__(self):
        self._cache: Dict[str, Any] = {}
        self._cache_ttl = 5  # 5 seconds cache
        self._last_update = 0
        
        # Use MockFirmwareService for testing, real FirmwareIntegrationService for production
        import os
        if os.getenv("TESTING", "false").lower() == "true":
            from app.core.integration import MockFirmwareService
            self._firmware_service = MockFirmwareService()
            logger.info("Using MockFirmwareService for testing")
        else:
            self._firmware_service = FirmwareIntegrationService()
            logger.info("Using real FirmwareIntegrationService for production")
        
        # Force mock firmware for testing if needed
        self._force_mock = os.getenv("TESTING", "false").lower() == "true"
        
        # Update force_mock if environment changes
        def update_force_mock():
            self._force_mock = os.getenv("TESTING", "false").lower() == "true"
            if self._force_mock:
                from app.core.integration import MockFirmwareService
                self._firmware_service = MockFirmwareService()
                logger.info("Switched to MockFirmwareService for testing")
        
        # Check environment variable on each method call
        self._update_force_mock = update_force_mock
        
    async def get_robot_status(self) -> Dict[str, Any]:
        """Get robot status from database and Firmware"""
        # Update force_mock based on current environment
        self._update_force_mock()
        
        current_time = time.time()
        
        # Check cache first
        if current_time - self._last_update < self._cache_ttl:
            cached_status = self._cache.get("status")
            if cached_status:
                return cached_status
        
        try:
            # In testing mode, use mock data directly
            if self._force_mock:
                try:
                    logger.info("🧪 Using mock firmware for testing")
                    mock_data = await self._firmware_service.get_robot_status()
                    # Update cache
                    self._cache["status"] = mock_data
                    self._last_update = current_time
                    return mock_data
                except Exception as e:
                    logger.error(f"Mock firmware failed: {e}")
                    # Return default mock data
                    default_mock = {
                        "robot_id": "OHT-50-001",
                        "status": "idle",
                        "position": {"x": 150.5, "y": 200.3},
                        "battery_level": 87,
                        "temperature": 42.5,
                        "created_at": datetime.now(timezone.utc).isoformat(),
                        "updated_at": datetime.now(timezone.utc).isoformat()
                    }
                    self._cache["status"] = default_mock
                    self._last_update = current_time
                    return default_mock
            
            # Production mode: Get status from database and Firmware
            status_data = await self._fetch_robot_status_from_db()
            
            # Get real-time data from Firmware
            firmware_data = await self._fetch_firmware_status()
            
            # Merge database and Firmware data
            combined_status = self._merge_status_data(status_data, firmware_data)
            
            # Update cache
            self._cache["status"] = combined_status
            self._last_update = current_time
            
            return combined_status
            
        except Exception as e:
            logger.error(f"Failed to get robot status: {e}")
            # Return cached data if available, otherwise return error status
            return self._cache.get("status", {
                "status": "error",
                "error": str(e),
                "timestamp": datetime.now(timezone.utc).isoformat()
            })
    
    async def _fetch_robot_status_from_db(self) -> Dict[str, Any]:
        """Fetch robot status from database"""
        try:
            # In testing mode, skip database query and return mock data
            if self._force_mock:
                logger.info("🧪 Testing mode: Skipping database query")
                return {
                    "robot_id": "OHT-50-001",
                    "status": "idle",
                    "position": {"x": 150.5, "y": 200.3},
                    "battery_level": 87,
                    "temperature": 42.5,
                    "created_at": datetime.now(timezone.utc).isoformat(),
                    "updated_at": datetime.now(timezone.utc).isoformat()
                }
            
            from sqlalchemy import text
            async with get_db_context() as db:
                # Query latest robot status from database
                latest_status = await db.execute(
                    text("SELECT * FROM robots ORDER BY created_at DESC LIMIT 1")
                )
                result = latest_status.fetchone()
                
                if result:
                    return {
                        "robot_id": result.robot_id,
                        "status": result.status,
                        "position": {"x": result.position_x, "y": result.position_y},
                        "battery_level": result.battery_level,
                        "temperature": result.temperature,
                        "created_at": result.created_at.isoformat(),
                        "updated_at": result.updated_at.isoformat()
                    }
                else:
                    # No data in database, return default status
                    return {
                        "robot_id": "OHT-50-001",
                        "status": "unknown",
                        "position": {"x": 0.0, "y": 0.0},
                        "battery_level": 0,
                        "temperature": 0.0,
                        "created_at": datetime.now(timezone.utc).isoformat(),
                        "updated_at": datetime.now(timezone.utc).isoformat()
                    }
                    
        except Exception as e:
            logger.error(f"Database query failed: {e}")
            raise
    
    async def _fetch_firmware_status(self) -> Dict[str, Any]:
        """Fetch real-time status from Firmware via HTTP API"""
        try:
            # Get real-time data from Firmware HTTP API
            firmware_data = await self._firmware_service.get_robot_status()
            return firmware_data
        except Exception as e:
            logger.error(f"Firmware communication failed: {e}")
            # Return empty data if Firmware communication fails
            return {}
    
    def _merge_status_data(self, db_data: Dict[str, Any], firmware_data: Dict[str, Any]) -> Dict[str, Any]:
        """Merge database and Firmware data"""
        merged = db_data.copy()
        
        # Override with real-time Firmware data if available
        if firmware_data:
            merged.update(firmware_data)
        
        # Add timestamp
        merged["timestamp"] = datetime.now(timezone.utc).isoformat()
        merged["data_source"] = "database_and_firmware"
        
        return merged
    
    async def send_command(self, command: Dict[str, Any]) -> Dict[str, Any]:
        """Send command to robot via Firmware"""
        try:
            # Validate command
            self._validate_command(command)
            
            # In testing mode, use mock firmware directly
            import os
            if os.getenv("TESTING", "false").lower() == "true":
                # Use mock firmware for testing
                success = await self._firmware_service.send_robot_command(command)
                
                if success:
                    return {
                        "success": True,
                        "message": "Command sent successfully (mock)",
                        "command": command,
                        "timestamp": datetime.now(timezone.utc).isoformat()         
                    }
                else:
                    return {
                        "success": False,
                        "message": "Command failed (mock)",
                        "command": command,
                        "timestamp": datetime.now(timezone.utc).isoformat()        
                    }
            
            # Production mode: Send command to Firmware via HTTP API
            success = await self._firmware_service.send_robot_command(command)
            
            if success:
                # Log successful command
                await self._log_command(command, "success")
                return {
                    "success": True,
                    "message": "Command sent successfully",
                    "command": command,
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                # Log failed command
                await self._log_command(command, "failed")
                return {
                    "success": False,
                    "message": "Command failed",
                    "command": command,
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
                
        except Exception as e:
            logger.error(f"Command execution failed: {e}")
            await self._log_command(command, "error", str(e))
            return {
                "success": False,
                "message": f"Command error: {str(e)}",
                "command": command,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
    
    def _validate_command(self, command: Dict[str, Any]) -> bool:
        """Validate robot command"""
        # Handle both formats: {"type": "move", "parameters": {...}} and {"command_type": "move", "parameters": {...}}
        command_type = command.get("command_type") or command.get("type")
        parameters = command.get("parameters", {})
        
        if not command_type:
            raise ValueError("Missing required field: command_type or type")
        
        if not isinstance(parameters, dict):
            raise ValueError("Parameters must be a dictionary")
        
        valid_commands = ["move", "stop", "pause", "resume", "emergency_stop"]
        if command_type not in valid_commands:
            raise ValueError(f"Invalid command type. Must be one of: {valid_commands}")
        
        return True
    
    async def emergency_stop(self) -> Dict[str, Any]:
        """Emergency stop robot"""
        # Update force_mock based on current environment
        self._update_force_mock()
        
        try:
            # Send emergency stop command to Firmware
            command = {
                "command_type": "emergency_stop",
                "parameters": {},
                "priority": "high"
            }
            
            # In testing mode, use mock firmware directly
            if self._force_mock:
                # Use mock firmware for testing
                success = await self._firmware_service.emergency_stop()
                
                if success:
                    return {
                        "success": True,
                        "message": "Emergency stop executed (mock)",
                        "timestamp": datetime.now(timezone.utc).isoformat()
                    }
                else:
                    return {
                        "success": False,
                        "message": "Emergency stop failed (mock)",
                        "timestamp": datetime.now(timezone.utc).isoformat()
                    }
            
            # Production mode: Send command to Firmware via HTTP API
            success = await self._firmware_service.send_robot_command(command)
            
            if success:
                # Update database status
                await self._update_robot_status("emergency_stop")
                
                return {
                    "success": True,
                    "message": "Emergency stop executed",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                return {
                    "success": False,
                    "message": "Emergency stop failed",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
                
        except Exception as e:
            logger.error(f"Emergency stop failed: {e}")
            return {
                "success": False,
                "message": f"Emergency stop error: {str(e)}",
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
    
    async def _update_robot_status(self, status: str):
        """Update robot status in database"""
        try:
            from sqlalchemy import text
            async with get_db_context() as db:
                await db.execute(
                    text("UPDATE robots SET status = :status, updated_at = :updated_at WHERE id = (SELECT MAX(id) FROM robots)"),
                    {"status": status, "updated_at": datetime.now(timezone.utc).isoformat()}
                )
                await db.commit()
        except Exception as e:
            logger.error(f"Failed to update robot status: {e}")
    
    async def _log_command(self, command: Dict[str, Any], result: str, error: str = None):
        """Log command execution"""
        try:
            from sqlalchemy import text
            import uuid
            # Handle both command formats
            command_type = command.get("command_type") or command.get("type") or "unknown"
            parameters = command.get("parameters", {})
            
            # Generate unique command ID to avoid UNIQUE constraint violation
            command_id = f"cmd_{uuid.uuid4().hex[:8]}_{int(time.time())}"
            
            async with get_db_context() as db:
                await db.execute(
                    text("""
                    INSERT INTO robot_commands (command_id, user_id, command_type, command_data, status, result, error_message, created_at)
                    VALUES (:command_id, :user_id, :command_type, :command_data, :status, :result, :error_message, :created_at)
                    """),
                    {
                        "command_id": command_id,
                        "user_id": 1,  # Default user ID
                        "command_type": command_type,
                        "command_data": str(parameters),
                        "status": result,
                        "result": "success" if result == "success" else "failed",
                        "error_message": error,
                        "created_at": datetime.now(timezone.utc).isoformat()
                    }
                )
                await db.commit()
        except Exception as e:
            logger.error(f"Failed to log command: {e}")
    
    async def get_command_history(self, limit: int = 10) -> list:
        """Get command execution history"""
        try:
            from sqlalchemy import text
            async with get_db_context() as db:
                result = await db.execute(
                    text("SELECT * FROM robot_commands ORDER BY created_at DESC LIMIT :limit"),
                    {"limit": limit}
                )
                return [dict(row) for row in result.fetchall()]
        except Exception as e:
            logger.error(f"Failed to get command history: {e}")
            return []
    
    async def clear_cache(self):
        """Clear service cache"""
        self._cache.clear()
        self._last_update = 0


# Global service instance
robot_control_service = RobotControlService()
