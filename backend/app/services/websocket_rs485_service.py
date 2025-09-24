"""
WebSocket RS485 Service for OHT-50 Backend
Real-time RS485 modules data streaming
"""

import logging
import asyncio
import json
from datetime import datetime
from typing import Dict, Any, List, Set
from fastapi import WebSocket

from app.core.websocket_service import websocket_service, WebSocketMessage
from app.services.rs485_service import rs485_service

logger = logging.getLogger(__name__)


class WebSocketRS485Service:
    """
    WebSocket service for real-time RS485 modules data
    
    Provides real-time streaming of:
    - RS485 modules status
    - Bus health metrics
    - Module telemetry data
    - Discovery progress
    - Module alerts
    """
    
    def __init__(self):
        """Initialize WebSocket RS485 service"""
        self.connections: Set[WebSocket] = set()
        self.is_running = False
        self.update_task = None
        self.update_interval = 2.0  # 2 seconds
        
    async def start(self) -> None:
        """Start WebSocket RS485 service"""
        if self.is_running:
            logger.warning("📡 WebSocket RS485 service already running")
            return
            
        logger.info("🚀 Starting WebSocket RS485 service...")
        self.is_running = True
        
        # Start background update task
        self.update_task = asyncio.create_task(self._background_updates())
        
        logger.info("✅ WebSocket RS485 service started")
        
    async def stop(self) -> None:
        """Stop WebSocket RS485 service"""
        if not self.is_running:
            return
            
        logger.info("🛑 Stopping WebSocket RS485 service...")
        self.is_running = False
        
        # Cancel update task
        if self.update_task:
            self.update_task.cancel()
            try:
                await self.update_task
            except asyncio.CancelledError:
                pass
                
        # Close all connections
        for websocket in self.connections.copy():
            try:
                await websocket.close()
            except Exception as e:
                logger.error(f"❌ Error closing WebSocket connection: {e}")
                
        self.connections.clear()
        logger.info("✅ WebSocket RS485 service stopped")
        
    async def add_connection(self, websocket: WebSocket) -> None:
        """Add WebSocket connection"""
        self.connections.add(websocket)
        logger.info(f"📡 RS485 WebSocket connection added - Total: {len(self.connections)}")
        
        # Send initial data to new connection
        await self._send_initial_data(websocket)
        
    async def remove_connection(self, websocket: WebSocket) -> None:
        """Remove WebSocket connection"""
        self.connections.discard(websocket)
        logger.info(f"📡 RS485 WebSocket connection removed - Total: {len(self.connections)}")
        
    async def _send_initial_data(self, websocket: WebSocket) -> None:
        """Send initial RS485 data to new connection"""
        try:
            # Get current modules and bus health
            modules = await rs485_service.get_modules()
            bus_health = await rs485_service.get_bus_health()
            
            # Send initial data
            message = WebSocketMessage(
                type="rs485_initial",
                data={
                    "modules": [module.model_dump() for module in modules],
                    "bus_health": bus_health.model_dump(),
                    "timestamp": datetime.now().isoformat()
                },
                timestamp=datetime.now()
            )
            
            await websocket.send_text(message.json())
            logger.info("📨 Sent initial RS485 data to new client")
            
        except Exception as e:
            logger.error(f"❌ Failed to send initial RS485 data: {e}")
            
    async def _background_updates(self) -> None:
        """Background task to send periodic RS485 updates"""
        logger.info(f"🔄 Starting RS485 background updates (every {self.update_interval}s)")
        
        while self.is_running:
            try:
                if self.connections:
                    await self._broadcast_rs485_update()
                    
                await asyncio.sleep(self.update_interval)
                
            except asyncio.CancelledError:
                break
            except Exception as e:
                logger.error(f"❌ RS485 background update error: {e}")
                await asyncio.sleep(5)  # Wait before retry
                
        logger.info("🛑 RS485 background updates stopped")
        
    async def _broadcast_rs485_update(self) -> None:
        """Broadcast RS485 data to all connected clients"""
        try:
            # Get current data
            modules = await rs485_service.get_modules()
            bus_health = await rs485_service.get_bus_health()
            
            # Create update message
            message = WebSocketMessage(
                type="rs485_update",
                data={
                    "modules": [module.model_dump() for module in modules],
                    "bus_health": bus_health.model_dump(),
                    "timestamp": datetime.now().isoformat()
                },
                timestamp=datetime.now()
            )
            
            # Send to all connections
            disconnected = set()
            for websocket in self.connections:
                try:
                    await websocket.send_text(message.json())
                except Exception as e:
                    logger.warning(f"⚠️ Failed to send RS485 update to client: {e}")
                    disconnected.add(websocket)
                    
            # Remove disconnected clients
            for websocket in disconnected:
                self.connections.discard(websocket)
                
            if disconnected:
                logger.info(f"🧹 Removed {len(disconnected)} disconnected RS485 clients")
                
        except Exception as e:
            logger.error(f"❌ Failed to broadcast RS485 update: {e}")
            
    async def broadcast_module_action(self, action: str, module_addr: int, result: Dict[str, Any]) -> None:
        """Broadcast module action result to all clients"""
        try:
            message = WebSocketMessage(
                type="rs485_module_action",
                data={
                    "action": action,
                    "module_addr": f"0x{module_addr:02X}",
                    "result": result,
                    "timestamp": datetime.now().isoformat()
                },
                timestamp=datetime.now()
            )
            
            # Send to all connections
            for websocket in self.connections:
                try:
                    await websocket.send_text(message.json())
                except Exception as e:
                    logger.warning(f"⚠️ Failed to send module action to client: {e}")
                    
            logger.info(f"📡 Broadcasted module action: {action} on 0x{module_addr:02X}")
            
        except Exception as e:
            logger.error(f"❌ Failed to broadcast module action: {e}")
            
    async def broadcast_discovery_update(self, discovery_status: Dict[str, Any]) -> None:
        """Broadcast discovery progress to all clients"""
        try:
            message = WebSocketMessage(
                type="rs485_discovery",
                data=discovery_status,
                timestamp=datetime.now()
            )
            
            # Send to all connections
            for websocket in self.connections:
                try:
                    await websocket.send_text(message.json())
                except Exception as e:
                    logger.warning(f"⚠️ Failed to send discovery update to client: {e}")
                    
            logger.info(f"📡 Broadcasted discovery update: {discovery_status.get('status_message', 'Unknown')}")
            
        except Exception as e:
            logger.error(f"❌ Failed to broadcast discovery update: {e}")
            
    async def broadcast_bus_health(self, bus_health: Dict[str, Any]) -> None:
        """Broadcast bus health update to all clients"""
        try:
            message = WebSocketMessage(
                type="rs485_bus_health",
                data=bus_health,
                timestamp=datetime.now()
            )
            
            # Send to all connections
            for websocket in self.connections:
                try:
                    await websocket.send_text(message.json())
                except Exception as e:
                    logger.warning(f"⚠️ Failed to send bus health to client: {e}")
                    
            logger.info("📡 Broadcasted bus health update")
            
        except Exception as e:
            logger.error(f"❌ Failed to broadcast bus health: {e}")
            
    async def broadcast_telemetry_update(self, module_addr: int, telemetry_data: Dict[str, Any]) -> None:
        """Broadcast telemetry update to all clients - Issue #91"""
        try:
            message = WebSocketMessage(
                type="rs485_telemetry_update",
                data={
                    "module_addr": f"0x{module_addr:02X}",
                    "telemetry": telemetry_data,
                    "timestamp": datetime.now().isoformat()
                },
                timestamp=datetime.now()
            )
            
            # Send to all connections
            for websocket in self.connections:
                try:
                    await websocket.send_text(message.json())
                except Exception as e:
                    logger.warning(f"⚠️ Failed to send telemetry update to client: {e}")
                    
            logger.info(f"📡 Broadcasted telemetry update for module 0x{module_addr:02X}")
            
        except Exception as e:
            logger.error(f"❌ Failed to broadcast telemetry update: {e}")
            
    async def broadcast_register_update(self, module_addr: int, register_addr: str, update_result: Dict[str, Any]) -> None:
        """Broadcast register update result to all clients - Issue #91"""
        try:
            message = WebSocketMessage(
                type="rs485_register_update",
                data={
                    "module_addr": f"0x{module_addr:02X}",
                    "register_address": register_addr,
                    "update_result": update_result,
                    "timestamp": datetime.now().isoformat()
                },
                timestamp=datetime.now()
            )
            
            # Send to all connections
            for websocket in self.connections:
                try:
                    await websocket.send_text(message.json())
                except Exception as e:
                    logger.warning(f"⚠️ Failed to send register update to client: {e}")
                    
            logger.info(f"📡 Broadcasted register update: {register_addr} on module 0x{module_addr:02X}")
            
        except Exception as e:
            logger.error(f"❌ Failed to broadcast register update: {e}")
            
    async def broadcast_scan_status(self, status: str, reason: str = None, timestamp: datetime = None) -> None:
        """Broadcast module scan status update to all clients - Issue #147"""
        try:
            message = WebSocketMessage(
                type="rs485_scan_status",
                data={
                    "status": status,
                    "reason": reason or "Status change",
                    "timestamp": (timestamp or datetime.now()).isoformat()
                },
                timestamp=timestamp or datetime.now()
            )
            
            # Send to all connections
            for websocket in self.connections:
                try:
                    await websocket.send_text(message.json())
                except Exception as e:
                    logger.warning(f"⚠️ Failed to send scan status to client: {e}")
                    
            logger.info(f"📡 Broadcasted scan status update: {status}")
            
        except Exception as e:
            logger.error(f"❌ Failed to broadcast scan status: {e}")
            
    async def broadcast_module_discovery(self, module_addr: int, discovery_data: Dict[str, Any]) -> None:
        """Broadcast module discovery notification to all clients - Issue #90"""
        try:
            message = WebSocketMessage(
                type="rs485_module_discovered",
                data={
                    "module_addr": f"0x{module_addr:02X}",
                    "discovery_data": discovery_data,
                    "timestamp": datetime.now().isoformat()
                },
                timestamp=datetime.now()
            )
            
            # Send to all connections
            for websocket in self.connections:
                try:
                    await websocket.send_text(message.json())
                except Exception as e:
                    logger.warning(f"⚠️ Failed to send module discovery to client: {e}")
                    
            logger.info(f"📡 Broadcasted module discovery: 0x{module_addr:02X}")
            
        except Exception as e:
            logger.error(f"❌ Failed to broadcast module discovery: {e}")


# Global WebSocket RS485 service instance
websocket_rs485_service = WebSocketRS485Service()
