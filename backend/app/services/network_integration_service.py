"""
Network Integration Service - OHT-50 Backend

This service provides network management integration with the OHT-50 Firmware,
handling WiFi AP mode, network configuration, and connectivity management.

WARNING: This service MUST connect to real Firmware via HTTP API
DO NOT use mock data in production
"""

import asyncio
import logging
import time
from datetime import datetime, timezone
from typing import Dict, Any, Optional, List
from enum import Enum

from app.lib.fw_client import FWClient, FWConfig, FWConnectionStatus, FWClientError
from app.config import settings

logger = logging.getLogger(__name__)


class NetworkStatus(Enum):
    """Network connection status"""
    DISCONNECTED = "disconnected"
    CONNECTING = "connecting"
    CONNECTED = "connected"
    ERROR = "error"
    AP_MODE = "ap_mode"
    FALLBACK = "fallback"


class WiFiAPStatus(Enum):
    """WiFi AP mode status"""
    STOPPED = "stopped"
    STARTING = "starting"
    RUNNING = "running"
    STOPPING = "stopping"
    ERROR = "error"


class NetworkIntegrationService:
    """
    Network Integration Service
    
    WARNING: This service MUST connect to real Firmware via HTTP API
    DO NOT use mock data in production
    """
    
    def __init__(self, firmware_url=None):
        """
        Initialize network integration service
        
        Args:
            firmware_url: Firmware HTTP API URL
        """
        self.firmware_url = firmware_url or getattr(settings, 'firmware_url', 'http://localhost:8080')
        
        # Initialize FW client
        self.fw_client: Optional[FWClient] = None
        self.fw_config = FWConfig(
            host="localhost",
            http_port=8080,
            auth_token="oht50_operator_token_2025",  # Set operator token for firmware authentication
            timeout=getattr(settings, 'firmware_timeout', 10.0),
            max_retries=getattr(settings, 'firmware_retry_count', 3)
        )
        
        # Network status
        self.status = NetworkStatus.DISCONNECTED
        self.ap_status = WiFiAPStatus.STOPPED
        self.last_heartbeat = None
        self.connection_errors = 0
        self.max_connection_errors = 20
        
        # Network configuration cache
        self.network_config = {}
        self.ap_clients = []
        self.fallback_enabled = False
        
        # Background tasks
        self._health_monitor_task: Optional[asyncio.Task] = None
        self._network_monitor_task: Optional[asyncio.Task] = None
        
        # WARNING: This service MUST connect to real Firmware
        # DO NOT use mock data in production
        logger.warning("🔌 Network Integration: Connecting to REAL Firmware at %s", self.firmware_url)
        logger.warning("⚠️  WARNING: This service MUST use real Firmware - NO mock data in production!")
        
    async def initialize(self) -> bool:
        """
        Initialize network integration service
        
        Returns:
            True if initialization successful, False otherwise
        """
        try:
            logger.info("🚀 Initializing Network Integration Service...")
            
            # Create FW client
            self.fw_client = FWClient(self.fw_config)
            
            # Connect to firmware
            connected = await self.fw_client.connect()
            if not connected:
                logger.error("❌ Failed to connect to firmware during initialization")
                self.status = NetworkStatus.ERROR
                return False
            
            self.status = NetworkStatus.CONNECTED
            self.last_heartbeat = datetime.now(timezone.utc)
            
            # Start background monitoring tasks
            await self._start_background_tasks()
            
            logger.info("✅ Network Integration Service initialized successfully")
            return True
            
        except Exception as e:
            logger.error(f"❌ Network Integration Service initialization failed: {e}")
            self.status = NetworkStatus.ERROR
            return False
    
    async def shutdown(self) -> None:
        """Shutdown network integration service"""
        logger.info("🛑 Shutting down Network Integration Service...")
        
        # Stop background tasks
        if self._health_monitor_task:
            self._health_monitor_task.cancel()
        if self._network_monitor_task:
            self._network_monitor_task.cancel()
        
        # Disconnect from firmware
        if self.fw_client:
            await self.fw_client.disconnect()
        
        self.status = NetworkStatus.DISCONNECTED
        logger.info("✅ Network Integration Service shutdown complete")
    
    async def _start_background_tasks(self) -> None:
        """Start background monitoring tasks"""
        # Health monitoring task
        self._health_monitor_task = asyncio.create_task(self._health_monitor_loop())
        
        # Network monitoring task
        self._network_monitor_task = asyncio.create_task(self._network_monitor_loop())
        
        logger.info("✅ Background monitoring tasks started")
    
    async def _health_monitor_loop(self) -> None:
        """Background health monitoring loop"""
        while True:
            try:
                if self.fw_client:
                    is_healthy = await self.fw_client.check_connection_health()
                    
                    if is_healthy:
                        self.status = NetworkStatus.CONNECTED
                        self.connection_errors = 0
                    else:
                        self.status = NetworkStatus.ERROR
                        self.connection_errors += 1
                        
                        # Attempt reconnection if too many errors
                        if self.connection_errors >= self.max_connection_errors:
                            await self._attempt_reconnection()
                
                # Wait before next check
                await asyncio.sleep(10)  # Check every 10 seconds
                
            except asyncio.CancelledError:
                logger.info("🛑 Health monitor task cancelled")
                break
            except Exception as e:
                logger.error(f"❌ Health monitor error: {e}")
                await asyncio.sleep(5)  # Wait before retry
    
    async def _network_monitor_loop(self) -> None:
        """Background network monitoring loop"""
        poll_interval = 5.0  # seconds
        while True:
            try:
                if self.fw_client and self.status == NetworkStatus.CONNECTED:
                    # Monitor network status
                    await self._update_network_status()
                    
                    # Monitor AP clients if AP is running
                    if self.ap_status == WiFiAPStatus.RUNNING:
                        await self._update_ap_clients()
                
                await asyncio.sleep(poll_interval)
                
            except asyncio.CancelledError:
                logger.info("🛑 Network monitor task cancelled")
                break
            except Exception as e:
                logger.error(f"❌ Network monitoring error: {e}")
                await asyncio.sleep(poll_interval)
    
    async def _update_network_status(self) -> None:
        """Update network status from firmware"""
        try:
            response = await self.fw_client.get("/api/v1/network/status")
            if response and response.get("success"):
                data = response.get("data", {})
                self.network_config = data.get("config", {})
                self.fallback_enabled = data.get("fallback_enabled", False)
                
                # Update status based on firmware response
                if data.get("ap_mode_active"):
                    self.ap_status = WiFiAPStatus.RUNNING
                    self.status = NetworkStatus.AP_MODE
                elif data.get("fallback_active"):
                    self.status = NetworkStatus.FALLBACK
                else:
                    self.status = NetworkStatus.CONNECTED
                    
        except Exception as e:
            logger.error(f"❌ Error updating network status: {e}")
    
    async def _update_ap_clients(self) -> None:
        """Update AP client list from firmware"""
        try:
            response = await self.fw_client.get("/api/v1/network/ap/clients")
            if response and response.get("success"):
                self.ap_clients = response.get("data", {}).get("clients", [])
                    
        except Exception as e:
            logger.error(f"❌ Error updating AP clients: {e}")
    
    async def _attempt_reconnection(self) -> None:
        """Attempt to reconnect to firmware"""
        try:
            logger.warning("🔄 Attempting firmware reconnection...")
            self.status = NetworkStatus.CONNECTING
            
            # Add exponential backoff delay
            backoff_delay = min(30, 2 ** (self.connection_errors // 5))  # Max 30 seconds
            logger.info(f"⏳ Waiting {backoff_delay}s before reconnection...")
            await asyncio.sleep(backoff_delay)
            
            # Disconnect current client
            if self.fw_client:
                await self.fw_client.disconnect()
            
            # Create new client and connect
            self.fw_client = FWClient(self.fw_config)
            connected = await self.fw_client.connect()
            
            if connected:
                self.status = NetworkStatus.CONNECTED
                self.connection_errors = 0
                logger.info("✅ Firmware reconnection successful")
                
                # Restart background tasks
                await self._start_background_tasks()
            else:
                self.status = NetworkStatus.ERROR
                logger.error("❌ Firmware reconnection failed")
                
        except Exception as e:
            logger.error(f"❌ Reconnection error: {e}")
            self.status = NetworkStatus.ERROR
    
    # Network Status API Methods
    
    async def get_network_status(self) -> Dict[str, Any]:
        """Get current network status"""
        try:
            # Auto-initialize if not already done
            if not self.fw_client:
                logger.info("🔄 Auto-initializing FW client...")
                await self.initialize()
                if not self.fw_client:
                    return {"success": False, "error": "Failed to initialize FW client"}
            
            response = await self.fw_client.get("/api/v1/network/status")
            if response and response.get("success"):
                # Get firmware data
                firmware_data = response.get("data", {})
                
                return {
                    "success": True,
                    "data": {
                        # Firmware data (real network info)
                        "connected": firmware_data.get("connected", False),
                        "current_ssid": firmware_data.get("current_ssid", ""),
                        "signal_strength": firmware_data.get("signal_strength", 0),
                        "ip_address": firmware_data.get("ip_address", ""),
                        "gateway": firmware_data.get("gateway", ""),
                        "dns": firmware_data.get("dns", ""),
                        "bytes_sent": firmware_data.get("bytes_sent", 0),
                        "bytes_received": firmware_data.get("bytes_received", 0),
                        "latency_ms": firmware_data.get("latency_ms", 0),
                        "roaming_active": firmware_data.get("roaming_active", False),
                        # Backend status
                        "status": self.status.value,
                        "ap_status": self.ap_status.value,
                        "fallback_enabled": self.fallback_enabled,
                        "config": self.network_config,
                        "last_heartbeat": self.last_heartbeat.isoformat() if self.last_heartbeat else None,
                        "connection_errors": self.connection_errors
                    },
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                return {"success": False, "error": "Failed to get network status from firmware"}
                
        except Exception as e:
            logger.error(f"❌ Failed to get network status: {e}")
            return {"success": False, "error": str(e)}
    
    # WiFi AP Mode API Methods
    
    async def start_wifi_ap(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Start WiFi AP mode"""
        try:
            if not self.fw_client:
                return {"success": False, "error": "FW client not initialized"}
            
            logger.info("📡 Starting WiFi AP mode...")
            
            # Validate config
            required_fields = ["ssid", "password", "channel"]
            if not all(field in config for field in required_fields):
                return {"success": False, "error": f"Missing required fields: {required_fields}"}
            
            # Call firmware API to start AP
            response = await self.fw_client.post("/api/v1/network/ap/start", data=config)
            
            if response and response.get("success"):
                self.ap_status = WiFiAPStatus.RUNNING
                self.status = NetworkStatus.AP_MODE
                logger.info("✅ WiFi AP started successfully")
                return {
                    "success": True,
                    "message": "WiFi AP started successfully",
                    "data": response.get("data", {}),
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                error_msg = response.get("error", "Unknown error") if response else "No response from firmware"
                logger.error(f"❌ Failed to start WiFi AP: {error_msg}")
                return {
                    "success": False,
                    "message": "Failed to start WiFi AP",
                    "error": error_msg
                }
                
        except Exception as e:
            logger.error(f"❌ Failed to start WiFi AP: {e}")
            return {
                "success": False,
                "message": f"Failed to start WiFi AP: {str(e)}",
                "error": str(e)
            }
    
    async def stop_wifi_ap(self) -> Dict[str, Any]:
        """Stop WiFi AP mode"""
        try:
            if not self.fw_client:
                return {"success": False, "error": "FW client not initialized"}
            
            logger.info("🛑 Stopping WiFi AP mode...")
            
            # Call firmware API to stop AP
            response = await self.fw_client.post("/api/v1/network/ap/stop", data={})
            
            if response and response.get("success"):
                self.ap_status = WiFiAPStatus.STOPPED
                self.status = NetworkStatus.CONNECTED
                self.ap_clients = []
                logger.info("✅ WiFi AP stopped successfully")
                return {
                    "success": True,
                    "message": "WiFi AP stopped successfully",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                error_msg = response.get("error", "Unknown error") if response else "No response from firmware"
                logger.error(f"❌ Failed to stop WiFi AP: {error_msg}")
                return {
                    "success": False,
                    "message": "Failed to stop WiFi AP",
                    "error": error_msg
                }
                
        except Exception as e:
            logger.error(f"❌ Failed to stop WiFi AP: {e}")
            return {
                "success": False,
                "message": f"Failed to stop WiFi AP: {str(e)}",
                "error": str(e)
            }
    
    async def get_ap_clients(self) -> Dict[str, Any]:
        """Get connected AP clients"""
        try:
            # Auto-initialize if not already done
            if not self.fw_client:
                logger.info("🔄 Auto-initializing FW client...")
                await self.initialize()
                if not self.fw_client:
                    return {"success": False, "error": "Failed to initialize FW client"}
            
            response = await self.fw_client.get("/api/v1/network/ap/clients")
            if response and response.get("success"):
                self.ap_clients = response.get("data", {}).get("clients", [])
                return {
                    "success": True,
                    "data": {
                        "clients": self.ap_clients,
                        "client_count": len(self.ap_clients)
                    },
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                error_msg = response.get("error", "Unknown error") if response else "No response from firmware"
                logger.error(f"❌ Failed to get AP clients: {error_msg}")
                return {
                    "success": False,
                    "message": "Failed to get AP clients",
                    "error": error_msg
                }
                
        except Exception as e:
            logger.error(f"❌ Failed to get AP clients: {e}")
            return {
                "success": False,
                "message": f"Failed to get AP clients: {str(e)}",
                "error": str(e)
            }
    
    async def configure_ap(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Configure WiFi AP settings"""
        try:
            if not self.fw_client:
                return {"success": False, "error": "FW client not initialized"}
            
            logger.info("⚙️ Configuring WiFi AP...")
            
            # Call firmware API to configure AP
            response = await self.fw_client.post("/api/v1/network/ap/config", data=config)
            
            if response and response.get("success"):
                self.network_config.update(config)
                logger.info("✅ WiFi AP configured successfully")
                return {
                    "success": True,
                    "message": "WiFi AP configured successfully",
                    "data": response.get("data", {}),
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                error_msg = response.get("error", "Unknown error") if response else "No response from firmware"
                logger.error(f"❌ Failed to configure WiFi AP: {error_msg}")
                return {
                    "success": False,
                    "message": "Failed to configure WiFi AP",
                    "error": error_msg
                }
                
        except Exception as e:
            logger.error(f"❌ Failed to configure WiFi AP: {e}")
            return {
                "success": False,
                "message": f"Failed to configure WiFi AP: {str(e)}",
                "error": str(e)
            }
    
    # Fallback Connectivity API Methods
    
    async def enable_fallback(self) -> Dict[str, Any]:
        """Enable fallback connectivity"""
        try:
            if not self.fw_client:
                return {"success": False, "error": "FW client not initialized"}
            
            logger.info("🔄 Enabling fallback connectivity...")
            
            # Call firmware API to enable fallback
            response = await self.fw_client.post("/api/v1/network/fallback/enable", data={})
            
            if response and response.get("success"):
                self.fallback_enabled = True
                self.status = NetworkStatus.FALLBACK
                logger.info("✅ Fallback connectivity enabled")
                return {
                    "success": True,
                    "message": "Fallback connectivity enabled",
                    "data": response.get("data", {}),
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                error_msg = response.get("error", "Unknown error") if response else "No response from firmware"
                logger.error(f"❌ Failed to enable fallback: {error_msg}")
                return {
                    "success": False,
                    "message": "Failed to enable fallback connectivity",
                    "error": error_msg
                }
                
        except Exception as e:
            logger.error(f"❌ Failed to enable fallback: {e}")
            return {
                "success": False,
                "message": f"Failed to enable fallback: {str(e)}",
                "error": str(e)
            }
    
    async def disable_fallback(self) -> Dict[str, Any]:
        """Disable fallback connectivity"""
        try:
            if not self.fw_client:
                return {"success": False, "error": "FW client not initialized"}
            
            logger.info("🛑 Disabling fallback connectivity...")
            
            # Call firmware API to disable fallback
            response = await self.fw_client.post("/api/v1/network/fallback/disable", data={})
            
            if response and response.get("success"):
                self.fallback_enabled = False
                self.status = NetworkStatus.CONNECTED
                logger.info("✅ Fallback connectivity disabled")
                return {
                    "success": True,
                    "message": "Fallback connectivity disabled",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                error_msg = response.get("error", "Unknown error") if response else "No response from firmware"
                logger.error(f"❌ Failed to disable fallback: {error_msg}")
                return {
                    "success": False,
                    "message": "Failed to disable fallback connectivity",
                    "error": error_msg
                }
                
        except Exception as e:
            logger.error(f"❌ Failed to disable fallback: {e}")
            return {
                "success": False,
                "message": f"Failed to disable fallback: {str(e)}",
                "error": str(e)
            }
    
    # WiFi Management Methods
    
    async def scan_wifi_networks(self) -> Dict[str, Any]:
        """Scan for available WiFi networks"""
        try:
            # Auto-initialize if not already done
            if not self.fw_client:
                logger.info("🔄 Auto-initializing FW client for WiFi scan...")
                await self.initialize()
                if not self.fw_client:
                    return {"success": False, "error": "Failed to initialize FW client"}
            
            logger.info("📡 Scanning for WiFi networks...")
            
            # Call firmware API to scan WiFi
            response = await self.fw_client.get("/api/v1/network/wifi/scan")
            
            if response and response.get("success"):
                logger.info("✅ WiFi scan completed successfully")
                return {
                    "success": True,
                    "data": response.get("data", {}),
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                error_msg = response.get("error", "Unknown error") if response else "No response from firmware"
                logger.error(f"❌ Failed to scan WiFi networks: {error_msg}")
                return {
                    "success": False,
                    "message": "Failed to scan WiFi networks",
                    "error": error_msg
                }
                
        except Exception as e:
            logger.error(f"❌ Failed to scan WiFi networks: {e}")
            return {
                "success": False,
                "message": f"Failed to scan WiFi networks: {str(e)}",
                "error": str(e)
            }
    
    async def connect_wifi(self, ssid: str, password: Optional[str] = None, security: Optional[str] = None) -> Dict[str, Any]:
        """Connect to WiFi network"""
        try:
            # Auto-initialize if not already done
            if not self.fw_client:
                logger.info("🔄 Auto-initializing FW client for WiFi connect...")
                await self.initialize()
                if not self.fw_client:
                    return {"success": False, "error": "Failed to initialize FW client"}
            
            logger.info(f"🔗 Connecting to WiFi: {ssid}")
            
            # Prepare connection data
            connection_data = {
                "ssid": ssid,
                "password": password,
                "security": security
            }
            
            # Call firmware API to connect WiFi
            response = await self.fw_client.post("/api/v1/network/wifi/connect", data=connection_data)
            
            if response and response.get("success"):
                logger.info(f"✅ Connected to WiFi: {ssid}")
                return {
                    "success": True,
                    "message": f"Connected to {ssid}",
                    "data": response.get("data", {}),
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                error_msg = response.get("error", "Unknown error") if response else "No response from firmware"
                logger.error(f"❌ Failed to connect to WiFi: {error_msg}")
                return {
                    "success": False,
                    "message": "Failed to connect to WiFi",
                    "error": error_msg
                }
                
        except Exception as e:
            logger.error(f"❌ Failed to connect to WiFi: {e}")
            return {
                "success": False,
                "message": f"Failed to connect to WiFi: {str(e)}",
                "error": str(e)
            }
    
    async def configure_wifi_ip(
        self,
        ip_mode: str,
        ip_address: str = None,
        netmask: str = None,
        gateway: str = None,
        dns: str = None
    ) -> Dict[str, Any]:
        """
        Configure WiFi IP settings (Static or DHCP) - Issue #206/#216
        
        Args:
            ip_mode: "Static" or "DHCP"
            ip_address: IP address (required for Static)
            netmask: Subnet mask (required for Static)
            gateway: Gateway address (required for Static)
            dns: DNS server (optional)
            
        Returns:
            {
              "success": True,
              "data": {...},
              "message": "IP configuration updated successfully"
            }
        """
        try:
            # Auto-initialize if not already done
            if not self.fw_client:
                logger.info("🔄 Auto-initializing FW client for WiFi IP config...")
                await self.initialize()
                if not self.fw_client:
                    return {"success": False, "error": "Failed to initialize FW client"}
            
            logger.info(f"⚙️ Configuring WiFi IP: mode={ip_mode}")
            
            # Call Firmware API
            response = await self.fw_client.put(
                "/api/v1/network/wifi/ip-config",
                json={
                    "ip_mode": ip_mode,
                    "ip_address": ip_address,
                    "netmask": netmask,
                    "gateway": gateway,
                    "dns": dns
                }
            )
            
            if response and response.get("success"):
                logger.info("✅ WiFi IP configuration updated successfully")
                return {
                    "success": True,
                    "data": response.get("data"),
                    "message": response.get("message", "IP configuration updated successfully")
                }
            else:
                error_msg = response.get("error", "Unknown error") if response else "No response from firmware"
                logger.error(f"❌ Failed to configure WiFi IP: {error_msg}")
                return {
                    "success": False,
                    "message": "Failed to configure WiFi IP",
                    "error": error_msg
                }
                
        except Exception as e:
            logger.error(f"❌ Failed to configure WiFi IP: {e}")
            return {
                "success": False,
                "message": f"Failed to configure WiFi IP: {str(e)}",
                "error": str(e)
            }
    
    async def disconnect_wifi(self) -> Dict[str, Any]:
        """Disconnect from current WiFi network"""
        try:
            # Auto-initialize if not already done
            if not self.fw_client:
                logger.info("🔄 Auto-initializing FW client for WiFi disconnect...")
                await self.initialize()
                if not self.fw_client:
                    return {"success": False, "error": "Failed to initialize FW client"}
            
            logger.info("🔌 Disconnecting from WiFi...")
            
            # Call firmware API to disconnect WiFi
            response = await self.fw_client.post("/api/v1/network/wifi/disconnect")
            
            if response and response.get("success"):
                logger.info("✅ Disconnected from WiFi")
                return {
                    "success": True,
                    "message": "Disconnected from WiFi",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                error_msg = response.get("error", "Unknown error") if response else "No response from firmware"
                logger.error(f"❌ Failed to disconnect from WiFi: {error_msg}")
                return {
                    "success": False,
                    "message": "Failed to disconnect from WiFi",
                    "error": error_msg
                }
                
        except Exception as e:
            logger.error(f"❌ Failed to disconnect from WiFi: {e}")
            return {
                "success": False,
                "message": f"Failed to disconnect from WiFi: {str(e)}",
                "error": str(e)
            }
    
    async def get_network_performance(self) -> Dict[str, Any]:
        """Get network performance metrics"""
        try:
            # Auto-initialize if not already done
            if not self.fw_client:
                logger.info("🔄 Auto-initializing FW client for network performance...")
                await self.initialize()
                if not self.fw_client:
                    return {"success": False, "error": "Failed to initialize FW client"}
            
            logger.info("📊 Getting network performance...")
            
            # Call firmware API to get performance
            response = await self.fw_client.get("/api/v1/network/performance")
            
            if response and response.get("success"):
                logger.info("✅ Network performance retrieved successfully")
                return {
                    "success": True,
                    "data": response.get("data", {}),
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            else:
                error_msg = response.get("error", "Unknown error") if response else "No response from firmware"
                logger.error(f"❌ Failed to get network performance: {error_msg}")
                return {
                    "success": False,
                    "message": "Failed to get network performance",
                    "error": error_msg
                }
                
        except Exception as e:
            logger.error(f"❌ Failed to get network performance: {e}")
            return {
                "success": False,
                "message": f"Failed to get network performance: {str(e)}",
                "error": str(e)
            }
    
    # Connection Status Methods
    
    def get_connection_status(self) -> Dict[str, Any]:
        """Get connection status"""
        return {
            "status": self.status.value,
            "ap_status": self.ap_status.value,
            "firmware_url": self.firmware_url,
            "last_heartbeat": self.last_heartbeat.isoformat() if self.last_heartbeat else None,
            "connection_errors": self.connection_errors,
            "fallback_enabled": self.fallback_enabled,
            "ap_clients_count": len(self.ap_clients)
        }


# Mock Network Service for Development/Testing ONLY
class MockNetworkService:
    """
    Mock Network Service - FOR DEVELOPMENT/TESTING ONLY
    
    WARNING: This is MOCK data - NOT for production use
    """
    
    def __init__(self):
        # WARNING: This is MOCK data - NOT for production use
        logger.warning("🧪 MOCK Network Service: Using simulated data - NOT real Firmware!")
        logger.warning("⚠️  WARNING: This is for development/testing ONLY - DO NOT use in production!")
        
        self.mock_data = self._initialize_mock_data()
        
    def _initialize_mock_data(self) -> Dict[str, Any]:
        """Initialize mock network data"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        # DO NOT USE IN PRODUCTION
        return {
            "network_status": {
                "status": "connected",
                "ap_status": "stopped",
                "fallback_enabled": False,
                "config": {
                    "ssid": "OHT-50-AP",
                    "password": "oht50password",
                    "channel": 6,
                    "security": "WPA2"
                }
            },
            "ap_clients": [
                {
                    "mac_address": "AA:BB:CC:DD:EE:FF",
                    "ip_address": "192.168.4.2",
                    "hostname": "mobile-device",
                    "connected_at": datetime.now(timezone.utc).isoformat(),
                    "signal_strength": -45
                }
            ]
        }
    
    async def get_network_status(self) -> Dict[str, Any]:
        """Mock get network status"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        return {
            "success": True,
            "data": self.mock_data["network_status"],
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    
    async def start_wifi_ap(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Mock start WiFi AP"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        logger.warning("🧪 MOCK: Starting WiFi AP with config: %s", config)
        return {
            "success": True,
            "message": "Mock: WiFi AP started successfully",
            "data": {"ap_ip": "192.168.4.1"},
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    
    async def stop_wifi_ap(self) -> Dict[str, Any]:
        """Mock stop WiFi AP"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        logger.warning("🧪 MOCK: Stopping WiFi AP")
        return {
            "success": True,
            "message": "Mock: WiFi AP stopped successfully",
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    
    async def get_ap_clients(self) -> Dict[str, Any]:
        """Mock get AP clients"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        return {
            "success": True,
            "data": {
                "clients": self.mock_data["ap_clients"],
                "client_count": len(self.mock_data["ap_clients"])
            },
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    
    async def configure_ap(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Mock configure AP"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        logger.warning("🧪 MOCK: Configuring AP with: %s", config)
        return {
            "success": True,
            "message": "Mock: WiFi AP configured successfully",
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    
    async def enable_fallback(self) -> Dict[str, Any]:
        """Mock enable fallback"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        logger.warning("🧪 MOCK: Enabling fallback connectivity")
        return {
            "success": True,
            "message": "Mock: Fallback connectivity enabled",
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    
    async def disable_fallback(self) -> Dict[str, Any]:
        """Mock disable fallback"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        logger.warning("🧪 MOCK: Disabling fallback connectivity")
        return {
            "success": True,
            "message": "Mock: Fallback connectivity disabled",
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    
    def get_connection_status(self) -> Dict[str, Any]:
        """Get connection status for mock service"""
        return {
            "status": "connected",
            "ap_status": "stopped",
            "fallback_enabled": False,
            "ap_clients_count": len(self.mock_data.get("ap_clients", [])),
            "connection_errors": 0,
            "last_heartbeat": datetime.now(timezone.utc).isoformat()
        }
    
    async def scan_wifi_networks(self) -> Dict[str, Any]:
        """Mock scan WiFi networks"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        logger.warning("🧪 MOCK: Scanning WiFi networks")
        return {
            "success": True,
            "data": {
                "networks": [
                    {
                        "ssid": "VanPhong5G",
                        "bssid": "aa:bb:cc:dd:ee:01",
                        "signal_strength": -45,
                        "frequency": 2412,
                        "security": "WPA2",
                        "is_connected": True,
                        "is_saved": True
                    },
                    {
                        "ssid": "OHT-50-Hotspot",
                        "bssid": "aa:bb:cc:dd:ee:02",
                        "signal_strength": -52,
                        "frequency": 2437,
                        "security": "WPA2",
                        "is_connected": False,
                        "is_saved": False
                    },
                    {
                        "ssid": "TestNetwork",
                        "bssid": "aa:bb:cc:dd:ee:03",
                        "signal_strength": -65,
                        "frequency": 2462,
                        "security": "Open",
                        "is_connected": False,
                        "is_saved": False
                    }
                ],
                "network_count": 3
            },
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    
    async def connect_wifi(self, ssid: str, password: Optional[str] = None, security: Optional[str] = None) -> Dict[str, Any]:
        """Mock connect to WiFi"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        logger.warning(f"🧪 MOCK: Connecting to WiFi {ssid}")
        return {
            "success": True,
            "message": f"Mock: Connected to {ssid}",
            "data": {
                "ssid": ssid,
                "ip_address": "192.168.1.100",
                "signal_strength": -45,
                "connection_time": datetime.now(timezone.utc).isoformat()
            },
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    
    async def configure_wifi_ip(
        self,
        ip_mode: str,
        ip_address: str = None,
        netmask: str = None,
        gateway: str = None,
        dns: str = None
    ) -> Dict[str, Any]:
        """
        Mock configure WiFi IP settings - Issue #206/#216
        
        MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        Returns simulated IP configuration result
        """
        logger.warning(f"🧪 MOCK: Configuring WiFi IP - mode={ip_mode}")
        
        # Simulate IP configuration
        if ip_mode == "DHCP":
            mock_ip = "192.168.1.100"  # Simulated DHCP-assigned IP
            mock_gateway = "192.168.1.1"
            mock_netmask = "255.255.255.0"
            mock_dns = "8.8.8.8"
        else:  # Static
            mock_ip = ip_address or "192.168.1.120"
            mock_gateway = gateway or "192.168.1.1"
            mock_netmask = netmask or "255.255.255.0"
            mock_dns = dns or "8.8.8.8"
        
        return {
            "success": True,
            "data": {
                "ssid": "OHT-50-Network",
                "ip_mode": ip_mode,
                "signal_strength": -52,
                "ip_address": mock_ip,
                "gateway": mock_gateway,
                "dns": mock_dns,
                "netmask": mock_netmask,
                "mac_address": "AA:BB:CC:DD:EE:FF",
                "security_type": "WPA2",
                "link_speed": 150,
                "uptime_seconds": 300
            },
            "message": f"IP configuration updated successfully (MOCK - {ip_mode} mode)",
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    
    async def disconnect_wifi(self) -> Dict[str, Any]:
        """Mock disconnect from WiFi"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        logger.warning("🧪 MOCK: Disconnecting from WiFi")
        return {
            "success": True,
            "message": "Mock: Disconnected from WiFi",
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
    
    async def get_network_performance(self) -> Dict[str, Any]:
        """Mock get network performance"""
        # MOCK DATA - ONLY FOR DEVELOPMENT/TESTING
        logger.warning("🧪 MOCK: Getting network performance")
        return {
            "success": True,
            "data": {
                "bandwidth_mbps": 100.0,
                "latency_ms": 15.5,
                "packet_loss_percent": 0.1,
                "signal_strength_dbm": -45,
                "connection_quality": "Excellent",
                "throughput_mbps": 85.2,
                "jitter_ms": 2.1,
                "uptime_seconds": 3600
            },
            "timestamp": datetime.now(timezone.utc).isoformat()
        }


# Factory function to get network service
def get_network_service(use_mock: bool = False) -> NetworkIntegrationService:
    """
    Get network service instance
    
    Args:
        use_mock: Whether to use mock service (development only)
        
    Returns:
        Network service instance
    """
    # Check environment variables
    import os
    from app.config import settings
    
    testing_mode = os.getenv("TESTING", "false").lower() == "true"
    use_mock_env = os.getenv("USE_MOCK_FIRMWARE", "false").lower() == "true"
    settings_mock = getattr(settings, 'use_mock_firmware', False)
    is_production = os.getenv("ENVIRONMENT", "development").lower() == "production"
    
    logger.info(f"🔧 Network Service initialization: testing_mode={testing_mode}, use_mock_env={use_mock_env}, settings_mock={settings_mock}, is_production={is_production}")
    
    # Determine if should use mock
    should_use_mock = (testing_mode or use_mock_env or settings_mock or use_mock) and not is_production
    
    if should_use_mock:
        logger.warning("🧪 MOCK MODE: Using MockNetworkService for development/testing")
        logger.warning("⚠️  WARNING: This is MOCK data - NOT for production use!")
        return MockNetworkService()
    else:
        logger.info("🔌 REAL FIRMWARE MODE: Using NetworkIntegrationService with real firmware")
        logger.info("🔌 Connecting to Firmware at: %s", settings.firmware_url)
        return NetworkIntegrationService()


# Global network service instance - Use factory function
network_service = get_network_service()
