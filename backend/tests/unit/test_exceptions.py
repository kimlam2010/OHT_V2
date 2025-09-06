"""
Unit tests for custom exceptions
"""

import pytest
from fastapi import HTTPException, status

from app.core.exceptions import (
    BackendException,
    DatabaseException,
    AuthenticationException,
    AuthorizationException,
    ValidationException,
    ConfigurationException,
    MonitoringException,
    FirmwareCommunicationException,
    RS485CommunicationException,
    RobotNotFoundError,
    RobotControlError,
    SafetyViolationError,
    CommunicationError
)


class TestBackendExceptions:
    """Test base backend exceptions"""
    
    def test_backend_exception_inheritance(self):
        """Test BackendException inheritance"""
        exception = BackendException("Test error")
        assert isinstance(exception, Exception)
        assert str(exception) == "Test error"
    
    def test_database_exception(self):
        """Test DatabaseException"""
        exception = DatabaseException("Database connection failed")
        assert isinstance(exception, BackendException)
        assert str(exception) == "Database connection failed"
    
    def test_authentication_exception(self):
        """Test AuthenticationException"""
        exception = AuthenticationException("Invalid credentials")
        assert isinstance(exception, BackendException)
        assert str(exception) == "Invalid credentials"
    
    def test_authorization_exception(self):
        """Test AuthorizationException"""
        exception = AuthorizationException("Insufficient permissions")
        assert isinstance(exception, BackendException)
        assert str(exception) == "Insufficient permissions"
    
    def test_validation_exception(self):
        """Test ValidationException"""
        exception = ValidationException("Invalid data format")
        assert isinstance(exception, BackendException)
        assert str(exception) == "Invalid data format"
    
    def test_configuration_exception(self):
        """Test ConfigurationException"""
        exception = ConfigurationException("Missing configuration")
        assert isinstance(exception, BackendException)
        assert str(exception) == "Missing configuration"
    
    def test_monitoring_exception(self):
        """Test MonitoringException"""
        exception = MonitoringException("Monitoring service failed")
        assert isinstance(exception, BackendException)
        assert str(exception) == "Monitoring service failed"
    
    def test_firmware_communication_exception(self):
        """Test FirmwareCommunicationException"""
        exception = FirmwareCommunicationException("Firmware timeout")
        assert isinstance(exception, BackendException)
        assert str(exception) == "Firmware timeout"
    
    def test_rs485_communication_exception(self):
        """Test RS485CommunicationException"""
        exception = RS485CommunicationException("RS485 connection lost")
        assert isinstance(exception, BackendException)
        assert str(exception) == "RS485 connection lost"


class TestHTTPExceptions:
    """Test HTTP exceptions"""
    
    def test_robot_not_found_error(self):
        """Test RobotNotFoundError"""
        exception = RobotNotFoundError()
        assert isinstance(exception, HTTPException)
        assert exception.status_code == status.HTTP_404_NOT_FOUND
        assert exception.detail == "Robot not found"
    
    def test_robot_control_error_default(self):
        """Test RobotControlError with default detail"""
        exception = RobotControlError()
        assert isinstance(exception, HTTPException)
        assert exception.status_code == status.HTTP_400_BAD_REQUEST
        assert exception.detail == "Robot control failed"
    
    def test_robot_control_error_custom(self):
        """Test RobotControlError with custom detail"""
        custom_detail = "Invalid movement command"
        exception = RobotControlError(custom_detail)
        assert isinstance(exception, HTTPException)
        assert exception.status_code == status.HTTP_400_BAD_REQUEST
        assert exception.detail == custom_detail
    
    def test_safety_violation_error_default(self):
        """Test SafetyViolationError with default detail"""
        exception = SafetyViolationError()
        assert isinstance(exception, HTTPException)
        assert exception.status_code == status.HTTP_403_FORBIDDEN
        assert exception.detail == "Safety violation detected"
    
    def test_safety_violation_error_custom(self):
        """Test SafetyViolationError with custom detail"""
        custom_detail = "Speed limit exceeded"
        exception = SafetyViolationError(custom_detail)
        assert isinstance(exception, HTTPException)
        assert exception.status_code == status.HTTP_403_FORBIDDEN
        assert exception.detail == custom_detail
    
    def test_communication_error_default(self):
        """Test CommunicationError with default detail"""
        exception = CommunicationError()
        assert isinstance(exception, HTTPException)
        assert exception.status_code == status.HTTP_503_SERVICE_UNAVAILABLE
        assert exception.detail == "Communication error"
    
    def test_communication_error_custom(self):
        """Test CommunicationError with custom detail"""
        custom_detail = "Network timeout"
        exception = CommunicationError(custom_detail)
        assert isinstance(exception, HTTPException)
        assert exception.status_code == status.HTTP_503_SERVICE_UNAVAILABLE
        assert exception.detail == custom_detail


class TestExceptionUsage:
    """Test exception usage patterns"""
    
    def test_exception_chaining(self):
        """Test exception chaining"""
        try:
            raise DatabaseException("Database error")
        except DatabaseException as e:
            assert isinstance(e, BackendException)
            assert str(e) == "Database error"
    
    def test_exception_with_context(self):
        """Test exception with additional context"""
        try:
            raise ValidationException("Invalid robot command")
        except ValidationException as e:
            assert "robot command" in str(e)
    
    def test_http_exception_raising(self):
        """Test HTTP exception raising"""
        with pytest.raises(RobotNotFoundError) as exc_info:
            raise RobotNotFoundError()
        
        exception = exc_info.value
        assert exception.status_code == 404
        assert exception.detail == "Robot not found"
    
    def test_multiple_exception_types(self):
        """Test multiple exception types"""
        exceptions = [
            DatabaseException("DB error"),
            AuthenticationException("Auth error"),
            ValidationException("Validation error")
        ]
        
        for exc in exceptions:
            assert isinstance(exc, BackendException)
            assert "error" in str(exc)
