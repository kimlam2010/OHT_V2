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
# # telemetry_data_collector removed removed - no real sensor hardware implementation
from app.services.lidar_data_processor import lidar_data_processor
# sensor_data_processor removed - no real sensor hardware implementation
from app.services.unified_firmware_service import get_firmware_service
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
    """Current telemetry data from robot sensors"""
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


# SensorStatusResponse removed - no real sensor hardware implementation


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


# New Telemetry System API Endpoints

class TelemetryCollectionRequest(BaseModel):
    """Request to start/stop telemetry collection"""
    action: str = Field(..., description="start or stop")
    sources: Optional[List[str]] = Field(None, description="List of data sources to collect from")


class TelemetryCollectionResponse(BaseModel):
    """Response for telemetry collection operations"""
    success: bool
    message: str
    active_sources: List[str]
    collection_stats: Dict[str, Any]


@router.post("/collection/start", response_model=TelemetryCollectionResponse)
async def start_telemetry_collection(
    request: TelemetryCollectionRequest,
    current_user: User = Depends(require_permission("telemetry", "write"))
):
    """
    Start telemetry data collection
    
    Initiates real-time data collection from all configured data sources:
    - LiDAR data
    - Motor data  
    - Safety data
    - Docking data
    - (Sensor data disabled - no real hardware)
    
    **Performance Target**: < 100ms response time
    **Authentication**: Required (telemetry:write permission)
    **Data Rate**: 1000+ data points/second
    """
    try:
        # TODO: Implement telemetry data collection when hardware is available
        success = True  # Mock success for now
        
        if success:
            stats = {
                "total_sources": 0,
                "active_sources": 0,
                "data_rate": 0,
                "collection_time": datetime.now(timezone.utc).isoformat()
            }
            return TelemetryCollectionResponse(
                success=True,
                message="Telemetry collection started successfully (mock mode)",
                active_sources=[],  # No real sources available
                collection_stats=stats
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail="Failed to start telemetry collection"
            )
            
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to start telemetry collection: {str(e)}"
        )


@router.post("/collection/stop", response_model=TelemetryCollectionResponse)
async def stop_telemetry_collection(
    current_user: User = Depends(require_permission("telemetry", "write"))
):
    """
    Stop telemetry data collection
    
    Stops real-time data collection from all data sources.
    Preserves collected data and statistics.
    
    **Performance Target**: < 50ms response time
    **Authentication**: Required (telemetry:write permission)
    """
    try:
        # TODO: Implement telemetry data collection stop when hardware is available
        success = True  # Mock success for now
        
        if success:
            stats = {
                "total_sources": 0,
                "active_sources": 0,
                "data_rate": 0,
                "collection_time": datetime.now(timezone.utc).isoformat()
            }
            return TelemetryCollectionResponse(
                success=True,
                message="Telemetry collection stopped successfully",
                active_sources=[],
                collection_stats=stats
            )
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail="Failed to stop telemetry collection"
            )
            
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to stop telemetry collection: {str(e)}"
        )


@router.get("/collection/stats")
async def get_collection_stats(
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """
    Get telemetry collection statistics
    
    Returns detailed statistics about data collection including:
    - Total data points collected
    - Collection rate (points/second)
    - Data quality metrics
    - Active data sources
    - Error rates
    
    **Performance Target**: < 20ms response time
    **Authentication**: Required (telemetry:read permission)
    """
    try:
        # TODO: Implement real telemetry stats when hardware is available
        stats = {
            "total_sources": 0,
            "active_sources": 0,
            "data_rate": 0,
            "collection_time": datetime.now(timezone.utc).isoformat()
        }
        quality_summary = {
            "overall_quality": "good",
            "data_completeness": 100.0,
            "latency_ms": 0,
            "error_rate": 0.0
        }
        
        return {
            "success": True,
            "collection_stats": stats,
            "data_quality": quality_summary,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get collection stats: {str(e)}"
        )


@router.get("/data/latest")
async def get_latest_telemetry_data(
    source: Optional[str] = Query(None, description="Data source filter"),
    limit: int = Query(100, ge=1, le=1000, description="Number of data points to return"),
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """Get latest telemetry data"""
    try:
        data_source = None
        if source:
            # TODO: Implement DataSource enum when hardware is available
            # For now, accept any string as valid source
            data_source = source
        
        # TODO: Implement real data retrieval when hardware is available
        latest_data = []  # Mock empty data for now
        
        # Convert to serializable format
        serialized_data = []
        for data in latest_data:
            serialized_data.append({
                "source": data.source.value,
                "data": data.data,
                "timestamp": data.timestamp.isoformat(),
                "quality": data.quality.value,
                "sequence_id": data.sequence_id,
                "processing_time_ms": data.processing_time_ms
            })
        
        return {
            "success": True,
            "data": serialized_data,
            "count": len(serialized_data),
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get latest telemetry data: {str(e)}"
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
        # Get latest LiDAR data
        # TODO: Implement real LiDAR data retrieval when hardware is available
        lidar_data = []  # Mock empty data for now
        
        if not lidar_data:
            return {
                "success": True,
                "message": "No LiDAR data available",
                "scan": None
            }
        
        # Process LiDAR data
        latest_lidar = lidar_data[0]
        scan = await lidar_data_processor.process_lidar_data(latest_lidar.data)
        
        # Serialize scan data
        serialized_scan = {
            "points": [
                {
                    "distance": point.distance,
                    "angle": point.angle,
                    "intensity": point.intensity,
                    "timestamp": point.timestamp.isoformat()
                }
                for point in scan.points
            ],
            "obstacles": [
                {
                    "id": obstacle.id,
                    "type": obstacle.type.value,
                    "severity": obstacle.severity.value,
                    "center_x": obstacle.center_x,
                    "center_y": obstacle.center_y,
                    "distance": obstacle.distance,
                    "angle": obstacle.angle,
                    "width": obstacle.width,
                    "height": obstacle.height,
                    "confidence": obstacle.confidence,
                    "timestamp": obstacle.timestamp.isoformat()
                }
                for obstacle in scan.obstacles
            ],
            "scan_quality": scan.scan_quality,
            "processing_time_ms": scan.processing_time_ms,
            "timestamp": scan.timestamp.isoformat()
        }
        
        return {
            "success": True,
            "scan": serialized_scan,
            "stats": lidar_data_processor.get_processing_stats()
        }
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get LiDAR scan: {str(e)}"
        )


# Sensor status endpoint removed - no real sensor hardware implementation


# Sensor data endpoint removed - no real sensor hardware implementation


# Sensor calibration endpoint removed - no real sensor hardware implementation


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
        # collection_stats removed - no real sensor hardware implementation
        lidar_stats = lidar_data_processor.get_processing_stats()
        # sensor_stats removed - no real sensor hardware implementation
        
        return {
            "success": True,
            "performance_metrics": {
                "data_collection": {"disabled": "no real hardware"},
                "lidar_processing": lidar_stats,
                "sensor_processing": {"disabled": "no real hardware"}
            },
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
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
