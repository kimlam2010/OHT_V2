"""
Pytest configuration và fixtures cho OHT-50 Backend
"""

from unittest.mock import Mock, patch

import pytest
from fastapi.testclient import TestClient

from app.config import Settings
from app.main import app


@pytest.fixture
def client():
    """Test client cho FastAPI app"""
    return TestClient(app)


@pytest.fixture
def mock_settings():
    """Mock settings cho testing"""
    return Settings(
        server_host="127.0.0.1",
        server_port=8000,
        debug=True,
        log_level="DEBUG",
        database_url="sqlite:///./test.db",
        api_key="test-api-key",
        cors_origins=["http://localhost:3000"],
        telemetry_buffer_size=1000,
        telemetry_flush_interval=5,
        center_url="http://localhost:9000",
        center_api_key="test-center-key",
        motor_max_speed=100.0,
        motor_acceleration=10.0,
        safety_zone_x_min=-1000.0,
        safety_zone_x_max=1000.0,
        safety_zone_y_min=-1000.0,
        safety_zone_y_max=1000.0,
    )


@pytest.fixture
def mock_logger():
    """Mock logger cho testing"""
    return Mock()


@pytest.fixture
def sample_telemetry_data():
    """Sample telemetry data cho testing"""
    return {
        "timestamp": "2024-01-01T00:00:00Z",
        "device_id": "OHT-50-001",
        "status": "idle",
        "position": {"x": 100.0, "y": 200.0, "z": 0.0, "theta": 0.0},
        "velocity": {"linear": 0.0, "angular": 0.0},
        "acceleration": {"linear": 0.0, "angular": 0.0},
        "battery_level": 85.5,
        "temperature": 25.0,
        "error_codes": [],
        "warnings": [],
    }


@pytest.fixture
def sample_config_data():
    """Sample configuration data cho testing"""
    return {
        "motor": {
            "max_speed": 100.0,
            "acceleration": 10.0,
            "deceleration": 10.0,
            "max_current": 5.0,
        },
        "encoder": {
            "resolution": 1000,
            "filter_type": "kalman",
            "calibration_offset": 0.0,
        },
        "safety": {
            "zones": [
                {
                    "name": "main_zone",
                    "x_min": -1000.0,
                    "x_max": 1000.0,
                    "y_min": -1000.0,
                    "y_max": 1000.0,
                    "enabled": True,
                }
            ],
            "emergency_stop_timeout": 1.0,
            "collision_detection": True,
        },
        "telemetry": {
            "buffer_size": 1000,
            "flush_interval": 5,
            "enabled_channels": ["position", "velocity", "battery"],
        },
        "communication": {
            "rs485": {
                "baud_rate": 115200,
                "data_bits": 8,
                "stop_bits": 1,
                "parity": "none",
            },
            "can": {"bitrate": 500000, "node_id": 1},
        },
    }


@pytest.fixture
def sample_mission_data():
    """Sample mission data cho testing"""
    return {
        "mission_id": "mission-001",
        "mission_type": "move_to",
        "target_position": {"x": 500.0, "y": 300.0, "z": 0.0, "theta": 0.0},
        "parameters": {"speed": 50.0, "precision": 10.0},
        "priority": 1,
        "timeout": 60,
    }
