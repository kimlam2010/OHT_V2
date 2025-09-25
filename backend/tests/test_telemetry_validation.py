"""
Unit Tests for Telemetry Validation System
For Issue #144: Add Value Range Validation for Module Telemetry Data
"""

import pytest
import asyncio
from datetime import datetime
from typing import Dict, Any

from app.models.module_telemetry import (
    TelemetryField, ModuleTelemetry, TELEMETRY_FIELD_CONFIGS
)
from app.services.validation_service import ValidationService
from app.services.database_service import DatabaseService


class TestTelemetryField:
    """Test TelemetryField model validation"""
    
    def test_telemetry_field_valid(self):
        """Test valid telemetry field"""
        field = TelemetryField(
            value=24.1,
            min_value=0.0,
            max_value=30.0,
            unit="V",
            description="Supply voltage"
        )
        assert field.valid == True
        assert field.value == 24.1
    
    def test_telemetry_field_invalid(self):
        """Test invalid telemetry field"""
        field = TelemetryField(
            value=35.0,
            min_value=0.0,
            max_value=30.0,
            unit="V",
            description="Supply voltage"
        )
        assert field.valid == False
    
    def test_telemetry_field_edge_cases(self):
        """Test edge cases for telemetry field"""
        # Test minimum value
        field_min = TelemetryField(
            value=0.0,
            min_value=0.0,
            max_value=30.0,
            unit="V",
            description="Supply voltage"
        )
        assert field_min.valid == True
        
        # Test maximum value
        field_max = TelemetryField(
            value=30.0,
            min_value=0.0,
            max_value=30.0,
            unit="V",
            description="Supply voltage"
        )
        assert field_max.valid == True


class TestModuleTelemetry:
    """Test ModuleTelemetry model validation"""
    
    def test_module_telemetry_valid(self):
        """Test valid module telemetry"""
        telemetry = ModuleTelemetry(
            module_id=2,
            module_name="Power Module",
            telemetry={
                "voltage": TelemetryField(
                    value=24.1,
                    min_value=0.0,
                    max_value=30.0,
                    unit="V",
                    description="Supply voltage"
                ),
                "current": TelemetryField(
                    value=2.5,
                    min_value=0.0,
                    max_value=10.0,
                    unit="A",
                    description="Load current"
                )
            },
            timestamp=int(datetime.utcnow().timestamp())
        )
        assert telemetry.validation_status == "valid"
    
    def test_module_telemetry_invalid(self):
        """Test invalid module telemetry"""
        telemetry = ModuleTelemetry(
            module_id=2,
            module_name="Power Module",
            telemetry={
                "voltage": TelemetryField(
                    value=35.0,  # Invalid - out of range
                    min_value=0.0,
                    max_value=30.0,
                    unit="V",
                    description="Supply voltage"
                ),
                "current": TelemetryField(
                    value=2.5,
                    min_value=0.0,
                    max_value=10.0,
                    unit="A",
                    description="Load current"
                )
            },
            timestamp=int(datetime.utcnow().timestamp())
        )
        assert telemetry.validation_status == "invalid"


class TestValidationService:
    """Test ValidationService functionality"""
    
    @pytest.fixture
    def validation_service(self):
        """Create validation service instance"""
        return ValidationService()
    
    @pytest.fixture
    def sample_raw_data(self):
        """Sample raw telemetry data"""
        return {
            "module_id": 2,
            "module_name": "Power Module",
            "telemetry": {
                "voltage": 24.1,
                "current": 2.5,
                "temperature": 38.5
            }
        }
    
    @pytest.mark.asyncio
    async def test_enhance_telemetry_data_valid(self, validation_service, sample_raw_data):
        """Test enhancing valid telemetry data"""
        enhanced = await validation_service.enhance_telemetry_data(sample_raw_data)
        
        assert enhanced["module_id"] == 2
        assert enhanced["module_name"] == "Power Module"
        assert enhanced["validation_status"] == "valid"
        assert "telemetry" in enhanced
        
        # Check voltage field
        voltage = enhanced["telemetry"]["voltage"]
        assert voltage["value"] == 24.1
        assert voltage["valid"] == True
        assert voltage["unit"] == "V"
    
    @pytest.mark.asyncio
    async def test_enhance_telemetry_data_invalid(self, validation_service):
        """Test enhancing invalid telemetry data"""
        invalid_data = {
            "module_id": 2,
            "module_name": "Power Module",
            "telemetry": {
                "voltage": 35.0,  # Invalid - out of range
                "current": 2.5,
                "temperature": 38.5
            }
        }
        
        enhanced = await validation_service.enhance_telemetry_data(invalid_data)
        
        assert enhanced["validation_status"] == "invalid"
        assert "validation_errors" in enhanced
        assert len(enhanced["validation_errors"]) > 0
        
        # Check voltage field is invalid
        voltage = enhanced["telemetry"]["voltage"]
        assert voltage["value"] == 35.0
        assert voltage["valid"] == False
    
    @pytest.mark.asyncio
    async def test_validate_telemetry_data(self, validation_service):
        """Test validating telemetry data"""
        # Create valid telemetry data
        telemetry = ModuleTelemetry(
            module_id=2,
            module_name="Power Module",
            telemetry={
                "voltage": TelemetryField(
                    value=24.1,
                    min_value=0.0,
                    max_value=30.0,
                    unit="V",
                    description="Supply voltage"
                )
            },
            timestamp=int(datetime.utcnow().timestamp())
        )
        
        result = await validation_service.validate_telemetry_data(telemetry)
        
        assert result.valid == True
        assert len(result.errors) == 0
        assert result.validation_details["module_id"] == 2
        assert result.validation_details["valid_fields"] == 1
        assert result.validation_details["invalid_fields"] == 0
    
    @pytest.mark.asyncio
    async def test_get_field_config(self, validation_service):
        """Test getting field configuration"""
        config = await validation_service.get_field_config("voltage")
        
        assert config is not None
        assert config["min_value"] == 0.0
        assert config["max_value"] == 30.0
        assert config["unit"] == "V"
    
    @pytest.mark.asyncio
    async def test_update_field_config(self, validation_service):
        """Test updating field configuration"""
        new_config = {
            "min_value": 10.0,
            "max_value": 40.0,
            "unit": "V",
            "description": "Updated voltage range"
        }
        
        success = await validation_service.update_field_config("test_field", new_config)
        assert success == True
        
        # Verify update
        config = await validation_service.get_field_config("test_field")
        assert config["min_value"] == 10.0
        assert config["max_value"] == 40.0


class TestDatabaseService:
    """Test DatabaseService functionality"""
    
    @pytest.fixture
    def db_service(self):
        """Create database service instance with test database"""
        return DatabaseService("test_telemetry.db")
    
    @pytest.fixture
    def sample_telemetry_data(self):
        """Sample telemetry data for testing"""
        return ModuleTelemetry(
            module_id=2,
            module_name="Power Module",
            telemetry={
                "voltage": TelemetryField(
                    value=24.1,
                    min_value=0.0,
                    max_value=30.0,
                    unit="V",
                    description="Supply voltage"
                )
            },
            timestamp=int(datetime.utcnow().timestamp())
        )
    
    @pytest.mark.asyncio
    async def test_update_module_telemetry(self, db_service, sample_telemetry_data):
        """Test updating module telemetry data"""
        success = await db_service.update_module_telemetry(2, sample_telemetry_data)
        assert success == True
    
    @pytest.mark.asyncio
    async def test_get_module_telemetry(self, db_service, sample_telemetry_data):
        """Test getting module telemetry data"""
        # First update data
        await db_service.update_module_telemetry(2, sample_telemetry_data)
        
        # Then retrieve it
        data = await db_service.get_module_telemetry(2)
        
        assert data is not None
        assert data["validation_status"] == "valid"
        assert "telemetry_data" in data
    
    @pytest.mark.asyncio
    async def test_get_validation_status(self, db_service):
        """Test getting validation status"""
        status = await db_service.get_validation_status()
        
        assert "total_modules" in status
        assert "valid_modules" in status
        assert "invalid_modules" in status
        assert "validation_rate" in status
    
    @pytest.mark.asyncio
    async def test_cleanup_old_data(self, db_service):
        """Test cleaning up old data"""
        deleted_count = await db_service.cleanup_old_telemetry_data(days_to_keep=0)
        assert deleted_count >= 0


class TestIntegration:
    """Integration tests for the complete validation system"""
    
    @pytest.mark.asyncio
    async def test_end_to_end_validation(self):
        """Test complete validation flow"""
        # Create services
        validation_service = ValidationService()
        db_service = DatabaseService("test_integration.db")
        
        # Sample raw data
        raw_data = {
            "module_id": 3,
            "module_name": "Safety Module",
            "telemetry": {
                "voltage": 24.1,
                "current": 2.5,
                "temperature": 38.5
            }
        }
        
        # Step 1: Enhance data
        enhanced_data = await validation_service.enhance_telemetry_data(raw_data)
        assert enhanced_data["validation_status"] == "valid"
        
        # Step 2: Create ModuleTelemetry object
        telemetry = ModuleTelemetry(**enhanced_data)
        assert telemetry.validation_status == "valid"
        
        # Step 3: Validate data
        validation_result = await validation_service.validate_telemetry_data(telemetry)
        assert validation_result.valid == True
        
        # Step 4: Store in database
        success = await db_service.update_module_telemetry(3, telemetry)
        assert success == True
        
        # Step 5: Retrieve from database
        stored_data = await db_service.get_module_telemetry(3)
        assert stored_data is not None
        assert stored_data["validation_status"] == "valid"


if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v"])
