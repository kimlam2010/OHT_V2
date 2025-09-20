"""
FW Integration API Endpoints - OHT-50 Backend

This module provides REST API endpoints for firmware integration,
acting as a proxy between frontend and firmware services.

Features:
- Connection management endpoints
- System status and health endpoints
- Module control endpoints
- Safety control endpoints
- Configuration management endpoints
- Diagnostics endpoints
"""

import logging
from typing import Dict, Any, List, Optional
from datetime import datetime, timezone

from fastapi import APIRouter, HTTPException, Depends, BackgroundTasks
from fastapi.responses import JSONResponse

from app.core.security import get_current_user
from app.services.firmware_integration_service import firmware_service, FirmwareIntegrationService
from app.schemas.user import User

logger = logging.getLogger(__name__)

# Create router
router = APIRouter(prefix="/fw", tags=["Firmware Integration"])


# Dependency to get firmware service
async def get_firmware_service() -> FirmwareIntegrationService:
    """Get firmware service instance"""
    return firmware_service


# Connection Management Endpoints

@router.post("/connect")
async def connect_to_firmware(
    background_tasks: BackgroundTasks,
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Connect to firmware
    
    Establishes connection to the firmware system via HTTP and WebSocket.
    """
    try:
        logger.info(f"🔌 User {current_user.username} requesting firmware connection")
        
        # Initialize firmware service
        success = await fw_service.initialize()
        
        if success:
            # Start background monitoring in background
            background_tasks.add_task(fw_service._start_background_tasks)
            
            return JSONResponse(
                status_code=200,
                content={
                    "success": True,
                    "message": "Successfully connected to firmware",
                    "firmware_url": fw_service.firmware_url,
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        else:
            return JSONResponse(
                status_code=503,
                content={
                    "success": False,
                    "error": "Failed to connect to firmware",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
            
    except Exception as e:
        logger.error(f"❌ Firmware connection error: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Firmware connection failed: {str(e)}"
        )


@router.post("/disconnect")
async def disconnect_from_firmware(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Disconnect from firmware
    
    Closes connection to the firmware system.
    """
    try:
        logger.info(f"🔌 User {current_user.username} requesting firmware disconnection")
        
        await fw_service.shutdown()
        
        return JSONResponse(
            status_code=200,
            content={
                "success": True,
                "message": "Successfully disconnected from firmware",
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
        )
        
    except Exception as e:
        logger.error(f"❌ Firmware disconnection error: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Firmware disconnection failed: {str(e)}"
        )


@router.get("/status")
async def get_firmware_status(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get firmware connection status
    
    Returns current connection status and health information.
    """
    try:
        status = fw_service.get_connection_status()
        
        return JSONResponse(
            status_code=200,
            content={
                "success": True,
                "data": status,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get firmware status: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get firmware status: {str(e)}"
        )


# System Endpoints

@router.get("/system/status")
async def get_system_status(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get firmware system status
    
    Returns detailed system status information from firmware.
    """
    try:
        status = await fw_service.get_system_status()
        
        return JSONResponse(
            status_code=200,
            content=status
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get system status: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get system status: {str(e)}"
        )


@router.get("/system/health")
async def get_system_health(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get firmware system health
    
    Returns system health check information.
    """
    try:
        health = await fw_service.get_system_health()
        
        return JSONResponse(
            status_code=200,
            content=health
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get system health: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get system health: {str(e)}"
        )


# Module Endpoints

@router.get("/modules")
async def get_modules(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get list of firmware modules
    
    Returns list of all available modules and their status.
    """
    try:
        modules = await fw_service.get_modules()
        
        return JSONResponse(
            status_code=200,
            content={
                "success": True,
                "data": modules,
                "count": len(modules),
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get modules: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get modules: {str(e)}"
        )


@router.get("/modules/{module_id}")
async def get_module_info(
    module_id: int,
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get module information
    
    Returns detailed information about a specific module.
    """
    try:
        info = await fw_service.get_module_info(module_id)
        
        if info.get("success", False):
            return JSONResponse(
                status_code=200,
                content=info
            )
        else:
            return JSONResponse(
                status_code=404,
                content={
                    "success": False,
                    "error": f"Module {module_id} not found",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        
    except Exception as e:
        logger.error(f"❌ Failed to get module {module_id} info: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get module info: {str(e)}"
        )


@router.post("/modules/{module_id}/command")
async def send_module_command(
    module_id: int,
    command_data: Dict[str, Any],
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Send command to module
    
    Sends a command to a specific firmware module.
    """
    try:
        command = command_data.get("command")
        parameters = command_data.get("parameters", {})
        
        if not command:
            raise HTTPException(
                status_code=400,
                detail="Command is required"
            )
        
        logger.info(f"📤 User {current_user.username} sending command '{command}' to module {module_id}")
        
        success = await fw_service.send_module_command(module_id, command, parameters)
        
        if success:
            return JSONResponse(
                status_code=200,
                content={
                    "success": True,
                    "message": f"Command '{command}' sent to module {module_id}",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        else:
            return JSONResponse(
                status_code=400,
                content={
                    "success": False,
                    "error": f"Failed to send command to module {module_id}",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Failed to send command to module {module_id}: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to send module command: {str(e)}"
        )


# Safety Endpoints

@router.get("/safety/status")
async def get_safety_status(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get safety status
    
    Returns current safety system status.
    """
    try:
        status = await fw_service.get_safety_status()
        
        return JSONResponse(
            status_code=200,
            content=status
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get safety status: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get safety status: {str(e)}"
        )


@router.post("/safety/emergency-stop")
async def emergency_stop(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Trigger emergency stop
    
    Immediately stops all robot operations for safety.
    """
    try:
        logger.warning(f"🚨 User {current_user.username} triggered emergency stop")
        
        success = await fw_service.emergency_stop()
        
        if success:
            return JSONResponse(
                status_code=200,
                content={
                    "success": True,
                    "message": "Emergency stop executed successfully",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        else:
            return JSONResponse(
                status_code=500,
                content={
                    "success": False,
                    "error": "Failed to execute emergency stop",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        
    except Exception as e:
        logger.error(f"❌ Emergency stop failed: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Emergency stop failed: {str(e)}"
        )


# Configuration Endpoints

@router.get("/config")
async def get_configuration(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get firmware configuration
    
    Returns current firmware configuration.
    """
    try:
        config = await fw_service.get_configuration()
        
        return JSONResponse(
            status_code=200,
            content=config
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get configuration: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get configuration: {str(e)}"
        )


@router.put("/config")
async def update_configuration(
    config_data: Dict[str, Any],
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Update firmware configuration
    
    Updates firmware configuration with new values.
    """
    try:
        logger.info(f"⚙️ User {current_user.username} updating firmware configuration")
        
        success = await fw_service.update_configuration(config_data)
        
        if success:
            return JSONResponse(
                status_code=200,
                content={
                    "success": True,
                    "message": "Configuration updated successfully",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        else:
            return JSONResponse(
                status_code=400,
                content={
                    "success": False,
                    "error": "Failed to update configuration",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        
    except Exception as e:
        logger.error(f"❌ Failed to update configuration: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to update configuration: {str(e)}"
        )


# Diagnostics Endpoints

@router.get("/diagnostics")
async def get_diagnostics(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get system diagnostics
    
    Returns comprehensive system diagnostics information.
    """
    try:
        diagnostics = await fw_service.get_diagnostics()
        
        return JSONResponse(
            status_code=200,
            content=diagnostics
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get diagnostics: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get diagnostics: {str(e)}"
        )


# Robot Control Endpoints

@router.get("/robot/status")
async def get_robot_status(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get robot status
    
    Returns current robot status and telemetry data.
    """
    try:
        status = await fw_service.get_robot_status()
        
        if status:
            return JSONResponse(
                status_code=200,
                content={
                    "success": True,
                    "data": status,
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        else:
            return JSONResponse(
                status_code=503,
                content={
                    "success": False,
                    "error": "Robot status unavailable",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        
    except Exception as e:
        logger.error(f"❌ Failed to get robot status: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get robot status: {str(e)}"
        )


@router.post("/robot/command")
async def send_robot_command(
    command_data: Dict[str, Any],
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Send robot command
    
    Sends a command to the robot system.
    """
    try:
        command_type = command_data.get("command_type")
        
        if not command_type:
            raise HTTPException(
                status_code=400,
                detail="command_type is required"
            )
        
        logger.info(f"🤖 User {current_user.username} sending robot command: {command_type}")
        
        success = await fw_service.send_robot_command(command_data)
        
        if success:
            return JSONResponse(
                status_code=200,
                content={
                    "success": True,
                    "message": f"Robot command '{command_type}' sent successfully",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        else:
            return JSONResponse(
                status_code=400,
                content={
                    "success": False,
                    "error": f"Failed to send robot command '{command_type}'",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Failed to send robot command: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to send robot command: {str(e)}"
        )


# Telemetry Endpoints

@router.get("/telemetry")
async def get_telemetry_data(
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get telemetry data
    
    Returns current telemetry data from sensors and robot.
    """
    try:
        telemetry = await fw_service.get_telemetry_data()
        
        return JSONResponse(
            status_code=200,
            content=telemetry
        )
        
    except Exception as e:
        logger.error(f"❌ Failed to get telemetry data: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get telemetry data: {str(e)}"
        )


@router.get("/sensors/{sensor_type}")
async def get_sensor_data(
    sensor_type: str,
    sensor_id: Optional[str] = None,
    current_user: User = Depends(get_current_user),
    fw_service: FirmwareIntegrationService = Depends(get_firmware_service)
):
    """
    Get sensor data
    
    Returns data from a specific sensor type.
    """
    try:
        sensor_data = await fw_service.get_sensor_data(sensor_type, sensor_id)
        
        if sensor_data:
            return JSONResponse(
                status_code=200,
                content={
                    "success": True,
                    "data": sensor_data,
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        else:
            return JSONResponse(
                status_code=404,
                content={
                    "success": False,
                    "error": f"Sensor data for '{sensor_type}' not found",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                }
            )
        
    except Exception as e:
        logger.error(f"❌ Failed to get sensor data for {sensor_type}: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get sensor data: {str(e)}"
        )
