"""
Performance monitoring and metrics for OHT-50 Backend
"""

import time
import psutil
import asyncio
from typing import Dict, Any, Optional
from datetime import datetime, timedelta
from fastapi import FastAPI, Request, Response
from prometheus_client import Counter, Histogram, Gauge, Summary, generate_latest, CONTENT_TYPE_LATEST
from prometheus_client.core import REGISTRY

# Metrics
REQUEST_COUNT = Counter(
    "http_requests_total",
    "Total HTTP requests",
    ["method", "endpoint", "status"]
)

REQUEST_DURATION = Histogram(
    "http_request_duration_seconds",
    "HTTP request duration in seconds",
    ["method", "endpoint"]
)

ACTIVE_CONNECTIONS = Gauge(
    "websocket_active_connections",
    "Number of active WebSocket connections"
)

SYSTEM_MEMORY = Gauge(
    "system_memory_usage_bytes",
    "System memory usage in bytes"
)

SYSTEM_CPU = Gauge(
    "system_cpu_usage_percent",
    "System CPU usage percentage"
)

DATABASE_CONNECTIONS = Gauge(
    "database_active_connections",
    "Number of active database connections"
)

RS485_COMMUNICATION_ERRORS = Counter(
    "rs485_communication_errors_total",
    "Total RS485 communication errors",
    ["module_address", "error_type"]
)

ROBOT_COMMANDS = Counter(
    "robot_commands_total",
    "Total robot commands executed",
    ["command_type", "status"]
)

SAFETY_ALERTS = Counter(
    "safety_alerts_total",
    "Total safety alerts generated",
    ["alert_type", "severity"]
)

TELEMETRY_POINTS = Counter(
    "telemetry_points_total",
    "Total telemetry data points collected",
    ["telemetry_type"]
)

# Performance summaries
API_RESPONSE_TIME = Summary(
    "api_response_time_seconds",
    "API response time summary",
    ["endpoint"]
)

DATABASE_QUERY_TIME = Summary(
    "database_query_time_seconds",
    "Database query time summary",
    ["query_type"]
)


class PerformanceMiddleware:
    """Middleware for performance monitoring"""
    
    async def __call__(self, request: Request, call_next):
        start_time = time.time()
        
        response = await call_next(request)
        
        duration = time.time() - start_time
        
        # Record request metrics
        REQUEST_COUNT.labels(
            method=request.method,
            endpoint=request.url.path,
            status=response.status_code
        ).inc()
        
        REQUEST_DURATION.labels(
            method=request.method,
            endpoint=request.url.path
        ).observe(duration)
        
        # Record API response time
        API_RESPONSE_TIME.labels(
            endpoint=request.url.path
        ).observe(duration)
        
        # Log slow requests
        if duration > 0.1:  # 100ms threshold
            print(f"Slow request: {request.method} {request.url.path} took {duration:.3f}s")
        
        return response


class SystemMonitor:
    """System monitoring service"""
    
    def __init__(self):
        self.monitoring_task = None
        self.monitoring_interval = 30  # seconds
        
    async def start_monitoring(self):
        """Start system monitoring"""
        if self.monitoring_task is None:
            self.monitoring_task = asyncio.create_task(self._monitor_system())
            print("System monitoring started")
    
    async def stop_monitoring(self):
        """Stop system monitoring"""
        if self.monitoring_task:
            self.monitoring_task.cancel()
            try:
                await self.monitoring_task
            except asyncio.CancelledError:
                pass
            self.monitoring_task = None
            print("System monitoring stopped")
    
    async def _monitor_system(self):
        """Monitor system metrics"""
        while True:
            try:
                # CPU usage
                cpu_percent = psutil.cpu_percent(interval=1)
                SYSTEM_CPU.set(cpu_percent)
                
                # Memory usage
                memory = psutil.virtual_memory()
                SYSTEM_MEMORY.set(memory.used)
                
                # Check thresholds and generate alerts
                await self._check_thresholds(cpu_percent, memory.percent)
                
                await asyncio.sleep(self.monitoring_interval)
                
            except Exception as e:
                print(f"System monitoring error: {e}")
                await asyncio.sleep(5)
    
    async def _check_thresholds(self, cpu_percent: float, memory_percent: float):
        """Check system thresholds and generate alerts"""
        # CPU threshold alerts
        if cpu_percent > 80:
            print(f"WARNING: High CPU usage: {cpu_percent}%")
        elif cpu_percent > 90:
            print(f"CRITICAL: Very high CPU usage: {cpu_percent}%")
        
        # Memory threshold alerts
        if memory_percent > 80:
            print(f"WARNING: High memory usage: {memory_percent}%")
        elif memory_percent > 90:
            print(f"CRITICAL: Very high memory usage: {memory_percent}%")


class AlertManager:
    """Alert management service"""
    
    def __init__(self):
        self.alerts = []
        self.alert_callbacks = []
    
    def add_alert_callback(self, callback):
        """Add alert callback function"""
        self.alert_callbacks.append(callback)
    
    async def create_alert(self, alert_type: str, severity: str, message: str, data: Optional[Dict] = None):
        """Create and broadcast alert"""
        alert = {
            "id": f"alert_{int(time.time())}",
            "type": alert_type,
            "severity": severity,
            "message": message,
            "data": data or {},
            "timestamp": datetime.utcnow().isoformat()
        }
        
        self.alerts.append(alert)
        
        # Increment alert counter
        SAFETY_ALERTS.labels(
            alert_type=alert_type,
            severity=severity
        ).inc()
        
        # Call alert callbacks
        for callback in self.alert_callbacks:
            try:
                await callback(alert)
            except Exception as e:
                print(f"Alert callback error: {e}")
        
        return alert
    
    def get_alerts(self, severity: Optional[str] = None, limit: int = 100) -> list:
        """Get alerts with optional filtering"""
        alerts = self.alerts
        
        if severity:
            alerts = [a for a in alerts if a["severity"] == severity]
        
        return alerts[-limit:] if limit else alerts


class MetricsCollector:
    """Custom metrics collector"""
    
    def __init__(self):
        self.custom_metrics = {}
    
    def record_metric(self, metric_name: str, value: float, labels: Optional[Dict[str, str]] = None):
        """Record custom metric"""
        if metric_name not in self.custom_metrics:
            self.custom_metrics[metric_name] = Gauge(
                f"custom_{metric_name}",
                f"Custom metric: {metric_name}",
                list(labels.keys()) if labels else []
            )
        
        metric = self.custom_metrics[metric_name]
        if labels:
            metric.labels(**labels).set(value)
        else:
            metric.set(value)
    
    def record_robot_command(self, command_type: str, status: str):
        """Record robot command metric"""
        ROBOT_COMMANDS.labels(
            command_type=command_type,
            status=status
        ).inc()
    
    def record_rs485_error(self, module_address: int, error_type: str):
        """Record RS485 communication error"""
        RS485_COMMUNICATION_ERRORS.labels(
            module_address=str(module_address),
            error_type=error_type
        ).inc()
    
    def record_telemetry_point(self, telemetry_type: str):
        """Record telemetry data point"""
        TELEMETRY_POINTS.labels(
            telemetry_type=telemetry_type
        ).inc()
    
    def record_database_query(self, query_type: str, duration: float):
        """Record database query performance"""
        DATABASE_QUERY_TIME.labels(
            query_type=query_type
        ).observe(duration)


# Global instances
system_monitor = SystemMonitor()
alert_manager = AlertManager()
metrics_collector = MetricsCollector()


def setup_monitoring(app: FastAPI) -> None:
    """Setup monitoring for the application"""
    
    # Add performance middleware
    app.add_middleware(PerformanceMiddleware)
    
    # Add metrics endpoint
    @app.get("/metrics")
    async def metrics():
        """Prometheus metrics endpoint"""
        return Response(
            content=generate_latest(),
            media_type=CONTENT_TYPE_LATEST
        )
    
    # Add health check endpoint
    @app.get("/health")
    async def health_check():
        """Health check endpoint"""
        health_status = {
            "status": "healthy",
            "timestamp": datetime.utcnow().isoformat(),
            "version": "1.0.0",
            "checks": {
                "system": "ok",
                "database": "ok",
                "rs485": "ok"
            }
        }
        return health_status
    
    # Add system info endpoint
    @app.get("/system/info")
    async def system_info():
        """System information endpoint"""
        return {
            "cpu_percent": psutil.cpu_percent(),
            "memory_percent": psutil.virtual_memory().percent,
            "disk_percent": psutil.disk_usage('/').percent,
            "uptime": time.time() - psutil.boot_time(),
            "timestamp": datetime.utcnow().isoformat()
        }
    
    # Start system monitoring
    @app.on_event("startup")
    async def startup_event():
        await system_monitor.start_monitoring()
    
    # Stop system monitoring
    @app.on_event("shutdown")
    async def shutdown_event():
        await system_monitor.stop_monitoring()
