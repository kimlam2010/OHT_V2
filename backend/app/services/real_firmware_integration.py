"""
Real Firmware Integration Service for Production Environment

This service provides real-time communication with robot firmware via HTTP API.
It includes health checks, retry mechanisms, and fallback strategies.
"""

import asyncio
import logging
from typing import Dict, Any, Optional
from datetime import datetime, timedelta

import httpx
from fastapi import HTTPException

from app.core.firmware_health_checker import FirmwareHealthChecker
from app.core.firmware_retry_manager import FirmwareRetryManager
from app.core.firmware_fallback_manager import FirmwareFallbackManager

logger = logging.getLogger(__name__)


class FirmwareUnavailableException(Exception):
    """Exception raised when firmware is unavailable"""
    pass


class RealFirmwareIntegrationService:
    """
    Production-grade firmware integration service
    
    Features:
    - Real HTTP API communication with firmware
    - Health monitoring và health checks
    - Retry mechanisms với exponential backoff
    - Fallback strategies khi firmware unavailable
    - Comprehensive error handling và logging
    """
    
    def __init__(self, firmware_url: str = "http://localhost:8080"):
        self.firmware_url = firmware_url
        self.http_client = httpx.AsyncClient(
            base_url=firmware_url,
            timeout=httpx.Timeout(10.0),  # 10 second timeout
            limits=httpx.Limits(max_keepalive_connections=5, max_connections=10)
        )
        
        # Initialize supporting services
        self.health_checker = FirmwareHealthChecker(firmware_url)
        self.retry_manager = FirmwareRetryManager()
        self.fallback_manager = FirmwareFallbackManager()
        
        # Service status
        self.last_health_check = None
        self.health_status = False
        self.connection_errors = 0
        self.max_connection_errors = 5
        
        logger.info(f"🚀 RealFirmwareIntegrationService initialized for {firmware_url}")
        
    async def __aenter__(self):
        """Async context manager entry"""
        return self
        
    async def __aexit__(self, exc_type, exc_val, exc_tb):
        """Async context manager exit"""
        await self.close()
        
    async def close(self):
        """Close HTTP client"""
        await self.http_client.aclose()
        logger.info("🔌 RealFirmwareIntegrationService HTTP client closed")
        
    async def check_health(self) -> bool:
        """Check firmware health status"""
        try:
            self.health_status = await self.health_checker.check_health()
            self.last_health_check = datetime.utcnow()
            
            if self.health_status:
                self.connection_errors = 0  # Reset error counter
                logger.debug("✅ Firmware health check passed")
            else:
                logger.warning("⚠️ Firmware health check failed")
                
            return self.health_status
            
        except Exception as e:
            logger.error(f"❌ Firmware health check error: {e}")
            self.health_status = False
            return False
            
    async def get_robot_status(self) -> Dict[str, Any]:
        """Get current robot status from firmware"""
        try:
            # Check health first
            if not await self.check_health():
                logger.warning("⚠️ Firmware unhealthy, using fallback status")
                return await self._get_fallback_status()
                
            # Use retry manager for robust communication
            response = await self.retry_manager.execute_with_retry(
                self._make_status_request
            )
            
            if response and response.status_code == 200:
                status_data = response.json()
                logger.debug(f"✅ Robot status retrieved: {status_data.get('status', 'unknown')}")
                return status_data
            else:
                logger.error(f"❌ Firmware status request failed: {response.status_code if response else 'No response'}")
                return await self._get_fallback_status()
                
        except Exception as e:
            logger.error(f"❌ Robot status request failed: {e}")
            self.connection_errors += 1
            
            if self.connection_errors >= self.max_connection_errors:
                logger.critical("🚨 Max connection errors reached, switching to fallback mode")
                
            return await self._get_fallback_status()
            
    async def send_robot_command(self, command: Dict[str, Any]) -> bool:
        """Send robot command to firmware"""
        try:
            # Validate command
            if not self._validate_command(command):
                raise ValueError("Invalid robot command format")
                
            # Check health before sending command
            if not await self.check_health():
                logger.warning("⚠️ Firmware unhealthy, command may fail")
                
            # Use retry manager for robust communication
            response = await self.retry_manager.execute_with_retry(
                self._make_command_request, command
            )
            
            if response and response.status_code == 200:
                result = response.json()
                success = result.get("success", False)
                
                if success:
                    logger.info(f"✅ Robot command executed successfully: {command.get('command_type', 'unknown')}")
                else:
                    logger.warning(f"⚠️ Robot command failed: {result.get('message', 'Unknown error')}")
                    
                return success
            else:
                logger.error(f"❌ Robot command request failed: {response.status_code if response else 'No response'}")
                return False
                
        except Exception as e:
            logger.error(f"❌ Robot command failed: {e}")
            self.connection_errors += 1
            return False
            
    async def get_telemetry_data(self) -> Dict[str, Any]:
        """Get telemetry data from firmware"""
        try:
            # Check health first
            if not await self.check_health():
                logger.warning("⚠️ Firmware unhealthy, using fallback telemetry")
                return await self._get_fallback_telemetry()
                
            # Use retry manager for robust communication
            response = await self.retry_manager.execute_with_retry(
                self._make_telemetry_request
            )
            
            if response and response.status_code == 200:
                telemetry_data = response.json()
                logger.debug("✅ Telemetry data retrieved successfully")
                return telemetry_data
            else:
                logger.error(f"❌ Telemetry request failed: {response.status_code if response else 'No response'}")
                return await self._get_fallback_telemetry()
                
        except Exception as e:
            logger.error(f"❌ Telemetry request failed: {e}")
            return await self._get_fallback_telemetry()
            
    async def get_firmware_info(self) -> Dict[str, Any]:
        """Get firmware information và version"""
        try:
            response = await self.http_client.get("/api/v1/firmware/info")
            if response.status_code == 200:
                return response.json()
            else:
                logger.warning(f"⚠️ Firmware info request failed: {response.status_code}")
                return {"version": "unknown", "status": "unavailable"}
                
        except Exception as e:
            logger.error(f"❌ Firmware info request failed: {e}")
            return {"version": "unknown", "status": "error"}
            
    async def get_system_metrics(self) -> Dict[str, Any]:
        """Get system performance metrics"""
        try:
            response = await self.http_client.get("/api/v1/system/metrics")
            if response.status_code == 200:
                return response.json()
            else:
                logger.warning(f"⚠️ System metrics request failed: {response.status_code}")
                return {}
                
        except Exception as e:
            logger.error(f"❌ System metrics request failed: {e}")
            return {}
            
    # Private methods for internal communication
    
    async def _make_status_request(self) -> Optional[httpx.Response]:
        """Make robot status request"""
        return await self.http_client.get("/api/v1/robot/status")
        
    async def _make_command_request(self, command: Dict[str, Any]) -> Optional[httpx.Response]:
        """Make robot command request"""
        return await self.http_client.post("/api/v1/robot/command", json=command)
        
    async def _make_telemetry_request(self) -> Optional[httpx.Response]:
        """Make telemetry request"""
        return await self.http_client.get("/api/v1/telemetry/current")
        
    def _validate_command(self, command: Dict[str, Any]) -> bool:
        """Validate robot command format"""
        required_fields = ["command_type", "parameters"]
        
        if not all(field in command for field in required_fields):
            return False
            
        valid_commands = ["move", "stop", "pause", "resume", "dock", "undock"]
        if command["command_type"] not in valid_commands:
            return False
            
        return True
        
    async def _get_fallback_status(self) -> Dict[str, Any]:
        """Get fallback robot status"""
        return await self.fallback_manager.get_fallback_status()
        
    async def _get_fallback_telemetry(self) -> Dict[str, Any]:
        """Get fallback telemetry data"""
        return await self.fallback_manager.get_fallback_telemetry()
        
    # Health monitoring methods
    
    async def get_service_health(self) -> Dict[str, Any]:
        """Get overall service health status"""
        return {
            "service": "RealFirmwareIntegrationService",
            "firmware_url": self.firmware_url,
            "health_status": self.health_status,
            "last_health_check": self.last_health_check.isoformat() if self.last_health_check else None,
            "connection_errors": self.connection_errors,
            "max_connection_errors": self.max_connection_errors,
            "fallback_mode": self.connection_errors >= self.max_connection_errors
        }
        
    async def reset_connection_errors(self):
        """Reset connection error counter"""
        self.connection_errors = 0
        logger.info("🔄 Connection error counter reset")
        
    async def force_health_check(self):
        """Force immediate health check"""
        logger.info("🔍 Forcing immediate firmware health check")
        return await self.check_health()
