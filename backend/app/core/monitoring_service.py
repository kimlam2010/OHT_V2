"""
Monitoring Service for Production Environment

This module provides comprehensive system monitoring and alerting.
It includes system metrics, performance monitoring, and alert management.
"""

import asyncio
import logging
import psutil
from typing import Dict, Any, List, Optional
from datetime import datetime, timedelta
from dataclasses import dataclass

logger = logging.getLogger(__name__)


@dataclass
class SystemMetrics:
    """System metrics data structure"""
    timestamp: datetime
    cpu_percent: float
    memory_percent: float
    disk_percent: float
    network_bytes_sent: int
    network_bytes_recv: int
    active_connections: int
    uptime: float


@dataclass
class Alert:
    """Alert data structure"""
    id: str
    level: str  # critical, warning, info
    message: str
    timestamp: datetime
    acknowledged: bool = False
    resolved: bool = False


class MonitoringService:
    """
    Production-grade monitoring and alerting system

    Features:
    - System metrics collection
    - Performance monitoring
    - Alert management
    - Threshold monitoring
    - Health status reporting
    """

    def __init__(self):
        self.metrics_history: List[SystemMetrics] = []
        self.alerts: List[Alert] = []
        self.max_metrics_history = 1000
        self.max_alerts = 100

        # Monitoring thresholds
        self.thresholds = {
            "cpu_critical": 90.0,
            "cpu_warning": 80.0,
            "memory_critical": 90.0,
            "memory_warning": 80.0,
            "disk_critical": 95.0,
            "disk_warning": 85.0
        }

        # Alert levels
        self.alert_levels = ["critical", "warning", "info"]

        logger.info("🔍 MonitoringService initialized")

    async def start_monitoring(self, interval_seconds: int = 30):
        """Start continuous monitoring"""
        logger.info(f"🚀 Starting monitoring with {interval_seconds}s interval")
        
        while True:
            try:
                # Collect metrics
                metrics = await self.collect_system_metrics()
                self._store_metrics(metrics)
                
                # Check thresholds and generate alerts
                await self._check_thresholds(metrics)
                
                # Cleanup old data
                self._cleanup_old_data()
                
                # Wait for next collection
                await asyncio.sleep(interval_seconds)
                
            except Exception as e:
                logger.error(f"❌ Monitoring error: {e}")
                await asyncio.sleep(interval_seconds)

    async def collect_system_metrics(self) -> SystemMetrics:
        """Collect current system metrics"""
        try:
            # CPU usage
            cpu_percent = psutil.cpu_percent(interval=1)
            
            # Memory usage
            memory = psutil.virtual_memory()
            memory_percent = memory.percent
            
            # Disk usage
            disk = psutil.disk_usage('/')
            disk_percent = disk.percent
            
            # Network I/O
            network = psutil.net_io_counters()
            network_bytes_sent = network.bytes_sent
            network_bytes_recv = network.bytes_recv
            
            # Active connections (approximate)
            try:
                connections = len(psutil.net_connections())
                active_connections = connections
            except Exception:
                active_connections = 0
            
            # System uptime
            uptime = psutil.boot_time()
            current_time = datetime.now().timestamp()
            uptime_seconds = current_time - uptime
            
            metrics = SystemMetrics(
                timestamp=datetime.now(),
                cpu_percent=cpu_percent,
                memory_percent=memory_percent,
                disk_percent=disk_percent,
                network_bytes_sent=network_bytes_sent,
                network_bytes_recv=network_bytes_recv,
                active_connections=active_connections,
                uptime=uptime_seconds
            )
            
            return metrics
            
        except Exception as e:
            logger.error(f"❌ Metrics collection failed: {e}")
            # Return default metrics
            return SystemMetrics(
                timestamp=datetime.now(),
                cpu_percent=0.0,
                memory_percent=0.0,
                disk_percent=0.0,
                network_bytes_sent=0,
                network_bytes_recv=0,
                active_connections=0,
                uptime=0.0
            )

    async def get_current_metrics(self) -> SystemMetrics:
        """Get current system metrics"""
        return await self.collect_system_metrics()

    async def get_metrics_history(self, hours: int = 24) -> List[SystemMetrics]:
        """Get metrics history for specified hours"""
        cutoff_time = datetime.now() - timedelta(hours=hours)
        
        return [
            metrics for metrics in self.metrics_history
            if metrics.timestamp >= cutoff_time
        ]

    async def get_system_health(self) -> Dict[str, Any]:
        """Get overall system health status"""
        try:
            current_metrics = await self.get_current_metrics()
            
            # Calculate health scores
            cpu_health = max(0, 100 - current_metrics.cpu_percent)
            memory_health = max(0, 100 - current_metrics.memory_percent)
            disk_health = max(0, 100 - current_metrics.disk_percent)
            
            # Overall health score (weighted average)
            overall_health = (cpu_health * 0.4 + memory_health * 0.4 + disk_health * 0.2)
            
            # Determine health status
            if overall_health >= 80:
                status = "healthy"
            elif overall_health >= 60:
                status = "warning"
            else:
                status = "critical"
            
            # Get recent alerts
            recent_alerts = self._get_recent_alerts(hours=1)
            
            return {
                "status": status,
                "overall_health_score": round(overall_health, 2),
                "component_health": {
                    "cpu": {
                        "score": round(cpu_health, 2),
                        "status": "healthy" if cpu_health >= 80 else "warning" if cpu_health >= 60 else "critical"
                    },
                    "memory": {
                        "score": round(memory_health, 2),
                        "status": "healthy" if memory_health >= 80 else "warning" if memory_health >= 60 else "critical"
                    },
                    "disk": {
                        "score": round(disk_health, 2),
                        "status": "healthy" if disk_health >= 80 else "warning" if disk_health >= 60 else "critical"
                    }
                },
                "current_metrics": {
                    "cpu_percent": current_metrics.cpu_percent,
                    "memory_percent": current_metrics.memory_percent,
                    "disk_percent": current_metrics.disk_percent,
                    "active_connections": current_metrics.active_connections,
                    "uptime_seconds": current_metrics.uptime
                },
                "recent_alerts": len(recent_alerts),
                "last_updated": datetime.now().isoformat()
            }
            
        except Exception as e:
            logger.error(f"❌ System health check failed: {e}")
            return {
                "status": "error",
                "error": str(e),
                "last_updated": datetime.now().isoformat()
            }

    async def get_performance_summary(self, hours: int = 24) -> Dict[str, Any]:
        """Get performance summary for specified hours"""
        try:
            metrics = await self.get_metrics_history(hours)
            
            if not metrics:
                return {"error": "No metrics available for specified period"}
            
            # Calculate averages
            cpu_values = [m.cpu_percent for m in metrics]
            memory_values = [m.memory_percent for m in metrics]
            disk_values = [m.disk_percent for m in metrics]
            
            avg_cpu = sum(cpu_values) / len(cpu_values)
            avg_memory = sum(memory_values) / len(memory_values)
            avg_disk = sum(disk_values) / len(disk_values)
            
            max_cpu = max(cpu_values)
            max_memory = max(memory_values)
            max_disk = max(disk_values)
            
            # Network totals
            total_sent = sum(m.network_bytes_sent for m in metrics)
            total_recv = sum(m.network_bytes_recv for m in metrics)
            
            return {
                "period_hours": hours,
                "metrics_count": len(metrics),
                "averages": {
                    "cpu_percent": round(avg_cpu, 2),
                    "memory_percent": round(avg_memory, 2),
                    "disk_percent": round(avg_disk, 2)
                },
                "peaks": {
                    "cpu_percent": round(max_cpu, 2),
                    "memory_percent": round(max_memory, 2),
                    "disk_percent": round(max_disk, 2)
                },
                "network_total": {
                    "bytes_sent": total_sent,
                    "bytes_recv": total_recv,
                    "bytes_sent_mb": round(total_sent / (1024 * 1024), 2),
                    "bytes_recv_mb": round(total_recv / (1024 * 1024), 2)
                },
                "period_start": metrics[-1].timestamp.isoformat(),
                "period_end": metrics[0].timestamp.isoformat()
            }
            
        except Exception as e:
            logger.error(f"❌ Performance summary failed: {e}")
            return {"error": str(e)}

    async def acknowledge_alert(self, alert_id: str) -> bool:
        """Acknowledge an alert"""
        try:
            for alert in self.alerts:
                if alert.id == alert_id:
                    alert.acknowledged = True
                    logger.info(f"✅ Alert acknowledged: {alert_id}")
                    return True
            
            logger.warning(f"⚠️ Alert not found: {alert_id}")
            return False
            
        except Exception as e:
            logger.error(f"❌ Alert acknowledgement failed: {e}")
            return False

    async def resolve_alert(self, alert_id: str) -> bool:
        """Resolve an alert"""
        try:
            for alert in self.alerts:
                if alert.id == alert_id:
                    alert.resolved = True
                    logger.info(f"✅ Alert resolved: {alert_id}")
                    return True
            
            logger.warning(f"⚠️ Alert not found: {alert_id}")
            return False
            
        except Exception as e:
            logger.error(f"❌ Alert resolution failed: {e}")
            return False

    async def get_active_alerts(self) -> List[Alert]:
        """Get all active (unresolved) alerts"""
        return [alert for alert in self.alerts if not alert.resolved]

    async def get_alerts_by_level(self, level: str) -> List[Alert]:
        """Get alerts by severity level"""
        if level not in self.alert_levels:
            return []
        
        return [alert for alert in self.alerts if alert.level == level]

    async def clear_resolved_alerts(self) -> int:
        """Clear all resolved alerts"""
        try:
            resolved_count = len([alert for alert in self.alerts if alert.resolved])
            self.alerts = [alert for alert in self.alerts if not alert.resolved]
            
            logger.info(f"🧹 Cleared {resolved_count} resolved alerts")
            return resolved_count
            
        except Exception as e:
            logger.error(f"❌ Clear resolved alerts failed: {e}")
            return 0

    async def update_thresholds(self, new_thresholds: Dict[str, float]):
        """Update monitoring thresholds"""
        try:
            self.thresholds.update(new_thresholds)
            logger.info(f"🔄 Monitoring thresholds updated: {new_thresholds}")
            
        except Exception as e:
            logger.error(f"❌ Threshold update failed: {e}")

    # Private methods

    def _store_metrics(self, metrics: SystemMetrics):
        """Store metrics in history"""
        self.metrics_history.append(metrics)
        
        # Maintain history size
        if len(self.metrics_history) > self.max_metrics_history:
            self.metrics_history.pop(0)

    async def _check_thresholds(self, metrics: SystemMetrics):
        """Check metrics against thresholds and generate alerts"""
        try:
            # CPU threshold checks
            if metrics.cpu_percent >= self.thresholds["cpu_critical"]:
                await self._create_alert("critical", f"CPU usage critical: {metrics.cpu_percent:.1f}%")
            elif metrics.cpu_percent >= self.thresholds["cpu_warning"]:
                await self._create_alert("warning", f"CPU usage high: {metrics.cpu_percent:.1f}%")
            
            # Memory threshold checks
            if metrics.memory_percent >= self.thresholds["memory_critical"]:
                await self._create_alert("critical", f"Memory usage critical: {metrics.memory_percent:.1f}%")
            elif metrics.memory_percent >= self.thresholds["memory_warning"]:
                await self._create_alert("warning", f"Memory usage high: {metrics.memory_percent:.1f}%")
            
            # Disk threshold checks
            if metrics.disk_percent >= self.thresholds["disk_critical"]:
                await self._create_alert("critical", f"Disk usage critical: {metrics.disk_percent:.1f}%")
            elif metrics.disk_percent >= self.thresholds["disk_warning"]:
                await self._create_alert("warning", f"Disk usage high: {metrics.disk_percent:.1f}%")
                
        except Exception as e:
            logger.error(f"❌ Threshold check failed: {e}")

    async def _create_alert(self, level: str, message: str):
        """Create new alert"""
        try:
            alert_id = f"alert_{int(datetime.now().timestamp())}_{len(self.alerts)}"
            
            alert = Alert(
                id=alert_id,
                level=level,
                message=message,
                timestamp=datetime.now()
            )
            
            self.alerts.append(alert)
            
            # Maintain alerts size
            if len(self.alerts) > self.max_alerts:
                self.alerts.pop(0)
            
            logger.warning(f"🚨 {level.upper()} Alert: {message}")
            
        except Exception as e:
            logger.error(f"❌ Alert creation failed: {e}")

    def _get_recent_alerts(self, hours: int = 1) -> List[Alert]:
        """Get recent alerts within specified hours"""
        cutoff_time = datetime.now() - timedelta(hours=hours)
        return [
            alert for alert in self.alerts
            if alert.timestamp >= cutoff_time
        ]

    def _cleanup_old_data(self):
        """Clean up old metrics and alerts"""
        try:
            # Clean up old metrics (keep last 24 hours)
            cutoff_time = datetime.now() - timedelta(hours=24)
            self.metrics_history = [
                metrics for metrics in self.metrics_history
                if metrics.timestamp >= cutoff_time
            ]
            
            # Clean up old alerts (keep last 7 days)
            cutoff_time = datetime.now() - timedelta(days=7)
            self.alerts = [
                alert for alert in self.alerts
                if alert.timestamp >= cutoff_time
            ]
            
        except Exception as e:
            logger.error(f"❌ Data cleanup failed: {e}")


# Global monitoring service instance
monitoring_service = MonitoringService()
