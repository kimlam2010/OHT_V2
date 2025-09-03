"""
Unit tests for safety service
"""

import pytest


class TestSafetyService:
    """Test safety service"""
    
    @pytest.mark.asyncio
    async def test_get_safety_status(self):
        """Test get safety status"""
        from app.services.safety import SafetyService
        test_service = SafetyService(use_mock=True)
        
        result = await test_service.get_safety_status()
        
        assert "status" in result
        assert "emergency_stop" in result
        assert "obstacles_detected" in result
        assert "temperature_normal" in result
    
    @pytest.mark.asyncio
    async def test_emergency_stop(self):
        """Test emergency stop"""
        from app.services.safety import SafetyService
        test_service = SafetyService(use_mock=True)
        
        result = await test_service.emergency_stop()
        
        assert result["success"] is True
        assert result["status"] == "emergency_stop"
    
    @pytest.mark.asyncio
    async def test_get_safety_alerts(self):
        """Test get safety alerts"""
        from app.services.safety import SafetyService
        test_service = SafetyService(use_mock=True)
        
        # get_safety_alerts is not async
        result = test_service.get_safety_alerts()
        
        assert isinstance(result, list)
    
    @pytest.mark.asyncio
    async def test_acknowledge_alert(self):
        """Test acknowledge alert"""
        from app.services.safety import SafetyService
        test_service = SafetyService(use_mock=True)
        
        # First create an alert by triggering emergency stop
        await test_service.emergency_stop()
        
        # Get the alert ID from the alerts list
        alerts = test_service.get_safety_alerts()
        if alerts:
            alert_id = alerts[0]["id"]
            result = await test_service.acknowledge_alert(alert_id)
            assert result["success"] is True
        else:
            # If no alerts created, test with non-existent ID
            result = await test_service.acknowledge_alert("test_alert_id")
            assert result["success"] is False
            assert "Alert not found" in result["error"]
