"""
RS485 API endpoints for OHT-50 Backend
API endpoints for RS485 module management and monitoring
"""

import logging
import asyncio
from datetime import datetime, timedelta
from typing import Dict, Any, List
from fastapi import APIRouter, Depends, HTTPException, status, Path, Query, Request

from app.core.security import require_permission
from app.models.user import User
from app.services.rs485_service import rs485_service
from app.schemas.rs485 import (
    RS485ModulesResponse, RS485ModuleResponse, RS485BusHealthResponse,
    RS485DiscoveryResponse, RS485DiscoveryResultsResponse, 
    RS485ModuleActionRequest, RS485ModuleActionResponse,
    RS485BusActionRequest, RS485BusActionResponse,
    RS485TelemetryResponse, RS485RegisterUpdateRequest, RS485RegisterUpdateResponse,
    RS485ScanControlRequest, RS485ScanStatusResponse, RS485ScanControlResponse
)

logger = logging.getLogger(__name__)

router = APIRouter(prefix="/api/v1/rs485", tags=["rs485"])


@router.get("/modules", response_model=RS485ModulesResponse)
async def get_rs485_modules(
    current_user: User = Depends(require_permission("monitoring", "read"))
):
    """Get list of all RS485 modules"""
    try:
        logger.info("📡 Getting RS485 modules list")
        
        modules = await rs485_service.get_modules()
        
        return RS485ModulesResponse(
            success=True,
            data=modules,
            message=f"Retrieved {len(modules)} RS485 modules successfully",
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get RS485 modules: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get RS485 modules: {str(e)}"
        )


@router.get("/modules/{address}", response_model=RS485ModuleResponse)
async def get_rs485_module(
    address: int = Path(..., description="Module address (1-15)", ge=1, le=15),
    current_user: User = Depends(require_permission("monitoring", "read"))
):
    """Get specific RS485 module by address"""
    try:
        logger.info(f"📡 Getting RS485 module 0x{address:02X}")
        
        module = await rs485_service.get_module(address)
        
        if not module:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail=f"RS485 module 0x{address:02X} not found"
            )
            
        return RS485ModuleResponse(
            success=True,
            data=module,
            message=f"Retrieved RS485 module 0x{address:02X} successfully",
            timestamp=datetime.utcnow()
        )
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Failed to get RS485 module 0x{address:02X}: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get RS485 module: {str(e)}"
        )


@router.get("/bus/health", response_model=RS485BusHealthResponse)
async def get_rs485_bus_health(
    current_user: User = Depends(require_permission("monitoring", "read"))
):
    """Get RS485 bus health status"""
    try:
        logger.info("📡 Getting RS485 bus health")
        
        health = await rs485_service.get_bus_health()
        
        return RS485BusHealthResponse(
            success=True,
            data=health,
            message="Retrieved RS485 bus health successfully",
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get RS485 bus health: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get RS485 bus health: {str(e)}"
        )


@router.post("/bus/restart", response_model=RS485BusActionResponse)
async def restart_rs485_bus(
    request: RS485BusActionRequest,
    current_user: User = Depends(require_permission("system", "control"))
):
    """Restart RS485 bus"""
    try:
        logger.info("🔄 Restarting RS485 bus")
        
        if request.action != "restart":
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Invalid action. Only 'restart' is supported."
            )
            
        result = await rs485_service.restart_bus()
        
        return RS485BusActionResponse(
            success=result.get("success", False),
            data=result,
            message=result.get("message", "RS485 bus restart completed"),
            timestamp=datetime.utcnow()
        )
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Failed to restart RS485 bus: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to restart RS485 bus: {str(e)}"
        )


@router.post("/discovery/start", response_model=RS485DiscoveryResponse)
async def start_rs485_discovery(
    current_user: User = Depends(require_permission("system", "control"))
):
    """Start RS485 module discovery"""
    try:
        logger.info("🔍 Starting RS485 module discovery")
        
        discovery_status = await rs485_service.start_discovery()
        
        return RS485DiscoveryResponse(
            success=True,
            data=discovery_status,
            message="RS485 discovery started successfully",
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to start RS485 discovery: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to start RS485 discovery: {str(e)}"
        )


@router.get("/discovery/status", response_model=RS485DiscoveryResponse)
async def get_rs485_discovery_status(
    current_user: User = Depends(require_permission("monitoring", "read"))
):
    """Get RS485 discovery status"""
    try:
        logger.info("📊 Getting RS485 discovery status")
        
        discovery_status = await rs485_service.get_discovery_status()
        
        return RS485DiscoveryResponse(
            success=True,
            data=discovery_status,
            message="Retrieved RS485 discovery status successfully",
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get RS485 discovery status: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get RS485 discovery status: {str(e)}"
        )


@router.get("/discovery/results", response_model=RS485DiscoveryResultsResponse)
async def get_rs485_discovery_results(
    current_user: User = Depends(require_permission("monitoring", "read"))
):
    """Get RS485 discovery results"""
    try:
        logger.info("📋 Getting RS485 discovery results")
        
        results = await rs485_service.get_discovery_results()
        
        return RS485DiscoveryResultsResponse(
            success=True,
            data=results,
            message=f"Retrieved {len(results)} discovery results successfully",
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get RS485 discovery results: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get RS485 discovery results: {str(e)}"
        )


@router.post("/modules/{address}/ping", response_model=RS485ModuleActionResponse)
async def ping_rs485_module(
    address: int = Path(..., description="Module address (1-15)", ge=1, le=15),
    current_user: User = Depends(require_permission("system", "control"))
):
    """Ping RS485 module"""
    try:
        logger.info(f"🏓 Pinging RS485 module 0x{address:02X}")
        
        result = await rs485_service.ping_module(address)
        
        return RS485ModuleActionResponse(
            success=result.get("success", False),
            data=result,
            message=result.get("message", f"Ping module 0x{address:02X} completed"),
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to ping RS485 module 0x{address:02X}: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to ping RS485 module: {str(e)}"
        )


@router.post("/modules/{address}/reset", response_model=RS485ModuleActionResponse)
async def reset_rs485_module(
    address: int = Path(..., description="Module address (1-15)", ge=1, le=15),
    current_user: User = Depends(require_permission("system", "control"))
):
    """Reset RS485 module"""
    try:
        logger.info(f"🔄 Resetting RS485 module 0x{address:02X}")
        
        result = await rs485_service.reset_module(address)
        
        return RS485ModuleActionResponse(
            success=result.get("success", False),
            data=result,
            message=result.get("message", f"Reset module 0x{address:02X} completed"),
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to reset RS485 module 0x{address:02X}: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to reset RS485 module: {str(e)}"
        )


@router.post("/modules/{address}/configure", response_model=RS485ModuleActionResponse)
async def configure_rs485_module(
    request: RS485ModuleActionRequest,
    address: int = Path(..., description="Module address (1-15)", ge=1, le=15),
    current_user: User = Depends(require_permission("system", "configure"))
):
    """Configure RS485 module"""
    try:
        logger.info(f"⚙️  Configuring RS485 module 0x{address:02X}")
        
        result = await rs485_service.configure_module(address, request.parameters)
        
        return RS485ModuleActionResponse(
            success=result.get("success", False),
            data=result,
            message=result.get("message", f"Configure module 0x{address:02X} completed"),
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to configure RS485 module 0x{address:02X}: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to configure RS485 module: {str(e)}"
        )


@router.post("/modules/{address}/quarantine", response_model=RS485ModuleActionResponse)
async def quarantine_rs485_module(
    address: int = Path(..., description="Module address (1-15)", ge=1, le=15),
    current_user: User = Depends(require_permission("system", "control"))
):
    """Quarantine RS485 module"""
    try:
        logger.warning(f"⚠️  Quarantining RS485 module 0x{address:02X}")
        
        result = await rs485_service.quarantine_module(address)
        
        return RS485ModuleActionResponse(
            success=result.get("success", False),
            data=result,
            message=result.get("message", f"Quarantine module 0x{address:02X} completed"),
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to quarantine RS485 module 0x{address:02X}: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to quarantine RS485 module: {str(e)}"
        )


# Health check endpoint for RS485 system
@router.get("/health")
async def get_rs485_health(
    current_user: User = Depends(require_permission("monitoring", "read"))
):
    """Get RS485 system health overview"""
    try:
        logger.info("🏥 Getting RS485 system health")
        
        # Get modules and bus health
        modules = await rs485_service.get_modules()
        bus_health = await rs485_service.get_bus_health()
        
        # Calculate health metrics
        total_modules = len(modules)
        healthy_modules = len([m for m in modules if m.status == "healthy"])
        warning_modules = len([m for m in modules if m.status == "warning"])
        error_modules = len([m for m in modules if m.status in ["error", "lost"]])
        
        health_score = (healthy_modules / total_modules * 100) if total_modules > 0 else 0
        
        return {
            "success": True,
            "data": {
                "overall_status": "healthy" if health_score > 80 else "warning" if health_score > 60 else "error",
                "health_score": round(health_score, 1),
                "total_modules": total_modules,
                "healthy_modules": healthy_modules,
                "warning_modules": warning_modules,
                "error_modules": error_modules,
                "bus_status": bus_health.status,
                "bus_error_rate": bus_health.error_rate,
                "bus_throughput": bus_health.throughput
            },
            "message": f"RS485 system health: {health_score:.1f}% ({healthy_modules}/{total_modules} modules healthy)",
            "timestamp": datetime.utcnow()
        }
        
    except Exception as e:
        logger.error(f"❌ Failed to get RS485 system health: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get RS485 system health: {str(e)}"
        )


# New Telemetry Endpoints for Issue #91
@router.get("/modules/{address}/telemetry", response_model=RS485TelemetryResponse)
async def get_module_telemetry(
    address: int = Path(..., description="Module address (1-15)", ge=1, le=15),
    current_user: User = Depends(require_permission("monitoring", "read"))
):
    """Get module telemetry table - Issue #91"""
    try:
        logger.info(f"📊 Getting telemetry for RS485 module 0x{address:02X}")
        
        telemetry = await rs485_service.get_module_telemetry(address)
        
        if not telemetry:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail=f"RS485 module 0x{address:02X} not found or no telemetry available"
            )
            
        return RS485TelemetryResponse(
            success=True,
            data=telemetry,
            message=f"Retrieved telemetry for module 0x{address:02X} ({telemetry.module_name}) with {len(telemetry.registers)} registers",
            timestamp=datetime.utcnow()
        )
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Failed to get telemetry for module 0x{address:02X}: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get module telemetry: {str(e)}"
        )


@router.post("/modules/{address}/telemetry", response_model=RS485RegisterUpdateResponse)
async def update_module_register(
    request: RS485RegisterUpdateRequest,
    address: int = Path(..., description="Module address (1-15)", ge=1, le=15),
    current_user: User = Depends(require_permission("system", "configure"))
):
    """Update module register (if writable) - Issue #91"""
    try:
        logger.info(f"✏️ Updating register {request.register_address} on module 0x{address:02X}")
        
        result = await rs485_service.update_module_register(
            address=address,
            register_address=request.register_address,
            value=request.value,
            force=request.force
        )
        
        # Broadcast register update via WebSocket
        try:
            from app.services.websocket_rs485_service import websocket_rs485_service
            await websocket_rs485_service.broadcast_register_update(
                module_addr=address,
                register_addr=request.register_address,
                update_result=result
            )
        except Exception as ws_error:
            logger.warning(f"⚠️ Failed to broadcast register update via WebSocket: {ws_error}")
        
        return RS485RegisterUpdateResponse(
            success=result.get("success", False),
            data=result,
            message=result.get("message", f"Register update completed"),
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to update register {request.register_address} on module 0x{address:02X}: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to update module register: {str(e)}"
        )


# Issue #147 - Module Scan Control Endpoints
@router.post("/modules/stop-scan", response_model=RS485ScanControlResponse)
async def stop_module_scan(
    request: RS485ScanControlRequest = None,
    current_user: User = Depends(require_permission("system", "control"))
):
    """Stop RS485 module scanning - Issue #147"""
    try:
        logger.info("🛑 Stopping RS485 module scan")
        
        # Call firmware service to stop scan
        result = await rs485_service.stop_module_scan(
            reason=request.reason if request else None
        )
        
        # Broadcast scan status change via WebSocket
        try:
            from app.services.websocket_rs485_service import websocket_rs485_service
            await websocket_rs485_service.broadcast_scan_status(
                status="stopped",
                reason=request.reason if request else "Manual stop via API",
                timestamp=datetime.utcnow()
            )
        except Exception as ws_error:
            logger.warning(f"⚠️ Failed to broadcast scan status via WebSocket: {ws_error}")
        
        return RS485ScanControlResponse(
            success=result.get("success", False),
            data=result,
            message=result.get("message", "Module scan stopped successfully"),
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to stop module scan: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to stop module scan: {str(e)}"
        )


@router.post("/modules/pause-scan", response_model=RS485ScanControlResponse)
async def pause_module_scan(
    request: RS485ScanControlRequest = None,
    current_user: User = Depends(require_permission("system", "control"))
):
    """Pause RS485 module scanning - Issue #147"""
    try:
        logger.info("⏸️ Pausing RS485 module scan")
        
        # Call firmware service to pause scan
        result = await rs485_service.pause_module_scan(
            reason=request.reason if request else None
        )
        
        # Broadcast scan status change via WebSocket
        try:
            from app.services.websocket_rs485_service import websocket_rs485_service
            await websocket_rs485_service.broadcast_scan_status(
                status="paused",
                reason=request.reason if request else "Manual pause via API",
                timestamp=datetime.utcnow()
            )
        except Exception as ws_error:
            logger.warning(f"⚠️ Failed to broadcast scan status via WebSocket: {ws_error}")
        
        return RS485ScanControlResponse(
            success=result.get("success", False),
            data=result,
            message=result.get("message", "Module scan paused successfully"),
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to pause module scan: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to pause module scan: {str(e)}"
        )


@router.post("/modules/resume-scan", response_model=RS485ScanControlResponse)
async def resume_module_scan(
    request: RS485ScanControlRequest = None,
    current_user: User = Depends(require_permission("system", "control"))
):
    """Resume RS485 module scanning - Issue #147"""
    try:
        logger.info("▶️ Resuming RS485 module scan")
        
        # Call firmware service to resume scan
        result = await rs485_service.resume_module_scan(
            reason=request.reason if request else None
        )
        
        # Broadcast scan status change via WebSocket
        try:
            from app.services.websocket_rs485_service import websocket_rs485_service
            await websocket_rs485_service.broadcast_scan_status(
                status="running",
                reason=request.reason if request else "Manual resume via API",
                timestamp=datetime.utcnow()
            )
        except Exception as ws_error:
            logger.warning(f"⚠️ Failed to broadcast scan status via WebSocket: {ws_error}")
        
        return RS485ScanControlResponse(
            success=result.get("success", False),
            data=result,
            message=result.get("message", "Module scan resumed successfully"),
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to resume module scan: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to resume module scan: {str(e)}"
        )




@router.get("/scan-status", response_model=RS485ScanStatusResponse)
async def get_module_scan_status(
    current_user: User = Depends(require_permission("monitoring", "read"))
):
    """Get current RS485 module scan status - Issue #147"""
    try:
        logger.info("📊 Getting RS485 module scan status")
        
        # Get scan status from firmware service
        status_data = await rs485_service.get_module_scan_status()
        
        return RS485ScanStatusResponse(
            success=True,
            data=status_data,
            message="Retrieved module scan status successfully",
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get module scan status: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get module scan status: {str(e)}"
        )


@router.post("/modules/start-scan", response_model=RS485ScanControlResponse)
async def start_module_scan(
    request: RS485ScanControlRequest,
    current_user: User = Depends(require_permission("system", "control"))
):
    """Start RS485 module scan - Issue #147"""
    try:
        logger.info("🚀 Starting RS485 module scan")
        
        # Start module scan via firmware service
        result = await rs485_service.start_module_scan(request.reason)
        
        return RS485ScanControlResponse(
            success=result.get("success", False),
            data=result.get("data", {}),
            message=result.get("message", "Module scan started successfully"),
            timestamp=datetime.utcnow()
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to start module scan: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to start module scan: {str(e)}"
        )

