"""
FW Client Library for OHT-50 Master Module Backend Integration
Version: 1.0.0
Date: 2025-01-27
Team: Backend
Task: BE-INT-01 (FW Client Library)
"""

import asyncio
import aiohttp
import websockets
import json
import logging
from typing import Dict, List, Optional, Any, Callable
from dataclasses import dataclass
from enum import Enum
import time
import ssl

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class FWConnectionState(Enum):
    """FW connection states"""
    DISCONNECTED = "disconnected"
    CONNECTING = "connecting"
    CONNECTED = "connected"
    ERROR = "error"

class FWMessageType(Enum):
    """FW message types"""
    SYSTEM_STATUS = "system_status"
    MODULE_INFO = "module_info"
    MODULE_COMMAND = "module_command"
    SAFETY_STATUS = "safety_status"
    CONFIG_UPDATE = "config_update"
    DIAGNOSTICS = "diagnostics"
    TELEMETRY = "telemetry"
    EVENT = "event"

@dataclass
class FWConfig:
    """FW client configuration"""
    host: str = "localhost"
    http_port: int = 8080
    ws_port: int = 8081
    timeout: int = 30
    retry_count: int = 3
    retry_delay: float = 1.0
    auth_token: Optional[str] = None
    enable_ssl: bool = False
    verify_ssl: bool = True
    max_connections: int = 5
    heartbeat_interval: int = 30

@dataclass
class FWSystemStatus:
    """FW system status"""
    system_name: str
    version: str
    status: str
    uptime_ms: int
    active_modules: int
    estop_active: bool
    safety_ok: bool

@dataclass
class FWModuleInfo:
    """FW module information"""
    module_id: int
    module_type: str
    status: str
    version: str
    last_seen: int
    capabilities: List[str]

@dataclass
class FWSafetyStatus:
    """FW safety status"""
    estop_active: bool
    safety_ok: bool
    interlock_active: bool
    fault_detected: bool
    fault_code: Optional[int] = None
    fault_message: Optional[str] = None

class FWClientError(Exception):
    """FW client exception"""
    pass

class FWConnectionError(FWClientError):
    """FW connection exception"""
    pass

class FWTimeoutError(FWClientError):
    """FW timeout exception"""
    pass

class FWClient:
    """
    FW Client Library for connecting to OHT-50 Master Module Firmware
    """
    
    def __init__(self, config: FWConfig):
        """
        Initialize FW client
        
        Args:
            config: FW client configuration
        """
        self.config = config
        self.state = FWConnectionState.DISCONNECTED
        self.http_session: Optional[aiohttp.ClientSession] = None
        self.ws_connection: Optional[websockets.WebSocketServerProtocol] = None
        self.message_handlers: Dict[str, Callable] = {}
        self.event_handlers: Dict[str, Callable] = {}
        self._last_heartbeat = 0
        self._connection_id = None
        self._stats = {
            'requests_sent': 0,
            'requests_failed': 0,
            'messages_received': 0,
            'bytes_sent': 0,
            'bytes_received': 0,
            'last_activity': 0
        }
        
        logger.info(f"FW Client initialized for {config.host}:{config.http_port}")
    
    async def connect(self) -> bool:
        """
        Connect to FW HTTP and WebSocket servers
        
        Returns:
            True if connection successful
        """
        try:
            self.state = FWConnectionState.CONNECTING
            
            # Create HTTP session
            connector = aiohttp.TCPConnector(
                limit=self.config.max_connections,
                limit_per_host=self.config.max_connections,
                ssl=self._create_ssl_context() if self.config.enable_ssl else None
            )
            
            timeout = aiohttp.ClientTimeout(total=self.config.timeout)
            self.http_session = aiohttp.ClientSession(
                connector=connector,
                timeout=timeout,
                headers=self._get_auth_headers()
            )
            
            # Test HTTP connection
            await self._test_http_connection()
            
            # Connect to WebSocket
            await self._connect_websocket()
            
            self.state = FWConnectionState.CONNECTED
            self._last_heartbeat = time.time()
            self._stats['last_activity'] = time.time()
            
            logger.info("FW Client connected successfully")
            return True
            
        except Exception as e:
            self.state = FWConnectionState.ERROR
            logger.error(f"FW Client connection failed: {e}")
            await self.disconnect()
            return False
    
    async def disconnect(self):
        """Disconnect from FW servers"""
        try:
            self.state = FWConnectionState.DISCONNECTED
            
            if self.ws_connection:
                await self.ws_connection.close()
                self.ws_connection = None
            
            if self.http_session:
                await self.http_session.close()
                self.http_session = None
            
            logger.info("FW Client disconnected")
            
        except Exception as e:
            logger.error(f"Error during disconnect: {e}")
    
    async def _test_http_connection(self):
        """Test HTTP connection to FW"""
        try:
            url = f"{self._get_http_url()}/api/v1/system/health"
            async with self.http_session.get(url) as response:
                if response.status != 200:
                    raise FWConnectionError(f"HTTP health check failed: {response.status}")
                logger.info("HTTP connection test successful")
        except Exception as e:
            raise FWConnectionError(f"HTTP connection test failed: {e}")
    
    async def _connect_websocket(self):
        """Connect to FW WebSocket server"""
        try:
            ws_url = self._get_ws_url()
            self.ws_connection = await websockets.connect(
                ws_url,
                ping_interval=self.config.heartbeat_interval,
                ping_timeout=10,
                close_timeout=10
            )
            
            # Start WebSocket listener
            asyncio.create_task(self._ws_listener())
            
            logger.info("WebSocket connection established")
            
        except Exception as e:
            raise FWConnectionError(f"WebSocket connection failed: {e}")
    
    async def _ws_listener(self):
        """WebSocket message listener"""
        try:
            async for message in self.ws_connection:
                await self._handle_ws_message(message)
                self._stats['messages_received'] += 1
                self._stats['bytes_received'] += len(message)
                self._stats['last_activity'] = time.time()
                
        except websockets.exceptions.ConnectionClosed:
            logger.info("WebSocket connection closed")
        except Exception as e:
            logger.error(f"WebSocket listener error: {e}")
        finally:
            self.state = FWConnectionState.DISCONNECTED
    
    async def _handle_ws_message(self, message: str):
        """Handle WebSocket message"""
        try:
            data = json.loads(message)
            msg_type = data.get('type')
            
            if msg_type in self.message_handlers:
                await self.message_handlers[msg_type](data)
            else:
                logger.warning(f"No handler for message type: {msg_type}")
                
        except json.JSONDecodeError as e:
            logger.error(f"Invalid JSON message: {e}")
        except Exception as e:
            logger.error(f"Error handling WebSocket message: {e}")
    
    # HTTP API Methods
    
    async def get_system_status(self) -> FWSystemStatus:
        """Get FW system status"""
        try:
            url = f"{self._get_http_url()}/api/v1/system/status"
            async with self.http_session.get(url) as response:
                await self._handle_response(response)
                data = await response.json()
                
                return FWSystemStatus(
                    system_name=data['system_name'],
                    version=data['version'],
                    status=data['status'],
                    uptime_ms=data['uptime_ms'],
                    active_modules=data['active_modules'],
                    estop_active=data['estop_active'],
                    safety_ok=data['safety_ok']
                )
                
        except Exception as e:
            await self._handle_request_error("get_system_status", e)
            raise
    
    async def get_modules_list(self) -> List[FWModuleInfo]:
        """Get list of FW modules"""
        try:
            url = f"{self._get_http_url()}/api/v1/modules"
            async with self.http_session.get(url) as response:
                await self._handle_response(response)
                data = await response.json()
                
                modules = []
                for module_data in data['modules']:
                    modules.append(FWModuleInfo(
                        module_id=module_data['id'],
                        module_type=module_data['type'],
                        status=module_data['status'],
                        version=module_data['version'],
                        last_seen=module_data['last_seen'],
                        capabilities=module_data.get('capabilities', [])
                    ))
                
                return modules
                
        except Exception as e:
            await self._handle_request_error("get_modules_list", e)
            raise
    
    async def get_module_info(self, module_id: int) -> FWModuleInfo:
        """Get specific module information"""
        try:
            url = f"{self._get_http_url()}/api/v1/modules/{module_id}"
            async with self.http_session.get(url) as response:
                await self._handle_response(response)
                data = await response.json()
                
                return FWModuleInfo(
                    module_id=data['id'],
                    module_type=data['type'],
                    status=data['status'],
                    version=data['version'],
                    last_seen=data['last_seen'],
                    capabilities=data.get('capabilities', [])
                )
                
        except Exception as e:
            await self._handle_request_error(f"get_module_info({module_id})", e)
            raise
    
    async def send_module_command(self, module_id: int, command: str, params: Dict = None) -> Dict:
        """Send command to module"""
        try:
            url = f"{self._get_http_url()}/api/v1/modules/{module_id}/command"
            payload = {
                'command': command,
                'parameters': params or {}
            }
            
            async with self.http_session.post(url, json=payload) as response:
                await self._handle_response(response)
                return await response.json()
                
        except Exception as e:
            await self._handle_request_error(f"send_module_command({module_id})", e)
            raise
    
    async def get_safety_status(self) -> FWSafetyStatus:
        """Get FW safety status"""
        try:
            url = f"{self._get_http_url()}/api/v1/safety/status"
            async with self.http_session.get(url) as response:
                await self._handle_response(response)
                data = await response.json()
                
                return FWSafetyStatus(
                    estop_active=data['estop_active'],
                    safety_ok=data['safety_ok'],
                    interlock_active=data['interlock_active'],
                    fault_detected=data['fault_detected'],
                    fault_code=data.get('fault_code'),
                    fault_message=data.get('fault_message')
                )
                
        except Exception as e:
            await self._handle_request_error("get_safety_status", e)
            raise
    
    async def send_estop(self) -> bool:
        """Send emergency stop command"""
        try:
            url = f"{self._get_http_url()}/api/v1/safety/estop"
            async with self.http_session.post(url) as response:
                await self._handle_response(response)
                return True
                
        except Exception as e:
            await self._handle_request_error("send_estop", e)
            raise
    
    async def get_config(self) -> Dict:
        """Get FW configuration"""
        try:
            url = f"{self._get_http_url()}/api/v1/config"
            async with self.http_session.get(url) as response:
                await self._handle_response(response)
                return await response.json()
                
        except Exception as e:
            await self._handle_request_error("get_config", e)
            raise
    
    async def update_config(self, config: Dict) -> bool:
        """Update FW configuration"""
        try:
            url = f"{self._get_http_url()}/api/v1/config"
            async with self.http_session.put(url, json=config) as response:
                await self._handle_response(response)
                return True
                
        except Exception as e:
            await self._handle_request_error("update_config", e)
            raise
    
    async def get_diagnostics(self) -> Dict:
        """Get FW diagnostics"""
        try:
            url = f"{self._get_http_url()}/api/v1/diagnostics"
            async with self.http_session.get(url) as response:
                await self._handle_response(response)
                return await response.json()
                
        except Exception as e:
            await self._handle_request_error("get_diagnostics", e)
            raise
    
    # WebSocket Methods
    
    async def send_ws_message(self, message_type: FWMessageType, data: Dict = None) -> bool:
        """Send WebSocket message"""
        try:
            if not self.ws_connection:
                raise FWConnectionError("WebSocket not connected")
            
            message = {
                'type': message_type.value,
                'timestamp': int(time.time() * 1000),
                'data': data or {}
            }
            
            await self.ws_connection.send(json.dumps(message))
            self._stats['requests_sent'] += 1
            self._stats['bytes_sent'] += len(json.dumps(message))
            self._stats['last_activity'] = time.time()
            
            return True
            
        except Exception as e:
            logger.error(f"Error sending WebSocket message: {e}")
            return False
    
    def register_message_handler(self, message_type: FWMessageType, handler: Callable):
        """Register message handler"""
        self.message_handlers[message_type.value] = handler
        logger.info(f"Registered handler for message type: {message_type.value}")
    
    def register_event_handler(self, event_type: str, handler: Callable):
        """Register event handler"""
        self.event_handlers[event_type] = handler
        logger.info(f"Registered event handler for: {event_type}")
    
    # Utility Methods
    
    def get_stats(self) -> Dict:
        """Get client statistics"""
        return {
            **self._stats,
            'state': self.state.value,
            'connected': self.state == FWConnectionState.CONNECTED
        }
    
    def is_connected(self) -> bool:
        """Check if client is connected"""
        return self.state == FWConnectionState.CONNECTED
    
    async def ping(self) -> bool:
        """Ping FW server"""
        try:
            url = f"{self._get_http_url()}/api/v1/system/health"
            async with self.http_session.get(url) as response:
                return response.status == 200
        except:
            return False
    
    # Private Methods
    
    def _get_http_url(self) -> str:
        """Get HTTP URL"""
        protocol = "https" if self.config.enable_ssl else "http"
        return f"{protocol}://{self.config.host}:{self.config.http_port}"
    
    def _get_ws_url(self) -> str:
        """Get WebSocket URL"""
        protocol = "wss" if self.config.enable_ssl else "ws"
        return f"{protocol}://{self.config.host}:{self.config.ws_port}"
    
    def _get_auth_headers(self) -> Dict[str, str]:
        """Get authentication headers"""
        headers = {'Content-Type': 'application/json'}
        if self.config.auth_token:
            headers['Authorization'] = f"Bearer {self.config.auth_token}"
        return headers
    
    def _create_ssl_context(self) -> ssl.SSLContext:
        """Create SSL context"""
        ssl_context = ssl.create_default_context()
        if not self.config.verify_ssl:
            ssl_context.check_hostname = False
            ssl_context.verify_mode = ssl.CERT_NONE
        return ssl_context
    
    async def _handle_response(self, response: aiohttp.ClientResponse):
        """Handle HTTP response"""
        if response.status >= 400:
            error_text = await response.text()
            raise FWClientError(f"HTTP {response.status}: {error_text}")
    
    async def _handle_request_error(self, operation: str, error: Exception):
        """Handle request error"""
        self._stats['requests_failed'] += 1
        logger.error(f"Request failed ({operation}): {error}")
        
        if isinstance(error, asyncio.TimeoutError):
            raise FWTimeoutError(f"Request timeout: {operation}")
        elif isinstance(error, aiohttp.ClientError):
            raise FWConnectionError(f"Connection error: {operation}")
        else:
            raise FWClientError(f"Request error: {operation}")

# Example usage
async def main():
    """Example usage of FW Client"""
    
    # Create configuration
    config = FWConfig(
        host="192.168.1.100",
        http_port=8080,
        ws_port=8081,
        timeout=30,
        auth_token="your-auth-token"
    )
    
    # Create client
    client = FWClient(config)
    
    try:
        # Connect to FW
        if await client.connect():
            print("Connected to FW")
            
            # Get system status
            status = await client.get_system_status()
            print(f"System: {status.system_name} v{status.version}")
            print(f"Status: {status.status}")
            print(f"Active modules: {status.active_modules}")
            
            # Get modules list
            modules = await client.get_modules_list()
            print(f"Found {len(modules)} modules")
            
            # Get safety status
            safety = await client.get_safety_status()
            print(f"Safety OK: {safety.safety_ok}")
            print(f"E-Stop Active: {safety.estop_active}")
            
            # Send WebSocket message
            await client.send_ws_message(FWMessageType.TELEMETRY, {
                'request': 'position_data'
            })
            
            # Wait for messages
            await asyncio.sleep(10)
            
    except Exception as e:
        print(f"Error: {e}")
    
    finally:
        await client.disconnect()

if __name__ == "__main__":
    asyncio.run(main())
