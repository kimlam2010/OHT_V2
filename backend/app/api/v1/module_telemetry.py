# -*- coding: utf-8 -*-
"""
Enhanced Module Telemetry API with Value Range Validation
For Issue #144: Add Value Range Validation for Module Telemetry Data
"""

import logging
from fastapi import APIRouter, HTTPException, Depends
from typing import Dict, Any, List
from datetime import datetime

from app.models.module_telemetry import (
    ModuleTelemetry, TelemetryValidationRequest, TelemetryValidationResponse,
    TelemetryUpdateRequest, TelemetryUpdateResponse
)
from app.services.validation_service import validation_service
from app.services.unified_firmware_service import get_firmware_service
from app.services.database_service import db_service

logger = logging.getLogger(__name__)

router = APIRouter()


@router.get("/api/v1/modules/{module_id}/telemetry")
async def get_module_telemetry(module_id: int):
    """
    Get module telemetry data with value range validation
    
    Returns enhanced telemetry data with validation information
    """
    try:
        logger.info(f"📊 Getting telemetry for module {module_id}")
        
        service = await get_firmware_service()
        resp = await service.get_module_telemetry(module_id)
        raw_data = resp.data if resp.success and resp.data else None
        
        if not raw_data:
            raise HTTPException(status_code=404, detail=f"Module {module_id} not found")
        
        enhanced_data = await validation_service.enhance_telemetry_data(raw_data)
        telemetry_response = ModuleTelemetry(**enhanced_data)
        
        return {
            "success": True,
            "data": telemetry_response.dict(),
            "message": "Module telemetry retrieved successfully",
            "timestamp": datetime.utcnow().isoformat()
        }
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Failed to get module telemetry: {e}")
        raise HTTPException(status_code=500, detail="Failed to get module telemetry")


@router.post("/api/v1/modules/{module_id}/telemetry/validate")
async def validate_module_telemetry(module_id: int, telemetry_data: ModuleTelemetry):
    """
    Validate module telemetry data against value ranges
    
    Returns validation results with detailed error information
    """
    try:
        logger.info(f"🔍 Validating telemetry for module {module_id}")
        
        validation_result = await validation_service.validate_telemetry_data(telemetry_data)
        
        return {
            "success": True,
            "validation_status": "valid" if validation_result.valid else "invalid",
            "valid": validation_result.valid,
            "errors": validation_result.errors,
            "validation_details": validation_result.validation_details,
            "message": "Validation completed successfully",
            "timestamp": datetime.utcnow().isoformat()
        }
        
    except Exception as e:
        logger.error(f"❌ Validation error: {e}")
        return {
            "success": False,
            "valid": False,
            "errors": [{"error": str(e)}],
            "message": f"Validation failed: {str(e)}",
            "timestamp": datetime.utcnow().isoformat()
        }


@router.post("/api/v1/modules/{module_id}/telemetry/update")
async def update_module_telemetry(module_id: int, request: TelemetryUpdateRequest):
    """
    Update module telemetry data with validation
    
    Updates telemetry data and stores validation results
    """
    try:
        logger.info(f"✏️ Updating telemetry for module {module_id}")
        
        validation_result = await validation_service.validate_telemetry_data(request.telemetry_data)
        
        if not validation_result.valid and not request.force:
            return {
                "success": False,
                "validation_status": "invalid",
                "data": {
                    "validation_errors": validation_result.errors,
                    "validation_details": validation_result.validation_details
                },
                "message": "Validation failed. Use force=true to update anyway.",
                "timestamp": datetime.utcnow().isoformat()
            }
        
        update_success = await db_service.update_module_telemetry(
            module_id, 
            request.telemetry_data
        )
        
        if not update_success:
            raise HTTPException(status_code=500, detail="Failed to update telemetry data")
        
        if validation_result.valid:
            service = await get_firmware_service()
            fw_resp = await service.update_module_telemetry(
                module_id,
                request.telemetry_data.dict()
            )
            if not fw_resp.success:
                logger.warning(f"⚠️ Failed to update firmware for module {module_id}: {fw_resp.error}")
        
        return {
            "success": True,
            "validation_status": "valid" if validation_result.valid else "invalid_forced",
            "data": {
                "module_id": module_id,
                "updated_fields": list(request.telemetry_data.telemetry.keys()),
                "validation_result": validation_result.dict() if validation_result.valid else None
            },
            "message": "Telemetry updated successfully",
            "timestamp": datetime.utcnow().isoformat()
        }
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Failed to update module telemetry: {e}")
        raise HTTPException(status_code=500, detail="Failed to update module telemetry")


@router.get("/api/v1/modules/{module_id}/telemetry/field-configs")
async def get_module_field_configs(module_id: int):
    """
    Get field configurations for module telemetry validation
    
    Returns available field configurations with min/max ranges
    """
    try:
        logger.info(f"⚙️ Getting field configs for module {module_id}")
        
        # Get all field configurations
        field_configs = await validation_service.get_all_field_configs()
        
        return {
            "success": True,
            "data": {
                "module_id": module_id,
                "field_configs": field_configs,
                "total_fields": len(field_configs)
            },
            "message": "Field configurations retrieved successfully",
            "timestamp": datetime.utcnow().isoformat()
        }
        
    except Exception as e:
        logger.error(f"❌ Failed to get field configs: {e}")
        raise HTTPException(status_code=500, detail="Failed to get field configurations")


@router.put("/api/v1/modules/{module_id}/telemetry/field-configs/{field_name}")
async def update_field_config(module_id: int, field_name: str, config: Dict[str, Any]):
    """
    Update field configuration for telemetry validation
    
    Updates min/max ranges and other validation parameters
    """
    try:
        logger.info(f"⚙️ Updating field config {field_name} for module {module_id}")
        
        # Validate config structure
        required_keys = ["min_value", "max_value", "unit", "description"]
        if not all(key in config for key in required_keys):
            raise HTTPException(
                status_code=400, 
                detail=f"Invalid config: missing required keys {required_keys}"
            )
        
        # Update field configuration
        update_success = await validation_service.update_field_config(field_name, config)
        
        if not update_success:
            raise HTTPException(status_code=500, detail="Failed to update field configuration")
        
        return {
            "success": True,
            "data": {
                "module_id": module_id,
                "field_name": field_name,
                "updated_config": config
            },
            "message": f"Field configuration {field_name} updated successfully",
            "timestamp": datetime.utcnow().isoformat()
        }
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Failed to update field config: {e}")
        raise HTTPException(status_code=500, detail="Failed to update field configuration")


@router.get("/api/v1/modules/telemetry/validation-status")
async def get_validation_status():
    """
    Get overall telemetry validation status
    
    Returns summary of validation status across all modules
    """
    try:
        logger.info("📊 Getting overall telemetry validation status")
        
        # Get validation status from database
        validation_status = await db_service.get_telemetry_validation_status()
        
        return {
            "success": True,
            "data": validation_status,
            "message": "Validation status retrieved successfully",
            "timestamp": datetime.utcnow().isoformat()
        }
        
    except Exception as e:
        logger.error(f"❌ Failed to get validation status: {e}")
        raise HTTPException(status_code=500, detail="Failed to get validation status")
