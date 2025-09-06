"""
Integration tests for Map API endpoints
"""

import pytest
import asyncio
from fastapi.testclient import TestClient
from unittest.mock import Mock, patch, AsyncMock
from datetime import datetime

from app.main import app
from app.models.user import User


class TestMapAPI:
    """Integration tests for Map API endpoints"""
    
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
    
    def test_start_mapping_success(self, client, mock_user, auth_headers):
        """Test successful mapping start"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.start_mapping') as mock_start:
            
            # Setup mock
            mock_start.return_value = {
                "success": True,
                "map_id": "test_map_001",
                "session_id": "test_session_001",
                "message": "Mapping started successfully"
            }
            
            # Test request
            response = client.post(
                "/api/v1/map/start-mapping",
                json={
                    "map_name": "Test Map",
                    "resolution": 50.0,
                    "width": 1000,
                    "height": 1000
                },
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 201
            data = response.json()
            assert data["success"] is True
            assert data["data"]["map_id"] == "test_map_001"
            assert data["data"]["session_id"] == "test_session_001"
            assert data["message"] == "Mapping started successfully"
    
    def test_start_mapping_validation_error(self, client, mock_user, auth_headers):
        """Test mapping start with validation error"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user):
            # Test request with invalid data
            response = client.post(
                "/api/v1/map/start-mapping",
                json={
                    "map_name": "",  # Invalid empty name
                    "resolution": -1,  # Invalid negative resolution
                    "width": 0,  # Invalid zero width
                    "height": 0  # Invalid zero height
                },
                headers=auth_headers
            )
            
            # Should return validation error
            assert response.status_code == 422
    
    def test_stop_mapping_success(self, client, mock_user, auth_headers):
        """Test successful mapping stop"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.stop_mapping') as mock_stop:
            
            # Setup mock
            mock_stop.return_value = {
                "success": True,
                "map_id": "test_map_001",
                "session_id": "test_session_001",
                "message": "Mapping stopped successfully"
            }
            
            # Test request
            response = client.post(
                "/api/v1/map/stop-mapping",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["map_id"] == "test_map_001"
            assert data["session_id"] == "test_session_001"
            assert data["message"] == "Mapping stopped successfully"
    
    def test_get_mapping_status(self, client, mock_user, auth_headers):
        """Test get mapping status"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.get_mapping_status') as mock_status:
            
            # Setup mock
            mock_status.return_value = {
                "is_mapping": True,
                "current_map_id": "test_map_001",
                "current_session_id": "test_session_001",
                "total_scans": 10,
                "mapping_quality": 0.8
            }
            
            # Test request
            response = client.get(
                "/api/v1/map/status",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["is_mapping"] is True
            assert data["current_map_id"] == "test_map_001"
            assert data["current_session_id"] == "test_session_001"
            assert data["total_scans"] == 10
            assert data["mapping_quality"] == 0.8
    
    def test_get_current_map_success(self, client, mock_user, auth_headers):
        """Test get current map when map exists"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.current_map') as mock_map:
            
            # Setup mock map
            mock_map.map_id = "test_map_001"
            mock_map.name = "Test Map"
            mock_map.resolution = 50.0
            mock_map.width = 1000
            mock_map.height = 1000
            mock_map.map_metadata = {"test": True}
            mock_map.created_at = datetime(2025, 1, 1)
            mock_map.updated_at = datetime(2025, 1, 1)
            
            # Test request
            response = client.get(
                "/api/v1/map/current",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["data"]["map_id"] == "test_map_001"
            assert data["data"]["name"] == "Test Map"
            assert data["data"]["resolution"] == 50.0
    
    def test_get_current_map_not_found(self, client, mock_user, auth_headers):
        """Test get current map when no map exists"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.current_map', None):
            
            # Test request
            response = client.get(
                "/api/v1/map/current",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 404
            data = response.json()
            assert data["detail"] == "No active map found"
    
    def test_load_map_success(self, client, mock_user, auth_headers):
        """Test successful map loading"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.load_map') as mock_load:
            
            # Setup mock
            mock_load.return_value = {
                "success": True,
                "map": {
                    "map_id": "test_map_001",
                    "name": "Test Map",
                    "resolution": 50.0,
                    "width": 1000,
                    "height": 1000,
                    "metadata": {"test": True}
                },
                "message": "Map loaded successfully"
            }
            
            # Test request
            response = client.post(
                "/api/v1/map/load/test_map_001",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["data"]["map_id"] == "test_map_001"
            assert data["message"] == "Map loaded successfully"
    
    def test_load_map_not_found(self, client, mock_user, auth_headers):
        """Test map loading when map not found"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.load_map') as mock_load:
            
            # Setup mock to raise exception
            mock_load.side_effect = ValueError("Map test_map_001 not found")
            
            # Test request
            response = client.post(
                "/api/v1/map/load/test_map_001",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 404
            data = response.json()
            assert data["detail"] == "Map test_map_001 not found"
    
    def test_get_robot_position_success(self, client, mock_user, auth_headers):
        """Test successful robot position retrieval"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.get_robot_position') as mock_position:
            
            # Setup mock
            mock_position.return_value = {
                "success": True,
                "position": {
                    "x": 100.0,
                    "y": 200.0,
                    "theta": 1.57,
                    "confidence": 0.9,
                    "source": "hybrid"
                },
                "timestamp": "2025-01-28T10:30:00Z"
            }
            
            # Test request
            response = client.get(
                "/api/v1/map/robot-position",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["position"]["x"] == 100.0
            assert data["position"]["y"] == 200.0
            assert data["position"]["confidence"] == 0.9
    
    def test_get_robot_position_failed(self, client, mock_user, auth_headers):
        """Test robot position retrieval when failed"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.get_robot_position') as mock_position:
            
            # Setup mock
            mock_position.return_value = {
                "success": False,
                "message": "Unable to determine robot position"
            }
            
            # Test request
            response = client.get(
                "/api/v1/map/robot-position",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is False
            assert data["message"] == "Unable to determine robot position"
    
    def test_get_occupancy_grid_success(self, client, mock_user, auth_headers):
        """Test successful occupancy grid retrieval"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.current_map') as mock_map:
            
            # Setup mock map
            mock_map.occupancy_grid = [[-1, 0, 1], [0, 1, 0]]
            mock_map.resolution = 50.0
            mock_map.width = 1000
            mock_map.height = 1000
            
            # Test request
            response = client.get(
                "/api/v1/map/occupancy-grid",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["occupancy_grid"]["resolution"] == 50.0
            assert data["occupancy_grid"]["width"] == 1000
            assert data["occupancy_grid"]["height"] == 1000
    
    def test_get_occupancy_grid_no_map(self, client, mock_user, auth_headers):
        """Test occupancy grid retrieval when no map exists"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.current_map', None):
            
            # Test request
            response = client.get(
                "/api/v1/map/occupancy-grid",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 404
            data = response.json()
            assert data["detail"] == "No active map found"
    
    def test_get_robot_trajectory_success(self, client, mock_user, auth_headers):
        """Test successful robot trajectory retrieval"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.current_map') as mock_map:
            
            # Setup mock map
            mock_map.robot_trajectory = [
                {"x": 100, "y": 200, "timestamp": "2025-01-28T10:00:00Z"},
                {"x": 105, "y": 205, "timestamp": "2025-01-28T10:01:00Z"}
            ]
            mock_map.map_id = "test_map_001"
            
            # Test request
            response = client.get(
                "/api/v1/map/trajectory",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["data"]["map_id"] == "test_map_001"
            assert data["data"]["total_points"] == 2
    
    def test_get_map_list_success(self, client, mock_user, auth_headers):
        """Test successful map list retrieval"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.get_map_list') as mock_list:
            
            # Setup mock
            mock_list.return_value = [
                {
                    "map_id": "map_001",
                    "name": "Map 1",
                    "resolution": 50.0,
                    "width": 1000,
                    "height": 1000,
                    "created_at": "2025-01-01T10:00:00Z",
                    "updated_at": "2025-01-01T10:00:00Z",
                    "metadata": {}
                },
                {
                    "map_id": "map_002",
                    "name": "Map 2",
                    "resolution": 25.0,
                    "width": 2000,
                    "height": 2000,
                    "created_at": "2025-01-02T10:00:00Z",
                    "updated_at": "2025-01-02T10:00:00Z",
                    "metadata": {}
                }
            ]
            
            # Test request
            response = client.get(
                "/api/v1/map/list",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["total_count"] == 2
            assert len(data["maps"]) == 2
            assert data["maps"][0]["map_id"] == "map_001"
            assert data["maps"][1]["map_id"] == "map_002"
    
    def test_delete_map_success(self, client, mock_user, auth_headers):
        """Test successful map deletion"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.delete_map') as mock_delete:
            
            # Setup mock
            mock_delete.return_value = {
                "success": True,
                "message": "Map test_map_001 deleted successfully"
            }
            
            # Test request
            response = client.delete(
                "/api/v1/map/test_map_001",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["message"] == "Map test_map_001 deleted successfully"
    
    def test_delete_map_not_found(self, client, mock_user, auth_headers):
        """Test map deletion when map not found"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user), \
             patch('app.api.v1.map.map_service.delete_map') as mock_delete:
            
            # Setup mock to raise exception
            mock_delete.side_effect = ValueError("Map test_map_001 not found")
            
            # Test request
            response = client.delete(
                "/api/v1/map/test_map_001",
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 404
            data = response.json()
            assert data["detail"] == "Map test_map_001 not found"
    
    def test_update_map_success(self, client, mock_user, auth_headers):
        """Test successful map update"""
        with patch('app.api.v1.map.get_current_user', return_value=mock_user):
            # Test request
            response = client.put(
                "/api/v1/map/test_map_001",
                json={"name": "Updated Map Name"},
                headers=auth_headers
            )
            
            # Assertions
            assert response.status_code == 200
            data = response.json()
            assert data["success"] is True
            assert data["message"] == "Map test_map_001 updated successfully"
    
    def test_unauthorized_access(self, client):
        """Test API access without authentication"""
        # Test request without auth headers
        response = client.post(
            "/api/v1/map/start-mapping",
            json={
                "map_name": "Test Map",
                "resolution": 50.0,
                "width": 1000,
                "height": 1000
            }
        )
        
        # Should return 401 Unauthorized
        assert response.status_code == 401
