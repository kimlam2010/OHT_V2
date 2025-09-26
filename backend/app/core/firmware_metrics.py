"""
Firmware Metrics - OHT-50
Prometheus metrics cho firmware communication
"""

from prometheus_client import Counter, Histogram, Gauge, CollectorRegistry
import time
from typing import Dict, Any
import logging

logger = logging.getLogger(__name__)

class FirmwareMetrics:
    """
    Firmware metrics collection với Prometheus
    """
    
    def __init__(self):
        self.registry = CollectorRegistry()
        
        # Request metrics
        self.request_count = Counter(
            'firmware_requests_total',
            'Total firmware requests',
            ['endpoint', 'method', 'status'],
            registry=self.registry
        )
        
        self.request_duration = Histogram(
            'firmware_request_duration_seconds',
            'Firmware request duration',
            ['endpoint', 'method'],
            buckets=[0.01, 0.05, 0.1, 0.5, 1.0, 2.0, 5.0],
            registry=self.registry
        )
        
        self.error_count = Counter(
            'firmware_errors_total',
            'Total firmware errors',
            ['endpoint', 'error_type'],
            registry=self.registry
        )
        
        # Circuit breaker metrics
        self.circuit_breaker_state = Gauge(
            'firmware_circuit_breaker_state',
            'Circuit breaker state (0=closed, 1=open, 2=half_open)',
            registry=self.registry
        )
        
        # Connection metrics
        self.connection_status = Gauge(
            'firmware_connection_status',
            'Firmware connection status (0=disconnected, 1=connected)',
            registry=self.registry
        )
        
        # Response time metrics
        self.response_time_p95 = Gauge(
            'firmware_response_time_p95_seconds',
            '95th percentile response time',
            ['endpoint'],
            registry=self.registry
        )
        
        # Throughput metrics
        self.requests_per_second = Gauge(
            'firmware_requests_per_second',
            'Requests per second',
            ['endpoint'],
            registry=self.registry
        )
        
        logger.info("📊 FirmwareMetrics initialized")
    
    def record_request(self, endpoint: str, method: str, duration: float, success: bool):
        """
        Record request metrics
        
        Args:
            endpoint: API endpoint
            method: HTTP method
            duration: Request duration in seconds
            success: Whether request was successful
        """
        status = "success" if success else "error"
        
        # Increment counters
        self.request_count.labels(
            endpoint=endpoint,
            method=method,
            status=status
        ).inc()
        
        # Record duration
        self.request_duration.labels(
            endpoint=endpoint,
            method=method
        ).observe(duration)
        
        # Record errors
        if not success:
            self.error_count.labels(
                endpoint=endpoint,
                error_type="request_failed"
            ).inc()
        
        logger.debug(f"📊 Recorded {endpoint} {method}: {duration:.3f}s, success={success}")
    
    def record_circuit_breaker_state(self, state: str):
        """Record circuit breaker state"""
        state_value = {"closed": 0, "open": 1, "half_open": 2}.get(state, 0)
        self.circuit_breaker_state.set(state_value)
        logger.debug(f"📊 Circuit breaker state: {state}")
    
    def record_connection_status(self, connected: bool):
        """Record connection status"""
        self.connection_status.set(1 if connected else 0)
        logger.debug(f"📊 Connection status: {connected}")
    
    def record_response_time_p95(self, endpoint: str, p95_value: float):
        """Record 95th percentile response time"""
        self.response_time_p95.labels(endpoint=endpoint).set(p95_value)
        logger.debug(f"📊 {endpoint} P95 response time: {p95_value:.3f}s")
    
    def record_requests_per_second(self, endpoint: str, rps: float):
        """Record requests per second"""
        self.requests_per_second.labels(endpoint=endpoint).set(rps)
        logger.debug(f"📊 {endpoint} RPS: {rps:.2f}")
    
    def get_metrics(self) -> str:
        """Get metrics in Prometheus format"""
        from prometheus_client import generate_latest
        return generate_latest(self.registry)
    
    def get_connection_metrics(self) -> Dict[str, Any]:
        """Get connection metrics as dict"""
        return {
            "connection_status": self.connection_status._value.get(),
            "circuit_breaker_state": self.circuit_breaker_state._value.get(),
            "total_requests": sum([
                self.request_count._value.get((endpoint, method, status), 0)
                for endpoint in ["robot_status", "telemetry", "robot_command"]
                for method in ["GET", "POST"]
                for status in ["success", "error"]
            ]),
            "total_errors": sum([
                self.error_count._value.get((endpoint, error_type), 0)
                for endpoint in ["robot_status", "telemetry", "robot_command"]
                for error_type in ["request_failed"]
            ])
        }
