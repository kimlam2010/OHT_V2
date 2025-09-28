"""
Network Monitor Service - OHT-50 Backend

Real-time network monitoring and WebSocket integration for network status updates.
"""

import asyncio
import logging
from datetime import datetime, timezone
from typing import Dict, Any, Optional, List, Set
from enum import Enum

from app.services.network_integration_service import network_service, NetworkStatus, WiFiAPStatus
from app.services.websocket_alert_service import websocket_alert_service

logger = logging.getLogger(__name__)


class NetworkEventType(Enum):
    """Network event types"""
    STATUS_CHANGED = "status_changed"
    AP_STARTED = "ap_started"
    AP_STOPPED = "ap_stopped"
    CLIENT_CONNECTED = "client_connected"
    CLIENT_DISCONNECTED = "client_disconnected"
    FALLBACK_ENABLED = "fallback_enabled"
    FALLBACK_DISABLED = "fallback_disabled"
    CONNECTION_ERROR = "connection_error"
    PERFORMANCE_ALERT = "performance_alert"


class NetworkMonitorService:
    """
    Network Monitor Service
    
    Provides real-time network monitoring and WebSocket broadcasting
    """
    
    def __init__(self):
        """Initialize network monitor service"""
        self.is_running = False
        self.monitor_task: Optional[asyncio.Task] = None
        
        # Monitoring state
        self.last_status = NetworkStatus.DISCONNECTED
        self.last_ap_status = WiFiAPStatus.STOPPED
        self.last_client_count = 0
        self.last_fallback_enabled = False
        
        # Performance thresholds
        self.latency_threshold_ms = 100.0
        self.packet_loss_threshold_percent = 5.0
        self.signal_strength_threshold_dbm = -80
        
        # Monitoring intervals
        self.status_check_interval = 5.0  # seconds
        self.performance_check_interval = 30.0  # seconds
        
        # Event tracking
        self.event_history: List[Dict[str, Any]] = []
        self.max_event_history = 100
        
        logger.info("🔍 Network Monitor Service initialized")
    
    async def start_monitoring(self) -> bool:
        """
        Start network monitoring
        
        Returns:
            True if monitoring started successfully
        """
        try:
            if self.is_running:
                logger.warning("⚠️ Network monitoring already running")
                return True
            
            logger.info("🚀 Starting network monitoring...")
            
            # Start monitoring task
            self.monitor_task = asyncio.create_task(self._monitoring_loop())
            self.is_running = True
            
            logger.info("✅ Network monitoring started")
            return True
            
        except Exception as e:
            logger.error(f"❌ Failed to start network monitoring: {e}")
            return False
    
    async def stop_monitoring(self) -> None:
        """Stop network monitoring"""
        try:
            if not self.is_running:
                logger.warning("⚠️ Network monitoring not running")
                return
            
            logger.info("🛑 Stopping network monitoring...")
            
            # Stop monitoring task
            if self.monitor_task:
                self.monitor_task.cancel()
                try:
                    await self.monitor_task
                except asyncio.CancelledError:
                    pass
            
            self.is_running = False
            logger.info("✅ Network monitoring stopped")
            
        except Exception as e:
            logger.error(f"❌ Error stopping network monitoring: {e}")
    
    async def _monitoring_loop(self) -> None:
        """Main monitoring loop"""
        last_performance_check = 0.0
        
        while self.is_running:
            try:
                current_time = asyncio.get_event_loop().time()
                
                # Check network status
                await self._check_network_status()
                
                # Check performance metrics periodically
                if current_time - last_performance_check >= self.performance_check_interval:
                    await self._check_performance_metrics()
                    last_performance_check = current_time
                
                # Wait before next check
                await asyncio.sleep(self.status_check_interval)
                
            except asyncio.CancelledError:
                logger.info("🛑 Network monitoring loop cancelled")
                break
            except Exception as e:
                logger.error(f"❌ Network monitoring error: {e}")
                await asyncio.sleep(5)  # Wait before retry
    
    async def _check_network_status(self) -> None:
        """Check network status and detect changes"""
        try:
            # Get current network status
            status_result = await network_service.get_network_status()
            
            if not status_result.get("success"):
                logger.warning("⚠️ Failed to get network status from service")
                return
            
            data = status_result.get("data", {})
            current_status = NetworkStatus(data.get("status", "disconnected"))
            current_ap_status = WiFiAPStatus(data.get("ap_status", "stopped"))
            current_fallback_enabled = data.get("fallback_enabled", False)
            
            # Get current client count
            clients_result = await network_service.get_ap_clients()
            current_client_count = 0
            if clients_result.get("success"):
                current_client_count = clients_result.get("data", {}).get("client_count", 0)
            
            # Check for status changes
            if current_status != self.last_status:
                await self._handle_status_change(self.last_status, current_status)
                self.last_status = current_status
            
            # Check for AP status changes
            if current_ap_status != self.last_ap_status:
                await self._handle_ap_status_change(self.last_ap_status, current_ap_status)
                self.last_ap_status = current_ap_status
            
            # Check for client count changes
            if current_client_count != self.last_client_count:
                await self._handle_client_count_change(self.last_client_count, current_client_count)
                self.last_client_count = current_client_count
            
            # Check for fallback status changes
            if current_fallback_enabled != self.last_fallback_enabled:
                await self._handle_fallback_status_change(self.last_fallback_enabled, current_fallback_enabled)
                self.last_fallback_enabled = current_fallback_enabled
                
        except Exception as e:
            logger.error(f"❌ Error checking network status: {e}")
    
    async def _check_performance_metrics(self) -> None:
        """Check network performance metrics"""
        try:
            # Get network status for performance data
            status_result = await network_service.get_network_status()
            
            if not status_result.get("success"):
                return
            
            data = status_result.get("data", {})
            
            # Check latency
            latency_ms = data.get("latency_ms")
            if latency_ms and latency_ms > self.latency_threshold_ms:
                await self._handle_performance_alert("high_latency", {
                    "latency_ms": latency_ms,
                    "threshold_ms": self.latency_threshold_ms
                })
            
            # Check packet loss
            packet_loss_percent = data.get("packet_loss_percent")
            if packet_loss_percent and packet_loss_percent > self.packet_loss_threshold_percent:
                await self._handle_performance_alert("high_packet_loss", {
                    "packet_loss_percent": packet_loss_percent,
                    "threshold_percent": self.packet_loss_threshold_percent
                })
            
            # Check signal strength (for WiFi)
            signal_strength = data.get("signal_strength")
            if signal_strength and signal_strength < self.signal_strength_threshold_dbm:
                await self._handle_performance_alert("weak_signal", {
                    "signal_strength_dbm": signal_strength,
                    "threshold_dbm": self.signal_strength_threshold_dbm
                })
                
        except Exception as e:
            logger.error(f"❌ Error checking performance metrics: {e}")
    
    async def _handle_status_change(self, old_status: NetworkStatus, new_status: NetworkStatus) -> None:
        """Handle network status change"""
        event_data = {
            "old_status": old_status.value,
            "new_status": new_status.value,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        await self._broadcast_network_event(NetworkEventType.STATUS_CHANGED, event_data)
        logger.info(f"📊 Network status changed: {old_status.value} → {new_status.value}")
    
    async def _handle_ap_status_change(self, old_status: WiFiAPStatus, new_status: WiFiAPStatus) -> None:
        """Handle WiFi AP status change"""
        event_type = None
        if new_status == WiFiAPStatus.RUNNING and old_status != WiFiAPStatus.RUNNING:
            event_type = NetworkEventType.AP_STARTED
        elif new_status == WiFiAPStatus.STOPPED and old_status != WiFiAPStatus.STOPPED:
            event_type = NetworkEventType.AP_STOPPED
        
        if event_type:
            event_data = {
                "old_status": old_status.value,
                "new_status": new_status.value,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
            
            await self._broadcast_network_event(event_type, event_data)
            logger.info(f"📡 WiFi AP status changed: {old_status.value} → {new_status.value}")
    
    async def _handle_client_count_change(self, old_count: int, new_count: int) -> None:
        """Handle AP client count change"""
        if new_count > old_count:
            # Client connected
            event_data = {
                "client_count": new_count,
                "change": new_count - old_count,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
            await self._broadcast_network_event(NetworkEventType.CLIENT_CONNECTED, event_data)
            logger.info(f"📱 AP client connected: {old_count} → {new_count}")
        elif new_count < old_count:
            # Client disconnected
            event_data = {
                "client_count": new_count,
                "change": old_count - new_count,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
            await self._broadcast_network_event(NetworkEventType.CLIENT_DISCONNECTED, event_data)
            logger.info(f"📱 AP client disconnected: {old_count} → {new_count}")
    
    async def _handle_fallback_status_change(self, old_enabled: bool, new_enabled: bool) -> None:
        """Handle fallback status change"""
        event_type = None
        if new_enabled and not old_enabled:
            event_type = NetworkEventType.FALLBACK_ENABLED
        elif not new_enabled and old_enabled:
            event_type = NetworkEventType.FALLBACK_DISABLED
        
        if event_type:
            event_data = {
                "fallback_enabled": new_enabled,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
            
            await self._broadcast_network_event(event_type, event_data)
            logger.info(f"🔄 Fallback status changed: {old_enabled} → {new_enabled}")
    
    async def _handle_performance_alert(self, alert_type: str, data: Dict[str, Any]) -> None:
        """Handle performance alert"""
        event_data = {
            "alert_type": alert_type,
            "data": data,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        await self._broadcast_network_event(NetworkEventType.PERFORMANCE_ALERT, event_data)
        logger.warning(f"⚠️ Network performance alert: {alert_type} - {data}")
    
    async def _broadcast_network_event(self, event_type: NetworkEventType, event_data: Dict[str, Any]) -> None:
        """Broadcast network event via WebSocket"""
        try:
            # Create event message
            message = {
                "type": "network_event",
                "event_type": event_type.value,
                "data": event_data,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
            
            # Broadcast via WebSocket alert service
            await websocket_alert_service.broadcast_alert(message)
            
            # Add to event history
            self._add_event_to_history(event_type, event_data)
            
        except Exception as e:
            logger.error(f"❌ Error broadcasting network event: {e}")
    
    def _add_event_to_history(self, event_type: NetworkEventType, event_data: Dict[str, Any]) -> None:
        """Add event to history"""
        event = {
            "event_type": event_type.value,
            "event_data": event_data,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        self.event_history.append(event)
        
        # Keep only recent events
        if len(self.event_history) > self.max_event_history:
            self.event_history = self.event_history[-self.max_event_history:]
    
    async def get_network_events(self, limit: int = 50) -> List[Dict[str, Any]]:
        """
        Get recent network events
        
        Args:
            limit: Maximum number of events to return
            
        Returns:
            List of recent network events
        """
        return self.event_history[-limit:] if self.event_history else []
    
    async def get_monitoring_status(self) -> Dict[str, Any]:
        """
        Get monitoring service status
        
        Returns:
            Monitoring status information
        """
        return {
            "is_running": self.is_running,
            "last_status": self.last_status.value,
            "last_ap_status": self.last_ap_status.value,
            "last_client_count": self.last_client_count,
            "last_fallback_enabled": self.last_fallback_enabled,
            "event_count": len(self.event_history),
            "status_check_interval": self.status_check_interval,
            "performance_check_interval": self.performance_check_interval,
            "latency_threshold_ms": self.latency_threshold_ms,
            "packet_loss_threshold_percent": self.packet_loss_threshold_percent,
            "signal_strength_threshold_dbm": self.signal_strength_threshold_dbm
        }


# Global network monitor instance
network_monitor = NetworkMonitorService()
