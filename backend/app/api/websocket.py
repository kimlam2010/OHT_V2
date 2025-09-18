"""
WebSocket API endpoints for real-time communication

This module provides WebSocket endpoints for real-time monitoring and communication.
"""

import logging
import json
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
    """Get WebSocket test page (disabled in production)"""
    import os
    env = os.getenv("ENVIRONMENT", "development").lower()
    testing = os.getenv("TESTING", "false").lower() == "true"
    if env == "production" and not testing:
        raise HTTPException(status_code=status.HTTP_404_NOT_FOUND, detail="Not found")
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
            from app.services.websocket_alert_service import websocket_alert_service
            from app.services.alert_system import alert_system
            
            # Get active alerts from alert system
            active_alerts = alert_system.get_active_alerts()
            
            # Convert to response format
            alert_responses = []
            for alert in active_alerts:
                alert_response = {
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
                alert_responses.append(alert_response)
            
            # Send initial alerts message
            message = WebSocketMessage(
                type="active_alerts_update",
                data={
                    "alerts": alert_responses,
                    "total_count": len(alert_responses),
                    "critical_count": len([a for a in alert_responses if a["severity"] == "critical"]),
                    "warning_count": len([a for a in alert_responses if a["severity"] in ["high", "medium", "warning"]])
                },
                timestamp=datetime.now()
            )
            await websocket_service.send_to_client(websocket, message)
            
            logger.info(f"📨 Sent initial alerts to client: {len(alert_responses)} alerts")
            
        except Exception as e:
            logger.error(f"❌ Failed to send initial alerts: {e}")
        
        # Handle WebSocket communication
        try:
            while True:
                # Receive message from client
                data = await websocket.receive_text()
                await websocket_service.handle_message(websocket, data)
                
        except WebSocketDisconnect:
            logger.info("🔌 WebSocket alerts client disconnected")
        except Exception as e:
            logger.error(f"❌ WebSocket communication error: {e}")
            
    except Exception as e:
        logger.error(f"❌ WebSocket alerts endpoint error: {e}")
    finally:
        # Clean up connection
        await websocket_service.disconnect(websocket)


@router.websocket("/ws/rs485")
async def websocket_rs485_endpoint(websocket: WebSocket):
    """WebSocket endpoint for real-time RS485 modules data"""
    try:
        # Accept WebSocket connection
        await websocket.accept()
        logger.info("📡 RS485 WebSocket connection accepted")
        
        # Add to RS485 WebSocket service
        from app.services.websocket_rs485_service import websocket_rs485_service
        await websocket_rs485_service.add_connection(websocket)
        
        # Send initial RS485 data directly
        try:
            from app.services.rs485_service import rs485_service
            
            # Get initial modules data
            modules = await rs485_service.get_modules()
            bus_health = await rs485_service.get_bus_health()
            
            # Send initial data directly
            initial_message = {
                "type": "rs485_initial",
                "data": {
                    "modules": [module.model_dump() for module in modules],
                    "bus_health": bus_health.model_dump(),
                    "timestamp": datetime.now().isoformat()
                },
                "timestamp": datetime.now().isoformat()
            }
            
            await websocket.send_text(json.dumps(initial_message, default=str))
            logger.info("📨 Sent initial RS485 data to client")
            
        except Exception as e:
            logger.error(f"❌ Failed to send initial RS485 data: {e}")
        
        # Handle WebSocket communication
        try:
            while True:
                # Receive message from client
                data = await websocket.receive_text()
                logger.info(f"📡 Received RS485 WebSocket message: {data}")
                
                # Parse and handle message
                try:
                    message = json.loads(data)
                    message_type = message.get('type')
                    
                    if message_type == 'ping':
                        # Send pong response
                        pong_message = {
                            "type": "pong",
                            "timestamp": datetime.now().isoformat()
                        }
                        await websocket.send_text(json.dumps(pong_message, default=str))
                        
                    elif message_type == 'get_modules':
                        # Send current modules data
                        modules = await rs485_service.get_modules()
                        modules_message = {
                            "type": "rs485_modules",
                            "data": {
                                "modules": [module.model_dump() for module in modules]
                            },
                            "timestamp": datetime.now().isoformat()
                        }
                        await websocket.send_text(json.dumps(modules_message, default=str))
                        
                except json.JSONDecodeError:
                    logger.warning(f"⚠️ Invalid JSON received: {data}")
                
        except WebSocketDisconnect:
            logger.info("🔌 RS485 WebSocket client disconnected")
        except Exception as e:
            logger.error(f"❌ RS485 WebSocket communication error: {e}")
            
    except Exception as e:
        logger.error(f"❌ RS485 WebSocket endpoint error: {e}")
    finally:
        # Remove from RS485 WebSocket service
        try:
            from app.services.websocket_rs485_service import websocket_rs485_service
            await websocket_rs485_service.remove_connection(websocket)
        except Exception as e:
            logger.error(f"❌ Failed to remove WebSocket connection: {e}")


@router.websocket("/ws/logs")
async def websocket_logs_endpoint(websocket: WebSocket):
    """WebSocket endpoint for real-time system log streaming - Issue #78"""
    try:
        # Connect to WebSocket service
        await websocket_service.connect(websocket, {"type": "logs"})
        
        # Send initial logs data
        try:
            from app.services.websocket_log_service import websocket_log_service
            
            # Send recent logs (default: info level and above, last 50 entries)
            await websocket_log_service.send_recent_logs(
                websocket, 
                level_filter="info", 
                limit=50
            )
            
            logger.info("📨 Sent initial logs to client")
            
        except Exception as e:
            logger.error(f"❌ Failed to send initial logs: {e}")
        
        # Handle WebSocket communication
        try:
            while True:
                # Receive message from client
                data = await websocket.receive_text()
                
                # Parse client message
                try:
                    message_data = json.loads(data)
                    message_type = message_data.get("type")
                    
                    # Handle log-specific messages
                    if message_type == "set_log_level":
                        level = message_data.get("data", {}).get("level", "info")
                        await websocket_log_service.set_log_level_filter(websocket, level)
                        
                        # Send confirmation
                        confirm_message = WebSocketMessage(
                            type="log_level_set",
                            data={"level": level, "message": f"Log level filter set to {level}"},
                            timestamp=datetime.now()
                        )
                        await websocket_service.send_to_client(websocket, confirm_message)
                        
                    elif message_type == "get_recent_logs":
                        level = message_data.get("data", {}).get("level", "info")
                        limit = message_data.get("data", {}).get("limit", 50)
                        await websocket_log_service.send_recent_logs(websocket, level, limit)
                        
                    else:
                        # Handle standard WebSocket messages
                        await websocket_service.handle_message(websocket, data)
                        
                except json.JSONDecodeError:
                    logger.warning(f"⚠️ Invalid JSON in logs WebSocket: {data}")
                
        except WebSocketDisconnect:
            logger.info("🔌 WebSocket logs client disconnected")
        except Exception as e:
            logger.error(f"❌ WebSocket logs communication error: {e}")
            
    except Exception as e:
        logger.error(f"❌ WebSocket logs endpoint error: {e}")
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
