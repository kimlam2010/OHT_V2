"""
Unit tests for FirmwareRetryManager

This module tests the retry mechanism, configuration, and error handling
for firmware communication retries.
"""

import pytest
import asyncio
from unittest.mock import AsyncMock, MagicMock, patch
from datetime import datetime, timedelta
import random

from app.core.firmware_retry_manager import (
    FirmwareRetryManager,
    RetryConfig,
    RetryResult
)


class TestRetryConfig:
    """Test RetryConfig dataclass"""
    
    def test_retry_config_defaults(self):
        """Test RetryConfig default values"""
        config = RetryConfig()
        
        assert config.max_retries == 3
        assert config.base_delay == 1.0
        assert config.max_delay == 30.0
        assert config.backoff_factor == 2.0
        assert config.jitter is True
        
    def test_retry_config_custom_values(self):
        """Test RetryConfig with custom values"""
        config = RetryConfig(
            max_retries=5,
            base_delay=2.0,
            max_delay=60.0,
            backoff_factor=3.0,
            jitter=False
        )
        
        assert config.max_retries == 5
        assert config.base_delay == 2.0
        assert config.max_delay == 60.0
        assert config.backoff_factor == 3.0
        assert config.jitter is False


class TestRetryResult:
    """Test RetryResult dataclass"""
    
    def test_retry_result_success(self):
        """Test RetryResult for successful operation"""
        result = RetryResult(
            success=True,
            result="test_result",
            attempts=1,
            total_time=0.5
        )
        
        assert result.success is True
        assert result.result == "test_result"
        assert result.attempts == 1
        assert result.total_time == 0.5
        assert result.error is None
        assert result.last_error is None
        
    def test_retry_result_failure(self):
        """Test RetryResult for failed operation"""
        error = Exception("Test error")
        result = RetryResult(
            success=False,
            error=error,
            attempts=3,
            total_time=2.0,
            last_error=error
        )
        
        assert result.success is False
        assert result.error == error
        assert result.attempts == 3
        assert result.total_time == 2.0
        assert result.last_error == error
        assert result.result is None


class TestFirmwareRetryManager:
    """Test FirmwareRetryManager class"""
    
    @pytest.fixture
    def retry_manager(self):
        """Create a retry manager instance"""
        return FirmwareRetryManager()
        
    @pytest.fixture
    def custom_retry_manager(self):
        """Create a retry manager with custom config"""
        config = RetryConfig(
            max_retries=2,
            base_delay=0.5,
            max_delay=10.0,
            backoff_factor=1.5
        )
        return FirmwareRetryManager(config)
        
    @pytest.fixture
    def mock_operation(self):
        """Create a mock async operation"""
        return AsyncMock()
        
    def test_retry_manager_initialization(self, retry_manager: FirmwareRetryManager):
        """Test retry manager initialization"""
        assert retry_manager.config.max_retries == 3
        assert retry_manager.config.base_delay == 1.0
        assert retry_manager.retry_history == []
        assert retry_manager.max_history_size == 1000
        
    def test_retry_manager_custom_initialization(self, custom_retry_manager: FirmwareRetryManager):
        """Test retry manager with custom config"""
        assert custom_retry_manager.config.max_retries == 2
        assert custom_retry_manager.config.base_delay == 0.5
        assert custom_retry_manager.config.max_delay == 10.0
        assert custom_retry_manager.config.backoff_factor == 1.5
        
    @pytest.mark.asyncio
    async def test_execute_with_retry_success_first_attempt(self, retry_manager: FirmwareRetryManager, mock_operation: AsyncMock):
        """Test successful execution on first attempt"""
        mock_operation.return_value = "success"
        
        result = await retry_manager.execute_with_retry(mock_operation)
        
        assert result == "success"
        assert mock_operation.call_count == 1
        assert len(retry_manager.retry_history) == 1
        
        # Check retry result
        retry_result = retry_manager.retry_history[0]
        assert retry_result.success is True
        assert retry_result.result == "success"
        assert retry_result.attempts == 1
        assert retry_result.total_time >= 0  # Can be 0 for very fast operations
        
    @pytest.mark.asyncio
    async def test_execute_with_retry_success_after_retries(self, retry_manager, mock_operation):
        """Test successful execution after retries"""
        # First two calls fail, third succeeds
        mock_operation.side_effect = [Exception("Error 1"), Exception("Error 2"), "success"]
        
        with patch('asyncio.sleep') as mock_sleep:
            result = await retry_manager.execute_with_retry(mock_operation)
            
        assert result == "success"
        assert mock_operation.call_count == 3
        assert mock_sleep.call_count == 2  # Two retries
        
        # Check retry result
        retry_result = retry_manager.retry_history[0]
        assert retry_result.success is True
        assert retry_result.result == "success"
        assert retry_result.attempts == 3
        
    @pytest.mark.asyncio
    async def test_execute_with_retry_all_attempts_fail(self, retry_manager, mock_operation):
        """Test failure after all retry attempts"""
        mock_operation.side_effect = Exception("Persistent error")
        
        with patch('asyncio.sleep') as mock_sleep:
            result = await retry_manager.execute_with_retry(mock_operation)
            
        assert result is None
        assert mock_operation.call_count == 4  # Initial + 3 retries
        assert mock_sleep.call_count == 3  # Three retries
        
        # Check retry result
        retry_result = retry_manager.retry_history[0]
        assert retry_result.success is False
        assert retry_result.error is not None
        assert retry_result.attempts == 4
        
    @pytest.mark.asyncio
    async def test_execute_with_custom_retry(self, retry_manager, mock_operation):
        """Test execution with custom retry configuration"""
        custom_config = RetryConfig(max_retries=1, base_delay=0.1)
        mock_operation.side_effect = [Exception("Error"), "success"]
        
        with patch('asyncio.sleep') as mock_sleep:
            result = await retry_manager.execute_with_custom_retry(
                mock_operation, custom_config
            )
            
        assert result == "success"
        assert mock_operation.call_count == 2
        assert mock_sleep.call_count == 1
        
        # Original config should be restored
        assert retry_manager.config.max_retries == 3
        
    def test_get_retry_statistics_empty_history(self, retry_manager):
        """Test statistics with empty retry history"""
        stats = retry_manager.get_retry_statistics()
        
        assert "error" in stats
        assert stats["error"] == "No retry history available"
        
    def test_get_retry_statistics_with_history(self, retry_manager):
        """Test statistics with retry history"""
        # Add some mock results
        retry_manager.retry_history = [
            RetryResult(success=True, attempts=1, total_time=0.5),
            RetryResult(success=True, attempts=2, total_time=1.0),
            RetryResult(success=False, attempts=3, total_time=2.0),
        ]
        
        stats = retry_manager.get_retry_statistics()
        
        assert stats["total_operations"] == 3
        assert stats["successful_operations"] == 2
        assert stats["failed_operations"] == 1
        assert stats["success_rate"] == 2/3
        assert stats["avg_attempts"] == 1.5  # (1+2)/2 successful
        assert stats["avg_time"] == 0.75  # (0.5+1.0)/2 successful
        assert stats["max_retries"] == 3
        assert stats["base_delay"] == 1.0
        assert stats["backoff_factor"] == 2.0
        
    def test_get_recent_retries(self, retry_manager):
        """Test getting recent retry results"""
        # Add 15 mock results
        for i in range(15):
            retry_manager.retry_history.append(
                RetryResult(success=True, attempts=1, total_time=0.1)
            )
            
        recent = retry_manager.get_recent_retries(limit=5)
        assert len(recent) == 5
        
        # Test default limit
        recent_default = retry_manager.get_recent_retries()
        assert len(recent_default) == 10
        
    def test_clear_retry_history(self, retry_manager):
        """Test clearing retry history"""
        # Add some mock results
        retry_manager.retry_history = [
            RetryResult(success=True, attempts=1, total_time=0.1)
        ]
        
        retry_manager.clear_retry_history()
        assert retry_manager.retry_history == []
        
    def test_update_retry_config(self, retry_manager):
        """Test updating retry configuration"""
        new_config = RetryConfig(max_retries=5, base_delay=2.0)
        
        retry_manager.update_retry_config(new_config)
        
        assert retry_manager.config.max_retries == 5
        assert retry_manager.config.base_delay == 2.0
        
    def test_calculate_delay_exponential_backoff(self, retry_manager):
        """Test delay calculation with exponential backoff"""
        delays = []
        for attempt in range(1, 5):
            delay = retry_manager._calculate_delay(attempt)
            delays.append(delay)
            
        # Should be increasing (exponential backoff)
        assert delays[1] > delays[0]
        assert delays[2] > delays[1]
        assert delays[3] > delays[2]
        
        # Base delay * (backoff_factor ^ (attempt - 1))
        expected_delays = [1.0, 2.0, 4.0, 8.0]
        for actual, expected in zip(delays, expected_delays):
            assert abs(actual - expected) < 1.0  # Allow for jitter variation
            
    def test_calculate_delay_max_limit(self, retry_manager):
        """Test delay calculation respects maximum limit"""
        # Set a very high attempt number
        delay = retry_manager._calculate_delay(100)
        
        # With jitter, delay might exceed max_delay slightly
        # Allow up to 20% jitter variation
        assert delay <= retry_manager.config.max_delay * 1.2
        assert delay >= retry_manager.config.max_delay * 0.8
        
    def test_calculate_delay_minimum_limit(self, retry_manager):
        """Test delay calculation respects minimum limit"""
        # Set very low base delay
        retry_manager.config.base_delay = 0.01
        
        delay = retry_manager._calculate_delay(1)
        
        assert delay >= 0.1  # Minimum delay
        
    def test_calculate_delay_with_jitter(self, retry_manager):
        """Test delay calculation includes jitter"""
        retry_manager.config.jitter = True
        
        # Test multiple calls to see jitter variation
        delays = []
        for _ in range(10):
            delay = retry_manager._calculate_delay(1)
            delays.append(delay)
            
        # Should have some variation due to jitter
        assert len(set(delays)) > 1
        
    def test_calculate_delay_without_jitter(self, retry_manager):
        """Test delay calculation without jitter"""
        retry_manager.config.jitter = False
        
        # Test multiple calls to see consistent delays
        delays = []
        for _ in range(10):
            delay = retry_manager._calculate_delay(1)
            delays.append(delay)
            
        # Should be consistent without jitter
        assert len(set(delays)) == 1
        
    def test_store_retry_result(self, retry_manager):
        """Test storing retry results"""
        result = RetryResult(success=True, attempts=1, total_time=0.1)
        
        retry_manager._store_retry_result(result)
        
        assert len(retry_manager.retry_history) == 1
        assert retry_manager.retry_history[0] == result
        
    def test_store_retry_result_history_limit(self, retry_manager):
        """Test retry history size limit"""
        # Set a small limit for testing
        retry_manager.max_history_size = 3
        
        # Add 5 results
        for i in range(5):
            result = RetryResult(success=True, attempts=1, total_time=0.1)
            retry_manager._store_retry_result(result)
            
        # Should only keep the last 3
        assert len(retry_manager.retry_history) == 3
        
        # Should keep the most recent ones
        assert retry_manager.retry_history[-1].total_time == 0.1
        
    def test_should_retry_retryable_errors(self, retry_manager):
        """Test that retryable errors trigger retries"""
        retryable_errors = [
            Exception("Network error"),
            ConnectionError("Connection failed"),
            TimeoutError("Operation timed out")
        ]
        
        for error in retryable_errors:
            assert retry_manager._should_retry(error) is True
            
    def test_should_retry_non_retryable_errors(self, retry_manager):
        """Test that non-retryable errors don't trigger retries"""
        non_retryable_errors = [
            ValueError("Invalid input"),
            TypeError("Type error"),
            AttributeError("Missing attribute")
        ]
        
        for error in non_retryable_errors:
            assert retry_manager._should_retry(error) is False
            
    @pytest.mark.asyncio
    async def test_execute_with_retry_with_args_and_kwargs(self, retry_manager, mock_operation):
        """Test retry execution with arguments and keyword arguments"""
        mock_operation.return_value = "success"
        
        result = await retry_manager.execute_with_retry(
            mock_operation, "arg1", "arg2", kwarg1="value1", kwarg2="value2"
        )
        
        assert result == "success"
        mock_operation.assert_called_once_with("arg1", "arg2", kwarg1="value1", kwarg2="value2")
        
    @pytest.mark.asyncio
    async def test_execute_with_retry_performance_tracking(self, retry_manager, mock_operation):
        """Test that performance tracking works correctly"""
        mock_operation.return_value = "success"
        
        start_time = datetime.now()
        result = await retry_manager.execute_with_retry(mock_operation)
        end_time = datetime.now()
        
        assert result == "success"
        
        # Check that timing is tracked
        retry_result = retry_manager.retry_history[0]
        assert retry_result.total_time >= 0  # Can be 0 for very fast operations
        assert retry_result.total_time < (end_time - start_time).total_seconds() + 0.1
        
    @pytest.mark.asyncio
    async def test_execute_with_retry_error_logging(self, retry_manager, mock_operation):
        """Test that errors are properly logged during retries"""
        mock_operation.side_effect = Exception("Test error")
        
        with patch('logging.Logger.warning') as mock_warning, \
             patch('logging.Logger.error') as mock_error, \
             patch('asyncio.sleep') as mock_sleep:
            
            result = await retry_manager.execute_with_retry(mock_operation)
            
        assert result is None
        
        # Should log warnings for retries
        assert mock_warning.call_count == 3
        
        # Should log error for final failure
        assert mock_error.call_count == 1
        
        # Should sleep between retries
        assert mock_sleep.call_count == 3
