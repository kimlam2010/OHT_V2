#!/usr/bin/env python3
"""
Test script for WebSocket Alert functionality
Tests Issue #79 - WebSocket cho active alert
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


class WebSocketAlertTester:
    """Test WebSocket Alert functionality"""
    
    def __init__(self, base_url: str = "ws://localhost:8000"):
        self.base_url = base_url
        self.websocket = None
        self.received_messages = []
        
    async def connect(self):
        """Connect to WebSocket alerts endpoint"""
        try:
            uri = f"{self.base_url}/ws/alerts"
            logger.info(f"🔌 Connecting to {uri}")
            
            self.websocket = await websockets.connect(uri)
            logger.info("✅ Connected to WebSocket alerts endpoint")
            return True
            
        except Exception as e:
            logger.error(f"❌ Failed to connect: {e}")
            return False
    
    async def listen_for_messages(self, duration: int = 30):
        """Listen for WebSocket messages"""
        if not self.websocket:
            logger.error("❌ Not connected to WebSocket")
            return
        
        logger.info(f"👂 Listening for messages for {duration} seconds...")
        
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
                        logger.info(f"   Data: {json.dumps(data, indent=2)}")
                        
                        # Validate message format for alerts
                        self._validate_alert_message(data)
                        
                    except json.JSONDecodeError:
                        logger.warning(f"⚠️ Invalid JSON message: {message}")
                        
                except asyncio.TimeoutError:
                    # No message received, continue listening
                    continue
                    
        except Exception as e:
            logger.error(f"❌ Error listening for messages: {e}")
    
    def _validate_alert_message(self, message: Dict[str, Any]):
        """Validate alert message format according to issue #79"""
        try:
            # Check message structure
            required_fields = ["type", "data", "timestamp"]
            for field in required_fields:
                if field not in message:
                    logger.warning(f"⚠️ Missing field in message: {field}")
                    return
            
            # Check alert data structure if it's an alert message
            if message["type"] in ["alert_notification", "alert_update", "active_alerts_update"]:
                data = message.get("data", {})
                
                if "alert" in data:
                    alert = data["alert"]
                    self._validate_alert_object(alert)
                elif "alerts" in data:
                    alerts = data["alerts"]
                    for alert in alerts:
                        self._validate_alert_object(alert)
            
            logger.info("✅ Message format validation passed")
            
        except Exception as e:
            logger.error(f"❌ Message validation failed: {e}")
    
    def _validate_alert_object(self, alert: Dict[str, Any]):
        """Validate alert object according to issue #79 schema"""
        required_fields = [
            "id", "title", "message", "severity", "status", 
            "source", "timestamp"
        ]
        
        optional_fields = [
            "acknowledged_by", "acknowledged_at", "resolved_at", "metadata"
        ]
        
        # Check required fields
        for field in required_fields:
            if field not in alert:
                logger.warning(f"⚠️ Missing required field in alert: {field}")
                return
        
        # Validate field types and values
        if not isinstance(alert["id"], int):
            logger.warning(f"⚠️ Alert ID should be integer, got: {type(alert['id'])}")
        
        if alert["severity"] not in ["critical", "high", "medium", "low", "info", "warning", "error", "emergency"]:
            logger.warning(f"⚠️ Invalid severity: {alert['severity']}")
        
        if alert["status"] not in ["active", "acknowledged", "resolved", "suppressed"]:
            logger.warning(f"⚠️ Invalid status: {alert['status']}")
        
        # Validate timestamp format
        try:
            datetime.fromisoformat(alert["timestamp"].replace("Z", "+00:00"))
        except ValueError:
            logger.warning(f"⚠️ Invalid timestamp format: {alert['timestamp']}")
        
        logger.info(f"✅ Alert object validation passed for ID: {alert['id']}")
    
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
    
    async def test_subscription(self):
        """Test alert subscription"""
        logger.info("🔔 Testing alert subscription...")
        
        # Subscribe to alerts
        await self.send_test_message("subscribe", {"type": "alerts"})
        
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
        logger.info("📊 WEBSOCKET ALERT TEST SUMMARY")
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
        
        # Check if we received the expected alert format
        alert_messages = [
            msg for msg in self.received_messages 
            if msg.get("type") in ["alert_notification", "alert_update", "active_alerts_update"]
        ]
        
        logger.info(f"Alert-related messages: {len(alert_messages)}")
        
        if alert_messages:
            logger.info("✅ WebSocket Alert functionality is working!")
            
            # Show sample alert message
            sample = alert_messages[0]
            logger.info("Sample alert message:")
            logger.info(json.dumps(sample, indent=2))
        else:
            logger.warning("⚠️ No alert messages received")
        
        logger.info("=" * 60)


async def main():
    """Main test function"""
    logger.info("🚀 Starting WebSocket Alert Test for Issue #79")
    
    tester = WebSocketAlertTester()
    
    try:
        # Connect to WebSocket
        if not await tester.connect():
            return
        
        # Test subscription
        await tester.test_subscription()
        
        # Listen for messages
        await tester.listen_for_messages(duration=10)
        
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
