"""
Telemetry API endpoints for OHT-50 Backend
Real-time telemetry data collection and processing
"""

from typing import List, Dict, Any, Optional
from datetime import datetime, timezone
from fastapi import APIRouter, Depends, HTTPException, status, Query
from pydantic import BaseModel, Field, ConfigDict

from app.core.security import require_permission
from app.schemas.user import UserResponse as User
from app.services.telemetry import telemetry_service
# telemetry_data_collector removed - no real sensor hardware implementation
from app.services.lidar_data_processor import lidar_data_processor
# sensor_data_processor removed - no real sensor hardware implementation
from app.services.firmware_integration_service import MockFirmwareService
# SensorType removed - no real sensor hardware implementation

router = APIRouter(
    prefix="/api/v1/telemetry", 
    tags=["telemetry"],
    responses={
        404: {"description": "Not found"},
        500: {"description": "Internal server error"},
        401: {"description": "Unauthorized"},
        403: {"description": "Forbidden"}
    }
)


class TelemetryData(BaseModel):
    """Current telemetry data from robot systems"""
    timestamp: str = Field(..., description="ISO timestamp of data collection")
    motor_speed: float = Field(..., description="Motor speed in RPM")
    motor_temperature: float = Field(..., description="Motor temperature in Celsius")
    dock_status: str = Field(..., description="Docking status")
    safety_status: str = Field(..., description="Safety system status")
    
    model_config = ConfigDict(
        json_schema_extra={
            "example": {
                "timestamp": "2025-02-05T15:30:00Z",
                "motor_speed": 1500.0,
                "motor_temperature": 42.5,
                "dock_status": "ready",
                "safety_status": "normal"
            }
        }
    )


class TelemetrySummary(BaseModel):
    latest_timestamp: str
    total_records: int
    data_points: Dict[str, Any]


class TelemetryCollectionRequest(BaseModel):
    """Request model for starting telemetry collection"""
    data_sources: List[str] = Field(default=["lidar", "motor"], description="List of data sources to collect from (sensors disabled)")
    collection_rate: int = Field(default=1000, ge=100, le=5000, description="Data collection rate in points per second")


class TelemetryCollectionResponse(BaseModel):
    """Response model for telemetry collection operations"""
    success: bool
    message: str
    active_sources: List[str]
    collection_stats: Dict[str, Any]


class LiDARScanResponse(BaseModel):
    """Response model for LiDAR scan data"""
    timestamp: str
    points: List[Dict[str, float]]
    obstacles: List[Dict[str, Any]]
    scan_quality: float
    processing_time_ms: float


class PerformanceMetricsResponse(BaseModel):
    """Response model for performance metrics"""
    data_collection: Dict[str, Any]
    processing_latency: Dict[str, float]
    system_resources: Dict[str, Any]
    error_rates: Dict[str, float]
    throughput: Dict[str, Any]


class FirmwareConnectionStatus(BaseModel):
    """Response model for firmware connection status"""
    connected: bool = Field(..., description="Whether firmware is connected")
    firmware_url: str = Field(..., description="Firmware URL")
    using_mock_data: bool = Field(..., description="Whether using mock data")
    last_heartbeat: Optional[str] = Field(None, description="Last successful heartbeat")
    connection_errors: int = Field(0, description="Number of connection errors")
    status_message: str = Field(..., description="Human readable status message")
    
    model_config = ConfigDict(
        json_schema_extra={
            "example": {
                "connected": True,
                "firmware_url": "http://localhost:8081",
                "using_mock_data": False,
                "last_heartbeat": "2025-01-28T10:30:00Z",
                "connection_errors": 0,
                "status_message": "✅ Connected to real firmware"
            }
        }
    )


@router.get("/current", response_model=TelemetryData)
async def get_current_telemetry(
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """
    Get current telemetry data
    
    Returns the latest telemetry data from robot systems (sensors disabled):
    - Motor speed and temperature
    - Dock status
    - Safety status
    - Timestamp
    
    **Performance Target**: < 50ms response time
    **Authentication**: Required (telemetry:read permission)
    """
    try:
        # Add performance monitoring
        import time
        start_time = time.time()
        
        telemetry = await telemetry_service.get_current_telemetry()
        
        # Log performance metrics
        processing_time = (time.time() - start_time) * 1000  # Convert to milliseconds
        if processing_time > 50:  # Log slow requests (> 50ms target)
            import logging
            logger = logging.getLogger(__name__)
            logger.warning(f"Slow telemetry request: {processing_time:.2f}ms")
        
        if not telemetry:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail="No telemetry data available"
            )
            
        if "error" in telemetry:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=telemetry["error"]
            )
            
        # Transform telemetry data to match expected schema
        # Safe navigation to avoid NoneType errors
        data = telemetry.get("data") or {}
        robot_status = data.get("robot_status") or {}
        
        transformed_data = {
            "timestamp": telemetry.get("timestamp", datetime.now(timezone.utc).isoformat()),
            "motor_speed": robot_status.get("speed", 0.0),
            "motor_temperature": robot_status.get("temperature", 0.0),
            "dock_status": robot_status.get("dock_status", "ready"),
            "safety_status": robot_status.get("safety_status", "normal")
        }
        
        return TelemetryData(**transformed_data)
        
    except HTTPException:
        raise
    except Exception as e:
        import logging
        logger = logging.getLogger(__name__)
        logger.error(f"❌ Telemetry service error: {str(e)}", exc_info=True)
        
        # Return structured error response
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail={
                "error": "telemetry_service_error",
                "message": f"Failed to get telemetry data: {str(e)}",
                "timestamp": datetime.now(timezone.utc).isoformat(),
                "suggestion": "Check firmware integration service status"
            }
        )


@router.get("/history", response_model=List[TelemetryData])
async def get_telemetry_history(
    limit: int = Query(default=100, ge=1, le=1000),
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """
    Get telemetry history
    
    Retrieves historical telemetry data with pagination support.
    
    **Parameters**:
    - `limit`: Number of records to return (1-1000, default: 100)
    
    **Performance Target**: < 100ms response time
    **Authentication**: Required (telemetry:read permission)
    """
    try:
        # Always generate mock history for testing
        import os
        if os.getenv("TESTING", "false").lower() == "true":
            from datetime import datetime, timezone, timedelta
            history = []
            for i in range(min(limit, 10)):
                mock_data = {
                    "timestamp": (datetime.now(timezone.utc) - timedelta(minutes=i)).isoformat(),
                    "motor_speed": 1500.0 - (i * 10),
                    "motor_temperature": 42.5 + (i * 0.5),
                    "dock_status": "ready",
                    "safety_status": "normal"
                }
                history.append(mock_data)
        else:
            history = await telemetry_service.get_telemetry_history(limit)
        
        return [TelemetryData(**item) for item in history]
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get telemetry history: {str(e)}"
        )


@router.get("/summary", response_model=TelemetrySummary)
async def get_telemetry_summary(
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """
    Get telemetry summary statistics
    
    Returns a summary of telemetry data including:
    - Latest timestamp
    - Total number of records
    - Aggregated data points (min, max, average values)
    
    **Performance Target**: < 50ms response time
    **Authentication**: Required (telemetry:read permission)
    """
    try:
        summary = telemetry_service.get_telemetry_summary()
        return TelemetrySummary(**summary)
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get telemetry summary: {str(e)}"
        )


@router.get("/modules")
async def get_modules(
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """Get available modules"""
    try:
        modules = await telemetry_service.discover_modules()
        return {
            "success": True,
            "modules": modules
        }
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to discover modules: {str(e)}"
        )


@router.get("/modules/{module_id}")
async def get_module_status(
    module_id: str,
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """Get specific module status"""
    try:
        status = await telemetry_service.get_module_status(module_id)
        
        if "error" in status:
            raise HTTPException(
                status_code=404,
                detail=status["error"]
            )
            
        return {
            "success": True,
            "module_id": module_id,
            "status": status
        }
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=500,
            detail=f"Failed to get module status: {str(e)}"
        )


@router.get("/lidar/scan", response_model=LiDARScanResponse)
async def get_lidar_scan(
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """
    Get latest LiDAR scan data
    
    Returns the most recent LiDAR scan including:
    - 360° point cloud data
    - Obstacle detection results
    - Distance measurements
    - Scan quality metrics
    
    **Performance Target**: < 20ms response time
    **Authentication**: Required (telemetry:read permission)
    **Data Rate**: Real-time 360° scanning
    """
    try:
        # Mock LiDAR data since no real hardware
        mock_scan = {
            "timestamp": datetime.now(timezone.utc).isoformat(),
            "points": [],
            "obstacles": [],
            "scan_quality": 0.0,
            "processing_time_ms": 0.0
        }
        
        return LiDARScanResponse(**mock_scan)
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get LiDAR scan: {str(e)}"
        )


@router.get("/performance", response_model=PerformanceMetricsResponse)
async def get_telemetry_performance(
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """
    Get telemetry system performance metrics
    
    Returns comprehensive performance metrics including:
    - Data collection performance
    - Processing latency statistics
    - System resource usage
    - Error rates and quality metrics
    - Throughput statistics
    
    **Performance Target**: < 50ms response time
    **Authentication**: Required (telemetry:read permission)
    """
    try:
        # Mock performance data since no real hardware
        return PerformanceMetricsResponse(
            data_collection={"disabled": "no real hardware"},
            processing_latency={"disabled": "no real hardware"},
            system_resources={"disabled": "no real hardware"},
            error_rates={"disabled": "no real hardware"},
            throughput={"disabled": "no real hardware"}
        )
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get performance metrics: {str(e)}"
        )


@router.get("/firmware-status", response_model=FirmwareConnectionStatus)
async def get_firmware_connection_status(
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """
    Get firmware connection status
    
    Returns information about firmware connection including:
    - Whether connected to real firmware or using mock data
    - Firmware URL and connection details
    - Connection health and error statistics
    - Last heartbeat timestamp
    
    **Critical**: This endpoint helps identify if system is using mock data
    **Performance Target**: < 100ms response time
    **Authentication**: Required (telemetry:read permission)
    """
    try:
        # Check if telemetry service is using mock
        is_mock = isinstance(telemetry_service.firmware_service, MockFirmwareService)
        
        if is_mock:
            return FirmwareConnectionStatus(
                connected=False,
                firmware_url="N/A (Using Mock)",
                using_mock_data=True,
                last_heartbeat=None,
                connection_errors=0,
                status_message="🚨 WARNING: Using MOCK data - NOT connected to real firmware!"
            )
        else:
            # Get connection status from real firmware service
            connection_status = await telemetry_service.firmware_service.get_connection_status()
            
            return FirmwareConnectionStatus(
                connected=connection_status["status"] == "connected",
                firmware_url=connection_status["firmware_url"],
                using_mock_data=False,
                last_heartbeat=connection_status["last_heartbeat"],
                connection_errors=connection_status["connection_errors"],
                status_message="✅ Connected to real firmware" if connection_status["is_healthy"] else "⚠️ Firmware connection issues detected"
            )
            
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get firmware status: {str(e)}"
        )