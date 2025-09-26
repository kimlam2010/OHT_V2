"""
Safety service for OHT-50 Backend
"""

import logging
from typing import Dict, Any, List
from datetime import datetime, timezone

import os
from app.services.unified_firmware_service import get_firmware_service
from app.core.monitoring import alert_manager, metrics_collector

logger = logging.getLogger(__name__)


class SafetyService:
    """Safety service"""
    
    def __init__(self, use_mock: bool = False):
        self.safety_alerts = []
        self.max_alerts = 100
        
    async def get_safety_status(self) -> Dict[str, Any]:
        """Get current safety status"""
        try:
            service = await get_firmware_service()
            response = await service.get_robot_status()
            data = response.data if response.success and response.data else {}
            
            safety_status = {
                "status": "normal",
                "emergency_stop": data.get("status") == "emergency_stop",
                "obstacles_detected": False,
                "temperature_normal": True,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
            
            if safety_status["emergency_stop"]:
                safety_status["status"] = "emergency"
                await self._create_safety_alert("emergency_stop", "critical", "Emergency stop activated")
            
            return safety_status
            
        except Exception as e:
            logger.error(f"Failed to get safety status: {e}")
            return {"status": "error", "error": str(e)}
    
    async def trigger_emergency_stop(self) -> Dict[str, Any]:
        """Trigger emergency stop (alias for emergency_stop)"""
        return await self.emergency_stop()
    
    async def log_safety_event(self, event_type: str, severity: str, message: str) -> None:
        """Log safety event"""
        await self._create_safety_alert(event_type, severity, message)
    
    async def emergency_stop(self) -> Dict[str, Any]:
        """Execute emergency stop"""
        try:
            service = await get_firmware_service()
            response = await service.emergency_stop()
            
            if response.success:
                await self._create_safety_alert("emergency_stop", "critical", "Emergency stop executed")
                return {"success": True, "status": "emergency_stop"}
            else:
                return {"success": False, "error": response.error or "Emergency stop failed"}
                
        except Exception as e:
            logger.error(f"Emergency stop failed: {e}")
            return {"success": False, "error": str(e)}
    
    async def acknowledge_alert(self, alert_id: str) -> Dict[str, Any]:
        """Acknowledge safety alert"""
        try:
            for alert in self.safety_alerts:
                if alert.get("id") == alert_id:
                    alert["acknowledged"] = True
                    alert["acknowledged_at"] = datetime.now(timezone.utc).isoformat()
                    return {"success": True, "alert": alert}
            return {"success": False, "error": "Alert not found"}
        except Exception as e:
            logger.error(f"Failed to acknowledge alert: {e}")
            return {"success": False, "error": str(e)}
    
    def get_safety_alerts(self, limit: int = 50) -> List[Dict[str, Any]]:
        """Get safety alerts"""
        return self.safety_alerts[-limit:] if limit else self.safety_alerts
    
    async def _create_safety_alert(self, alert_type: str, severity: str, message: str) -> None:
        """Create safety alert"""
        alert = {
            "id": f"safety_{int(datetime.now(timezone.utc).timestamp())}",
            "type": alert_type,
            "severity": severity,
            "message": message,
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "acknowledged": False
        }
        
        self.safety_alerts.append(alert)
        if len(self.safety_alerts) > self.max_alerts:
            self.safety_alerts = self.safety_alerts[-self.max_alerts:]
        
        await alert_manager.create_alert(alert_type, severity, message)
        logger.warning(f"Safety alert: {severity.upper()} - {message}")


# Global instance
safety_service = SafetyService()
