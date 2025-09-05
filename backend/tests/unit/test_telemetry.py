"""
Unit tests for telemetry service
"""

import pytest
import os
from app.services.telemetry import telemetry_service


class TestTelemetryService:
    """Test telemetry service"""
    
    @pytest.mark.asyncio
    async def test_get_current_telemetry(self):
        """Test get current telemetry"""
        # Set testing environment
        os.environ["TESTING"] = "true"
        
        result = await telemetry_service.get_current_telemetry()
        
        assert "motor_speed" in result
        assert "motor_temperature" in result
        assert "dock_status" in result
        assert "safety_status" in result
        assert result["motor_speed"] == 1500
        assert result["motor_temperature"] == 45
    
    @pytest.mark.asyncio
    async def test_get_telemetry_history(self):
        """Test get telemetry history"""
        # Set testing environment
        os.environ["TESTING"] = "true"
        
        result = await telemetry_service.get_telemetry_history(limit=10)
        
        assert isinstance(result, list)
        assert len(result) <= 10
    
    @pytest.mark.asyncio
    async def test_get_telemetry_summary(self):
        """Test get telemetry summary"""
        # Set testing environment
        os.environ["TESTING"] = "true"
        
        # First get some telemetry data to populate history
        await telemetry_service.get_current_telemetry()
        
        # Then get summary (not async)
        result = telemetry_service.get_telemetry_summary()
        
        assert "total_records" in result
        assert "data_points" in result
    
    @pytest.mark.asyncio
    async def test_get_module_status(self):
        """Test get module status - now supported by MockFirmwareService"""
        os.environ["TESTING"] = "true"
        result = await telemetry_service.get_module_status(module_id="mod-001")
        assert result.get("status") in ["online", "offline", "error"]
        assert result.get("module_id") == "mod-001"
    
    @pytest.mark.asyncio
    async def test_discover_modules(self):
        """Test discover modules - now supported by MockFirmwareService"""
        os.environ["TESTING"] = "true"
        modules = await telemetry_service.discover_modules()
        assert isinstance(modules, list)
        assert len(modules) >= 1
