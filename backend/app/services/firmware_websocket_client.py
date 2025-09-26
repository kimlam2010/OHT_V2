"""
DEPRECATED: Firmware WebSocket Client Service

Per Architecture Issue #156, Firmware does NOT provide WebSocket.
This module is kept as a no-op placeholder for backward compatibility
and will log warnings if used. All firmware communication must use HTTP.
"""

import logging
import asyncio
import json
# websockets removed — FW is HTTP-only
from datetime import datetime
from typing import Optional, Dict, Any, Callable
# from websockets.exceptions import ConnectionClosed, WebSocketException

from app.core.websocket_service import websocket_service, WebSocketMessage
from app.services.websocket_rs485_service import websocket_rs485_service
from app.config import settings

logger = logging.getLogger(__name__)


class FirmwareWebSocketClient:
    """
    WebSocket client for connecting to Firmware WebSocket Server
    
    Receives RS485 module telemetry data from Firmware and relays to Frontend
    """
    
    def __init__(self):
        """Initialize as disabled client"""
        self.firmware_url = None
        self.websocket = None
        self.is_connected = False
        self.is_running = False
        self.reconnect_task = None
        self.reconnect_interval = 5.0  # 5 seconds
        self.max_reconnect_attempts = 0
        self.reconnect_attempts = 0
        
        # Statistics
        self.messages_received = 0
        self.messages_relayed = 0
        self.connection_errors = 0
        self.last_message_time = None
        
    async def start(self) -> None:
        """Start Firmware WebSocket client"""
        logger.warning("⚠️ Firmware WebSocket client is deprecated and disabled (FW is HTTP-only).")
        self.is_running = False
        
    async def stop(self) -> None:
        """Stop Firmware WebSocket client"""
        logger.info("🛑 No-op stop: Firmware WebSocket client disabled")
        
    async def _connection_manager(self) -> None:
        logger.warning("⚠️ No-op: Firmware WebSocket connection manager disabled (HTTP-only)")
        await asyncio.sleep(0)
                        
    async def _connect(self) -> None:
        """Connect to Firmware WebSocket Server"""
        logger.warning("⚠️ No-op: Firmware WebSocket connect disabled (HTTP-only)")
            
    async def _listen_for_messages(self) -> None:
        """Listen for messages from Firmware WebSocket"""
        logger.warning("⚠️ No-op: Firmware WebSocket listen disabled (HTTP-only)")
            
    async def _handle_firmware_message(self, raw_message: str) -> None:
        """Handle message received from Firmware WebSocket"""
        logger.warning("⚠️ No-op: Firmware WebSocket handler disabled (HTTP-only)")
            
    async def _relay_telemetry_to_frontend(self, telemetry_message: Dict[str, Any]) -> None:
        """Relay RS485 telemetry data to Frontend clients"""
        try:
            # Extract module info
            data = telemetry_message.get("data", {})
            module_addr = data.get("module_addr", "0x00")
            module_name = data.get("module_name", "Unknown")
            
            logger.info(f"📡 Relaying telemetry: {module_name} ({module_addr})")
            
            # Create WebSocket message for Frontend
            frontend_message = WebSocketMessage(
                type="rs485_telemetry_update",
                data={
                    "module_addr": module_addr,
                    "module_name": module_name,
                    "registers": data.get("registers", []),
                    "timestamp": data.get("timestamp", datetime.now().isoformat()),
                    "source": "firmware_direct"
                },
                timestamp=datetime.now()
            )
            
            # Broadcast to Frontend clients via existing WebSocket service
            await websocket_rs485_service.broadcast_telemetry_update(
                module_addr=int(module_addr, 16),
                telemetry_data=frontend_message.data
            )
            
            self.messages_relayed += 1
            logger.debug(f"✅ Telemetry relayed to Frontend: {module_name}")
            
        except Exception as e:
            logger.error(f"❌ Failed to relay telemetry to Frontend: {e}")
            
    async def _handle_system_status(self, status_message: Dict[str, Any]) -> None:
        """Handle system status update from Firmware"""
        try:
            data = status_message.get("data", {})
            logger.debug(f"📊 Firmware system status: {data}")
            
            # Relay to Frontend if needed
            # This can be extended based on requirements
            
        except Exception as e:
            logger.error(f"❌ Error handling system status: {e}")
            
    async def _handle_module_discovery(self, discovery_message: Dict[str, Any]) -> None:
        """Handle module discovery notification from Firmware"""
        try:
            data = discovery_message.get("data", {})
            module_addr = data.get("address", 0)
            
            logger.info(f"🔍 Firmware discovered new module: 0x{module_addr:02X}")
            
            # Notify Frontend about new module discovery
            frontend_message = WebSocketMessage(
                type="rs485_module_discovered",
                data={
                    "module_addr": f"0x{module_addr:02X}",
                    "timestamp": datetime.now().isoformat()
                },
                timestamp=datetime.now()
            )
            
            # Broadcast discovery notification
            await websocket_rs485_service.broadcast_module_discovery(
                module_addr, frontend_message.data
            )
            
        except Exception as e:
            logger.error(f"❌ Error handling module discovery: {e}")
            
    def get_statistics(self) -> Dict[str, Any]:
        """Get client statistics"""
        return {
            "connected": False,
            "firmware_url": None,
            "messages_received": self.messages_received,
            "messages_relayed": self.messages_relayed,
            "connection_errors": self.connection_errors,
            "reconnect_attempts": self.reconnect_attempts,
            "last_message_time": None
        }
        
    def is_healthy(self) -> bool:
        """Check if client is healthy"""
        return False


# Global Firmware WebSocket Client instance
firmware_websocket_client = FirmwareWebSocketClient()
