"""
Structured Logging Configuration for OHT-50 Backend
"""

import logging
import json
import uuid
from datetime import datetime
from typing import Any, Dict, Optional
from contextvars import ContextVar

# Context variable for correlation ID
correlation_id: ContextVar[Optional[str]] = ContextVar('correlation_id', default=None)


class StructuredFormatter(logging.Formatter):
    """Structured JSON formatter for production logging"""
    
    def format(self, record: logging.LogRecord) -> str:
        """Format log record as structured JSON"""
        log_entry = {
            "timestamp": datetime.utcnow().isoformat(),
            "level": record.levelname,
            "logger": record.name,
            "message": record.getMessage(),
            "correlation_id": correlation_id.get(),
            "module": record.module,
            "function": record.funcName,
            "line": record.lineno
        }
        
        # Add exception info if present
        if record.exc_info:
            log_entry["exception"] = {
                "type": record.exc_info[0].__name__ if record.exc_info[0] else None,
                "message": str(record.exc_info[1]) if record.exc_info[1] else None,
                "traceback": self.formatException(record.exc_info)
            }
        
        # Add extra fields if present
        if hasattr(record, 'extra_fields'):
            log_entry.update(record.extra_fields)
        
        return json.dumps(log_entry, ensure_ascii=False)


class CorrelationFilter(logging.Filter):
    """Filter to add correlation ID to log records"""
    
    def filter(self, record: logging.LogRecord) -> bool:
        """Add correlation ID to log record"""
        record.correlation_id = correlation_id.get()
        return True


def setup_logging(level: str = "INFO", 
                  format_type: str = "structured",
                  log_file: Optional[str] = None) -> None:
    """Setup logging configuration"""
    
    # Clear existing handlers
    root_logger = logging.getLogger()
    for handler in root_logger.handlers[:]:
        root_logger.removeHandler(handler)
    
    # Set log level
    log_level = getattr(logging, level.upper())
    root_logger.setLevel(log_level)
    
    # Create formatter
    if format_type == "structured":
        formatter = StructuredFormatter()
    else:
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
    
    # Console handler
    console_handler = logging.StreamHandler()
    console_handler.setLevel(log_level)
    console_handler.setFormatter(formatter)
    console_handler.addFilter(CorrelationFilter())
    root_logger.addHandler(console_handler)
    
    # File handler (optional)
    if log_file:
        file_handler = logging.FileHandler(log_file)
        file_handler.setLevel(log_level)
        file_handler.setFormatter(formatter)
        file_handler.addFilter(CorrelationFilter())
        root_logger.addHandler(file_handler)
    
    # Set specific logger levels
    logging.getLogger("uvicorn").setLevel(logging.WARNING)
    logging.getLogger("uvicorn.access").setLevel(logging.WARNING)
    logging.getLogger("asyncio").setLevel(logging.WARNING)
    
    logging.info("Logging configured", extra={
        "extra_fields": {
            "level": level,
            "format_type": format_type,
            "log_file": log_file
        }
    })


def get_logger(name: str) -> logging.Logger:
    """Get logger with correlation ID support"""
    return logging.getLogger(name)


def set_correlation_id(corr_id: Optional[str] = None) -> str:
    """Set correlation ID for current context"""
    if corr_id is None:
        corr_id = str(uuid.uuid4())
    
    correlation_id.set(corr_id)
    return corr_id


def get_correlation_id() -> Optional[str]:
    """Get current correlation ID"""
    return correlation_id.get()


def log_with_context(logger: logging.Logger, 
                    level: str, 
                    message: str, 
                    **kwargs) -> None:
    """Log message with additional context"""
    extra_fields = {
        "extra_fields": kwargs
    }
    
    log_method = getattr(logger, level.lower())
    log_method(message, extra=extra_fields)


# Performance logging utilities
class PerformanceLogger:
    """Performance logging utility"""
    
    def __init__(self, logger: logging.Logger, operation: str):
        self.logger = logger
        self.operation = operation
        self.start_time = datetime.utcnow()
    
    def __enter__(self):
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        duration = (datetime.utcnow() - self.start_time).total_seconds()
        
        if exc_type is None:
            self.logger.info(
                f"Operation completed: {self.operation}",
                extra={
                    "extra_fields": {
                        "operation": self.operation,
                        "duration_seconds": duration,
                        "status": "success"
                    }
                }
            )
        else:
            self.logger.error(
                f"Operation failed: {self.operation}",
                extra={
                    "extra_fields": {
                        "operation": self.operation,
                        "duration_seconds": duration,
                        "status": "failed",
                        "error": str(exc_val)
                    }
                }
            )


def log_performance(logger: logging.Logger, operation: str):
    """Context manager for performance logging"""
    return PerformanceLogger(logger, operation)


# Request logging middleware
class RequestLogger:
    """Request logging utility"""
    
    def __init__(self, logger: logging.Logger):
        self.logger = logger
    
    def log_request(self, 
                   method: str, 
                   url: str, 
                   headers: Dict[str, str], 
                   body: Optional[str] = None) -> None:
        """Log incoming request"""
        self.logger.info(
            "Incoming request",
            extra={
                "extra_fields": {
                    "request_method": method,
                    "request_url": url,
                    "request_headers": dict(headers),
                    "request_body": body,
                    "correlation_id": get_correlation_id()
                }
            }
        )
    
    def log_response(self, 
                    status_code: int, 
                    response_time: float, 
                    response_size: Optional[int] = None) -> None:
        """Log response details"""
        self.logger.info(
            "Response sent",
            extra={
                "extra_fields": {
                    "response_status_code": status_code,
                    "response_time_seconds": response_time,
                    "response_size_bytes": response_size,
                    "correlation_id": get_correlation_id()
                }
            }
        )
    
    def log_error(self, 
                  error: Exception, 
                  context: Optional[Dict[str, Any]] = None) -> None:
        """Log error with context"""
        self.logger.error(
            f"Request error: {error}",
            extra={
                "extra_fields": {
                    "error_type": type(error).__name__,
                    "error_message": str(error),
                    "error_context": context or {},
                    "correlation_id": get_correlation_id()
                }
            }
        )
