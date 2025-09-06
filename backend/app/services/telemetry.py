"""
Telemetry service for OHT-50 Backend
"""

import logging
from typing import Dict, Any, List
from datetime import datetime, timezone

from app.core.integration import FirmwareIntegrationService
from app.core.monitoring import metrics_collector

logger = logging.getLogger(__name__)


class TelemetryService:
    """Telemetry service"""
    
    def __init__(self, use_mock: bool = True):
        from app.config import settings
        import os
        allow_mock = os.getenv("USE_FIRMWARE_MOCK", "false").lower() == "true" or settings.use_firmware_mock or use_mock
        is_production = settings.environment.lower() == "production"
        if allow_mock and not is_production:
            from app.core.integration import MockFirmwareService
            self.firmware_service = MockFirmwareService()
            logger.warning("🧪 MOCK ENABLED: TelemetryService using MockFirmwareService (non-production)")
        else:
            self.firmware_service = FirmwareIntegrationService()
        self.telemetry_history = []
        self.max_history_size = 1000
        
    async def get_current_telemetry(self) -> Dict[str, Any]:
        """Get current telemetry data"""
        try:
            telemetry = await self.firmware_service.get_telemetry_data()
            
            # Add timestamp
            telemetry["timestamp"] = datetime.now(timezone.utc).isoformat()
            
            # Store in history
            self._store_telemetry(telemetry)
            
            # Record metrics
            metrics_collector.record_telemetry_point("current")
            
            return telemetry
            
        except Exception as e:
            logger.error(f"Failed to get telemetry: {e}")
            return {"error": str(e), "timestamp": datetime.now(timezone.utc).isoformat()}
    
    async def get_telemetry_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get telemetry history"""
        return self.telemetry_history[-limit:] if limit else self.telemetry_history
    
    async def get_module_status(self, module_id: str) -> Dict[str, Any]:
        """Get specific module status"""
        try:
            status = await self.firmware_service.get_module_status(module_id)
            return status
        except Exception as e:
            logger.error(f"Failed to get module status for {module_id}: {e}")
            return {"status": "error", "error": str(e)}
    
    async def discover_modules(self) -> List[Dict[str, Any]]:
        """Discover available modules"""
        try:
            modules = await self.firmware_service.discover_modules()
            return modules
        except Exception as e:
            logger.error(f"Failed to discover modules: {e}")
            return []
    
    def _store_telemetry(self, telemetry: Dict[str, Any]) -> None:
        """Store telemetry data in history"""
        self.telemetry_history.append(telemetry)
        
        # Limit history size
        if len(self.telemetry_history) > self.max_history_size:
            self.telemetry_history = self.telemetry_history[-self.max_history_size:]
    
    def get_telemetry_summary(self) -> Dict[str, Any]:
        """Get telemetry summary statistics"""
        if not self.telemetry_history:
            return {"message": "No telemetry data available"}
        
        latest = self.telemetry_history[-1]
        
        return {
            "latest_timestamp": latest.get("timestamp"),
            "total_records": len(self.telemetry_history),
            "data_points": {
                "motor_speed": latest.get("motor_speed"),
                "motor_temperature": latest.get("motor_temperature"),
                "dock_status": latest.get("dock_status"),
                "safety_status": latest.get("safety_status")
            }
        }


# Global instance
telemetry_service = TelemetryService()
