"""
AnalyticsService - Analytics và monitoring cho OHT-50
"""

import asyncio
import logging
import math
from datetime import datetime, timedelta
from typing import List, Optional, Dict, Any
from uuid import UUID

from app.services.log_service import LogService


logger = logging.getLogger(__name__)


class AnalyticsService:
    """Service analytics và monitoring"""

    def __init__(self, log_service: LogService):
        self.log_service = log_service
        self.metrics_history: List[Dict[str, Any]] = []
        self.performance_data: Dict[str, List[float]] = {}
        self.alert_history: List[Dict[str, Any]] = []
        self._lock = asyncio.Lock()

        logger.info("AnalyticsService initialized")

    async def collect_performance_metrics(self, metrics_data: Dict[str, Any]):
        """Thu thập performance metrics"""
        async with self._lock:
            try:
                timestamp = datetime.utcnow()

                # Thêm timestamp
                metrics_data["timestamp"] = timestamp.isoformat()
                metrics_data["timestamp_unix"] = timestamp.timestamp()

                # Lưu vào history
                self.metrics_history.append(metrics_data)

                # Giữ lịch sử trong giới hạn (1000 records)
                if len(self.metrics_history) > 1000:
                    self.metrics_history = self.metrics_history[-1000:]

                # Cập nhật performance data
                for key, value in metrics_data.items():
                    if isinstance(value, (int, float)) and key != "timestamp_unix":
                        if key not in self.performance_data:
                            self.performance_data[key] = []
                        self.performance_data[key].append(value)

                        # Giữ 1000 values gần nhất
                        if len(self.performance_data[key]) > 1000:
                            self.performance_data[key] = self.performance_data[key][
                                -1000:
                            ]

                logger.debug(
                    f"Collected performance metrics: {len(metrics_data)} metrics"
                )

            except Exception as e:
                logger.error(f"Error collecting performance metrics: {e}")

    async def get_performance_summary(self, hours: int = 24) -> Dict[str, Any]:
        """Lấy summary performance trong khoảng thời gian"""
        async with self._lock:
            try:
                cutoff_time = datetime.utcnow() - timedelta(hours=hours)

                # Lọc metrics trong khoảng thời gian
                recent_metrics = [
                    m
                    for m in self.metrics_history
                    if datetime.fromisoformat(m["timestamp"]) >= cutoff_time
                ]

                if not recent_metrics:
                    return {
                        "message": "No metrics available for the specified time range"
                    }

                # Tính toán statistics
                summary = {
                    "time_range_hours": hours,
                    "total_records": len(recent_metrics),
                    "metrics": {},
                }

                # Tính statistics cho từng metric
                for key in recent_metrics[0].keys():
                    if key not in ["timestamp", "timestamp_unix"]:
                        values = [
                            m.get(key)
                            for m in recent_metrics
                            if isinstance(m.get(key), (int, float))
                        ]
                        if values:
                            summary["metrics"][key] = {
                                "count": len(values),
                                "min": min(values),
                                "max": max(values),
                                "avg": sum(values) / len(values),
                                "std": self._calculate_std(values),
                            }

                return summary

            except Exception as e:
                logger.error(f"Error getting performance summary: {e}")
                raise

    async def get_trend_analysis(
        self, metric_name: str, hours: int = 24
    ) -> Dict[str, Any]:
        """Phân tích trend của một metric"""
        async with self._lock:
            try:
                if metric_name not in self.performance_data:
                    return {"error": f"Metric '{metric_name}' not found"}

                values = self.performance_data[metric_name]

                if len(values) < 2:
                    return {"error": "Insufficient data for trend analysis"}

                # Tính trend
                trend = self._calculate_trend(values)

                # Phân tích pattern
                pattern = self._analyze_pattern(values)

                # Dự đoán (đơn giản)
                prediction = self._simple_prediction(values)

                return {
                    "metric_name": metric_name,
                    "time_range_hours": hours,
                    "data_points": len(values),
                    "trend": trend,
                    "pattern": pattern,
                    "prediction": prediction,
                    "current_value": values[-1] if values else None,
                    "min_value": min(values),
                    "max_value": max(values),
                    "avg_value": sum(values) / len(values),
                }

            except Exception as e:
                logger.error(f"Error getting trend analysis: {e}")
                raise

    async def generate_alert(
        self,
        alert_type: str,
        severity: str,
        message: str,
        data: Optional[Dict[str, Any]] = None,
    ):
        """Tạo alert"""
        async with self._lock:
            try:
                alert = {
                    "id": str(UUID.uuid4()),
                    "timestamp": datetime.utcnow().isoformat(),
                    "type": alert_type,
                    "severity": severity,
                    "message": message,
                    "data": data or {},
                    "acknowledged": False,
                    "resolved": False,
                }

                self.alert_history.append(alert)

                # Giữ lịch sử trong giới hạn
                if len(self.alert_history) > 500:
                    self.alert_history = self.alert_history[-500:]

                logger.warning(f"Alert generated: {alert_type} - {message}")

                return alert

            except Exception as e:
                logger.error(f"Error generating alert: {e}")
                raise

    async def get_alerts(
        self,
        severity: Optional[str] = None,
        acknowledged: Optional[bool] = None,
        limit: int = 100,
    ) -> List[Dict[str, Any]]:
        """Lấy danh sách alerts"""
        async with self._lock:
            try:
                alerts = self.alert_history.copy()

                # Filter theo severity
                if severity:
                    alerts = [a for a in alerts if a["severity"] == severity]

                # Filter theo acknowledged
                if acknowledged is not None:
                    alerts = [a for a in alerts if a["acknowledged"] == acknowledged]

                # Sort theo timestamp (mới nhất trước)
                alerts.sort(key=lambda x: x["timestamp"], reverse=True)

                return alerts[:limit]

            except Exception as e:
                logger.error(f"Error getting alerts: {e}")
                raise

    async def acknowledge_alert(self, alert_id: str) -> bool:
        """Acknowledge alert"""
        async with self._lock:
            try:
                for alert in self.alert_history:
                    if alert["id"] == alert_id:
                        alert["acknowledged"] = True
                        alert["acknowledged_at"] = datetime.utcnow().isoformat()
                        logger.info(f"Alert acknowledged: {alert_id}")
                        return True

                return False

            except Exception as e:
                logger.error(f"Error acknowledging alert: {e}")
                raise

    async def resolve_alert(self, alert_id: str) -> bool:
        """Resolve alert"""
        async with self._lock:
            try:
                for alert in self.alert_history:
                    if alert["id"] == alert_id:
                        alert["resolved"] = True
                        alert["resolved_at"] = datetime.utcnow().isoformat()
                        logger.info(f"Alert resolved: {alert_id}")
                        return True

                return False

            except Exception as e:
                logger.error(f"Error resolving alert: {e}")
                raise

    async def get_system_health_score(self) -> Dict[str, Any]:
        """Tính toán system health score"""
        async with self._lock:
            try:
                # Lấy metrics gần đây (1 giờ)
                recent_metrics = await self.get_performance_summary(hours=1)

                if "message" in recent_metrics:
                    return {
                        "health_score": 0.0,
                        "status": "unknown",
                        "message": recent_metrics["message"],
                    }

                # Tính health score dựa trên các metrics
                health_components = {}
                total_score = 0.0
                component_count = 0

                # CPU usage
                if "cpu_usage" in recent_metrics["metrics"]:
                    cpu_avg = recent_metrics["metrics"]["cpu_usage"]["avg"]
                    cpu_score = max(0.0, 1.0 - (cpu_avg / 100.0))
                    health_components["cpu"] = cpu_score
                    total_score += cpu_score
                    component_count += 1

                # Memory usage
                if "memory_usage" in recent_metrics["metrics"]:
                    mem_avg = recent_metrics["metrics"]["memory_usage"]["avg"]
                    mem_score = max(0.0, 1.0 - (mem_avg / 100.0))
                    health_components["memory"] = mem_score
                    total_score += mem_score
                    component_count += 1

                # Response time
                if "response_time" in recent_metrics["metrics"]:
                    resp_avg = recent_metrics["metrics"]["response_time"]["avg"]
                    resp_score = max(
                        0.0, 1.0 - (resp_avg / 1000.0)
                    )  # Giả sử 1000ms là threshold
                    health_components["response_time"] = resp_score
                    total_score += resp_score
                    component_count += 1

                # Error rate
                if "error_rate" in recent_metrics["metrics"]:
                    error_avg = recent_metrics["metrics"]["error_rate"]["avg"]
                    error_score = max(0.0, 1.0 - error_avg)
                    health_components["error_rate"] = error_score
                    total_score += error_score
                    component_count += 1

                # Tính overall health score
                overall_score = (
                    total_score / component_count if component_count > 0 else 0.0
                )

                # Xác định status
                if overall_score >= 0.8:
                    status = "healthy"
                elif overall_score >= 0.6:
                    status = "warning"
                else:
                    status = "critical"

                return {
                    "health_score": round(overall_score, 3),
                    "status": status,
                    "components": health_components,
                    "timestamp": datetime.utcnow().isoformat(),
                }

            except Exception as e:
                logger.error(f"Error calculating system health score: {e}")
                raise

    async def get_predictive_insights(self) -> Dict[str, Any]:
        """Tạo predictive insights"""
        async with self._lock:
            try:
                insights = {
                    "timestamp": datetime.utcnow().isoformat(),
                    "predictions": {},
                    "recommendations": [],
                }

                # Phân tích các metrics quan trọng
                important_metrics = [
                    "cpu_usage",
                    "memory_usage",
                    "response_time",
                    "error_rate",
                ]

                for metric in important_metrics:
                    if metric in self.performance_data:
                        trend_analysis = await self.get_trend_analysis(metric, hours=6)
                        insights["predictions"][metric] = trend_analysis

                        # Tạo recommendations dựa trên trend
                        if "trend" in trend_analysis:
                            trend = trend_analysis["trend"]
                            if trend == "increasing" and metric in [
                                "cpu_usage",
                                "memory_usage",
                            ]:
                                insights["recommendations"].append(
                                    f"Consider scaling resources as {metric} is trending upward"
                                )
                            elif trend == "increasing" and metric == "response_time":
                                insights["recommendations"].append(
                                    "Performance degradation detected, investigate bottlenecks"
                                )
                            elif trend == "increasing" and metric == "error_rate":
                                insights["recommendations"].append(
                                    "Error rate is increasing, review recent changes"
                                )

                return insights

            except Exception as e:
                logger.error(f"Error generating predictive insights: {e}")
                raise

    # Private methods
    def _calculate_std(self, values: List[float]) -> float:
        """Tính standard deviation"""
        if len(values) < 2:
            return 0.0

        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return math.sqrt(variance)

    def _calculate_trend(self, values: List[float]) -> str:
        """Tính trend của values"""
        if len(values) < 2:
            return "stable"

        # Sử dụng linear regression đơn giản
        n = len(values)
        x_sum = sum(range(n))
        y_sum = sum(values)
        xy_sum = sum(i * val for i, val in enumerate(values))
        x2_sum = sum(i * i for i in range(n))

        slope = (n * xy_sum - x_sum * y_sum) / (n * x2_sum - x_sum * x_sum)

        if abs(slope) < 0.01:
            return "stable"
        elif slope > 0:
            return "increasing"
        else:
            return "decreasing"

    def _analyze_pattern(self, values: List[float]) -> str:
        """Phân tích pattern của values"""
        if len(values) < 10:
            return "insufficient_data"

        # Tính coefficient of variation
        mean = sum(values) / len(values)
        std = self._calculate_std(values)
        cv = std / mean if mean > 0 else 0

        if cv < 0.1:
            return "stable"
        elif cv < 0.3:
            return "moderate_variation"
        else:
            return "high_variation"

    def _simple_prediction(self, values: List[float]) -> Dict[str, Any]:
        """Dự đoán đơn giản"""
        if len(values) < 5:
            return {"next_value": None, "confidence": 0.0}

        # Sử dụng moving average
        recent_values = values[-5:]
        predicted_value = sum(recent_values) / len(recent_values)

        # Tính confidence dựa trên variation
        std = self._calculate_std(recent_values)
        mean = sum(recent_values) / len(recent_values)
        confidence = max(0.0, 1.0 - (std / mean)) if mean > 0 else 0.0

        return {
            "next_value": round(predicted_value, 3),
            "confidence": round(confidence, 3),
            "method": "moving_average",
        }
