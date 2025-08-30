"""
Analytics API endpoints
"""

from fastapi import APIRouter, Depends, HTTPException, status, Query
from typing import List, Optional, Dict, Any

from app.services.analytics_service import AnalyticsService
from app.services.log_service import LogService

router = APIRouter(tags=["Analytics"])


def get_analytics_service() -> AnalyticsService:
    """Dependency injection cho AnalyticsService"""
    log_service = LogService()
    return AnalyticsService(log_service)


@router.post("/metrics/collect")
async def collect_metrics(
    metrics_data: Dict[str, Any],
    analytics_service: AnalyticsService = Depends(get_analytics_service)
):
    """Thu thập performance metrics"""
    try:
        await analytics_service.collect_performance_metrics(metrics_data)
        return {"message": "Metrics collected successfully"}
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error collecting metrics: {str(e)}"
        )


@router.get("/performance/summary")
async def get_performance_summary(
    hours: int = Query(24, ge=1, le=168, description="Số giờ để lấy dữ liệu"),
    analytics_service: AnalyticsService = Depends(get_analytics_service)
):
    """Lấy performance summary"""
    try:
        summary = await analytics_service.get_performance_summary(hours)
        return summary
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting performance summary: {str(e)}"
        )


@router.get("/trends/{metric_name}")
async def get_trend_analysis(
    metric_name: str,
    hours: int = Query(24, ge=1, le=168, description="Số giờ để phân tích"),
    analytics_service: AnalyticsService = Depends(get_analytics_service)
):
    """Phân tích trend của metric"""
    try:
        analysis = await analytics_service.get_trend_analysis(metric_name, hours)
        return analysis
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error analyzing trend: {str(e)}"
        )


@router.post("/alerts")
async def create_alert(
    alert_type: str,
    severity: str,
    message: str,
    data: Optional[Dict[str, Any]] = None,
    analytics_service: AnalyticsService = Depends(get_analytics_service)
):
    """Tạo alert mới"""
    try:
        alert = await analytics_service.generate_alert(alert_type, severity, message, data)
        return alert
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error creating alert: {str(e)}"
        )


@router.get("/alerts")
async def get_alerts(
    severity: Optional[str] = Query(None, description="Filter theo severity"),
    acknowledged: Optional[bool] = Query(None, description="Filter theo acknowledged status"),
    limit: int = Query(100, ge=1, le=500, description="Số lượng alerts tối đa"),
    analytics_service: AnalyticsService = Depends(get_analytics_service)
):
    """Lấy danh sách alerts"""
    try:
        alerts = await analytics_service.get_alerts(severity, acknowledged, limit)
        return alerts
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting alerts: {str(e)}"
        )


@router.put("/alerts/{alert_id}/acknowledge")
async def acknowledge_alert(
    alert_id: str,
    analytics_service: AnalyticsService = Depends(get_analytics_service)
):
    """Acknowledge alert"""
    try:
        success = await analytics_service.acknowledge_alert(alert_id)
        if success:
            return {"message": "Alert acknowledged successfully"}
        else:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="Alert not found"
            )
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error acknowledging alert: {str(e)}"
        )


@router.put("/alerts/{alert_id}/resolve")
async def resolve_alert(
    alert_id: str,
    analytics_service: AnalyticsService = Depends(get_analytics_service)
):
    """Resolve alert"""
    try:
        success = await analytics_service.resolve_alert(alert_id)
        if success:
            return {"message": "Alert resolved successfully"}
        else:
            raise HTTPException(
                status_code=status.HTTP_404_NOT_FOUND,
                detail="Alert not found"
            )
    except HTTPException:
        raise
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error resolving alert: {str(e)}"
        )


@router.get("/health/score")
async def get_system_health_score(
    analytics_service: AnalyticsService = Depends(get_analytics_service)
):
    """Lấy system health score"""
    try:
        health_score = await analytics_service.get_system_health_score()
        return health_score
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting health score: {str(e)}"
        )


@router.get("/insights/predictive")
async def get_predictive_insights(
    analytics_service: AnalyticsService = Depends(get_analytics_service)
):
    """Lấy predictive insights"""
    try:
        insights = await analytics_service.get_predictive_insights()
        return insights
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting predictive insights: {str(e)}"
        )


@router.get("/metrics/available")
async def get_available_metrics(
    analytics_service: AnalyticsService = Depends(get_analytics_service)
):
    """Lấy danh sách metrics có sẵn"""
    try:
        # Lấy danh sách metrics từ performance_data
        available_metrics = list(analytics_service.performance_data.keys())
        
        return {
            "available_metrics": available_metrics,
            "total_metrics": len(available_metrics),
            "timestamp": analytics_service.metrics_history[-1]["timestamp"] if analytics_service.metrics_history else None
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting available metrics: {str(e)}"
        )


@router.get("/dashboard/summary")
async def get_dashboard_summary(
    analytics_service: AnalyticsService = Depends(get_analytics_service)
):
    """Lấy dashboard summary"""
    try:
        # Lấy các thông tin tổng hợp cho dashboard
        health_score = await analytics_service.get_system_health_score()
        performance_summary = await analytics_service.get_performance_summary(hours=1)
        alerts = await analytics_service.get_alerts(acknowledged=False, limit=10)
        insights = await analytics_service.get_predictive_insights()
        
        return {
            "health_score": health_score,
            "performance_summary": performance_summary,
            "active_alerts": alerts,
            "insights": insights,
            "timestamp": health_score.get("timestamp")
        }
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error getting dashboard summary: {str(e)}"
        )


@router.get("/reports/performance")
async def generate_performance_report(
    hours: int = Query(24, ge=1, le=168, description="Số giờ cho báo cáo"),
    analytics_service: AnalyticsService = Depends(get_analytics_service)
):
    """Tạo performance report"""
    try:
        # Lấy performance summary
        summary = await analytics_service.get_performance_summary(hours)
        
        # Lấy health score
        health_score = await analytics_service.get_system_health_score()
        
        # Lấy alerts trong khoảng thời gian
        alerts = await analytics_service.get_alerts(limit=50)
        
        # Tạo report
        report = {
            "report_type": "performance",
            "time_range_hours": hours,
            "generated_at": health_score.get("timestamp"),
            "summary": summary,
            "health_score": health_score,
            "alerts_summary": {
                "total_alerts": len(alerts),
                "critical_alerts": len([a for a in alerts if a.get("severity") == "critical"]),
                "warning_alerts": len([a for a in alerts if a.get("severity") == "warning"]),
                "info_alerts": len([a for a in alerts if a.get("severity") == "info"])
            },
            "recommendations": []
        }
        
        # Thêm recommendations dựa trên data
        if "health_score" in health_score and health_score["health_score"] < 0.7:
            report["recommendations"].append("System health is below optimal level, consider investigation")
        
        if summary.get("metrics"):
            for metric_name, metric_data in summary["metrics"].items():
                if metric_name in ["cpu_usage", "memory_usage"] and metric_data["avg"] > 80:
                    report["recommendations"].append(f"High {metric_name} detected, consider resource scaling")
        
        return report
        
    except Exception as e:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=f"Error generating performance report: {str(e)}"
        )
