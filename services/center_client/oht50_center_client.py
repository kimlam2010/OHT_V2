#!/usr/bin/env python3
"""
OHT-50 Center Communication Service
Connects to Center via WebSocket, sends heartbeat, receives commands, publishes telemetry
"""

import asyncio
import json
import logging
import signal
import sys
import time
import yaml
from datetime import datetime
from typing import Dict, Any, Optional

try:
    import websockets
except ImportError:
    print("Error: websockets not installed. Run: pip install websockets")
    sys.exit(1)

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(message)s')
logger = logging.getLogger(__name__)


def log_json(event: str, **fields: Any) -> None:
    """Emit structured JSON log for journalctl-friendly parsing."""
    payload = {
        "ts": datetime.utcnow().isoformat() + "Z",
        "service": "center_client",
        "event": event,
        **fields,
    }
    # Use logger to keep consistent sinks (journald, files)
    logger.info(json.dumps(payload))

class OHT50CenterClient:
    def __init__(self, config_path: str = "/opt/oht50/config/system.yaml"):
        self.config_path = config_path
        self.config = self.load_config()
        self.websocket = None
        self.running = False
        # Intervals/backoff with config overrides
        firmware_cfg = self.config.get('firmware', {}) if isinstance(self.config, dict) else {}
        center_cfg = self.config.get('center', {}) if isinstance(self.config, dict) else {}
        self.heartbeat_interval = int(firmware_cfg.get('heartbeat_interval', 30))
        self.telemetry_interval = int(firmware_cfg.get('telemetry_interval', 60))
        self.reconnect_delay = int(center_cfg.get('reconnect_initial', 5))  # seconds
        self.max_reconnect_delay = int(center_cfg.get('reconnect_max', 300))  # seconds
        self.current_reconnect_delay = self.reconnect_delay
        device_cfg = self.config.get('device', {}) if isinstance(self.config, dict) else {}
        self.device_id = device_cfg.get('id', self.config.get('device_id', 'OHT50-001'))
        
    def load_config(self) -> Dict[str, Any]:
        """Load configuration from YAML file"""
        try:
            with open(self.config_path, 'r') as f:
                config = yaml.safe_load(f)
                logger.info(f"Loaded config from {self.config_path}")
                return config
        except FileNotFoundError:
            logger.warning(f"Config file {self.config_path} not found, using defaults")
            return {
                'center': {
                    'host': 'localhost',
                    'port': 8080,
                    'path': '/ws/oht50'
                },
                'device_id': 'OHT50-001',
                'heartbeat_interval': 30
            }
        except Exception as e:
            logger.error(f"Error loading config: {e}")
            sys.exit(1)
    
    async def connect_to_center(self) -> bool:
        """Connect to Center via WebSocket"""
        try:
            center_config = self.config.get('center', {})
            host = center_config.get('host', 'localhost')
            port = center_config.get('port', 8080)
            path = center_config.get('path', '/ws/oht50')
            
            uri = f"ws://{host}:{port}{path}"
            log_json("center_connect_attempt", uri=uri)
            
            self.websocket = await websockets.connect(uri)
            log_json("center_connected", uri=uri)
            
            # Send initial registration
            await self.send_registration()
            
            return True
            
        except Exception as e:
            logger.error(f"Failed to connect to Center: {e}")
            return False
    
    async def send_registration(self):
        """Send device registration to Center"""
        registration = {
            "type": "registration",
            "device_id": self.device_id,
            "timestamp": datetime.utcnow().isoformat(),
            "capabilities": {
                "modules": ["power", "motor", "io", "dock"],
                "protocols": ["modbus_rtu", "rs485"],
                "features": ["estop", "led_status", "safety_monitoring"]
            }
        }
        await self.send_message(registration)
        log_json("registration_sent")
    
    async def send_heartbeat(self):
        """Send heartbeat to Center"""
        heartbeat = {
            "type": "heartbeat",
            "device_id": self.device_id,
            "timestamp": datetime.utcnow().isoformat(),
            "status": {
                "state": "IDLE",  # TODO: Get from firmware
                "safety": "NORMAL",  # TODO: Get from firmware
                "modules_online": 4,  # TODO: Get from firmware
                "uptime": int(time.time())
            }
        }
        await self.send_message(heartbeat)
        log_json("heartbeat_sent")
    
    async def send_telemetry(self):
        """Send telemetry data to Center"""
        telemetry = {
            "type": "telemetry",
            "device_id": self.device_id,
            "timestamp": datetime.utcnow().isoformat(),
            "data": {
                "power": {
                    "voltage": 24.5,  # TODO: Get from power module
                    "current": 3.2,
                    "temperature": 42.1
                },
                "modules": {
                    "0x02": {"type": "power", "status": "online"},
                    "0x03": {"type": "motor", "status": "online"},
                    "0x04": {"type": "io", "status": "online"},
                    "0x05": {"type": "dock", "status": "online"}
                }
            }
        }
        await self.send_message(telemetry)
        log_json("telemetry_sent")
    
    async def send_message(self, message: Dict[str, Any]):
        """Send message to Center"""
        if self.websocket and self.websocket.open:
            try:
                await self.websocket.send(json.dumps(message))
                log_json("ws_send", msg_type=message.get("type"))
            except Exception as e:
                log_json("ws_send_error", error=str(e))
                self.running = False
    
    async def handle_center_message(self, message: str):
        """Handle incoming message from Center"""
        try:
            data = json.loads(message)
            msg_type = data.get('type')
            
            log_json("ws_recv", msg_type=msg_type)
            
            if msg_type == "command":
                await self.handle_command(data)
            elif msg_type == "config":
                await self.handle_config(data)
            elif msg_type == "ping":
                await self.send_message({"type": "pong", "timestamp": datetime.utcnow().isoformat()})
            else:
                log_json("ws_unknown_type", msg_type=str(msg_type))
                
        except json.JSONDecodeError as e:
            log_json("ws_invalid_json", error=str(e))
        except Exception as e:
            log_json("ws_handle_error", error=str(e))
    
    async def handle_command(self, command: Dict[str, Any]):
        """Handle command from Center"""
        cmd = command.get('command')
        params = command.get('params', {})
        
        log_json("command_execute", command=str(cmd), params=params)
        
        # TODO: Send command to firmware via IPC/shared memory
        response = {
            "type": "command_response",
            "command": cmd,
            "status": "executed",
            "timestamp": datetime.utcnow().isoformat()
        }
        await self.send_message(response)
    
    async def handle_config(self, config: Dict[str, Any]):
        """Handle configuration update from Center"""
        log_json("config_update_received")
        # TODO: Apply configuration to firmware
        response = {
            "type": "config_response",
            "status": "applied",
            "timestamp": datetime.utcnow().isoformat()
        }
        await self.send_message(response)
    
    async def message_handler(self):
        """Handle incoming messages from Center"""
        try:
            async for message in self.websocket:
                await self.handle_center_message(message)
        except websockets.exceptions.ConnectionClosed:
            log_json("center_connection_closed")
            self.running = False
        except Exception as e:
            log_json("message_handler_error", error=str(e))
            self.running = False
    
    async def heartbeat_task(self):
        """Send periodic heartbeat"""
        while self.running:
            try:
                await self.send_heartbeat()
                await asyncio.sleep(self.heartbeat_interval)
            except Exception as e:
                logger.error(f"Error in heartbeat task: {e}")
                break
    
    async def telemetry_task(self):
        """Send periodic telemetry"""
        while self.running:
            try:
                await self.send_telemetry()
                await asyncio.sleep(self.telemetry_interval)
            except Exception as e:
                log_json("telemetry_task_error", error=str(e))
                break
    
    async def run(self):
        """Main run loop with reconnection logic"""
        while True:
            try:
                if await self.connect_to_center():
                    self.running = True
                    self.current_reconnect_delay = self.reconnect_delay
                    
                    # Start tasks
                    tasks = [
                        asyncio.create_task(self.message_handler()),
                        asyncio.create_task(self.heartbeat_task()),
                        asyncio.create_task(self.telemetry_task())
                    ]
                    
                    # Wait for any task to complete (or fail)
                    done, pending = await asyncio.wait(
                        tasks, 
                        return_when=asyncio.FIRST_COMPLETED
                    )
                    
                    # Cancel remaining tasks
                    for task in pending:
                        task.cancel()
                    
                    self.running = False
                    
                # Wait before reconnecting
                log_json("reconnect_wait", seconds=self.current_reconnect_delay)
                await asyncio.sleep(self.current_reconnect_delay)
                
                # Exponential backoff
                self.current_reconnect_delay = min(
                    self.current_reconnect_delay * 2, 
                    self.max_reconnect_delay
                )
                
            except KeyboardInterrupt:
                log_json("shutdown_requested")
                break
            except Exception as e:
                log_json("unexpected_error", error=str(e))
                await asyncio.sleep(self.current_reconnect_delay)
    
    async def shutdown(self):
        """Graceful shutdown"""
        self.running = False
        if self.websocket:
            await self.websocket.close()

def signal_handler(signum, frame):
    """Handle shutdown signals"""
    log_json("signal_received", signum=int(signum))
    sys.exit(0)

async def main():
    """Main entry point"""
    # Set up signal handlers
    signal.signal(signal.SIGINT, signal_handler)
    signal.signal(signal.SIGTERM, signal_handler)
    
    # Create and run client
    client = OHT50CenterClient()
    
    try:
        await client.run()
    except KeyboardInterrupt:
        logger.info("Shutdown requested")
    finally:
        await client.shutdown()

if __name__ == "__main__":
    asyncio.run(main())
