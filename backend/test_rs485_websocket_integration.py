#!/usr/bin/env python3
"""
RS485 WebSocket Telemetry Integration Test
Tests the complete flow: Firmware WebSocket → Backend → Frontend
"""

import asyncio
import json
import websockets
import time
from datetime import datetime

# Test configuration
FIRMWARE_WS_URL = "ws://127.0.0.1:8081"  # Firmware WebSocket Server
BACKEND_WS_URL = "ws://127.0.0.1:8000/ws/rs485"  # Backend WebSocket

class RS485TelemetryIntegrationTest:
    def __init__(self):
        self.firmware_ws = None
        self.backend_ws = None
        self.test_results = {
            "firmware_connected": False,
            "backend_connected": False,
            "telemetry_received_from_firmware": 0,
            "telemetry_relayed_to_frontend": 0,
            "test_duration_seconds": 0,
            "errors": []
        }
        
    async def run_integration_test(self, duration_seconds=30):
        """Run complete integration test"""
        print("🧪 Starting RS485 WebSocket Telemetry Integration Test")
        print(f"📊 Test duration: {duration_seconds} seconds")
        print(f"🔌 Firmware WebSocket: {FIRMWARE_WS_URL}")
        print(f"🖥️ Backend WebSocket: {BACKEND_WS_URL}")
        print("-" * 60)
        
        start_time = time.time()
        
        try:
            # Test 1: Connect to Firmware WebSocket
            await self._test_firmware_connection()
            
            # Test 2: Connect to Backend WebSocket  
            await self._test_backend_connection()
            
            # Test 3: Listen for telemetry data
            await self._test_telemetry_flow(duration_seconds)
            
        except Exception as e:
            self.test_results["errors"].append(f"Integration test failed: {e}")
            print(f"❌ Integration test failed: {e}")
            
        finally:
            # Cleanup connections
            await self._cleanup_connections()
            
        # Calculate test duration
        self.test_results["test_duration_seconds"] = time.time() - start_time
        
        # Print test results
        self._print_test_results()
        
    async def _test_firmware_connection(self):
        """Test connection to Firmware WebSocket Server"""
        try:
            print("🔌 Testing Firmware WebSocket connection...")
            
            self.firmware_ws = await websockets.connect(
                FIRMWARE_WS_URL,
                timeout=10.0
            )
            
            self.test_results["firmware_connected"] = True
            print("✅ Firmware WebSocket connected successfully")
            
            # Send subscription message
            subscribe_msg = {
                "type": "subscribe",
                "data": {
                    "subscription": "rs485_telemetry",
                    "client_id": "integration_test"
                }
            }
            
            await self.firmware_ws.send(json.dumps(subscribe_msg))
            print("📡 Subscribed to Firmware RS485 telemetry")
            
        except Exception as e:
            self.test_results["errors"].append(f"Firmware connection failed: {e}")
            print(f"❌ Firmware WebSocket connection failed: {e}")
            raise
            
    async def _test_backend_connection(self):
        """Test connection to Backend WebSocket"""
        try:
            print("🖥️ Testing Backend WebSocket connection...")
            
            self.backend_ws = await websockets.connect(
                BACKEND_WS_URL,
                timeout=10.0
            )
            
            self.test_results["backend_connected"] = True
            print("✅ Backend WebSocket connected successfully")
            
        except Exception as e:
            self.test_results["errors"].append(f"Backend connection failed: {e}")
            print(f"❌ Backend WebSocket connection failed: {e}")
            raise
            
    async def _test_telemetry_flow(self, duration_seconds):
        """Test telemetry data flow"""
        print(f"📊 Testing telemetry flow for {duration_seconds} seconds...")
        
        # Create tasks for listening to both connections
        firmware_task = asyncio.create_task(self._listen_firmware_telemetry())
        backend_task = asyncio.create_task(self._listen_backend_telemetry())
        
        # Wait for specified duration
        await asyncio.sleep(duration_seconds)
        
        # Cancel listening tasks
        firmware_task.cancel()
        backend_task.cancel()
        
        try:
            await firmware_task
        except asyncio.CancelledError:
            pass
            
        try:
            await backend_task
        except asyncio.CancelledError:
            pass
            
    async def _listen_firmware_telemetry(self):
        """Listen for telemetry from Firmware WebSocket"""
        try:
            async for message in self.firmware_ws:
                try:
                    data = json.loads(message)
                    message_type = data.get("type")
                    
                    if message_type == "telemetry":
                        self.test_results["telemetry_received_from_firmware"] += 1
                        module_addr = data.get("data", {}).get("module_addr", "0x00")
                        module_name = data.get("data", {}).get("module_name", "Unknown")
                        register_count = len(data.get("data", {}).get("registers", []))
                        
                        print(f"📨 Firmware telemetry: {module_name} ({module_addr}) - {register_count} registers")
                        
                except json.JSONDecodeError as e:
                    self.test_results["errors"].append(f"Invalid JSON from Firmware: {e}")
                except Exception as e:
                    self.test_results["errors"].append(f"Error processing Firmware message: {e}")
                    
        except websockets.exceptions.ConnectionClosed:
            print("🔌 Firmware WebSocket connection closed")
        except Exception as e:
            self.test_results["errors"].append(f"Firmware listening error: {e}")
            
    async def _listen_backend_telemetry(self):
        """Listen for telemetry from Backend WebSocket"""
        try:
            async for message in self.backend_ws:
                try:
                    data = json.loads(message)
                    message_type = data.get("type")
                    
                    if message_type == "rs485_telemetry_update":
                        self.test_results["telemetry_relayed_to_frontend"] += 1
                        module_addr = data.get("data", {}).get("module_addr", "0x00")
                        
                        print(f"📡 Backend relay: {module_addr} telemetry relayed to Frontend")
                        
                except json.JSONDecodeError as e:
                    self.test_results["errors"].append(f"Invalid JSON from Backend: {e}")
                except Exception as e:
                    self.test_results["errors"].append(f"Error processing Backend message: {e}")
                    
        except websockets.exceptions.ConnectionClosed:
            print("🔌 Backend WebSocket connection closed")
        except Exception as e:
            self.test_results["errors"].append(f"Backend listening error: {e}")
            
    async def _cleanup_connections(self):
        """Cleanup WebSocket connections"""
        if self.firmware_ws:
            await self.firmware_ws.close()
            print("🔌 Firmware WebSocket disconnected")
            
        if self.backend_ws:
            await self.backend_ws.close()
            print("🔌 Backend WebSocket disconnected")
            
    def _print_test_results(self):
        """Print comprehensive test results"""
        print("\n" + "=" * 60)
        print("🧪 RS485 WEBSOCKET TELEMETRY INTEGRATION TEST RESULTS")
        print("=" * 60)
        
        # Connection Results
        print("\n🔌 CONNECTION RESULTS:")
        print(f"   Firmware WebSocket: {'✅ Connected' if self.test_results['firmware_connected'] else '❌ Failed'}")
        print(f"   Backend WebSocket:  {'✅ Connected' if self.test_results['backend_connected'] else '❌ Failed'}")
        
        # Telemetry Flow Results
        print("\n📊 TELEMETRY FLOW RESULTS:")
        print(f"   Firmware → Test:    {self.test_results['telemetry_received_from_firmware']} messages")
        print(f"   Backend → Frontend: {self.test_results['telemetry_relayed_to_frontend']} messages")
        
        # Calculate success rate
        firmware_msgs = self.test_results['telemetry_received_from_firmware']
        relayed_msgs = self.test_results['telemetry_relayed_to_frontend']
        relay_rate = (relayed_msgs / firmware_msgs * 100) if firmware_msgs > 0 else 0
        
        print(f"   Relay Success Rate: {relay_rate:.1f}%")
        
        # Performance Results
        print("\n⚡ PERFORMANCE RESULTS:")
        print(f"   Test Duration:      {self.test_results['test_duration_seconds']:.1f}s")
        if firmware_msgs > 0:
            msg_rate = firmware_msgs / self.test_results['test_duration_seconds']
            print(f"   Message Rate:       {msg_rate:.2f} msg/s")
            
        # Error Summary
        print("\n🚨 ERROR SUMMARY:")
        if self.test_results['errors']:
            for i, error in enumerate(self.test_results['errors'], 1):
                print(f"   {i}. {error}")
        else:
            print("   ✅ No errors detected")
            
        # Overall Result
        print("\n🎯 OVERALL RESULT:")
        if (self.test_results['firmware_connected'] and 
            self.test_results['backend_connected'] and 
            firmware_msgs > 0 and 
            relay_rate >= 80.0 and
            len(self.test_results['errors']) == 0):
            print("   🎉 INTEGRATION TEST PASSED!")
            print("   ✅ RS485 WebSocket Telemetry is working correctly")
        else:
            print("   ❌ INTEGRATION TEST FAILED")
            print("   🔧 Please check the issues above")
            
        print("=" * 60)


async def main():
    """Main test function"""
    test = RS485TelemetryIntegrationTest()
    
    print("🚀 RS485 WebSocket Telemetry Integration Test")
    print("📋 This test will:")
    print("   1. Connect to Firmware WebSocket Server (port 8081)")
    print("   2. Connect to Backend WebSocket (port 8000)")
    print("   3. Monitor telemetry flow for 30 seconds")
    print("   4. Verify data relay from Firmware → Backend → Frontend")
    print()
    
    # Wait for user confirmation
    input("⏳ Press Enter to start the test (make sure Firmware and Backend are running)...")
    
    # Run the test
    await test.run_integration_test(duration_seconds=30)


if __name__ == "__main__":
    asyncio.run(main())
