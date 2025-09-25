# -*- coding: utf-8 -*-
"""
Validation Service for Module Telemetry Data
For Issue #144: Add Value Range Validation for Module Telemetry Data
"""

import logging
from typing import Dict, Any, List, Optional
from datetime import datetime

from app.models.module_telemetry import (
    ModuleTelemetry, TelemetryField, TELEMETRY_FIELD_CONFIGS,
    TelemetryValidationResponse
)

logger = logging.getLogger(__name__)


class ValidationService:
    """Service for validating module telemetry data with value ranges"""
    
    def __init__(self):
        """Initialize validation service"""
        self.field_configs = TELEMETRY_FIELD_CONFIGS
        logger.info("✅ ValidationService initialized with field configurations")
    
    async def enhance_telemetry_data(self, raw_data: Dict[str, Any]) -> Dict[str, Any]:
        """
        Enhance raw telemetry data with validation information
        
        Args:
            raw_data: Raw telemetry data from firmware
            
        Returns:
            Enhanced telemetry data with validation info
        """
        try:
            logger.info(f"🔍 Enhancing telemetry data for module {raw_data.get('module_id', 'unknown')}")
            
            enhanced_data = {
                "module_id": raw_data.get("module_id"),
                "module_name": raw_data.get("module_name", "Unknown Module"),
                "telemetry": {},
                "timestamp": int(datetime.utcnow().timestamp()),
                "validation_status": "valid"
            }
            
            # Process each telemetry field
            raw_telemetry = raw_data.get("telemetry", {})
            validation_errors = []
            
            for field_name, field_value in raw_telemetry.items():
                enhanced_field = await self._enhance_field(field_name, field_value)
                enhanced_data["telemetry"][field_name] = enhanced_field
                
                # Check for validation errors
                if not enhanced_field.get("valid", True):
                    validation_errors.append({
                        "field": field_name,
                        "value": field_value,
                        "range": {
                            "min": enhanced_field.get("min_value"),
                            "max": enhanced_field.get("max_value")
                        },
                        "error": f"Value {field_value} is out of range [{enhanced_field.get('min_value')}, {enhanced_field.get('max_value')}]"
                    })
            
            # Update overall validation status
            if validation_errors:
                enhanced_data["validation_status"] = "invalid"
                enhanced_data["validation_errors"] = validation_errors
                logger.warning(f"⚠️ Telemetry validation failed with {len(validation_errors)} errors")
            else:
                logger.info("✅ Telemetry validation passed")
            
            return enhanced_data
            
        except Exception as e:
            logger.error(f"❌ Failed to enhance telemetry data: {e}")
            raise
    
    async def _enhance_field(self, field_name: str, field_value: Any) -> Dict[str, Any]:
        """
        Enhance individual telemetry field with validation info
        
        Args:
            field_name: Name of the telemetry field
            field_value: Raw field value
            
        Returns:
            Enhanced field data with validation
        """
        try:
            # Get field configuration
            config = self.field_configs.get(field_name, {
                "min_value": 0.0,
                "max_value": 1000.0,
                "unit": "unit",
                "description": f"{field_name} value"
            })
            
            # Convert value to float if possible
            try:
                value = float(field_value)
            except (ValueError, TypeError):
                logger.warning(f"⚠️ Cannot convert {field_name} value {field_value} to float")
                value = 0.0
            
            # Check if value is within range
            min_val = config["min_value"]
            max_val = config["max_value"]
            is_valid = min_val <= value <= max_val
            
            enhanced_field = {
                "value": value,
                "min_value": min_val,
                "max_value": max_val,
                "unit": config["unit"],
                "description": config["description"],
                "valid": is_valid
            }
            
            if not is_valid:
                logger.warning(f"⚠️ Field {field_name} value {value} is out of range [{min_val}, {max_val}]")
            
            return enhanced_field
            
        except Exception as e:
            logger.error(f"❌ Failed to enhance field {field_name}: {e}")
            return {
                "value": 0.0,
                "min_value": 0.0,
                "max_value": 1000.0,
                "unit": "unit",
                "description": f"{field_name} value",
                "valid": False
            }
    
    async def validate_telemetry_data(self, telemetry_data: ModuleTelemetry) -> TelemetryValidationResponse:
        """
        Validate complete telemetry data
        
        Args:
            telemetry_data: ModuleTelemetry object to validate
            
        Returns:
            TelemetryValidationResponse with validation results
        """
        try:
            logger.info(f"🔍 Validating telemetry data for module {telemetry_data.module_id}")
            
            validation_errors = []
            validation_details = {
                "module_id": telemetry_data.module_id,
                "module_name": telemetry_data.module_name,
                "total_fields": len(telemetry_data.telemetry),
                "valid_fields": 0,
                "invalid_fields": 0,
                "field_details": {}
            }
            
            # Validate each field
            for field_name, field_data in telemetry_data.telemetry.items():
                field_valid = field_data.valid
                
                if field_valid:
                    validation_details["valid_fields"] += 1
                else:
                    validation_details["invalid_fields"] += 1
                    validation_errors.append({
                        "field": field_name,
                        "value": field_data.value,
                        "range": {"min": field_data.min_value, "max": field_data.max_value},
                        "error": f"Value {field_data.value} is out of range [{field_data.min_value}, {field_data.max_value}]"
                    })
                
                validation_details["field_details"][field_name] = {
                    "value": field_data.value,
                    "valid": field_valid,
                    "unit": field_data.unit
                }
            
            is_valid = len(validation_errors) == 0
            
            if is_valid:
                logger.info("✅ Telemetry validation passed")
            else:
                logger.warning(f"⚠️ Telemetry validation failed with {len(validation_errors)} errors")
            
            return TelemetryValidationResponse(
                valid=is_valid,
                errors=validation_errors,
                validation_details=validation_details
            )
            
        except Exception as e:
            logger.error(f"❌ Validation error: {e}")
            return TelemetryValidationResponse(
                valid=False,
                errors=[{"error": str(e)}],
                validation_details={}
            )
    
    async def get_field_config(self, field_name: str) -> Optional[Dict[str, Any]]:
        """
        Get configuration for a specific telemetry field
        
        Args:
            field_name: Name of the telemetry field
            
        Returns:
            Field configuration or None if not found
        """
        return self.field_configs.get(field_name)
    
    async def get_all_field_configs(self) -> Dict[str, Dict[str, Any]]:
        """
        Get all available field configurations
        
        Returns:
            Dictionary of all field configurations
        """
        return self.field_configs.copy()
    
    async def update_field_config(self, field_name: str, config: Dict[str, Any]) -> bool:
        """
        Update configuration for a specific telemetry field
        
        Args:
            field_name: Name of the telemetry field
            config: New field configuration
            
        Returns:
            True if update successful, False otherwise
        """
        try:
            # Validate config
            required_keys = ["min_value", "max_value", "unit", "description"]
            if not all(key in config for key in required_keys):
                logger.error(f"❌ Invalid config for field {field_name}: missing required keys")
                return False
            
            # Update config
            self.field_configs[field_name] = config
            logger.info(f"✅ Updated config for field {field_name}")
            return True
            
        except Exception as e:
            logger.error(f"❌ Failed to update field config for {field_name}: {e}")
            return False


# Global validation service instance
validation_service = ValidationService()
