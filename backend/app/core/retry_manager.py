"""
Smart Retry Manager - OHT-50
Exponential backoff với jitter để tránh thundering herd
"""

import asyncio
import random
from typing import Dict, Any, Callable
from dataclasses import dataclass
import logging

logger = logging.getLogger(__name__)

@dataclass
class RetryConfig:
    """Retry configuration cho từng loại operation"""
    max_retries: int = 3
    base_delay: float = 0.1
    max_delay: float = 30.0
    backoff_factor: float = 2.0
    jitter: bool = True

class RetryManager:
    """
    Smart Retry Manager với exponential backoff
    Khác nhau cho từng loại operation
    """
    
    def __init__(self):
        # Retry configs cho từng loại operation
        self.retry_configs = {
            "telemetry": RetryConfig(
                max_retries=3, 
                base_delay=0.1,
                max_delay=5.0
            ),
            "robot_command": RetryConfig(
                max_retries=5, 
                base_delay=0.5,
                max_delay=10.0
            ),
            "emergency_stop": RetryConfig(
                max_retries=10, 
                base_delay=0.1,
                max_delay=2.0
            ),
            "robot_status": RetryConfig(
                max_retries=3,
                base_delay=0.2,
                max_delay=5.0
            ),
            "safety_status": RetryConfig(
                max_retries=5,
                base_delay=0.3,
                max_delay=8.0
            ),
            "modules_status": RetryConfig(
                max_retries=3,
                base_delay=0.5,
                max_delay=10.0
            ),
            "default": RetryConfig(
                max_retries=3, 
                base_delay=1.0,
                max_delay=30.0
            )
        }
        
        logger.info("🔄 RetryManager initialized với configs cho {len(self.retry_configs)} operations")
    
    async def execute_with_retry(self, operation: str, func: Callable, priority: str = "normal") -> Any:
        """
        Execute function với retry strategy
        
        Args:
            operation: Tên operation (telemetry, robot_command, etc.)
            func: Function to execute
            priority: Priority level (normal, high, emergency)
            
        Returns:
            Result from function
            
        Raises:
            Last exception if all retries failed
        """
        config = self.retry_configs.get(operation, self.retry_configs["default"])
        
        # Adjust config based on priority
        if priority == "emergency":
            config = RetryConfig(
                max_retries=10,
                base_delay=0.05,
                max_delay=1.0,
                backoff_factor=1.5,
                jitter=False  # No jitter for emergency
            )
        elif priority == "high":
            config = RetryConfig(
                max_retries=config.max_retries + 2,
                base_delay=config.base_delay * 0.5,
                max_delay=config.max_delay * 0.5
            )
        
        last_exception = None
        
        for attempt in range(config.max_retries):
            try:
                logger.debug(f"🔄 {operation} attempt {attempt + 1}/{config.max_retries}")
                return await func()
                
            except Exception as e:
                last_exception = e
                logger.warning(f"❌ {operation} attempt {attempt + 1} failed: {e}")
                
                # Don't retry on last attempt
                if attempt == config.max_retries - 1:
                    break
                
                # Calculate delay với exponential backoff
                delay = self._calculate_delay(attempt, config)
                logger.debug(f"⏳ {operation} waiting {delay:.3f}s before retry")
                await asyncio.sleep(delay)
        
        # Log final failure
        logger.error(f"💥 {operation} failed after {config.max_retries} attempts")
        
        # Re-raise last exception
        raise last_exception
    
    def _calculate_delay(self, attempt: int, config: RetryConfig) -> float:
        """
        Calculate retry delay với exponential backoff và jitter
        
        Args:
            attempt: Current attempt number (0-based)
            config: Retry configuration
            
        Returns:
            Delay in seconds
        """
        # Exponential backoff
        delay = config.base_delay * (config.backoff_factor ** attempt)
        delay = min(delay, config.max_delay)
        
        if config.jitter:
            # Add random jitter để tránh thundering herd
            jitter_range = delay * 0.1  # 10% jitter
            jitter = random.uniform(-jitter_range, jitter_range)
            delay += jitter
            delay = max(0, delay)  # Ensure non-negative
        
        return delay
    
    def get_config(self, operation: str) -> RetryConfig:
        """Get retry config for operation"""
        return self.retry_configs.get(operation, self.retry_configs["default"])
    
    def update_config(self, operation: str, config: RetryConfig):
        """Update retry config for operation"""
        self.retry_configs[operation] = config
        logger.info(f"🔄 Updated retry config for {operation}")
    
    def get_all_configs(self) -> Dict[str, RetryConfig]:
        """Get all retry configs"""
        return self.retry_configs.copy()
