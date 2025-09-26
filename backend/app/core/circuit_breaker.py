"""
Circuit Breaker Pattern Implementation - OHT-50
Ngăn chặn cascade failures khi firmware không available
"""

import time
from enum import Enum
from typing import Optional, Dict, Any
import logging

logger = logging.getLogger(__name__)

class CircuitBreakerState(Enum):
    CLOSED = "closed"
    OPEN = "open"
    HALF_OPEN = "half_open"

class CircuitBreaker:
    """
    Circuit Breaker để protect firmware calls
    State transitions: CLOSED -> OPEN -> HALF_OPEN -> CLOSED
    """
    
    def __init__(self, 
                 failure_threshold: int = 5,
                 recovery_timeout: float = 60.0,
                 success_threshold: int = 3):
        """
        Initialize Circuit Breaker
        
        Args:
            failure_threshold: Số failures để chuyển sang OPEN
            recovery_timeout: Thời gian chờ trước khi thử HALF_OPEN
            success_threshold: Số successes để chuyển từ HALF_OPEN sang CLOSED
        """
        self.failure_threshold = failure_threshold
        self.recovery_timeout = recovery_timeout
        self.success_threshold = success_threshold
        
        self.state = CircuitBreakerState.CLOSED
        self.failure_count = 0
        self.success_count = 0
        self.last_failure_time: Optional[float] = None
        
        logger.info(f"🔧 CircuitBreaker initialized: threshold={failure_threshold}, timeout={recovery_timeout}s")
    
    def allow_request(self) -> bool:
        """
        Check if request is allowed based on current state
        
        Returns:
            True if request is allowed, False otherwise
        """
        if self.state == CircuitBreakerState.CLOSED:
            return True
        elif self.state == CircuitBreakerState.OPEN:
            if self._should_attempt_reset():
                logger.info("🔄 Circuit breaker attempting reset: OPEN -> HALF_OPEN")
                self.state = CircuitBreakerState.HALF_OPEN
                self.success_count = 0
                return True
            return False
        elif self.state == CircuitBreakerState.HALF_OPEN:
            return True
        
        return False
    
    def record_success(self):
        """Record successful request"""
        if self.state == CircuitBreakerState.HALF_OPEN:
            self.success_count += 1
            logger.debug(f"✅ Circuit breaker success count: {self.success_count}/{self.success_threshold}")
            
            if self.success_count >= self.success_threshold:
                logger.info("🔄 Circuit breaker reset: HALF_OPEN -> CLOSED")
                self._reset()
        elif self.state == CircuitBreakerState.CLOSED:
            # Reset failure count on success
            self.failure_count = 0
    
    def record_failure(self):
        """Record failed request"""
        self.failure_count += 1
        self.last_failure_time = time.time()
        
        logger.warning(f"❌ Circuit breaker failure count: {self.failure_count}/{self.failure_threshold}")
        
        if self.failure_count >= self.failure_threshold:
            logger.error("🚨 Circuit breaker OPEN: Too many failures")
            self.state = CircuitBreakerState.OPEN
    
    def _should_attempt_reset(self) -> bool:
        """Check if should attempt reset from OPEN state"""
        if self.last_failure_time is None:
            return True
        return time.time() - self.last_failure_time >= self.recovery_timeout
    
    def _reset(self):
        """Reset circuit breaker to CLOSED state"""
        self.state = CircuitBreakerState.CLOSED
        self.failure_count = 0
        self.success_count = 0
        self.last_failure_time = None
        logger.info("✅ Circuit breaker reset to CLOSED")
    
    def get_status(self) -> Dict[str, Any]:
        """Get circuit breaker status"""
        return {
            "state": self.state.value,
            "failure_count": self.failure_count,
            "success_count": self.success_count,
            "last_failure_time": self.last_failure_time,
            "failure_threshold": self.failure_threshold,
            "recovery_timeout": self.recovery_timeout,
            "success_threshold": self.success_threshold
        }
    
    def force_reset(self):
        """Force reset circuit breaker (for testing)"""
        logger.warning("⚠️ Circuit breaker force reset")
        self._reset()
    
    def force_open(self):
        """Force open circuit breaker (for testing)"""
        logger.warning("⚠️ Circuit breaker force open")
        self.state = CircuitBreakerState.OPEN
        self.failure_count = self.failure_threshold
        self.last_failure_time = time.time()
