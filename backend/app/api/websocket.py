"""
WebSocket endpoints for OHT-50 Backend
"""

import json
import logging
from typing import Dict, Set
from fastapi import WebSocket, WebSocketDisconnect, APIRouter
from fastapi.responses import HTMLResponse

logger = logging.getLogger(__name__)

router = APIRouter()


class WebSocketManager:
    """WebSocket connection manager"""
    
    def __init__(self):
        self.active_connections: Set[WebSocket] = set()
        self.connection_types: Dict[WebSocket, str] = {}  # client type
    
    async def connect(self, websocket: WebSocket, client_type: str = "general"):
        """Accept WebSocket connection"""
        await websocket.accept()
        self.active_connections.add(websocket)
        self.connection_types[websocket] = client_type
        logger.info(f"WebSocket connected: {client_type}")
    
    def disconnect(self, websocket: WebSocket):
        """Remove WebSocket connection"""
        self.active_connections.discard(websocket)
        self.connection_types.pop(websocket, None)
        logger.info("WebSocket disconnected")
    
    async def send_personal_message(self, message: dict, websocket: WebSocket):
        """Send message to specific WebSocket"""
        try:
            await websocket.send_text(json.dumps(message))
        except Exception as e:
            logger.error(f"Failed to send personal message: {e}")
            self.disconnect(websocket)
    
    async def broadcast(self, message: dict, client_type: str = None):
        """Broadcast message to all connections or specific type"""
        disconnected = set()
        
        for websocket in self.active_connections:
            if client_type is None or self.connection_types.get(websocket) == client_type:
                try:
                    await websocket.send_text(json.dumps(message))
                except Exception as e:
                    logger.error(f"Failed to broadcast message: {e}")
                    disconnected.add(websocket)
        
        # Remove disconnected websockets
        for websocket in disconnected:
            self.disconnect(websocket)


# Global WebSocket manager
websocket_manager = WebSocketManager()


@router.get("/ws")
async def get_websocket_page():
    """WebSocket test page"""
    html = """
    <!DOCTYPE html>
    <html>
        <head>
            <title>OHT-50 WebSocket Test</title>
        </head>
        <body>
            <h1>OHT-50 WebSocket Test</h1>
            <div id="messages"></div>
            <script>
                var ws = new WebSocket("ws://localhost:8000/ws/telemetry");
                ws.onmessage = function(event) {
                    var messages = document.getElementById('messages');
                    var message = document.createElement('div');
                    var content = document.createTextNode(event.data);
                    message.appendChild(content);
                    messages.appendChild(message);
                };
            </script>
        </body>
    </html>
    """
    return HTMLResponse(html)


@router.websocket("/ws/telemetry")
async def websocket_telemetry(websocket: WebSocket):
    """WebSocket endpoint for telemetry data"""
    await websocket_manager.connect(websocket, "telemetry")
    
    try:
        while True:
            # Keep connection alive
            data = await websocket.receive_text()
            # Echo back for testing
            await websocket_manager.send_personal_message(
                {"type": "echo", "data": data}, 
                websocket
            )
    except WebSocketDisconnect:
        websocket_manager.disconnect(websocket)


@router.websocket("/ws/control")
async def websocket_control(websocket: WebSocket):
    """WebSocket endpoint for control commands"""
    await websocket_manager.connect(websocket, "control")
    
    try:
        while True:
            data = await websocket.receive_text()
            command = json.loads(data)
            
            # Process control command
            logger.info(f"Received control command: {command}")
            
            # Send acknowledgment
            await websocket_manager.send_personal_message(
                {"type": "ack", "command": command}, 
                websocket
            )
    except WebSocketDisconnect:
        websocket_manager.disconnect(websocket)


@router.websocket("/ws/alerts")
async def websocket_alerts(websocket: WebSocket):
    """WebSocket endpoint for alert notifications"""
    await websocket_manager.connect(websocket, "alerts")
    
    try:
        while True:
            # Keep connection alive
            await websocket.receive_text()
    except WebSocketDisconnect:
        websocket_manager.disconnect(websocket)


# Utility functions for broadcasting
async def broadcast_telemetry(telemetry_data: dict):
    """Broadcast telemetry data to all telemetry clients"""
    message = {
        "type": "telemetry",
        "data": telemetry_data,
        "timestamp": "2025-01-28T10:30:00Z"
    }
    await websocket_manager.broadcast(message, "telemetry")


async def broadcast_alert(alert_data: dict):
    """Broadcast alert to all alert clients"""
    message = {
        "type": "alert",
        "data": alert_data,
        "timestamp": "2025-01-28T10:30:00Z"
    }
    await websocket_manager.broadcast(message, "alerts")


async def broadcast_status(status_data: dict):
    """Broadcast status change to all clients"""
    message = {
        "type": "status_change",
        "data": status_data,
        "timestamp": "2025-01-28T10:30:00Z"
    }
    await websocket_manager.broadcast(message)
