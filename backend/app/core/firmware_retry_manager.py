"""
Firmware Retry Manager for Production Environment

This module provides robust retry mechanisms for firmware communication.
It includes exponential backoff, retry limits, and comprehensive error handling.
"""

import asyncio
import logging
from typing import Callable, Any, Optional, TypeVar, Awaitable
from datetime import datetime, timedelta
from dataclasses import dataclass

logger = logging.getLogger(__name__)

T = TypeVar('T')


@dataclass
class RetryConfig:
    """Retry configuration parameters"""
    max_retries: int = 3
    base_delay: float = 1.0
    max_delay: float = 30.0
    backoff_factor: float = 2.0
    jitter: bool = True


@dataclass
class RetryResult:
    """Retry operation result"""
    success: bool
    result: Optional[T] = None
    error: Optional[Exception] = None
    attempts: int = 0
    total_time: float = 0.0
    last_error: Optional[Exception] = None


class FirmwareRetryManager:
    """
    Robust retry management system for firmware communication
    
    Features:
    - Exponential backoff strategy
    - Configurable retry limits
    - Jitter for distributed systems
    - Comprehensive error tracking
    - Performance monitoring
    """
    
    def __init__(self, config: Optional[RetryConfig] = None):
        self.config = config or RetryConfig()
        self.retry_history: list[RetryResult] = []
        self.max_history_size = 1000
        
        logger.info(f"🔄 FirmwareRetryManager initialized with max_retries={self.config.max_retries}")
        
    async def execute_with_retry(
        self, 
        operation: Callable[..., Awaitable[T]], 
        *args, 
        **kwargs
    ) -> Optional[T]:
        """
        Execute operation with retry mechanism
        
        Args:
            operation: Async function to execute
            *args: Positional arguments for operation
            **kwargs: Keyword arguments for operation
            
        Returns:
            Operation result or None if all retries failed
        """
        start_time = datetime.now()
        last_error = None
        
        for attempt in range(self.config.max_retries + 1):
            try:
                # Execute operation
                result = await operation(*args, **kwargs)
                
                # Success - create retry result
                total_time = (datetime.now() - start_time).total_seconds()
                retry_result = RetryResult(
                    success=True,
                    result=result,
                    attempts=attempt + 1,
                    total_time=total_time
                )
                
                # Store result
                self._store_retry_result(retry_result)
                
                if attempt > 0:
                    logger.info(f"✅ Operation succeeded after {attempt + 1} attempts")
                else:
                    logger.debug("✅ Operation succeeded on first attempt")
                    
                return result
                
            except Exception as e:
                last_error = e
                attempt_num = attempt + 1
                
                if attempt_num <= self.config.max_retries:
                    # Calculate delay with exponential backoff
                    delay = self._calculate_delay(attempt_num)
                    
                    logger.warning(
                        f"⚠️ Operation failed (attempt {attempt_num}/{self.config.max_retries + 1}): {e}. "
                        f"Retrying in {delay:.2f}s..."
                    )
                    
                    # Wait before retry
                    await asyncio.sleep(delay)
                else:
                    # All retries exhausted
                    total_time = (datetime.now() - start_time).total_seconds()
                    retry_result = RetryResult(
                        success=False,
                        error=e,
                        attempts=attempt_num,
                        total_time=total_time,
                        last_error=e
                    )
                    
                    # Store result
                    self._store_retry_result(retry_result)
                    
                    logger.error(f"❌ Operation failed after {attempt_num} attempts: {e}")
                    return None
                    
        return None
        
    async def execute_with_custom_retry(
        self,
        operation: Callable[..., Awaitable[T]],
        retry_config: RetryConfig,
        *args,
        **kwargs
    ) -> Optional[T]:
        """
        Execute operation with custom retry configuration
        
        Args:
            operation: Async function to execute
            retry_config: Custom retry configuration
            *args: Positional arguments for operation
            **kwargs: Keyword arguments for operation
            
        Returns:
            Operation result or None if all retries failed
        """
        # Store original config
        original_config = self.config
        
        try:
            # Use custom config
            self.config = retry_config
            return await self.execute_with_retry(operation, *args, **kwargs)
        finally:
            # Restore original config
            self.config = original_config
            
    def get_retry_statistics(self) -> dict[str, Any]:
        """Get retry performance statistics"""
        if not self.retry_history:
            return {"error": "No retry history available"}
            
        total_operations = len(self.retry_history)
        successful_operations = sum(1 for r in self.retry_history if r.success)
        failed_operations = total_operations - successful_operations
        
        # Calculate average attempts for successful operations
        successful_attempts = [r.attempts for r in self.retry_history if r.success]
        avg_attempts = sum(successful_attempts) / len(successful_attempts) if successful_attempts else 0
        
        # Calculate average time for successful operations
        successful_times = [r.total_time for r in self.retry_history if r.success]
        avg_time = sum(successful_times) / len(successful_times) if successful_times else 0
        
        # Calculate success rate
        success_rate = successful_operations / total_operations if total_operations > 0 else 0
        
        return {
            "total_operations": total_operations,
            "successful_operations": successful_operations,
            "failed_operations": failed_operations,
            "success_rate": success_rate,
            "avg_attempts": avg_attempts,
            "avg_time": avg_time,
            "max_retries": self.config.max_retries,
            "base_delay": self.config.base_delay,
            "backoff_factor": self.config.backoff_factor
        }
        
    def get_recent_retries(self, limit: int = 10) -> list[RetryResult]:
        """Get recent retry results"""
        return self.retry_history[-limit:] if self.retry_history else []
        
    def clear_retry_history(self):
        """Clear retry history"""
        self.retry_history.clear()
        logger.info("🔄 Retry history cleared")
        
    def update_retry_config(self, config: RetryConfig):
        """Update retry configuration"""
        self.config = config
        logger.info(f"🔄 Retry configuration updated: max_retries={config.max_retries}")
        
    # Private methods
    
    def _calculate_delay(self, attempt: int) -> float:
        """Calculate delay for retry attempt"""
        # Exponential backoff: base_delay * (backoff_factor ^ (attempt - 1))
        delay = self.config.base_delay * (self.config.backoff_factor ** (attempt - 1))
        
        # Apply maximum delay limit
        delay = min(delay, self.config.max_delay)
        
        # Add jitter if enabled
        if self.config.jitter:
            import random
            jitter_factor = 0.1  # 10% jitter
            jitter = delay * jitter_factor * random.uniform(-1, 1)
            delay += jitter
            
        return max(0.1, delay)  # Minimum 0.1 second delay
        
    def _store_retry_result(self, result: RetryResult):
        """Store retry result in history"""
        self.retry_history.append(result)
        
        # Maintain history size
        if len(self.retry_history) > self.max_history_size:
            self.retry_history.pop(0)
            
    def _should_retry(self, error: Exception) -> bool:
        """Determine if error should trigger retry"""
        # Retry on most exceptions except specific ones
        non_retryable_errors = (
            ValueError,  # Invalid input
            TypeError,   # Type errors
            AttributeError,  # Missing attributes
        )
        
        return not isinstance(error, non_retryable_errors)
