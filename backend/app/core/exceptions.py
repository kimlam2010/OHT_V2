"""
Custom exceptions for OHT-50 Backend
"""

from fastapi import HTTPException, status


class BackendException(Exception):
    """Base exception for backend errors"""
    pass


class DatabaseException(BackendException):
    """Database operation failed"""
    pass


class AuthenticationException(BackendException):
    """Authentication failed"""
    pass


class AuthorizationException(BackendException):
    """Authorization failed"""
    pass


class ValidationException(BackendException):
    """Data validation failed"""
    pass


class ConfigurationException(BackendException):
    """Configuration error"""
    pass


class MonitoringException(BackendException):
    """Monitoring system error"""
    pass


class FirmwareCommunicationException(BackendException):
    """Firmware communication failed"""
    pass


class RS485CommunicationException(BackendException):
    """RS485 communication failed"""
    pass


# HTTP Exceptions
class RobotNotFoundError(HTTPException):
    def __init__(self):
        super().__init__(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Robot not found"
        )


class RobotControlError(HTTPException):
    def __init__(self, detail: str = "Robot control failed"):
        super().__init__(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail=detail
        )


class SafetyViolationError(HTTPException):
    def __init__(self, detail: str = "Safety violation detected"):
        super().__init__(
            status_code=status.HTTP_403_FORBIDDEN,
            detail=detail
        )


class CommunicationError(HTTPException):
    def __init__(self, detail: str = "Communication error"):
        super().__init__(
            status_code=status.HTTP_503_SERVICE_UNAVAILABLE,
            detail=detail
        )
