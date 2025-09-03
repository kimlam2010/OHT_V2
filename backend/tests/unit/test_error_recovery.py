"""
Unit tests for error recovery service
"""

import pytest
import asyncio
from unittest.mock import AsyncMock, patch
from app.core.error_recovery import (
    CircuitBreaker, 
    RetryManager, 
    ErrorRecoveryService,
    CircuitState
)


class TestCircuitBreaker:
    """Test circuit breaker implementation"""
    
    def setup_method(self):
        """Setup test method"""
        self.circuit_breaker = CircuitBreaker(
            failure_threshold=3,
            recovery_timeout=1.0
        )
    
    @pytest.mark.asyncio
    async def test_circuit_closed_initially(self):
        """Test circuit breaker starts in closed state"""
        assert self.circuit_breaker.state == CircuitState.CLOSED
        assert self.circuit_breaker.failure_count == 0
    
    @pytest.mark.asyncio
    async def test_successful_call(self):
        """Test successful function call"""
        async def mock_func():
            return "success"
        
        result = await self.circuit_breaker.call(mock_func)
        assert result == "success"
        assert self.circuit_breaker.state == CircuitState.CLOSED
        assert self.circuit_breaker.failure_count == 0
    
    @pytest.mark.asyncio
    async def test_failure_opens_circuit(self):
        """Test circuit opens after failure threshold"""
        async def mock_func():
            raise Exception("Test error")
        
        # First few failures should not open circuit
        for i in range(3):
            with pytest.raises(Exception):
                await self.circuit_breaker.call(mock_func)
            assert self.circuit_breaker.failure_count == i + 1
            if i < 2:  # First 2 failures should keep circuit closed
                assert self.circuit_breaker.state == CircuitState.CLOSED
        
        # Third failure should open circuit
        with pytest.raises(Exception):
            await self.circuit_breaker.call(mock_func)
        
        assert self.circuit_breaker.state == CircuitState.OPEN
        assert self.circuit_breaker.failure_count == 3
    
    @pytest.mark.asyncio
    async def test_circuit_rejects_when_open(self):
        """Test circuit rejects calls when open"""
        # Open circuit first
        self.circuit_breaker.state = CircuitState.OPEN
        self.circuit_breaker.failure_count = 3
        
        async def mock_func():
            return "success"
        
        with pytest.raises(Exception, match="Circuit breaker is OPEN"):
            await self.circuit_breaker.call(mock_func)
    
    @pytest.mark.asyncio
    async def test_circuit_reset_after_timeout(self):
        """Test circuit resets after recovery timeout"""
        # Open circuit
        self.circuit_breaker.state = CircuitState.OPEN
        self.circuit_breaker.failure_count = 3
        
        # Mock the last failure time to simulate timeout
        from datetime import datetime, timedelta
        self.circuit_breaker.last_failure_time = datetime.now() - timedelta(seconds=2.0)
        
        # Should attempt reset to half-open
        async def mock_func():
            return "success"
        
        result = await self.circuit_breaker.call(mock_func)
        assert result == "success"
        assert self.circuit_breaker.state == CircuitState.CLOSED


class TestRetryManager:
    """Test retry manager implementation"""
    
    def setup_method(self):
        """Setup test method"""
        self.retry_manager = RetryManager(
            max_retries=2,
            base_delay=0.1,
            max_delay=1.0
        )
    
    @pytest.mark.asyncio
    async def test_successful_first_attempt(self):
        """Test successful function on first attempt"""
        async def mock_func():
            return "success"
        
        result = await self.retry_manager.execute_with_retry(mock_func)
        assert result == "success"
    
    @pytest.mark.asyncio
    async def test_successful_after_retry(self):
        """Test successful function after retry"""
        attempt_count = 0
        
        async def mock_func():
            nonlocal attempt_count
            attempt_count += 1
            if attempt_count == 1:
                raise Exception("First attempt failed")
            return "success"
        
        result = await self.retry_manager.execute_with_retry(mock_func)
        assert result == "success"
        assert attempt_count == 2
    
    @pytest.mark.asyncio
    async def test_all_retries_fail(self):
        """Test all retry attempts fail"""
        async def mock_func():
            raise Exception("Always fails")
        
        with pytest.raises(Exception, match="Always fails"):
            await self.retry_manager.execute_with_retry(mock_func)
    
    @pytest.mark.asyncio
    async def test_delay_calculation(self):
        """Test delay calculation between retries"""
        delays = []
        
        async def mock_func():
            delays.append(0)  # Record attempt
            raise Exception("Fails")
        
        with pytest.raises(Exception):
            await self.retry_manager.execute_with_retry(mock_func)
        
        # Should have 3 attempts (initial + 2 retries)
        assert len(delays) == 3


class TestErrorRecoveryService:
    """Test error recovery service"""
    
    def setup_method(self):
        """Setup test method"""
        self.service = ErrorRecoveryService()
    
    def test_get_circuit_breaker(self):
        """Test getting circuit breaker"""
        cb = self.service.get_circuit_breaker("test_service")
        assert isinstance(cb, CircuitBreaker)
        assert "test_service" in self.service.circuit_breakers
    
    def test_get_retry_manager(self):
        """Test getting retry manager"""
        rm = self.service.get_retry_manager("test_service")
        assert isinstance(rm, RetryManager)
        assert "test_service" in self.service.retry_managers
    
    @pytest.mark.asyncio
    async def test_execute_with_recovery_success(self):
        """Test successful execution with recovery"""
        async def mock_func():
            return "success"
        
        result = await self.service.execute_with_recovery("test_service", mock_func)
        assert result == "success"
        
        # Check stats
        status = self.service.get_service_status("test_service")
        assert status["error_stats"]["success_count"] == 1
        assert status["error_stats"]["failure_count"] == 0
    
    @pytest.mark.asyncio
    async def test_execute_with_recovery_failure(self):
        """Test failed execution with recovery"""
        async def mock_func():
            raise Exception("Test error")
        
        with pytest.raises(Exception, match="Test error"):
            await self.service.execute_with_recovery("test_service", mock_func)
        
        # Check stats
        status = self.service.get_service_status("test_service")
        assert status["error_stats"]["success_count"] == 0
        assert status["error_stats"]["failure_count"] == 1
    
    def test_get_service_status(self):
        """Test getting service status"""
        # Create some services first
        self.service.get_circuit_breaker("service1")
        self.service.get_retry_manager("service2")
        
        status = self.service.get_all_status()
        assert "service1" in status
        assert "service2" in status
        assert isinstance(status["service1"]["circuit_breaker"], dict)
        assert isinstance(status["service2"]["retry_manager"], dict)
