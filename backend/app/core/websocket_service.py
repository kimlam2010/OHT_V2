"""
WebSocket Service for Production Environment

This module provides real-time communication via WebSockets.
It includes client management, message broadcasting, and real-time monitoring.
"""

import asyncio
import json
import logging
from typing import Dict, Any, List, Optional, Set
from datetime import datetime
from dataclasses import dataclass

from fastapi import WebSocket, WebSocketDisconnect
from fastapi.websockets import WebSocketState

logger = logging.getLogger(__name__)


@dataclass
class WebSocketMessage:
    """WebSocket message data structure"""
    type: str
    data: Dict[str, Any]
    timestamp: datetime
    source: str = "backend"


class WebSocketService:
    """
    Production-grade WebSocket service for real-time communication

    Features:
    - Client connection management
    - Message broadcasting
    - Real-time monitoring updates
    - Connection health monitoring
    - Message queuing and delivery
    """

    def __init__(self):
        self.active_connections: Set[WebSocket] = set()
        self.connection_metadata: Dict[WebSocket, Dict[str, Any]] = {}
        self.message_queue: asyncio.Queue = asyncio.Queue()
        self.broadcast_task: Optional[asyncio.Task] = None
        self.health_check_task: Optional[asyncio.Task] = None
        
        # Service status
        self.is_running = False
        self.total_messages_sent = 0
        self.total_messages_received = 0
        self.connection_count = 0
        
        logger.info("🔌 WebSocketService initialized")

    async def start(self):
        """Start WebSocket service"""
        if self.is_running:
            logger.warning("⚠️ WebSocket service already running")
            return
        
        try:
            self.is_running = True
            
            # Start background tasks
            self.broadcast_task = asyncio.create_task(self._broadcast_worker())
            self.health_check_task = asyncio.create_task(self._health_check_worker())
            
            logger.info("🚀 WebSocket service started")
            
        except Exception as e:
            logger.error(f"❌ Failed to start WebSocket service: {e}")
            self.is_running = False
            raise

    async def stop(self):
        """Stop WebSocket service"""
        if not self.is_running:
            logger.warning("⚠️ WebSocket service not running")
            return
        
        try:
            self.is_running = False
            
            # Cancel background tasks
            if self.broadcast_task:
                self.broadcast_task.cancel()
            if self.health_check_task:
                self.health_check_task.cancel()
            
            # Close all connections
            await self._close_all_connections()
            
            logger.info("🛑 WebSocket service stopped")
            
        except Exception as e:
            logger.error(f"❌ Failed to stop WebSocket service: {e}")
            raise

    async def connect(self, websocket: WebSocket, client_info: Optional[Dict[str, Any]] = None):
        """Accept new WebSocket connection"""
        try:
            await websocket.accept()
            
            # Store connection
            self.active_connections.add(websocket)
            self.connection_count += 1
            
            # Store metadata
            metadata = {
                "connected_at": datetime.now(),
                "client_info": client_info or {},
                "last_activity": datetime.now(),
                "messages_sent": 0,
                "messages_received": 0
            }
            self.connection_metadata[websocket] = metadata
            
            logger.info(f"✅ WebSocket connected: {self.connection_count} active connections")
            
            # Send welcome message
            welcome_message = WebSocketMessage(
                type="connection_established",
                data={
                    "message": "WebSocket connection established",
                    "connection_id": id(websocket),
                    "timestamp": datetime.now().isoformat()
                },
                timestamp=datetime.now()
            )
            
            await self._send_message(websocket, welcome_message)
            
        except Exception as e:
            logger.error(f"❌ WebSocket connection failed: {e}")
            if websocket in self.active_connections:
                self.active_connections.remove(websocket)
            if websocket in self.connection_metadata:
                del self.connection_metadata[websocket]

    async def disconnect(self, websocket: WebSocket):
        """Handle WebSocket disconnection"""
        try:
            if websocket in self.active_connections:
                self.active_connections.remove(websocket)
                self.connection_count -= 1
                
            if websocket in self.connection_metadata:
                del self.connection_metadata[websocket]
                
            logger.info(f"🔌 WebSocket disconnected: {self.connection_count} active connections")
            
        except Exception as e:
            logger.error(f"❌ WebSocket disconnection handling failed: {e}")

    async def handle_message(self, websocket: WebSocket, message: str):
        """Handle incoming WebSocket message"""
        try:
            # Parse message
            try:
                data = json.loads(message)
                message_type = data.get("type", "unknown")
                message_data = data.get("data", {})
            except json.JSONDecodeError:
                logger.warning(f"⚠️ Invalid JSON message received: {message}")
                return
            
            # Update connection metadata
            if websocket in self.connection_metadata:
                self.connection_metadata[websocket]["last_activity"] = datetime.now()
                self.connection_metadata[websocket]["messages_received"] += 1
                self.total_messages_received += 1
            
            # Handle different message types
            if message_type == "ping":
                await self._send_pong(websocket)
            elif message_type == "subscribe":
                await self._handle_subscription(websocket, message_data)
            elif message_type == "unsubscribe":
                await self._handle_unsubscription(websocket, message_data)
            else:
                logger.debug(f"📨 Message received: {message_type}")
                
        except Exception as e:
            logger.error(f"❌ Message handling failed: {e}")

    async def broadcast_message(self, message: WebSocketMessage):
        """Broadcast message to all connected clients"""
        try:
            await self.message_queue.put(message)
            logger.debug(f"📢 Message queued for broadcast: {message.type}")
            
        except Exception as e:
            logger.error(f"❌ Failed to queue broadcast message: {e}")

    async def broadcast_to_subscribers(self, message: WebSocketMessage, subscription_type: str):
        """Broadcast message to specific subscribers"""
        try:
            # Filter connections by subscription type
            target_connections = []
            
            for websocket, metadata in self.connection_metadata.items():
                subscriptions = metadata.get("subscriptions", [])
                if subscription_type in subscriptions:
                    target_connections.append(websocket)
            
            # Send message to subscribers
            for websocket in target_connections:
                if websocket.client_state == WebSocketState.CONNECTED:
                    await self._send_message(websocket, message)
                    
            logger.debug(f"📢 Message broadcast to {len(target_connections)} subscribers: {subscription_type}")
            
        except Exception as e:
            logger.error(f"❌ Failed to broadcast to subscribers: {e}")

    async def send_to_client(self, websocket: WebSocket, message: WebSocketMessage):
        """Send message to specific client"""
        try:
            if websocket.client_state == WebSocketState.CONNECTED:
                await self._send_message(websocket, message)
                logger.debug(f"📤 Message sent to client: {message.type}")
            else:
                logger.warning("⚠️ Attempted to send message to disconnected client")
                
        except Exception as e:
            logger.error(f"❌ Failed to send message to client: {e}")

    async def get_connection_stats(self) -> Dict[str, Any]:
        """Get WebSocket connection statistics"""
        try:
            active_connections = len(self.active_connections)
            total_connections = self.connection_count
            
            # Calculate message rates
            current_time = datetime.now()
            messages_per_minute = 0
            
            if self.connection_metadata:
                for metadata in self.connection_metadata.values():
                    last_activity = metadata.get("last_activity")
                    if last_activity:
                        time_diff = (current_time - last_activity).total_seconds()
                        if time_diff <= 60:  # Last minute
                            messages_per_minute += metadata.get("messages_received", 0)
            
            return {
                "service_status": "running" if self.is_running else "stopped",
                "active_connections": active_connections,
                "total_connections": total_connections,
                "total_messages_sent": self.total_messages_sent,
                "total_messages_received": self.total_messages_received,
                "messages_per_minute": messages_per_minute,
                "last_updated": current_time.isoformat()
            }
            
        except Exception as e:
            logger.error(f"❌ Failed to get connection stats: {e}")
            return {"error": str(e)}

    # Private methods

    async def _send_message(self, websocket: WebSocket, message: WebSocketMessage):
        """Send message to WebSocket client"""
        try:
            message_dict = {
                "type": message.type,
                "data": message.data,
                "timestamp": message.timestamp.isoformat(),
                "source": message.source
            }
            
            await websocket.send_text(json.dumps(message_dict))
            
            # Update metadata
            if websocket in self.connection_metadata:
                self.connection_metadata[websocket]["messages_sent"] += 1
                self.total_messages_sent += 1
                
        except Exception as e:
            logger.error(f"❌ Failed to send message: {e}")
            # Mark connection for cleanup
            await self.disconnect(websocket)

    async def _send_pong(self, websocket: WebSocket):
        """Send pong response to ping"""
        try:
            pong_message = WebSocketMessage(
                type="pong",
                data={"timestamp": datetime.now().isoformat()},
                timestamp=datetime.now()
            )
            await self._send_message(websocket, pong_message)
            
        except Exception as e:
            logger.error(f"❌ Failed to send pong: {e}")

    async def _handle_subscription(self, websocket: WebSocket, data: Dict[str, Any]):
        """Handle client subscription request"""
        try:
            subscription_type = data.get("type")
            if not subscription_type:
                return
            
            # Initialize subscriptions if not exists
            if websocket not in self.connection_metadata:
                self.connection_metadata[websocket] = {}
            
            if "subscriptions" not in self.connection_metadata[websocket]:
                self.connection_metadata[websocket]["subscriptions"] = []
            
            # Add subscription
            if subscription_type not in self.connection_metadata[websocket]["subscriptions"]:
                self.connection_metadata[websocket]["subscriptions"].append(subscription_type)
                
            # Send confirmation
            confirm_message = WebSocketMessage(
                type="subscription_confirmed",
                data={"subscription_type": subscription_type},
                timestamp=datetime.now()
            )
            await self._send_message(websocket, confirm_message)
            
            logger.info(f"✅ Client subscribed to: {subscription_type}")
            
        except Exception as e:
            logger.error(f"❌ Subscription handling failed: {e}")

    async def _handle_unsubscription(self, websocket: WebSocket, data: Dict[str, Any]):
        """Handle client unsubscription request"""
        try:
            subscription_type = data.get("type")
            if not subscription_type:
                return
            
            # Remove subscription
            if (websocket in self.connection_metadata and 
                "subscriptions" in self.connection_metadata[websocket]):
                
                if subscription_type in self.connection_metadata[websocket]["subscriptions"]:
                    self.connection_metadata[websocket]["subscriptions"].remove(subscription_type)
                    
                    # Send confirmation
                    confirm_message = WebSocketMessage(
                        type="unsubscription_confirmed",
                        data={"subscription_type": subscription_type},
                        timestamp=datetime.now()
                    )
                    await self._send_message(websocket, confirm_message)
                    
                    logger.info(f"✅ Client unsubscribed from: {subscription_type}")
                    
        except Exception as e:
            logger.error(f"❌ Unsubscription handling failed: {e}")

    async def _broadcast_worker(self):
        """Background worker for message broadcasting"""
        try:
            while self.is_running:
                try:
                    # Wait for message with timeout
                    message = await asyncio.wait_for(self.message_queue.get(), timeout=1.0)
                    
                    # Broadcast to all active connections
                    disconnected_websockets = []
                    
                    for websocket in self.active_connections:
                        try:
                            if websocket.client_state == WebSocketState.CONNECTED:
                                await self._send_message(websocket, message)
                            else:
                                disconnected_websockets.append(websocket)
                        except Exception as e:
                            logger.error(f"❌ Broadcast to client failed: {e}")
                            disconnected_websockets.append(websocket)
                    
                    # Clean up disconnected websockets
                    for websocket in disconnected_websockets:
                        await self.disconnect(websocket)
                        
                except asyncio.TimeoutError:
                    # No messages in queue, continue
                    continue
                except Exception as e:
                    logger.error(f"❌ Broadcast worker error: {e}")
                    await asyncio.sleep(1)
                    
        except asyncio.CancelledError:
            logger.info("🛑 Broadcast worker cancelled")
        except Exception as e:
            logger.error(f"❌ Broadcast worker failed: {e}")

    async def _health_check_worker(self):
        """Background worker for connection health checks"""
        try:
            while self.is_running:
                try:
                    await asyncio.sleep(30)  # Check every 30 seconds
                    
                    # Check for stale connections
                    current_time = datetime.now()
                    stale_websockets = []
                    
                    for websocket, metadata in self.connection_metadata.items():
                        last_activity = metadata.get("last_activity")
                        if last_activity:
                            time_diff = (current_time - last_activity).total_seconds()
                            if time_diff > 300:  # 5 minutes of inactivity
                                stale_websockets.append(websocket)
                    
                    # Clean up stale connections
                    for websocket in stale_websockets:
                        logger.warning(f"⚠️ Cleaning up stale connection: {id(websocket)}")
                        await self.disconnect(websocket)
                        
                except Exception as e:
                    logger.error(f"❌ Health check worker error: {e}")
                    await asyncio.sleep(30)
                    
        except asyncio.CancelledError:
            logger.info("🛑 Health check worker cancelled")
        except Exception as e:
            logger.error(f"❌ Health check worker failed: {e}")

    async def _close_all_connections(self):
        """Close all active WebSocket connections"""
        try:
            for websocket in list(self.active_connections):
                try:
                    await websocket.close()
                except Exception as e:
                    logger.error(f"❌ Failed to close connection: {e}")
                    
            self.active_connections.clear()
            self.connection_metadata.clear()
            self.connection_count = 0
            
        except Exception as e:
            logger.error(f"❌ Failed to close all connections: {e}")


# Global WebSocket service instance
websocket_service = WebSocketService()
