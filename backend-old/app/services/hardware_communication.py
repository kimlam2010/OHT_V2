"""
Hardware Communication Service cho OHT-50 Backend
Giao tiếp với Firmware qua Socket/IPC và WebSocket
"""

import asyncio
import json
import structlog
import websockets
from datetime import datetime
from typing import Dict, List, Optional, Any, Callable
from uuid import UUID, uuid4
import socket
import threading
import time

from app.services.log_service import LogService

logger = structlog.get_logger(__name__)


class HardwareCommunicationService:
    """Service giao tiếp với Hardware/Firmware"""
    
    def __init__(self, log_service: LogService):
        self.log_service = log_service
        self.initialized = False
        
        # Communication settings
        self.firmware_host = "127.0.0.1"
        self.firmware_port = 8080
        self.websocket_port = 8081
        
        # Connection state
        self.firmware_connected = False
        self.websocket_connected = False
        self.socket_connection = None
        self.websocket_server = None
        
        # Event handlers
        self.event_handlers: Dict[str, List[Callable]] = {
            "hardware_status": [],
            "estop_triggered": [],
            "led_state_changed": [],
            "relay_state_changed": [],
            "hardware_error": []
        }
        
        # Command queue
        self.command_queue = asyncio.Queue()
        self.response_queue = asyncio.Queue()
        
        # Threading
        self.communication_thread = None
        self.running = False
        
    async def initialize(self) -> bool:
        """Khởi tạo hardware communication"""
        try:
            logger.info("Initializing Hardware Communication Service")
            
            # Start communication thread
            self.running = True
            self.communication_thread = threading.Thread(
                target=self._communication_loop,
                daemon=True
            )
            self.communication_thread.start()
            
            # Start WebSocket server
            await self._start_websocket_server()
            
            # Connect to firmware
            await self._connect_to_firmware()
            
            self.initialized = True
            logger.info("Hardware Communication Service initialized successfully")
            return True
            
        except Exception as e:
            logger.error(f"Failed to initialize Hardware Communication Service: {e}")
            return False
    
    async def shutdown(self):
        """Shutdown hardware communication"""
        try:
            logger.info("Shutting down Hardware Communication Service")
            self.running = False
            
            # Stop communication thread
            if self.communication_thread:
                self.communication_thread.join(timeout=5)
            
            # Close connections
            if self.socket_connection:
                self.socket_connection.close()
            
            if self.websocket_server:
                self.websocket_server.close()
            
            self.initialized = False
            logger.info("Hardware Communication Service shutdown complete")
            
        except Exception as e:
            logger.error(f"Error during shutdown: {e}")
    
    async def _connect_to_firmware(self) -> bool:
        """Kết nối tới firmware qua socket"""
        try:
            self.socket_connection = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket_connection.connect((self.firmware_host, self.firmware_port))
            self.firmware_connected = True
            logger.info(f"Connected to firmware at {self.firmware_host}:{self.firmware_port}")
            return True
        except Exception as e:
            logger.error(f"Failed to connect to firmware: {e}")
            return False
    
    async def _start_websocket_server(self):
        """Khởi động WebSocket server cho real-time updates"""
        try:
            self.websocket_server = await websockets.serve(
                self._websocket_handler,
                "localhost",
                self.websocket_port
            )
            logger.info(f"WebSocket server started on port {self.websocket_port}")
        except Exception as e:
            logger.error(f"Failed to start WebSocket server: {e}")
    
    async def _websocket_handler(self, websocket, path):
        """Xử lý WebSocket connections"""
        try:
            async for message in websocket:
                await self._handle_websocket_message(message)
        except websockets.exceptions.ConnectionClosed:
            pass
        except Exception as e:
            logger.error(f"WebSocket error: {e}")
    
    async def _handle_websocket_message(self, message: str):
        """Xử lý WebSocket message"""
        try:
            data = json.loads(message)
            event_type = data.get("type")
            
            if event_type in self.event_handlers:
                for handler in self.event_handlers[event_type]:
                    await handler(data)
            else:
                logger.warning(f"Unknown event type: {event_type}")
                
        except json.JSONDecodeError as e:
            logger.error(f"Invalid JSON in WebSocket message: {e}")
        except Exception as e:
            logger.error(f"Error handling WebSocket message: {e}")
    
    def _communication_loop(self):
        """Communication loop trong thread riêng"""
        while self.running:
            try:
                # Send commands to firmware
                if not self.command_queue.empty():
                    command = self.command_queue.get_nowait()
                    self._send_command_to_firmware(command)
                
                # Receive responses from firmware
                response = self._receive_response_from_firmware()
                if response:
                    asyncio.run(self.response_queue.put(response))
                
                time.sleep(0.01)  # 10ms delay
                
            except Exception as e:
                logger.error(f"Communication loop error: {e}")
                time.sleep(1)  # Wait before retry
    
    def _send_command_to_firmware(self, command: Dict[str, Any]):
        """Gửi command tới firmware"""
        try:
            if self.socket_connection and self.firmware_connected:
                message = json.dumps(command) + "\n"
                self.socket_connection.send(message.encode())
                logger.debug(f"Sent command to firmware: {command}")
        except Exception as e:
            logger.error(f"Failed to send command to firmware: {e}")
            self.firmware_connected = False
    
    def _receive_response_from_firmware(self) -> Optional[Dict[str, Any]]:
        """Nhận response từ firmware"""
        try:
            if self.socket_connection and self.firmware_connected:
                self.socket_connection.settimeout(0.1)  # 100ms timeout
                data = self.socket_connection.recv(1024)
                if data:
                    response = json.loads(data.decode().strip())
                    logger.debug(f"Received response from firmware: {response}")
                    return response
        except socket.timeout:
            pass
        except Exception as e:
            logger.error(f"Failed to receive response from firmware: {e}")
            self.firmware_connected = False
        return None
    
    async def send_hardware_command(self, command_type: str, parameters: Dict[str, Any]) -> Dict[str, Any]:
        """Gửi hardware command và nhận response"""
        try:
            command = {
                "id": str(uuid4()),
                "type": command_type,
                "parameters": parameters,
                "timestamp": datetime.utcnow().isoformat()
            }
            
            await self.command_queue.put(command)
            
            # Wait for response
            timeout = 5.0  # 5 seconds timeout
            start_time = time.time()
            
            while time.time() - start_time < timeout:
                if not self.response_queue.empty():
                    response = await self.response_queue.get()
                    if response.get("id") == command["id"]:
                        return response
                await asyncio.sleep(0.01)
            
            raise TimeoutError("Command timeout")
            
        except Exception as e:
            logger.error(f"Failed to send hardware command: {e}")
            return {"error": str(e)}
    
    def register_event_handler(self, event_type: str, handler: Callable):
        """Đăng ký event handler"""
        if event_type in self.event_handlers:
            self.event_handlers[event_type].append(handler)
            logger.info(f"Registered handler for event: {event_type}")
        else:
            logger.warning(f"Unknown event type: {event_type}")
    
    def unregister_event_handler(self, event_type: str, handler: Callable):
        """Hủy đăng ký event handler"""
        if event_type in self.event_handlers:
            if handler in self.event_handlers[event_type]:
                self.event_handlers[event_type].remove(handler)
                logger.info(f"Unregistered handler for event: {event_type}")
    
    async def get_connection_status(self) -> Dict[str, Any]:
        """Lấy trạng thái kết nối"""
        return {
            "firmware_connected": self.firmware_connected,
            "websocket_connected": self.websocket_connected,
            "initialized": self.initialized,
            "command_queue_size": self.command_queue.qsize(),
            "response_queue_size": self.response_queue.qsize()
        }
    
    # Hardware command methods
    async def set_led_state(self, led_id: str, state: bool) -> Dict[str, Any]:
        """Set LED state"""
        return await self.send_hardware_command("set_led", {
            "led_id": led_id,
            "state": state
        })
    
    async def get_estop_status(self) -> Dict[str, Any]:
        """Get E-Stop status"""
        return await self.send_hardware_command("get_estop", {})
    
    async def set_relay_state(self, relay_id: str, state: bool) -> Dict[str, Any]:
        """Set relay state"""
        return await self.send_hardware_command("set_relay", {
            "relay_id": relay_id,
            "state": state
        })
    
    async def get_hardware_status(self) -> Dict[str, Any]:
        """Get overall hardware status"""
        return await self.send_hardware_command("get_status", {})
