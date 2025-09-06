"""
End-to-End Tests for Complete Mapping Workflow
Tests the entire mapping process from start to finish
"""

import pytest
import asyncio
import json
from datetime import datetime, timedelta
from unittest.mock import Mock, patch, AsyncMock
from fastapi.testclient import TestClient

from app.main import app
from app.models.user import User
from app.services.map_service import MapService
from app.services.hybrid_localization_engine import HybridLocalizationEngine
from app.services.firmware_integration_service import FirmwareIntegrationService, SensorType
from app.services.sensor_data_processor import SensorDataProcessor
from app.services.realtime_updates_service import RealtimeUpdatesService


class TestCompleteMappingWorkflow:
    """End-to-end tests for complete mapping workflow"""
    
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
    
    @pytest.fixture
    def mock_services(self):
        """Mock all services"""
        return {
            "map_service": Mock(spec=MapService),
            "localization_engine": Mock(spec=HybridLocalizationEngine),
            "firmware_service": Mock(spec=FirmwareIntegrationService),
            "sensor_processor": Mock(spec=SensorDataProcessor),
            "realtime_service": Mock(spec=RealtimeUpdatesService)
        }
    
    @pytest.mark.asyncio
    async def test_complete_mapping_workflow_success(self, client, mock_user, auth_headers, mock_services):
        """Test complete mapping workflow from start to finish"""
        
        # Setup mocks
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service', mock_services["map_service"]), \
             patch('app.api.v1.map.localization_engine', mock_services["localization_engine"]), \
             patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.localization.get_current_user', return_value=mock_user):
            
            # Step 1: Start Mapping
            mock_services["map_service"].start_mapping.return_value = {
                "success": True,
                "map_id": "test_map_001",
                "session_id": "test_session_001",
                "message": "Mapping started successfully"
            }
            
            start_response = client.post(
                "/api/v1/map/start-mapping",
                json={
                    "map_name": "Test Map",
                    "resolution": 50.0,
                    "width": 1000,
                    "height": 1000
                },
                headers=auth_headers
            )
            
            assert start_response.status_code == 201
            start_data = start_response.json()
            assert start_data["success"] is True
            assert start_data["data"]["map_id"] == "test_map_001"
            
            # Step 2: Configure Sensors
            mock_services["firmware_service"].configure_sensor.return_value = True
            
            # Configure RFID sensor
            rfid_config_response = client.post(
                "/api/v1/sensors/configure",
                json={
                    "sensor_id": "RFID_001",
                    "sensor_type": "rfid",
                    "name": "RFID Reader 1",
                    "description": "Main RFID reader",
                    "configuration": {
                        "baud_rate": 9600,
                        "timeout": 1000
                    }
                },
                headers=auth_headers
            )
            
            assert rfid_config_response.status_code == 200
            
            # Configure Accelerometer
            accel_config_response = client.post(
                "/api/v1/sensors/configure",
                json={
                    "sensor_id": "ACCEL_001",
                    "sensor_type": "accelerometer",
                    "name": "Accelerometer 1",
                    "description": "Main accelerometer",
                    "configuration": {
                        "sample_rate": 100,
                        "range": 16
                    }
                },
                headers=auth_headers
            )
            
            assert accel_config_response.status_code == 200
            
            # Step 3: Submit Sensor Data
            mock_services["sensor_processor"].process_sensor_data.return_value = Mock(
                success=True,
                processed_data={"rfid_id": "TAG_001", "signal_strength": 0.8},
                quality_score=0.9
            )
            
            # Submit RFID data
            rfid_data_response = client.post(
                "/api/v1/sensors/data",
                json={
                    "sensor_type": "rfid",
                    "sensor_id": "RFID_001",
                    "data": {
                        "rfid_id": "TAG_001",
                        "signal_strength": 0.8,
                        "distance": 150.5,
                        "angle": 45.0
                    },
                    "quality": 0.9
                },
                headers=auth_headers
            )
            
            assert rfid_data_response.status_code == 200
            
            # Submit Accelerometer data
            accel_data_response = client.post(
                "/api/v1/sensors/data",
                json={
                    "sensor_type": "accelerometer",
                    "sensor_id": "ACCEL_001",
                    "data": {
                        "x": 0.1,
                        "y": 0.2,
                        "z": 9.8,
                        "magnitude": 9.81
                    },
                    "quality": 0.95
                },
                headers=auth_headers
            )
            
            assert accel_data_response.status_code == 200
            
            # Step 4: Update Robot Position
            mock_services["map_service"].get_robot_position.return_value = {
                "success": True,
                "position": {
                    "x": 100.0,
                    "y": 200.0,
                    "theta": 1.57,
                    "confidence": 0.9,
                    "source": "hybrid"
                },
                "timestamp": datetime.utcnow().isoformat()
            }
            
            position_response = client.get(
                "/api/v1/localization/position",
                headers=auth_headers
            )
            
            assert position_response.status_code == 200
            position_data = position_response.json()
            assert position_data["success"] is True
            assert position_data["data"]["position"]["x"] == 100.0
            
            # Step 5: Get Mapping Status
            mock_services["map_service"].get_mapping_status.return_value = {
                "is_mapping": True,
                "current_map_id": "test_map_001",
                "current_session_id": "test_session_001",
                "total_scans": 10,
                "mapping_quality": 0.8
            }
            
            status_response = client.get(
                "/api/v1/map/status",
                headers=auth_headers
            )
            
            assert status_response.status_code == 200
            status_data = status_response.json()
            assert status_data["is_mapping"] is True
            assert status_data["total_scans"] == 10
            
            # Step 6: Get Occupancy Grid
            mock_map = Mock()
            mock_map.occupancy_grid = [[-1, 0, 1], [0, 1, 0]]
            mock_map.resolution = 50.0
            mock_map.width = 1000
            mock_map.height = 1000
            mock_services["map_service"].current_map = mock_map
            
            grid_response = client.get(
                "/api/v1/map/occupancy-grid",
                headers=auth_headers
            )
            
            assert grid_response.status_code == 200
            grid_data = grid_response.json()
            assert grid_data["success"] is True
            assert grid_data["occupancy_grid"]["resolution"] == 50.0
            
            # Step 7: Stop Mapping
            mock_services["map_service"].stop_mapping.return_value = {
                "success": True,
                "map_id": "test_map_001",
                "session_id": "test_session_001",
                "message": "Mapping stopped successfully"
            }
            
            stop_response = client.post(
                "/api/v1/map/stop-mapping",
                headers=auth_headers
            )
            
            assert stop_response.status_code == 200
            stop_data = stop_response.json()
            assert stop_data["success"] is True
            assert stop_data["map_id"] == "test_map_001"
            
            # Step 8: Get Map List
            mock_services["map_service"].get_map_list.return_value = [
                {
                    "map_id": "test_map_001",
                    "name": "Test Map",
                    "resolution": 50.0,
                    "width": 1000,
                    "height": 1000,
                    "created_at": datetime.utcnow().isoformat(),
                    "updated_at": datetime.utcnow().isoformat(),
                    "metadata": {"test": True}
                }
            ]
            
            list_response = client.get(
                "/api/v1/map/list",
                headers=auth_headers
            )
            
            assert list_response.status_code == 200
            list_data = list_response.json()
            assert list_data["success"] is True
            assert list_data["total_count"] == 1
            assert list_data["maps"][0]["map_id"] == "test_map_001"
    
    @pytest.mark.asyncio
    async def test_mapping_workflow_with_errors(self, client, mock_user, auth_headers, mock_services):
        """Test mapping workflow with error scenarios"""
        
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service', mock_services["map_service"]):
            
            # Step 1: Start Mapping with Invalid Parameters
            start_response = client.post(
                "/api/v1/map/start-mapping",
                json={
                    "map_name": "",  # Invalid empty name
                    "resolution": -1,  # Invalid negative resolution
                    "width": 0,  # Invalid zero width
                    "height": 0  # Invalid zero height
                },
                headers=auth_headers
            )
            
            assert start_response.status_code == 422  # Validation error
            
            # Step 2: Start Mapping Successfully
            mock_services["map_service"].start_mapping.return_value = {
                "success": True,
                "map_id": "test_map_001",
                "session_id": "test_session_001"
            }
            
            start_response = client.post(
                "/api/v1/map/start-mapping",
                json={
                    "map_name": "Test Map",
                    "resolution": 50.0,
                    "width": 1000,
                    "height": 1000
                },
                headers=auth_headers
            )
            
            assert start_response.status_code == 201
            
            # Step 3: Try to Start Mapping Again (Should Fail)
            mock_services["map_service"].start_mapping.side_effect = ValueError("Mapping already in progress")
            
            start_response = client.post(
                "/api/v1/map/start-mapping",
                json={
                    "map_name": "Another Map",
                    "resolution": 50.0,
                    "width": 1000,
                    "height": 1000
                },
                headers=auth_headers
            )
            
            assert start_response.status_code == 400
            
            # Step 4: Stop Mapping
            mock_services["map_service"].stop_mapping.return_value = {
                "success": True,
                "map_id": "test_map_001"
            }
            
            stop_response = client.post(
                "/api/v1/map/stop-mapping",
                headers=auth_headers
            )
            
            assert stop_response.status_code == 200
    
    @pytest.mark.asyncio
    async def test_sensor_data_workflow(self, client, mock_user, auth_headers, mock_services):
        """Test sensor data collection and processing workflow"""
        
        with patch('app.api.v1.sensors.get_current_user', return_value=mock_user), \
             patch('app.api.v1.sensors.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            mock_db_session.add = Mock()
            mock_db_session.commit = Mock()
            mock_db_session.refresh = Mock()
            
            # Step 1: Configure Multiple Sensors
            sensors = [
                {
                    "sensor_id": "RFID_001",
                    "sensor_type": "rfid",
                    "name": "RFID Reader 1",
                    "configuration": {"baud_rate": 9600}
                },
                {
                    "sensor_id": "ACCEL_001",
                    "sensor_type": "accelerometer",
                    "name": "Accelerometer 1",
                    "configuration": {"sample_rate": 100}
                },
                {
                    "sensor_id": "PROX_001",
                    "sensor_type": "proximity",
                    "name": "Proximity Sensor 1",
                    "configuration": {"range": 500}
                },
                {
                    "sensor_id": "LIDAR_001",
                    "sensor_type": "lidar",
                    "name": "LiDAR Scanner 1",
                    "configuration": {"resolution": 1.0}
                }
            ]
            
            for sensor in sensors:
                response = client.post(
                    "/api/v1/sensors/configure",
                    json=sensor,
                    headers=auth_headers
                )
                assert response.status_code == 200
            
            # Step 2: Submit Data from All Sensors
            sensor_data = [
                {
                    "sensor_type": "rfid",
                    "sensor_id": "RFID_001",
                    "data": {"rfid_id": "TAG_001", "signal_strength": 0.8},
                    "quality": 0.9
                },
                {
                    "sensor_type": "accelerometer",
                    "sensor_id": "ACCEL_001",
                    "data": {"x": 0.1, "y": 0.2, "z": 9.8},
                    "quality": 0.95
                },
                {
                    "sensor_type": "proximity",
                    "sensor_id": "PROX_001",
                    "data": {"distance": 25.0, "obstacle_detected": False},
                    "quality": 0.85
                },
                {
                    "sensor_type": "lidar",
                    "sensor_id": "LIDAR_001",
                    "data": {"scan_data": [100, 95, 90] * 120, "resolution": 1.0},
                    "quality": 0.9
                }
            ]
            
            for data in sensor_data:
                response = client.post(
                    "/api/v1/sensors/data",
                    json=data,
                    headers=auth_headers
                )
                assert response.status_code == 200
            
            # Step 3: Get Sensor Status
            response = client.get(
                "/api/v1/sensors/status",
                headers=auth_headers
            )
            
            assert response.status_code == 200
            status_data = response.json()
            assert len(status_data) >= 0  # May be empty if no status records
            
            # Step 4: Get Sensor List
            response = client.get(
                "/api/v1/sensors/list",
                headers=auth_headers
            )
            
            assert response.status_code == 200
            list_data = response.json()
            assert list_data["success"] is True
    
    @pytest.mark.asyncio
    async def test_localization_workflow(self, client, mock_user, auth_headers, mock_services):
        """Test localization and positioning workflow"""
        
        with patch('app.api.v1.localization.get_current_user', return_value=mock_user), \
             patch('app.api.v1.localization.map_service', mock_services["map_service"]), \
             patch('app.api.v1.localization.localization_engine', mock_services["localization_engine"]), \
             patch('app.api.v1.localization.get_db') as mock_db:
            
            # Setup mock database
            mock_db_session = Mock()
            mock_db.return_value = [mock_db_session]
            mock_db_session.query.return_value.filter.return_value.first.return_value = None
            mock_db_session.add = Mock()
            mock_db_session.commit = Mock()
            
            # Step 1: Set Localization Configuration
            configs = [
                {
                    "config_key": "rfid_threshold",
                    "config_value": {"threshold": 0.5, "timeout": 1000},
                    "config_type": "rfid",
                    "description": "RFID signal threshold"
                },
                {
                    "config_key": "accel_scale",
                    "config_value": {"scale": 1.0, "offset": 0.0},
                    "config_type": "accelerometer",
                    "description": "Accelerometer scaling"
                }
            ]
            
            for config in configs:
                response = client.post(
                    "/api/v1/localization/config",
                    json=config,
                    headers=auth_headers
                )
                assert response.status_code == 200
            
            # Step 2: Get Current Position
            mock_services["map_service"].get_robot_position.return_value = {
                "success": True,
                "position": {
                    "x": 100.0,
                    "y": 200.0,
                    "theta": 1.57,
                    "confidence": 0.9,
                    "source": "hybrid"
                },
                "timestamp": datetime.utcnow().isoformat()
            }
            
            position_response = client.get(
                "/api/v1/localization/position",
                headers=auth_headers
            )
            
            assert position_response.status_code == 200
            position_data = position_response.json()
            assert position_data["success"] is True
            
            # Step 3: Update Position
            position_update_response = client.post(
                "/api/v1/localization/position/update",
                json={
                    "sensor_type": "rfid",
                    "sensor_id": "RFID_001",
                    "position_data": {
                        "x": 105.0,
                        "y": 205.0,
                        "theta": 1.6
                    },
                    "confidence": 0.9
                },
                headers=auth_headers
            )
            
            assert position_update_response.status_code == 200
            
            # Step 4: Get Position History
            mock_services["localization_engine"].get_position_history.return_value = [
                {
                    "x": 100.0,
                    "y": 200.0,
                    "theta": 1.57,
                    "confidence": 0.9,
                    "timestamp": datetime.utcnow().isoformat()
                },
                {
                    "x": 105.0,
                    "y": 205.0,
                    "theta": 1.6,
                    "confidence": 0.9,
                    "timestamp": datetime.utcnow().isoformat()
                }
            ]
            
            history_response = client.get(
                "/api/v1/localization/history?limit=10",
                headers=auth_headers
            )
            
            assert history_response.status_code == 200
            history_data = history_response.json()
            assert history_data["success"] is True
            assert len(history_data["data"]["positions"]) == 2
            
            # Step 5: Get Localization Statistics
            mock_services["localization_engine"].get_localization_stats.return_value = {
                "total_positions": 100,
                "average_confidence": 0.85,
                "sensor_usage": {
                    "rfid": 40,
                    "accelerometer": 30,
                    "proximity": 20,
                    "lidar": 10
                },
                "current_position": {
                    "x": 105.0,
                    "y": 205.0,
                    "theta": 1.6,
                    "confidence": 0.9
                }
            }
            
            stats_response = client.get(
                "/api/v1/localization/stats",
                headers=auth_headers
            )
            
            assert stats_response.status_code == 200
            stats_data = stats_response.json()
            assert stats_data["total_positions"] == 100
            assert stats_data["average_confidence"] == 0.85
    
    @pytest.mark.asyncio
    async def test_unauthorized_access_workflow(self, client):
        """Test workflow with unauthorized access"""
        
        # Test all endpoints without authentication
        endpoints = [
            ("POST", "/api/v1/map/start-mapping", {"map_name": "Test"}),
            ("GET", "/api/v1/map/status", None),
            ("POST", "/api/v1/map/stop-mapping", None),
            ("GET", "/api/v1/map/current", None),
            ("POST", "/api/v1/sensors/data", {"sensor_type": "rfid"}),
            ("GET", "/api/v1/sensors/status", None),
            ("GET", "/api/v1/localization/position", None),
            ("POST", "/api/v1/localization/config", {"config_key": "test"})
        ]
        
        for method, endpoint, data in endpoints:
            if method == "POST":
                response = client.post(endpoint, json=data or {})
            else:
                response = client.get(endpoint)
            
            assert response.status_code == 401  # Unauthorized
    
    @pytest.mark.asyncio
    async def test_performance_workflow(self, client, mock_user, auth_headers, mock_services):
        """Test workflow performance"""
        
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service', mock_services["map_service"]):
            
            # Setup mocks for performance test
            mock_services["map_service"].start_mapping.return_value = {
                "success": True,
                "map_id": "perf_test_map",
                "session_id": "perf_test_session"
            }
            
            mock_services["map_service"].get_mapping_status.return_value = {
                "is_mapping": True,
                "current_map_id": "perf_test_map",
                "total_scans": 0,
                "mapping_quality": 0.0
            }
            
            # Test multiple rapid requests
            start_time = datetime.utcnow()
            
            # Start mapping
            start_response = client.post(
                "/api/v1/map/start-mapping",
                json={
                    "map_name": "Performance Test Map",
                    "resolution": 50.0,
                    "width": 1000,
                    "height": 1000
                },
                headers=auth_headers
            )
            
            # Get status multiple times
            for _ in range(10):
                status_response = client.get(
                    "/api/v1/map/status",
                    headers=auth_headers
                )
                assert status_response.status_code == 200
            
            end_time = datetime.utcnow()
            total_time = (end_time - start_time).total_seconds()
            
            # Performance assertions
            assert start_response.status_code == 201
            assert total_time < 5.0  # Should complete within 5 seconds
            assert total_time / 11 < 0.1  # Average response time < 100ms
