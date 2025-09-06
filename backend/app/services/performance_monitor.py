"""
Performance Monitoring Service for OHT-50
Real-time performance monitoring and metrics collection
Target: < 5ms monitoring overhead for system performance tracking
"""

import asyncio
import logging
import statistics
import time
import psutil
from typing import Dict, Any, List, Optional, Callable
from datetime import datetime, timezone, timedelta
from dataclasses import dataclass, field
from enum import Enum
import threading

logger = logging.getLogger(__name__)


class MetricType(Enum):
    """Types of performance metrics"""
    CPU_USAGE = "cpu_usage"
    MEMORY_USAGE = "memory_usage"
    DISK_USAGE = "disk_usage"
    NETWORK_IO = "network_io"
    RESPONSE_TIME = "response_time"
    THROUGHPUT = "throughput"
    ERROR_RATE = "error_rate"
    CUSTOM = "custom"


class AlertLevel(Enum):
    """Performance alert levels"""
    INFO = "info"
    WARNING = "warning"
    CRITICAL = "critical"


@dataclass
class PerformanceMetric:
    """Performance metric data"""
    metric_type: MetricType
    value: float
    unit: str
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class PerformanceAlert:
    """Performance alert"""
    metric_type: MetricType
    current_value: float
    threshold_value: float
    alert_level: AlertLevel
    message: str
    timestamp: datetime = field(default_factory=lambda: datetime.now(timezone.utc))
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class PerformanceConfig:
    """Performance monitoring configuration"""
    metric_type: MetricType
    enabled: bool = True
    collection_interval: float = 1.0  # seconds
    threshold_warning: float = 80.0
    threshold_critical: float = 95.0
    window_size: int = 100
    unit: str = ""
    custom_collector: Optional[Callable] = None


class PerformanceMonitor:
    """
    Real-time performance monitoring service
    Tracks system metrics and provides alerts for performance issues
    """

    def __init__(self):
        self.monitoring_configs: Dict[MetricType, PerformanceConfig] = {}
        self.metrics_history: Dict[MetricType, List[PerformanceMetric]] = {}
        self.active_alerts: List[PerformanceAlert] = []
        self.alert_history: List[PerformanceAlert] = []
        self.monitoring_active = False
        self.monitoring_task: Optional[asyncio.Task] = None
        self.processing_times: List[float] = []
        
        # Initialize default configurations
        self._initialize_default_configs()
        
        logger.info("PerformanceMonitor initialized with %d metric types", len(self.monitoring_configs))

    def _initialize_default_configs(self):
        """Initialize default monitoring configurations"""
        self.monitoring_configs = {
            MetricType.CPU_USAGE: PerformanceConfig(
                metric_type=MetricType.CPU_USAGE,
                collection_interval=1.0,
                threshold_warning=70.0,
                threshold_critical=90.0,
                unit="%"
            ),
            MetricType.MEMORY_USAGE: PerformanceConfig(
                metric_type=MetricType.MEMORY_USAGE,
                collection_interval=1.0,
                threshold_warning=80.0,
                threshold_critical=95.0,
                unit="%"
            ),
            MetricType.DISK_USAGE: PerformanceConfig(
                metric_type=MetricType.DISK_USAGE,
                collection_interval=5.0,
                threshold_warning=85.0,
                threshold_critical=95.0,
                unit="%"
            ),
            MetricType.NETWORK_IO: PerformanceConfig(
                metric_type=MetricType.NETWORK_IO,
                collection_interval=1.0,
                threshold_warning=1000.0,  # MB/s
                threshold_critical=2000.0,
                unit="MB/s"
            ),
            MetricType.RESPONSE_TIME: PerformanceConfig(
                metric_type=MetricType.RESPONSE_TIME,
                collection_interval=0.1,
                threshold_warning=100.0,  # ms
                threshold_critical=500.0,
                unit="ms"
            ),
            MetricType.THROUGHPUT: PerformanceConfig(
                metric_type=MetricType.THROUGHPUT,
                collection_interval=1.0,
                threshold_warning=1000.0,  # requests/s
                threshold_critical=2000.0,
                unit="req/s"
            ),
            MetricType.ERROR_RATE: PerformanceConfig(
                metric_type=MetricType.ERROR_RATE,
                collection_interval=1.0,
                threshold_warning=5.0,  # %
                threshold_critical=10.0,
                unit="%"
            )
        }
        
        # Initialize metrics history
        for metric_type in self.monitoring_configs:
            self.metrics_history[metric_type] = []

    async def start_monitoring(self):
        """Start performance monitoring"""
        if self.monitoring_active:
            logger.warning("Performance monitoring is already active")
            return
        
        self.monitoring_active = True
        self.monitoring_task = asyncio.create_task(self._monitoring_loop())
        logger.info("Performance monitoring started")

    async def stop_monitoring(self):
        """Stop performance monitoring"""
        if not self.monitoring_active:
            logger.warning("Performance monitoring is not active")
            return
        
        self.monitoring_active = False
        if self.monitoring_task:
            self.monitoring_task.cancel()
            try:
                await self.monitoring_task
            except asyncio.CancelledError:
                pass
        logger.info("Performance monitoring stopped")

    async def _monitoring_loop(self):
        """Main monitoring loop"""
        while self.monitoring_active:
            try:
                await self._collect_all_metrics()
                await asyncio.sleep(0.1)  # 100ms monitoring cycle
            except Exception as e:
                logger.error(f"Error in monitoring loop: {e}")
                await asyncio.sleep(1.0)

    async def _collect_all_metrics(self):
        """Collect all enabled metrics"""
        for metric_type, config in self.monitoring_configs.items():
            if config.enabled:
                try:
                    await self._collect_metric(metric_type, config)
                except Exception as e:
                    logger.error(f"Error collecting metric {metric_type.value}: {e}")

    async def _collect_metric(self, metric_type: MetricType, config: PerformanceConfig):
        """Collect specific metric"""
        start_time = time.perf_counter()
        
        try:
            if metric_type == MetricType.CPU_USAGE:
                value = await self._collect_cpu_usage()
            elif metric_type == MetricType.MEMORY_USAGE:
                value = await self._collect_memory_usage()
            elif metric_type == MetricType.DISK_USAGE:
                value = await self._collect_disk_usage()
            elif metric_type == MetricType.NETWORK_IO:
                value = await self._collect_network_io()
            elif metric_type == MetricType.RESPONSE_TIME:
                value = await self._collect_response_time()
            elif metric_type == MetricType.THROUGHPUT:
                value = await self._collect_throughput()
            elif metric_type == MetricType.ERROR_RATE:
                value = await self._collect_error_rate()
            elif metric_type == MetricType.CUSTOM and config.custom_collector:
                value = await config.custom_collector()
            else:
                return
            
            # Create metric
            metric = PerformanceMetric(
                metric_type=metric_type,
                value=value,
                unit=config.unit,
                metadata={"collection_time": time.perf_counter() - start_time}
            )
            
            # Store metric
            self._store_metric(metric)
            
            # Check for alerts
            await self._check_metric_alerts(metric, config)
            
            # Update processing time
            processing_time = (time.perf_counter() - start_time) * 1000
            self.processing_times.append(processing_time)
            if len(self.processing_times) > 1000:
                self.processing_times.pop(0)
            
            if processing_time > 5.0:  # Target overhead
                logger.warning(f"Metric collection {metric_type.value} took {processing_time:.2f}ms, exceeding 5ms target")
                
        except Exception as e:
            logger.error(f"Error collecting metric {metric_type.value}: {e}")

    async def _collect_cpu_usage(self) -> float:
        """Collect CPU usage percentage"""
        return psutil.cpu_percent(interval=0.1)

    async def _collect_memory_usage(self) -> float:
        """Collect memory usage percentage"""
        memory = psutil.virtual_memory()
        return memory.percent

    async def _collect_disk_usage(self) -> float:
        """Collect disk usage percentage"""
        disk = psutil.disk_usage('/')
        return (disk.used / disk.total) * 100

    async def _collect_network_io(self) -> float:
        """Collect network I/O rate"""
        network = psutil.net_io_counters()
        # Simplified network I/O calculation
        return (network.bytes_sent + network.bytes_recv) / (1024 * 1024)  # MB

    async def _collect_response_time(self) -> float:
        """Collect average response time"""
        # This would typically be collected from application metrics
        # For now, return a simulated value
        return statistics.mean(self.processing_times) if self.processing_times else 0.0

    async def _collect_throughput(self) -> float:
        """Collect throughput rate"""
        # This would typically be collected from application metrics
        # For now, return a simulated value
        return len(self.processing_times) * 10  # Simulated requests per second

    async def _collect_error_rate(self) -> float:
        """Collect error rate percentage"""
        # This would typically be collected from application metrics
        # For now, return a simulated value
        return 0.0  # Simulated 0% error rate

    def _store_metric(self, metric: PerformanceMetric):
        """Store metric in history"""
        history = self.metrics_history[metric.metric_type]
        history.append(metric)
        
        # Maintain window size
        config = self.monitoring_configs[metric.metric_type]
        if len(history) > config.window_size:
            history.pop(0)

    async def _check_metric_alerts(self, metric: PerformanceMetric, config: PerformanceConfig):
        """Check if metric triggers alerts"""
        current_value = metric.value
        
        # Check critical threshold
        if current_value >= config.threshold_critical:
            alert = PerformanceAlert(
                metric_type=metric.metric_type,
                current_value=current_value,
                threshold_value=config.threshold_critical,
                alert_level=AlertLevel.CRITICAL,
                message=f"Critical {metric.metric_type.value}: {current_value:.2f}{config.unit} >= {config.threshold_critical}{config.unit}",
                metadata={"config": config.__dict__}
            )
            await self._handle_alert(alert)
        
        # Check warning threshold
        elif current_value >= config.threshold_warning:
            alert = PerformanceAlert(
                metric_type=metric.metric_type,
                current_value=current_value,
                threshold_value=config.threshold_warning,
                alert_level=AlertLevel.WARNING,
                message=f"Warning {metric.metric_type.value}: {current_value:.2f}{config.unit} >= {config.threshold_warning}{config.unit}",
                metadata={"config": config.__dict__}
            )
            await self._handle_alert(alert)

    async def _handle_alert(self, alert: PerformanceAlert):
        """Handle performance alert"""
        # Add to active alerts
        self.active_alerts.append(alert)
        
        # Add to alert history
        self.alert_history.append(alert)
        
        # Keep only recent alerts
        if len(self.alert_history) > 1000:
            self.alert_history = self.alert_history[-1000:]
        
        # Log alert
        logger.warning(f"Performance Alert: {alert.message}")
        
        # In a real implementation, this would trigger notifications
        # await self._send_alert_notification(alert)

    def configure_metric(self, metric_type: MetricType, config: PerformanceConfig):
        """Configure performance metric monitoring"""
        self.monitoring_configs[metric_type] = config
        logger.info(f"Configured metric {metric_type.value} with threshold_warning={config.threshold_warning}")

    def get_metric_history(self, metric_type: MetricType, limit: int = 100) -> List[PerformanceMetric]:
        """Get metric history"""
        return self.metrics_history.get(metric_type, [])[-limit:]

    def get_current_metrics(self) -> Dict[str, Any]:
        """Get current performance metrics"""
        current_metrics = {}
        
        for metric_type, history in self.metrics_history.items():
            if history:
                latest_metric = history[-1]
                current_metrics[metric_type.value] = {
                    "value": latest_metric.value,
                    "unit": latest_metric.unit,
                    "timestamp": latest_metric.timestamp.isoformat()
                }
        
        return current_metrics

    def get_performance_summary(self) -> Dict[str, Any]:
        """Get performance monitoring summary"""
        summary = {
            "monitoring_active": self.monitoring_active,
            "total_metrics": len(self.monitoring_configs),
            "enabled_metrics": len([c for c in self.monitoring_configs.values() if c.enabled]),
            "active_alerts": len(self.active_alerts),
            "total_alerts": len(self.alert_history),
            "avg_processing_time_ms": statistics.mean(self.processing_times) if self.processing_times else 0.0,
            "current_metrics": self.get_current_metrics()
        }
        
        return summary

    def get_alert_summary(self) -> Dict[str, Any]:
        """Get alert summary"""
        alert_summary = {
            "active_alerts": len(self.active_alerts),
            "alerts_by_level": {level.value: 0 for level in AlertLevel},
            "alerts_by_metric": {metric.value: 0 for metric in MetricType},
            "recent_alerts": []
        }
        
        # Count alerts by level and metric
        for alert in self.alert_history[-100:]:  # Last 100 alerts
            alert_summary["alerts_by_level"][alert.alert_level.value] += 1
            alert_summary["alerts_by_metric"][alert.metric_type.value] += 1
        
        # Get recent alerts
        alert_summary["recent_alerts"] = [
            {
                "metric_type": alert.metric_type.value,
                "alert_level": alert.alert_level.value,
                "message": alert.message,
                "timestamp": alert.timestamp.isoformat()
            }
            for alert in self.alert_history[-10:]
        ]
        
        return alert_summary

    def clear_alerts(self, alert_level: Optional[AlertLevel] = None):
        """Clear alerts"""
        if alert_level:
            self.active_alerts = [a for a in self.active_alerts if a.alert_level != alert_level]
            logger.info(f"Cleared {alert_level.value} alerts")
        else:
            self.active_alerts.clear()
            logger.info("Cleared all active alerts")

    def add_custom_metric(self, metric_type: MetricType, value: float, unit: str = "", 
                         metadata: Optional[Dict[str, Any]] = None):
        """Add custom metric"""
        metric = PerformanceMetric(
            metric_type=metric_type,
            value=value,
            unit=unit,
            metadata=metadata or {}
        )
        
        self._store_metric(metric)
        
        # Check for alerts
        config = self.monitoring_configs.get(metric_type)
        if config:
            asyncio.create_task(self._check_metric_alerts(metric, config))

    def get_metric_statistics(self, metric_type: MetricType, window_minutes: int = 5) -> Dict[str, Any]:
        """Get metric statistics for time window"""
        history = self.metrics_history.get(metric_type, [])
        
        if not history:
            return {}
        
        # Filter by time window
        cutoff_time = datetime.now(timezone.utc) - timedelta(minutes=window_minutes)
        recent_metrics = [m for m in history if m.timestamp >= cutoff_time]
        
        if not recent_metrics:
            return {}
        
        values = [m.value for m in recent_metrics]
        
        return {
            "count": len(values),
            "min": min(values),
            "max": max(values),
            "mean": statistics.mean(values),
            "median": statistics.median(values),
            "std_dev": statistics.stdev(values) if len(values) > 1 else 0.0,
            "window_minutes": window_minutes
        }


# Global instance
performance_monitor = PerformanceMonitor()
