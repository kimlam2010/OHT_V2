"""
WebSocket API endpoints for real-time communication

This module provides WebSocket endpoints for real-time monitoring and communication.
"""

import logging
from typing import Dict, Any
from fastapi import APIRouter, WebSocket, WebSocketDisconnect, Depends, HTTPException, status
from fastapi.responses import HTMLResponse
from datetime import datetime

from app.core.websocket_service import websocket_service, WebSocketMessage
from app.core.monitoring_service import monitoring_service
from app.core.production_rbac import require_permission
from app.models.user import User
from app.core.security import get_current_user

logger = logging.getLogger(__name__)

router = APIRouter(tags=["websocket"])


@router.get("/ws")
async def get_websocket_page():
    """Get WebSocket test page"""
    html_content = """
    <!DOCTYPE html>
    <html>
    <head>
        <title>OHT-50 WebSocket Test</title>
        <style>
            body { font-family: Arial, sans-serif; margin: 20px; }
            .container { max-width: 800px; margin: 0 auto; }
            .status { padding: 10px; margin: 10px 0; border-radius: 5px; }
            .connected { background-color: #d4edda; color: #155724; }
            .disconnected { background-color: #f8d7da; color: #721c24; }
            .message { background-color: #f8f9fa; border: 1px solid #dee2e6; padding: 10px; margin: 5px 0; border-radius: 3px; }
            .controls { margin: 20px 0; }
            button { padding: 10px 20px; margin: 5px; border: none; border-radius: 5px; cursor: pointer; }
            .connect { background-color: #28a745; color: white; }
            .disconnect { background-color: #dc3545; color: white; }
            .subscribe { background-color: #007bff; color: white; }
            .unsubscribe { background-color: #6c757d; color: white; }
            input[type="text"] { padding: 8px; margin: 5px; border: 1px solid #ddd; border-radius: 3px; }
        </style>
    </head>
    <body>
        <div class="container">
            <h1>OHT-50 WebSocket Test Interface</h1>
            
            <div id="status" class="status disconnected">
                Status: Disconnected
            </div>
            
            <div class="controls">
                <button id="connectBtn" class="connect" onclick="connect()">Connect</button>
                <button id="disconnectBtn" class="disconnect" onclick="disconnect()" disabled>Disconnect</button>
                <br><br>
                <input type="text" id="subscriptionType" placeholder="Subscription type (e.g., telemetry, alerts)" value="telemetry">
                <button id="subscribeBtn" class="subscribe" onclick="subscribe()" disabled>Subscribe</button>
                <button id="unsubscribeBtn" class="unsubscribe" onclick="unsubscribe()" disabled>Unsubscribe</button>
            </div>
            
            <h3>Messages</h3>
            <div id="messages"></div>
        </div>

        <script>
            let ws = null;
            let isConnected = false;

            function updateStatus(connected, message) {
                const statusDiv = document.getElementById('status');
                statusDiv.className = connected ? 'status connected' : 'status disconnected';
                statusDiv.textContent = 'Status: ' + message;
                
                document.getElementById('connectBtn').disabled = connected;
                document.getElementById('disconnectBtn').disabled = !connected;
                document.getElementById('subscribeBtn').disabled = !connected;
                document.getElementById('unsubscribeBtn').disabled = !connected;
            }

            function addMessage(message) {
                const messagesDiv = document.getElementById('messages');
                const messageDiv = document.createElement('div');
                messageDiv.className = 'message';
                messageDiv.innerHTML = `
                    <strong>${message.type}</strong><br>
                    <small>${message.timestamp}</small><br>
                    <pre>${JSON.stringify(message.data, null, 2)}</pre>
                `;
                messagesDiv.insertBefore(messageDiv, messagesDiv.firstChild);
            }

            function connect() {
                if (ws) {
                    ws.close();
                }

                ws = new WebSocket('ws://localhost:8000/ws/telemetry');
                
                ws.onopen = function(event) {
                    isConnected = true;
                    updateStatus(true, 'Connected');
                    addMessage({
                        type: 'connection',
                        timestamp: new Date().toLocaleTimeString(),
                        data: { message: 'WebSocket connection established' }
                    });
                };
                
                ws.onmessage = function(event) {
                    try {
                        const message = JSON.parse(event.data);
                        addMessage({
                            type: message.type,
                            timestamp: new Date(message.timestamp).toLocaleTimeString(),
                            data: message.data
                        });
                    } catch (e) {
                        addMessage({
                            type: 'error',
                            timestamp: new Date().toLocaleTimeString(),
                            data: { error: 'Failed to parse message', raw: event.data }
                        });
                    }
                };
                
                ws.onclose = function(event) {
                    isConnected = false;
                    updateStatus(false, 'Disconnected');
                    addMessage({
                        type: 'connection',
                        timestamp: new Date().toLocaleTimeString(),
                        data: { message: 'WebSocket connection closed' }
                    });
                };
                
                ws.onerror = function(error) {
                    addMessage({
                        type: 'error',
                        timestamp: new Date().toLocaleTimeString(),
                        data: { error: 'WebSocket error', details: error }
                    });
                };
            }

            function disconnect() {
                if (ws) {
                    ws.close();
                    ws = null;
                }
            }

            function subscribe() {
                if (ws && isConnected) {
                    const subscriptionType = document.getElementById('subscriptionType').value;
                    const message = {
                        type: 'subscribe',
                        data: { type: subscriptionType }
                    };
                    ws.send(JSON.stringify(message));
                }
            }

            function unsubscribe() {
                if (ws && isConnected) {
                    const subscriptionType = document.getElementById('subscriptionType').value;
                    const message = {
                        type: 'unsubscribe',
                        data: { type: subscriptionType }
                    };
                    ws.send(JSON.stringify(message));
                }
            }

            // Auto-connect on page load
            window.onload = function() {
                setTimeout(connect, 1000);
            };
        </script>
    </body>
    </html>
    """
    return HTMLResponse(content=html_content)


@router.websocket("/ws/telemetry")
async def websocket_telemetry_endpoint(websocket: WebSocket):
    """WebSocket endpoint for telemetry data"""
    try:
        # Connect to WebSocket service
        await websocket_service.connect(websocket, {"type": "telemetry"})
        
        # Send initial telemetry data
        try:
            telemetry_data = await monitoring_service.get_current_metrics()
            message = WebSocketMessage(
                type="telemetry_update",
                data={
                    "cpu_percent": telemetry_data.cpu_percent,
                    "memory_percent": telemetry_data.memory_percent,
                    "disk_percent": telemetry_data.disk_percent,
                    "active_connections": telemetry_data.active_connections
                },
                timestamp=telemetry_data.timestamp
            )
            await websocket_service.send_to_client(websocket, message)
        except Exception as e:
            logger.error(f"❌ Failed to send initial telemetry: {e}")
        
        # Handle WebSocket communication
        try:
            while True:
                # Receive message from client
                data = await websocket.receive_text()
                await websocket_service.handle_message(websocket, data)
                
        except WebSocketDisconnect:
            logger.info("🔌 WebSocket client disconnected")
        except Exception as e:
            logger.error(f"❌ WebSocket communication error: {e}")
            
    except Exception as e:
        logger.error(f"❌ WebSocket endpoint error: {e}")
    finally:
        # Clean up connection
        await websocket_service.disconnect(websocket)


@router.websocket("/ws/monitoring")
async def websocket_monitoring_endpoint(websocket: WebSocket):
    """WebSocket endpoint for monitoring data"""
    try:
        # Connect to WebSocket service
        await websocket_service.connect(websocket, {"type": "monitoring"})
        
        # Send initial monitoring data
        try:
            health_data = await monitoring_service.get_system_health()
            message = WebSocketMessage(
                type="system_health",
                data=health_data,
                timestamp=datetime.now()
            )
            await websocket_service.send_to_client(websocket, message)
        except Exception as e:
            logger.error(f"❌ Failed to send initial health data: {e}")
        
        # Handle WebSocket communication
        try:
            while True:
                # Receive message from client
                data = await websocket.receive_text()
                await websocket_service.handle_message(websocket, data)
                
        except WebSocketDisconnect:
            logger.info("🔌 WebSocket client disconnected")
        except Exception as e:
            logger.error(f"❌ WebSocket communication error: {e}")
            
    except Exception as e:
        logger.error(f"❌ WebSocket endpoint error: {e}")
    finally:
        # Clean up connection
        await websocket_service.disconnect(websocket)


@router.websocket("/ws/map")
async def websocket_map_endpoint(websocket: WebSocket):
    """WebSocket endpoint for map service real-time updates"""
    try:
        # Connect to WebSocket service
        await websocket_service.connect(websocket, {"type": "map"})
        
        # Send initial map status
        try:
            from app.services.map_service import MapService
            map_service = MapService()
            status_info = map_service.get_mapping_status()
            
            message = WebSocketMessage(
                type="map_status",
                data=status_info,
                timestamp=datetime.now()
            )
            await websocket_service.send_to_client(websocket, message)
        except Exception as e:
            logger.error(f"❌ Failed to send initial map status: {e}")
        
        # Handle WebSocket communication
        try:
            while True:
                # Receive message from client
                data = await websocket.receive_text()
                await websocket_service.handle_message(websocket, data)
                
        except WebSocketDisconnect:
            logger.info("🔌 Map WebSocket client disconnected")
        except Exception as e:
            logger.error(f"❌ Map WebSocket communication error: {e}")
            
    except Exception as e:
        logger.error(f"❌ Map WebSocket endpoint error: {e}")
    finally:
        # Clean up connection
        await websocket_service.disconnect(websocket)


@router.websocket("/ws/alerts")
async def websocket_alerts_endpoint(websocket: WebSocket):
    """WebSocket endpoint for alert notifications"""
    try:
        # Connect to WebSocket service
        await websocket_service.connect(websocket, {"type": "alerts"})
        
        # Send initial alerts data
        try:
            active_alerts = await monitoring_service.get_active_alerts()
            message = WebSocketMessage(
                type="alerts_update",
                data={
                    "alerts_count": len(active_alerts),
                    "alerts": [
                        {
                            "id": alert.id,
                            "level": alert.level,
                            "message": alert.message,
                            "timestamp": alert.timestamp.isoformat()
                        }
                        for alert in active_alerts
                    ]
                },
                timestamp=datetime.now()
            )
            await websocket_service.send_to_client(websocket, message)
        except Exception as e:
            logger.error(f"❌ Failed to send initial alerts: {e}")
        
        # Handle WebSocket communication
        try:
            while True:
                # Receive message from client
                data = await websocket.receive_text()
                await websocket_service.handle_message(websocket, data)
                
        except WebSocketDisconnect:
            logger.info("🔌 WebSocket client disconnected")
        except Exception as e:
            logger.error(f"❌ WebSocket communication error: {e}")
            
    except Exception as e:
        logger.error(f"❌ WebSocket endpoint error: {e}")
    finally:
        # Clean up connection
        await websocket_service.disconnect(websocket)


@router.get("/ws/status")
async def get_websocket_status(
    current_user: User = Depends(require_permission("system", "read"))
) -> Dict[str, Any]:
    """Get WebSocket service status"""
    try:
        stats = await websocket_service.get_connection_stats()
        
        return {
            "success": True,
            "data": stats,
            "message": "WebSocket status retrieved successfully"
        }
    except Exception as e:
        logger.error(f"❌ WebSocket status retrieval failed: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to retrieve WebSocket status"
        )
