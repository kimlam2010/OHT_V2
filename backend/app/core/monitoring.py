"""
Performance monitoring and metrics for OHT-50 Backend
"""

import time
from typing import Dict, Any
from fastapi import FastAPI, Request, Response
from prometheus_client import Counter, Histogram, Gauge

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


class PerformanceMiddleware:
    """Middleware for performance monitoring"""
    
    async def __call__(self, request: Request, call_next):
        start_time = time.time()
        
        response = await call_next(request)
        
        duration = time.time() - start_time
        
        REQUEST_COUNT.labels(
            method=request.method,
            endpoint=request.url.path,
            status=response.status_code
        ).inc()
        
        REQUEST_DURATION.labels(
            method=request.method,
            endpoint=request.url.path
        ).observe(duration)
        
        return response


def setup_monitoring(app: FastAPI) -> None:
    """Setup monitoring for the application"""
    
    # Add performance middleware
    app.add_middleware(PerformanceMiddleware)
    
    # Add metrics endpoint
    from prometheus_client import generate_latest, CONTENT_TYPE_LATEST
    from fastapi.responses import Response as FastAPIResponse
    
    @app.get("/metrics")
    async def metrics():
        """Prometheus metrics endpoint"""
        return FastAPIResponse(
            content=generate_latest(),
            media_type=CONTENT_TYPE_LATEST
        )
