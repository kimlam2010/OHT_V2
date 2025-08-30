"""
FW Integration API Endpoints
Proxy endpoints for FW API communication
"""

from fastapi import APIRouter, Depends, HTTPException, status
from typing import Dict, List, Optional
import asyncio
import json

from app.lib.fw_client import FWClient, FWConfig, FWSystemStatus, FWModule
from app.services.log_service import LogService
from app.config import settings

router = APIRouter(tags=["FW Integration"])

# Global FW client instance
fw_client: Optional[FWClient] = None


def get_fw_client() -> Optional[FWClient]:
    """Get FW client instance"""
    global fw_client
    if fw_client is None:
        # Initialize FW client with configuration
        config = FWConfig(
            host=settings.FW_HOST or "localhost",
            http_port=settings.FW_HTTP_PORT or 8080,
            ws_port=settings.FW_WS_PORT or 8081,
            auth_token=settings.FW_AUTH_TOKEN,
            secret_key=settings.FW_SECRET_KEY
        )
        fw_client = FWClient(config)
    return fw_client


async def ensure_fw_connection() -> FWClient:
    """Ensure FW connection is established"""
    client = get_fw_client()
    if client is None:
        raise HTTPException(
            status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
            detail="FW client not initialized"
        )
    
    if client.state.value == "disconnected":
        # Try to connect
        connected = await client.connect()
        if not connected:
            raise HTTPException(
                status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
                detail="Failed to connect to FW"
            )
    
    return client


# Connection Management Endpoints

@router.post("/connect")
async def connect_to_fw():
    """Connect to FW server"""
    try:
        client = get_fw_client()
        if client is None:
            raise HTTPException(
                status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
                detail="FW client not initialized"
            )
        
        connected = await client.connect()
        if connected:
            return {
                "success": True,
                "message": "Connected to FW successfully",
                "status": client.get_connection_status()
            }
        else:
            raise HTTPException(
                status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
                detail="Failed to connect to FW"
            )
            
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Connection error: {str(e)}"
        )


@router.post("/disconnect")
async def disconnect_from_fw():
    """Disconnect from FW server"""
    try:
        client = get_fw_client()
        if client:
            await client.disconnect()
            return {
                "success": True,
                "message": "Disconnected from FW successfully"
            }
        else:
            return {
                "success": True,
                "message": "No active FW connection"
            }
            
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Disconnect error: {str(e)}"
        )


@router.get("/status")
async def get_fw_connection_status():
    """Get FW connection status"""
    try:
        client = get_fw_client()
        if client:
            return {
                "success": True,
                "data": client.get_connection_status()
            }
        else:
            return {
                "success": False,
                "message": "FW client not initialized"
            }
            
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Status error: {str(e)}"
        )


# System Endpoints

@router.get("/system/status")
async def get_fw_system_status():
    """Get FW system status"""
    try:
        client = await ensure_fw_connection()
        status = await client.get_system_status()
        
        if status:
            return {
                "success": True,
                "data": {
                    "system_name": status.system_name,
                    "version": status.version,
                    "status": status.status,
                    "uptime_ms": status.uptime_ms,
                    "active_modules": status.active_modules,
                    "estop_active": status.estop_active,
                    "safety_ok": status.safety_ok
                }
            }
        else:
            raise HTTPException(
                status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
                detail="Failed to get FW system status"
            )
            
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"System status error: {str(e)}"
        )


@router.get("/system/health")
async def get_fw_system_health():
    """Get FW system health"""
    try:
        client = await ensure_fw_connection()
        
        # Test connection by getting system status
        status = await client.get_system_status()
        if status:
            return {
                "success": True,
                "data": {
                    "status": "healthy",
                    "timestamp": status.uptime_ms,
                    "response_time_ms": 50,  # Mock response time
                    "details": "FW system operational"
                }
            }
        else:
            return {
                "success": False,
                "data": {
                    "status": "unhealthy",
                    "timestamp": 0,
                    "response_time_ms": 0,
                    "details": "FW system not responding"
                }
            }
            
    except Exception as e:
        return {
            "success": False,
            "data": {
                "status": "error",
                "timestamp": 0,
                "response_time_ms": 0,
                "details": f"FW health check error: {str(e)}"
            }
        }


# Module Endpoints

@router.get("/modules")
async def get_fw_modules():
    """Get list of FW modules"""
    try:
        client = await ensure_fw_connection()
        modules = await client.get_modules()
        
        return {
            "success": True,
            "data": {
                "modules": [
                    {
                        "module_id": module.module_id,
                        "module_type": module.module_type,
                        "status": module.status,
                        "online": module.online,
                        "last_seen": module.last_seen,
                        "version": module.version
                    }
                    for module in modules
                ],
                "module_count": len(modules)
            }
        }
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Modules error: {str(e)}"
        )


@router.get("/modules/{module_id}")
async def get_fw_module_info(module_id: int):
    """Get specific FW module information"""
    try:
        client = await ensure_fw_connection()
        module = await client.get_module_info(module_id)
        
        if module:
            return {
                "success": True,
                "data": {
                    "module_id": module.module_id,
                    "module_type": module.module_type,
                    "status": module.status,
                    "online": module.online,
                    "last_seen": module.last_seen,
                    "version": module.version
                }
            }
        else:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail=f"Module {module_id} not found"
            )
            
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Module info error: {str(e)}"
        )


@router.post("/modules/{module_id}/command")
async def send_fw_module_command(module_id: int, command_data: Dict):
    """Send command to FW module"""
    try:
        client = await ensure_fw_connection()
        
        command = command_data.get("command", "")
        parameters = command_data.get("parameters", {})
        
        if not command:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Command is required"
            )
        
        result = await client.send_module_command(module_id, command, parameters)
        
        if result.get("error"):
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=result.get("message", "Command failed")
            )
        
        return {
            "success": True,
            "data": result
        }
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Module command error: {str(e)}"
        )


# Safety Endpoints

@router.get("/safety/status")
async def get_fw_safety_status():
    """Get FW safety status"""
    try:
        client = await ensure_fw_connection()
        result = await client.get_safety_status()
        
        if result.get("error"):
            raise HTTPException(
                status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
                detail=result.get("message", "Safety status failed")
            )
        
        return {
            "success": True,
            "data": result
        }
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Safety status error: {str(e)}"
        )


@router.post("/safety/estop")
async def trigger_fw_emergency_stop(estop_data: Dict):
    """Trigger FW emergency stop"""
    try:
        client = await ensure_fw_connection()
        
        reason = estop_data.get("estop_reason", "Manual emergency stop")
        result = await client.trigger_emergency_stop(reason)
        
        if result.get("error"):
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=result.get("message", "E-Stop failed")
            )
        
        return {
            "success": True,
            "data": result
        }
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"E-Stop error: {str(e)}"
        )


# Configuration Endpoints

@router.get("/config")
async def get_fw_configuration():
    """Get FW configuration"""
    try:
        client = await ensure_fw_connection()
        result = await client.get_configuration()
        
        if result.get("error"):
            raise HTTPException(
                status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
                detail=result.get("message", "Configuration failed")
            )
        
        return {
            "success": True,
            "data": result
        }
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Configuration error: {str(e)}"
        )


@router.put("/config")
async def update_fw_configuration(config_data: Dict):
    """Update FW configuration"""
    try:
        client = await ensure_fw_connection()
        result = await client.update_configuration(config_data)
        
        if result.get("error"):
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=result.get("message", "Configuration update failed")
            )
        
        return {
            "success": True,
            "data": result
        }
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Configuration update error: {str(e)}"
        )


# Diagnostics Endpoints

@router.get("/diagnostics")
async def get_fw_diagnostics():
    """Get FW diagnostics"""
    try:
        client = await ensure_fw_connection()
        result = await client.get_diagnostics()
        
        if result.get("error"):
            raise HTTPException(
                status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
                detail=result.get("message", "Diagnostics failed")
            )
        
        return {
            "success": True,
            "data": result
        }
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Diagnostics error: {str(e)}"
        )


# WebSocket Endpoints

@router.post("/websocket/send")
async def send_fw_websocket_message(message_data: Dict):
    """Send message via FW WebSocket"""
    try:
        client = await ensure_fw_connection()
        
        message = message_data.get("message", "")
        if not message:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Message is required"
            )
        
        success = await client.send_websocket_message(message)
        
        if success:
            return {
                "success": True,
                "message": "Message sent successfully"
            }
        else:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Failed to send message"
            )
            
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"WebSocket send error: {str(e)}"
        )


@router.get("/websocket/receive")
async def receive_fw_websocket_message():
    """Receive message via FW WebSocket"""
    try:
        client = await ensure_fw_connection()
        
        message = await client.receive_websocket_message()
        
        if message:
            return {
                "success": True,
                "data": {
                    "message": message
                }
            }
        else:
            return {
                "success": False,
                "message": "No message received"
            }
            
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"WebSocket receive error: {str(e)}"
        )
