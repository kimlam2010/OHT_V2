"""
Simple unit tests for Telemetry System
Basic functionality tests for telemetry data collection, LiDAR processing, and sensor data processing
"""

import pytest
import asyncio
from datetime import datetime, timezone

from app.services.telemetry_data_collector import (
    telemetry_data_collector, 
    TelemetryDataCollector, 
    DataSource, 
    DataQuality
)
from app.services.lidar_data_processor import (
    lidar_data_processor, 
    LiDARDataProcessor
)
from app.services.sensor_data_processor import (
    sensor_data_processor, 
    SensorDataProcessor, 
    SensorType
)


class TestTelemetryDataCollectorSimple:
    """Simple test cases for TelemetryDataCollector"""
    
    @pytest.fixture
    def collector(self):
        """Create a fresh TelemetryDataCollector instance for testing"""
        return TelemetryDataCollector()
    
    def test_initialization(self, collector):
        """Test TelemetryDataCollector initialization"""
        assert collector.max_buffer_size == 10000
        assert collector.is_collecting is False
        assert len(collector.data_buffer) == 0
        assert collector.stats.total_collected == 0
    
    @pytest.mark.asyncio
    async def test_start_stop_collection(self, collector):
        """Test starting and stopping data collection"""
        # Test start collection
        result = await collector.start_collection()
        assert result is True
        assert collector.is_collecting is True
        
        # Test stop collection
        result = await collector.stop_collection()
        assert result is True
        assert collector.is_collecting is False
    
    def test_get_collection_stats(self, collector):
        """Test getting collection statistics"""
        stats = collector.get_collection_stats()
        
        assert "active_sources" in stats
        assert "buffer_size" in stats
        assert "data_rate_per_second" in stats
        assert "avg_processing_time_ms" in stats


class TestLiDARDataProcessorSimple:
    """Simple test cases for LiDARDataProcessor"""
    
    @pytest.fixture
    def processor(self):
        """Create a fresh LiDARDataProcessor instance for testing"""
        return LiDARDataProcessor()
    
    def test_initialization(self, processor):
        """Test LiDARDataProcessor initialization"""
        assert processor.scan_count == 0
        assert processor.obstacle_count == 0
        assert len(processor.processing_times) == 0
    
    @pytest.mark.asyncio
    async def test_process_lidar_data_success(self, processor):
        """Test successful LiDAR data processing"""
        raw_data = {
            "points": [
                {"distance": 1.5, "angle": 0.0, "intensity": 0.8},
                {"distance": 2.0, "angle": 45.0, "intensity": 0.6},
                {"distance": 0.8, "angle": 90.0, "intensity": 0.9}  # Close obstacle
            ],
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        result = await processor.process_lidar_data(raw_data)
        
        assert result is not None
        assert hasattr(result, 'points')
        assert hasattr(result, 'obstacles')
        assert hasattr(result, 'scan_quality')
        assert hasattr(result, 'processing_time_ms')
        assert result.processing_time_ms < 20.0  # Performance target
    
    def test_get_processing_stats(self, processor):
        """Test getting processing statistics"""
        stats = processor.get_processing_stats()
        
        assert "avg_processing_time_ms" in stats
        assert "max_processing_time_ms" in stats
        assert "min_processing_time_ms" in stats


class TestSensorDataProcessorSimple:
    """Simple test cases for SensorDataProcessor"""
    
    @pytest.fixture
    def processor(self):
        """Create a fresh SensorDataProcessor instance for testing"""
        return SensorDataProcessor()
    
    def test_initialization(self, processor):
        """Test SensorDataProcessor initialization"""
        assert len(processor.data_history) > 0
        assert len(processor.data_history) == 4  # 4 sensor types initialized
        assert hasattr(processor, 'rfid_signal_threshold')
        assert hasattr(processor, 'acceleration_threshold')
    
    @pytest.mark.asyncio
    async def test_process_sensor_data_rfid_success(self, processor):
        """Test successful RFID sensor data processing"""
        raw_data = {"tag_id": "RFID123", "signal_strength": -45}
        
        result = await processor.process_sensor_data(SensorType.RFID, raw_data)
        
        assert result is not None
        assert hasattr(result, 'processed_data')
        assert hasattr(result, 'processing_time_ms')
        assert result.processing_time_ms < 10.0  # Performance target
    
    def test_get_sensor_statistics(self, processor):
        """Test getting sensor statistics"""
        # Add some test data first to avoid KeyError
        processor.data_history[SensorType.RFID] = [
            type('MockData', (), {
                'sensor_type': SensorType.RFID,
                'timestamp': datetime.now(timezone.utc),
                'status': 'OK'
            })()
        ]
        
        # Initialize sensor status to avoid KeyError
        from app.services.sensor_data_processor import SensorStatus
        processor.sensor_status = {
            SensorType.RFID: SensorStatus.ACTIVE,
            SensorType.ACCELEROMETER: SensorStatus.ACTIVE,
            SensorType.COMPASS: SensorStatus.ACTIVE,
            SensorType.DOCKING: SensorStatus.ACTIVE,
            SensorType.MOTOR: SensorStatus.ACTIVE,
            SensorType.BATTERY: SensorStatus.ACTIVE,
            SensorType.TEMPERATURE: SensorStatus.ACTIVE
        }
        
        stats = processor.get_sensor_statistics()
        
        # Check that stats contains sensor type data
        assert "rfid" in stats
        assert "accelerometer" in stats
        assert "compass" in stats
        assert "docking" in stats


class TestIntegrationSimple:
    """Simple integration tests for telemetry system components"""
    
    @pytest.mark.asyncio
    async def test_telemetry_data_flow(self):
        """Test complete telemetry data flow"""
        # Test data collection
        collector = TelemetryDataCollector()
        await collector.start_collection()
        
        # Test LiDAR processing
        processor = LiDARDataProcessor()
        lidar_data = {
            "points": [
                {"distance": 1.0, "angle": 0.0, "intensity": 0.8},
                {"distance": 2.0, "angle": 45.0, "intensity": 0.6}
            ],
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        lidar_result = await processor.process_lidar_data(lidar_data)
        assert lidar_result is not None
        
        # Test sensor processing
        sensor_processor = SensorDataProcessor()
        sensor_data = {"tag_id": "TEST123"}
        sensor_result = await sensor_processor.process_sensor_data(SensorType.RFID, sensor_data)
        assert sensor_result is not None
        
        # Verify data flow
        assert processor.scan_count == 1
        assert len(sensor_processor.data_history[SensorType.RFID]) == 1
        
        await collector.stop_collection()
    
    @pytest.mark.asyncio
    async def test_performance_targets(self):
        """Test that performance targets are met"""
        processor = LiDARDataProcessor()
        sensor_processor = SensorDataProcessor()
        
        # Test LiDAR processing performance
        lidar_data = {
            "points": [{"distance": 1.0, "angle": 0.0, "intensity": 0.8}],
            "timestamp": datetime.now(timezone.utc).isoformat()
        }
        
        start_time = datetime.now(timezone.utc)
        lidar_result = await processor.process_lidar_data(lidar_data)
        processing_time = (datetime.now(timezone.utc) - start_time).total_seconds() * 1000
        
        assert processing_time < 20.0  # Target: < 20ms
        assert lidar_result.processing_time_ms < 20.0
        
        # Test sensor processing performance
        start_time = datetime.now(timezone.utc)
        sensor_result = await sensor_processor.process_sensor_data(SensorType.RFID, {"tag_id": "TEST"})
        sensor_time = (datetime.now(timezone.utc) - start_time).total_seconds() * 1000
        
        assert sensor_time < 10.0  # Target: < 10ms
        assert sensor_result.processing_time_ms < 10.0


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
