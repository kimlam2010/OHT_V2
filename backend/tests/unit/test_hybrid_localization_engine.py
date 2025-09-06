"""
Unit tests for Hybrid Localization Engine
"""

import pytest
import asyncio
from unittest.mock import Mock, patch, AsyncMock
from datetime import datetime, timedelta

from app.services.hybrid_localization_engine import HybridLocalizationEngine
from app.models.map import RFIDTag
from app.models.sensors import SensorData


class TestHybridLocalizationEngine:
    """Test cases for HybridLocalizationEngine"""
    
    @pytest.fixture
    def localization_engine(self):
        """Create HybridLocalizationEngine instance for testing"""
        return HybridLocalizationEngine()
    
    @pytest.fixture
    def mock_db(self):
        """Mock database session"""
        return Mock()
    
    @pytest.fixture
    def sample_rfid_data(self):
        """Sample RFID data for testing"""
        return {
            "rfid_id": "RFID_001",
            "signal_strength": 0.8,
            "timestamp": datetime.utcnow().isoformat()
        }
    
    @pytest.fixture
    def sample_accelerometer_data(self):
        """Sample accelerometer data for testing"""
        return {
            "ax": 0.1,
            "ay": 0.2,
            "az": 0.05,
            "timestamp": datetime.utcnow().isoformat()
        }
    
    @pytest.fixture
    def sample_proximity_data(self):
        """Sample proximity data for testing"""
        return {
            "front": 0.5,
            "left": 0.3,
            "right": 0.4,
            "back": 0.6,
            "timestamp": datetime.utcnow().isoformat()
        }
    
    @pytest.fixture
    def sample_lidar_data(self):
        """Sample LiDAR data for testing"""
        return {
            "points": [
                {"x": 0, "y": 0, "distance": 0.1, "angle": 0},
                {"x": 100, "y": 100, "distance": 0.5, "angle": 1.57}
            ],
            "timestamp": datetime.utcnow().isoformat()
        }
    
    @pytest.mark.asyncio
    async def test_initialize_success(self, localization_engine, mock_db):
        """Test successful initialization"""
        with patch('app.services.hybrid_localization_engine.get_db') as mock_get_db:
            # Setup mocks
            mock_get_db.return_value = [mock_db]
            mock_db.query.return_value.filter.return_value.all.return_value = []
            
            # Test
            await localization_engine.initialize("test_map_001", 50.0)
            
            # Assertions
            assert localization_engine.map_id == "test_map_001"
            assert localization_engine.resolution == 50.0
            assert localization_engine.current_position is None
            assert localization_engine.position_history == []
    
    @pytest.mark.asyncio
    async def test_get_robot_position_no_sensor_data(self, localization_engine):
        """Test getting robot position with no sensor data"""
        # Initialize engine
        localization_engine.map_id = "test_map_001"
        
        with patch.object(localization_engine, '_get_sensor_data') as mock_get_data:
            # Setup mock
            mock_get_data.return_value = {}
            
            # Test
            result = await localization_engine.get_robot_position()
            
            # Assertions
            assert result is None
    
    @pytest.mark.asyncio
    async def test_get_robot_position_rfid_only(self, localization_engine, sample_rfid_data):
        """Test getting robot position with RFID data only"""
        # Initialize engine
        localization_engine.map_id = "test_map_001"
        
        # Setup mock RFID tag
        mock_rfid_tag = Mock()
        mock_rfid_tag.rfid_id = "RFID_001"
        mock_rfid_tag.position_x = 100.0
        mock_rfid_tag.position_y = 200.0
        mock_rfid_tag.orientation = 1.57
        mock_rfid_tag.is_active = True
        
        with patch.object(localization_engine, '_get_sensor_data') as mock_get_data, \
             patch('app.services.hybrid_localization_engine.get_db') as mock_get_db:
            
            # Setup mocks
            mock_get_data.return_value = {"rfid": sample_rfid_data}
            mock_get_db.return_value = [mock_db]
            mock_db.query.return_value.filter.return_value.first.return_value = mock_rfid_tag
            
            # Test
            result = await localization_engine.get_robot_position()
            
            # Assertions
            assert result is not None
            assert result["x"] == 100.0
            assert result["y"] == 200.0
            assert result["theta"] == 1.57
            assert result["confidence"] == 0.8
            assert result["source"] == "hybrid"
            assert result["primary_source"] == "rfid"
            assert result["sensor_count"] == 1
    
    @pytest.mark.asyncio
    async def test_get_robot_position_multiple_sensors(self, localization_engine, 
                                                      sample_rfid_data, sample_accelerometer_data):
        """Test getting robot position with multiple sensors"""
        # Initialize engine
        localization_engine.map_id = "test_map_001"
        localization_engine.current_position = {
            "x": 50.0,
            "y": 100.0,
            "theta": 0.0,
            "vx": 0.0,
            "vy": 0.0
        }
        
        # Setup mock RFID tag
        mock_rfid_tag = Mock()
        mock_rfid_tag.rfid_id = "RFID_001"
        mock_rfid_tag.position_x = 100.0
        mock_rfid_tag.position_y = 200.0
        mock_rfid_tag.orientation = 1.57
        mock_rfid_tag.is_active = True
        
        with patch.object(localization_engine, '_get_sensor_data') as mock_get_data, \
             patch('app.services.hybrid_localization_engine.get_db') as mock_get_db:
            
            # Setup mocks
            mock_get_data.return_value = {
                "rfid": sample_rfid_data,
                "accelerometer": sample_accelerometer_data
            }
            mock_get_db.return_value = [mock_db]
            mock_db.query.return_value.filter.return_value.first.return_value = mock_rfid_tag
            
            # Test
            result = await localization_engine.get_robot_position()
            
            # Assertions
            assert result is not None
            assert result["source"] == "hybrid"
            assert result["sensor_count"] == 2
            assert "primary_source" in result
            assert "timestamp" in result
    
    @pytest.mark.asyncio
    async def test_process_rfid_data_success(self, localization_engine, sample_rfid_data):
        """Test successful RFID data processing"""
        # Setup mock RFID tag
        mock_rfid_tag = Mock()
        mock_rfid_tag.rfid_id = "RFID_001"
        mock_rfid_tag.position_x = 100.0
        mock_rfid_tag.position_y = 200.0
        mock_rfid_tag.orientation = 1.57
        mock_rfid_tag.is_active = True
        
        with patch('app.services.hybrid_localization_engine.get_db') as mock_get_db:
            # Setup mocks
            mock_get_db.return_value = [mock_db]
            mock_db.query.return_value.filter.return_value.first.return_value = mock_rfid_tag
            
            # Test
            result = await localization_engine._process_rfid_data(sample_rfid_data)
            
            # Assertions
            assert result is not None
            assert result["x"] == 100.0
            assert result["y"] == 200.0
            assert result["theta"] == 1.57
            assert result["confidence"] == 0.8
            assert result["source"] == "rfid"
    
    @pytest.mark.asyncio
    async def test_process_rfid_data_weak_signal(self, localization_engine):
        """Test RFID data processing with weak signal"""
        weak_rfid_data = {
            "rfid_id": "RFID_001",
            "signal_strength": 0.3,  # Below threshold
            "timestamp": datetime.utcnow().isoformat()
        }
        
        # Test
        result = await localization_engine._process_rfid_data(weak_rfid_data)
        
        # Assertions
        assert result is None
    
    @pytest.mark.asyncio
    async def test_process_rfid_data_tag_not_found(self, localization_engine, sample_rfid_data):
        """Test RFID data processing when tag not found"""
        with patch('app.services.hybrid_localization_engine.get_db') as mock_get_db:
            # Setup mocks
            mock_get_db.return_value = [mock_db]
            mock_db.query.return_value.filter.return_value.first.return_value = None
            
            # Test
            result = await localization_engine._process_rfid_data(sample_rfid_data)
            
            # Assertions
            assert result is None
    
    @pytest.mark.asyncio
    async def test_process_accelerometer_data_success(self, localization_engine, sample_accelerometer_data):
        """Test successful accelerometer data processing"""
        # Setup current position
        localization_engine.current_position = {
            "x": 50.0,
            "y": 100.0,
            "theta": 0.0,
            "vx": 0.0,
            "vy": 0.0
        }
        
        # Test
        result = await localization_engine._process_accelerometer_data(sample_accelerometer_data)
        
        # Assertions
        assert result is not None
        assert result["x"] > 50.0  # Should have moved
        assert result["y"] > 100.0  # Should have moved
        assert result["confidence"] == 0.6
        assert result["source"] == "accelerometer"
        assert "vx" in result
        assert "vy" in result
    
    @pytest.mark.asyncio
    async def test_process_accelerometer_data_no_current_position(self, localization_engine, sample_accelerometer_data):
        """Test accelerometer data processing with no current position"""
        # Test
        result = await localization_engine._process_accelerometer_data(sample_accelerometer_data)
        
        # Assertions
        assert result is None
    
    @pytest.mark.asyncio
    async def test_process_proximity_data_success(self, localization_engine, sample_proximity_data):
        """Test successful proximity data processing"""
        # Setup current position
        localization_engine.current_position = {
            "x": 50.0,
            "y": 100.0,
            "theta": 0.0
        }
        
        # Test
        result = await localization_engine._process_proximity_data(sample_proximity_data)
        
        # Assertions
        assert result is not None
        assert result["x"] == 50.0
        assert result["y"] == 100.0
        assert result["theta"] == 0.0
        assert result["confidence"] == 0.7
        assert result["source"] == "proximity"
    
    @pytest.mark.asyncio
    async def test_process_lidar_data_success(self, localization_engine, sample_lidar_data):
        """Test successful LiDAR data processing"""
        # Setup current position
        localization_engine.current_position = {
            "x": 50.0,
            "y": 100.0,
            "theta": 0.0
        }
        
        # Test
        result = await localization_engine._process_lidar_data(sample_lidar_data)
        
        # Assertions
        assert result is not None
        assert result["x"] == 50.0
        assert result["y"] == 100.0
        assert result["theta"] == 0.0
        assert result["confidence"] == 0.5
        assert result["source"] == "lidar"
    
    @pytest.mark.asyncio
    async def test_process_lidar_data_no_points(self, localization_engine):
        """Test LiDAR data processing with no points"""
        empty_lidar_data = {
            "points": [],
            "timestamp": datetime.utcnow().isoformat()
        }
        
        # Test
        result = await localization_engine._process_lidar_data(empty_lidar_data)
        
        # Assertions
        assert result is None
    
    @pytest.mark.asyncio
    async def test_fuse_positions_success(self, localization_engine):
        """Test successful position fusion"""
        # Setup positions
        positions = {
            "rfid": {
                "x": 100.0,
                "y": 200.0,
                "theta": 1.57,
                "confidence": 0.9,
                "source": "rfid"
            },
            "accelerometer": {
                "x": 105.0,
                "y": 195.0,
                "theta": 1.6,
                "confidence": 0.6,
                "source": "accelerometer"
            }
        }
        
        confidences = {
            "rfid": 0.9,
            "accelerometer": 0.6
        }
        
        # Test
        result = await localization_engine._fuse_positions(positions, confidences)
        
        # Assertions
        assert result is not None
        assert result["source"] == "hybrid"
        assert result["primary_source"] == "rfid"
        assert result["sensor_count"] == 2
        assert result["confidence"] > 0.0
        assert "timestamp" in result
        
        # Check that fused position is between the two inputs
        assert 100.0 <= result["x"] <= 105.0
        assert 195.0 <= result["y"] <= 200.0
    
    @pytest.mark.asyncio
    async def test_fuse_positions_empty(self, localization_engine):
        """Test position fusion with empty positions"""
        # Test
        result = await localization_engine._fuse_positions({}, {})
        
        # Assertions
        assert result is None
    
    @pytest.mark.asyncio
    async def test_fuse_positions_zero_weight(self, localization_engine):
        """Test position fusion with zero total weight"""
        # Setup positions with zero confidence
        positions = {
            "rfid": {
                "x": 100.0,
                "y": 200.0,
                "theta": 1.57,
                "confidence": 0.0,
                "source": "rfid"
            }
        }
        
        confidences = {
            "rfid": 0.0
        }
        
        # Test
        result = await localization_engine._fuse_positions(positions, confidences)
        
        # Assertions
        assert result is None
    
    @pytest.mark.asyncio
    async def test_load_rfid_tags_success(self, localization_engine, mock_db):
        """Test successful RFID tags loading"""
        # Setup mock RFID tags
        mock_tag1 = Mock()
        mock_tag1.rfid_id = "RFID_001"
        mock_tag2 = Mock()
        mock_tag2.rfid_id = "RFID_002"
        
        with patch('app.services.hybrid_localization_engine.get_db') as mock_get_db:
            # Setup mocks
            mock_get_db.return_value = [mock_db]
            mock_db.query.return_value.filter.return_value.all.return_value = [mock_tag1, mock_tag2]
            
            # Test
            await localization_engine._load_rfid_tags()
            
            # No exceptions should be raised
            assert True
    
    @pytest.mark.asyncio
    async def test_finalize_success(self, localization_engine):
        """Test successful finalization"""
        # Setup state
        localization_engine.map_id = "test_map_001"
        localization_engine.current_position = {"x": 100, "y": 200}
        localization_engine.position_history = [{"x": 100, "y": 200}]
        
        # Test
        await localization_engine.finalize()
        
        # Assertions
        assert localization_engine.map_id is None
        assert localization_engine.current_position is None
        assert localization_engine.position_history == []
    
    def test_get_position_history_empty(self, localization_engine):
        """Test getting position history when empty"""
        # Test
        history = localization_engine.get_position_history()
        
        # Assertions
        assert history == []
    
    def test_get_position_history_with_data(self, localization_engine):
        """Test getting position history with data"""
        # Setup history
        localization_engine.position_history = [
            {"x": 100, "y": 200, "timestamp": "2025-01-01T10:00:00"},
            {"x": 105, "y": 205, "timestamp": "2025-01-01T10:01:00"},
            {"x": 110, "y": 210, "timestamp": "2025-01-01T10:02:00"}
        ]
        
        # Test
        history = localization_engine.get_position_history(limit=2)
        
        # Assertions
        assert len(history) == 2
        assert history[0]["x"] == 105
        assert history[1]["x"] == 110
    
    def test_get_localization_stats_empty(self, localization_engine):
        """Test getting localization stats when empty"""
        # Test
        stats = localization_engine.get_localization_stats()
        
        # Assertions
        assert stats["total_positions"] == 0
        assert stats["average_confidence"] == 0.0
        assert stats["sensor_usage"] == {}
        assert stats["current_position"] is None
    
    def test_get_localization_stats_with_data(self, localization_engine):
        """Test getting localization stats with data"""
        # Setup history
        localization_engine.position_history = [
            {"x": 100, "y": 200, "confidence": 0.8, "primary_source": "rfid"},
            {"x": 105, "y": 205, "confidence": 0.9, "primary_source": "rfid"},
            {"x": 110, "y": 210, "confidence": 0.7, "primary_source": "accelerometer"}
        ]
        localization_engine.current_position = {"x": 110, "y": 210}
        
        # Test
        stats = localization_engine.get_localization_stats()
        
        # Assertions
        assert stats["total_positions"] == 3
        assert abs(stats["average_confidence"] - 0.8) < 0.01
        assert stats["sensor_usage"]["rfid"] == 2
        assert stats["sensor_usage"]["accelerometer"] == 1
        assert stats["current_position"]["x"] == 110
