"""
Communication API Endpoints - OHT-50 Backend

This module provides communication-specific API endpoints for
managing communication channels, protocols, and status.

Features:
- Communication status monitoring
- Protocol management
- Channel configuration
- Message routing
- Connection diagnostics
"""

import logging
from typing import Dict, Any, List, Optional
from datetime import datetime, timezone

from fastapi import APIRouter, HTTPException, Depends, Query
from fastapi.responses import JSONResponse

from app.core.security import get_current_user
from app.services.firmware_integration_service import firmware_service, FirmwareIntegrationService
from app.models.user import User

logger = logging.getLogger(__name__)

# Create router
router = APIRouter(prefix="/communication", tags=["Communication"])


# Dependency to get firmware service
async def get_firmware_service() -> FirmwareIntegrationService:
    """Get firmware service instance"""
    return firmware_service


@router.get("/status")
async def get_communication_status(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get communication status
    
    Returns overall communication system status including
    firmware connection, protocols, and channels.
    """
    try:
        # Get firmware connection status
        fw_status = fw_service.get_connection_status()
        
        # Get system health
        system_health = await fw_service.get_system_health()
        
        # Get modules for communication channels
        modules = await fw_service.get_modules()
        
        communication_status = {
            "success": True,
            "data": {
                "overall_status": "healthy" if fw_status["status"] == "connected" else "degraded",
                "firmware_connection": fw_status,
                "system_health": system_health,
                "communication_channels": {
                    "http_api": {
                        "status": "active" if fw_status["status"] == "connected" else "inactive",
                        "protocol": "HTTP/1.1",
                        "port": 8080,
                        "endpoints": len(modules) if modules else 0
                    },
                    "websocket": {
                        "status": "active" if fw_status["status"] == "connected" else "inactive",
                        "protocol": "WebSocket",
                        "port": 8081,
                        "real_time": True
                    },
                    "rs485": {
                        "status": "active" if modules else "inactive",
                        "protocol": "RS485/Modbus",
                        "modules": len(modules) if modules else 0,
                        "baud_rate": "115200"
                    }
                },
                "modules": modules,
                "last_heartbeat": fw_status.get("last_heartbeat"),
                "error_count": fw_status.get("connection_errors", 0)
            },
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        return JSONResponse(
            status_code=200,
            content=communication_status
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get communication status: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get communication status: {str(e)}"
        )


@router.get("/protocols")
async def get_communication_protocols(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get available communication protocols
    
    Returns list of supported communication protocols
    and their current status.
    """
    try:
        protocols = [
            {
                "name": "HTTP REST API",
                "type": "request_response",
                "status": "active",
                "port": 8080,
                "description": "Primary API communication protocol",
                "features": ["CRUD operations", "JSON data format", "RESTful endpoints"]
            },
            {
                "name": "WebSocket",
                "type": "real_time",
                "status": "active",
                "port": 8081,
                "description": "Real-time bidirectional communication",
                "features": ["Telemetry streaming", "Event notifications", "Low latency"]
            },
            {
                "name": "RS485 Modbus",
                "type": "serial",
                "status": "active",
                "port": "/dev/ttyOHT485",
                "description": "Hardware module communication",
                "features": ["Multi-module support", "Industrial protocol", "Reliable communication"]
            }
        ]
        
        # Update status based on actual connection
        fw_status = fw_service.get_connection_status()
        is_connected = fw_status["status"] == "connected"
        
        for protocol in protocols:
            if protocol["name"] in ["HTTP REST API", "WebSocket"]:
                protocol["status"] = "active" if is_connected else "inactive"
        
        return JSONResponse(
            status_code=200,
            content={
                "success": True,
                "data": {
                    "protocols": protocols,
                    "total_count": len(protocols),
                    "active_count": len([p for p in protocols if p["status"] == "active"])
                },
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get communication protocols: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get communication protocols: {str(e)}"
        )


@router.get("/channels")
async def get_communication_channels(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get communication channels
    
    Returns available communication channels and their status.
    """
    try:
        # Get modules as communication channels
        modules = await fw_service.get_modules()
        
        channels = []
        
        # Add firmware channels
        fw_status = fw_service.get_connection_status()
        is_connected = fw_status["status"] == "connected"
        
        channels.append({
            "id": "fw_http",
            "name": "Firmware HTTP API",
            "type": "http",
            "status": "active" if is_connected else "inactive",
            "endpoint": "http://localhost:8080",
            "description": "Primary firmware communication channel"
        })
        
        channels.append({
            "id": "fw_websocket",
            "name": "Firmware WebSocket",
            "type": "websocket",
            "status": "active" if is_connected else "inactive",
            "endpoint": "ws://localhost:8081/ws",
            "description": "Real-time firmware communication channel"
        })
        
        # Add module channels
        for module in modules:
            if isinstance(module, dict):
                module_id = module.get("id", "unknown")
                channels.append({
                    "id": f"module_{module_id}",
                    "name": f"Module {module_id}",
                    "type": "rs485",
                    "status": module.get("status", "unknown"),
                    "endpoint": f"RS485 Module {module_id}",
                    "description": f"RS485 communication channel for module {module_id}"
                })
        
        return JSONResponse(
            status_code=200,
            content={
                "success": True,
                "data": {
                    "channels": channels,
                    "total_count": len(channels),
                    "active_count": len([c for c in channels if c["status"] == "active"])
                },
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get communication channels: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get communication channels: {str(e)}"
        )


@router.get("/diagnostics")
async def get_communication_diagnostics(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get communication diagnostics
    
    Returns detailed communication system diagnostics
    including performance metrics and error information.
    """
    try:
        # Get firmware diagnostics
        fw_diagnostics = await fw_service.get_diagnostics()
        
        # Get connection status
        fw_status = fw_service.get_connection_status()
        
        # Get modules
        modules = await fw_service.get_modules()
        
        diagnostics = {
            "success": True,
            "data": {
                "system_overview": {
                    "status": fw_status["status"],
                    "uptime": "unknown",  # Would need to track this
                    "last_heartbeat": fw_status.get("last_heartbeat"),
                    "error_count": fw_status.get("connection_errors", 0)
                },
                "performance_metrics": {
                    "response_time_ms": fw_status.get("response_time_ms", 0),
                    "sensor_cache_size": fw_status.get("sensor_cache_size", 0),
                    "active_connections": 1 if fw_status["status"] == "connected" else 0
                },
                "communication_protocols": {
                    "http": {
                        "status": "active" if fw_status["status"] == "connected" else "inactive",
                        "port": 8080,
                        "timeout": "5.0s"
                    },
                    "websocket": {
                        "status": "active" if fw_status["status"] == "connected" else "inactive",
                        "port": 8081,
                        "real_time": True
                    },
                    "rs485": {
                        "status": "active" if modules else "inactive",
                        "modules_count": len(modules) if modules else 0,
                        "baud_rate": "115200"
                    }
                },
                "module_status": modules if modules else [],
                "firmware_diagnostics": fw_diagnostics.get("data", {}) if fw_diagnostics.get("success") else {},
                "recommendations": []
            },
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        # Add recommendations based on status
        if fw_status["status"] != "connected":
            diagnostics["data"]["recommendations"].append({
                "type": "warning",
                "message": "Firmware connection is not active. Check firmware service status.",
                "action": "Verify firmware is running on ports 8080/8081"
            })
        
        if fw_status.get("connection_errors", 0) > 5:
            diagnostics["data"]["recommendations"].append({
                "type": "error",
                "message": "High error count detected. Communication may be unstable.",
                "action": "Check network connectivity and firmware health"
            })
        
        return JSONResponse(
            status_code=200,
            content=diagnostics
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get communication diagnostics: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get communication diagnostics: {str(e)}"
        )


@router.post("/test")
async def test_communication(
    test_type: str = Query(..., description="Type of communication test", pattern="^(ping|health|modules|full)$"),
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Test communication system
    
    Performs communication tests to verify system health.
    """
    try:
        logger.info(f"🧪 User {current_user.username} running communication test: {test_type}")
        
        test_results = {
            "success": True,
            "test_type": test_type,
            "results": {},
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        if test_type in ["ping", "health", "full"]:
            # Test firmware health
            health_result = await fw_service.get_system_health()
            test_results["results"]["health_check"] = {
                "status": "passed" if health_result.get("success") else "failed",
                "response": health_result
            }
        
        if test_type in ["modules", "full"]:
            # Test module communication
            modules = await fw_service.get_modules()
            test_results["results"]["module_check"] = {
                "status": "passed" if modules else "failed",
                "modules_found": len(modules) if modules else 0,
                "modules": modules
            }
        
        if test_type == "full":
            # Test all communication channels
            fw_status = fw_service.get_connection_status()
            test_results["results"]["connection_check"] = {
                "status": "passed" if fw_status["status"] == "connected" else "failed",
                "connection_status": fw_status
            }
            
            # Test telemetry
            try:
                telemetry = await fw_service.get_telemetry_data()
                test_results["results"]["telemetry_check"] = {
                    "status": "passed" if telemetry.get("success") else "failed",
                    "telemetry_data": telemetry
                }
            except Exception as e:
                test_results["results"]["telemetry_check"] = {
                    "status": "failed",
                    "error": str(e)
                }
        
        # Determine overall test status
        all_passed = all(
            result.get("status") == "passed" 
            for result in test_results["results"].values()
        )
        
        test_results["overall_status"] = "passed" if all_passed else "failed"
        
        return JSONResponse(
            status_code=200,
            content=test_results
        )
        
    except Exception as e:
        logger.error(f"❌ Communication test failed: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Communication test failed: {str(e)}"
        )


@router.get("/metrics")
async def get_communication_metrics(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get communication metrics
    
    Returns communication performance metrics and statistics.
    """
    try:
        fw_status = fw_service.get_connection_status()
        modules = await fw_service.get_modules()
        
        metrics = {
            "success": True,
            "data": {
                "connection_metrics": {
                    "status": fw_status["status"],
                    "uptime_seconds": 0,  # Would need to track this
                    "error_count": fw_status.get("connection_errors", 0),
                    "last_heartbeat": fw_status.get("last_heartbeat")
                },
                "performance_metrics": {
                    "response_time_ms": fw_status.get("response_time_ms", 0),
                    "sensor_cache_size": fw_status.get("sensor_cache_size", 0),
                    "active_connections": 1 if fw_status["status"] == "connected" else 0
                },
                "protocol_metrics": {
                    "http_requests": 0,  # Would need to track this
                    "websocket_messages": 0,  # Would need to track this
                    "rs485_messages": 0  # Would need to track this
                },
                "module_metrics": {
                    "total_modules": len(modules) if modules else 0,
                    "active_modules": len([m for m in modules if m.get("status") == "active"]) if modules else 0,
                    "error_modules": len([m for m in modules if m.get("status") == "error"]) if modules else 0
                }
            },
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        return JSONResponse(
            status_code=200,
            content=metrics
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get communication metrics: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get communication metrics: {str(e)}"
        )
