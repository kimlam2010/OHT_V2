"""
Custom exceptions cho OHT-50 Backend
"""
from typing import Any, Dict, Optional


class OHT50Exception(Exception):
    """Base exception cho OHT-50 Backend"""

    def __init__(
        self,
        message: str,
        error_code: Optional[str] = None,
        details: Optional[Dict[str, Any]] = None,
    ):
        super().__init__(message)
        self.message = message
        self.error_code = error_code
        self.details = details or {}


class ConfigurationError(OHT50Exception):
    """Lỗi cấu hình"""

    pass


class ValidationError(OHT50Exception):
    """Lỗi validation dữ liệu"""

    pass


class CommunicationError(OHT50Exception):
    """Lỗi giao tiếp (RS485, WebSocket, etc.)"""

    pass


class SafetyError(OHT50Exception):
    """Lỗi an toàn (E-Stop, interlock, etc.)"""

    pass


class TelemetryError(OHT50Exception):
    """Lỗi telemetry"""

    pass


class CenterCommunicationError(OHT50Exception):
    """Lỗi giao tiếp với Center"""

    pass


class MotorControlError(OHT50Exception):
    """Lỗi điều khiển motor"""

    pass


class StateMachineError(OHT50Exception):
    """Lỗi state machine"""

    pass


class DatabaseError(OHT50Exception):
    """Lỗi database"""

    pass


class AuthenticationError(OHT50Exception):
    """Lỗi xác thực"""

    pass


class PermissionError(OHT50Exception):
    """Lỗi quyền truy cập"""

    pass


# Error codes
ERROR_CODES = {
    "CONFIG_INVALID": "Cấu hình không hợp lệ",
    "CONFIG_NOT_FOUND": "Không tìm thấy cấu hình",
    "VALIDATION_FAILED": "Validation dữ liệu thất bại",
    "COMM_RS485_ERROR": "Lỗi giao tiếp RS485",
    "COMM_WEBSOCKET_ERROR": "Lỗi WebSocket",
    "SAFETY_ESTOP_ACTIVE": "E-Stop đang kích hoạt",
    "SAFETY_INTERLOCK": "Interlock an toàn",
    "TELEMETRY_BUFFER_FULL": "Buffer telemetry đầy",
    "CENTER_CONNECTION_LOST": "Mất kết nối Center",
    "MOTOR_FAULT": "Lỗi motor",
    "STATE_INVALID_TRANSITION": "Chuyển trạng thái không hợp lệ",
    "DB_CONNECTION_ERROR": "Lỗi kết nối database",
    "AUTH_INVALID_TOKEN": "Token không hợp lệ",
    "PERMISSION_DENIED": "Không có quyền truy cập",
}
