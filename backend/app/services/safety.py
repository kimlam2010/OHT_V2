"""
Safety service for OHT-50 Backend
"""

import logging
from typing import Dict, Any, List
from datetime import datetime, timezone

from app.core.integration import FirmwareIntegrationService
from app.core.monitoring import alert_manager, metrics_collector

logger = logging.getLogger(__name__)


class SafetyService:
    """Safety service"""
    
    def __init__(self, use_mock: bool = True):
        if use_mock:
            from app.core.integration import MockFirmwareService
            self.firmware_service = MockFirmwareService()
        else:
            self.firmware_service = FirmwareIntegrationService()
        self.safety_alerts = []
        self.max_alerts = 100
        
    async def get_safety_status(self) -> Dict[str, Any]:
        """Get current safety status"""
        try:
            # Get robot status to check safety
            robot_status = await self.firmware_service.get_robot_status()
            
            safety_status = {
                "status": "normal",
                "emergency_stop": False,
                "obstacles_detected": False,
                "temperature_normal": True,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
            
            # Check for safety issues
            if robot_status.get("status") == "emergency_stop":
                safety_status["status"] = "emergency"
                safety_status["emergency_stop"] = True
                await self._create_safety_alert("emergency_stop", "critical", "Emergency stop activated")
            
            # Check temperature
            if robot_status.get("temperature", 0) > 60:
                safety_status["temperature_normal"] = False
                safety_status["status"] = "warning"
                await self._create_safety_alert("high_temperature", "warning", f"High temperature: {robot_status.get('temperature')}°C")
            
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
            success = await self.firmware_service.emergency_stop()
            
            if success:
                await self._create_safety_alert("emergency_stop", "critical", "Emergency stop executed")
                return {"success": True, "status": "emergency_stop"}
            else:
                return {"success": False, "error": "Emergency stop failed"}
                
        except Exception as e:
            logger.error(f"Emergency stop failed: {e}")
            return {"success": False, "error": str(e)}
    
    async def acknowledge_alert(self, alert_id: str) -> Dict[str, Any]:
        """Acknowledge safety alert"""
        try:
            # Find and update alert
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
        
        # Limit alerts
        if len(self.safety_alerts) > self.max_alerts:
            self.safety_alerts = self.safety_alerts[-self.max_alerts:]
        
        # Send to alert manager
        await alert_manager.create_alert(alert_type, severity, message)
        
        logger.warning(f"Safety alert: {severity.upper()} - {message}")


# Global instance
safety_service = SafetyService()
