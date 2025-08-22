# Communication Protocols - OHT-50 Backend

## 📋 Tổng quan
Hướng dẫn các giao thức giao tiếp được sử dụng trong OHT-50 Backend, bao gồm HTTP/HTTPS, WebSocket, RS485 và CAN Bus.

## 🔧 Core Protocols

### 1. HTTP/HTTPS API Protocol

#### Request/Response Format
```python
# Standard HTTP Request
GET /api/v1/missions HTTP/1.1
Host: localhost:8000
Authorization: Bearer <jwt_token>
Content-Type: application/json

# Standard HTTP Response
HTTP/1.1 200 OK
Content-Type: application/json

{
    "success": true,
    "data": [
        {
            "id": 1,
            "mission_id": "mission_001",
            "status": "running"
        }
    ]
}

# Error Response
HTTP/1.1 400 Bad Request
{
    "success": false,
    "error_code": "VALIDATION_ERROR",
    "message": "Invalid parameters"
}
```

#### Authentication Protocol
```python
# JWT Token Format
{
    "header": {"alg": "HS256", "typ": "JWT"},
    "payload": {
        "sub": "user_id",
        "role": "operator",
        "exp": 1642234567
    }
}

# Authentication Flow
1. POST /api/v1/auth/login
2. Server returns access_token
3. Client uses: Authorization: Bearer <token>
```

### 2. WebSocket Protocol

#### Connection Handshake
```python
# WebSocket Upgrade Request
GET /ws/telemetry HTTP/1.1
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Key: dGhlIHNhbXBsZSBub25jZQ==
Authorization: Bearer <jwt_token>

# WebSocket Upgrade Response
HTTP/1.1 101 Switching Protocols
Upgrade: websocket
Connection: Upgrade
Sec-WebSocket-Accept: s3pPLMBiTxaQ9kYGzzhZRbK+xOo=
```

#### Message Format
```python
# Telemetry Message
{
    "type": "telemetry",
    "timestamp": "2024-01-15T10:30:00Z",
    "module_id": "module_001",
    "data": {
        "position": {"x": 100.5, "y": 200.3, "z": 0.0},
        "sensors": {"temperature": 25.5, "battery": 85.0},
        "status": {"system_status": "moving", "safety_status": "safe"}
    }
}

# Safety Event Message
{
    "type": "safety_event",
    "event_type": "e_stop",
    "severity": "critical",
    "description": "Emergency stop triggered"
}

# Subscription
{
    "type": "subscribe",
    "channels": ["telemetry", "safety_events"],
    "filters": {"module_id": "module_001"}
}
```

### 3. RS485 Protocol

#### Frame Format
```python
# RS485 Frame Structure
[START_BYTE] [LENGTH] [ADDRESS] [COMMAND] [DATA] [CRC] [END_BYTE]

# Example: Move to position command
0xAA 0x0C 0x01 0x10 0x64 0x00 0xC8 0x00 0x00 0x00 0x00 0x00 0x1234 0x55

# Field descriptions:
# START_BYTE: 0xAA (170)
# LENGTH: 0x0C (12 bytes of data)
# ADDRESS: 0x01 (Module ID 1)
# COMMAND: 0x10 (Move to position)
# DATA: Position data (x, y, z, theta)
# CRC: 16-bit CRC checksum
# END_BYTE: 0x55 (85)
```

#### Command Set
```python
# Command definitions
COMMANDS = {
    0x10: "MOVE_TO_POSITION",
    0x11: "GET_POSITION",
    0x12: "SET_SPEED",
    0x13: "GET_SPEED",
    0x14: "EMERGENCY_STOP",
    0x15: "GET_STATUS",
    0x16: "CALIBRATE",
    0x17: "GET_SENSORS"
}

# Status codes
STATUS_CODES = {
    0x00: "SUCCESS",
    0x01: "INVALID_COMMAND",
    0x02: "INVALID_PARAMETERS",
    0x03: "EXECUTION_ERROR",
    0x04: "BUSY",
    0x05: "TIMEOUT",
    0x06: "SAFETY_VIOLATION"
}
```

### 4. CAN Bus Protocol

#### CAN Frame Format
```python
# Standard CAN Frame (11-bit ID)
{
    "id": 0x123,  # 11-bit identifier
    "dlc": 8,     # Data length code
    "data": [0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08],
    "rtr": False, # Remote transmission request
    "ide": False  # Identifier extension
}

# Extended CAN Frame (29-bit ID)
{
    "id": 0x18FF1234,  # 29-bit identifier
    "dlc": 8,          # Data length code
    "data": [0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08],
    "rtr": False,      # Remote transmission request
    "ide": True        # Identifier extension
}
```

#### CAN Message IDs
```python
# CAN ID allocation
CAN_IDS = {
    # System messages (0x000-0x0FF)
    0x001: "SYSTEM_STATUS",
    0x002: "SYSTEM_CONFIG",
    0x003: "SYSTEM_ERROR",
    
    # Mission messages (0x100-0x1FF)
    0x101: "MISSION_COMMAND",
    0x102: "MISSION_STATUS",
    0x103: "MISSION_PROGRESS",
    
    # Safety messages (0x200-0x2FF)
    0x201: "SAFETY_STATUS",
    0x202: "SAFETY_EVENT",
    0x203: "EMERGENCY_STOP",
    
    # Sensor messages (0x300-0x3FF)
    0x301: "POSITION_DATA",
    0x302: "SENSOR_DATA",
    0x303: "ENCODER_DATA"
}
```

## 🔄 Protocol Integration

### Protocol Gateway
```python
class ProtocolGateway:
    def __init__(self):
        self.protocols = {
            'http': HTTPProtocol(),
            'websocket': WebSocketProtocol(),
            'rs485': RS485Protocol(),
            'can': CANProtocol()
        }
    
    async def handle_message(self, protocol: str, message: dict):
        """Route message to appropriate protocol handler"""
        if protocol in self.protocols:
            return await self.protocols[protocol].handle_message(message)
        else:
            raise ValueError(f"Unsupported protocol: {protocol}")
    
    async def broadcast_message(self, message: dict, protocols: list = None):
        """Broadcast message to multiple protocols"""
        if protocols is None:
            protocols = list(self.protocols.keys())
        
        tasks = []
        for protocol in protocols:
            if protocol in self.protocols:
                task = asyncio.create_task(
                    self.protocols[protocol].send_message(message)
                )
                tasks.append(task)
        
        await asyncio.gather(*tasks, return_exceptions=True)
```

### Message Translation
```python
class MessageTranslator:
    def __init__(self):
        self.translators = {
            'http_to_rs485': self.http_to_rs485,
            'rs485_to_http': self.rs485_to_http,
            'websocket_to_can': self.websocket_to_can,
            'can_to_websocket': self.can_to_websocket
        }
    
    def translate_message(self, from_protocol: str, to_protocol: str, message: dict) -> dict:
        """Translate message between protocols"""
        translator_key = f"{from_protocol}_to_{to_protocol}"
        
        if translator_key in self.translators:
            return self.translators[translator_key](message)
        else:
            raise ValueError(f"No translator for {translator_key}")
    
    def http_to_rs485(self, message: dict) -> dict:
        """Translate HTTP message to RS485 format"""
        if message.get('type') == 'mission_command':
            return {
                'command': 0x10,  # MOVE_TO_POSITION
                'address': message.get('module_id', 1),
                'data': self._position_to_bytes(message.get('target_position', {}))
            }
        return message
    
    def rs485_to_http(self, message: dict) -> dict:
        """Translate RS485 message to HTTP format"""
        if message.get('command') == 0x10:  # MOVE_TO_POSITION
            return {
                'type': 'mission_status',
                'module_id': message.get('address'),
                'status': 'completed' if message.get('status') == 0x00 else 'failed'
            }
        return message
    
    def _position_to_bytes(self, position: dict) -> list:
        """Convert position to byte array"""
        x = int(position.get('x', 0) * 100)  # Convert to mm
        y = int(position.get('y', 0) * 100)
        z = int(position.get('z', 0) * 100)
        theta = int(position.get('theta', 0) * 1000)  # Convert to milliradians
        
        return [
            (x >> 8) & 0xFF, x & 0xFF,
            (y >> 8) & 0xFF, y & 0xFF,
            (z >> 8) & 0xFF, z & 0xFF,
            (theta >> 8) & 0xFF, theta & 0xFF
        ]
```

## 🔒 Security Protocols

### TLS/SSL Configuration
```python
# SSL/TLS Configuration
SSL_CONFIG = {
    'certfile': '/path/to/certificate.pem',
    'keyfile': '/path/to/private_key.pem',
    'ca_certs': '/path/to/ca_certificates.pem',
    'cert_reqs': ssl.CERT_REQUIRED,
    'ssl_version': ssl.PROTOCOL_TLSv1_3,
    'ciphers': 'ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256'
}
```

### Message Encryption
```python
from cryptography.fernet import Fernet
import base64

class MessageEncryption:
    def __init__(self, key: str = None):
        self.key = key or Fernet.generate_key()
        self.cipher = Fernet(self.key)
    
    def encrypt_message(self, message: dict) -> str:
        """Encrypt message for secure transmission"""
        message_str = json.dumps(message)
        encrypted_data = self.cipher.encrypt(message_str.encode())
        return base64.b64encode(encrypted_data).decode()
    
    def decrypt_message(self, encrypted_message: str) -> dict:
        """Decrypt message"""
        encrypted_data = base64.b64decode(encrypted_message.encode())
        decrypted_data = self.cipher.decrypt(encrypted_data)
        return json.loads(decrypted_data.decode())
```

## 📊 Protocol Monitoring

### Protocol Metrics
```python
from prometheus_client import Counter, Histogram, Gauge

class ProtocolMetrics:
    def __init__(self):
        # Message counters
        self.messages_total = Counter(
            'protocol_messages_total',
            'Total messages by protocol',
            ['protocol', 'direction', 'type']
        )
        
        # Message latency
        self.message_latency = Histogram(
            'protocol_message_latency_seconds',
            'Message latency by protocol',
            ['protocol', 'type']
        )
        
        # Connection status
        self.connections_active = Gauge(
            'protocol_connections_active',
            'Active connections by protocol',
            ['protocol']
        )
        
        # Error counters
        self.errors_total = Counter(
            'protocol_errors_total',
            'Total errors by protocol',
            ['protocol', 'error_type']
        )
    
    def record_message(self, protocol: str, direction: str, message_type: str):
        """Record message metric"""
        self.messages_total.labels(
            protocol=protocol,
            direction=direction,
            type=message_type
        ).inc()
    
    def record_latency(self, protocol: str, message_type: str, latency: float):
        """Record latency metric"""
        self.message_latency.labels(
            protocol=protocol,
            type=message_type
        ).observe(latency)
    
    def set_connections(self, protocol: str, count: int):
        """Set active connections count"""
        self.connections_active.labels(protocol=protocol).set(count)
    
    def record_error(self, protocol: str, error_type: str):
        """Record error metric"""
        self.errors_total.labels(
            protocol=protocol,
            error_type=error_type
        ).inc()
```

### Protocol Health Checks
```python
class ProtocolHealthChecker:
    def __init__(self):
        self.health_checks = {
            'http': self.check_http_health,
            'websocket': self.check_websocket_health,
            'rs485': self.check_rs485_health,
            'can': self.check_can_health
        }
    
    async def check_all_protocols(self) -> dict:
        """Check health of all protocols"""
        results = {}
        
        for protocol, check_func in self.health_checks.items():
            try:
                results[protocol] = await check_func()
            except Exception as e:
                results[protocol] = {
                    'status': 'unhealthy',
                    'error': str(e),
                    'timestamp': datetime.utcnow().isoformat()
                }
        
        return results
    
    async def check_http_health(self) -> dict:
        """Check HTTP protocol health"""
        return {
            'status': 'healthy',
            'response_time': 0.1,
            'timestamp': datetime.utcnow().isoformat()
        }
    
    async def check_websocket_health(self) -> dict:
        """Check WebSocket protocol health"""
        return {
            'status': 'healthy',
            'connections': 5,
            'timestamp': datetime.utcnow().isoformat()
        }
    
    async def check_rs485_health(self) -> dict:
        """Check RS485 protocol health"""
        return {
            'status': 'healthy',
            'baud_rate': 115200,
            'timestamp': datetime.utcnow().isoformat()
        }
    
    async def check_can_health(self) -> dict:
        """Check CAN protocol health"""
        return {
            'status': 'healthy',
            'bit_rate': 500000,
            'timestamp': datetime.utcnow().isoformat()
        }
```

## 📋 Protocol Checklist

### HTTP/HTTPS Setup
- [ ] Configure SSL/TLS certificates
- [ ] Set up authentication middleware
- [ ] Implement rate limiting
- [ ] Configure CORS policies
- [ ] Set up request/response logging

### WebSocket Setup
- [ ] Configure WebSocket upgrade handling
- [ ] Implement connection management
- [ ] Set up message broadcasting
- [ ] Configure heartbeat mechanism
- [ ] Implement reconnection logic

### RS485 Setup
- [ ] Configure serial port parameters
- [ ] Set up frame validation
- [ ] Implement CRC checking
- [ ] Configure timeout handling
- [ ] Set up error recovery

### CAN Bus Setup
- [ ] Configure CAN interface
- [ ] Set up message filtering
- [ ] Implement error handling
- [ ] Configure bit rate
- [ ] Set up message queuing

---

**Changelog v1.0:**
- ✅ Created communication protocols guide
- ✅ Added HTTP/HTTPS protocol specifications
- ✅ Implemented WebSocket protocol
- ✅ Added RS485 protocol details
- ✅ Included CAN bus protocol
- ✅ Created protocol integration layer
- ✅ Added message translation
- ✅ Implemented security protocols
- ✅ Added protocol monitoring
- ✅ Created health checks