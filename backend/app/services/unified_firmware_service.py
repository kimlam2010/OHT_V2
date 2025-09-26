"""
Unified Firmware Service - OHT-50
Tích hợp tất cả firmware communication với circuit breaker, retry và metrics
"""

from dataclasses import dataclass, field
from typing import Dict, Any, Optional, Callable
import httpx
import asyncio
import time
import random
from datetime import datetime, timezone
from enum import Enum
import logging

from app.core.circuit_breaker import CircuitBreaker, CircuitBreakerState
from app.core.retry_manager import RetryManager
from app.core.firmware_metrics import FirmwareMetrics
from app.config import settings
from app.services.firmware_cache import get_firmware_cache

logger = logging.getLogger(__name__)

@dataclass
class FirmwareResponse:
    success: bool
    data: Optional[Dict[str, Any]] = None
    error: Optional[str] = None
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))
    response_time_ms: float = 0.0
    circuit_breaker_state: Optional[str] = None

class UnifiedFirmwareService:
    """
    Unified Firmware Service với circuit breaker, retry và metrics
    Thay thế tất cả firmware integration services
    """
    
    def __init__(self, firmware_url: str = None):
        self.firmware_url = firmware_url or settings.firmware_url
        
        # Optimized HTTP client với connection pooling
        self.http_client = httpx.AsyncClient(
            base_url=self.firmware_url,
            timeout=httpx.Timeout(10.0),
            limits=httpx.Limits(
                max_connections=20,
                max_keepalive_connections=10,
                keepalive_expiry=30.0
            ),
            http2=False
        )
        
        # Circuit breaker
        self.circuit_breaker = CircuitBreaker(
            failure_threshold=5,
            recovery_timeout=60.0,
            success_threshold=3
        )
        
        # Retry manager
        self.retry_manager = RetryManager()
        
        # Metrics
        self.metrics = FirmwareMetrics()
        
        # Connection status
        self.last_heartbeat = None
        self.connection_status = False
        
        logger.info("UnifiedFirmwareService initialized với firmware_url: %s", self.firmware_url)
    
    async def get_robot_status(self) -> FirmwareResponse:
        """Get robot status với circuit breaker và retry (cached)"""
        cache = await get_firmware_cache()
        async def fetch():
            return await self._http_get("/api/v1/robot/status")
        try:
            data = await cache.get_robot_status_cached(fetch)
            return await self._wrap_success("robot_status", data)
        except Exception as _:
            # fallback to protected exec path
            return await self._execute_with_protection(
                "robot_status",
                lambda: self._http_get("/api/v1/robot/status")
            )
    
    async def send_robot_command(self, command: Dict[str, Any]) -> FirmwareResponse:
        """Send robot command với validation"""
        self._validate_command(command)
        return await self._execute_with_protection(
            "robot_command", 
            lambda: self._http_post("/api/v1/robot/command", command)
        )
    
    async def emergency_stop(self) -> FirmwareResponse:
        """Emergency stop với highest priority"""
        return await self._execute_with_protection(
            "emergency_stop",
            lambda: self._http_post("/api/v1/robot/emergency-stop", {
                "command": "emergency_stop",
                "timestamp": datetime.now(timezone.utc).isoformat()
            }),
            priority="emergency"
        )
    
    async def get_telemetry_data(self) -> FirmwareResponse:
        """Get telemetry data với caching"""
        cache = await get_firmware_cache()
        async def fetch():
            return await self._http_get("/api/v1/telemetry/current")
        try:
            data = await cache.get_telemetry_cached(fetch)
            return await self._wrap_success("telemetry", data)
        except Exception:
            return await self._execute_with_protection(
                "telemetry",
                lambda: self._http_get("/api/v1/telemetry/current")
            )

    async def get_robot_position(self) -> Dict[str, Any]:
        """Get robot position từ Firmware"""
        resp = await self._execute_with_protection(
            "robot_position",
            lambda: self._http_get("/api/v1/robot/position")
        )
        if resp.success and resp.data:
            return resp.data
        raise RuntimeError(resp.error or "Failed to get robot position")

    async def get_robot_configuration(self) -> Dict[str, Any]:
        """Get robot configuration từ Firmware"""
        resp = await self._execute_with_protection(
            "robot_configuration",
            lambda: self._http_get("/api/v1/robot/configuration")
        )
        if resp.success and resp.data:
            return resp.data
        raise RuntimeError(resp.error or "Failed to get robot configuration")
    
    async def get_safety_status(self) -> FirmwareResponse:
        """Get safety system status"""
        cache = await get_firmware_cache()
        async def fetch():
            return await self._http_get("/api/v1/safety/status")
        try:
            data = await cache.get_safety_status_cached(fetch)
            return await self._wrap_success("safety_status", data)
        except Exception:
            return await self._execute_with_protection(
                "safety_status",
                lambda: self._http_get("/api/v1/safety/status")
            )
    
    async def get_modules_status(self) -> FirmwareResponse:
        """Get all modules status"""
        cache = await get_firmware_cache()
        async def fetch():
            return await self._http_get("/api/v1/modules/status")
        try:
            data = await cache.get_modules_status_cached(fetch)
            return await self._wrap_success("modules_status", data)
        except Exception:
            return await self._execute_with_protection(
                "modules_status",
                lambda: self._http_get("/api/v1/modules/status")
            )

    async def get_module_info(self, module_id: int) -> FirmwareResponse:
        """Get detailed module information"""
        return await self._execute_with_protection(
            "module_info",
            lambda: self._http_get(f"/api/v1/modules/{module_id}")
        )

    async def send_module_command(self, module_id: int, command: str, parameters: Dict[str, Any]) -> FirmwareResponse:
        """Send a command to a specific module"""
        payload = {"command": command, "parameters": parameters or {}}
        return await self._execute_with_protection(
            "module_command",
            lambda: self._http_post(f"/api/v1/modules/{module_id}/command", payload)
        )

    async def get_configuration(self) -> FirmwareResponse:
        """Get firmware configuration"""
        return await self._execute_with_protection(
            "configuration_get",
            lambda: self._http_get("/api/v1/system/config")
        )

    async def update_configuration(self, config_data: Dict[str, Any]) -> FirmwareResponse:
        """Update firmware configuration"""
        return await self._execute_with_protection(
            "configuration_update",
            lambda: self._http_post("/api/v1/system/config", config_data)
        )

    async def get_diagnostics(self) -> FirmwareResponse:
        """Get system diagnostics"""
        return await self._execute_with_protection(
            "diagnostics",
            lambda: self._http_get("/api/v1/system/diagnostics")
        )

    async def get_sensor_data(self, sensor_type: str, sensor_id: Optional[str] = None) -> FirmwareResponse:
        """Get sensor data by type (and optional id)"""
        endpoint = f"/api/v1/sensors/{sensor_type}"
        if sensor_id:
            endpoint = f"{endpoint}/{sensor_id}"
        return await self._execute_with_protection(
            "sensor_data",
            lambda: self._http_get(endpoint)
        )

    # Module telemetry helpers (for module_telemetry API)
    async def get_module_telemetry(self, module_id: int) -> FirmwareResponse:
        return await self._execute_with_protection(
            "module_telemetry_get",
            lambda: self._http_get(f"/api/v1/modules/{module_id}/telemetry")
        )

    async def update_module_telemetry(self, module_id: int, telemetry_data: Dict[str, Any]) -> FirmwareResponse:
        return await self._execute_with_protection(
            "module_telemetry_update",
            lambda: self._http_post(f"/api/v1/modules/{module_id}/telemetry/update", telemetry_data)
        )
    
    async def _wrap_success(self, endpoint: str, data: Dict[str, Any]) -> FirmwareResponse:
        start_time = time.time()
        response_time = (time.time() - start_time) * 1000
        self.metrics.record_request(endpoint, "GET", response_time / 1000, True)
        self.circuit_breaker.record_success()
        self.connection_status = True
        self.last_heartbeat = datetime.now(timezone.utc)
        return FirmwareResponse(
            success=True,
            data=data,
            response_time_ms=response_time,
            circuit_breaker_state=self.circuit_breaker.state.value
        )
    
    async def _execute_with_protection(self, operation: str, func: Callable, priority: str = "normal") -> FirmwareResponse:
        """Execute operation với circuit breaker, retry và metrics"""
        start_time = time.time()
        
        try:
            if not self.circuit_breaker.allow_request():
                logger.warning("Circuit breaker OPEN cho operation: %s", operation)
                return FirmwareResponse(
                    success=False,
                    error="Circuit breaker is OPEN - Firmware unavailable",
                    response_time_ms=(time.time() - start_time) * 1000,
                    circuit_breaker_state=self.circuit_breaker.state.value
                )
            
            result = await self.retry_manager.execute_with_retry(operation, func, priority)
            
            response_time = (time.time() - start_time) * 1000
            self.metrics.record_request(operation, "GET", response_time / 1000, True)
            self.circuit_breaker.record_success()
            self.connection_status = True
            self.last_heartbeat = datetime.now(timezone.utc)
            
            logger.debug("%s success trong %.2fms", operation, response_time)
            
            return FirmwareResponse(
                success=True,
                data=result,
                response_time_ms=response_time,
                circuit_breaker_state=self.circuit_breaker.state.value
            )
            
        except Exception as e:
            response_time = (time.time() - start_time) * 1000
            self.metrics.record_request(operation, "GET", response_time / 1000, False)
            self.circuit_breaker.record_failure()
            self.connection_status = False
            
            logger.error("%s failed: %s", operation, e)
            
            return FirmwareResponse(
                success=False,
                error=str(e),
                response_time_ms=response_time,
                circuit_breaker_state=self.circuit_breaker.state.value
            )
    
    async def _http_get(self, endpoint: str) -> Dict[str, Any]:
        """HTTP GET request"""
        response = await self.http_client.get(endpoint)
        response.raise_for_status()
        return response.json()
    
    async def _http_post(self, endpoint: str, data: Dict[str, Any]) -> Dict[str, Any]:
        """HTTP POST request"""
        response = await self.http_client.post(endpoint, json=data)
        response.raise_for_status()
        return response.json()
    
    def _validate_command(self, command: Dict[str, Any]) -> None:
        """Validate robot command"""
        required_fields = ["command_type", "parameters"]
        if not all(field in command for field in required_fields):
            raise ValueError("Missing required fields in command")
        
        valid_commands = ["move", "stop", "pause", "resume", "dock", "undock", "emergency_stop"]
        if command["command_type"] not in valid_commands:
            raise ValueError(f"Invalid command type: {command['command_type']}")
    
    def is_connected(self) -> bool:
        """Check if firmware is connected"""
        return self.connection_status and self.circuit_breaker.state != CircuitBreakerState.OPEN
    
    def get_health_status(self) -> Dict[str, Any]:
        """Get comprehensive health status"""
        return {
            "firmware_connected": self.is_connected(),
            "circuit_breaker_status": self.circuit_breaker.get_status(),
            "last_heartbeat": self.last_heartbeat.isoformat() if self.last_heartbeat else None,
            "connection_status": self.connection_status,
            "firmware_url": self.firmware_url
        }
    
    async def close(self):
        """Close HTTP client"""
        await self.http_client.aclose()
        logger.info("UnifiedFirmwareService closed")

# Global instance
_firmware_service: Optional[UnifiedFirmwareService] = None

async def get_firmware_service() -> UnifiedFirmwareService:
    """Get global firmware service instance"""
    global _firmware_service
    if _firmware_service is None:
        _firmware_service = UnifiedFirmwareService()
    return _firmware_service

async def close_firmware_service():
    """Close global firmware service"""
    global _firmware_service
    if _firmware_service:
        await _firmware_service.close()
        _firmware_service = None
