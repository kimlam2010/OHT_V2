"""
Custom exceptions for OHT-50 Backend
"""

from fastapi import HTTPException, status


class OHT50Exception(Exception):
    """Base exception for OHT-50 Backend"""
    pass


class RobotControlException(OHT50Exception):
    """Exception for robot control errors"""
    pass


class RS485CommunicationException(OHT50Exception):
    """Exception for RS485 communication errors"""
    pass


class SafetyException(OHT50Exception):
    """Exception for safety system errors"""
    pass


class TelemetryException(OHT50Exception):
    """Exception for telemetry errors"""
    pass


class ConfigurationException(OHT50Exception):
    """Exception for configuration errors"""
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
