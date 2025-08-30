"""
Configuration API endpoints
Quản lý cấu hình hệ thống OHT-50
"""

from typing import List, Optional
from fastapi import APIRouter, HTTPException, Depends, Query
from fastapi.responses import PlainTextResponse

from app.core.logging import get_logger
from app.core.security import verify_token
from app.core.exceptions import ConfigurationError, ValidationError
from app.models.config import SystemConfig, ConfigHistory, ConfigResponse
from app.services.config_service import ConfigService

logger = get_logger(__name__)
router = APIRouter(tags=["Configuration"])

# Global config service instance
config_service = None

def get_config_service():
    """Get config service instance"""
    global config_service
    if config_service is None:
        config_service = ConfigService()
    return config_service


@router.get("/", response_model=SystemConfig, summary="Lấy cấu hình hiện tại", dependencies=[Depends(verify_token)])
async def get_current_config():
    """
    Lấy cấu hình hệ thống hiện tại
    
    Returns:
        SystemConfig: Cấu hình hiện tại của hệ thống
    """
    try:
        service = get_config_service()
        config = service.get_current_config()
        logger.info("Retrieved current configuration", version=config.version)
        return config
    except ConfigurationError as e:
        logger.error("Failed to get current config", error=str(e))
        raise HTTPException(status_code=404, detail=str(e))
    except Exception as e:
        logger.error("Unexpected error getting config", error=str(e))
        raise HTTPException(status_code=500, detail="Internal server error")


@router.get("/history", response_model=List[ConfigHistory], summary="Lấy lịch sử cấu hình", dependencies=[Depends(verify_token)])
async def get_config_history(limit: int = Query(10, ge=1, le=50, description="Số lượng bản ghi tối đa")):
    """
    Lấy lịch sử thay đổi cấu hình
    
    Args:
        limit: Số lượng bản ghi tối đa (1-50)
    
    Returns:
        List[ConfigHistory]: Danh sách lịch sử cấu hình
    """
    try:
        service = get_config_service()
        history = service.get_config_history(limit)
        logger.info("Retrieved config history", count=len(history))
        return history
    except Exception as e:
        logger.error("Failed to get config history", error=str(e))
        raise HTTPException(status_code=500, detail="Internal server error")


@router.get("/version/{version}", response_model=SystemConfig, summary="Lấy cấu hình theo version", dependencies=[Depends(verify_token)])
async def get_config_by_version(version: int):
    """
    Lấy cấu hình theo version cụ thể
    
    Args:
        version: Version của cấu hình
    
    Returns:
        SystemConfig: Cấu hình theo version
    """
    try:
        service = get_config_service()
        config = service.get_config_by_version(version)
        if not config:
            raise HTTPException(status_code=404, detail=f"Configuration version {version} not found")
        
        logger.info("Retrieved config by version", version=version)
        return config
    except HTTPException:
        raise
    except Exception as e:
        logger.error("Failed to get config by version", version=version, error=str(e))
        raise HTTPException(status_code=500, detail="Internal server error")


@router.put("/", response_model=ConfigResponse, summary="Cập nhật cấu hình", dependencies=[Depends(verify_token)])
async def update_config(config: SystemConfig, user: str = Query("system", description="Tên người dùng")):
    """
    Cập nhật cấu hình hệ thống
    
    Args:
        config: Cấu hình mới
        user: Tên người dùng thực hiện thay đổi
    
    Returns:
        ConfigResponse: Kết quả cập nhật
    """
    try:
        # Bỏ version từ input để service tự tạo version mới
        config_data = config.model_dump()
        config_data.pop("version", None)
        
        service = get_config_service()
        result = service.update_config(config_data, user)
        logger.info("Configuration updated successfully", 
                   old_version=config.version, new_version=result.version, user=user)
        return result
    except (ConfigurationError, ValidationError) as e:
        logger.error("Failed to update config", error=str(e), user=user)
        raise HTTPException(status_code=400, detail=str(e))
    except Exception as e:
        logger.error("Unexpected error updating config", error=str(e), user=user)
        raise HTTPException(status_code=500, detail="Internal server error")


@router.post("/rollback/{version}", response_model=ConfigResponse, summary="Rollback cấu hình", dependencies=[Depends(verify_token)])
async def rollback_config(version: int, user: str = Query("system", description="Tên người dùng")):
    """
    Rollback cấu hình về version cũ
    
    Args:
        version: Version muốn rollback về
        user: Tên người dùng thực hiện rollback
    
    Returns:
        ConfigResponse: Kết quả rollback
    """
    try:
        service = get_config_service()
        result = service.rollback_config(version, user)
        logger.info("Configuration rollback successful", 
                   from_version=version, to_version=result.version, user=user)
        return result
    except ConfigurationError as e:
        logger.error("Failed to rollback config", error=str(e), version=version, user=user)
        raise HTTPException(status_code=404, detail=str(e))
    except ValidationError as e:
        logger.error("Validation failed for rollback", error=str(e), version=version, user=user)
        raise HTTPException(status_code=400, detail=str(e))
    except Exception as e:
        logger.error("Unexpected error during rollback", error=str(e), version=version, user=user)
        raise HTTPException(status_code=500, detail="Internal server error")


@router.get("/export", response_class=PlainTextResponse, summary="Export cấu hình", dependencies=[Depends(verify_token)])
async def export_config(format: str = Query("json", pattern="^(json|yaml)$", description="Định dạng export")):
    """
    Export cấu hình hiện tại
    
    Args:
        format: Định dạng export (json hoặc yaml)
    
    Returns:
        str: Cấu hình dưới dạng text
    """
    try:
        service = get_config_service()
        content = service.export_config(format)
        logger.info("Configuration exported", format=format)
        return content
    except ConfigurationError as e:
        logger.error("Failed to export config", error=str(e))
        raise HTTPException(status_code=404, detail=str(e))
    except Exception as e:
        logger.error("Unexpected error exporting config", error=str(e))
        raise HTTPException(status_code=500, detail="Internal server error")


@router.post("/import", response_model=ConfigResponse, summary="Import cấu hình", dependencies=[Depends(verify_token)])
async def import_config(
    content: str = Query(..., description="Nội dung cấu hình"),
    format: str = Query("json", pattern="^(json|yaml)$", description="Định dạng import"),
    user: str = Query("system", description="Tên người dùng")
):
    """
    Import cấu hình từ string
    
    Args:
        content: Nội dung cấu hình
        format: Định dạng import (json hoặc yaml)
        user: Tên người dùng thực hiện import
    
    Returns:
        ConfigResponse: Kết quả import
    """
    try:
        service = get_config_service()
        result = service.import_config(content, format, user)
        logger.info("Configuration imported successfully", format=format, user=user)
        return result
    except (ConfigurationError, ValidationError) as e:
        logger.error("Failed to import config", error=str(e), format=format, user=user)
        raise HTTPException(status_code=400, detail=str(e))
    except Exception as e:
        logger.error("Unexpected error importing config", error=str(e), format=format, user=user)
        raise HTTPException(status_code=500, detail="Internal server error")


@router.post("/validate", summary="Validate cấu hình", dependencies=[Depends(verify_token)])
async def validate_config(config: SystemConfig):
    """
    Validate cấu hình mà không lưu
    
    Args:
        config: Cấu hình cần validate
    
    Returns:
        dict: Kết quả validation
    """
    try:
        config_data = config.model_dump()
        config_data.pop("version", None)
        
        service = get_config_service()
        is_valid = service.validate_config_data(config_data)
        
        if is_valid:
            logger.info("Configuration validation passed")
            return {"valid": True, "message": "Configuration is valid"}
        else:
            logger.warning("Configuration validation failed")
            return {"valid": False, "message": "Configuration validation failed"}
            
    except Exception as e:
        logger.error("Error during validation", error=str(e))
        return {"valid": False, "message": f"Validation error: {str(e)}"}


@router.get("/schema", summary="Lấy JSON schema", dependencies=[Depends(verify_token)])
async def get_config_schema():
    """
    Lấy JSON schema cho cấu hình
    
    Returns:
        dict: JSON schema
    """
    try:
        schema = SystemConfig.model_json_schema()
        logger.info("Retrieved config schema")
        return schema
    except Exception as e:
        logger.error("Failed to get schema", error=str(e))
        raise HTTPException(status_code=500, detail="Internal server error")
