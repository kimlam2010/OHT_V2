# ADR-004: Communication Protocols Decision

## Status
**APPROVED** - 2024-08-21

## Context
OHT-50 Master Module cần giao thức giao tiếp cho:
- Hardware communication (RS485/Modbus)
- Real-time telemetry streaming
- API communication với Center
- WebSocket connections
- Mobile app communication
- Safety-critical messaging
- Network redundancy

## Decision
**Chọn Multi-Protocol Architecture với Protocol Gateway**

### Communication Architecture:
```
┌─────────────────────────────────────┐
│         Protocol Gateway            │
└─────────────────┬───────────────────┘
                  │
    ┌─────────────┼─────────────┐
    │             │             │
┌───▼───┐   ┌────▼────┐   ┌────▼────┐
│ HTTP  │   │WebSocket│   │  RS485  │
│ REST  │   │Streaming│   │ Modbus  │
└───────┘   └─────────┘   └─────────┘
    │             │             │
┌───▼───┐   ┌────▼────┐   ┌────▼────┐
│Center │   │Frontend │   │Hardware │
│System │   │  UI     │   │Modules  │
└───────┘   └─────────┘   └─────────┘
```

### Protocol Specifications:

#### 1. HTTP/REST API
```yaml
# API Configuration
api_config:
  base_url: "https://api.oht50.local"
  version: "v1"
  timeout: 30
  retry_attempts: 3
  
# Endpoints
endpoints:
  config: "/api/v1/config"
  missions: "/api/v1/missions"
  telemetry: "/api/v1/telemetry"
  safety: "/api/v1/safety"
  auth: "/api/v1/auth"
```

#### 2. WebSocket Streaming
```json
{
  "protocol": "ws://oht50.local/ws",
  "message_types": {
    "telemetry": "real-time sensor data",
    "safety_events": "safety alerts",
    "mission_status": "mission updates",
    "system_status": "system health"
  },
  "subscription": {
    "topics": ["telemetry", "safety", "missions"],
    "frequency": "100ms"
  }
}
```

#### 3. RS485/Modbus RTU
```python
# Modbus Configuration
MODBUS_CONFIG = {
    "port": "/dev/ttyOHT485",
    "baudrate": 115200,
    "parity": "N",
    "stopbits": 1,
    "timeout": 1.0,
    "retry_count": 3,
    
    # Register Map
    "registers": {
        "position_x": 1000,
        "position_y": 1001,
        "velocity": 1002,
        "status": 1003,
        "safety_status": 1004,
        "command": 2000,
        "target_position": 2001
    }
}
```

## Consequences

### Positive:
- **Flexibility**: Multiple protocols cho different use cases
- **Reliability**: Protocol redundancy
- **Performance**: Optimized cho each use case
- **Standards**: Industry-standard protocols
- **Interoperability**: Easy integration với existing systems
- **Scalability**: Protocol-independent scaling

### Negative:
- **Complexity**: Multiple protocol management
- **Maintenance**: Protocol-specific code
- **Testing**: Multi-protocol testing complexity
- **Debugging**: Protocol-specific debugging

### Risks:
- **Protocol Conflicts**: Interference between protocols
- **Performance Overhead**: Protocol translation
- **Security Vulnerabilities**: Multiple attack vectors
- **Network Congestion**: Bandwidth management

## Mitigation Strategies:
1. **Protocol Isolation**: Separate protocol handlers
2. **Message Translation**: Centralized message converter
3. **Security Hardening**: Protocol-specific security
4. **Performance Monitoring**: Protocol performance metrics
5. **Error Handling**: Graceful protocol failures

## Performance Impact Assessment:
- **HTTP API**: < 100ms response time
- **WebSocket**: < 10ms message latency
- **RS485**: < 50ms command response
- **Bandwidth**: < 1MB/s total usage
- **Concurrent Connections**: 100+ WebSocket, 10+ HTTP

## Security Considerations:
```yaml
security_config:
  http:
    authentication: "JWT Bearer"
    encryption: "TLS 1.3"
    rate_limiting: "100 requests/minute"
  
  websocket:
    authentication: "JWT Token"
    encryption: "WSS (TLS)"
    connection_limit: "100 concurrent"
  
  rs485:
    authentication: "None (hardware level)"
    encryption: "None (physical security)"
    access_control: "Hardware isolation"
```

## Protocol Gateway Implementation:
```python
class ProtocolGateway:
    def __init__(self):
        self.http_handler = HTTPHandler()
        self.websocket_handler = WebSocketHandler()
        self.modbus_handler = ModbusHandler()
        self.message_translator = MessageTranslator()
    
    async def route_message(self, protocol: str, message: dict):
        """Route message to appropriate protocol handler"""
        if protocol == "http":
            return await self.http_handler.process(message)
        elif protocol == "websocket":
            return await self.websocket_handler.process(message)
        elif protocol == "modbus":
            return await self.modbus_handler.process(message)
        else:
            raise ValueError(f"Unsupported protocol: {protocol}")
    
    def translate_message(self, from_protocol: str, to_protocol: str, message: dict):
        """Translate message between protocols"""
        return self.message_translator.translate(from_protocol, to_protocol, message)
```

## Network Redundancy:
```yaml
redundancy_config:
  primary: "Ethernet"
  backup: "WiFi"
  failover_time: "5 seconds"
  health_check_interval: "30 seconds"
  
  protocols:
    ethernet:
      interface: "eth0"
      ip: "192.168.1.100"
      gateway: "192.168.1.1"
    
    wifi:
      interface: "wlan0"
      ssid: "OHT50_Backup"
      ip: "192.168.2.100"
      gateway: "192.168.2.1"
```

## Monitoring & Metrics:
```python
# Protocol Metrics
PROTOCOL_METRICS = {
    "http_requests_total": Counter("http_requests_total", ["method", "endpoint"]),
    "websocket_messages_total": Counter("websocket_messages_total", ["topic"]),
    "modbus_commands_total": Counter("modbus_commands_total", ["command_type"]),
    "protocol_latency": Histogram("protocol_latency_seconds", ["protocol"]),
    "connection_errors": Counter("connection_errors_total", ["protocol", "error_type"])
}
```

## Alternatives Considered:
1. **Single Protocol**: Limited flexibility
2. **MQTT**: Overkill cho current requirements
3. **gRPC**: Complex cho embedded systems
4. **Custom Protocol**: Non-standard approach

## Compliance Requirements:
- **Safety Standards**: SIL2 compliance
- **Network Security**: Industry standards
- **Data Integrity**: Message validation
- **Performance**: Real-time requirements
- **Reliability**: 99.9% uptime

## Review Schedule:
- **Daily**: Protocol health checks
- **Weekly**: Performance monitoring
- **Monthly**: Security audit
- **Quarterly**: Protocol optimization

---

**Approved by:** CTO  
**Date:** 2024-08-21  
**Next Review:** 2024-09-21
