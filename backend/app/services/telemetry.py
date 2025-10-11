"""
Telemetry service for OHT-50 Backend
"""

import logging
from typing import Dict, Any, List
from datetime import datetime, timezone, timedelta

from app.services.unified_firmware_service import get_firmware_service
from app.core.monitoring import metrics_collector

logger = logging.getLogger(__name__)


class TelemetryService:
    """Telemetry service"""
    
    def __init__(self, use_mock: bool = False):
        self.telemetry_history = []
        # Add validation flag
        self._validated_connection = False
        self.max_history_size = 1000
        
    async def validate_firmware_connection(self) -> bool:
        """Validate firmware connection and warn if using mock data"""
        if not self._validated_connection:
            service = await get_firmware_service()
            self._validated_connection = service.is_connected()
        return self._validated_connection
    
    async def get_current_telemetry(self) -> Dict[str, Any]:
        """Get current telemetry data"""
        try:
            # Check if we should use mock data
            import os
            testing_mode = os.getenv("TESTING", "false").lower() == "true"
            use_mock_env = os.getenv("USE_MOCK_FIRMWARE", "false").lower() == "true"
            is_production = os.getenv("ENVIRONMENT", "development").lower() == "production"
            
            if (testing_mode or use_mock_env) and not is_production:
                # Use mock telemetry data directly
                logger.warning("🧪 MOCK MODE: Using mock telemetry data")
                mock_telemetry = {
                    "data": {
                        "robot_status": {
                            "speed": 1500,
                            "temperature": 45.5,
                            "dock_status": "ready",
                            "safety_status": "normal"
                        },
                        "motor_speed": 1500,
                        "motor_temperature": 45.5,
                        "dock_status": "ready",
                        "safety_status": "normal"
                    },
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
                self._store_telemetry(mock_telemetry)
                metrics_collector.record_telemetry_point("current")
                return mock_telemetry
            
            # Real firmware mode
            await self.validate_firmware_connection()
            service = await get_firmware_service()
            response = await service.get_telemetry_data()
            
            telemetry: Dict[str, Any] = {}
            if response.success and response.data:
                telemetry = response.data
            else:
                telemetry = {"error": response.error or "firmware_unavailable"}
            
            telemetry["timestamp"] = datetime.now(timezone.utc).isoformat()
            
            self._store_telemetry(telemetry)
            metrics_collector.record_telemetry_point("current")
            
            return telemetry
            
        except Exception as e:
            logger.error(f"Failed to get telemetry: {e}")
            return {"error": str(e), "timestamp": datetime.now(timezone.utc).isoformat()}
    
    async def get_telemetry_history(self, limit: int = 100) -> List[Dict[str, Any]]:
        """Get telemetry history"""
        # Generate mock history if empty
        if not self.telemetry_history:
            for i in range(min(limit, 10)):
                mock_data = {
                    "timestamp": (datetime.now(timezone.utc) - timedelta(minutes=i)).isoformat(),
                    "motor_speed": 1500.0 - (i * 10),
                    "motor_temperature": 42.5 + (i * 0.5),
                    "dock_status": "ready",
                    "safety_status": "normal"
                }
                self.telemetry_history.append(mock_data)
        
        return self.telemetry_history[-limit:] if limit else self.telemetry_history
    
    async def get_module_status(self, module_id: str) -> Dict[str, Any]:
        """Get specific module status"""
        try:
            service = await get_firmware_service()
            response = await service.get_modules_status()
            data = response.data if response.success and response.data else {}
            return next((m for m in data.get("modules", []) if str(m.get("module_id")) == str(module_id)), {"status": "unknown"})
        except Exception as e:
            logger.error(f"Failed to get module status for {module_id}: {e}")
            return {"status": "error", "error": str(e)}
    
    async def discover_modules(self) -> List[Dict[str, Any]]:
        """Discover available modules (best-effort from modules status)"""
        try:
            service = await get_firmware_service()
            response = await service.get_modules_status()
            if response.success and response.data:
                modules = response.data.get("modules", [])
                return modules
            return []
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
