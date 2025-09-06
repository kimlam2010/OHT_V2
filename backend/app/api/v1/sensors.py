"""
Sensor Data API endpoints for OHT-50 Backend
"""

import logging
from typing import Dict, Any, List, Optional
from fastapi import APIRouter, Depends, HTTPException, status, BackgroundTasks
from fastapi.responses import JSONResponse
from pydantic import BaseModel, Field
from datetime import datetime

from app.core.security import require_permission
from app.core.database import get_db
from app.models.user import User
from app.models.telemetry import SensorData
from app.models.sensors import SensorConfiguration, SensorReading, SensorStatus

logger = logging.getLogger(__name__)

router = APIRouter(prefix="/api/v1/sensors", tags=["sensors"])


# Pydantic models for request/response
class SensorDataRequest(BaseModel):
    """Request model for sensor data"""
    sensor_type: str = Field(..., description="Type of sensor (rfid, accelerometer, proximity, lidar)")
    sensor_id: str = Field(..., description="Unique sensor identifier")
    data: Dict[str, Any] = Field(..., description="Sensor data payload")
    quality: Optional[float] = Field(None, description="Data quality score (0.0 to 1.0)")


class SensorDataResponse(BaseModel):
    """Response model for sensor data"""
    success: bool
    sensor_id: str
    message: str
    timestamp: str


class SensorStatusResponse(BaseModel):
    """Response model for sensor status"""
    sensor_id: str
    sensor_type: str
    status: str
    last_reading_time: Optional[datetime] = None
    error_count: int
    health_score: float
    updated_at: datetime


class SensorConfigurationRequest(BaseModel):
    """Request model for sensor configuration"""
    sensor_id: str = Field(..., description="Unique sensor identifier")
    sensor_type: str = Field(..., description="Type of sensor")
    name: str = Field(..., description="Sensor name")
    description: Optional[str] = Field(None, description="Sensor description")
    configuration: Dict[str, Any] = Field(..., description="Sensor configuration")
    calibration_data: Optional[Dict[str, Any]] = Field(None, description="Calibration parameters")


class SensorConfigurationResponse(BaseModel):
    """Response model for sensor configuration"""
    success: bool
    sensor_id: str
    message: str


class SensorListResponse(BaseModel):
    """Response model for sensor list"""
    success: bool
    sensors: List[Dict[str, Any]]
    total_count: int


@router.post("/data", response_model=SensorDataResponse)
async def submit_sensor_data(
    request: SensorDataRequest,
    background_tasks: BackgroundTasks,
    current_user: User = Depends(require_permission("sensors", "read")),
    db = Depends(get_db)
):
    """
    Submit sensor data for processing
    
    Args:
        request: Sensor data
        background_tasks: Background task handler
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        Submission result
    """
    try:
        logger.info(f"Received sensor data from {request.sensor_id} ({request.sensor_type})")
        
        # Create sensor data record
        sensor_data = SensorData(
            sensor_type=request.sensor_type,
            sensor_id=request.sensor_id,
            data=request.data,
            quality=request.quality,
            is_valid=True
        )
        
        db.add(sensor_data)
        db.commit()
        db.refresh(sensor_data)
        
        # Update sensor status in background
        background_tasks.add_task(update_sensor_status, request.sensor_id, request.sensor_type, db)
        
        logger.info(f"Sensor data submitted successfully: {sensor_data.id}")
        
        return SensorDataResponse(
            success=True,
            sensor_id=request.sensor_id,
            message="Sensor data submitted successfully",
            timestamp=datetime.utcnow().isoformat()
        )
        
    except Exception as e:
        logger.error(f"Failed to submit sensor data: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to submit sensor data"
        )


@router.get("/status/{sensor_id}", response_model=SensorStatusResponse)
async def get_sensor_status(
    sensor_id: str,
    current_user: User = Depends(require_permission("sensors", "read"))
):
    """
    Get sensor status
    
    Args:
        sensor_id: Sensor identifier
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        Sensor status information
    """
    try:
        # Get sensor status from database
        from app.core.database import get_db_context
        async with get_db_context() as db:
            from sqlalchemy import select
            result = await db.execute(select(SensorStatus).filter(
                SensorStatus.sensor_id == sensor_id
            ))
            sensor_status = result.scalar_one_or_none()
            
            if not sensor_status:
                raise HTTPException(
                    status_code=status.HTTP_404_NOT_FOUND,
                    detail=f"Sensor {sensor_id} not found"
                )
            
            return SensorStatusResponse(
                sensor_id=sensor_status.sensor_id,
                sensor_type=sensor_status.sensor_type,
                status=sensor_status.status,
                last_reading_time=sensor_status.last_reading_time,
                error_count=sensor_status.error_count,
                health_score=sensor_status.health_score,
                updated_at=sensor_status.updated_at
            )
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"Failed to get sensor status: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get sensor status"
        )


@router.get("/status", response_model=List[SensorStatusResponse])
async def get_all_sensor_status(
    current_user: User = Depends(require_permission("sensors", "read")),
    db = Depends(get_db)
):
    """
    Get status of all sensors
    
    Args:
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        List of sensor statuses
    """
    try:
        # Get all sensor statuses
        from sqlalchemy import select
        result = await db.execute(select(SensorStatus))
        sensor_statuses = result.scalars().all()
        
        status_list = []
        for sensor_status in sensor_statuses:
            status_list.append(SensorStatusResponse(
                sensor_id=sensor_status.sensor_id,
                sensor_type=sensor_status.sensor_type,
                status=sensor_status.status,
                last_reading_time=sensor_status.last_reading_time,
                error_count=sensor_status.error_count,
                health_score=sensor_status.health_score,
                updated_at=sensor_status.updated_at
            ))
        
        return status_list
        
    except Exception as e:
        logger.error(f"Failed to get sensor statuses: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get sensor statuses"
        )


@router.post("/configure", response_model=SensorConfigurationResponse)
async def configure_sensor(
    request: SensorConfigurationRequest,
    current_user: User = Depends(require_permission("sensors", "read")),
    db = Depends(get_db)
):
    """
    Configure sensor
    
    Args:
        request: Sensor configuration
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        Configuration result
    """
    try:
        logger.info(f"Configuring sensor {request.sensor_id} ({request.sensor_type})")
        
        # Check if sensor configuration already exists
        existing_config = db.query(SensorConfiguration).filter(
            SensorConfiguration.sensor_id == request.sensor_id
        ).first()
        
        if existing_config:
            # Update existing configuration
            existing_config.sensor_type = request.sensor_type
            existing_config.name = request.name
            existing_config.description = request.description
            existing_config.configuration = request.configuration
            existing_config.calibration_data = request.calibration_data
            existing_config.updated_at = datetime.utcnow()
        else:
            # Create new configuration
            sensor_config = SensorConfiguration(
                sensor_id=request.sensor_id,
                sensor_type=request.sensor_type,
                name=request.name,
                description=request.description,
                configuration=request.configuration,
                calibration_data=request.calibration_data,
                is_active=True
            )
            db.add(sensor_config)
        
        db.commit()
        
        logger.info(f"Sensor configured successfully: {request.sensor_id}")
        
        return SensorConfigurationResponse(
            success=True,
            sensor_id=request.sensor_id,
            message="Sensor configured successfully"
        )
        
    except Exception as e:
        logger.error(f"Failed to configure sensor: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to configure sensor"
        )


@router.get("/configure/{sensor_id}", response_model=Dict[str, Any])
async def get_sensor_configuration(
    sensor_id: str,
    current_user: User = Depends(require_permission("sensors", "read")),
    db = Depends(get_db)
):
    """
    Get sensor configuration
    
    Args:
        sensor_id: Sensor identifier
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        Sensor configuration
    """
    try:
        # Get sensor configuration
        sensor_config = db.query(SensorConfiguration).filter(
            SensorConfiguration.sensor_id == sensor_id
        ).first()
        
        if not sensor_config:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail=f"Sensor configuration {sensor_id} not found"
            )
        
        return JSONResponse(
            status_code=status.HTTP_200_OK,
            content={
                "success": True,
                "data": {
                    "sensor_id": sensor_config.sensor_id,
                    "sensor_type": sensor_config.sensor_type,
                    "name": sensor_config.name,
                    "description": sensor_config.description,
                    "configuration": sensor_config.configuration,
                    "calibration_data": sensor_config.calibration_data,
                    "is_active": sensor_config.is_active,
                    "created_at": sensor_config.created_at.isoformat(),
                    "updated_at": sensor_config.updated_at.isoformat()
                }
            }
        )
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"Failed to get sensor configuration: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get sensor configuration"
        )


@router.get("/list", response_model=SensorListResponse)
async def get_sensor_list(
    current_user: User = Depends(require_permission("sensors", "read"))
):
    """
    Get list of all sensors
    
    Args:
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        List of sensors
    """
    try:
        # Get all sensor configurations
        from app.core.database import get_db_context
        async with get_db_context() as db:
            from sqlalchemy import select
            result = await db.execute(select(SensorConfiguration).filter(
                SensorConfiguration.is_active == True
            ))
            sensor_configs = result.scalars().all()
            
            sensor_list = []
            for config in sensor_configs:
                # Get latest status
                status_result = await db.execute(select(SensorStatus).filter(
                    SensorStatus.sensor_id == config.sensor_id
                ))
                status = status_result.scalar_one_or_none()
                
                sensor_info = {
                    "sensor_id": config.sensor_id,
                    "sensor_type": config.sensor_type,
                    "name": config.name,
                    "description": config.description,
                    "is_active": config.is_active,
                    "status": status.status if status else "unknown",
                    "health_score": status.health_score if status else 0.0,
                    "last_reading_time": status.last_reading_time.isoformat() if status and status.last_reading_time else None,
                    "created_at": config.created_at.isoformat(),
                    "updated_at": config.updated_at.isoformat()
                }
                sensor_list.append(sensor_info)
            
            return SensorListResponse(
                success=True,
                sensors=sensor_list,
                total_count=len(sensor_list)
            )
        
    except Exception as e:
        logger.error(f"Failed to get sensor list: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get sensor list"
        )


@router.get("/data/{sensor_id}", response_model=Dict[str, Any])
async def get_sensor_data(
    sensor_id: str,
    limit: int = 100,
    current_user: User = Depends(require_permission("sensors", "read")),
    db = Depends(get_db)
):
    """
    Get recent sensor data
    
    Args:
        sensor_id: Sensor identifier
        limit: Maximum number of records to return
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        Recent sensor data
    """
    try:
        # Get recent sensor data
        sensor_data = db.query(SensorData).filter(
            SensorData.sensor_id == sensor_id,
            SensorData.is_valid == True
        ).order_by(SensorData.timestamp.desc()).limit(limit).all()
        
        data_list = []
        for data in sensor_data:
            data_list.append({
                "id": data.id,
                "sensor_type": data.sensor_type,
                "sensor_id": data.sensor_id,
                "data": data.data,
                "quality": data.quality,
                "timestamp": data.timestamp.isoformat()
            })
        
        return JSONResponse(
            status_code=status.HTTP_200_OK,
            content={
                "success": True,
                "data": data_list,
                "total_count": len(data_list),
                "sensor_id": sensor_id
            }
        )
        
    except Exception as e:
        logger.error(f"Failed to get sensor data: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to get sensor data"
        )


@router.post("/calibrate/{sensor_id}", response_model=Dict[str, Any])
async def calibrate_sensor(
    sensor_id: str,
    calibration_data: Dict[str, Any],
    current_user: User = Depends(require_permission("sensors", "read")),
    db = Depends(get_db)
):
    """
    Calibrate sensor
    
    Args:
        sensor_id: Sensor identifier
        calibration_data: Calibration parameters
        current_user: Current authenticated user
        db: Database session
        
    Returns:
        Calibration result
    """
    try:
        logger.info(f"Calibrating sensor {sensor_id}")
        
        # Get sensor configuration
        sensor_config = db.query(SensorConfiguration).filter(
            SensorConfiguration.sensor_id == sensor_id
        ).first()
        
        if not sensor_config:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail=f"Sensor {sensor_id} not found"
            )
        
        # Update calibration data
        sensor_config.calibration_data = calibration_data
        sensor_config.last_calibration = datetime.utcnow()
        sensor_config.updated_at = datetime.utcnow()
        
        db.commit()
        
        logger.info(f"Sensor calibrated successfully: {sensor_id}")
        
        return JSONResponse(
            status_code=status.HTTP_200_OK,
            content={
                "success": True,
                "message": f"Sensor {sensor_id} calibrated successfully",
                "calibration_data": calibration_data
            }
        )
        
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"Failed to calibrate sensor: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to calibrate sensor"
        )


# Background task functions
async def update_sensor_status(sensor_id: str, sensor_type: str, db):
    """
    Update sensor status in background
    
    Args:
        sensor_id: Sensor identifier
        sensor_type: Sensor type
        db: Database session
    """
    try:
        # Get or create sensor status
        sensor_status = db.query(SensorStatus).filter(
            SensorStatus.sensor_id == sensor_id
        ).first()
        
        if not sensor_status:
            sensor_status = SensorStatus(
                sensor_id=sensor_id,
                sensor_type=sensor_type,
                status="online",
                error_count=0,
                health_score=1.0
            )
            db.add(sensor_status)
        else:
            sensor_status.status = "online"
            sensor_status.last_reading_time = datetime.utcnow()
            sensor_status.health_score = min(1.0, sensor_status.health_score + 0.1)
            sensor_status.updated_at = datetime.utcnow()
        
        db.commit()
        
    except Exception as e:
        logger.error(f"Failed to update sensor status: {e}")
        db.rollback()
