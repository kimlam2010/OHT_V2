"""
Unit tests for telemetry service NoneType error fix
Tests for issue #105: Telemetry Service NoneType Error
"""

import pytest
from unittest.mock import AsyncMock, patch
from datetime import datetime, timezone
from fastapi import HTTPException

from app.api.v1.telemetry import get_current_telemetry
from app.schemas.user import UserResponse
from app.services.telemetry import TelemetryService


@pytest.fixture
def mock_user():
    """Mock authenticated user"""
    from datetime import datetime, timezone
    return UserResponse(
        id=1,
        username="test_user",
        email="test@example.com",
        full_name="Test User",
        role="operator",
        status="active",
        created_at=datetime.now(timezone.utc),
        updated_at=datetime.now(timezone.utc)
    )


@pytest.fixture
def telemetry_service():
    """Create telemetry service instance for testing"""
    return TelemetryService(use_mock=True)


class TestTelemetryNoneTypeFix:
    """Test suite for telemetry NoneType error fix"""
    
    @pytest.mark.asyncio
    async def test_telemetry_with_none_data_field(self, mock_user, telemetry_service):
        """Test telemetry endpoint handles None data field gracefully"""
        # Mock firmware service to return None for data field
        mock_telemetry = {
            "timestamp": "2025-01-28T10:00:00Z",
            "data": None  # This was causing the NoneType error
        }
        
        with patch.object(telemetry_service, 'get_current_telemetry', return_value=mock_telemetry):
            with patch('app.api.v1.telemetry.telemetry_service', telemetry_service):
                result = await get_current_telemetry(current_user=mock_user)
                
                # Should not raise NoneType error
                assert result is not None
                assert result.motor_speed == 0.0  # Default value
                assert result.motor_temperature == 0.0  # Default value
                assert result.dock_status == "ready"
                assert result.safety_status == "normal"
    
    @pytest.mark.asyncio
    async def test_telemetry_with_missing_robot_status(self, mock_user, telemetry_service):
        """Test telemetry endpoint handles missing robot_status field"""
        mock_telemetry = {
            "timestamp": "2025-01-28T10:00:00Z",
            "data": {}  # Empty data, no robot_status
        }
        
        with patch.object(telemetry_service, 'get_current_telemetry', return_value=mock_telemetry):
            with patch('app.api.v1.telemetry.telemetry_service', telemetry_service):
                result = await get_current_telemetry(current_user=mock_user)
                
                assert result is not None
                assert result.motor_speed == 0.0
                assert result.motor_temperature == 0.0
                assert result.dock_status == "ready"
                assert result.safety_status == "normal"
    
    @pytest.mark.asyncio
    async def test_telemetry_with_valid_data(self, mock_user, telemetry_service):
        """Test telemetry endpoint works with valid data structure"""
        mock_telemetry = {
            "timestamp": "2025-01-28T10:00:00Z",
            "data": {
                "robot_status": {
                    "speed": 1500.0,
                    "temperature": 42.5,
                    "dock_status": "docked",
                    "safety_status": "active"
                }
            }
        }
        
        with patch.object(telemetry_service, 'get_current_telemetry', return_value=mock_telemetry):
            with patch('app.api.v1.telemetry.telemetry_service', telemetry_service):
                result = await get_current_telemetry(current_user=mock_user)
                
                assert result is not None
                assert result.motor_speed == 1500.0
                assert result.motor_temperature == 42.5
                assert result.dock_status == "docked"
                assert result.safety_status == "active"
    
    @pytest.mark.asyncio
    async def test_telemetry_with_empty_response(self, mock_user, telemetry_service):
        """Test telemetry endpoint handles empty response"""
        with patch.object(telemetry_service, 'get_current_telemetry', return_value=None):
            with patch('app.api.v1.telemetry.telemetry_service', telemetry_service):
                with pytest.raises(HTTPException) as exc_info:
                    await get_current_telemetry(current_user=mock_user)
                
                assert exc_info.value.status_code == 500
                assert "No telemetry data available" in str(exc_info.value.detail)
    
    @pytest.mark.asyncio
    async def test_telemetry_with_error_response(self, mock_user, telemetry_service):
        """Test telemetry endpoint handles error response"""
        mock_telemetry = {
            "error": "Firmware communication failed",
            "timestamp": "2025-01-28T10:00:00Z"
        }
        
        with patch.object(telemetry_service, 'get_current_telemetry', return_value=mock_telemetry):
            with patch('app.api.v1.telemetry.telemetry_service', telemetry_service):
                with pytest.raises(HTTPException) as exc_info:
                    await get_current_telemetry(current_user=mock_user)
                
                assert exc_info.value.status_code == 500
                assert "Firmware communication failed" in str(exc_info.value.detail)
    
    @pytest.mark.asyncio
    async def test_telemetry_performance_monitoring(self, mock_user, telemetry_service):
        """Test telemetry endpoint performance monitoring"""
        mock_telemetry = {
            "timestamp": "2025-01-28T10:00:00Z",
            "data": {"robot_status": {"speed": 1500.0}}
        }
        
        with patch.object(telemetry_service, 'get_current_telemetry', return_value=mock_telemetry):
            with patch('app.api.v1.telemetry.telemetry_service', telemetry_service):
                # Should complete without performance warnings for fast requests
                result = await get_current_telemetry(current_user=mock_user)
                assert result is not None
    
    @pytest.mark.asyncio
    async def test_telemetry_service_exception_handling(self, mock_user, telemetry_service):
        """Test telemetry service exception handling"""
        with patch.object(telemetry_service, 'get_current_telemetry', side_effect=Exception("Service error")):
            with patch('app.api.v1.telemetry.telemetry_service', telemetry_service):
                with pytest.raises(HTTPException) as exc_info:
                    await get_current_telemetry(current_user=mock_user)
                
                assert exc_info.value.status_code == 500
                # Should return structured error response
                error_detail = exc_info.value.detail
                assert isinstance(error_detail, dict)
                assert "error" in error_detail
                assert "message" in error_detail
                assert "timestamp" in error_detail
                assert "suggestion" in error_detail


class TestTelemetryServiceFix:
    """Test telemetry service internal fixes"""
    
    @pytest.mark.asyncio
    async def test_safe_navigation_in_service(self):
        """Test safe navigation in telemetry service"""
        service = TelemetryService(use_mock=True)
        
        # Mock firmware service to return problematic data
        with patch.object(service.firmware_service, 'get_telemetry_data') as mock_firmware:
            mock_firmware.return_value = {"data": None}
            
            result = await service.get_current_telemetry()
            
            # Should not raise error and should have timestamp
            assert result is not None
            assert "timestamp" in result
            assert "error" not in result
    
    def test_telemetry_summary_with_empty_history(self):
        """Test telemetry summary with empty history"""
        service = TelemetryService(use_mock=True)
        service.telemetry_history = []
        
        summary = service.get_telemetry_summary()
        assert "message" in summary
        assert summary["message"] == "No telemetry data available"
    
    def test_telemetry_history_limit(self):
        """Test telemetry history size limit"""
        service = TelemetryService(use_mock=True)
        
        # Add more than max history size
        for i in range(1200):  # More than max_history_size (1000)
            service._store_telemetry({"id": i, "timestamp": f"2025-01-28T{i:02d}:00:00Z"})
        
        # Should maintain max size
        assert len(service.telemetry_history) == service.max_history_size
