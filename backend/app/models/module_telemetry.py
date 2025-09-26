# -*- coding: utf-8 -*-
"""
Enhanced Module Telemetry Models with Value Range Validation
For Issue #144: Add Value Range Validation for Module Telemetry Data
"""

from pydantic import BaseModel, validator, Field
from typing import Dict, Any, Optional
from datetime import datetime


class TelemetryField(BaseModel):
    """Enhanced telemetry field with value range validation"""
    value: float = Field(..., description="Current field value")
    min_value: float = Field(..., description="Minimum allowed value")
    max_value: float = Field(..., description="Maximum allowed value")
    unit: str = Field(..., description="Value unit (e.g., 'V', 'A', '%', '°C')")
    description: str = Field(..., description="Field description")
    valid: bool = Field(default=True, description="Field validation status")
    
    @validator("value")
    def passthrough_value(cls, v):
        """Do not raise on out-of-range; validity is computed in `valid`."""
        return v
    
    @validator("valid", always=True)
    def validate_field_validity(cls, v, values):
        """Auto-calculate validity based on value range"""
        if "value" in values and "min_value" in values and "max_value" in values:
            return values["min_value"] <= values["value"] <= values["max_value"]
        return v


class ModuleTelemetry(BaseModel):
    """Enhanced module telemetry with validation"""
    module_id: int = Field(..., description="Module ID")
    module_name: str = Field(..., description="Module name")
    telemetry: Dict[str, TelemetryField] = Field(..., description="Telemetry fields with validation")
    timestamp: int = Field(..., description="Timestamp")
    validation_status: str = Field(default="valid", description="Overall validation status")
    
    @validator("validation_status", always=True)
    def validate_overall_status(cls, v, values):
        """Calculate overall validation status"""
        if "telemetry" in values:
            for field_name, field_data in values["telemetry"].items():
                if not field_data.valid:
                    return "invalid"
        return "valid"


class TelemetryValidationRequest(BaseModel):
    """Request for telemetry validation"""
    module_id: int = Field(..., description="Module ID to validate")
    telemetry_data: ModuleTelemetry = Field(..., description="Telemetry data to validate")


class TelemetryValidationResponse(BaseModel):
    """Response for telemetry validation"""
    valid: bool = Field(..., description="Validation result")
    errors: list = Field(default_factory=list, description="Validation errors")
    validation_details: Dict[str, Any] = Field(default_factory=dict, description="Detailed validation info")


class TelemetryUpdateRequest(BaseModel):
    """Request to update module telemetry"""
    module_id: int = Field(..., description="Module ID")
    telemetry_data: ModuleTelemetry = Field(..., description="Telemetry data to update")
    force: bool = Field(default=False, description="Force update even if validation fails")


class TelemetryUpdateResponse(BaseModel):
    """Response for telemetry update"""
    success: bool = Field(..., description="Update success")
    validation_status: str = Field(..., description="Validation status")
    data: Dict[str, Any] = Field(default_factory=dict, description="Update result data")
    message: str = Field(..., description="Response message")
    timestamp: datetime = Field(default_factory=datetime.utcnow)


# Predefined telemetry field configurations
TELEMETRY_FIELD_CONFIGS = {
    "voltage": {
        "min_value": 0.0,
        "max_value": 30.0,
        "unit": "V",
        "description": "Supply voltage"
    },
    "current": {
        "min_value": 0.0,
        "max_value": 10.0,
        "unit": "A", 
        "description": "Load current"
    },
    "power": {
        "min_value": 0.0,
        "max_value": 300.0,
        "unit": "W",
        "description": "Power consumption"
    },
    "temperature": {
        "min_value": -40.0,
        "max_value": 85.0,
        "unit": "°C",
        "description": "Operating temperature"
    },
    "battery_level": {
        "min_value": 0.0,
        "max_value": 100.0,
        "unit": "%",
        "description": "Battery level percentage"
    },
    "motor_speed": {
        "min_value": 0.0,
        "max_value": 5000.0,
        "unit": "RPM",
        "description": "Motor rotation speed"
    },
    "position": {
        "min_value": 0.0,
        "max_value": 1000.0,
        "unit": "mm",
        "description": "Position on rail"
    },
    "pressure": {
        "min_value": 0.0,
        "max_value": 10.0,
        "unit": "bar",
        "description": "Hydraulic pressure"
    }
}
