"""
Structured logging với correlation ID cho OHT-50 Backend
"""
import logging
import sys
import uuid
from contextvars import ContextVar
from typing import Any, Dict, Optional

import structlog
from structlog.stdlib import LoggerFactory

# Context variable để lưu correlation ID
correlation_id_var: ContextVar[Optional[str]] = ContextVar(
    "correlation_id", default=None
)


def get_correlation_id() -> Optional[str]:
    """Lấy correlation ID từ context"""
    return correlation_id_var.get()


def set_correlation_id(correlation_id: str) -> None:
    """Set correlation ID vào context"""
    correlation_id_var.set(correlation_id)


def generate_correlation_id() -> str:
    """Tạo correlation ID mới"""
    return str(uuid.uuid4())


def add_correlation_id_to_log_record(record: logging.LogRecord) -> None:
    """Thêm correlation ID vào log record"""
    correlation_id = get_correlation_id()
    if correlation_id:
        record.correlation_id = correlation_id


def configure_logging(
    log_level: str = "INFO",
    log_format: str = "json",
    service_name: str = "oht50-backend",
) -> None:
    """Cấu hình structured logging"""

    # Cấu hình structlog
    structlog.configure(
        processors=[
            structlog.stdlib.filter_by_level,
            structlog.stdlib.add_logger_name,
            structlog.stdlib.add_log_level,
            structlog.stdlib.PositionalArgumentsFormatter(),
            structlog.processors.TimeStamper(fmt="iso"),
            structlog.processors.StackInfoRenderer(),
            structlog.processors.format_exc_info,
            structlog.processors.UnicodeDecoder(),
            add_correlation_id,
            structlog.processors.JSONRenderer()
            if log_format == "json"
            else structlog.dev.ConsoleRenderer(),
        ],
        context_class=dict,
        logger_factory=LoggerFactory(),
        wrapper_class=structlog.stdlib.BoundLogger,
        cache_logger_on_first_use=True,
    )

    # Cấu hình standard library logging
    logging.basicConfig(
        format="%(message)s",
        stream=sys.stdout,
        level=getattr(logging, log_level.upper()),
    )

    # Thêm correlation ID processor
    logging.getLogger().handlers[0].addFilter(add_correlation_id_to_log_record)


def add_correlation_id(
    logger: Any, method_name: str, event_dict: Dict[str, Any]
) -> Dict[str, Any]:
    """Thêm correlation ID vào log event"""
    correlation_id = get_correlation_id()
    if correlation_id:
        event_dict["correlation_id"] = correlation_id
    return event_dict


def get_logger(name: str) -> structlog.stdlib.BoundLogger:
    """Lấy logger với structured logging"""
    return structlog.get_logger(name)


class CorrelationIdMiddleware:
    """Middleware để tự động tạo và quản lý correlation ID"""

    def __init__(self, app):
        self.app = app

    async def __call__(self, scope, receive, send):
        # Tạo correlation ID mới cho mỗi request
        correlation_id = generate_correlation_id()
        set_correlation_id(correlation_id)

        # Thêm correlation ID vào scope để sử dụng trong request
        scope["correlation_id"] = correlation_id

        await self.app(scope, receive, send)


# Khởi tạo logging khi import module
configure_logging()
