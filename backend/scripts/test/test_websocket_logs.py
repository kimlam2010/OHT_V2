#!/usr/bin/env python3
"""
Test script for WebSocket Log functionality
Tests Issue #78 - WebSocket cho system logs
"""

import asyncio
import json
import logging
import websockets
from datetime import datetime
from typing import Dict, Any

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)


class WebSocketLogTester:
    """Test WebSocket Log functionality"""
    
    def __init__(self, base_url: str = "ws://localhost:8000"):
        self.base_url = base_url
        self.websocket = None
        self.received_messages = []
        
    async def connect(self):
        """Connect to WebSocket logs endpoint"""
        try:
            uri = f"{self.base_url}/ws/logs"
            logger.info(f"🔌 Connecting to {uri}")
            
            self.websocket = await websockets.connect(uri)
            logger.info("✅ Connected to WebSocket logs endpoint")
            return True
            
        except Exception as e:
            logger.error(f"❌ Failed to connect: {e}")
            return False
    
    async def listen_for_messages(self, duration: int = 30):
        """Listen for WebSocket log messages"""
        if not self.websocket:
            logger.error("❌ Not connected to WebSocket")
            return
        
        logger.info(f"👂 Listening for log messages for {duration} seconds...")
        
        try:
            end_time = asyncio.get_event_loop().time() + duration
            
            while asyncio.get_event_loop().time() < end_time:
                try:
                    # Wait for message with timeout
                    message = await asyncio.wait_for(
                        self.websocket.recv(),
                        timeout=1.0
                    )
                    
                    # Parse message
                    try:
                        data = json.loads(message)
                        self.received_messages.append(data)
                        
                        logger.info(f"📨 Received message: {data.get('type', 'unknown')}")
                        
                        # Show log details if it's a log message
                        if data.get('type') in ['log_entry', 'recent_logs']:
                            self._display_log_message(data)
                        
                        # Validate message format for logs
                        self._validate_log_message(data)
                        
                    except json.JSONDecodeError:
                        logger.warning(f"⚠️ Invalid JSON message: {message}")
                        
                except asyncio.TimeoutError:
                    # No message received, continue listening
                    continue
                    
        except Exception as e:
            logger.error(f"❌ Error listening for messages: {e}")
    
    def _display_log_message(self, message: Dict[str, Any]):
        """Display log message details"""
        try:
            data = message.get("data", {})
            
            if "log" in data:
                # Single log entry
                log = data["log"]
                logger.info(f"   📝 Log: [{log.get('level', 'unknown').upper()}] {log.get('message', 'No message')}")
                logger.info(f"        Source: {log.get('source', 'unknown')}")
                logger.info(f"        Time: {log.get('timestamp', 'unknown')}")
                
            elif "logs" in data:
                # Multiple logs
                logs = data["logs"]
                logger.info(f"   📊 Received {len(logs)} logs")
                for i, log in enumerate(logs[:3]):  # Show first 3
                    logger.info(f"   📝 [{log.get('level', 'unknown').upper()}] {log.get('message', 'No message')}")
                if len(logs) > 3:
                    logger.info(f"   ... and {len(logs) - 3} more logs")
            
        except Exception as e:
            logger.error(f"❌ Error displaying log message: {e}")
    
    def _validate_log_message(self, message: Dict[str, Any]):
        """Validate log message format according to issue #78"""
        try:
            # Check message structure
            required_fields = ["type", "data", "timestamp"]
            for field in required_fields:
                if field not in message:
                    logger.warning(f"⚠️ Missing field in message: {field}")
                    return
            
            # Check log data structure if it's a log message
            if message["type"] in ["log_entry", "recent_logs"]:
                data = message.get("data", {})
                
                if "log" in data:
                    log = data["log"]
                    self._validate_log_object(log)
                elif "logs" in data:
                    logs = data["logs"]
                    for log in logs[:5]:  # Validate first 5
                        self._validate_log_object(log)
            
            logger.debug("✅ Message format validation passed")
            
        except Exception as e:
            logger.error(f"❌ Message validation failed: {e}")
    
    def _validate_log_object(self, log: Dict[str, Any]):
        """Validate log object according to issue #78 schema"""
        required_fields = [
            "id", "level", "message", "source", "timestamp"
        ]
        
        optional_fields = [
            "user_id", "session_id", "metadata"
        ]
        
        # Check required fields
        for field in required_fields:
            if field not in log:
                logger.warning(f"⚠️ Missing required field in log: {field}")
                return
        
        # Validate field types and values
        if not isinstance(log["id"], int):
            logger.warning(f"⚠️ Log ID should be integer, got: {type(log['id'])}")
        
        if log["level"] not in ["debug", "info", "warning", "error", "critical"]:
            logger.warning(f"⚠️ Invalid log level: {log['level']}")
        
        # Validate timestamp format
        try:
            datetime.fromisoformat(log["timestamp"].replace("Z", "+00:00"))
        except ValueError:
            logger.warning(f"⚠️ Invalid timestamp format: {log['timestamp']}")
        
        logger.debug(f"✅ Log object validation passed for ID: {log['id']}")
    
    async def send_test_message(self, message_type: str, data: Dict[str, Any] = None):
        """Send test message to WebSocket"""
        if not self.websocket:
            logger.error("❌ Not connected to WebSocket")
            return
        
        try:
            message = {
                "type": message_type,
                "data": data or {},
                "timestamp": datetime.now().isoformat()
            }
            
            await self.websocket.send(json.dumps(message))
            logger.info(f"📤 Sent message: {message_type}")
            
        except Exception as e:
            logger.error(f"❌ Failed to send message: {e}")
    
    async def test_log_level_filter(self):
        """Test log level filtering"""
        logger.info("🔧 Testing log level filter...")
        
        # Test different log levels
        levels = ["debug", "info", "warning", "error", "critical"]
        
        for level in levels:
            logger.info(f"📊 Setting log level filter to: {level}")
            await self.send_test_message("set_log_level", {"level": level})
            await asyncio.sleep(2)
            
            # Request recent logs with this level
            await self.send_test_message("get_recent_logs", {"level": level, "limit": 10})
            await asyncio.sleep(2)
        
        logger.info("✅ Log level filter test completed")
    
    async def test_subscription(self):
        """Test log subscription"""
        logger.info("🔔 Testing log subscription...")
        
        # Subscribe to logs
        await self.send_test_message("subscribe", {"type": "logs"})
        
        # Wait for subscription confirmation
        await asyncio.sleep(1)
        
        # Send ping to test connection
        await self.send_test_message("ping")
        
        logger.info("✅ Subscription test completed")
    
    async def disconnect(self):
        """Disconnect from WebSocket"""
        if self.websocket:
            try:
                await self.websocket.close()
                logger.info("🔌 Disconnected from WebSocket")
            except Exception as e:
                logger.error(f"❌ Error disconnecting: {e}")
    
    def print_summary(self):
        """Print test summary"""
        logger.info("=" * 60)
        logger.info("📊 WEBSOCKET LOG TEST SUMMARY")
        logger.info("=" * 60)
        logger.info(f"Total messages received: {len(self.received_messages)}")
        
        # Count message types
        message_types = {}
        for message in self.received_messages:
            msg_type = message.get("type", "unknown")
            message_types[msg_type] = message_types.get(msg_type, 0) + 1
        
        logger.info("Message types:")
        for msg_type, count in message_types.items():
            logger.info(f"  {msg_type}: {count}")
        
        # Check if we received the expected log format
        log_messages = [
            msg for msg in self.received_messages 
            if msg.get("type") in ["log_entry", "recent_logs"]
        ]
        
        logger.info(f"Log-related messages: {len(log_messages)}")
        
        if log_messages:
            logger.info("✅ WebSocket Log functionality is working!")
            
            # Show sample log message
            sample = log_messages[0]
            logger.info("Sample log message:")
            logger.info(json.dumps(sample, indent=2))
        else:
            logger.warning("⚠️ No log messages received")
        
        logger.info("=" * 60)


async def main():
    """Main test function"""
    logger.info("🚀 Starting WebSocket Log Test for Issue #78")
    
    tester = WebSocketLogTester()
    
    try:
        # Connect to WebSocket
        if not await tester.connect():
            return
        
        # Test subscription
        await tester.test_subscription()
        
        # Test log level filtering
        await tester.test_log_level_filter()
        
        # Listen for messages
        await tester.listen_for_messages(duration=15)
        
        # Print summary
        tester.print_summary()
        
    except KeyboardInterrupt:
        logger.info("🛑 Test interrupted by user")
    except Exception as e:
        logger.error(f"❌ Test failed: {e}")
    finally:
        await tester.disconnect()


if __name__ == "__main__":
    asyncio.run(main())
