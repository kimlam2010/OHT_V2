"""
WebSocket Alert Service for Real-time Alert Notifications
Handles real-time alert broadcasting via WebSocket connections
"""

import asyncio
import logging
from typing import Dict, Any, List, Optional
from datetime import datetime, timezone

from app.core.websocket_service import websocket_service, WebSocketMessage
from app.services.alert_system import alert_system, Alert, AlertStatus, AlertSeverity
from app.schemas.dashboard import AlertResponse

logger = logging.getLogger(__name__)


class WebSocketAlertService:
    """
    Service for broadcasting alerts via WebSocket
    Integrates with alert system to provide real-time notifications
    """
    
    def __init__(self):
        self.active_alert_subscriptions = set()
        self.alert_broadcast_queue = asyncio.Queue()
        self.is_running = False
        
        # Register alert handlers
        self._register_alert_handlers()
        
        logger.info("🚨 WebSocketAlertService initialized")
    
    def _register_alert_handlers(self):
        """Register handlers for different alert types"""
        from app.services.alert_system import AlertType
        
        # Register handlers for all alert types
        for alert_type in AlertType:
            alert_system.add_alert_handler(alert_type, self._handle_new_alert)
        
        logger.info("✅ Alert handlers registered for all alert types")
    
    async def start(self):
        """Start WebSocket alert service"""
        if self.is_running:
            logger.warning("⚠️ WebSocket alert service already running")
            return
        
        try:
            self.is_running = True
            
            # Start broadcast worker
            asyncio.create_task(self._alert_broadcast_worker())
            
            logger.info("🚀 WebSocket alert service started")
            
        except Exception as e:
            logger.error(f"❌ Failed to start WebSocket alert service: {e}")
            self.is_running = False
            raise
    
    async def stop(self):
        """Stop WebSocket alert service"""
        self.is_running = False
        logger.info("🛑 WebSocket alert service stopped")
    
    async def _handle_new_alert(self, alert: Alert):
        """Handle new alert from alert system"""
        try:
            # Convert Alert to AlertResponse format
            alert_response = self._convert_alert_to_response(alert)
            
            # Create WebSocket message
            message = WebSocketMessage(
                type="alert_notification",
                data={
                    "alert": alert_response,
                    "action": "created"
                },
                timestamp=datetime.now(timezone.utc)
            )
            
            # Queue for broadcast
            await self.alert_broadcast_queue.put(message)
            
            logger.info(f"🚨 Alert queued for broadcast: {alert.title} ({alert.severity.value})")
            
        except Exception as e:
            logger.error(f"❌ Failed to handle new alert: {e}")
    
    async def broadcast_alert_update(self, alert_id: str, action: str):
        """Broadcast alert status update"""
        try:
            # Get alert from active alerts or history
            alert = alert_system.active_alerts.get(alert_id)
            if not alert:
                # Try to find in history
                for hist_alert in alert_system.alert_history:
                    if hist_alert.alert_id == alert_id:
                        alert = hist_alert
                        break
            
            if not alert:
                logger.warning(f"⚠️ Alert not found for update: {alert_id}")
                return
            
            # Convert to response format
            alert_response = self._convert_alert_to_response(alert)
            
            # Create WebSocket message
            message = WebSocketMessage(
                type="alert_update",
                data={
                    "alert": alert_response,
                    "action": action  # "acknowledged", "resolved", etc.
                },
                timestamp=datetime.now(timezone.utc)
            )
            
            # Broadcast to alert subscribers
            await websocket_service.broadcast_to_subscribers(message, "alerts")
            
            logger.info(f"🔄 Alert update broadcast: {alert.title} - {action}")
            
        except Exception as e:
            logger.error(f"❌ Failed to broadcast alert update: {e}")
    
    async def broadcast_active_alerts(self):
        """Broadcast current active alerts summary"""
        try:
            active_alerts = alert_system.get_active_alerts()
            
            # Convert to response format
            alert_responses = [
                self._convert_alert_to_response(alert) 
                for alert in active_alerts
            ]
            
            # Create WebSocket message
            message = WebSocketMessage(
                type="active_alerts_update",
                data={
                    "alerts": alert_responses,
                    "total_count": len(alert_responses),
                    "critical_count": len([a for a in alert_responses if a["severity"] == "critical"]),
                    "warning_count": len([a for a in alert_responses if a["severity"] in ["high", "medium", "warning"]])
                },
                timestamp=datetime.now(timezone.utc)
            )
            
            # Broadcast to alert subscribers
            await websocket_service.broadcast_to_subscribers(message, "alerts")
            
            logger.debug(f"📊 Active alerts summary broadcast: {len(alert_responses)} alerts")
            
        except Exception as e:
            logger.error(f"❌ Failed to broadcast active alerts: {e}")
    
    async def send_alert_to_client(self, websocket, alert: Alert):
        """Send specific alert to WebSocket client"""
        try:
            alert_response = self._convert_alert_to_response(alert)
            
            message = WebSocketMessage(
                type="alert_notification",
                data={
                    "alert": alert_response,
                    "action": "initial"
                },
                timestamp=datetime.now(timezone.utc)
            )
            
            await websocket_service.send_to_client(websocket, message)
            
        except Exception as e:
            logger.error(f"❌ Failed to send alert to client: {e}")
    
    def _convert_alert_to_response(self, alert: Alert) -> Dict[str, Any]:
        """Convert Alert object to AlertResponse format matching issue requirements"""
        return {
            "id": hash(alert.alert_id) & 0x7FFFFFFF,  # Convert UUID to int
            "title": alert.title,
            "message": alert.message,
            "severity": alert.severity.value,
            "status": alert.status.value,
            "source": alert.source.value,
            "timestamp": alert.created_at.isoformat(),
            "acknowledged_by": alert.acknowledged_by,
            "acknowledged_at": alert.acknowledged_at.isoformat() if alert.acknowledged_at else None,
            "resolved_at": alert.resolved_at.isoformat() if alert.resolved_at else None,
            "metadata": alert.metadata
        }
    
    async def _alert_broadcast_worker(self):
        """Background worker for broadcasting alerts"""
        try:
            while self.is_running:
                try:
                    # Wait for alert message with timeout
                    message = await asyncio.wait_for(
                        self.alert_broadcast_queue.get(), 
                        timeout=1.0
                    )
                    
                    # Broadcast to alert subscribers
                    await websocket_service.broadcast_to_subscribers(message, "alerts")
                    
                    # If critical alert, also broadcast to all connections
                    if (message.data.get("alert", {}).get("severity") in 
                        ["critical", "emergency"]):
                        await websocket_service.broadcast_message(message)
                    
                except asyncio.TimeoutError:
                    # No messages in queue, continue
                    continue
                except Exception as e:
                    logger.error(f"❌ Alert broadcast worker error: {e}")
                    await asyncio.sleep(1)
                    
        except asyncio.CancelledError:
            logger.info("🛑 Alert broadcast worker cancelled")
        except Exception as e:
            logger.error(f"❌ Alert broadcast worker failed: {e}")
    
    async def get_alert_service_stats(self) -> Dict[str, Any]:
        """Get WebSocket alert service statistics"""
        try:
            alert_stats = alert_system.get_alert_statistics()
            
            return {
                "service_status": "running" if self.is_running else "stopped",
                "alert_statistics": alert_stats,
                "queue_size": self.alert_broadcast_queue.qsize(),
                "subscription_count": len(self.active_alert_subscriptions),
                "last_updated": datetime.now(timezone.utc).isoformat()
            }
            
        except Exception as e:
            logger.error(f"❌ Failed to get alert service stats: {e}")
            return {"error": str(e)}


# Global instance
websocket_alert_service = WebSocketAlertService()
