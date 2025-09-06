"""
Integration tests for Sensor API endpoints
"""

import pytest
from fastapi.testclient import TestClient
from unittest.mock import Mock, patch, AsyncMock
from datetime import datetime

from app.main import app
from app.models.user import User


class TestSensorAPI:
    """Integration tests for Sensor API endpoints"""
    
    @pytest.fixture
    def client(self):
        """Create test client"""
        return TestClient(app)
    
    @pytest.fixture
    def mock_user(self):
        """Mock authenticated user"""
        user = Mock(spec=User)
        user.id = 1
        user.username = "test_user"
        user.email = "test@example.com"
        user.role = "user"
        user.is_active = True
        return user
    
    @pytest.fixture
    def auth_headers(self):
        """Mock authentication headers"""
        return {"Authorization": "Bearer mock_token"}
    
    def test_submit_sensor_data_success(self, client, mock_user, auth_headers):
        """Test successful sensor data submission"""
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            mock_db_session.add = Mock()
            mock_db_session.commit = Mock()
            mock_db_session.refresh = Mock()
            
            # Test request
            response = client.post(
                "/api/v1/sensors/data",
                json={
                    "sensor_type": "rfid",
                    "sensor_id": "RFID_001",
                    "data": {
                        "rfid_id": "TAG_001",
                        "signal_strength": 0.8
                    },
                    "quality": 0.9
                },
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["sensor_id"] == "RFID_001"
            assert data["message"] == "Sensor data submitted successfully"
    
    def test_submit_sensor_data_validation_error(self, client, mock_user, auth_headers):
        """Test sensor data submission with validation error"""
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user):
            # Test request with invalid data
            response = client.post(
                "/api/v1/sensors/data",
                json={
                    "sensor_type": "",  # Invalid empty type
                    "sensor_id": "",    # Invalid empty ID
                    "data": {}          # Empty data
                },
                headers=auth_headers
            )
            
            # Should return validation error
            assert response.status_code == 422
    
    def test_get_sensor_status_success(self, client, mock_user, auth_headers):
        """Test successful sensor status retrieval"""
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            
            # Mock sensor status
            mock_status = Mock()
            mock_status.sensor_id = "RFID_001"
            mock_status.sensor_type = "rfid"
            mock_status.status = "online"
            mock_status.last_reading_time = datetime(2025, 1, 28, 10, 30, 0)
            mock_status.error_count = 0
            mock_status.health_score = 1.0
            mock_status.updated_at = datetime(2025, 1, 28, 10, 30, 0)
            
            mock_db_session.query.return_value.filter.return_value.first.return_value = mock_status
            
            # Test request
            response = client.get(
                "/api/v1/sensors/status/RFID_001",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["sensor_id"] == "RFID_001"
            assert data["sensor_type"] == "rfid"
            assert data["status"] == "online"
            assert data["error_count"] == 0
            assert data["health_score"] == 1.0
    
    def test_get_sensor_status_not_found(self, client, mock_user, auth_headers):
        """Test sensor status retrieval when sensor not found"""
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            mock_db_session.query.return_value.filter.return_value.first.return_value = None
            
            # Test request
            response = client.get(
                "/api/v1/sensors/status/NONEXISTENT",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 404
            data = response.json()
            assert data["detail"] == "Sensor NONEXISTENT not found"
    
    def test_get_all_sensor_status(self, client, mock_user, auth_headers):
        """Test get all sensor statuses"""
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            
            # Mock sensor statuses
            mock_status1 = Mock()
            mock_status1.sensor_id = "RFID_001"
            mock_status1.sensor_type = "rfid"
            mock_status1.status = "online"
            mock_status1.last_reading_time = None
            mock_status1.error_count = 0
            mock_status1.health_score = 1.0
            mock_status1.updated_at = datetime(2025, 1, 28, 10, 30, 0)
            
            mock_status2 = Mock()
            mock_status2.sensor_id = "ACCEL_001"
            mock_status2.sensor_type = "accelerometer"
            mock_status2.status = "online"
            mock_status2.last_reading_time = None
            mock_status2.error_count = 0
            mock_status2.health_score = 0.9
            mock_status2.updated_at = datetime(2025, 1, 28, 10, 30, 0)
            
            mock_db_session.query.return_value.all.return_value = [mock_status1, mock_status2]
            
            # Test request
            response = client.get(
                "/api/v1/sensors/status",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert len(data) == 2
            assert data[0]["sensor_id"] == "RFID_001"
            assert data[1]["sensor_id"] == "ACCEL_001"
    
    def test_configure_sensor_success(self, client, mock_user, auth_headers):
        """Test successful sensor configuration"""
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            mock_db_session.query.return_value.filter.return_value.first.return_value = None
            mock_db_session.add = Mock()
            mock_db_session.commit = Mock()
            
            # Test request
            response = client.post(
                "/api/v1/sensors/configure",
                json={
                    "sensor_id": "RFID_001",
                    "sensor_type": "rfid",
                    "name": "RFID Reader 1",
                    "description": "Main RFID reader",
                    "configuration": {
                        "baud_rate": 9600,
                        "timeout": 1000
                    },
                    "calibration_data": {
                        "offset_x": 0.0,
                        "offset_y": 0.0
                    }
                },
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["sensor_id"] == "RFID_001"
            assert data["message"] == "Sensor configured successfully"
    
    def test_get_sensor_configuration_success(self, client, mock_user, auth_headers):
        """Test successful sensor configuration retrieval"""
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            
            # Mock sensor configuration
            mock_config = Mock()
            mock_config.sensor_id = "RFID_001"
            mock_config.sensor_type = "rfid"
            mock_config.name = "RFID Reader 1"
            mock_config.description = "Main RFID reader"
            mock_config.configuration = {"baud_rate": 9600, "timeout": 1000}
            mock_config.calibration_data = {"offset_x": 0.0, "offset_y": 0.0}
            mock_config.is_active = True
            mock_config.created_at = datetime(2025, 1, 1)
            mock_config.updated_at = datetime(2025, 1, 1)
            
            mock_db_session.query.return_value.filter.return_value.first.return_value = mock_config
            
            # Test request
            response = client.get(
                "/api/v1/sensors/configure/RFID_001",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["data"]["sensor_id"] == "RFID_001"
            assert data["data"]["name"] == "RFID Reader 1"
            assert data["data"]["configuration"]["baud_rate"] == 9600
    
    def test_get_sensor_configuration_not_found(self, client, mock_user, auth_headers):
        """Test sensor configuration retrieval when not found"""
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            mock_db_session.query.return_value.filter.return_value.first.return_value = None
            
            # Test request
            response = client.get(
                "/api/v1/sensors/configure/NONEXISTENT",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 404
            data = response.json()
            assert data["detail"] == "Sensor configuration NONEXISTENT not found"
    
    def test_get_sensor_list_success(self, client, mock_user, auth_headers):
        """Test successful sensor list retrieval"""
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            
            # Mock sensor configurations
            mock_config1 = Mock()
            mock_config1.sensor_id = "RFID_001"
            mock_config1.sensor_type = "rfid"
            mock_config1.name = "RFID Reader 1"
            mock_config1.description = "Main RFID reader"
            mock_config1.is_active = True
            mock_config1.created_at = datetime(2025, 1, 1)
            mock_config1.updated_at = datetime(2025, 1, 1)
            
            mock_config2 = Mock()
            mock_config2.sensor_id = "ACCEL_001"
            mock_config2.sensor_type = "accelerometer"
            mock_config2.name = "Accelerometer 1"
            mock_config2.description = "Main accelerometer"
            mock_config2.is_active = True
            mock_config2.created_at = datetime(2025, 1, 1)
            mock_config2.updated_at = datetime(2025, 1, 1)
            
            # Mock sensor statuses
            mock_status1 = Mock()
            mock_status1.status = "online"
            mock_status1.health_score = 1.0
            mock_status1.last_reading_time = None
            
            mock_status2 = Mock()
            mock_status2.status = "online"
            mock_status2.health_score = 0.9
            mock_status2.last_reading_time = None
            
            # Setup query chain
            mock_db_session.query.return_value.filter.return_value.all.return_value = [mock_config1, mock_config2]
            mock_db_session.query.return_value.filter.return_value.first.side_effect = [mock_status1, mock_status2]
            
            # Test request
            response = client.get(
                "/api/v1/sensors/list",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["total_count"] == 2
            assert len(data["sensors"]) == 2
            assert data["sensors"][0]["sensor_id"] == "RFID_001"
            assert data["sensors"][1]["sensor_id"] == "ACCEL_001"
    
    def test_get_sensor_data_success(self, client, mock_user, auth_headers):
        """Test successful sensor data retrieval"""
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            
            # Mock sensor data
            mock_data1 = Mock()
            mock_data1.id = 1
            mock_data1.sensor_type = "rfid"
            mock_data1.sensor_id = "RFID_001"
            mock_data1.data = {"rfid_id": "TAG_001", "signal_strength": 0.8}
            mock_data1.quality = 0.9
            mock_data1.timestamp = datetime(2025, 1, 28, 10, 30, 0)
            
            mock_data2 = Mock()
            mock_data2.id = 2
            mock_data2.sensor_type = "rfid"
            mock_data2.sensor_id = "RFID_001"
            mock_data2.data = {"rfid_id": "TAG_002", "signal_strength": 0.7}
            mock_data2.quality = 0.8
            mock_data2.timestamp = datetime(2025, 1, 28, 10, 29, 0)
            
            # Setup query chain
            mock_db_session.query.return_value.filter.return_value.order_by.return_value.limit.return_value.all.return_value = [mock_data1, mock_data2]
            
            # Test request
            response = client.get(
                "/api/v1/sensors/data/RFID_001?limit=100",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["total_count"] == 2
            assert data["sensor_id"] == "RFID_001"
            assert len(data["data"]) == 2
            assert data["data"][0]["sensor_type"] == "rfid"
            assert data["data"][0]["quality"] == 0.9
    
    def test_calibrate_sensor_success(self, client, mock_user, auth_headers):
        """Test successful sensor calibration"""
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            
            # Mock sensor configuration
            mock_config = Mock()
            mock_config.sensor_id = "RFID_001"
            mock_config.calibration_data = None
            mock_config.last_calibration = None
            mock_config.updated_at = None
            
            mock_db_session.query.return_value.filter.return_value.first.return_value = mock_config
            mock_db_session.commit = Mock()
            
            # Test request
            response = client.post(
                "/api/v1/sensors/calibrate/RFID_001",
                json={
                    "offset_x": 0.1,
                    "offset_y": 0.2,
                    "scale_factor": 1.0
                },
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["message"] == "Sensor RFID_001 calibrated successfully"
            assert data["calibration_data"]["offset_x"] == 0.1
            assert data["calibration_data"]["offset_y"] == 0.2
    
    def test_calibrate_sensor_not_found(self, client, mock_user, auth_headers):
        """Test sensor calibration when sensor not found"""
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            mock_db_session.query.return_value.filter.return_value.first.return_value = None
            
            # Test request
            response = client.post(
                "/api/v1/sensors/calibrate/NONEXISTENT",
                json={"offset_x": 0.1, "offset_y": 0.2},
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 404
            data = response.json()
            assert data["detail"] == "Sensor NONEXISTENT not found"
    
    def test_unauthorized_access(self, client):
        """Test API access without authentication"""
        # Test request without auth headers
        response = client.post(
            "/api/v1/sensors/data",
            json={
                "sensor_type": "rfid",
                "sensor_id": "RFID_001",
                "data": {"test": "data"}
            }
        )
        
        # Should return 401 Unauthorized
        assert response.status_code == 401
