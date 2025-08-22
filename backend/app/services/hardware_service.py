"""
Hardware Service cho OHT-50 Backend
Quản lý LED, E-Stop, Relay, GPIO từ Backend
"""

import asyncio
import structlog
from datetime import datetime
from typing import Dict, List, Optional, Any
from uuid import UUID, uuid4

from app.services.log_service import LogService
from app.services.hardware_communication import HardwareCommunicationService

logger = structlog.get_logger(__name__)


class HardwareService:
    """Service quản lý hardware của OHT-50"""
    
    def __init__(self, log_service: LogService):
        self.log_service = log_service
        self.initialized = False
        
        # Hardware communication service
        self.communication_service = HardwareCommunicationService(log_service)
        
        # Hardware status
        self.led_status = {
            "power": False,
            "system": False, 
            "comm": False,
            "network": False,
            "error": False
        }
        
        self.estop_status = {
            "triggered": False,
            "last_triggered": None,
            "reset_count": 0
        }
        
        self.relay_status = {
            "relay1": False,
            "relay2": False,
            "last_updated": None
        }
        
        # Hardware events
        self.hardware_events = []
        
    async def initialize(self) -> bool:
        """Khởi tạo hardware service"""
        try:
            logger.info("Initializing Hardware Service")
            
            # Initialize communication service
            if not await self.communication_service.initialize():
                logger.error("Failed to initialize communication service")
                return False
            
            # Register event handlers
            self.communication_service.register_event_handler(
                "hardware_status", self._handle_hardware_status
            )
            self.communication_service.register_event_handler(
                "estop_triggered", self._handle_estop_triggered
            )
            self.communication_service.register_event_handler(
                "led_state_changed", self._handle_led_state_changed
            )
            self.communication_service.register_event_handler(
                "relay_state_changed", self._handle_relay_state_changed
            )
            self.communication_service.register_event_handler(
                "hardware_error", self._handle_hardware_error
            )
            
            # Get initial hardware status
            await self._update_hardware_status()
            
            self.initialized = True
            logger.info("Hardware Service initialized successfully")
            return True
            
        except Exception as e:
            logger.error(f"Failed to initialize Hardware Service: {e}")
            return False
    
    async def shutdown(self):
        """Shutdown hardware service"""
        try:
            logger.info("Shutting down Hardware Service")
            
            if self.communication_service:
                await self.communication_service.shutdown()
            
            self.initialized = False
            logger.info("Hardware Service shutdown complete")
            
        except Exception as e:
            logger.error(f"Error during shutdown: {e}")
    
    async def _update_hardware_status(self):
        """Cập nhật trạng thái hardware"""
        try:
            status = await self.communication_service.get_hardware_status()
            if "error" not in status:
                # Update LED status
                if "leds" in status:
                    for led_id, led_state in status["leds"].items():
                        if led_id in self.led_status:
                            self.led_status[led_id] = led_state
                
                # Update E-Stop status
                if "estop" in status:
                    self.estop_status["triggered"] = status["estop"].get("triggered", False)
                    if status["estop"].get("last_triggered"):
                        self.estop_status["last_triggered"] = status["estop"]["last_triggered"]
                
                # Update relay status
                if "relays" in status:
                    for relay_id, relay_state in status["relays"].items():
                        if relay_id in self.relay_status:
                            self.relay_status[relay_id] = relay_state
                    self.relay_status["last_updated"] = datetime.utcnow().isoformat()
                
        except Exception as e:
            logger.error(f"Failed to update hardware status: {e}")
    
    async def _handle_hardware_status(self, data: Dict[str, Any]):
        """Xử lý hardware status event"""
        try:
            await self._update_hardware_status()
            self._add_hardware_event("hardware_status", data)
        except Exception as e:
            logger.error(f"Error handling hardware status event: {e}")
    
    async def _handle_estop_triggered(self, data: Dict[str, Any]):
        """Xử lý E-Stop triggered event"""
        try:
            self.estop_status["triggered"] = True
            self.estop_status["last_triggered"] = datetime.utcnow().isoformat()
            self._add_hardware_event("estop_triggered", data)
            logger.warning("E-Stop triggered!")
        except Exception as e:
            logger.error(f"Error handling E-Stop event: {e}")
    
    async def _handle_led_state_changed(self, data: Dict[str, Any]):
        """Xử lý LED state changed event"""
        try:
            led_id = data.get("led_id")
            state = data.get("state")
            if led_id in self.led_status:
                self.led_status[led_id] = state
            self._add_hardware_event("led_state_changed", data)
        except Exception as e:
            logger.error(f"Error handling LED state change event: {e}")
    
    async def _handle_relay_state_changed(self, data: Dict[str, Any]):
        """Xử lý relay state changed event"""
        try:
            relay_id = data.get("relay_id")
            state = data.get("state")
            if relay_id in self.relay_status:
                self.relay_status[relay_id] = state
            self.relay_status["last_updated"] = datetime.utcnow().isoformat()
            self._add_hardware_event("relay_state_changed", data)
        except Exception as e:
            logger.error(f"Error handling relay state change event: {e}")
    
    async def _handle_hardware_error(self, data: Dict[str, Any]):
        """Xử lý hardware error event"""
        try:
            self._add_hardware_event("hardware_error", data)
            logger.error(f"Hardware error: {data}")
        except Exception as e:
            logger.error(f"Error handling hardware error event: {e}")
    
    def _add_hardware_event(self, event_type: str, data: Dict[str, Any]):
        """Thêm hardware event vào history"""
        event = {
            "id": str(uuid4()),
            "type": event_type,
            "data": data,
            "timestamp": datetime.utcnow().isoformat()
        }
        self.hardware_events.append(event)
        
        # Keep only last 100 events
        if len(self.hardware_events) > 100:
            self.hardware_events = self.hardware_events[-100:]
    
    async def get_hardware_status(self) -> Dict[str, Any]:
        """Lấy trạng thái hardware"""
        try:
            await self._update_hardware_status()
            
            return {
                "leds": self.led_status,
                "estop": self.estop_status,
                "relays": self.relay_status,
                "communication": await self.communication_service.get_connection_status(),
                "last_updated": datetime.utcnow().isoformat()
            }
        except Exception as e:
            logger.error(f"Failed to get hardware status: {e}")
            return {"error": str(e)}
    
    async def set_led_state(self, led_id: str, state: bool) -> Dict[str, Any]:
        """Set LED state"""
        try:
            if led_id not in self.led_status:
                return {"error": f"Invalid LED ID: {led_id}"}
            
            result = await self.communication_service.set_led_state(led_id, state)
            
            if "error" not in result:
                self.led_status[led_id] = state
                self._add_hardware_event("led_control", {
                    "led_id": led_id,
                    "state": state,
                    "success": True
                })
            
            return result
        except Exception as e:
            logger.error(f"Failed to set LED state: {e}")
            return {"error": str(e)}
    
    async def get_estop_status(self) -> Dict[str, Any]:
        """Lấy trạng thái E-Stop"""
        try:
            result = await self.communication_service.get_estop_status()
            
            if "error" not in result and "estop" in result:
                self.estop_status.update(result["estop"])
            
            return {
                "estop": self.estop_status,
                "communication_result": result
            }
        except Exception as e:
            logger.error(f"Failed to get E-Stop status: {e}")
            return {"error": str(e)}
    
    async def reset_estop(self) -> Dict[str, Any]:
        """Reset E-Stop"""
        try:
            result = await self.communication_service.send_hardware_command("reset_estop", {})
            
            if "error" not in result:
                self.estop_status["triggered"] = False
                self.estop_status["reset_count"] += 1
                self._add_hardware_event("estop_reset", {"success": True})
            
            return result
        except Exception as e:
            logger.error(f"Failed to reset E-Stop: {e}")
            return {"error": str(e)}
    
    async def set_relay_state(self, relay_id: str, state: bool) -> Dict[str, Any]:
        """Set relay state"""
        try:
            if relay_id not in self.relay_status:
                return {"error": f"Invalid relay ID: {relay_id}"}
            
            result = await self.communication_service.set_relay_state(relay_id, state)
            
            if "error" not in result:
                self.relay_status[relay_id] = state
                self.relay_status["last_updated"] = datetime.utcnow().isoformat()
                self._add_hardware_event("relay_control", {
                    "relay_id": relay_id,
                    "state": state,
                    "success": True
                })
            
            return result
        except Exception as e:
            logger.error(f"Failed to set relay state: {e}")
            return {"error": str(e)}
    
    async def get_hardware_events(self, limit: int = 50) -> List[Dict[str, Any]]:
        """Lấy hardware events"""
        try:
            return self.hardware_events[-limit:] if limit > 0 else self.hardware_events
        except Exception as e:
            logger.error(f"Failed to get hardware events: {e}")
            return []
    
    async def clear_hardware_events(self) -> Dict[str, Any]:
        """Xóa hardware events"""
        try:
            self.hardware_events.clear()
            return {"success": True, "message": "Hardware events cleared"}
        except Exception as e:
            logger.error(f"Failed to clear hardware events: {e}")
            return {"error": str(e)}
    
    async def get_hardware_health(self) -> Dict[str, Any]:
        """Lấy hardware health metrics"""
        try:
            comm_status = await self.communication_service.get_connection_status()
            
            health_score = 100
            
            # Check communication
            if not comm_status.get("firmware_connected", False):
                health_score -= 30
            
            # Check E-Stop
            if self.estop_status.get("triggered", False):
                health_score -= 20
            
            # Check LED status
            led_errors = sum(1 for led in self.led_status.values() if led is None)
            health_score -= led_errors * 5
            
            # Check relay status
            relay_errors = sum(1 for relay in self.relay_status.values() if relay is None)
            health_score -= relay_errors * 10
            
            return {
                "health_score": max(0, health_score),
                "status": "healthy" if health_score >= 80 else "warning" if health_score >= 50 else "critical",
                "communication": comm_status,
                "estop_status": self.estop_status,
                "led_errors": led_errors,
                "relay_errors": relay_errors,
                "last_updated": datetime.utcnow().isoformat()
            }
        except Exception as e:
            logger.error(f"Failed to get hardware health: {e}")
            return {"error": str(e)}
