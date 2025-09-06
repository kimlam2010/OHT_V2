"""
Unit tests for firmware fallback manager
"""

import pytest
from unittest.mock import AsyncMock, MagicMock, patch
from datetime import datetime, timedelta, timezone

from app.core.firmware_fallback_manager import FirmwareFallbackManager
from app.core.integration import MockFirmwareService


class TestFirmwareFallbackManager:
    """Test FirmwareFallbackManager class"""
    
    @pytest.fixture
    def fallback_manager(self) -> FirmwareFallbackManager:
        """FirmwareFallbackManager instance"""
        return FirmwareFallbackManager()
    
    @pytest.fixture
    def mock_mock_service(self) -> AsyncMock:
        """Mock MockFirmwareService"""
        mock_service = AsyncMock(spec=MockFirmwareService)
        mock_service.get_robot_status.return_value = {
            "status": "idle",
            "position": {"x": 100, "y": 200},
            "battery_level": 85
        }
        mock_service.get_telemetry_data.return_value = {
            "motor_speed": 1500,
            "temperature": 45.5
        }
        mock_service.send_robot_command.return_value = True
        return mock_service
    
    def test_firmware_fallback_manager_creation(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test FirmwareFallbackManager creation"""
        assert fallback_manager.fallback_enabled is True
        assert fallback_manager.fallback_mode is False
        assert fallback_manager.fallback_start_time is None
        assert fallback_manager.fallback_duration == timedelta(minutes=30)
        assert fallback_manager.fallback_count == 0
        assert fallback_manager.total_fallback_time == timedelta(0)
        assert isinstance(fallback_manager.mock_service, MockFirmwareService)
    
    async def test_get_fallback_status_success(self, fallback_manager: FirmwareFallbackManager, mock_mock_service: AsyncMock) -> None:
        """Test successful fallback status retrieval"""
        fallback_manager.mock_service = mock_mock_service
        
        result = await fallback_manager.get_fallback_status()
        
        assert result["status"] == "idle"
        assert result["position"]["x"] == 100
        assert result["battery_level"] == 85
        assert result["fallback_mode"] is False
        assert result["fallback_count"] == 0
        # fallback_enabled is not returned in success response
    
    async def test_get_fallback_status_fallback_mode(self, fallback_manager: FirmwareFallbackManager, mock_mock_service: AsyncMock) -> None:
        """Test fallback status in fallback mode"""
        fallback_manager.mock_service = mock_mock_service
        fallback_manager.fallback_mode = True
        fallback_manager.fallback_start_time = datetime.utcnow()
        
        result = await fallback_manager.get_fallback_status()
        
        assert result["fallback_mode"] is True
        assert result["fallback_start_time"] is not None
        assert result["fallback_count"] == 0
    
    async def test_get_fallback_status_fallback_duration_exceeded(self, fallback_manager: FirmwareFallbackManager, mock_mock_service: AsyncMock) -> None:
        """Test fallback status when duration is exceeded"""
        fallback_manager.mock_service = mock_mock_service
        fallback_manager.fallback_mode = True
        fallback_manager.fallback_start_time = datetime.utcnow() - timedelta(minutes=35)
        
        result = await fallback_manager.get_fallback_status()
        
        assert fallback_manager.fallback_mode is False
        assert fallback_manager.fallback_start_time is None
        assert result["fallback_mode"] is False
    
    async def test_get_fallback_status_disabled(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test fallback status when fallback is disabled"""
        fallback_manager.fallback_enabled = False
        
        with pytest.raises(Exception, match="Fallback mode is disabled"):
            await fallback_manager.get_fallback_status()
    
    async def test_get_fallback_status_error(self, fallback_manager: FirmwareFallbackManager, mock_mock_service: AsyncMock) -> None:
        """Test fallback status when mock service fails"""
        fallback_manager.mock_service = mock_mock_service
        mock_mock_service.get_robot_status.side_effect = Exception("Mock service error")
        
        result = await fallback_manager.get_fallback_status()
        
        assert result["status"] == "error"
        assert "Mock service error" in result["error"]
        assert result["fallback_mode"] is False
        assert result["fallback_enabled"] is True
    
    async def test_get_fallback_telemetry_success(self, fallback_manager: FirmwareFallbackManager, mock_mock_service: AsyncMock) -> None:
        """Test successful fallback telemetry retrieval"""
        fallback_manager.mock_service = mock_mock_service
        
        result = await fallback_manager.get_fallback_telemetry()
        
        assert result["motor_speed"] == 1500
        assert result["temperature"] == 45.5
        assert result["fallback_mode"] is False
        assert result["fallback_count"] == 0
        # fallback_enabled is not returned in success response
    
    async def test_get_fallback_telemetry_fallback_mode(self, fallback_manager: FirmwareFallbackManager, mock_mock_service: AsyncMock) -> None:
        """Test fallback telemetry in fallback mode"""
        fallback_manager.mock_service = mock_mock_service
        fallback_manager.fallback_mode = True
        fallback_manager.fallback_start_time = datetime.utcnow()
        
        result = await fallback_manager.get_fallback_telemetry()
        
        assert result["fallback_mode"] is True
        assert result["fallback_start_time"] is not None
        assert result["fallback_count"] == 0
    
    async def test_get_fallback_telemetry_disabled(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test fallback telemetry when fallback is disabled"""
        fallback_manager.fallback_enabled = False
        
        with pytest.raises(Exception, match="Fallback mode is disabled"):
            await fallback_manager.get_fallback_telemetry()
    
    async def test_get_fallback_telemetry_error(self, fallback_manager: FirmwareFallbackManager, mock_mock_service: AsyncMock) -> None:
        """Test fallback telemetry when mock service fails"""
        fallback_manager.mock_service = mock_mock_service
        mock_mock_service.get_telemetry_data.side_effect = Exception("Mock service error")
        
        result = await fallback_manager.get_fallback_telemetry()
        
        assert result["error"] == "Mock service error"
        assert result["fallback_mode"] is False
        assert result["fallback_enabled"] is True
    
    async def test_execute_fallback_command_success(self, fallback_manager: FirmwareFallbackManager, mock_mock_service: AsyncMock) -> None:
        """Test successful fallback command execution"""
        fallback_manager.mock_service = mock_mock_service
        
        command = {"command_type": "move", "parameters": {"x": 100, "y": 200}}
        result = await fallback_manager.execute_fallback_command(command)
        
        assert result is True
        mock_mock_service.send_robot_command.assert_called_once_with(command)
    
    async def test_execute_fallback_command_disabled(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test fallback command when fallback is disabled"""
        fallback_manager.fallback_enabled = False
        
        with pytest.raises(Exception, match="Fallback mode is disabled"):
            await fallback_manager.execute_fallback_command({"command_type": "move"})
    
    async def test_execute_fallback_command_error(self, fallback_manager: FirmwareFallbackManager, mock_mock_service: AsyncMock) -> None:
        """Test fallback command when mock service fails"""
        fallback_manager.mock_service = mock_mock_service
        mock_mock_service.send_robot_command.side_effect = Exception("Mock service error")
        
        result = await fallback_manager.execute_fallback_command({"command_type": "move"})
        
        assert result is False
    
    def test_enable_fallback_mode(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test enabling fallback mode"""
        fallback_manager.fallback_mode = False
        fallback_manager.fallback_start_time = None
        fallback_manager.fallback_count = 0
        
        fallback_manager.enable_fallback_mode()
        
        assert fallback_manager.fallback_mode is True
        assert fallback_manager.fallback_start_time is not None
        assert fallback_manager.fallback_count == 1
    
    def test_disable_fallback_mode(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test disabling fallback mode"""
        fallback_manager.fallback_mode = True
        fallback_manager.fallback_start_time = datetime.utcnow()
        
        fallback_manager.disable_fallback_mode()
        
        assert fallback_manager.fallback_mode is False
        assert fallback_manager.fallback_start_time is None
    
    def test_set_fallback_enabled(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test setting fallback enabled"""
        fallback_manager.set_fallback_enabled(False)
        assert fallback_manager.fallback_enabled is False
        
        fallback_manager.set_fallback_enabled(True)
        assert fallback_manager.fallback_enabled is True
    
    def test_set_fallback_duration(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test setting fallback duration"""
        new_duration = timedelta(hours=1)
        fallback_manager.set_fallback_duration(new_duration)
        
        assert fallback_manager.fallback_duration == new_duration
    
    def test_get_fallback_status_info(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test getting fallback status info"""
        result = fallback_manager.get_fallback_status_info()
        
        assert result["fallback_enabled"] is True
        assert result["fallback_mode"] is False
        assert result["fallback_count"] == 0
        assert result["fallback_duration"] == str(timedelta(minutes=30))
    
    def test_get_fallback_status_info_fallback_mode(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test getting fallback status info in fallback mode"""
        fallback_manager.enable_fallback_mode()
        
        result = fallback_manager.get_fallback_status_info()
        
        assert result["fallback_mode"] is True
        assert result["fallback_start_time"] is not None
        # current_fallback_duration and remaining_fallback_time may be None if timing is too fast
        assert result["fallback_count"] == 1
    
    def test_get_fallback_statistics(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test getting fallback statistics"""
        result = fallback_manager.get_fallback_statistics()
        
        assert result["total_fallbacks"] == 0
        assert result["fallback_enabled"] is True
        assert result["current_fallback_mode"] is False
    
    def test_get_fallback_statistics_with_fallbacks(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test getting fallback statistics with fallback history"""
        fallback_manager.enable_fallback_mode()
        fallback_manager.disable_fallback_mode()
        fallback_manager.enable_fallback_mode()
        
        result = fallback_manager.get_fallback_statistics()
        
        assert result["total_fallbacks"] == 2
        assert result["current_fallback_mode"] is True
    
    def test_reset_fallback_statistics(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test resetting fallback statistics"""
        fallback_manager.fallback_count = 5
        fallback_manager.total_fallback_time = timedelta(hours=2)
        fallback_manager.fallback_mode = True
        fallback_manager.fallback_start_time = datetime.utcnow()
        
        fallback_manager.reset_fallback_statistics()
        
        assert fallback_manager.fallback_count == 0
        assert fallback_manager.total_fallback_time == timedelta(0)
        assert fallback_manager.fallback_mode is False
        assert fallback_manager.fallback_start_time is None
    
    def test_is_fallback_mode_active(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test checking if fallback mode is active"""
        # Initially not active
        assert fallback_manager.is_fallback_mode_active() is False
        
        # Enable fallback mode
        fallback_manager.enable_fallback_mode()
        assert fallback_manager.is_fallback_mode_active() is True
        
        # Disable fallback mode
        fallback_manager.disable_fallback_mode()
        assert fallback_manager.is_fallback_mode_active() is False
    
    def test_should_use_fallback(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test determining if fallback should be used"""
        # Initially should not use fallback
        assert fallback_manager.should_use_fallback() is False
        
        # Enable fallback mode
        fallback_manager.enable_fallback_mode()
        assert fallback_manager.should_use_fallback() is True
        
        # Disable fallback mode
        fallback_manager.disable_fallback_mode()
        assert fallback_manager.should_use_fallback() is False
    
    def test_should_use_fallback_duration_expired(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test fallback usage when duration is expired"""
        fallback_manager.enable_fallback_mode()
        fallback_manager.fallback_start_time = datetime.utcnow() - timedelta(minutes=35)
        
        # Should not use fallback when duration expired
        assert fallback_manager.should_use_fallback() is False
        assert fallback_manager.fallback_mode is False
    
    def test_should_use_fallback_disabled(self, fallback_manager: FirmwareFallbackManager) -> None:
        """Test fallback usage when fallback is disabled"""
        fallback_manager.fallback_enabled = False
        
        assert fallback_manager.should_use_fallback() is False
    
    @patch('app.core.firmware_fallback_manager.datetime')
    def test_get_fallback_status_info_with_mock_datetime(self, mock_datetime, fallback_manager: FirmwareFallbackManager) -> None:
        """Test fallback status info with mocked datetime"""
        mock_now = datetime(2025, 1, 28, 10, 30, 0)
        mock_datetime.utcnow.return_value = mock_now
        
        fallback_manager.enable_fallback_mode()
        fallback_manager.fallback_start_time = mock_now - timedelta(minutes=15)
        
        result = fallback_manager.get_fallback_status_info()
        
        assert result["current_fallback_duration"] == str(timedelta(minutes=15))
        assert result["remaining_fallback_time"] == str(timedelta(minutes=15))
    
    @patch('app.core.firmware_fallback_manager.datetime')
    def test_get_fallback_statistics_with_mock_datetime(self, mock_datetime, fallback_manager: FirmwareFallbackManager) -> None:
        """Test fallback statistics with mocked datetime"""
        mock_now = datetime(2025, 1, 28, 10, 30, 0)
        mock_datetime.utcnow.return_value = mock_now
        
        fallback_manager.enable_fallback_mode()
        fallback_manager.fallback_start_time = mock_now - timedelta(minutes=20)
        fallback_manager.disable_fallback_mode()
        
        result = fallback_manager.get_fallback_statistics()
        
        assert result["total_fallbacks"] == 1
        assert "avg_fallback_duration" in result
