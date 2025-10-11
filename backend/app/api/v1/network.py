"""
Network Management API - OHT-50 Backend

REST API endpoints for network management, WiFi AP mode, and connectivity.
"""

import logging
from datetime import datetime, timezone
from typing import Dict, Any, Optional, List
from fastapi import APIRouter, HTTPException, Depends, status
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials

from app.schemas.network import (
    NetworkStatusResponse, NetworkStatusData,
    APStartRequest, APStartResponse, APStopResponse,
    APConfigRequest, APConfigResponse,
    APClientsResponse, APClientsData, APClient,
    FallbackEnableResponse, FallbackDisableResponse,
    NetworkErrorResponse,
    WiFiScanResponse, WiFiNetwork,
    WiFiConnectRequest, WiFiConnectResponse,
    WiFiDisconnectResponse,
    WiFiIPConfigRequest, WiFiIPConfigResponse, WiFiIPConfigData,  # Issue #216
    NetworkPerformanceResponse, NetworkPerformanceData,
    NetworkHealthResponse, NetworkHealthData
)
from app.services.network_integration_service import network_service
from app.api.deps import get_current_user
from app.core.security import verify_token

logger = logging.getLogger(__name__)

# Create router
router = APIRouter(prefix="/api/v1/network", tags=["network"])

# Security
security = HTTPBearer()


async def verify_admin_token(credentials: HTTPAuthorizationCredentials = Depends(security)) -> Dict[str, Any]:
    """Verify admin token for write operations"""
    try:
        # Simple token verification for development
        valid_tokens = [
            "oht50_admin_token_2025",
            "admin_token",
            "oht50_admin_token",
            "admin"
        ]
        
        if credentials.credentials in valid_tokens:
            return {
                "sub": "admin",
                "is_admin": True,
                "type": "admin"
            }
        
        # Try JWT verification as fallback
        payload = verify_token(credentials.credentials)
        if payload and payload.get("is_admin", False):
            return payload
            
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid or expired token"
        )
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Invalid or expired token"
        )


# Network Status Endpoints

@router.get("/status", response_model=NetworkStatusResponse)
async def get_network_status():
    """
    Get current network status
    
    Returns:
        NetworkStatusResponse: Current network status and configuration
    """
    try:
        logger.info("📊 Getting network status...")
        
        # Get network status from service
        result = await network_service.get_network_status()
        
        if result.get("success"):
            return NetworkStatusResponse(
                success=True,
                data=result["data"],
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to get network status")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error getting network status: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


# WiFi AP Mode Endpoints

@router.post("/ap/start", response_model=APStartResponse)
async def start_wifi_ap(
    request: APStartRequest,
    admin_user: Dict[str, Any] = Depends(verify_admin_token)
):
    """
    Start WiFi AP mode
    
    Args:
        request: WiFi AP configuration
        admin_user: Admin user token
        
    Returns:
        APStartResponse: AP start result
    """
    try:
        logger.info(f"📡 Starting WiFi AP: {request.ssid}")
        
        # Convert request to config dict
        config = {
            "ssid": request.ssid,
            "password": request.password,
            "channel": request.channel,
            "security": request.security.value,
            "hidden": request.hidden,
            "max_clients": request.max_clients
        }
        
        # Start AP via service
        result = await network_service.start_wifi_ap(config)
        
        if result.get("success"):
            return APStartResponse(
                success=True,
                message=result["message"],
                data=result.get("data"),
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to start WiFi AP")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error starting WiFi AP: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


@router.post("/ap/stop", response_model=APStopResponse)
async def stop_wifi_ap(
    admin_user: Dict[str, Any] = Depends(verify_admin_token)
):
    """
    Stop WiFi AP mode
    
    Args:
        admin_user: Admin user token
        
    Returns:
        APStopResponse: AP stop result
    """
    try:
        logger.info("🛑 Stopping WiFi AP...")
        
        # Stop AP via service
        result = await network_service.stop_wifi_ap()
        
        if result.get("success"):
            return APStopResponse(
                success=True,
                message=result["message"],
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to stop WiFi AP")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error stopping WiFi AP: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


@router.get("/ap/clients", response_model=APClientsResponse)
async def get_ap_clients():
    """
    Get connected AP clients
    
    Returns:
        APClientsResponse: List of connected clients
    """
    try:
        logger.info("📱 Getting AP clients...")
        
        # Get clients via service
        result = await network_service.get_ap_clients()
        
        if result.get("success"):
            clients_data = result["data"]
            clients = [
                APClient(
                    mac_address=client["mac_address"],
                    ip_address=client.get("ip_address"),
                    hostname=client.get("hostname"),
                    device_type=client.get("device_type"),
                    signal_strength=client.get("signal_strength"),
                    connected_at=datetime.fromisoformat(client["connected_at"]),
                    last_seen=datetime.fromisoformat(client.get("last_seen", client.get("connected_at", datetime.now(timezone.utc).isoformat()))),
                    bytes_sent=client.get("bytes_sent", 0),
                    bytes_received=client.get("bytes_received", 0),
                    session_duration=client.get("session_duration", 0),
                    is_connected=client.get("is_connected", True),
                    is_blocked=client.get("is_blocked", False)
                )
                for client in clients_data.get("clients", [])
            ]
            
            return APClientsResponse(
                success=True,
                data={
                    "clients": [client.dict() for client in clients],
                    "client_count": clients_data.get("client_count", len(clients))
                },
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to get AP clients")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error getting AP clients: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


@router.get("/ap/config", response_model=APConfigResponse)
async def get_ap_config(
    current_user: Dict[str, Any] = Depends(get_current_user)
):
    """
    Get WiFi Access Point configuration - Issue #209
    
    Returns current AP configuration including:
    - is_enabled: AP status (true/false)
    - ssid: Access Point name
    - password: AP password
    - channel: WiFi channel (1-13)
    - security_type: Security type (WPA2/WPA3/Open)
    - max_clients: Maximum allowed clients
    - ip_address: AP IP address
    
    Returns:
        APConfigResponse: AP configuration data
        
    Note:
    - Currently uses MOCK data for development
    - Will integrate with Firmware after testing
    - Firmware URL: GET /api/v1/network/ap/config
    """
    try:
        username = current_user.username if hasattr(current_user, 'username') else 'user'
        logger.info(f"📡 User {username} requesting AP config...")
        
        # Get AP config via service (Mock or Real)
        result = await network_service.get_ap_config()
        
        if result.get("success"):
            return APConfigResponse(
                success=True,
                message=result.get("message", "AP configuration retrieved successfully"),
                data=result.get("data"),
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to get AP configuration")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error getting AP config: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


@router.post("/ap/config", response_model=APConfigResponse)
async def configure_ap(
    request: APConfigRequest,
    admin_user: Dict[str, Any] = Depends(verify_admin_token)
):
    """
    Configure WiFi AP settings
    
    Args:
        request: AP configuration
        admin_user: Admin user token
        
    Returns:
        APConfigResponse: Configuration result
    """
    try:
        logger.info("⚙️ Configuring WiFi AP...")
        
        # Convert request to config dict (only non-None values)
        config = {}
        if request.ssid is not None:
            config["ssid"] = request.ssid
        if request.password is not None:
            config["password"] = request.password
        if request.channel is not None:
            config["channel"] = request.channel
        if request.security is not None:
            config["security"] = request.security.value
        if request.hidden is not None:
            config["hidden"] = request.hidden
        if request.max_clients is not None:
            config["max_clients"] = request.max_clients
        if request.bandwidth_limit is not None:
            config["bandwidth_limit"] = request.bandwidth_limit
        
        # Configure AP via service
        result = await network_service.configure_ap(config)
        
        if result.get("success"):
            return APConfigResponse(
                success=True,
                message=result["message"],
                data=result.get("data"),
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to configure WiFi AP")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error configuring WiFi AP: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


# Fallback Connectivity Endpoints

@router.post("/fallback/enable", response_model=FallbackEnableResponse)
async def enable_fallback(
    admin_user: Dict[str, Any] = Depends(verify_admin_token)
):
    """
    Enable fallback connectivity
    
    Args:
        admin_user: Admin user token
        
    Returns:
        FallbackEnableResponse: Fallback enable result
    """
    try:
        logger.info("🔄 Enabling fallback connectivity...")
        
        # Enable fallback via service
        result = await network_service.enable_fallback()
        
        if result.get("success"):
            return FallbackEnableResponse(
                success=True,
                message=result["message"],
                data=result.get("data"),
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to enable fallback")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error enabling fallback: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


@router.post("/fallback/disable", response_model=FallbackDisableResponse)
async def disable_fallback(
    admin_user: Dict[str, Any] = Depends(verify_admin_token)
):
    """
    Disable fallback connectivity
    
    Args:
        admin_user: Admin user token
        
    Returns:
        FallbackDisableResponse: Fallback disable result
    """
    try:
        logger.info("🛑 Disabling fallback connectivity...")
        
        # Disable fallback via service
        result = await network_service.disable_fallback()
        
        if result.get("success"):
            return FallbackDisableResponse(
                success=True,
                message=result["message"],
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to disable fallback")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error disabling fallback: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


# WiFi Management Endpoints

@router.get("/wifi/scan", response_model=WiFiScanResponse)
async def scan_wifi_networks():
    """
    Scan for available WiFi networks
    
    Returns:
        WiFiScanResponse: List of available WiFi networks
    """
    try:
        logger.info("📡 Scanning for WiFi networks...")
        
        # Scan WiFi via service
        result = await network_service.scan_wifi_networks()
        
        if result.get("success"):
            networks_data = result["data"]
            networks = [
                WiFiNetwork(
                    ssid=network["ssid"],
                    bssid=network.get("bssid"),
                    signal_strength=network.get("signal_strength"),
                    frequency=network.get("frequency"),
                    security=network.get("security"),
                    is_connected=network.get("is_connected", False),
                    is_saved=network.get("is_saved", False)
                )
                for network in networks_data.get("networks", [])
            ]
            
            return WiFiScanResponse(
                success=True,
                data={
                    "networks": [network.dict() for network in networks],
                    "network_count": networks_data.get("network_count", len(networks))
                },
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to scan WiFi networks")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error scanning WiFi networks: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


@router.post("/wifi/connect", response_model=WiFiConnectResponse)
async def connect_wifi(
    request: WiFiConnectRequest,
    admin_user: Dict[str, Any] = Depends(verify_admin_token)
):
    """
    Connect to WiFi network
    
    Args:
        request: WiFi connection details
        admin_user: Admin user token
        
    Returns:
        WiFiConnectResponse: Connection result
    """
    try:
        logger.info(f"🔗 Connecting to WiFi: {request.ssid}")
        
        # Connect via service
        result = await network_service.connect_wifi(
            ssid=request.ssid,
            password=request.password,
            security=request.security
        )
        
        if result.get("success"):
            return WiFiConnectResponse(
                success=True,
                message=result["message"],
                data=result.get("data"),
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to connect to WiFi")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error connecting to WiFi: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


@router.post("/wifi/disconnect", response_model=WiFiDisconnectResponse)
async def disconnect_wifi(
    admin_user: Dict[str, Any] = Depends(verify_admin_token)
):
    """
    Disconnect from current WiFi network
    
    Args:
        admin_user: Admin user token
        
    Returns:
        WiFiDisconnectResponse: Disconnection result
    """
    try:
        logger.info("🔌 Disconnecting from WiFi...")
        
        # Disconnect via service
        result = await network_service.disconnect_wifi()
        
        if result.get("success"):
            return WiFiDisconnectResponse(
                success=True,
                message=result["message"],
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to disconnect from WiFi")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error disconnecting from WiFi: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


@router.put("/wifi/ip-config", response_model=WiFiIPConfigResponse)
async def update_wifi_ip_config(
    request: WiFiIPConfigRequest,
    admin_user: Dict[str, Any] = Depends(verify_admin_token)
):
    """
    🆕 Update WiFi IP Configuration (Static or DHCP) - Issue #206/#216
    
    **STATUS:** 🧪 MOCK MODE (Firmware Issue #215 in progress)
    
    Configure WiFi IP address mode and settings.
    
    **Args:**
        request: IP configuration
            - ip_mode: "Static" or "DHCP"
            - ip_address: IP address (required for Static mode)
            - netmask: Subnet mask (required for Static mode)
            - gateway: Gateway address (required for Static mode)
            - dns: DNS server (optional, default: 8.8.8.8)
        admin_user: Admin user token
        
    **Returns:**
        WiFiIPConfigResponse: Updated network configuration with current IP info
        
    **Example Request (Static IP):**
    ```json
    {
      "ip_mode": "Static",
      "ip_address": "192.168.1.120",
      "netmask": "255.255.255.0",
      "gateway": "192.168.1.1",
      "dns": "8.8.8.8"
    }
    ```
    
    **Example Request (DHCP):**
    ```json
    {
      "ip_mode": "DHCP"
    }
    ```
    
    **Example Response:**
    ```json
    {
      "success": true,
      "data": {
        "ssid": "OHT-50-Network",
        "ip_mode": "Static",
        "signal_strength": -52,
        "ip_address": "192.168.1.120",
        "gateway": "192.168.1.1",
        "dns": "8.8.8.8",
        "netmask": "255.255.255.0",
        "mac_address": "AA:BB:CC:DD:EE:FF",
        "security_type": "WPA2",
        "link_speed": 150,
        "uptime_seconds": 300
      },
      "message": "IP configuration updated successfully",
      "timestamp": "2025-01-11T10:30:00Z"
    }
    ```
    
    **Validation:**
    - Static mode requires: ip_address, netmask, gateway
    - DHCP mode ignores IP fields
    - IPv4 format validation
    
    **Notes:**
    - Currently uses MOCK data for development
    - Will integrate with Firmware after Issue #215 completion
    - Firmware URL: PUT /api/v1/network/wifi/ip-config
    """
    try:
        logger.info(f"⚙️ User {admin_user.get('username', 'admin')} updating WiFi IP config: {request.ip_mode.value}")
        
        # Call service (Mock or Real)
        result = await network_service.configure_wifi_ip(
            ip_mode=request.ip_mode.value,
            ip_address=request.ip_address,
            netmask=request.netmask,
            gateway=request.gateway,
            dns=request.dns
        )
        
        if result.get("success"):
            logger.info("✅ WiFi IP configuration updated successfully")
            
            # Build response data
            data_dict = result.get("data")
            if data_dict:
                response_data = WiFiIPConfigData(**data_dict)
            else:
                response_data = None
            
            return WiFiIPConfigResponse(
                success=True,
                data=response_data,
                message=result.get("message", "IP configuration updated successfully"),
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to update WiFi IP configuration")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error updating WiFi IP configuration: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


@router.get("/performance", response_model=NetworkPerformanceResponse)
async def get_network_performance():
    """
    Get network performance metrics
    
    Returns:
        NetworkPerformanceResponse: Network performance data
    """
    try:
        logger.info("📊 Getting network performance...")
        
        # Get performance via service
        result = await network_service.get_network_performance()
        
        if result.get("success"):
            performance_data = result["data"]
            return NetworkPerformanceResponse(
                success=True,
                data=NetworkPerformanceData(
                    bandwidth_mbps=performance_data.get("bandwidth_mbps"),
                    latency_ms=performance_data.get("latency_ms"),
                    packet_loss_percent=performance_data.get("packet_loss_percent"),
                    signal_strength_dbm=performance_data.get("signal_strength_dbm"),
                    connection_quality=performance_data.get("connection_quality"),
                    throughput_mbps=performance_data.get("throughput_mbps"),
                    jitter_ms=performance_data.get("jitter_ms"),
                    uptime_seconds=performance_data.get("uptime_seconds")
                ),
                timestamp=datetime.now(timezone.utc)
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=result.get("error", "Failed to get network performance")
            )
            
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Error getting network performance: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Internal server error: {str(e)}"
        )


# Health Check Endpoint

@router.get("/health", response_model=NetworkHealthResponse)
async def network_health():
    """
    Network service health check
    
    Returns:
        NetworkHealthResponse: Health status
    """
    try:
        # Get connection status
        status = network_service.get_connection_status()
        
        health_data = NetworkHealthData(
            status="healthy" if status["status"] == "connected" else "unhealthy",
            service="network",
            firmware_connected=status["status"] == "connected",
            connection_errors=status["connection_errors"],
            last_heartbeat=status["last_heartbeat"],
            uptime_seconds=status.get("uptime_seconds", 0),
            memory_usage_percent=status.get("memory_usage_percent", 0),
            cpu_usage_percent=status.get("cpu_usage_percent", 0)
        )
        
        return NetworkHealthResponse(
            success=True,
            data=health_data,
            timestamp=datetime.now(timezone.utc)
        )
        
    except Exception as e:
        logger.error(f"❌ Network health check failed: {e}")
        return NetworkHealthResponse(
            success=False,
            error=str(e),
            timestamp=datetime.now(timezone.utc)
        )
