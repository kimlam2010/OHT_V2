# REQ_BE_18 - WebSocket Implementation

**Document ID:** REQ_BE_18  
**Version:** 1.0  
**Date:** 2025-01-28  
**Status:** Draft  
**Classification:** Internal  
**Author:** CTO Team  
**Standard Reference:** ISO/IEC/IEEE 29148:2018

---

## 1. DOCUMENT INFORMATION

### 1.1 Document Control
- **Document Title:** WebSocket Implementation
- **Document Type:** WebSocket Implementation Specification
- **Standard Reference:** ISO/IEC/IEEE 29148:2018, ISO/IEC 25010:2011
- **Project:** OHT-50 Industrial Robot Control System
- **Component:** Backend WebSocket Implementation

### 1.2 Revision History
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-01-28 | CTO Team | Initial creation |

---

## 2. WEBSOCKET IMPLEMENTATION OVERVIEW

### 2.1 WebSocket Strategy
The WebSocket implementation provides **real-time bidirectional communication** for **telemetry streaming** and **live control**:

```yaml
WebSocket Strategy:
  Purpose: "Real-time bidirectional communication"
  Protocol: "WebSocket over HTTP/HTTPS"
  Authentication: "JWT token validation"
  Message Format: "JSON with structured payloads"
  Connection Management: "Connection pooling và heartbeat"
  Error Handling: "Automatic reconnection với exponential backoff"
  Security: "TLS encryption và message validation"
```

### 2.2 WebSocket Architecture
```
WebSocket Architecture:
├── Connection Management
│   ├── Authentication
│   ├── Connection pooling
│   ├── Heartbeat monitoring
│   └── Reconnection logic
├── Message Handling
│   ├── Message validation
│   ├── Routing
│   ├── Broadcasting
│   └── Error handling
├── Data Streaming
│   ├── Telemetry streaming
│   ├── LiDAR data streaming
│   ├── Status updates
│   └── Event notifications
└── Security
    ├── TLS encryption
    ├── Message validation
    ├── Rate limiting
    └── Access control
```

---

## 3. WEBSOCKET CONNECTION MANAGEMENT

### 3.1 WebSocket Manager
```python
# app/websocket/manager.py
import asyncio
import json
import logging
from typing import Dict, Set, Optional, Any
from fastapi import WebSocket, WebSocketDisconnect
from app.security.authentication import AuthenticationService
from app.models.user import User

logger = logging.getLogger(__name__)

class ConnectionManager:
    def __init__(self):
        self.active_connections: Dict[str, WebSocket] = {}
        self.user_connections: Dict[int, Set[str]] = {}
        self.auth_service = AuthenticationService()
    
    async def connect(self, websocket: WebSocket, token: str) -> Optional[User]:
        """Establish WebSocket connection with authentication"""
        try:
            await websocket.accept()
            
            # Validate JWT token
            payload = self.auth_service.verify_token(token)
            user_id = payload.get("user_id")
            username = payload.get("sub")
            
            if not user_id or not username:
                await websocket.close(code=4001, reason="Invalid token")
                return None
            
            # Store connection
            connection_id = f"{user_id}_{username}"
            self.active_connections[connection_id] = websocket
            
            # Track user connections
            if user_id not in self.user_connections:
                self.user_connections[user_id] = set()
            self.user_connections[user_id].add(connection_id)
            
            logger.info(f"WebSocket connected: {connection_id}")
            
            # Send connection confirmation
            await self.send_personal_message({
                "type": "connection_established",
                "user_id": user_id,
                "username": username,
                "timestamp": datetime.utcnow().isoformat()
            }, connection_id)
            
            return {"user_id": user_id, "username": username}
            
        except Exception as e:
            logger.error(f"WebSocket connection failed: {e}")
            await websocket.close(code=4000, reason="Connection failed")
            return None
    
    def disconnect(self, connection_id: str):
        """Remove WebSocket connection"""
        if connection_id in self.active_connections:
            del self.active_connections[connection_id]
            
            # Remove from user connections
            for user_id, connections in self.user_connections.items():
                if connection_id in connections:
                    connections.remove(connection_id)
                    if not connections:
                        del self.user_connections[user_id]
                    break
            
            logger.info(f"WebSocket disconnected: {connection_id}")
    
    async def send_personal_message(self, message: Dict[str, Any], connection_id: str):
        """Send message to specific connection"""
        if connection_id in self.active_connections:
            try:
                await self.active_connections[connection_id].send_text(
                    json.dumps(message)
                )
            except Exception as e:
                logger.error(f"Failed to send message to {connection_id}: {e}")
                self.disconnect(connection_id)
    
    async def broadcast(self, message: Dict[str, Any]):
        """Broadcast message to all connections"""
        disconnected = []
        
        for connection_id, websocket in self.active_connections.items():
            try:
                await websocket.send_text(json.dumps(message))
            except Exception as e:
                logger.error(f"Failed to broadcast to {connection_id}: {e}")
                disconnected.append(connection_id)
        
        # Clean up disconnected connections
        for connection_id in disconnected:
            self.disconnect(connection_id)
    
    async def send_to_user(self, message: Dict[str, Any], user_id: int):
        """Send message to all connections of a specific user"""
        if user_id in self.user_connections:
            for connection_id in self.user_connections[user_id]:
                await self.send_personal_message(message, connection_id)
    
    def get_connection_count(self) -> int:
        """Get total number of active connections"""
        return len(self.active_connections)
    
    def get_user_connection_count(self, user_id: int) -> int:
        """Get number of connections for a specific user"""
        return len(self.user_connections.get(user_id, set()))
```

### 3.2 WebSocket Endpoint
```python
# app/websocket/endpoints.py
from fastapi import APIRouter, WebSocket, WebSocketDisconnect, Query
from app.websocket.manager import ConnectionManager
from app.websocket.handlers import MessageHandler
import logging

logger = logging.getLogger(__name__)

router = APIRouter()
manager = ConnectionManager()
message_handler = MessageHandler(manager)

@router.websocket("/ws")
async def websocket_endpoint(
    websocket: WebSocket,
    token: str = Query(..., description="JWT authentication token")
):
    """WebSocket endpoint for real-time communication"""
    
    # Establish connection
    user_info = await manager.connect(websocket, token)
    if not user_info:
        return
    
    connection_id = f"{user_info['user_id']}_{user_info['username']}"
    
    try:
        # Handle incoming messages
        while True:
            # Receive message
            data = await websocket.receive_text()
            
            try:
                # Parse JSON message
                message = json.loads(data)
                
                # Handle message
                await message_handler.handle_message(message, user_info)
                
            except json.JSONDecodeError:
                # Send error for invalid JSON
                await manager.send_personal_message({
                    "type": "error",
                    "message": "Invalid JSON format",
                    "timestamp": datetime.utcnow().isoformat()
                }, connection_id)
                
            except Exception as e:
                logger.error(f"Message handling error: {e}")
                await manager.send_personal_message({
                    "type": "error",
                    "message": "Internal server error",
                    "timestamp": datetime.utcnow().isoformat()
                }, connection_id)
    
    except WebSocketDisconnect:
        logger.info(f"WebSocket disconnected: {connection_id}")
        manager.disconnect(connection_id)
    
    except Exception as e:
        logger.error(f"WebSocket error: {e}")
        manager.disconnect(connection_id)
```

---

## 4. MESSAGE HANDLING

### 4.1 Message Handler
```python
# app/websocket/handlers.py
import asyncio
import json
from typing import Dict, Any
from datetime import datetime
from app.websocket.manager import ConnectionManager
from app.services.robot_service import RobotService
from app.services.lidar_service import LiDARService
from app.services.telemetry_service import TelemetryService

class MessageHandler:
    def __init__(self, connection_manager: ConnectionManager):
        self.manager = connection_manager
        self.robot_service = RobotService()
        self.lidar_service = LiDARService()
        self.telemetry_service = TelemetryService()
    
    async def handle_message(self, message: Dict[str, Any], user_info: Dict[str, Any]):
        """Handle incoming WebSocket messages"""
        
        message_type = message.get("type")
        if not message_type:
            raise ValueError("Message type is required")
        
        # Route message based on type
        if message_type == "subscribe":
            await self.handle_subscribe(message, user_info)
        elif message_type == "unsubscribe":
            await self.handle_unsubscribe(message, user_info)
        elif message_type == "robot_control":
            await self.handle_robot_control(message, user_info)
        elif message_type == "lidar_control":
            await self.handle_lidar_control(message, user_info)
        elif message_type == "ping":
            await self.handle_ping(message, user_info)
        else:
            raise ValueError(f"Unknown message type: {message_type}")
    
    async def handle_subscribe(self, message: Dict[str, Any], user_info: Dict[str, Any]):
        """Handle subscription requests"""
        subscription_type = message.get("subscription_type")
        connection_id = f"{user_info['user_id']}_{user_info['username']}"
        
        if subscription_type == "telemetry":
            # Subscribe to telemetry updates
            await self.telemetry_service.subscribe_user(user_info['user_id'])
            await self.manager.send_personal_message({
                "type": "subscription_confirmed",
                "subscription_type": "telemetry",
                "timestamp": datetime.utcnow().isoformat()
            }, connection_id)
            
        elif subscription_type == "lidar":
            # Subscribe to LiDAR updates
            await self.lidar_service.subscribe_user(user_info['user_id'])
            await self.manager.send_personal_message({
                "type": "subscription_confirmed",
                "subscription_type": "lidar",
                "timestamp": datetime.utcnow().isoformat()
            }, connection_id)
            
        elif subscription_type == "robot_status":
            # Subscribe to robot status updates
            await self.robot_service.subscribe_user(user_info['user_id'])
            await self.manager.send_personal_message({
                "type": "subscription_confirmed",
                "subscription_type": "robot_status",
                "timestamp": datetime.utcnow().isoformat()
            }, connection_id)
            
        else:
            raise ValueError(f"Unknown subscription type: {subscription_type}")
    
    async def handle_unsubscribe(self, message: Dict[str, Any], user_info: Dict[str, Any]):
        """Handle unsubscription requests"""
        subscription_type = message.get("subscription_type")
        connection_id = f"{user_info['user_id']}_{user_info['username']}"
        
        if subscription_type == "telemetry":
            await self.telemetry_service.unsubscribe_user(user_info['user_id'])
        elif subscription_type == "lidar":
            await self.lidar_service.unsubscribe_user(user_info['user_id'])
        elif subscription_type == "robot_status":
            await self.robot_service.unsubscribe_user(user_info['user_id'])
        
        await self.manager.send_personal_message({
            "type": "unsubscription_confirmed",
            "subscription_type": subscription_type,
            "timestamp": datetime.utcnow().isoformat()
        }, connection_id)
    
    async def handle_robot_control(self, message: Dict[str, Any], user_info: Dict[str, Any]):
        """Handle robot control commands"""
        command = message.get("command")
        parameters = message.get("parameters", {})
        connection_id = f"{user_info['user_id']}_{user_info['username']}"
        
        try:
            # Execute robot command
            result = await self.robot_service.execute_command(command, parameters, user_info['user_id'])
            
            # Send confirmation
            await self.manager.send_personal_message({
                "type": "robot_control_response",
                "command": command,
                "success": result.success,
                "message": result.message,
                "timestamp": datetime.utcnow().isoformat()
            }, connection_id)
            
        except Exception as e:
            await self.manager.send_personal_message({
                "type": "robot_control_response",
                "command": command,
                "success": False,
                "message": str(e),
                "timestamp": datetime.utcnow().isoformat()
            }, connection_id)
    
    async def handle_lidar_control(self, message: Dict[str, Any], user_info: Dict[str, Any]):
        """Handle LiDAR control commands"""
        command = message.get("command")
        parameters = message.get("parameters", {})
        connection_id = f"{user_info['user_id']}_{user_info['username']}"
        
        try:
            # Execute LiDAR command
            result = await self.lidar_service.execute_command(command, parameters, user_info['user_id'])
            
            # Send confirmation
            await self.manager.send_personal_message({
                "type": "lidar_control_response",
                "command": command,
                "success": result.success,
                "message": result.message,
                "timestamp": datetime.utcnow().isoformat()
            }, connection_id)
            
        except Exception as e:
            await self.manager.send_personal_message({
                "type": "lidar_control_response",
                "command": command,
                "success": False,
                "message": str(e),
                "timestamp": datetime.utcnow().isoformat()
            }, connection_id)
    
    async def handle_ping(self, message: Dict[str, Any], user_info: Dict[str, Any]):
        """Handle ping messages for connection health"""
        connection_id = f"{user_info['user_id']}_{user_info['username']}"
        
        await self.manager.send_personal_message({
            "type": "pong",
            "timestamp": datetime.utcnow().isoformat()
        }, connection_id)
```

---

## 5. DATA STREAMING

### 5.1 Telemetry Streaming
```python
# app/websocket/streaming.py
import asyncio
import json
from datetime import datetime
from typing import Dict, Any, Set
from app.websocket.manager import ConnectionManager

class TelemetryStreamer:
    def __init__(self, connection_manager: ConnectionManager):
        self.manager = connection_manager
        self.subscribed_users: Set[int] = set()
        self.streaming_task: Optional[asyncio.Task] = None
    
    async def subscribe_user(self, user_id: int):
        """Subscribe user to telemetry streaming"""
        self.subscribed_users.add(user_id)
        
        if not self.streaming_task or self.streaming_task.done():
            self.streaming_task = asyncio.create_task(self.stream_telemetry())
    
    async def unsubscribe_user(self, user_id: int):
        """Unsubscribe user from telemetry streaming"""
        self.subscribed_users.discard(user_id)
        
        if not self.subscribed_users and self.streaming_task:
            self.streaming_task.cancel()
    
    async def stream_telemetry(self):
        """Stream telemetry data to subscribed users"""
        try:
            while self.subscribed_users:
                # Get latest telemetry data
                telemetry_data = await self.get_latest_telemetry()
                
                # Create telemetry message
                message = {
                    "type": "telemetry_update",
                    "data": telemetry_data,
                    "timestamp": datetime.utcnow().isoformat()
                }
                
                # Send to all subscribed users
                for user_id in self.subscribed_users:
                    await self.manager.send_to_user(message, user_id)
                
                # Wait before next update
                await asyncio.sleep(0.1)  # 10 Hz update rate
                
        except asyncio.CancelledError:
            logger.info("Telemetry streaming cancelled")
        except Exception as e:
            logger.error(f"Telemetry streaming error: {e}")
    
    async def get_latest_telemetry(self) -> Dict[str, Any]:
        """Get latest telemetry data from robot"""
        # This would integrate with the actual robot telemetry service
        return {
            "robot_id": 1,
            "status": "MOVING",
            "position": {"x": 10.5, "y": 25.3, "z": 0.0},
            "speed": 0.5,
            "battery_level": 85,
            "temperature": 45.2
        }

class LiDARStreamer:
    def __init__(self, connection_manager: ConnectionManager):
        self.manager = connection_manager
        self.subscribed_users: Set[int] = set()
        self.streaming_task: Optional[asyncio.Task] = None
    
    async def subscribe_user(self, user_id: int):
        """Subscribe user to LiDAR streaming"""
        self.subscribed_users.add(user_id)
        
        if not self.streaming_task or self.streaming_task.done():
            self.streaming_task = asyncio.create_task(self.stream_lidar_data())
    
    async def unsubscribe_user(self, user_id: int):
        """Unsubscribe user from LiDAR streaming"""
        self.subscribed_users.discard(user_id)
        
        if not self.subscribed_users and self.streaming_task:
            self.streaming_task.cancel()
    
    async def stream_lidar_data(self):
        """Stream LiDAR data to subscribed users"""
        try:
            while self.subscribed_users:
                # Get latest LiDAR data
                lidar_data = await self.get_latest_lidar_data()
                
                # Create LiDAR message
                message = {
                    "type": "lidar_update",
                    "data": lidar_data,
                    "timestamp": datetime.utcnow().isoformat()
                }
                
                # Send to all subscribed users
                for user_id in self.subscribed_users:
                    await self.manager.send_to_user(message, user_id)
                
                # Wait before next update
                await asyncio.sleep(0.05)  # 20 Hz update rate
                
        except asyncio.CancelledError:
            logger.info("LiDAR streaming cancelled")
        except Exception as e:
            logger.error(f"LiDAR streaming error: {e}")
    
    async def get_latest_lidar_data(self) -> Dict[str, Any]:
        """Get latest LiDAR data"""
        # This would integrate with the actual LiDAR service
        return {
            "scan_data": {
                "points": [[1.0, 2.0], [2.0, 3.0], [3.0, 4.0]],
                "quality": 0.95
            },
            "obstacles": [
                {
                    "id": "obs_1",
                    "position": {"x": 5.0, "y": 5.0},
                    "type": "static",
                    "confidence": 0.9
                }
            ],
            "safety_zones": [
                {
                    "id": "sz_1",
                    "type": "warning",
                    "boundary": [[0, 0], [10, 0], [10, 10], [0, 10]]
                }
            ]
        }
```

---

## 6. MESSAGE VALIDATION

### 6.1 Message Schema Validation
```python
# app/websocket/validation.py
from pydantic import BaseModel, validator
from typing import Optional, Dict, Any, List
from enum import Enum

class MessageType(str, Enum):
    SUBSCRIBE = "subscribe"
    UNSUBSCRIBE = "unsubscribe"
    ROBOT_CONTROL = "robot_control"
    LIDAR_CONTROL = "lidar_control"
    PING = "ping"

class SubscriptionType(str, Enum):
    TELEMETRY = "telemetry"
    LIDAR = "lidar"
    ROBOT_STATUS = "robot_status"

class RobotCommand(str, Enum):
    MOVE_FORWARD = "move_forward"
    MOVE_BACKWARD = "move_backward"
    TURN_LEFT = "turn_left"
    TURN_RIGHT = "turn_right"
    STOP = "stop"
    DOCK = "dock"
    UNDOCK = "undock"
    EMERGENCY_STOP = "emergency_stop"

class LiDARCommand(str, Enum):
    START_SCAN = "start_scan"
    STOP_SCAN = "stop_scan"
    SET_FREQUENCY = "set_frequency"
    SET_RANGE = "set_range"

class BaseMessage(BaseModel):
    type: MessageType
    timestamp: Optional[str] = None

class SubscribeMessage(BaseMessage):
    type: MessageType = MessageType.SUBSCRIBE
    subscription_type: SubscriptionType
    
    @validator('subscription_type')
    def validate_subscription_type(cls, v):
        if v not in SubscriptionType:
            raise ValueError(f"Invalid subscription type: {v}")
        return v

class UnsubscribeMessage(BaseMessage):
    type: MessageType = MessageType.UNSUBSCRIBE
    subscription_type: SubscriptionType

class RobotControlMessage(BaseMessage):
    type: MessageType = MessageType.ROBOT_CONTROL
    command: RobotCommand
    parameters: Optional[Dict[str, Any]] = {}
    
    @validator('command')
    def validate_command(cls, v):
        if v not in RobotCommand:
            raise ValueError(f"Invalid robot command: {v}")
        return v
    
    @validator('parameters')
    def validate_parameters(cls, v):
        if v is None:
            return {}
        
        # Validate speed parameter
        if 'speed' in v:
            speed = v['speed']
            if not isinstance(speed, (int, float)) or speed < 0 or speed > 10:
                raise ValueError("Speed must be between 0 and 10")
        
        # Validate duration parameter
        if 'duration' in v:
            duration = v['duration']
            if not isinstance(duration, (int, float)) or duration < 0 or duration > 3600:
                raise ValueError("Duration must be between 0 and 3600 seconds")
        
        return v

class LiDARControlMessage(BaseMessage):
    type: MessageType = MessageType.LIDAR_CONTROL
    command: LiDARCommand
    parameters: Optional[Dict[str, Any]] = {}
    
    @validator('command')
    def validate_command(cls, v):
        if v not in LiDARCommand:
            raise ValueError(f"Invalid LiDAR command: {v}")
        return v
    
    @validator('parameters')
    def validate_parameters(cls, v):
        if v is None:
            return {}
        
        # Validate frequency parameter
        if 'frequency' in v:
            freq = v['frequency']
            if not isinstance(freq, (int, float)) or freq < 1 or freq > 50:
                raise ValueError("Frequency must be between 1 and 50 Hz")
        
        # Validate range parameter
        if 'range' in v:
            range_val = v['range']
            if not isinstance(range_val, (int, float)) or range_val < 0.1 or range_val > 20:
                raise ValueError("Range must be between 0.1 and 20 meters")
        
        return v

class PingMessage(BaseMessage):
    type: MessageType = MessageType.PING

class MessageValidator:
    @staticmethod
    def validate_message(message_data: Dict[str, Any]) -> BaseMessage:
        """Validate incoming message data"""
        
        message_type = message_data.get("type")
        if not message_type:
            raise ValueError("Message type is required")
        
        try:
            message_type_enum = MessageType(message_type)
        except ValueError:
            raise ValueError(f"Invalid message type: {message_type}")
        
        # Validate based on message type
        if message_type_enum == MessageType.SUBSCRIBE:
            return SubscribeMessage(**message_data)
        elif message_type_enum == MessageType.UNSUBSCRIBE:
            return UnsubscribeMessage(**message_data)
        elif message_type_enum == MessageType.ROBOT_CONTROL:
            return RobotControlMessage(**message_data)
        elif message_type_enum == MessageType.LIDAR_CONTROL:
            return LiDARControlMessage(**message_data)
        elif message_type_enum == MessageType.PING:
            return PingMessage(**message_data)
        else:
            raise ValueError(f"Unknown message type: {message_type}")
```

---

## 7. ERROR HANDLING & RECONNECTION

### 7.1 Error Handling
```python
# app/websocket/error_handling.py
import asyncio
import logging
from typing import Optional
from datetime import datetime

logger = logging.getLogger(__name__)

class WebSocketErrorHandler:
    def __init__(self):
        self.error_counts = {}
        self.max_errors = 5
        self.error_window = 60  # seconds
    
    async def handle_connection_error(self, connection_id: str, error: Exception):
        """Handle WebSocket connection errors"""
        current_time = datetime.utcnow()
        
        if connection_id not in self.error_counts:
            self.error_counts[connection_id] = []
        
        # Add error to count
        self.error_counts[connection_id].append(current_time)
        
        # Clean old errors
        cutoff_time = current_time - timedelta(seconds=self.error_window)
        self.error_counts[connection_id] = [
            error_time for error_time in self.error_counts[connection_id]
            if error_time > cutoff_time
        ]
        
        # Check if too many errors
        if len(self.error_counts[connection_id]) >= self.max_errors:
            logger.warning(f"Too many errors for connection {connection_id}, closing")
            return False
        
        logger.error(f"WebSocket error for {connection_id}: {error}")
        return True
    
    def reset_error_count(self, connection_id: str):
        """Reset error count for connection"""
        if connection_id in self.error_counts:
            del self.error_counts[connection_id]

class ReconnectionManager:
    def __init__(self):
        self.reconnection_attempts = {}
        self.max_attempts = 5
        self.base_delay = 1  # seconds
        self.max_delay = 30  # seconds
    
    async def handle_disconnection(self, connection_id: str):
        """Handle WebSocket disconnection"""
        if connection_id not in self.reconnection_attempts:
            self.reconnection_attempts[connection_id] = 0
        
        attempts = self.reconnection_attempts[connection_id]
        
        if attempts < self.max_attempts:
            # Calculate delay with exponential backoff
            delay = min(self.base_delay * (2 ** attempts), self.max_delay)
            
            logger.info(f"Attempting reconnection for {connection_id} in {delay} seconds")
            
            await asyncio.sleep(delay)
            
            self.reconnection_attempts[connection_id] += 1
            return True
        else:
            logger.warning(f"Max reconnection attempts reached for {connection_id}")
            return False
    
    def reset_reconnection_attempts(self, connection_id: str):
        """Reset reconnection attempts for connection"""
        if connection_id in self.reconnection_attempts:
            del self.reconnection_attempts[connection_id]
```

---

## 8. PERFORMANCE OPTIMIZATION

### 8.1 Connection Pooling
```python
# app/websocket/pooling.py
import asyncio
from typing import Dict, Set, Optional
from fastapi import WebSocket

class ConnectionPool:
    def __init__(self, max_connections: int = 1000):
        self.max_connections = max_connections
        self.active_connections: Dict[str, WebSocket] = {}
        self.connection_groups: Dict[str, Set[str]] = {}
    
    async def add_connection(self, connection_id: str, websocket: WebSocket, group: str = "default"):
        """Add connection to pool"""
        if len(self.active_connections) >= self.max_connections:
            # Remove oldest connection
            oldest_connection = next(iter(self.active_connections))
            await self.remove_connection(oldest_connection)
        
        self.active_connections[connection_id] = websocket
        
        if group not in self.connection_groups:
            self.connection_groups[group] = set()
        self.connection_groups[group].add(connection_id)
    
    async def remove_connection(self, connection_id: str):
        """Remove connection from pool"""
        if connection_id in self.active_connections:
            del self.active_connections[connection_id]
            
            # Remove from groups
            for group_connections in self.connection_groups.values():
                group_connections.discard(connection_id)
    
    async def broadcast_to_group(self, message: str, group: str):
        """Broadcast message to specific group"""
        if group in self.connection_groups:
            disconnected = []
            
            for connection_id in self.connection_groups[group]:
                if connection_id in self.active_connections:
                    try:
                        await self.active_connections[connection_id].send_text(message)
                    except Exception:
                        disconnected.append(connection_id)
            
            # Clean up disconnected connections
            for connection_id in disconnected:
                await self.remove_connection(connection_id)
    
    def get_connection_count(self) -> int:
        """Get total connection count"""
        return len(self.active_connections)
    
    def get_group_connection_count(self, group: str) -> int:
        """Get connection count for specific group"""
        return len(self.connection_groups.get(group, set()))
```

---

## 9. REFERENCES

### 9.1 Related Documents
- REQ_BE_01: System Overview
- REQ_BE_02: API Specifications
- REQ_BE_03: Database Design
- REQ_BE_04: Security Requirements
- REQ_BE_05: Integration Specifications
- REQ_BE_06: LiDAR Integration
- REQ_BE_07: Detailed Architecture Requirements
- REQ_BE_08: Service Design Requirements
- REQ_BE_09: Data Flow Requirements
- REQ_BE_10: Performance Requirements
- REQ_BE_11: Implementation Roadmap
- REQ_BE_12: Development Environment
- REQ_BE_13: Testing Requirements
- REQ_BE_14: Deployment Requirements
- REQ_BE_15: API Documentation
- REQ_BE_16: Database Migration
- REQ_BE_17: Security Implementation

### 9.2 Standards References
- ISO/IEC/IEEE 29148:2018 - Requirements Engineering
- ISO/IEC 25010:2011 - Software Quality Requirements
- RFC 6455 - WebSocket Protocol
- RFC 7230 - HTTP/1.1 Message Syntax and Routing
- WebSocket API - W3C Recommendation

---

## 10. APPROVAL

| Role | Name | Date | Signature |
|------|------|------|-----------|
| CTO | [Name] | [Date] | [Signature] |
| Technical Lead | [Name] | [Date] | [Signature] |
| Backend Developer | [Name] | [Date] | [Signature] |

---

**Document End**
