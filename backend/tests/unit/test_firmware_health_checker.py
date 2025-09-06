"""
Unit tests for firmware health checker
"""

import pytest
from unittest.mock import AsyncMock, MagicMock, patch
from datetime import datetime

from app.core.firmware_health_checker import FirmwareHealthChecker, HealthMetrics


class TestFirmwareHealthChecker:
    """Test FirmwareHealthChecker class"""
    
    @pytest.fixture
    def health_checker(self) -> FirmwareHealthChecker:
        """FirmwareHealthChecker instance"""
        return FirmwareHealthChecker("http://localhost:8081")
    
    def test_firmware_health_checker_creation(self, health_checker: FirmwareHealthChecker) -> None:
        """Test FirmwareHealthChecker creation"""
        assert health_checker.firmware_url == "http://localhost:8081"
        assert health_checker.max_history_size == 100
        assert health_checker.consecutive_failures == 0
        assert health_checker.max_consecutive_failures == 3
        assert health_checker.max_response_time == 2.0
        assert health_checker.max_error_rate == 0.1
        assert len(health_checker.health_history) == 0
    
    def test_health_metrics_creation(self) -> None:
        """Test HealthMetrics creation"""
        now = datetime.utcnow()
        metrics = HealthMetrics(
            response_time=1.5,
            status_code=200,
            error_count=0,
            last_check=now,
            uptime=3600.0,
            memory_usage=45.2,
            cpu_usage=25.8
        )
        
        assert metrics.response_time == 1.5
        assert metrics.status_code == 200
        assert metrics.error_count == 0
        assert metrics.last_check == now
        assert metrics.uptime == 3600.0
        assert metrics.memory_usage == 45.2
        assert metrics.cpu_usage == 25.8
    
    async def test_reset_health_tracking(self, health_checker: FirmwareHealthChecker) -> None:
        """Test resetting health tracking data"""
        # Add some metrics and set failures
        metrics = HealthMetrics(
            response_time=1.0,
            status_code=200,
            error_count=0,
            last_check=datetime.now()
        )
        health_checker.health_history.append(metrics)
        health_checker.consecutive_failures = 3
        
        await health_checker.reset_health_tracking()
        
        assert len(health_checker.health_history) == 0
        assert health_checker.consecutive_failures == 0
    
    def test_store_metrics(self, health_checker: FirmwareHealthChecker) -> None:
        """Test storing metrics in history"""
        metrics = HealthMetrics(
            response_time=1.0,
            status_code=200,
            error_count=0,
            last_check=datetime.utcnow()
        )
        
        health_checker._store_metrics(metrics)
        
        assert len(health_checker.health_history) == 1
        assert health_checker.health_history[0] == metrics
    
    def test_store_metrics_history_size_limit(self, health_checker: FirmwareHealthChecker) -> None:
        """Test metrics history size limit"""
        health_checker.max_history_size = 3
        
        # Add more metrics than limit
        for i in range(5):
            metrics = HealthMetrics(
                response_time=float(i),
                status_code=200,
                error_count=0,
                last_check=datetime.utcnow()
            )
            health_checker._store_metrics(metrics)
        
        # Should maintain max size
        assert len(health_checker.health_history) == 3
        # Should keep latest metrics
        assert health_checker.health_history[-1].response_time == 4.0
    
    def test_calculate_health_statistics_empty_history(self, health_checker: FirmwareHealthChecker) -> None:
        """Test calculating health statistics with empty history"""
        result = health_checker._calculate_health_statistics()
        
        assert result == {}
    
    def test_calculate_health_statistics_with_history(self, health_checker: FirmwareHealthChecker) -> None:
        """Test calculating health statistics with history"""
        # Add some metrics to history
        for i in range(3):
            metrics = HealthMetrics(
                response_time=1.0 + i * 0.1,
                status_code=200,
                error_count=0,
                last_check=datetime.utcnow()
            )
            health_checker.health_history.append(metrics)
        
        result = health_checker._calculate_health_statistics()
        
        assert "avg_response_time" in result
        assert "total_checks" in result
        assert "error_rate" in result
    
    async def test_health_checker_configuration(self, health_checker: FirmwareHealthChecker) -> None:
        """Test health checker configuration"""
        # Test custom configuration
        health_checker.max_response_time = 5.0
        health_checker.max_error_rate = 0.2
        health_checker.max_consecutive_failures = 5
        
        assert health_checker.max_response_time == 5.0
        assert health_checker.max_error_rate == 0.2
        assert health_checker.max_consecutive_failures == 5
    
    async def test_health_checker_with_different_url(self) -> None:
        """Test health checker with different URL"""
        checker = FirmwareHealthChecker("http://192.168.1.100:8080")
        
        assert checker.firmware_url == "http://192.168.1.100:8080"
        assert checker.max_history_size == 100
        assert checker.consecutive_failures == 0
