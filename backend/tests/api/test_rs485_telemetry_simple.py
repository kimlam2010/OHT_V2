"""
Simple Tests for RS485 Telemetry API endpoints - Issue #91
"""

import pytest
import os
from datetime import datetime
from fastapi.testclient import TestClient
from unittest.mock import AsyncMock, patch, MagicMock

# Set testing environment
os.environ["TESTING"] = "true"
os.environ["USE_MOCK_FIRMWARE"] = "true"

from app.main import app
from app.schemas.rs485 import RS485ModuleTelemetry, RS485RegisterInfo


class TestRS485TelemetrySimple:
    """Simple test for RS485 Telemetry API endpoints - Issue #91"""
    
    @pytest.fixture
    def client(self):
        """Test client fixture"""
        return TestClient(app)
    
    def test_telemetry_data_structure(self):
        """Test telemetry data structure matches issue #91 requirements"""
        # Create sample telemetry data
        registers = [
            RS485RegisterInfo(
                address="0x0001",
                name="Battery Level",
                value=85.4,
                unit="%",
                mode="R",
                status="normal",
                threshold_warning=20.0,
                threshold_critical=10.0
            )
        ]
        
        telemetry = RS485ModuleTelemetry(
            module_addr="0x02",
            module_name="Power",
            registers=registers,
            last_updated=datetime.now()
        )
        
        # Convert to dict to check structure
        data = telemetry.model_dump()
        
        # Check structure matches issue #91 requirements
        assert "module_addr" in data
        assert "module_name" in data
        assert "registers" in data
        assert "last_updated" in data
        
        register = data["registers"][0]
        assert "address" in register
        assert "name" in register
        assert "value" in register
        assert "unit" in register
        assert "mode" in register
        assert "status" in register
        assert "threshold_warning" in register
        assert "threshold_critical" in register
        
        # Check values match expected format from issue #91
        assert data["module_addr"] == "0x02"
        assert data["module_name"] == "Power"
        assert register["address"] == "0x0001"
        assert register["name"] == "Battery Level"
        assert register["value"] == 85.4
        assert register["unit"] == "%"
        assert register["mode"] == "R"
        assert register["status"] == "normal"
    
    def test_mock_service_telemetry_generation(self):
        """Test mock service generates proper telemetry data"""
        from app.services.mock_rs485_service import mock_rs485_service
        
        # Test different module types
        test_modules = [2, 3, 4, 5]  # Power, Safety, Travel, Dock
        
        for module_addr in test_modules:
            registers = mock_rs485_service._generate_mock_registers(module_addr, f"Module_{module_addr}")
            
            assert len(registers) > 0
            
            # Check register structure
            for register in registers:
                assert register.address.startswith("0x")
                assert register.name != ""
                assert isinstance(register.value, (int, float))
                assert register.mode in ["R", "W", "RW"]
                assert register.status in ["normal", "warning", "critical"]
                
                # Check address format
                assert len(register.address) >= 6  # 0x0001 format
                assert register.address[0:2] == "0x"
                
                # Check unit is provided
                assert isinstance(register.unit, str)
                
                # Check thresholds are optional but if present, are numbers
                if register.threshold_warning is not None:
                    assert isinstance(register.threshold_warning, (int, float))
                if register.threshold_critical is not None:
                    assert isinstance(register.threshold_critical, (int, float))
    
    @pytest.mark.asyncio
    async def test_mock_service_telemetry_async(self):
        """Test mock service async telemetry methods"""
        from app.services.mock_rs485_service import mock_rs485_service
        
        # Initialize mock service
        await mock_rs485_service.initialize()
        
        # Test get_module_telemetry
        telemetry = await mock_rs485_service.get_module_telemetry(2)  # Power module
        
        assert telemetry is not None
        assert telemetry.module_addr == "0x02"
        assert telemetry.module_name == "Power"
        assert len(telemetry.registers) > 0
        
        # Find a writable register
        writable_register = None
        for reg in telemetry.registers:
            if reg.mode in ["W", "RW"]:
                writable_register = reg
                break
        
        if writable_register:
            # Test update_module_register
            result = await mock_rs485_service.update_module_register(
                address=2,
                register_address=writable_register.address,
                value=writable_register.value + 10,
                force=False
            )
            
            assert result["success"] is True
            assert "updated successfully" in result["message"]
            assert "old_value" in result
            assert "new_value" in result
        
        # Test update read-only register (should fail)
        readonly_register = None
        for reg in telemetry.registers:
            if reg.mode == "R":
                readonly_register = reg
                break
                
        if readonly_register:
            result = await mock_rs485_service.update_module_register(
                address=2,
                register_address=readonly_register.address,
                value=readonly_register.value + 10,
                force=False
            )
            
            assert result["success"] is False
            assert "read-only" in result["message"].lower()
        
        # Test non-existent module
        telemetry_none = await mock_rs485_service.get_module_telemetry(99)
        assert telemetry_none is None
        
        # Cleanup
        await mock_rs485_service.shutdown()
    
    def test_register_info_validation(self):
        """Test RS485RegisterInfo validation"""
        # Valid register
        register = RS485RegisterInfo(
            address="0x0001",
            name="Battery Level",
            value=85.4,
            unit="%",
            mode="R",
            status="normal",
            threshold_warning=20.0,
            threshold_critical=10.0
        )
        
        assert register.address == "0x0001"
        assert register.name == "Battery Level"
        assert register.value == 85.4
        assert register.unit == "%"
        assert register.mode == "R"
        assert register.status == "normal"
        assert register.threshold_warning == 20.0
        assert register.threshold_critical == 10.0
        
        # Register without thresholds
        register2 = RS485RegisterInfo(
            address="0x0002",
            name="Status",
            value=1,
            unit="",
            mode="R",
            status="normal"
        )
        
        assert register2.threshold_warning is None
        assert register2.threshold_critical is None
    
    def test_telemetry_response_structure(self):
        """Test RS485TelemetryResponse structure"""
        from app.schemas.rs485 import RS485TelemetryResponse
        
        # Create sample data
        registers = [
            RS485RegisterInfo(
                address="0x0001",
                name="Battery Level",
                value=85.4,
                unit="%",
                mode="R",
                status="normal",
                threshold_warning=20.0,
                threshold_critical=10.0
            )
        ]
        
        telemetry = RS485ModuleTelemetry(
            module_addr="0x02",
            module_name="Power",
            registers=registers,
            last_updated=datetime.now()
        )
        
        response = RS485TelemetryResponse(
            success=True,
            data=telemetry,
            message="Test message",
            timestamp=datetime.now()
        )
        
        # Convert to dict
        response_dict = response.model_dump()
        
        # Check response structure
        assert "success" in response_dict
        assert "data" in response_dict
        assert "message" in response_dict
        assert "timestamp" in response_dict
        
        # Check data structure matches issue #91 format
        data = response_dict["data"]
        assert data["module_addr"] == "0x02"
        assert data["module_name"] == "Power"
        assert len(data["registers"]) == 1
        
        register = data["registers"][0]
        assert register["address"] == "0x0001"
        assert register["name"] == "Battery Level"
        assert register["value"] == 85.4
        assert register["unit"] == "%"
        assert register["mode"] == "R"
        assert register["status"] == "normal"
    
    def test_register_update_request_structure(self):
        """Test RS485RegisterUpdateRequest structure"""
        from app.schemas.rs485 import RS485RegisterUpdateRequest
        
        # Basic request
        request = RS485RegisterUpdateRequest(
            register_address="0x0010",
            value=600.0
        )
        
        assert request.register_address == "0x0010"
        assert request.value == 600.0
        assert request.force is False  # Default value
        
        # Request with force
        request2 = RS485RegisterUpdateRequest(
            register_address="0x0010",
            value=1200.0,
            force=True
        )
        
        assert request2.force is True
    
    def test_authentication_endpoints_exist(self, client):
        """Test that telemetry endpoints exist (will fail auth but not 404)"""
        # GET endpoint should exist (will return 401 without auth)
        response = client.get("/api/v1/rs485/modules/2/telemetry")
        assert response.status_code != 404  # Should be 401 (auth required)
        
        # POST endpoint should exist (will return 401 without auth)
        payload = {
            "register_address": "0x0010",
            "value": 600.0
        }
        response = client.post("/api/v1/rs485/modules/2/telemetry", json=payload)
        assert response.status_code != 404  # Should be 401 (auth required)
    
    def test_api_validation(self, client):
        """Test API validation for invalid requests"""
        # Invalid module address (too high)
        response = client.get("/api/v1/rs485/modules/99/telemetry")
        # Should be 401 (auth) or 422 (validation) but not 404 (not found)
        assert response.status_code in [401, 422]
        
        # Invalid module address (too low)
        response = client.get("/api/v1/rs485/modules/0/telemetry")
        # Should be 401 (auth) or 422 (validation) but not 404 (not found)
        assert response.status_code in [401, 422]
        
        # Invalid JSON payload
        response = client.post("/api/v1/rs485/modules/2/telemetry", 
                             json={"invalid": "payload"})
        # Should be 401 (auth) or 422 (validation) but not 404 (not found)
        assert response.status_code in [401, 422]
