"""
Unit tests for Map Service
"""

import pytest
import asyncio
from unittest.mock import Mock, patch, AsyncMock
from datetime import datetime

from app.services.map_service import MapService
from app.models.map import Map, MapSession


class TestMapService:
    """Test cases for MapService"""
    
    @pytest.fixture
    def map_service(self):
        """Create MapService instance for testing"""
        return MapService()
    
    @pytest.fixture
    def mock_db(self):
        """Mock database session"""
        return Mock()
    
    @pytest.fixture
    def sample_map_data(self):
        """Sample map data for testing"""
        return {
            "map_id": "test_map_001",
            "name": "Test Map",
            "resolution": 50.0,
            "width": 1000,
            "height": 1000,
            "occupancy_grid": [[-1, 0, 1], [0, 1, 0], [1, 0, -1]],
            "robot_trajectory": [],
            "rfid_positions": [],
            "metadata": {"test": True}
        }
    
    @pytest.mark.asyncio
    async def test_start_mapping_success(self, map_service, mock_db):
        """Test successful mapping start"""
        with patch('app.services.map_service.get_db') as mock_get_db, \
             patch('app.services.map_service.uuid') as mock_uuid, \
             patch('app.services.hybrid_localization_engine.HybridLocalizationEngine.initialize') as mock_init:
            
            # Setup mocks
            mock_get_db.return_value = [mock_db]
            mock_uuid.uuid4.return_value.hex = "test12345678"
            mock_db.add = Mock()
            mock_db.commit = Mock()
            mock_db.refresh = Mock()
            
            # Test
            result = await map_service.start_mapping("Test Map")
            
            # Assertions
            assert result["success"] is True
            assert "map_id" in result
            assert "session_id" in result
            assert result["message"] == "Mapping started successfully"
            assert map_service.is_mapping is True
            assert map_service.current_map is not None
            assert map_service.current_session is not None
            
            # Verify database operations
            assert mock_db.add.call_count == 2  # Map and Session
            assert mock_db.commit.call_count == 2
            assert mock_db.refresh.call_count == 2
    
    @pytest.mark.asyncio
    async def test_start_mapping_already_mapping(self, map_service):
        """Test starting mapping when already mapping"""
        # Set mapping state
        map_service.is_mapping = True
        
        # Test
        with pytest.raises(ValueError, match="Mapping already in progress"):
            await map_service.start_mapping("Test Map")
    
    @pytest.mark.asyncio
    async def test_stop_mapping_success(self, map_service, mock_db):
        """Test successful mapping stop"""
        # Setup mapping state
        map_service.is_mapping = True
        map_service.current_session = Mock()
        map_service.current_session.session_id = "test_session"
        map_service.current_session.total_scans = 10
        map_service.current_map = Mock()
        map_service.current_map.map_id = "test_map"
        map_service.current_map.occupancy_grid = [[-1, 0, 1], [0, 1, 0]]
        
        with patch('app.services.map_service.get_db') as mock_get_db, \
             patch('app.services.hybrid_localization_engine.HybridLocalizationEngine.finalize') as mock_finalize:
            
            # Setup mocks
            mock_get_db.return_value = [mock_db]
            mock_db.commit = Mock()
            
            # Test
            result = await map_service.stop_mapping()
            
            # Assertions
            assert result["success"] is True
            assert result["map_id"] == "test_map"
            assert result["session_id"] == "test_session"
            assert result["message"] == "Mapping stopped successfully"
            assert map_service.is_mapping is False
            assert map_service.current_map is None
            assert map_service.current_session is None
            
            # Verify database operations
            assert mock_db.commit.call_count == 1
            mock_finalize.assert_called_once()
    
    @pytest.mark.asyncio
    async def test_stop_mapping_no_active_session(self, map_service):
        """Test stopping mapping when no active session"""
        # Test
        with pytest.raises(ValueError, match="No active mapping session"):
            await map_service.stop_mapping()
    
    @pytest.mark.asyncio
    async def test_get_robot_position_success(self, map_service):
        """Test successful robot position retrieval"""
        # Setup
        map_service.current_map = Mock()
        map_service.current_map.map_id = "test_map"
        
        with patch('app.services.hybrid_localization_engine.HybridLocalizationEngine.get_robot_position') as mock_get_pos, \
             patch.object(map_service, '_store_robot_position') as mock_store:
            
            # Setup mock
            mock_position = {
                "x": 100.0,
                "y": 200.0,
                "theta": 1.57,
                "confidence": 0.9,
                "source": "hybrid"
            }
            mock_get_pos.return_value = mock_position
            
            # Test
            result = await map_service.get_robot_position()
            
            # Assertions
            assert result["success"] is True
            assert result["position"]["x"] == 100.0
            assert result["position"]["y"] == 200.0
            assert result["position"]["theta"] == 1.57
            assert result["position"]["confidence"] == 0.9
            assert result["position"]["source"] == "hybrid"
            assert "timestamp" in result
            
            # Verify position was stored
            mock_store.assert_called_once_with(mock_position)
    
    @pytest.mark.asyncio
    async def test_get_robot_position_no_map(self, map_service):
        """Test getting robot position when no active map"""
        # Test
        with pytest.raises(ValueError, match="No active map for localization"):
            await map_service.get_robot_position()
    
    @pytest.mark.asyncio
    async def test_update_occupancy_grid_success(self, map_service, mock_db):
        """Test successful occupancy grid update"""
        # Setup
        map_service.is_mapping = True
        map_service.current_map = Mock()
        map_service.current_map.map_id = "test_map"
        map_service.current_map.resolution = 50.0
        map_service.current_map.occupancy_grid = [[-1, 0, 1], [0, 1, 0]]
        map_service.current_session = Mock()
        map_service.current_session.total_scans = 5
        
        lidar_data = [
            {"x": 0, "y": 0, "distance": 0.05, "angle": 0},
            {"x": 100, "y": 100, "distance": 0.6, "angle": 1.57}
        ]
        
        with patch('app.services.map_service.get_db') as mock_get_db:
            # Setup mocks
            mock_get_db.return_value = [mock_db]
            mock_db.commit = Mock()
            
            # Test
            result = await map_service.update_occupancy_grid(lidar_data)
            
            # Assertions
            assert result["success"] is True
            assert result["total_scans"] == 6
            assert result["message"] == "Occupancy grid updated"
            
            # Verify database operations
            assert mock_db.commit.call_count == 1
    
    @pytest.mark.asyncio
    async def test_update_occupancy_grid_no_mapping(self, map_service):
        """Test updating occupancy grid when not mapping"""
        lidar_data = [{"x": 0, "y": 0, "distance": 0.1, "angle": 0}]
        
        # Test
        with pytest.raises(ValueError, match="No active mapping session"):
            await map_service.update_occupancy_grid(lidar_data)
    
    @pytest.mark.asyncio
    async def test_load_map_success(self, map_service, mock_db, sample_map_data):
        """Test successful map loading"""
        # Setup mock map
        mock_map = Mock()
        mock_map.map_id = "test_map_001"
        mock_map.name = "Test Map"
        mock_map.resolution = 50.0
        mock_map.width = 1000
        mock_map.height = 1000
        mock_map.metadata = {"test": True}
        
        with patch('app.services.map_service.get_db') as mock_get_db, \
             patch('app.services.hybrid_localization_engine.HybridLocalizationEngine.initialize') as mock_init:
            
            # Setup mocks
            mock_get_db.return_value = [mock_db]
            mock_db.query.return_value.filter.return_value.first.return_value = mock_map
            
            # Test
            result = await map_service.load_map("test_map_001")
            
            # Assertions
            assert result["success"] is True
            assert result["map"]["map_id"] == "test_map_001"
            assert result["map"]["name"] == "Test Map"
            assert result["map"]["resolution"] == 50.0
            assert result["message"] == "Map loaded successfully"
            assert map_service.current_map == mock_map
            
            # Verify localization engine initialization
            mock_init.assert_called_once_with("test_map_001", 50.0)
    
    @pytest.mark.asyncio
    async def test_load_map_not_found(self, map_service, mock_db):
        """Test loading non-existent map"""
        with patch('app.services.map_service.get_db') as mock_get_db:
            # Setup mocks
            mock_get_db.return_value = [mock_db]
            mock_db.query.return_value.filter.return_value.first.return_value = None
            
            # Test
            with pytest.raises(ValueError, match="Map test_map_001 not found"):
                await map_service.load_map("test_map_001")
    
    @pytest.mark.asyncio
    async def test_get_map_list_success(self, map_service, mock_db):
        """Test successful map list retrieval"""
        # Setup mock maps
        mock_map1 = Mock()
        mock_map1.map_id = "map_001"
        mock_map1.name = "Map 1"
        mock_map1.resolution = 50.0
        mock_map1.width = 1000
        mock_map1.height = 1000
        mock_map1.created_at = datetime(2025, 1, 1)
        mock_map1.updated_at = datetime(2025, 1, 1)
        mock_map1.metadata = {}
        
        mock_map2 = Mock()
        mock_map2.map_id = "map_002"
        mock_map2.name = "Map 2"
        mock_map2.resolution = 25.0
        mock_map2.width = 2000
        mock_map2.height = 2000
        mock_map2.created_at = datetime(2025, 1, 2)
        mock_map2.updated_at = datetime(2025, 1, 2)
        mock_map2.metadata = {}
        
        with patch('app.services.map_service.get_db') as mock_get_db:
            # Setup mocks
            mock_get_db.return_value = [mock_db]
            mock_db.query.return_value.filter.return_value.all.return_value = [mock_map1, mock_map2]
            
            # Test
            result = await map_service.get_map_list()
            
            # Assertions
            assert len(result) == 2
            assert result[0]["map_id"] == "map_001"
            assert result[0]["name"] == "Map 1"
            assert result[1]["map_id"] == "map_002"
            assert result[1]["name"] == "Map 2"
    
    @pytest.mark.asyncio
    async def test_delete_map_success(self, map_service, mock_db):
        """Test successful map deletion"""
        # Setup mock map
        mock_map = Mock()
        mock_map.map_id = "test_map_001"
        mock_map.is_active = True
        
        with patch('app.services.map_service.get_db') as mock_get_db:
            # Setup mocks
            mock_get_db.return_value = [mock_db]
            mock_db.query.return_value.filter.return_value.first.return_value = mock_map
            mock_db.commit = Mock()
            
            # Test
            result = await map_service.delete_map("test_map_001")
            
            # Assertions
            assert result["success"] is True
            assert result["message"] == "Map test_map_001 deleted successfully"
            assert mock_map.is_active is False
            
            # Verify database operations
            assert mock_db.commit.call_count == 1
    
    @pytest.mark.asyncio
    async def test_delete_map_not_found(self, map_service, mock_db):
        """Test deleting non-existent map"""
        with patch('app.services.map_service.get_db') as mock_get_db:
            # Setup mocks
            mock_get_db.return_value = [mock_db]
            mock_db.query.return_value.filter.return_value.first.return_value = None
            
            # Test
            with pytest.raises(ValueError, match="Map test_map_001 not found"):
                await map_service.delete_map("test_map_001")
    
    def test_calculate_map_coverage(self, map_service):
        """Test map coverage calculation"""
        # Test with known grid
        occupancy_grid = [
            [-1, 0, 1],
            [0, 1, 0],
            [1, 0, -1]
        ]
        
        coverage = map_service._calculate_map_coverage(occupancy_grid)
        
        # 6 explored cells out of 9 total = 0.667
        assert abs(coverage - 0.667) < 0.01
    
    def test_get_mapping_status(self, map_service):
        """Test mapping status retrieval"""
        # Test initial status
        status = map_service.get_mapping_status()
        assert status["is_mapping"] is False
        assert status["current_map_id"] is None
        assert status["current_session_id"] is None
        assert status["total_scans"] == 0
        assert status["mapping_quality"] == 0.0
        
        # Test with active mapping
        map_service.is_mapping = True
        map_service.current_map = Mock()
        map_service.current_map.map_id = "test_map"
        map_service.current_session = Mock()
        map_service.current_session.session_id = "test_session"
        map_service.current_session.total_scans = 10
        map_service.current_session.mapping_quality = 0.8
        
        status = map_service.get_mapping_status()
        assert status["is_mapping"] is True
        assert status["current_map_id"] == "test_map"
        assert status["current_session_id"] == "test_session"
        assert status["total_scans"] == 10
        assert status["mapping_quality"] == 0.8
