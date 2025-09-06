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
from app.services.telemetry_data_collector import telemetry_data_collector, DataSource
from app.services.lidar_data_processor import lidar_data_processor
from app.services.sensor_data_processor import sensor_data_processor, SensorType

router = APIRouter(
    prefix="/telemetry", 
    tags=["Telemetry System"],
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
    data_sources: List[str] = Field(default=["lidar", "sensors", "motor"], description="List of data sources to collect from")
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


class SensorStatusResponse(BaseModel):
    """Response model for sensor status"""
    sensor_type: str
    status: str
    last_update: str
    data_quality: float
    error_count: int


class PerformanceMetricsResponse(BaseModel):
    """Response model for performance metrics"""
    data_collection: Dict[str, Any]
    processing_latency: Dict[str, float]
    system_resources: Dict[str, Any]
    error_rates: Dict[str, float]
    throughput: Dict[str, Any]


@router.get("/current", response_model=TelemetryData)
async def get_current_telemetry(
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """
    Get current telemetry data
    
    Returns the latest telemetry data from all sensors including:
    - Motor speed and temperature
    - Dock status
    - Safety status
    - Timestamp
    
    **Performance Target**: < 50ms response time
    **Authentication**: Required (telemetry:read permission)
    """
    try:
        telemetry = await telemetry_service.get_current_telemetry()
        
        if "error" in telemetry:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=telemetry["error"]
            )
            
        return TelemetryData(**telemetry)
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get telemetry: {str(e)}"
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
    - LiDAR sensors
    - Motor sensors
    - Safety sensors
    - Docking sensors
    
    **Performance Target**: < 100ms response time
    **Authentication**: Required (telemetry:write permission)
    **Data Rate**: 1000+ data points/second
    """
    try:
        success = await telemetry_data_collector.start_collection()
        
        if success:
            stats = telemetry_data_collector.get_collection_stats()
            return TelemetryCollectionResponse(
                success=True,
                message="Telemetry collection started successfully",
                active_sources=[source.value for source in telemetry_data_collector.source_handlers.keys()],
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
        success = await telemetry_data_collector.stop_collection()
        
        if success:
            stats = telemetry_data_collector.get_collection_stats()
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
        stats = telemetry_data_collector.get_collection_stats()
        quality_summary = telemetry_data_collector.get_data_quality_summary()
        
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
            try:
                data_source = DataSource(source)
            except ValueError:
                raise HTTPException(
                    status_code=status.HTTP_400_BAD_REQUEST,
                    detail=f"Invalid data source: {source}"
                )
        
        latest_data = telemetry_data_collector.get_latest_data(data_source, limit)
        
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
        lidar_data = telemetry_data_collector.get_latest_data(DataSource.LIDAR, 1)
        
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


@router.get("/sensors/status", response_model=List[SensorStatusResponse])
async def get_sensor_status(
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """
    Get sensor status and statistics
    
    Returns the current status of all robot sensors including:
    - RFID reader status
    - Accelerometer status
    - Compass status
    - Docking sensor status
    - Battery sensor status
    
    **Performance Target**: < 30ms response time
    **Authentication**: Required (telemetry:read permission)
    """
    try:
        sensor_stats = sensor_data_processor.get_sensor_statistics()
        
        return {
            "success": True,
            "sensor_statistics": sensor_stats,
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get sensor status: {str(e)}"
        )


@router.get("/sensors/{sensor_type}/data")
async def get_sensor_data(
    sensor_type: str,
    limit: int = Query(50, ge=1, le=200, description="Number of data points to return"),
    current_user: User = Depends(require_permission("telemetry", "read"))
):
    """Get sensor data for specific sensor type"""
    try:
        try:
            sensor = SensorType(sensor_type)
        except ValueError:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=f"Invalid sensor type: {sensor_type}"
            )
        
        sensor_history = sensor_data_processor.get_sensor_history(sensor, limit)
        
        # Serialize sensor data
        serialized_data = []
        for data in sensor_history:
            if hasattr(data, '__dict__'):
                serialized_data.append({
                    **data.__dict__,
                    'timestamp': data.timestamp.isoformat() if hasattr(data, 'timestamp') else None
                })
            else:
                serialized_data.append(data)
        
        return {
            "success": True,
            "sensor_type": sensor_type,
            "data": serialized_data,
            "count": len(serialized_data),
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get sensor data: {str(e)}"
        )


@router.post("/sensors/{sensor_type}/calibrate")
async def calibrate_sensor(
    sensor_type: str,
    calibration_data: Dict[str, Any],
    current_user: User = Depends(require_permission("telemetry", "write"))
):
    """Calibrate sensor with provided data"""
    try:
        try:
            sensor = SensorType(sensor_type)
        except ValueError:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=f"Invalid sensor type: {sensor_type}"
            )
        
        success = sensor_data_processor.calibrate_sensor(sensor, calibration_data)
        
        if success:
            return {
                "success": True,
                "message": f"Sensor {sensor_type} calibrated successfully",
                "calibration_data": calibration_data,
                "timestamp": datetime.now(timezone.utc).isoformat()
            }
        else:
            raise HTTPException(
                status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
                detail=f"Failed to calibrate sensor {sensor_type}"
            )
            
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to calibrate sensor: {str(e)}"
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
        collection_stats = telemetry_data_collector.get_collection_stats()
        lidar_stats = lidar_data_processor.get_processing_stats()
        sensor_stats = sensor_data_processor.get_sensor_statistics()
        
        return {
            "success": True,
            "performance_metrics": {
                "data_collection": collection_stats,
                "lidar_processing": lidar_stats,
                "sensor_processing": sensor_stats
            },
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get performance metrics: {str(e)}"
        )
