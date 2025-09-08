"""
Dashboard API endpoints for OHT-50 Backend
"""

from datetime import datetime, timezone, timedelta
from typing import List, Dict, Any, Optional
from fastapi import APIRouter, Depends, HTTPException, status, Query
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select, text

from app.core.security import require_permission
from app.core.database import get_db
from app.models.user import User
from app.schemas.dashboard import (
    DashboardSummary,
    AlertResponse,
    LogResponse,
    PerformanceMetric,
    RobotSummary,
    SystemSummary,
    PerformanceDataResponse,
    AlertListResponse,
    LogListResponse,
    DashboardStats
)

router = APIRouter(prefix="/api/v1/dashboard", tags=["dashboard"])


@router.get("/summary", response_model=DashboardSummary)
async def get_dashboard_summary(
    current_user: User = Depends(require_permission("dashboard", "read")),
    db: AsyncSession = Depends(get_db)
):
    """Get dashboard overview data - robot status, alerts, performance"""
    try:
        # Get robot status (mock data for now)
        robot_summary = RobotSummary(
            robot_id="OHT-50-001",
            status="idle",
            mode="auto",
            position={"x": 150.5, "y": 200.3, "z": 0.0},
            battery_level=87,
            temperature=42.5,
            speed=0.0,
            last_command="stop",
            uptime=3600,
            health_score=95.0
        )
        
        # Get system summary
        system_summary = SystemSummary(
            total_alerts=5,
            active_alerts=2,
            critical_alerts=0,
            system_uptime=86400,
            total_users=10,
            active_users=3,
            last_backup=datetime.now(timezone.utc) - timedelta(hours=6),
            database_size=125.5
        )
        
        # Get current performance metrics
        performance = PerformanceMetric(
            timestamp=datetime.now(timezone.utc),
            cpu_usage=45.2,
            memory_usage=67.8,
            disk_usage=23.4,
            network_in=1024.5,
            network_out=2048.7,
            response_time=25.3,
            throughput=150.0
        )
        
        return DashboardSummary(
            robot=robot_summary,
            system=system_summary,
            performance=performance,
            last_updated=datetime.now(timezone.utc)
        )
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get dashboard summary: {str(e)}"
        )


@router.get("/alerts", response_model=AlertListResponse)
async def get_active_alerts(
    page: int = Query(1, ge=1, description="Page number"),
    page_size: int = Query(50, ge=1, le=100, description="Items per page"),
    severity: Optional[str] = Query(None, description="Filter by severity"),
    current_user: User = Depends(require_permission("dashboard", "read")),
    db: AsyncSession = Depends(get_db)
):
    """Get active alerts list with severity levels"""
    try:
        # Mock alerts data
        alerts = [
            AlertResponse(
                id=1,
                title="Low Battery Warning",
                message="Robot battery level is below 20%",
                severity="medium",
                status="active",
                source="battery_monitor",
                timestamp=datetime.now(timezone.utc) - timedelta(minutes=30),
                acknowledged_by=None,
                acknowledged_at=None,
                resolved_at=None,
                metadata={"battery_level": 18, "threshold": 20}
            ),
            AlertResponse(
                id=2,
                title="Temperature Alert",
                message="Robot temperature is above normal range",
                severity="high",
                status="active",
                source="temperature_sensor",
                timestamp=datetime.now(timezone.utc) - timedelta(minutes=15),
                acknowledged_by=None,
                acknowledged_at=None,
                resolved_at=None,
                metadata={"temperature": 75.5, "normal_max": 70.0}
            ),
            AlertResponse(
                id=3,
                title="Network Connection Lost",
                message="Lost connection to center system",
                severity="critical",
                status="acknowledged",
                source="network_monitor",
                timestamp=datetime.now(timezone.utc) - timedelta(hours=1),
                acknowledged_by="admin",
                acknowledged_at=datetime.now(timezone.utc) - timedelta(minutes=30),
                resolved_at=None,
                metadata={"connection_type": "ethernet", "retry_count": 5}
            )
        ]
        
        # Filter by severity if specified
        if severity:
            alerts = [alert for alert in alerts if alert.severity == severity]
        
        # Pagination
        total_count = len(alerts)
        start_idx = (page - 1) * page_size
        end_idx = start_idx + page_size
        paginated_alerts = alerts[start_idx:end_idx]
        
        return AlertListResponse(
            alerts=paginated_alerts,
            total_count=total_count,
            page=page,
            page_size=page_size,
            has_next=end_idx < total_count
        )
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get alerts: {str(e)}"
        )


@router.get("/logs", response_model=LogListResponse)
async def get_system_logs(
    limit: int = Query(50, ge=1, le=1000, description="Number of log entries"),
    level: str = Query("all", description="Log level filter"),
    page: int = Query(1, ge=1, description="Page number"),
    current_user: User = Depends(require_permission("dashboard", "read")),
    db: AsyncSession = Depends(get_db)
):
    """Get system logs for dashboard display"""
    try:
        # Mock logs data
        logs = [
            LogResponse(
                id=1,
                level="info",
                message="Robot started successfully",
                source="robot_controller",
                timestamp=datetime.now(timezone.utc) - timedelta(hours=2),
                user_id=1,
                session_id="session_001",
                metadata={"startup_time": 2.5, "version": "1.0.0"}
            ),
            LogResponse(
                id=2,
                level="warning",
                message="Battery level low",
                source="battery_monitor",
                timestamp=datetime.now(timezone.utc) - timedelta(minutes=45),
                user_id=None,
                session_id=None,
                metadata={"battery_level": 18, "threshold": 20}
            ),
            LogResponse(
                id=3,
                level="error",
                message="Failed to connect to center system",
                source="network_manager",
                timestamp=datetime.now(timezone.utc) - timedelta(hours=1),
                user_id=None,
                session_id=None,
                metadata={"error_code": "CONN_TIMEOUT", "retry_count": 3}
            ),
            LogResponse(
                id=4,
                level="debug",
                message="Position update received",
                source="localization",
                timestamp=datetime.now(timezone.utc) - timedelta(minutes=5),
                user_id=None,
                session_id=None,
                metadata={"x": 150.5, "y": 200.3, "accuracy": 0.1}
            )
        ]
        
        # Filter by level if specified
        if level != "all":
            logs = [log for log in logs if log.level == level]
        
        # Pagination
        total_count = len(logs)
        page_size = min(limit, 100)  # Limit page size
        start_idx = (page - 1) * page_size
        end_idx = start_idx + page_size
        paginated_logs = logs[start_idx:end_idx]
        
        return LogListResponse(
            logs=paginated_logs,
            total_count=total_count,
            page=page,
            page_size=page_size,
            has_next=end_idx < total_count
        )
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get system logs: {str(e)}"
        )


@router.get("/performance", response_model=PerformanceDataResponse)
async def get_performance_data(
    hours: int = Query(24, ge=1, le=168, description="Hours of data to retrieve"),
    current_user: User = Depends(require_permission("dashboard", "read")),
    db: AsyncSession = Depends(get_db)
):
    """Get performance data for charts"""
    try:
        # Generate mock performance data
        metrics = []
        now = datetime.now(timezone.utc)
        
        for i in range(hours):
            timestamp = now - timedelta(hours=i)
            metrics.append(PerformanceMetric(
                timestamp=timestamp,
                cpu_usage=45.0 + (i % 10) * 2.5,
                memory_usage=60.0 + (i % 15) * 1.8,
                disk_usage=20.0 + (i % 5) * 0.5,
                network_in=1000.0 + (i % 20) * 50.0,
                network_out=2000.0 + (i % 25) * 80.0,
                response_time=20.0 + (i % 8) * 3.0,
                throughput=120.0 + (i % 12) * 10.0
            ))
        
        # Reverse to get chronological order
        metrics.reverse()
        
        return PerformanceDataResponse(
            metrics=metrics,
            time_range=f"{hours} hours",
            interval="1 hour",
            total_points=len(metrics)
        )
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get performance data: {str(e)}"
        )


@router.get("/stats", response_model=DashboardStats)
async def get_dashboard_stats(
    current_user: User = Depends(require_permission("dashboard", "read")),
    db: AsyncSession = Depends(get_db)
):
    """Get dashboard statistics"""
    try:
        # Mock stats data
        stats = DashboardStats(
            total_requests=15420,
            successful_requests=15380,
            failed_requests=40,
            average_response_time=28.5,
            error_rate=0.26,
            active_connections=5,
            last_24h_requests=1250
        )
        
        return stats
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to get dashboard stats: {str(e)}"
        )


@router.post("/alerts/{alert_id}/acknowledge")
async def acknowledge_alert(
    alert_id: int,
    current_user: User = Depends(require_permission("dashboard", "write")),
    db: AsyncSession = Depends(get_db)
):
    """Acknowledge an alert"""
    try:
        # In a real implementation, update the alert in database
        # For now, return success
        return {
            "success": True,
            "message": f"Alert {alert_id} acknowledged successfully",
            "acknowledged_by": current_user.username,
            "acknowledged_at": datetime.now(timezone.utc)
        }
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to acknowledge alert: {str(e)}"
        )


@router.post("/alerts/{alert_id}/resolve")
async def resolve_alert(
    alert_id: int,
    current_user: User = Depends(require_permission("dashboard", "write")),
    db: AsyncSession = Depends(get_db)
):
    """Resolve an alert"""
    try:
        # In a real implementation, update the alert in database
        # For now, return success
        return {
            "success": True,
            "message": f"Alert {alert_id} resolved successfully",
            "resolved_by": current_user.username,
            "resolved_at": datetime.now(timezone.utc)
        }
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_500_INTERNAL_SERVER_ERROR,
            detail=f"Failed to resolve alert: {str(e)}"
        )
