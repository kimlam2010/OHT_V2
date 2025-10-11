"""
FW Integration API Endpoints - OHT-50 Backend

This module provides REST API endpoints for firmware integration,
acting as a proxy between frontend and firmware services.
"""

import logging
from typing import Dict, Any, List, Optional
from datetime import datetime, timezone

from fastapi import APIRouter, HTTPException, Depends, BackgroundTasks
from fastapi.responses import JSONResponse

from app.core.security import get_current_user, require_permission
from app.services.unified_firmware_service import UnifiedFirmwareService, get_firmware_service
from app.services.firmware_integration_service import MockFirmwareService
from app.models.user import User
import os

logger = logging.getLogger(__name__)

# Create router
# Prefix needed because main.py includes this router directly (not through v1/__init__.py)
router = APIRouter(prefix="/api/v1", tags=["Firmware Integration"])


# Global mock service instance
_mock_service = None

# Dependency to get firmware service (with mock support)
async def _get_fw_service():
    """Get firmware service - returns Mock wrapper if USE_MOCK_FIRMWARE=true"""
    use_mock = os.getenv("USE_MOCK_FIRMWARE", "false").lower() == "true"
    
    if use_mock:
        global _mock_service
        if _mock_service is None:
            logger.warning("🧪 MOCK MODE: Using MockFirmwareService for fw_integration endpoints")
            _mock_service = MockFirmwareService()
        return _mock_service
    
    return await get_firmware_service()


@router.post("/connect")
async def connect_to_firmware(
    background_tasks: BackgroundTasks,
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    try:
        logger.info(f"🔌 User {current_user.username} requesting firmware connection")
        # No explicit connect; just probe once
        resp = await fw_service.get_robot_status()
        ok = resp.success
        if ok:
            return JSONResponse(status_code=200, content={
                "success": True,
                "message": "Successfully connected to firmware",
                "firmware_url": fw_service.firmware_url,
                "timestamp": datetime.now(timezone.utc).isoformat()
            })
        return JSONResponse(status_code=503, content={
            "success": False,
            "error": resp.error or "Failed to connect to firmware",
            "timestamp": datetime.now(timezone.utc).isoformat()
        })
    except Exception as e:
        logger.error(f"❌ Firmware connection error: {e}")
        raise HTTPException(status_code=500, detail=f"Firmware connection failed: {str(e)}")


@router.post("/disconnect")
async def disconnect_from_firmware(
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    try:
        await fw_service.close()
        return JSONResponse(status_code=200, content={
            "success": True,
            "message": "Successfully disconnected from firmware",
            "timestamp": datetime.now(timezone.utc).isoformat()
        })
    except Exception as e:
        logger.error(f"❌ Firmware disconnection error: {e}")
        raise HTTPException(status_code=500, detail=f"Firmware disconnection failed: {str(e)}")


@router.get("/status")
async def get_firmware_status(
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    try:
        return JSONResponse(status_code=200, content={
            "success": True,
            "data": fw_service.get_health_status(),
            "timestamp": datetime.now(timezone.utc).isoformat()
        })
    except Exception as e:
        logger.error(f"❌ Failed to get firmware status: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to get firmware status: {str(e)}")


@router.get("/system/status")
async def get_system_status(
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    try:
        resp = await fw_service.get_robot_status()
        return JSONResponse(status_code=200, content={"success": resp.success, "data": resp.data, "error": resp.error})
    except Exception as e:
        logger.error(f"❌ Failed to get system status: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to get system status: {str(e)}")


@router.get("/system/health")
async def get_system_health(
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    try:
        return JSONResponse(status_code=200, content=fw_service.get_health_status())
    except Exception as e:
        logger.error(f"❌ Failed to get system health: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to get system health: {str(e)}")


@router.get("/modules")
async def get_modules(
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    try:
        resp = await fw_service.get_modules_status()
        modules = resp.data.get("modules", []) if resp.success and resp.data else []
        return JSONResponse(status_code=200, content={
            "success": True,
            "data": modules,
            "count": len(modules),
            "timestamp": datetime.now(timezone.utc).isoformat()
        })
    except Exception as e:
        logger.error(f"❌ Failed to get modules: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to get modules: {str(e)}")


@router.get("/modules/{module_id}")
async def get_module_info(
    module_id: int,
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    """
    Get module information
    
    Returns detailed information about a specific module.
    """
    try:
        resp = await fw_service.get_module_info(module_id)
        if resp.success:
            return JSONResponse(status_code=200, content=resp.data)
        return JSONResponse(status_code=404, content={
            "success": False,
            "error": f"Module {module_id} not found",
            "timestamp": datetime.now(timezone.utc).isoformat()
        })
    except Exception as e:
        logger.error(f"❌ Failed to get module {module_id} info: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to get module info: {str(e)}")


@router.post("/modules/{module_id}/command")
async def send_module_command(
    module_id: int,
    command_data: Dict[str, Any],
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
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
        
        resp = await fw_service.send_module_command(module_id, command, parameters)
        
        if resp.success:
            return JSONResponse(status_code=200, content={
                "success": True,
                "message": f"Command '{command}' sent to module {module_id}",
                "timestamp": datetime.now(timezone.utc).isoformat()
            })
        return JSONResponse(status_code=400, content={
            "success": False,
            "error": f"Failed to send command to module {module_id}",
            "timestamp": datetime.now(timezone.utc).isoformat()
        })
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Failed to send command to module {module_id}: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to send module command: {str(e)}")


@router.get("/modules/{module_id}/registers")
async def get_module_registers(
    module_id: int,
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    """
    🆕 Get module register data with address, mode, and cached values
    
    **Issue #176:** Expose Firmware API v3.1.0 register data
    
    **Returns:**
    ```json
    {
      "success": true,
      "data": {
        "module_addr": 2,
        "module_name": "Power Module",
        "online": true,
        "register_count": 58,
        "registers": [
          {
            "address": "0x0000",
            "name": "Battery Pack Voltage",
            "mode": "READ",
            "data_type": "UINT16",
            "description": "Battery Pack Voltage",
            "value": 24400,
            "timestamp": "2025-10-10T10:30:45.123Z"
          }
        ]
      }
    }
    ```
    
    **Auth:** Requires user login (monitoring permission)
    
    **Firmware API:** GET /api/v1/modules/{id}/data (v3.1.0)
    """
    try:
        logger.info(f"📊 User {current_user.username} getting registers for module {module_id}")
        
        # Call service (Mock or Real)
        resp = await fw_service.get_module_registers(module_id)
        
        # Handle Mock response (dict) vs Real response (FirmwareResponse)
        if isinstance(resp, dict):
            # Mock service returns dict directly
            if resp.get("success"):
                logger.info(f"✅ Successfully retrieved registers for module {module_id} (MOCK)")
                return JSONResponse(status_code=200, content=resp)
            else:
                logger.warning(f"⚠️ Module {module_id} offline or not found (MOCK)")
                return JSONResponse(status_code=404, content=resp)
        else:
            # Real service returns FirmwareResponse
            if resp.success:
                logger.info(f"✅ Successfully retrieved registers for module {module_id}")
                return JSONResponse(status_code=200, content=resp.data)
            
            # Module offline or not found
            logger.warning(f"⚠️ Module {module_id} offline or not found: {resp.error}")
            return JSONResponse(status_code=404, content={
                "success": False,
                "error": f"Module {module_id} offline or not found",
                "detail": resp.error,
                "timestamp": datetime.now(timezone.utc).isoformat()
            })
        
    except Exception as e:
        logger.error(f"❌ Failed to get registers for module {module_id}: {e}")
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get module registers: {str(e)}"
        )


@router.post("/modules/{module_id}/registers/{reg_addr}")
async def write_module_register(
    module_id: int,
    reg_addr: str,
    value_data: Dict[str, Any],
    current_user: User = Depends(require_permission("control", "write")),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    """
    🆕 Write value to module register
    
    **Issue #176:** Write register control
    
    **Auth:** 🔒 Requires ADMIN permission (control:write)
    
    **Request Body:**
    ```json
    {
      "value": 1
    }
    ```
    
    **Response (Success):**
    ```json
    {
      "success": true,
      "register": "0x0049",
      "value": 1,
      "message": "Register written successfully"
    }
    ```
    
    **Validation:**
    - Register must exist
    - Register mode must be WRITE or READ_WRITE
    - Value must be within min/max bounds
    
    **Firmware API:** POST /api/v1/modules/{id}/registers/{addr} (v3.1.0)
    """
    try:
        value = value_data.get("value")
        if value is None:
            raise HTTPException(status_code=400, detail="Value is required in request body")
        
        logger.info(f"📝 User {current_user.username} writing register {reg_addr} on module {module_id}: value={value}")
        
        # Call service (Mock or Real)
        resp = await fw_service.write_module_register(module_id, reg_addr, value)
        
        # Handle Mock response (dict) vs Real response (FirmwareResponse)
        if isinstance(resp, dict):
            # Mock service returns dict directly
            if resp.get("success"):
                logger.info(f"✅ Successfully wrote register {reg_addr} on module {module_id} (MOCK)")
                return JSONResponse(status_code=200, content=resp)
            else:
                logger.warning(f"⚠️ Failed to write register {reg_addr} (MOCK): {resp.get('error')}")
                return JSONResponse(status_code=400, content=resp)
        else:
            # Real service returns FirmwareResponse
            if resp.success:
                logger.info(f"✅ Successfully wrote register {reg_addr} on module {module_id}")
                return JSONResponse(status_code=200, content={
                    "success": True,
                    "register": reg_addr,
                    "value": value,
                    "message": "Register written successfully",
                    "timestamp": datetime.now(timezone.utc).isoformat()
                })
            
            # Write failed (validation error, read-only register, etc.)
            logger.warning(f"⚠️ Failed to write register {reg_addr}: {resp.error}")
            return JSONResponse(status_code=400, content={
                "success": False,
                "error": resp.error or "Write failed",
                "register": reg_addr,
                "timestamp": datetime.now(timezone.utc).isoformat()
            })
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Write register {reg_addr} failed: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to write register: {str(e)}")


@router.get("/safety/status")
async def get_safety_status(
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    try:
        resp = await fw_service.get_safety_status()
        return JSONResponse(status_code=200, content=resp.data or {"status": "unknown", "error": resp.error})
    except Exception as e:
        logger.error(f"❌ Failed to get safety status: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to get safety status: {str(e)}")


@router.post("/safety/emergency-stop")
async def emergency_stop(
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    try:
        resp = await fw_service.emergency_stop()
        if resp.success:
            return JSONResponse(status_code=200, content={
                "success": True,
                "message": "Emergency stop executed successfully",
                "timestamp": datetime.now(timezone.utc).isoformat()
            })
        return JSONResponse(status_code=500, content={
            "success": False,
            "error": resp.error or "Failed to execute emergency stop",
            "timestamp": datetime.now(timezone.utc).isoformat()
        })
    except Exception as e:
        logger.error(f"❌ Emergency stop failed: {e}")
        raise HTTPException(status_code=500, detail=f"Emergency stop failed: {str(e)}")


@router.get("/config")
async def get_configuration(
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    """
    Get firmware configuration
    
    Returns current firmware configuration.
    """
    try:
        resp = await fw_service.get_configuration()
        return JSONResponse(status_code=200, content=resp.data or {"error": resp.error})
    except Exception as e:
        logger.error(f"❌ Failed to get configuration: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to get configuration: {str(e)}")


@router.put("/config")
async def update_configuration(
    config_data: Dict[str, Any],
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    """
    Update firmware configuration
    
    Updates firmware configuration with new values.
    """
    try:
        logger.info(f"⚙️ User {current_user.username} updating firmware configuration")
        
        resp = await fw_service.update_configuration(config_data)
        
        if resp.success:
            return JSONResponse(status_code=200, content={
                "success": True,
                "message": "Configuration updated successfully",
                "timestamp": datetime.now(timezone.utc).isoformat()
            })
        return JSONResponse(status_code=400, content={
            "success": False,
            "error": "Failed to update configuration",
            "timestamp": datetime.now(timezone.utc).isoformat()
        })
    except Exception as e:
        logger.error(f"❌ Failed to update configuration: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to update configuration: {str(e)}")


@router.get("/diagnostics")
async def get_diagnostics(
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    """
    Get system diagnostics
    
    Returns comprehensive system diagnostics information.
    """
    try:
        resp = await fw_service.get_diagnostics()
        return JSONResponse(status_code=200, content=resp.data or {"error": resp.error})
    except Exception as e:
        logger.error(f"❌ Failed to get diagnostics: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to get diagnostics: {str(e)}")


@router.get("/robot/status")
async def get_robot_status(
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    """
    Get robot status
    
    Returns current robot status and telemetry data.
    """
    try:
        resp = await fw_service.get_robot_status()
        
        if resp.success:
            return JSONResponse(status_code=200, content={
                "success": True,
                "data": resp.data,
                "timestamp": datetime.now(timezone.utc).isoformat()
            })
        else:
            return JSONResponse(status_code=503, content={
                "success": False,
                "error": resp.error or "Robot status unavailable",
                "timestamp": datetime.now(timezone.utc).isoformat()
            })
        
    except Exception as e:
        logger.error(f"❌ Failed to get robot status: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to get robot status: {str(e)}")


@router.post("/robot/command")
async def send_robot_command(
    command_data: Dict[str, Any],
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
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
        
        resp = await fw_service.send_robot_command(command_data)
        
        if resp.success:
            return JSONResponse(status_code=200, content={
                "success": True,
                "message": f"Robot command '{command_type}' sent successfully",
                "timestamp": datetime.now(timezone.utc).isoformat()
            })
        return JSONResponse(status_code=400, content={
            "success": False,
            "error": f"Failed to send robot command '{command_type}'",
            "timestamp": datetime.now(timezone.utc).isoformat()
        })
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Failed to send robot command: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to send robot command: {str(e)}")


@router.get("/telemetry")
async def get_telemetry_data(
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    """
    Get telemetry data
    
    Returns current telemetry data from robot (sensors disabled - no real hardware).
    """
    try:
        resp = await fw_service.get_telemetry_data()
        return JSONResponse(status_code=200, content=resp.data or {"error": resp.error})
    except Exception as e:
        logger.error(f"❌ Failed to get telemetry data: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to get telemetry data: {str(e)}")


@router.get("/sensors/{sensor_type}")
async def get_sensor_data(
    sensor_type: str,
    sensor_id: Optional[str] = None,
    current_user: User = Depends(get_current_user),
    fw_service: UnifiedFirmwareService = Depends(_get_fw_service)
):
    """
    Get sensor data
    
    Returns data from a specific sensor type.
    """
    try:
        resp = await fw_service.get_sensor_data(sensor_type, sensor_id)
        if resp.success:
            return JSONResponse(status_code=200, content={
                "success": True,
                "data": resp.data,
                "timestamp": datetime.now(timezone.utc).isoformat()
            })
        return JSONResponse(status_code=404, content={
            "success": False,
            "error": f"Sensor data for '{sensor_type}' not found",
            "timestamp": datetime.now(timezone.utc).isoformat()
        })
    except Exception as e:
        logger.error(f"❌ Failed to get sensor data for {sensor_type}: {e}")
        raise HTTPException(status_code=500, detail=f"Failed to get sensor data: {str(e)}")
