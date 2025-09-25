"""
FW Client Library - OHT-50 Firmware Communication

This module provides a comprehensive client library for communicating
with the OHT-50 Firmware via HTTP REST API and WebSocket.

Features:
- HTTP REST API client
- WebSocket real-time communication
- Connection management and health monitoring
- Authentication support
- Error handling and recovery
- Async/await pattern for non-blocking operations
"""

import asyncio
import json
import logging
import time
from typing import Dict, Any, Optional, AsyncGenerator, List
from dataclasses import dataclass
from datetime import datetime, timezone

import aiohttp
import websockets
from websockets.exceptions import ConnectionClosed, WebSocketException

logger = logging.getLogger(__name__)


@dataclass
class FWConfig:
    """Firmware client configuration"""
    host: str = "localhost"
    http_port: int = 8080
    ws_port: int = 8081
    auth_token: Optional[str] = None
    timeout: float = 10.0
    max_retries: int = 3
    retry_delay: float = 1.0


@dataclass
class FWConnectionStatus:
    """Firmware connection status"""
    connected: bool = False
    last_heartbeat: Optional[datetime] = None
    error_count: int = 0
    response_time_ms: float = 0.0


class FWClientError(Exception):
    """Base exception for FW client errors"""
    pass


class FWConnectionError(FWClientError):
    """Connection-related errors"""
    pass


class FWProtocolError(FWClientError):
    """Protocol-related errors"""
    pass


class FWClient:
    """
    Firmware client for HTTP and WebSocket communication
    
    This client provides a unified interface for communicating with
    the OHT-50 Firmware via both HTTP REST API and WebSocket.
    """
    
    def __init__(self, config: FWConfig):
        """
        Initialize firmware client
        
        Args:
            config: Firmware client configuration
        """
        self.config = config
        self.http_session: Optional[aiohttp.ClientSession] = None
        self.ws_connection: Optional[websockets.WebSocketServerProtocol] = None
        self.status = FWConnectionStatus()
        
        # Connection URLs
        self.http_url = f"http://{config.host}:{config.http_port}"
        self.ws_url = f"ws://{config.host}:{config.ws_port}/ws"
        
        # Authentication headers
        self.headers = {
            "Content-Type": "application/json",
            "User-Agent": "OHT-50-Backend/1.0"
        }
        if config.auth_token:
            self.headers["Authorization"] = f"Bearer {config.auth_token}"
        
        logger.info(f"🔌 FW Client initialized: HTTP={self.http_url}, WS={self.ws_url}")
    
    async def __aenter__(self):
        """Async context manager entry"""
        await self.connect()
        return self
    
    async def __aexit__(self, exc_type, exc_val, exc_tb):
        """Async context manager exit"""
        await self.disconnect()
    
    async def connect(self) -> bool:
        """
        Establish connection to firmware
        
        Returns:
            True if connection successful, False otherwise
        """
        try:
            logger.info("🔌 Connecting to firmware...")
            
            # Create HTTP session
            timeout = aiohttp.ClientTimeout(total=self.config.timeout)
            self.http_session = aiohttp.ClientSession(
                base_url=self.http_url,
                headers=self.headers,
                timeout=timeout
            )
            
            # Test HTTP connection
            health_status = await self.get_system_health()
            if not health_status:
                logger.error("❌ HTTP connection test failed")
                return False
            
            # Connect WebSocket
            await self._connect_websocket()
            
            self.status.connected = True
            self.status.last_heartbeat = datetime.now(timezone.utc)
            
            logger.info("✅ Firmware connection established")
            return True
            
        except Exception as e:
            logger.error(f"❌ Failed to connect to firmware: {e}")
            self.status.error_count += 1
            return False
    
    async def disconnect(self) -> None:
        """Close connection to firmware"""
        logger.info("🔌 Disconnecting from firmware...")
        
        # Close WebSocket
        if self.ws_connection:
            try:
                await self.ws_connection.close()
            except Exception as e:
                logger.warning(f"Warning closing WebSocket: {e}")
            finally:
                self.ws_connection = None
        
        # Close HTTP session
        if self.http_session:
            try:
                await self.http_session.close()
                # Wait for session to fully close
                await asyncio.sleep(0.1)
            except Exception as e:
                logger.warning(f"Warning closing HTTP session: {e}")
            finally:
                self.http_session = None
        
        self.status.connected = False
        logger.info("✅ Firmware connection closed")
    
    async def _connect_websocket(self) -> None:
        """Connect to WebSocket server"""
        try:
            # Add auth token to WebSocket URL if provided
            ws_url = self.ws_url
            if self.config.auth_token:
                ws_url += f"?token={self.config.auth_token}"
            
            self.ws_connection = await websockets.connect(
                ws_url,
                ping_interval=30,
                ping_timeout=10
            )
            
            logger.info("✅ WebSocket connection established")
            
        except Exception as e:
            logger.warning(f"⚠️ WebSocket connection failed: {e}")
            # WebSocket failure is not critical for basic functionality
    
    # HTTP API Methods
    
    async def get(self, endpoint: str, params: Optional[Dict] = None) -> Dict[str, Any]:
        """
        HTTP GET request
        
        Args:
            endpoint: API endpoint
            params: Query parameters
            
        Returns:
            Response data
            
        Raises:
            FWConnectionError: Connection failed
            FWProtocolError: Protocol error
        """
        if not self.http_session:
            raise FWConnectionError("HTTP session not initialized")
        
        try:
            start_time = time.time()
            
            async with self.http_session.get(endpoint, params=params) as response:
                response_time = (time.time() - start_time) * 1000
                self.status.response_time_ms = response_time
                
                if response.status == 200:
                    data = await response.json()
                    logger.debug(f"✅ GET {endpoint}: {response_time:.1f}ms")
                    return data
                else:
                    error_text = await response.text()
                    raise FWProtocolError(f"HTTP {response.status}: {error_text}")
                    
        except aiohttp.ClientError as e:
            raise FWConnectionError(f"HTTP request failed: {e}")
        except json.JSONDecodeError as e:
            raise FWProtocolError(f"Invalid JSON response: {e}")
    
    async def post(self, endpoint: str, data: Dict[str, Any]) -> Dict[str, Any]:
        """
        HTTP POST request
        
        Args:
            endpoint: API endpoint
            data: Request data
            
        Returns:
            Response data
            
        Raises:
            FWConnectionError: Connection failed
            FWProtocolError: Protocol error
        """
        if not self.http_session:
            raise FWConnectionError("HTTP session not initialized")
        
        try:
            start_time = time.time()
            
            async with self.http_session.post(endpoint, json=data) as response:
                response_time = (time.time() - start_time) * 1000
                self.status.response_time_ms = response_time
                
                if response.status in [200, 201]:
                    response_data = await response.json()
                    logger.debug(f"✅ POST {endpoint}: {response_time:.1f}ms")
                    return response_data
                else:
                    error_text = await response.text()
                    raise FWProtocolError(f"HTTP {response.status}: {error_text}")
                    
        except aiohttp.ClientError as e:
            raise FWConnectionError(f"HTTP request failed: {e}")
        except json.JSONDecodeError as e:
            raise FWProtocolError(f"Invalid JSON response: {e}")
    
    # System API Methods
    
    async def get_system_health(self) -> Dict[str, Any]:
        """Get system health status"""
        try:
            return await self.get("/health")
        except Exception as e:
            logger.error(f"❌ Failed to get system health: {e}")
            return {"success": False, "error": str(e)}
    
    async def get_system_status(self) -> Dict[str, Any]:
        """Get detailed system status"""
        try:
            return await self.get("/api/v1/system/status")
        except Exception as e:
            logger.error(f"❌ Failed to get system status: {e}")
            return {"success": False, "error": str(e)}
    
    # Module API Methods
    
    async def get_modules(self) -> List[Dict[str, Any]]:
        """Get list of modules"""
        try:
            response = await self.get("/api/v1/modules")
            return response.get("data", [])
        except Exception as e:
            logger.error(f"❌ Failed to get modules: {e}")
            return []
    
    async def get_module_info(self, module_id: int) -> Dict[str, Any]:
        """Get module information"""
        try:
            return await self.get(f"/api/v1/modules/{module_id}")
        except Exception as e:
            logger.error(f"❌ Failed to get module {module_id} info: {e}")
            return {"success": False, "error": str(e)}
    
    async def send_module_command(self, module_id: int, command: str, parameters: Dict[str, Any]) -> bool:
        """Send command to module"""
        try:
            data = {
                "command": command,
                "parameters": parameters,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
            
            response = await self.post(f"/api/v1/modules/{module_id}/cmd", data)
            success = response.get("success", False)
            
            if success:
                logger.info(f"✅ Module {module_id} command '{command}' sent successfully")
            else:
                logger.warning(f"⚠️ Module {module_id} command '{command}' failed")
            
            return success
            
        except Exception as e:
            logger.error(f"❌ Failed to send command to module {module_id}: {e}")
            return False
    
    # Safety API Methods
    
    async def get_safety_status(self) -> Dict[str, Any]:
        """Get safety status"""
        try:
            return await self.get("/api/v1/safety/status")
        except Exception as e:
            logger.error(f"❌ Failed to get safety status: {e}")
            return {"success": False, "error": str(e)}
    
    async def emergency_stop(self) -> bool:
        """Trigger emergency stop"""
        try:
            data = {
                "command": "emergency_stop",
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
            
            response = await self.post("/api/v1/safety/estop", data)
            success = response.get("success", False)
            
            if success:
                logger.warning("🚨 Emergency stop triggered successfully")
            else:
                logger.error("❌ Emergency stop failed")
            
            return success
            
        except Exception as e:
            logger.error(f"❌ Failed to trigger emergency stop: {e}")
            return False
    
    # Configuration API Methods
    
    async def get_configuration(self) -> Dict[str, Any]:
        """Get current configuration"""
        try:
            return await self.get("/api/v1/config")
        except Exception as e:
            logger.error(f"❌ Failed to get configuration: {e}")
            return {"success": False, "error": str(e)}
    
    async def update_configuration(self, config: Dict[str, Any]) -> bool:
        """Update configuration"""
        try:
            response = await self.post("/api/v1/config", config)
            success = response.get("success", False)
            
            if success:
                logger.info("✅ Configuration updated successfully")
            else:
                logger.warning("⚠️ Configuration update failed")
            
            return success
            
        except Exception as e:
            logger.error(f"❌ Failed to update configuration: {e}")
            return False
    
    # Diagnostics API Methods
    
    async def get_diagnostics(self) -> Dict[str, Any]:
        """Get system diagnostics"""
        try:
            return await self.get("/api/v1/diagnostics")
        except Exception as e:
            logger.error(f"❌ Failed to get diagnostics: {e}")
            return {"success": False, "error": str(e)}
    
    # WebSocket Methods
    
    async def websocket_send(self, message: Dict[str, Any]) -> None:
        """
        Send WebSocket message
        
        Args:
            message: Message to send
            
        Raises:
            FWConnectionError: WebSocket not connected
        """
        if not self.ws_connection:
            raise FWConnectionError("WebSocket not connected")
        
        try:
            message_json = json.dumps(message)
            await self.ws_connection.send(message_json)
            logger.debug(f"📤 WebSocket message sent: {message['type']}")
            
        except ConnectionClosed:
            raise FWConnectionError("WebSocket connection closed")
        except WebSocketException as e:
            raise FWConnectionError(f"WebSocket error: {e}")
    
    async def websocket_receive(self) -> AsyncGenerator[Dict[str, Any], None]:
        """
        Receive WebSocket messages
        
        Yields:
            Received messages
            
        Raises:
            FWConnectionError: WebSocket not connected
        """
        if not self.ws_connection:
            raise FWConnectionError("WebSocket not connected")
        
        try:
            async for message in self.ws_connection:
                try:
                    data = json.loads(message)
                    logger.debug(f"📥 WebSocket message received: {data.get('type', 'unknown')}")
                    yield data
                    
                except json.JSONDecodeError as e:
                    logger.warning(f"⚠️ Invalid JSON in WebSocket message: {e}")
                    continue
                    
        except ConnectionClosed:
            logger.warning("⚠️ WebSocket connection closed")
        except WebSocketException as e:
            logger.error(f"❌ WebSocket error: {e}")
    
    async def get_telemetry_stream(self) -> AsyncGenerator[Dict[str, Any], None]:
        """
        Get real-time telemetry stream
        
        Yields:
            Telemetry data messages
        """
        async for message in self.websocket_receive():
            if message.get("type") == "telemetry":
                yield message.get("data", {})
    
    # Health Monitoring
    
    async def check_connection_health(self) -> bool:
        """Check if connection is healthy"""
        try:
            # Test HTTP connection
            health = await self.get_system_health()
            http_ok = health.get("success", False)
            
            # Test WebSocket connection
            ws_ok = self.ws_connection is not None and not self.ws_connection.closed
            
            is_healthy = http_ok and ws_ok
            
            if is_healthy:
                self.status.last_heartbeat = datetime.now(timezone.utc)
                self.status.error_count = 0
            else:
                self.status.error_count += 1
            
            return is_healthy
            
        except Exception as e:
            logger.error(f"❌ Health check failed: {e}")
            self.status.error_count += 1
            return False
    
    def get_connection_status(self) -> FWConnectionStatus:
        """Get current connection status"""
        return self.status


# Factory function for easy client creation
def create_fw_client(
    host: str = "localhost",
    http_port: int = 8080,
    ws_port: int = 8081,
    auth_token: Optional[str] = None
) -> FWClient:
    """
    Create firmware client with default configuration
    
    Args:
        host: Firmware host address
        http_port: HTTP API port
        ws_port: WebSocket port
        auth_token: Authentication token
        
    Returns:
        Configured FW client
    """
    config = FWConfig(
        host=host,
        http_port=http_port,
        ws_port=ws_port,
        auth_token=auth_token
    )
    
    return FWClient(config)


# Example usage
if __name__ == "__main__":
    async def main():
        """Example usage of FW client"""
        client = create_fw_client()
        
        try:
            # Connect to firmware
            connected = await client.connect()
            if not connected:
                print("❌ Failed to connect to firmware")
                return
            
            # Get system status
            status = await client.get_system_status()
            print(f"System Status: {status}")
            
            # Get modules
            modules = await client.get_modules()
            print(f"Modules: {len(modules)} found")
            
            # Listen to telemetry
            print("Listening to telemetry...")
            async for telemetry in client.get_telemetry_stream():
                print(f"Telemetry: {telemetry}")
                break  # Just one message for example
            
        except Exception as e:
            print(f"❌ Error: {e}")
        finally:
            await client.disconnect()
    
    asyncio.run(main())
