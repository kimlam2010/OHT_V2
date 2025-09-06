"""
Firmware Health Checker for Production Environment

This module provides comprehensive health monitoring for robot firmware.
It includes health checks, metrics collection, and health status reporting.
"""

import asyncio
import logging
from typing import Dict, Any, Optional
from datetime import datetime, timedelta
from dataclasses import dataclass

import httpx

logger = logging.getLogger(__name__)


@dataclass
class HealthMetrics:
    """Health metrics data structure"""
    response_time: float
    status_code: int
    error_count: int
    last_check: datetime
    uptime: Optional[float] = None
    memory_usage: Optional[float] = None
    cpu_usage: Optional[float] = None


class FirmwareHealthChecker:
    """
    Comprehensive firmware health monitoring system
    
    Features:
    - Health endpoint monitoring
    - Response time tracking
    - Error rate monitoring
    - System metrics collection
    - Health status reporting
    """
    
    def __init__(self, firmware_url: str):
        self.firmware_url = firmware_url
        self.http_client = httpx.AsyncClient(
            timeout=httpx.Timeout(5.0),  # 5 second timeout for health checks
            limits=httpx.Limits(max_keepalive_connections=2, max_connections=5)
        )
        
        # Health tracking
        self.health_history: list[HealthMetrics] = []
        self.max_history_size = 100
        self.consecutive_failures = 0
        self.max_consecutive_failures = 3
        
        # Health thresholds
        self.max_response_time = 2.0  # 2 seconds
        self.max_error_rate = 0.1     # 10%
        
        logger.info(f"🔍 FirmwareHealthChecker initialized for {firmware_url}")
        
    async def __aenter__(self):
        """Async context manager entry"""
        return self
        
    async def __aexit__(self, exc_type, exc_val, exc_tb):
        """Async context manager exit"""
        await self.close()
        
    async def close(self):
        """Close HTTP client"""
        await self.http_client.aclose()
        logger.info("🔌 FirmwareHealthChecker HTTP client closed")
        
    async def check_health(self) -> bool:
        """Perform comprehensive health check"""
        try:
            start_time = datetime.utcnow()
            
            # Basic health endpoint check
            response = await self.http_client.get(f"{self.firmware_url}/health")
            response_time = (datetime.utcnow() - start_time).total_seconds()
            
            # Create health metrics
            metrics = HealthMetrics(
                response_time=response_time,
                status_code=response.status_code,
                error_count=0 if response.status_code == 200 else 1,
                last_check=start_time
            )
            
            # Check if health check passed
            is_healthy = response.status_code == 200 and response_time <= self.max_response_time
            
            if is_healthy:
                self.consecutive_failures = 0
                logger.debug(f"✅ Health check passed: {response_time:.3f}s")
            else:
                self.consecutive_failures += 1
                logger.warning(f"⚠️ Health check failed: {response.status_code}, {response_time:.3f}s")
                
            # Store metrics
            self._store_metrics(metrics)
            
            # Check if we should consider firmware unhealthy
            if self.consecutive_failures >= self.max_consecutive_failures:
                logger.error(f"🚨 Max consecutive failures reached: {self.consecutive_failures}")
                return False
                
            return is_healthy
            
        except Exception as e:
            self.consecutive_failures += 1
            logger.error(f"❌ Health check error: {e}")
            
            # Store error metrics
            error_metrics = HealthMetrics(
                response_time=0.0,
                status_code=0,
                error_count=1,
                last_check=datetime.utcnow()
            )
            self._store_metrics(error_metrics)
            
            return False
            
    async def get_detailed_health(self) -> Dict[str, Any]:
        """Get detailed health information"""
        try:
            # Basic health check
            basic_health = await self.check_health()
            
            # Get system metrics if available
            system_metrics = await self._get_system_metrics()
            
            # Calculate health statistics
            health_stats = self._calculate_health_statistics()
            
            return {
                "overall_health": basic_health,
                "firmware_url": self.firmware_url,
                "last_check": datetime.utcnow().isoformat(),
                "consecutive_failures": self.consecutive_failures,
                "max_consecutive_failures": self.max_consecutive_failures,
                "system_metrics": system_metrics,
                "health_statistics": health_stats,
                "health_history_count": len(self.health_history)
            }
            
        except Exception as e:
            logger.error(f"❌ Detailed health check failed: {e}")
            return {
                "overall_health": False,
                "error": str(e),
                "firmware_url": self.firmware_url,
                "last_check": datetime.utcnow().isoformat()
            }
            
    async def get_health_metrics(self) -> Dict[str, Any]:
        """Get current health metrics"""
        if not self.health_history:
            return {"error": "No health metrics available"}
            
        latest = self.health_history[-1]
        return {
            "response_time": latest.response_time,
            "status_code": latest.status_code,
            "error_count": latest.error_count,
            "last_check": latest.last_check.isoformat(),
            "uptime": latest.uptime,
            "memory_usage": latest.memory_usage,
            "cpu_usage": latest.cpu_usage
        }
        
    async def force_health_check(self) -> bool:
        """Force immediate health check"""
        logger.info("🔍 Forcing immediate health check")
        return await self.check_health()
        
    async def reset_health_tracking(self):
        """Reset health tracking data"""
        self.health_history.clear()
        self.consecutive_failures = 0
        logger.info("🔄 Health tracking data reset")
        
    # Private methods
    
    def _store_metrics(self, metrics: HealthMetrics):
        """Store health metrics in history"""
        self.health_history.append(metrics)
        
        # Maintain history size
        if len(self.health_history) > self.max_history_size:
            self.health_history.pop(0)
            
    def _calculate_health_statistics(self) -> Dict[str, Any]:
        """Calculate health statistics from history"""
        if not self.health_history:
            return {}
            
        recent_metrics = self.health_history[-10:]  # Last 10 checks
        
        response_times = [m.response_time for m in recent_metrics if m.response_time > 0]
        error_counts = [m.error_count for m in recent_metrics]
        
        if response_times:
            avg_response_time = sum(response_times) / len(response_times)
            max_response_time = max(response_times)
            min_response_time = min(response_times)
        else:
            avg_response_time = max_response_time = min_response_time = 0
            
        total_checks = len(recent_metrics)
        total_errors = sum(error_counts)
        error_rate = total_errors / total_checks if total_checks > 0 else 0
        
        return {
            "total_checks": total_checks,
            "total_errors": total_errors,
            "error_rate": error_rate,
            "avg_response_time": avg_response_time,
            "max_response_time": max_response_time,
            "min_response_time": min_response_time,
            "health_score": max(0, 100 - (error_rate * 100))
        }
        
    async def _get_system_metrics(self) -> Dict[str, Any]:
        """Get system metrics from firmware if available"""
        try:
            response = await self.http_client.get(f"{self.firmware_url}/api/v1/system/metrics")
            if response.status_code == 200:
                metrics = response.json()
                
                # Update latest health metrics with system info
                if self.health_history:
                    latest = self.health_history[-1]
                    latest.uptime = metrics.get("uptime")
                    latest.memory_usage = metrics.get("memory_usage")
                    latest.cpu_usage = metrics.get("cpu_usage")
                    
                return metrics
            else:
                return {}
                
        except Exception as e:
            logger.debug(f"System metrics unavailable: {e}")
            return {}
            
    async def _check_health_endpoints(self) -> Dict[str, bool]:
        """Check multiple health endpoints"""
        endpoints = [
            "/health",
            "/api/v1/health",
            "/status"
        ]
        
        results = {}
        for endpoint in endpoints:
            try:
                response = await self.http_client.get(f"{self.firmware_url}{endpoint}")
                results[endpoint] = response.status_code == 200
            except Exception:
                results[endpoint] = False
                
        return results
