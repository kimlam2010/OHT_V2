"""
Health Check API for OHT-50 Backend
Provides system health monitoring and status endpoints
"""

import logging
import psutil
import asyncio
from datetime import datetime
from typing import Dict, Any, List
from fastapi import APIRouter, HTTPException, Depends
from sqlalchemy.ext.asyncio import AsyncSession

from app.core.database import get_db
from app.services.firmware_integration_service import firmware_service
# realtime_service removed - no real sensor hardware implementation
# sensor_processor removed - no real sensor hardware implementation

logger = logging.getLogger(__name__)

router = APIRouter(prefix="/api/v1/health", tags=["health"])


@router.get("/")
async def basic_health_check():
    """
    Basic health check endpoint
    
    Returns:
        Health status of the system
    """
    try:
        return {
            "status": "healthy",
            "timestamp": datetime.utcnow().isoformat(),
            "service": "OHT-50 Backend",
            "version": "1.0.0"
        }
    except Exception as e:
        logger.error("Health check failed: %s", e)
        raise HTTPException(status_code=500, detail="Health check failed")


@router.get("/detailed")
async def detailed_health_check(db: AsyncSession = Depends(get_db)):
    """
    Detailed health check endpoint
    
    Returns:
        Detailed health status of all system components
    """
    try:
        health_status = {
            "status": "healthy",
            "timestamp": datetime.utcnow().isoformat(),
            "service": "OHT-50 Backend",
            "version": "1.0.0",
            "components": {}
        }
        
        # Check database connection
        try:
            await db.execute("SELECT 1")
            health_status["components"]["database"] = {
                "status": "healthy",
                "message": "Database connection successful"
            }
        except Exception as e:
            health_status["components"]["database"] = {
                "status": "unhealthy",
                "message": f"Database connection failed: {str(e)}"
            }
            health_status["status"] = "unhealthy"
        
        # Check firmware connection
        try:
            firmware_status = await firmware_service.heartbeat()
            if firmware_status:
                health_status["components"]["firmware"] = {
                    "status": "healthy",
                    "message": "Firmware connection successful"
                }
            else:
                health_status["components"]["firmware"] = {
                    "status": "unhealthy",
                    "message": "Firmware connection failed"
                }
                health_status["status"] = "degraded"
        except Exception as e:
            health_status["components"]["firmware"] = {
                "status": "unhealthy",
                "message": f"Firmware connection error: {str(e)}"
            }
            health_status["status"] = "degraded"
        
        # Check real-time updates service
        try:
            realtime_stats = await realtime_service.get_service_stats()
            if realtime_stats.get("is_running", False):
                health_status["components"]["realtime_updates"] = {
                    "status": "healthy",
                    "message": "Real-time updates service running",
                    "stats": realtime_stats
                }
            else:
                health_status["components"]["realtime_updates"] = {
                    "status": "unhealthy",
                    "message": "Real-time updates service not running"
                }
                health_status["status"] = "degraded"
        except Exception as e:
            health_status["components"]["realtime_updates"] = {
                "status": "unhealthy",
                "message": f"Real-time updates service error: {str(e)}"
            }
            health_status["status"] = "degraded"
        
        # Check sensor data processor
        try:
            processing_stats = await sensor_processor.get_processing_stats()
            health_status["components"]["sensor_processor"] = {
                "status": "healthy",
                "message": "Sensor data processor operational",
                "stats": processing_stats
            }
        except Exception as e:
            health_status["components"]["sensor_processor"] = {
                "status": "unhealthy",
                "message": f"Sensor data processor error: {str(e)}"
            }
            health_status["status"] = "degraded"
        
        # Check system resources
        try:
            cpu_percent = psutil.cpu_percent(interval=1)
            memory = psutil.virtual_memory()
            disk = psutil.disk_usage('/')
            
            health_status["components"]["system_resources"] = {
                "status": "healthy",
                "cpu_usage": cpu_percent,
                "memory_usage": memory.percent,
                "disk_usage": disk.percent,
                "message": "System resources within normal limits"
            }
            
            # Check resource thresholds
            if cpu_percent > 80:
                health_status["components"]["system_resources"]["status"] = "warning"
                health_status["components"]["system_resources"]["message"] = "High CPU usage"
                if health_status["status"] == "healthy":
                    health_status["status"] = "degraded"
            
            if memory.percent > 85:
                health_status["components"]["system_resources"]["status"] = "warning"
                health_status["components"]["system_resources"]["message"] = "High memory usage"
                if health_status["status"] == "healthy":
                    health_status["status"] = "degraded"
            
            if disk.percent > 90:
                health_status["components"]["system_resources"]["status"] = "critical"
                health_status["components"]["system_resources"]["message"] = "Critical disk usage"
                health_status["status"] = "unhealthy"
                
        except Exception as e:
            health_status["components"]["system_resources"] = {
                "status": "unhealthy",
                "message": f"System resource check failed: {str(e)}"
            }
            health_status["status"] = "degraded"
        
        return health_status
        
    except Exception as e:
        logger.error("Detailed health check failed: %s", e)
        raise HTTPException(status_code=500, detail="Detailed health check failed")


@router.get("/readiness")
async def readiness_check(db: AsyncSession = Depends(get_db)):
    """
    Readiness check endpoint for Kubernetes
    
    Returns:
        Readiness status of the system
    """
    try:
        # Check if system is ready to accept requests
        readiness_status = {
            "status": "ready",
            "timestamp": datetime.utcnow().isoformat(),
            "checks": []
        }
        
        # Check database
        try:
            await db.execute("SELECT 1")
            readiness_status["checks"].append({
                "name": "database",
                "status": "ready"
            })
        except Exception as e:
            readiness_status["checks"].append({
                "name": "database",
                "status": "not_ready",
                "error": str(e)
            })
            readiness_status["status"] = "not_ready"
        
        # Check if real-time service is running
        try:
            realtime_stats = await realtime_service.get_service_stats()
            if realtime_stats.get("is_running", False):
                readiness_status["checks"].append({
                    "name": "realtime_service",
                    "status": "ready"
                })
            else:
                readiness_status["checks"].append({
                    "name": "realtime_service",
                    "status": "not_ready",
                    "error": "Service not running"
                })
                readiness_status["status"] = "not_ready"
        except Exception as e:
            readiness_status["checks"].append({
                "name": "realtime_service",
                "status": "not_ready",
                "error": str(e)
            })
            readiness_status["status"] = "not_ready"
        
        return readiness_status
        
    except Exception as e:
        logger.error("Readiness check failed: %s", e)
        raise HTTPException(status_code=500, detail="Readiness check failed")


@router.get("/liveness")
async def liveness_check():
    """
    Liveness check endpoint for Kubernetes
    
    Returns:
        Liveness status of the system
    """
    try:
        # Simple liveness check - if we can respond, we're alive
        return {
            "status": "alive",
            "timestamp": datetime.utcnow().isoformat(),
            "uptime": "System is running"
        }
    except Exception as e:
        logger.error("Liveness check failed: %s", e)
        raise HTTPException(status_code=500, detail="Liveness check failed")


@router.get("/metrics")
async def health_metrics():
    """
    Health metrics endpoint for monitoring
    
    Returns:
        System health metrics
    """
    try:
        metrics = {
            "timestamp": datetime.utcnow().isoformat(),
            "system": {},
            "services": {},
            "performance": {}
        }
        
        # System metrics
        cpu_percent = psutil.cpu_percent(interval=1)
        memory = psutil.virtual_memory()
        disk = psutil.disk_usage('/')
        
        metrics["system"] = {
            "cpu_usage_percent": cpu_percent,
            "memory_usage_percent": memory.percent,
            "memory_available_mb": memory.available / 1024 / 1024,
            "disk_usage_percent": disk.percent,
            "disk_free_gb": disk.free / 1024 / 1024 / 1024
        }
        
        # Service metrics
        try:
            firmware_status = await firmware_service.get_connection_status()
            metrics["services"]["firmware"] = firmware_status
        except Exception as e:
            metrics["services"]["firmware"] = {"error": str(e)}
        
        try:
            realtime_stats = await realtime_service.get_service_stats()
            metrics["services"]["realtime_updates"] = realtime_stats
        except Exception as e:
            metrics["services"]["realtime_updates"] = {"error": str(e)}
        
        try:
            processing_stats = await sensor_processor.get_processing_stats()
            metrics["services"]["sensor_processor"] = processing_stats
        except Exception as e:
            metrics["services"]["sensor_processor"] = {"error": str(e)}
        
        # Performance metrics
        metrics["performance"] = {
            "response_time_ms": 0,  # Would be measured in middleware
            "requests_per_second": 0,  # Would be tracked in middleware
            "error_rate_percent": 0,  # Would be tracked in middleware
            "active_connections": 0  # Would be tracked in middleware
        }
        
        return metrics
        
    except Exception as e:
        logger.error("Health metrics failed: %s", e)
        raise HTTPException(status_code=500, detail="Health metrics failed")


@router.get("/status")
async def status_check():
    """
    Status check endpoint for system status
    
    Returns:
        Current system status
    """
    try:
        status = {
            "timestamp": datetime.utcnow().isoformat(),
            "overall_status": "operational",
            "services": {},
            "alerts": []
        }
        
        # Check firmware status
        try:
            firmware_status = await firmware_service.get_connection_status()
            if firmware_status.get("is_healthy", False):
                status["services"]["firmware"] = "operational"
            else:
                status["services"]["firmware"] = "degraded"
                status["alerts"].append({
                    "level": "warning",
                    "message": "Firmware connection issues",
                    "service": "firmware"
                })
        except Exception as e:
            status["services"]["firmware"] = "down"
            status["alerts"].append({
                "level": "critical",
                "message": f"Firmware service error: {str(e)}",
                "service": "firmware"
            })
        
        # Check real-time service status
        try:
            realtime_stats = await realtime_service.get_service_stats()
            if realtime_stats.get("is_running", False):
                status["services"]["realtime_updates"] = "operational"
            else:
                status["services"]["realtime_updates"] = "down"
                status["alerts"].append({
                    "level": "critical",
                    "message": "Real-time updates service not running",
                    "service": "realtime_updates"
                })
        except Exception as e:
            status["services"]["realtime_updates"] = "down"
            status["alerts"].append({
                "level": "critical",
                "message": f"Real-time updates service error: {str(e)}",
                "service": "realtime_updates"
            })
        
        # Check system resources
        cpu_percent = psutil.cpu_percent(interval=1)
        memory = psutil.virtual_memory()
        disk = psutil.disk_usage('/')
        
        if cpu_percent > 90:
            status["alerts"].append({
                "level": "critical",
                "message": f"High CPU usage: {cpu_percent}%",
                "service": "system"
            })
            status["overall_status"] = "degraded"
        elif cpu_percent > 80:
            status["alerts"].append({
                "level": "warning",
                "message": f"Elevated CPU usage: {cpu_percent}%",
                "service": "system"
            })
        
        if memory.percent > 95:
            status["alerts"].append({
                "level": "critical",
                "message": f"Critical memory usage: {memory.percent}%",
                "service": "system"
            })
            status["overall_status"] = "degraded"
        elif memory.percent > 85:
            status["alerts"].append({
                "level": "warning",
                "message": f"High memory usage: {memory.percent}%",
                "service": "system"
            })
        
        if disk.percent > 95:
            status["alerts"].append({
                "level": "critical",
                "message": f"Critical disk usage: {disk.percent}%",
                "service": "system"
            })
            status["overall_status"] = "degraded"
        elif disk.percent > 90:
            status["alerts"].append({
                "level": "warning",
                "message": f"High disk usage: {disk.percent}%",
                "service": "system"
            })
        
        # Determine overall status
        if any(service_status == "down" for service_status in status["services"].values()):
            status["overall_status"] = "degraded"
        
        if any(alert["level"] == "critical" for alert in status["alerts"]):
            status["overall_status"] = "critical"
        
        return status
        
    except Exception as e:
        logger.error("Status check failed: %s", e)
        raise HTTPException(status_code=500, detail="Status check failed")
