"""
Integration tests for Firmware Integration
"""

import pytest
import asyncio
from unittest.mock import Mock, patch, AsyncMock
from datetime import datetime

from app.services.firmware_integration_service import (
    FirmwareIntegrationService, 
    SensorType, 
    FirmwareStatus,
    get_firmware_service
)
from app.services.sensor_data_processor import SensorDataProcessor, ProcessingResult
from app.services.realtime_updates_service import RealtimeUpdatesService, UpdateType


class TestFirmwareIntegration:
    """Integration tests for firmware integration"""
    
    @pytest.fixture
    def firmware_service(self):
        """Create firmware service instance"""
        return FirmwareIntegrationService("http://localhost:8081")
    
    @pytest.fixture
    def sensor_processor(self):
        """Create sensor processor instance"""
        return SensorDataProcessor()
    
    @pytest.fixture
    def realtime_service(self):
        """Create real-time updates service instance"""
        return RealtimeUpdatesService()
    
    @pytest.mark.asyncio
    async def test_firmware_initialization_success(self, firmware_service):
        """Test successful firmware initialization"""
        with patch('httpx.AsyncClient.request') as mock_request:
            # Mock successful response
            mock_response = Mock()
            mock_response.status_code = 200
            mock_response.json.return_value = {"status": "healthy"}
            mock_request.return_value = mock_response
            
            # Test initialization
            result = await firmware_service.initialize()
            
            # Assertions
            assert result is True
            assert firmware_service.status == FirmwareStatus.CONNECTED
            assert firmware_service.connection_errors == 0
            assert firmware_service.last_heartbeat is not None
    
    @pytest.mark.asyncio
    async def test_firmware_initialization_failure(self, firmware_service):
        """Test firmware initialization failure"""
        with patch('httpx.AsyncClient.request') as mock_request:
            # Mock failed response
            mock_response = Mock()
            mock_response.status_code = 500
            mock_response.text = "Internal Server Error"
            mock_request.return_value = mock_response
            
            # Test initialization
            result = await firmware_service.initialize()
            
            # Assertions
            assert result is False
            assert firmware_service.status == FirmwareStatus.ERROR
            assert firmware_service.connection_errors == 1
    
    @pytest.mark.asyncio
    async def test_get_sensor_data_success(self, firmware_service):
        """Test successful sensor data retrieval"""
        with patch('httpx.AsyncClient.request') as mock_request:
            # Mock successful response
            mock_response = Mock()
            mock_response.status_code = 200
            mock_response.json.return_value = {
                "rfid_id": "TAG_001",
                "signal_strength": 0.8,
                "distance": 150.5,
                "angle": 45.0,
                "quality": 0.9,
                "timestamp": datetime.utcnow().isoformat(),
                "is_valid": True
            }
            mock_request.return_value = mock_response
            
            # Test sensor data retrieval
            result = await firmware_service.get_sensor_data(SensorType.RFID, "RFID_001")
            
            # Assertions
            assert result is not None
            assert result.sensor_type == SensorType.RFID
            assert result.sensor_id == "RFID_001"
            assert result.quality == 0.9
            assert result.is_valid is True
            assert result.data["rfid_id"] == "TAG_001"
    
    @pytest.mark.asyncio
    async def test_get_sensor_data_failure(self, firmware_service):
        """Test sensor data retrieval failure"""
        with patch('httpx.AsyncClient.request') as mock_request:
            # Mock failed response
            mock_response = Mock()
            mock_response.status_code = 404
            mock_response.text = "Sensor not found"
            mock_request.return_value = mock_response
            
            # Test sensor data retrieval
            result = await firmware_service.get_sensor_data(SensorType.RFID, "NONEXISTENT")
            
            # Assertions
            assert result is None
    
    @pytest.mark.asyncio
    async def test_send_robot_command_success(self, firmware_service):
        """Test successful robot command sending"""
        with patch('httpx.AsyncClient.request') as mock_request:
            # Mock successful response
            mock_response = Mock()
            mock_response.status_code = 200
            mock_response.json.return_value = {"success": True, "message": "Command executed"}
            mock_request.return_value = mock_response
            
            # Test robot command
            command = {"type": "move", "direction": "forward", "speed": 0.5}
            result = await firmware_service.send_robot_command(command)
            
            # Assertions
            assert result is True
    
    @pytest.mark.asyncio
    async def test_send_robot_command_failure(self, firmware_service):
        """Test robot command sending failure"""
        with patch('httpx.AsyncClient.request') as mock_request:
            # Mock failed response
            mock_response = Mock()
            mock_response.status_code = 400
            mock_response.text = "Invalid command"
            mock_request.return_value = mock_response
            
            # Test robot command
            command = {"type": "invalid", "data": "test"}
            result = await firmware_service.send_robot_command(command)
            
            # Assertions
            assert result is False
    
    @pytest.mark.asyncio
    async def test_heartbeat_success(self, firmware_service):
        """Test successful heartbeat"""
        with patch('httpx.AsyncClient.request') as mock_request:
            # Mock successful response
            mock_response = Mock()
            mock_response.status_code = 200
            mock_response.json.return_value = {"status": "healthy"}
            mock_request.return_value = mock_response
            
            # Test heartbeat
            result = await firmware_service.heartbeat()
            
            # Assertions
            assert result is True
            assert firmware_service.status == FirmwareStatus.CONNECTED
            assert firmware_service.connection_errors == 0
    
    @pytest.mark.asyncio
    async def test_heartbeat_failure(self, firmware_service):
        """Test heartbeat failure"""
        with patch('httpx.AsyncClient.request') as mock_request:
            # Mock failed response
            mock_response = Mock()
            mock_response.status_code = 500
            mock_response.text = "Service unavailable"
            mock_request.return_value = mock_response
            
            # Test heartbeat
            result = await firmware_service.heartbeat()
            
            # Assertions
            assert result is False
            assert firmware_service.status == FirmwareStatus.ERROR
            assert firmware_service.connection_errors == 1
    
    @pytest.mark.asyncio
    async def test_get_connection_status(self, firmware_service):
        """Test connection status retrieval"""
        # Set some test values
        firmware_service.status = FirmwareStatus.CONNECTED
        firmware_service.last_heartbeat = datetime.utcnow()
        firmware_service.connection_errors = 0
        
        # Test connection status
        status = await firmware_service.get_connection_status()
        
        # Assertions
        assert status["status"] == "connected"
        assert status["connection_errors"] == 0
        assert status["is_healthy"] is True
        assert status["last_heartbeat"] is not None
    
    @pytest.mark.asyncio
    async def test_sensor_data_processing_success(self, sensor_processor):
        """Test successful sensor data processing"""
        from app.services.firmware_integration_service import SensorReading
        
        # Create test sensor reading
        sensor_reading = SensorReading(
            sensor_type=SensorType.RFID,
            sensor_id="RFID_001",
            data={
                "rfid_id": "TAG_001",
                "signal_strength": 0.8,
                "distance": 150.5,
                "angle": 45.0
            },
            quality=0.9,
            timestamp=datetime.utcnow(),
            is_valid=True
        )
        
        # Test processing
        result = await sensor_processor.process_sensor_data(sensor_reading)
        
        # Assertions
        assert result.success is True
        assert result.processed_data is not None
        assert result.quality_score > 0.0
        assert result.processing_time > 0.0
        assert result.errors is not None
        assert result.warnings is not None
    
    @pytest.mark.asyncio
    async def test_sensor_data_processing_validation_error(self, sensor_processor):
        """Test sensor data processing with validation error"""
        from app.services.firmware_integration_service import SensorReading
        
        # Create invalid sensor reading
        sensor_reading = SensorReading(
            sensor_type=SensorType.RFID,
            sensor_id="RFID_001",
            data={
                "rfid_id": "",  # Invalid empty ID
                "signal_strength": 1.5,  # Invalid value > 1.0
                "distance": -10.0,  # Invalid negative distance
                "angle": 200.0  # Invalid angle > 180
            },
            quality=0.9,
            timestamp=datetime.utcnow(),
            is_valid=True
        )
        
        # Test processing
        result = await sensor_processor.process_sensor_data(sensor_reading)
        
        # Assertions
        assert result.success is True  # Processing succeeds but with errors
        assert len(result.errors) > 0  # Should have validation errors
        assert result.quality_score < 1.0  # Quality should be reduced
    
    @pytest.mark.asyncio
    async def test_realtime_updates_service_start_stop(self, realtime_service):
        """Test real-time updates service start and stop"""
        # Test start
        result = await realtime_service.start()
        assert result is True
        assert realtime_service.is_running is True
        
        # Test stop
        await realtime_service.stop()
        assert realtime_service.is_running is False
    
    @pytest.mark.asyncio
    async def test_realtime_updates_subscription(self, realtime_service):
        """Test real-time updates subscription"""
        # Start service
        await realtime_service.start()
        
        try:
            # Test subscription
            result = await realtime_service.subscribe("client_001", [UpdateType.POSITION, UpdateType.TELEMETRY])
            assert result is True
            
            # Check subscribers
            position_subscribers = await realtime_service.get_subscriber_count(UpdateType.POSITION)
            telemetry_subscribers = await realtime_service.get_subscriber_count(UpdateType.TELEMETRY)
            
            assert position_subscribers == 1
            assert telemetry_subscribers == 1
            
            # Test unsubscription
            result = await realtime_service.unsubscribe("client_001", [UpdateType.POSITION])
            assert result is True
            
            # Check subscribers after unsubscription
            position_subscribers = await realtime_service.get_subscriber_count(UpdateType.POSITION)
            telemetry_subscribers = await realtime_service.get_subscriber_count(UpdateType.TELEMETRY)
            
            assert position_subscribers == 0
            assert telemetry_subscribers == 1  # Still subscribed to telemetry
            
        finally:
            await realtime_service.stop()
    
    @pytest.mark.asyncio
    async def test_realtime_updates_publishing(self, realtime_service):
        """Test real-time updates publishing"""
        # Start service
        await realtime_service.start()
        
        try:
            # Test position update publishing
            position_data = {"x": 100.0, "y": 200.0, "theta": 1.57}
            result = await realtime_service.publish_position_update(position_data)
            assert result is True
            
            # Test telemetry update publishing
            telemetry_data = {"battery": 85, "temperature": 42.5}
            result = await realtime_service.publish_telemetry_update(telemetry_data)
            assert result is True
            
            # Test alert publishing
            alert_data = {"level": "warning", "message": "Low battery"}
            result = await realtime_service.publish_alert(alert_data)
            assert result is True
            
        finally:
            await realtime_service.stop()
    
    @pytest.mark.asyncio
    async def test_realtime_updates_service_stats(self, realtime_service):
        """Test real-time updates service statistics"""
        # Start service
        await realtime_service.start()
        
        try:
            # Get service stats
            stats = await realtime_service.get_service_stats()
            
            # Assertions
            assert stats["is_running"] is True
            assert stats["update_types"] == len(UpdateType)
            assert stats["active_tasks"] > 0
            assert "subscribers" in stats
            assert "queue_sizes" in stats
            assert "frequencies" in stats
            
        finally:
            await realtime_service.stop()
    
    @pytest.mark.asyncio
    async def test_firmware_service_factory(self):
        """Test firmware service factory function"""
        # Test real service
        real_service = get_firmware_service(use_mock=False)
        assert isinstance(real_service, FirmwareIntegrationService)
        
        # Test mock service
        mock_service = get_firmware_service(use_mock=True)
        assert isinstance(mock_service, Mock)  # MockFirmwareService is a Mock
    
    @pytest.mark.asyncio
    async def test_sensor_processor_stats(self, sensor_processor):
        """Test sensor processor statistics"""
        # Get processing stats
        stats = await sensor_processor.get_processing_stats()
        
        # Assertions
        assert "total_processed" in stats
        assert "history_size" in stats
        assert "max_history_size" in stats
        assert "sensor_types" in stats
        assert len(stats["sensor_types"]) > 0
    
    @pytest.mark.asyncio
    async def test_integration_workflow(self, firmware_service, sensor_processor, realtime_service):
        """Test complete integration workflow"""
        # Start real-time service
        await realtime_service.start()
        
        try:
            # Mock firmware responses
            with patch('httpx.AsyncClient.request') as mock_request:
                # Mock successful sensor data response
                mock_response = Mock()
                mock_response.status_code = 200
                mock_response.json.return_value = {
                    "rfid_id": "TAG_001",
                    "signal_strength": 0.8,
                    "distance": 150.5,
                    "angle": 45.0,
                    "quality": 0.9,
                    "timestamp": datetime.utcnow().isoformat(),
                    "is_valid": True
                }
                mock_request.return_value = mock_response
                
                # Step 1: Get sensor data from firmware
                sensor_reading = await firmware_service.get_sensor_data(SensorType.RFID, "RFID_001")
                assert sensor_reading is not None
                
                # Step 2: Process sensor data
                processing_result = await sensor_processor.process_sensor_data(sensor_reading)
                assert processing_result.success is True
                
                # Step 3: Publish real-time update
                update_result = await realtime_service.publish_sensor_data_update(processing_result.processed_data)
                assert update_result is True
                
                # Step 4: Verify service stats
                firmware_stats = await firmware_service.get_connection_status()
                processing_stats = await sensor_processor.get_processing_stats()
                realtime_stats = await realtime_service.get_service_stats()
                
                assert firmware_stats["is_healthy"] is True
                assert processing_stats["total_processed"] > 0
                assert realtime_stats["is_running"] is True
                
        finally:
            await realtime_service.stop()
