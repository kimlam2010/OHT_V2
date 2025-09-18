"""
Firmware WebSocket Client Service for OHT-50 Backend
Connects to Firmware WebSocket Server to receive RS485 telemetry data
"""

import logging
import asyncio
import json
import websockets
from datetime import datetime
from typing import Optional, Dict, Any, Callable
from websockets.exceptions import ConnectionClosed, WebSocketException

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
        """Initialize Firmware WebSocket client"""
        self.firmware_url = "ws://127.0.0.1:8081"  # Firmware WebSocket Server
        self.websocket = None
        self.is_connected = False
        self.is_running = False
        self.reconnect_task = None
        self.reconnect_interval = 5.0  # 5 seconds
        self.max_reconnect_attempts = 10
        self.reconnect_attempts = 0
        
        # Statistics
        self.messages_received = 0
        self.messages_relayed = 0
        self.connection_errors = 0
        self.last_message_time = None
        
    async def start(self) -> None:
        """Start Firmware WebSocket client"""
        if self.is_running:
            logger.warning("🔌 Firmware WebSocket client already running")
            return
            
        logger.info("🚀 Starting Firmware WebSocket client...")
        self.is_running = True
        
        # Start connection task
        self.reconnect_task = asyncio.create_task(self._connection_manager())
        
        logger.info("✅ Firmware WebSocket client started")
        
    async def stop(self) -> None:
        """Stop Firmware WebSocket client"""
        if not self.is_running:
            return
            
        logger.info("🛑 Stopping Firmware WebSocket client...")
        self.is_running = False
        
        # Cancel reconnect task
        if self.reconnect_task:
            self.reconnect_task.cancel()
            try:
                await self.reconnect_task
            except asyncio.CancelledError:
                pass
                
        # Close connection
        if self.websocket:
            await self.websocket.close()
            self.websocket = None
            self.is_connected = False
            
        logger.info("✅ Firmware WebSocket client stopped")
        
    async def _connection_manager(self) -> None:
        """Manage WebSocket connection with auto-reconnect"""
        while self.is_running:
            try:
                if not self.is_connected:
                    await self._connect()
                    
                if self.is_connected:
                    await self._listen_for_messages()
                    
            except Exception as e:
                logger.error(f"❌ Firmware WebSocket connection error: {e}")
                self.connection_errors += 1
                self.is_connected = False
                
                if self.websocket:
                    await self.websocket.close()
                    self.websocket = None
                
                # Wait before reconnecting
                if self.is_running:
                    self.reconnect_attempts += 1
                    if self.reconnect_attempts <= self.max_reconnect_attempts:
                        logger.info(f"🔄 Reconnecting in {self.reconnect_interval}s (attempt {self.reconnect_attempts}/{self.max_reconnect_attempts})")
                        await asyncio.sleep(self.reconnect_interval)
                    else:
                        logger.error(f"❌ Max reconnection attempts ({self.max_reconnect_attempts}) exceeded")
                        self.is_running = False
                        
    async def _connect(self) -> None:
        """Connect to Firmware WebSocket Server"""
        try:
            logger.info(f"🔌 Connecting to Firmware WebSocket: {self.firmware_url}")
            
            # Connect to firmware WebSocket server
            self.websocket = await websockets.connect(
                self.firmware_url,
                timeout=10.0,
                ping_interval=30.0,
                ping_timeout=10.0
            )
            
            self.is_connected = True
            self.reconnect_attempts = 0
            
            logger.info("✅ Connected to Firmware WebSocket Server")
            
            # Send initial subscription message
            subscribe_message = {
                "type": "subscribe",
                "data": {
                    "subscription": "rs485_telemetry",
                    "client_id": "backend_relay"
                }
            }
            
            await self.websocket.send(json.dumps(subscribe_message))
            logger.info("📡 Subscribed to RS485 telemetry stream")
            
        except Exception as e:
            logger.error(f"❌ Failed to connect to Firmware WebSocket: {e}")
            self.is_connected = False
            raise
            
    async def _listen_for_messages(self) -> None:
        """Listen for messages from Firmware WebSocket"""
        try:
            async for message in self.websocket:
                await self._handle_firmware_message(message)
                
        except ConnectionClosed:
            logger.warning("🔌 Firmware WebSocket connection closed")
            self.is_connected = False
        except WebSocketException as e:
            logger.error(f"❌ Firmware WebSocket error: {e}")
            self.is_connected = False
            raise
            
    async def _handle_firmware_message(self, raw_message: str) -> None:
        """Handle message received from Firmware WebSocket"""
        try:
            self.messages_received += 1
            self.last_message_time = datetime.now()
            
            # Parse JSON message
            message = json.loads(raw_message)
            message_type = message.get("type")
            
            logger.debug(f"📨 Received from Firmware: {message_type}")
            
            if message_type == "telemetry":
                # This is RS485 module telemetry data - relay to Frontend
                await self._relay_telemetry_to_frontend(message)
                
            elif message_type == "system_status":
                # System status update
                await self._handle_system_status(message)
                
            elif message_type == "module_discovered":
                # New module discovered
                await self._handle_module_discovery(message)
                
            elif message_type == "heartbeat":
                # Firmware heartbeat - just log
                logger.debug("💓 Firmware heartbeat received")
                
            else:
                logger.debug(f"🤷 Unknown message type from Firmware: {message_type}")
                
        except json.JSONDecodeError as e:
            logger.error(f"❌ Invalid JSON from Firmware: {e}")
        except Exception as e:
            logger.error(f"❌ Error handling Firmware message: {e}")
            
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
            "connected": self.is_connected,
            "firmware_url": self.firmware_url,
            "messages_received": self.messages_received,
            "messages_relayed": self.messages_relayed,
            "connection_errors": self.connection_errors,
            "reconnect_attempts": self.reconnect_attempts,
            "last_message_time": self.last_message_time.isoformat() if self.last_message_time else None
        }
        
    def is_healthy(self) -> bool:
        """Check if client is healthy"""
        if not self.is_connected:
            return False
            
        # Check if we received messages recently (within 30 seconds)
        if self.last_message_time:
            time_since_last = (datetime.now() - self.last_message_time).total_seconds()
            return time_since_last < 30.0
            
        return True


# Global Firmware WebSocket Client instance
firmware_websocket_client = FirmwareWebSocketClient()
