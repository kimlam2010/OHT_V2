"""
Monitoring API endpoints for production environment

This module provides API endpoints for system monitoring, metrics, and alerts.
"""

import logging
from typing import Dict, Any, List
from fastapi import APIRouter, HTTPException, status, Depends
from fastapi.responses import JSONResponse

from app.core.monitoring_service import monitoring_service
from app.core.production_rbac import require_permission, require_role
from app.models.user import User
from app.core.security import get_current_user

logger = logging.getLogger(__name__)

router = APIRouter(prefix="/api/v1/monitoring", tags=["monitoring"])


@router.get("/health")
async def get_system_health(
    current_user: User = Depends(require_permission("system", "read"))
) -> Dict[str, Any]:
    """Get overall system health status"""
    try:
        health_data = await monitoring_service.get_system_health()
        return {
            "success": True,
            "data": health_data,
            "message": "System health retrieved successfully"
        }
    except Exception as e:
        logger.error(f"❌ System health check failed: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to retrieve system health"
        )


@router.get("/metrics")
async def get_current_metrics(
    current_user: User = Depends(require_permission("system", "read"))
) -> Dict[str, Any]:
    """Get current system metrics"""
    try:
        metrics = await monitoring_service.get_current_metrics()
        
        return {
            "success": True,
            "data": {
                "timestamp": metrics.timestamp.isoformat(),
                "cpu_percent": metrics.cpu_percent,
                "memory_percent": metrics.memory_percent,
                "disk_percent": metrics.disk_percent,
                "network_bytes_sent": metrics.network_bytes_sent,
                "network_bytes_recv": metrics.network_bytes_recv,
                "active_connections": metrics.active_connections,
                "uptime_seconds": metrics.uptime
            },
            "message": "Current metrics retrieved successfully"
        }
    except Exception as e:
        logger.error(f"❌ Current metrics retrieval failed: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to retrieve current metrics"
        )


@router.get("/metrics/history")
async def get_metrics_history(
    hours: int = 24,
    current_user: User = Depends(require_permission("system", "read"))
) -> Dict[str, Any]:
    """Get metrics history for specified hours"""
    try:
        if hours < 1 or hours > 168:  # Max 1 week
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Hours must be between 1 and 168"
            )
        
        metrics = await monitoring_service.get_metrics_history(hours)
        
        metrics_data = []
        for metric in metrics:
            metrics_data.append({
                "timestamp": metric.timestamp.isoformat(),
                "cpu_percent": metric.cpu_percent,
                "memory_percent": metric.memory_percent,
                "disk_percent": metric.disk_percent,
                "network_bytes_sent": metric.network_bytes_sent,
                "network_bytes_recv": metric.network_bytes_recv,
                "active_connections": metric.active_connections,
                "uptime_seconds": metric.uptime
            })
        
        return {
            "success": True,
            "data": {
                "period_hours": hours,
                "metrics_count": len(metrics_data),
                "metrics": metrics_data
            },
            "message": f"Metrics history for {hours} hours retrieved successfully"
        }
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Metrics history retrieval failed: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to retrieve metrics history"
        )


@router.get("/performance/summary")
async def get_performance_summary(
    hours: int = 24,
    current_user: User = Depends(require_permission("system", "read"))
) -> Dict[str, Any]:
    """Get performance summary for specified hours"""
    try:
        if hours < 1 or hours > 168:  # Max 1 week
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Hours must be between 1 and 168"
            )
        
        summary = await monitoring_service.get_performance_summary(hours)
        
        if "error" in summary:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail=summary["error"]
            )
        
        return {
            "success": True,
            "data": summary,
            "message": f"Performance summary for {hours} hours retrieved successfully"
        }
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Performance summary retrieval failed: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to retrieve performance summary"
        )


@router.get("/alerts/active")
async def get_active_alerts(
    current_user: User = Depends(require_permission("system", "read"))
) -> Dict[str, Any]:
    """Get all active alerts"""
    try:
        alerts = await monitoring_service.get_active_alerts()
        
        alerts_data = []
        for alert in alerts:
            alerts_data.append({
                "id": alert.id,
                "level": alert.level,
                "message": alert.message,
                "timestamp": alert.timestamp.isoformat(),
                "acknowledged": alert.acknowledged,
                "resolved": alert.resolved
            })
        
        return {
            "success": True,
            "data": {
                "alerts_count": len(alerts_data),
                "alerts": alerts_data
            },
            "message": "Active alerts retrieved successfully"
        }
    except Exception as e:
        logger.error(f"❌ Active alerts retrieval failed: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to retrieve active alerts"
        )


@router.get("/alerts/by-level/{level}")
async def get_alerts_by_level(
    level: str,
    current_user: User = Depends(require_permission("system", "read"))
) -> Dict[str, Any]:
    """Get alerts by severity level"""
    try:
        if level not in ["critical", "warning", "info"]:
            raise HTTPException(
                status_code=status.HTTP_400_BAD_REQUEST,
                detail="Level must be critical, warning, or info"
            )
        
        alerts = await monitoring_service.get_alerts_by_level(level)
        
        alerts_data = []
        for alert in alerts:
            alerts_data.append({
                "id": alert.id,
                "level": alert.level,
                "message": alert.message,
                "timestamp": alert.timestamp.isoformat(),
                "acknowledged": alert.acknowledged,
                "resolved": alert.resolved
            })
        
        return {
            "success": True,
            "data": {
                "level": level,
                "alerts_count": len(alerts_data),
                "alerts": alerts_data
            },
            "message": f"{level.capitalize()} alerts retrieved successfully"
        }
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Alerts by level retrieval failed: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to retrieve alerts by level"
        )


@router.post("/alerts/{alert_id}/acknowledge")
async def acknowledge_alert(
    alert_id: str,
    current_user: User = Depends(require_permission("system", "write"))
) -> Dict[str, Any]:
    """Acknowledge an alert"""
    try:
        success = await monitoring_service.acknowledge_alert(alert_id)
        
        if success:
            return {
                "success": True,
                "message": f"Alert {alert_id} acknowledged successfully"
            }
        else:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail=f"Alert {alert_id} not found"
            )
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Alert acknowledgement failed: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to acknowledge alert"
        )


@router.post("/alerts/{alert_id}/resolve")
async def resolve_alert(
    alert_id: str,
    current_user: User = Depends(require_permission("system", "write"))
) -> Dict[str, Any]:
    """Resolve an alert"""
    try:
        success = await monitoring_service.resolve_alert(alert_id)
        
        if success:
            return {
                "success": True,
                "message": f"Alert {alert_id} resolved successfully"
            }
        else:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail=f"Alert {alert_id} not found"
            )
    except HTTPException:
        raise
    except Exception as e:
        logger.error(f"❌ Alert resolution failed: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to resolve alert"
        )


@router.delete("/alerts/clear-resolved")
async def clear_resolved_alerts(
    current_user: User = Depends(require_role("admin"))
) -> Dict[str, Any]:
    """Clear all resolved alerts (admin only)"""
    try:
        cleared_count = await monitoring_service.clear_resolved_alerts()
        
        return {
            "success": True,
            "data": {"cleared_count": cleared_count},
            "message": f"Cleared {cleared_count} resolved alerts"
        }
    except Exception as e:
        logger.error(f"❌ Clear resolved alerts failed: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to clear resolved alerts"
        )


@router.get("/status")
async def get_monitoring_status(
    current_user: User = Depends(require_permission("system", "read"))
) -> Dict[str, Any]:
    """Get monitoring service status"""
    try:
        # Get current metrics
        current_metrics = await monitoring_service.get_current_metrics()
        
        # Get active alerts count
        active_alerts = await monitoring_service.get_active_alerts()
        
        # Get system health
        health = await monitoring_service.get_system_health()
        
        return {
            "success": True,
            "data": {
                "monitoring_service": "active",
                "last_metrics_collection": current_metrics.timestamp.isoformat(),
                "active_alerts_count": len(active_alerts),
                "system_health": health["status"],
                "overall_health_score": health.get("overall_health_score", 0),
                "thresholds": monitoring_service.thresholds
            },
            "message": "Monitoring status retrieved successfully"
        }
    except Exception as e:
        logger.error(f"❌ Monitoring status retrieval failed: {e}")
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail="Failed to retrieve monitoring status"
        )
