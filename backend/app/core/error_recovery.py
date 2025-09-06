"""
Error Recovery Service - Retry mechanisms and circuit breaker pattern
"""

import asyncio
import logging
import time
from typing import Any, Callable, Dict, Optional, TypeVar
from enum import Enum
from datetime import datetime, timedelta

# Type variable for generic functions
T = TypeVar('T')

logger = logging.getLogger(__name__)


class CircuitState(Enum):
    """Circuit breaker states"""
    CLOSED = "closed"      # Normal operation
    OPEN = "open"          # Failing, reject requests
    HALF_OPEN = "half_open"  # Testing if service recovered


class CircuitBreaker:
    """Circuit breaker implementation"""
    
    def __init__(self, 
                 failure_threshold: int = 5,
                 recovery_timeout: float = 60.0,
                 expected_exception: type = Exception):
        self.failure_threshold = failure_threshold
        self.recovery_timeout = recovery_timeout
        self.expected_exception = expected_exception
        
        self.failure_count = 0
        self.last_failure_time = None
        self.state = CircuitState.CLOSED
        
        logger.info(f"Circuit breaker initialized: threshold={failure_threshold}, timeout={recovery_timeout}s")
    
    async def call(self, func: Callable[..., T], *args: Any, **kwargs: Any) -> T:
        """Execute function with circuit breaker protection"""
        if self.state == CircuitState.OPEN:
            if self._should_attempt_reset():
                logger.info("Attempting circuit breaker reset")
                self.state = CircuitState.HALF_OPEN
            else:
                raise Exception(f"Circuit breaker is OPEN - service unavailable")
        
        try:
            result = await func(*args, **kwargs)
            self._on_success()
            return result
            
        except self.expected_exception as e:
            self._on_failure(e)
            raise
    
    def _on_success(self):
        """Handle successful execution"""
        self.failure_count = 0
        if self.state == CircuitState.HALF_OPEN:
            logger.info("Circuit breaker reset to CLOSED")
            self.state = CircuitState.CLOSED
    
    def _on_failure(self, error: Exception):
        """Handle execution failure"""
        self.failure_count += 1
        self.last_failure_time = datetime.now()
        
        logger.warning(f"Circuit breaker failure: {self.failure_count}/{self.failure_threshold}")
        
        if self.failure_count >= self.failure_threshold:
            logger.error("Circuit breaker opened - service failing")
            self.state = CircuitState.OPEN
    
    def _should_attempt_reset(self) -> bool:
        """Check if enough time has passed to attempt reset"""
        if not self.last_failure_time:
            return False
        
        elapsed = datetime.now() - self.last_failure_time
        return elapsed.total_seconds() >= self.recovery_timeout
    
    def get_status(self) -> Dict[str, Any]:
        """Get circuit breaker status"""
        return {
            "state": self.state.value,
            "failure_count": self.failure_count,
            "failure_threshold": self.failure_threshold,
            "last_failure": self.last_failure_time.isoformat() if self.last_failure_time else None,
            "recovery_timeout": self.recovery_timeout
        }


class RetryManager:
    """Retry manager with exponential backoff"""
    
    def __init__(self, 
                 max_retries: int = 3,
                 base_delay: float = 1.0,
                 max_delay: float = 60.0,
                 backoff_factor: float = 2.0):
        self.max_retries = max_retries
        self.base_delay = base_delay
        self.max_delay = max_delay
        self.backoff_factor = backoff_factor
        
        logger.info(f"Retry manager initialized: max_retries={max_retries}, base_delay={base_delay}s")
    
    async def execute_with_retry(self, 
                                func: Callable, 
                                *args, 
                                **kwargs) -> Any:
        """Execute function with retry logic"""
        last_exception = None
        
        for attempt in range(self.max_retries + 1):
            try:
                if attempt > 0:
                    delay = self._calculate_delay(attempt)
                    logger.info(f"Retry attempt {attempt}/{self.max_retries} after {delay}s delay")
                    await asyncio.sleep(delay)
                
                return await func(*args, **kwargs)
                
            except Exception as e:
                last_exception = e
                logger.warning(f"Attempt {attempt + 1} failed: {e}")
                
                if attempt == self.max_retries:
                    logger.error(f"All {self.max_retries + 1} attempts failed")
                    break
        
        # All retries exhausted
        raise last_exception
    
    def _calculate_delay(self, attempt: int) -> float:
        """Calculate delay for retry attempt"""
        delay = self.base_delay * (self.backoff_factor ** (attempt - 1))
        return min(delay, self.max_delay)


class ErrorRecoveryService:
    """Main error recovery service"""
    
    def __init__(self):
        self.circuit_breakers: Dict[str, CircuitBreaker] = {}
        self.retry_managers: Dict[str, RetryManager] = {}
        self.error_stats: Dict[str, Dict[str, Any]] = {}
        
        logger.info("Error Recovery Service initialized")
    
    def get_circuit_breaker(self, service_name: str, **kwargs) -> CircuitBreaker:
        """Get or create circuit breaker for service"""
        if service_name not in self.circuit_breakers:
            self.circuit_breakers[service_name] = CircuitBreaker(**kwargs)
        return self.circuit_breakers[service_name]
    
    def get_retry_manager(self, service_name: str, **kwargs) -> RetryManager:
        """Get or create retry manager for service"""
        if service_name not in self.retry_managers:
            self.retry_managers[service_name] = RetryManager(**kwargs)
        return self.retry_managers[service_name]
    
    async def execute_with_recovery(self, 
                                   service_name: str,
                                   func: Callable,
                                   *args,
                                   **kwargs) -> Any:
        """Execute function with both retry and circuit breaker protection"""
        start_time = time.time()
        
        try:
            # Get circuit breaker and retry manager
            circuit_breaker = self.get_circuit_breaker(service_name)
            retry_manager = self.get_retry_manager(service_name)
            
            # Execute with circuit breaker protection
            result = await circuit_breaker.call(
                retry_manager.execute_with_retry,
                func,
                *args,
                **kwargs
            )
            
            # Record success
            self._record_success(service_name, time.time() - start_time)
            return result
            
        except Exception as e:
            # Record failure
            self._record_failure(service_name, str(e), time.time() - start_time)
            raise
    
    def _record_success(self, service_name: str, execution_time: float):
        """Record successful execution"""
        if service_name not in self.error_stats:
            self.error_stats[service_name] = {
                "success_count": 0,
                "failure_count": 0,
                "total_execution_time": 0.0,
                "last_success": None,
                "last_failure": None
            }
        
        stats = self.error_stats[service_name]
        stats["success_count"] += 1
        stats["total_execution_time"] += execution_time
        stats["last_success"] = datetime.now().isoformat()
    
    def _record_failure(self, service_name: str, error: str, execution_time: float):
        """Record execution failure"""
        if service_name not in self.error_stats:
            self.error_stats[service_name] = {
                "success_count": 0,
                "failure_count": 0,
                "total_execution_time": 0.0,
                "last_success": None,
                "last_failure": None
            }
        
        stats = self.error_stats[service_name]
        stats["failure_count"] += 1
        stats["total_execution_time"] += execution_time
        stats["last_failure"] = datetime.now().isoformat()
    
    def get_service_status(self, service_name: str) -> Dict[str, Any]:
        """Get comprehensive status for a service"""
        status = {
            "service_name": service_name,
            "error_stats": self.error_stats.get(service_name, {}),
            "circuit_breaker": None,
            "retry_manager": None
        }
        
        if service_name in self.circuit_breakers:
            status["circuit_breaker"] = self.circuit_breakers[service_name].get_status()
        
        if service_name in self.retry_managers:
            retry_manager = self.retry_managers[service_name]
            status["retry_manager"] = {
                "max_retries": retry_manager.max_retries,
                "base_delay": retry_manager.base_delay,
                "max_delay": retry_manager.max_delay,
                "backoff_factor": retry_manager.backoff_factor
            }
        
        return status
    
    def get_all_status(self) -> Dict[str, Any]:
        """Get status for all services"""
        return {
            service_name: self.get_service_status(service_name)
            for service_name in set(list(self.circuit_breakers.keys()) + list(self.retry_managers.keys()))
        }


# Global error recovery service instance
error_recovery_service = ErrorRecoveryService()
