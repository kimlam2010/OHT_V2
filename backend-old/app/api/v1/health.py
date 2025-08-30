"""
Health check API endpoints cho OHT-50 Backend
"""
import time
from datetime import datetime
from typing import Any, Dict

import psutil
from fastapi import APIRouter, status
from fastapi.responses import JSONResponse

from app.config import get_settings
from app.core.logging import get_logger

# Router
router = APIRouter()

# Logger
logger = get_logger(__name__)

# Settings
settings = get_settings()

# Startup time
startup_time = datetime.now()


@router.get("/", tags=["Health"])
async def health_check() -> Dict[str, Any]:
    """Basic health check endpoint"""
    return {
        "success": True,
        "status": "healthy",
        "timestamp": datetime.now().isoformat(),
        "service": "OHT-50 Backend",
        "version": "1.0.0",
    }


@router.get("/detailed", tags=["Health"])
async def detailed_health_check() -> Dict[str, Any]:
    """Detailed health check với system metrics"""
    try:
        # System metrics
        cpu_percent = psutil.cpu_percent(interval=1)
        memory = psutil.virtual_memory()
        disk = psutil.disk_usage("/")

        # Uptime
        uptime = datetime.now() - startup_time

        # Service status (placeholder - sẽ implement sau)
        services_status = {
            "database": "healthy",  # Placeholder
            "telemetry": "healthy",  # Placeholder
            "center_communication": "healthy",  # Placeholder
            "rs485_communication": "healthy",  # Placeholder
        }

        health_data = {
            "success": True,
            "status": "healthy",
            "timestamp": datetime.now().isoformat(),
            "service": "OHT-50 Backend",
            "version": "1.0.0",
            "uptime_seconds": int(uptime.total_seconds()),
            "system": {
                "cpu_percent": cpu_percent,
                "memory_percent": memory.percent,
                "memory_available_gb": round(memory.available / (1024**3), 2),
                "disk_percent": disk.percent,
                "disk_free_gb": round(disk.free / (1024**3), 2),
            },
            "services": services_status,
            "settings": {
                "host": settings.host,
                "port": settings.port,
                "debug": settings.debug,
                "log_level": settings.log_level,
            },
        }

        logger.info(
            "Health check completed",
            extra={
                "cpu_percent": cpu_percent,
                "memory_percent": memory.percent,
                "uptime_seconds": int(uptime.total_seconds()),
            },
        )

        return health_data

    except Exception as e:
        logger.error("Health check failed", extra={"error": str(e)})

        return JSONResponse(
            status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
            content={
                "success": False,
                "status": "unhealthy",
                "timestamp": datetime.now().isoformat(),
                "error": str(e),
            },
        )


@router.get("/ready", tags=["Health"])
async def readiness_check() -> Dict[str, Any]:
    """Readiness check - service sẵn sàng nhận traffic"""
    try:
        # Kiểm tra các dependencies
        checks = {
            "database": True,  # Placeholder
            "telemetry_service": True,  # Placeholder
            "center_connection": True,  # Placeholder
        }

        all_ready = all(checks.values())

        if all_ready:
            return {
                "success": True,
                "status": "ready",
                "timestamp": datetime.now().isoformat(),
                "checks": checks,
            }
        else:
            return JSONResponse(
                status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
                content={
                    "success": False,
                    "status": "not_ready",
                    "timestamp": datetime.now().isoformat(),
                    "checks": checks,
                },
            )

    except Exception as e:
        logger.error("Readiness check failed", extra={"error": str(e)})

        return JSONResponse(
            status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
            content={
                "success": False,
                "status": "not_ready",
                "timestamp": datetime.now().isoformat(),
                "error": str(e),
            },
        )


@router.get("/live", tags=["Health"])
async def liveness_check() -> Dict[str, Any]:
    """Liveness check - service đang chạy"""
    return {
        "success": True,
        "status": "alive",
        "timestamp": datetime.now().isoformat(),
        "uptime_seconds": int((datetime.now() - startup_time).total_seconds()),
    }
