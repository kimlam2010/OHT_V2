"""
Tests cho Config API endpoints
"""

import json
import pytest
from fastapi.testclient import TestClient
from unittest.mock import Mock, patch, MagicMock

from datetime import datetime
from app.models.config import SystemConfig, ConfigResponse, ConfigHistory


@pytest.fixture
def sample_config_data():
    """Sample configuration data cho testing"""
    return {
        "version": 1,
        "hardware": {
            "motor": {
                "interface": "rs485",
                "r": 0.05,
                "vmax_mms": 500.0,
                "amax_mms2": 300.0,
                "jmax_mms3": 5000.0
            },
            "encoder": {
                "type": "incremental",
                "resolution_ppr": 2048,
                "invert": False
            }
        },
        "safety": {
            "zones": [
                {"start_mm": 0, "end_mm": 200, "vmax_mms": 200}
            ],
            "estop_reset_policy": "manual"
        },
        "telemetry": {
            "ws_hz": 10
        },
        "comm": {
            "rs485": {
                "device": "/dev/ttyOHT485",
                "baud": 115200
            },
            "can": {
                "iface": "can0"
            }
        }
    }


@pytest.fixture
def sample_config(sample_config_data):
    """Sample SystemConfig object"""
    return SystemConfig(**sample_config_data)


def test_get_current_config_success(client: TestClient, sample_config):
    """Test lấy cấu hình hiện tại thành công"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        mock_service = MagicMock()
        mock_service.get_current_config.return_value = sample_config
        mock_get_service.return_value = mock_service
        
        response = client.get("/api/v1/config/")
        
        assert response.status_code == 200
        data = response.json()
        assert data["version"] == 1
        assert "hardware" in data
        assert "safety" in data
        assert "telemetry" in data
        assert "comm" in data


def test_get_current_config_not_found(client: TestClient):
    """Test lấy cấu hình khi không có config"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        from app.core.exceptions import ConfigurationError
        mock_service = MagicMock()
        mock_service.get_current_config.side_effect = ConfigurationError("No config available")
        mock_get_service.return_value = mock_service
        
        response = client.get("/api/v1/config/")
        
        assert response.status_code == 404
        assert "No config available" in response.json()["message"]


def test_get_config_history_success(client: TestClient):
    """Test lấy lịch sử cấu hình thành công"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        mock_service = MagicMock()
        history_entry = ConfigHistory(
            version=1,
            timestamp=datetime.now(),
            action="created",
            user="test_user",
            config_data={"version": 1, "hardware": {}}
        )
        mock_service.get_config_history.return_value = [history_entry]
        mock_get_service.return_value = mock_service
        
        response = client.get("/api/v1/config/history?limit=5")
        
        assert response.status_code == 200
        data = response.json()
        assert len(data) == 1
        assert data[0]["version"] == 1
        assert data[0]["action"] == "created"


def test_get_config_by_version_success(client: TestClient, sample_config):
    """Test lấy cấu hình theo version thành công"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        mock_service = MagicMock()
        mock_service.get_config_by_version.return_value = sample_config
        mock_get_service.return_value = mock_service
        
        response = client.get("/api/v1/config/version/1")
        
        assert response.status_code == 200
        data = response.json()
        assert data["version"] == 1


def test_get_config_by_version_not_found(client: TestClient):
    """Test lấy cấu hình theo version không tồn tại"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        mock_service = MagicMock()
        mock_service.get_config_by_version.return_value = None
        mock_get_service.return_value = mock_service
        
        response = client.get("/api/v1/config/version/999")
        
        assert response.status_code == 404
        assert "Configuration version 999 not found" in response.json()["message"]


def test_update_config_success(client: TestClient, sample_config_data):
    """Test cập nhật cấu hình thành công"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        mock_service = MagicMock()
        response_data = ConfigResponse(
            success=True,
            message="Configuration updated successfully",
            config=SystemConfig(**sample_config_data),
            version=2
        )
        mock_service.update_config.return_value = response_data
        mock_get_service.return_value = mock_service
        
        response = client.put("/api/v1/config/?user=test_user", json=sample_config_data)
        
        assert response.status_code == 200
        data = response.json()
        assert data["success"] is True
        assert data["version"] == 2


def test_update_config_validation_error(client: TestClient, sample_config_data):
    """Test cập nhật cấu hình với validation error"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        from app.core.exceptions import ValidationError
        mock_service = MagicMock()
        mock_service.update_config.side_effect = ValidationError("Invalid config")
        mock_get_service.return_value = mock_service
        
        # Tạo config không hợp lệ
        invalid_config = sample_config_data.copy()
        invalid_config["hardware"]["motor"]["vmax_mms"] = -1  # Giá trị âm
        
        response = client.put("/api/v1/config/?user=test_user", json=invalid_config)
        
        assert response.status_code == 400
        assert "Invalid config" in response.json()["message"]


def test_rollback_config_success(client: TestClient, sample_config_data):
    """Test rollback cấu hình thành công"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        mock_service = MagicMock()
        response_data = ConfigResponse(
            success=True,
            message="Configuration rolled back to version 1",
            config=SystemConfig(**sample_config_data),
            version=3
        )
        mock_service.rollback_config.return_value = response_data
        mock_get_service.return_value = mock_service
        
        response = client.post("/api/v1/config/rollback/1?user=test_user")
        
        assert response.status_code == 200
        data = response.json()
        assert data["success"] is True
        assert data["version"] == 3


def test_rollback_config_not_found(client: TestClient):
    """Test rollback cấu hình version không tồn tại"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        from app.core.exceptions import ConfigurationError
        mock_service = MagicMock()
        mock_service.rollback_config.side_effect = ConfigurationError("Version not found")
        mock_get_service.return_value = mock_service
        
        response = client.post("/api/v1/config/rollback/999?user=test_user")
        
        assert response.status_code == 404
        assert "Version not found" in response.json()["message"]


def test_export_config_json(client: TestClient):
    """Test export cấu hình dạng JSON"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        mock_service = MagicMock()
        mock_service.export_config.return_value = '{"version": 1, "hardware": {}}'
        mock_get_service.return_value = mock_service
        
        response = client.get("/api/v1/config/export?format=json")
        
        assert response.status_code == 200
        assert response.headers["content-type"] == "text/plain; charset=utf-8"
        data = json.loads(response.text)
        assert data["version"] == 1


def test_export_config_yaml(client: TestClient):
    """Test export cấu hình dạng YAML"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        mock_service = MagicMock()
        mock_service.export_config.return_value = "version: 1\nhardware: {}\n"
        mock_get_service.return_value = mock_service
        
        response = client.get("/api/v1/config/export?format=yaml")
        
        assert response.status_code == 200
        assert response.headers["content-type"] == "text/plain; charset=utf-8"
        assert "version: 1" in response.text


def test_import_config_success(client: TestClient, sample_config_data):
    """Test import cấu hình thành công"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        mock_service = MagicMock()
        response_data = ConfigResponse(
            success=True,
            message="Configuration imported successfully",
            config=SystemConfig(**sample_config_data),
            version=2
        )
        mock_service.import_config.return_value = response_data
        mock_get_service.return_value = mock_service
        
        config_content = json.dumps(sample_config_data)
        response = client.post(f"/api/v1/config/import?content={config_content}&format=json&user=test_user")
        
        assert response.status_code == 200
        data = response.json()
        assert data["success"] is True
        assert data["version"] == 2


def test_validate_config_valid(client: TestClient, sample_config_data):
    """Test validate cấu hình hợp lệ"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        mock_service = MagicMock()
        mock_service.validate_config_data.return_value = True
        mock_get_service.return_value = mock_service
        
        response = client.post("/api/v1/config/validate", json=sample_config_data)
        
        assert response.status_code == 200
        data = response.json()
        assert data["valid"] is True
        assert "valid" in data["message"]


def test_validate_config_invalid(client: TestClient, sample_config_data):
    """Test validate cấu hình không hợp lệ"""
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        mock_service = MagicMock()
        mock_service.validate_config_data.return_value = False
        mock_get_service.return_value = mock_service
        
        # Tạo config không hợp lệ
        invalid_config = sample_config_data.copy()
        invalid_config["hardware"]["motor"]["vmax_mms"] = -1
        
        response = client.post("/api/v1/config/validate", json=invalid_config)
        
        assert response.status_code == 200
        data = response.json()
        assert data["valid"] is False
        assert "failed" in data["message"]


def test_get_config_schema(client: TestClient):
    """Test lấy JSON schema"""
    response = client.get("/api/v1/config/schema")
    
    assert response.status_code == 200
    data = response.json()
    assert "properties" in data
    assert "SystemConfig" in data["title"]


def test_config_endpoints_integration(client: TestClient, sample_config_data):
    """Test tích hợp các endpoints config"""
    # Test tạo config mới
    with patch('app.api.v1.config.get_config_service') as mock_get_service:
        mock_service = MagicMock()
        # Mock cho get_current_config
        mock_service.get_current_config.return_value = SystemConfig(**sample_config_data)
        
        # Mock cho update_config
        response_data = ConfigResponse(
            success=True,
            message="Configuration updated successfully",
            config=SystemConfig(**sample_config_data),
            version=2
        )
        mock_service.update_config.return_value = response_data
        mock_get_service.return_value = mock_service
        
        # Test get current config
        response = client.get("/api/v1/config/")
        assert response.status_code == 200
        
        # Test update config
        response = client.put("/api/v1/config/?user=test_user", json=sample_config_data)
        assert response.status_code == 200
        assert response.json()["success"] is True
