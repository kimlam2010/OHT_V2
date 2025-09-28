"""
Network Events WebSocket - OHT-50 Backend

WebSocket endpoints for real-time network events and monitoring.
"""

import logging
from datetime import datetime, timezone
from typing import Dict, Any, List
from fastapi import WebSocket, WebSocketDisconnect, Depends
from fastapi.routing import APIRouter

from app.services.network_monitor import network_monitor
from app.services.websocket_alert_service import websocket_alert_service

logger = logging.getLogger(__name__)

# Create router
router = APIRouter()


class NetworkWebSocketManager:
    """WebSocket manager for network events"""
    
    def __init__(self):
        self.active_connections: List[WebSocket] = []
    
    async def connect(self, websocket: WebSocket) -> None:
        """Accept WebSocket connection"""
        await websocket.accept()
        self.active_connections.append(websocket)
        logger.info(f"📡 Network WebSocket connected. Total connections: {len(self.active_connections)}")
    
    def disconnect(self, websocket: WebSocket) -> None:
        """Remove WebSocket connection"""
        if websocket in self.active_connections:
            self.active_connections.remove(websocket)
        logger.info(f"📡 Network WebSocket disconnected. Total connections: {len(self.active_connections)}")
    
    async def send_personal_message(self, message: Dict[str, Any], websocket: WebSocket) -> None:
        """Send message to specific WebSocket"""
        try:
            await websocket.send_json(message)
        except Exception as e:
            logger.error(f"❌ Error sending personal message: {e}")
            self.disconnect(websocket)
    
    async def broadcast(self, message: Dict[str, Any]) -> None:
        """Broadcast message to all connected WebSockets"""
        disconnected = []
        
        for connection in self.active_connections:
            try:
                await connection.send_json(message)
            except Exception as e:
                logger.error(f"❌ Error broadcasting to WebSocket: {e}")
                disconnected.append(connection)
        
        # Remove disconnected connections
        for connection in disconnected:
            self.disconnect(connection)


# Global WebSocket manager
network_ws_manager = NetworkWebSocketManager()


@router.websocket("/network/events")
async def network_events_websocket(websocket: WebSocket):
    """
    WebSocket endpoint for real-time network events
    
    Provides real-time updates for:
    - Network status changes
    - WiFi AP status changes
    - Client connect/disconnect events
    - Performance alerts
    - Fallback connectivity changes
    """
    await network_ws_manager.connect(websocket)
    
    try:
        # Send initial status
        initial_status = await network_monitor.get_monitoring_status()
        await network_ws_manager.send_personal_message({
            "type": "initial_status",
            "data": initial_status,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }, websocket)
        
        # Keep connection alive and handle incoming messages
        while True:
            try:
                # Wait for client message (ping/pong)
                data = await websocket.receive_text()
                
                if data == "ping":
                    await network_ws_manager.send_personal_message({
                        "type": "pong",
                        "timestamp": datetime.now(timezone.utc).isoformat()
                    }, websocket)
                elif data == "get_status":
                    # Send current network status
                    status = await network_monitor.get_monitoring_status()
                    await network_ws_manager.send_personal_message({
                        "type": "status_update",
                        "data": status,
                        "timestamp": datetime.now(timezone.utc).isoformat()
                    }, websocket)
                elif data == "get_events":
                    # Send recent events
                    events = await network_monitor.get_network_events(limit=20)
                    await network_ws_manager.send_personal_message({
                        "type": "events_history",
                        "data": events,
                        "timestamp": datetime.now(timezone.utc).isoformat()
                    }, websocket)
                
            except WebSocketDisconnect:
                break
            except Exception as e:
                logger.error(f"❌ WebSocket error: {e}")
                break
                
    except WebSocketDisconnect:
        pass
    except Exception as e:
        logger.error(f"❌ Network WebSocket error: {e}")
    finally:
        network_ws_manager.disconnect(websocket)


@router.websocket("/network/status")
async def network_status_websocket(websocket: WebSocket):
    """
    WebSocket endpoint for real-time network status
    
    Provides real-time network status updates:
    - Connection status
    - AP status
    - Client count
    - Performance metrics
    """
    await network_ws_manager.connect(websocket)
    
    try:
        # Send initial network status
        from app.services.network_integration_service import network_service
        
        initial_status = await network_service.get_network_status()
        await network_ws_manager.send_personal_message({
            "type": "initial_network_status",
            "data": initial_status.get("data", {}),
            "timestamp": datetime.now(timezone.utc).isoformat()
        }, websocket)
        
        # Keep connection alive
        while True:
            try:
                # Wait for client message
                data = await websocket.receive_text()
                
                if data == "ping":
                    await network_ws_manager.send_personal_message({
                        "type": "pong",
                        "timestamp": datetime.now(timezone.utc).isoformat()
                    }, websocket)
                elif data == "refresh":
                    # Send current network status
                    current_status = await network_service.get_network_status()
                    await network_ws_manager.send_personal_message({
                        "type": "network_status_update",
                        "data": current_status.get("data", {}),
                        "timestamp": datetime.now(timezone.utc).isoformat()
                    }, websocket)
                
            except WebSocketDisconnect:
                break
            except Exception as e:
                logger.error(f"❌ WebSocket error: {e}")
                break
                
    except WebSocketDisconnect:
        pass
    except Exception as e:
        logger.error(f"❌ Network status WebSocket error: {e}")
    finally:
        network_ws_manager.disconnect(websocket)


@router.websocket("/network/ap/clients")
async def network_ap_clients_websocket(websocket: WebSocket):
    """
    WebSocket endpoint for real-time AP client updates
    
    Provides real-time updates for:
    - Client connections
    - Client disconnections
    - Client status changes
    - Client usage statistics
    """
    await network_ws_manager.connect(websocket)
    
    try:
        # Send initial client list
        from app.services.network_integration_service import network_service
        
        initial_clients = await network_service.get_ap_clients()
        await network_ws_manager.send_personal_message({
            "type": "initial_clients",
            "data": initial_clients.get("data", {}),
            "timestamp": datetime.now(timezone.utc).isoformat()
        }, websocket)
        
        # Keep connection alive
        while True:
            try:
                # Wait for client message
                data = await websocket.receive_text()
                
                if data == "ping":
                    await network_ws_manager.send_personal_message({
                        "type": "pong",
                        "timestamp": datetime.now(timezone.utc).isoformat()
                    }, websocket)
                elif data == "refresh":
                    # Send current client list
                    current_clients = await network_service.get_ap_clients()
                    await network_ws_manager.send_personal_message({
                        "type": "clients_update",
                        "data": current_clients.get("data", {}),
                        "timestamp": datetime.now(timezone.utc).isoformat()
                    }, websocket)
                
            except WebSocketDisconnect:
                break
            except Exception as e:
                logger.error(f"❌ WebSocket error: {e}")
                break
                
    except WebSocketDisconnect:
        pass
    except Exception as e:
        logger.error(f"❌ AP clients WebSocket error: {e}")
    finally:
        network_ws_manager.disconnect(websocket)


# WebSocket event handlers for network monitor integration

async def broadcast_network_event(event_type: str, event_data: Dict[str, Any]) -> None:
    """
    Broadcast network event to all connected WebSockets
    
    Args:
        event_type: Type of network event
        event_data: Event data
    """
    try:
        message = {
            "type": "network_event",
            "event_type": event_type,
            "data": event_data,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        await network_ws_manager.broadcast(message)
        
    except Exception as e:
        logger.error(f"❌ Error broadcasting network event: {e}")


async def broadcast_network_status(status_data: Dict[str, Any]) -> None:
    """
    Broadcast network status update to all connected WebSockets
    
    Args:
        status_data: Network status data
    """
    try:
        message = {
            "type": "network_status",
            "data": status_data,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        await network_ws_manager.broadcast(message)
        
    except Exception as e:
        logger.error(f"❌ Error broadcasting network status: {e}")


async def broadcast_ap_clients(clients_data: Dict[str, Any]) -> None:
    """
    Broadcast AP clients update to all connected WebSockets
    
    Args:
        clients_data: AP clients data
    """
    try:
        message = {
            "type": "ap_clients",
            "data": clients_data,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        await network_ws_manager.broadcast(message)
        
    except Exception as e:
        logger.error(f"❌ Error broadcasting AP clients: {e}")


# Health check endpoint
@router.get("/network/ws/health")
async def websocket_health():
    """
    WebSocket health check
    
    Returns:
        WebSocket connection status
    """
    return {
        "status": "healthy",
        "active_connections": len(network_ws_manager.active_connections),
        "service": "network_websocket",
        "timestamp": datetime.now(timezone.utc).isoformat()
    }
