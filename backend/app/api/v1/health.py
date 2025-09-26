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
from app.services.unified_firmware_service import get_firmware_service

logger = logging.getLogger(__name__)

router = APIRouter(prefix="/api/v1/health", tags=["health"])


@router.get("/")
async def basic_health_check():
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
    try:
        health_status = {
            "status": "healthy",
            "timestamp": datetime.utcnow().isoformat(),
            "service": "OHT-50 Backend",
            "version": "1.0.0",
            "components": {}
        }
        
        # DB
        try:
            await db.execute("SELECT 1")
            health_status["components"]["database"] = {"status": "healthy", "message": "Database connection successful"}
        except Exception as e:
            health_status["components"]["database"] = {"status": "unhealthy", "message": f"Database connection failed: {str(e)}"}
            health_status["status"] = "unhealthy"
        
        # Firmware
        try:
            fw = await get_firmware_service()
            status = fw.get_health_status()
            if status.get("firmware_connected"):
                health_status["components"]["firmware"] = {"status": "healthy", "message": "Firmware connection successful"}
            else:
                health_status["components"]["firmware"] = {"status": "unhealthy", "message": "Firmware connection failed"}
                health_status["status"] = "degraded"
        except Exception as e:
            health_status["components"]["firmware"] = {"status": "unhealthy", "message": f"Firmware connection error: {str(e)}"}
            health_status["status"] = "degraded"
        
        # System resources
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
            if cpu_percent > 80 or memory.percent > 85 or disk.percent > 90:
                health_status["status"] = "degraded"
        except Exception as e:
            health_status["components"]["system_resources"] = {"status": "unhealthy", "message": f"System resource check failed: {str(e)}"}
            health_status["status"] = "degraded"
        
        return health_status
        
    except Exception as e:
        logger.error("Detailed health check failed: %s", e)
        raise HTTPException(status_code=500, detail="Detailed health check failed")


@router.get("/metrics")
async def health_metrics():
    try:
        metrics = {
            "timestamp": datetime.utcnow().isoformat(),
            "system": {},
            "services": {},
            "performance": {}
        }
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
        try:
            fw = await get_firmware_service()
            metrics["services"]["firmware"] = fw.get_health_status()
        except Exception as e:
            metrics["services"]["firmware"] = {"error": str(e)}
        return metrics
    except Exception as e:
        logger.error("Health metrics failed: %s", e)
        raise HTTPException(status_code=500, detail="Health metrics failed")


@router.get("/status")
async def status_check():
    try:
        status = {
            "timestamp": datetime.utcnow().isoformat(),
            "overall_status": "operational",
            "services": {},
            "alerts": []
        }
        try:
            fw = await get_firmware_service()
            fw_status = fw.get_health_status()
            status["services"]["firmware"] = "operational" if fw_status.get("firmware_connected") else "degraded"
        except Exception as e:
            status["services"]["firmware"] = "down"
            status["alerts"].append({"level": "critical", "message": f"Firmware service error: {str(e)}", "service": "firmware"})
        cpu_percent = psutil.cpu_percent(interval=1)
        memory = psutil.virtual_memory()
        disk = psutil.disk_usage('/')
        if cpu_percent > 90:
            status["alerts"].append({"level": "critical", "message": f"High CPU usage: {cpu_percent}%", "service": "system"})
            status["overall_status"] = "degraded"
        elif cpu_percent > 80:
            status["alerts"].append({"level": "warning", "message": f"Elevated CPU usage: {cpu_percent}%", "service": "system"})
        if memory.percent > 95:
            status["alerts"].append({"level": "critical", "message": f"Critical memory usage: {memory.percent}%", "service": "system"})
            status["overall_status"] = "degraded"
        elif memory.percent > 85:
            status["alerts"].append({"level": "warning", "message": f"High memory usage: {memory.percent}%", "service": "system"})
        if disk.percent > 95:
            status["alerts"].append({"level": "critical", "message": f"Critical disk usage: {disk.percent}%", "service": "system"})
            status["overall_status"] = "degraded"
        elif disk.percent > 90:
            status["alerts"].append({"level": "warning", "message": f"High disk usage: {disk.percent}%", "service": "system"})
        if any(s == "down" for s in status["services"].values()):
            status["overall_status"] = "degraded"
        if any(a["level"] == "critical" for a in status["alerts"]):
            status["overall_status"] = "critical"
        return status
    except Exception as e:
        logger.error("Status check failed: %s", e)
        raise HTTPException(status_code=500, detail="Status check failed")
