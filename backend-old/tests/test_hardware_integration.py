"""
Hardware Integration Tests cho OHT-50 Backend
Test Backend ↔ Firmware integration
"""

import pytest
import asyncio
import json
from unittest.mock import Mock, patch, AsyncMock
from datetime import datetime

from app.services.hardware_service import HardwareService
from app.services.hardware_communication import HardwareCommunicationService
from app.services.log_service import LogService


class TestHardwareCommunication:
    """Test Hardware Communication Service"""
    
    @pytest.fixture
    def communication_service(self):
        """Fixture cho HardwareCommunicationService"""
        log_service = LogService()
        service = HardwareCommunicationService(log_service)
        return service
    
    @pytest.mark.asyncio
    async def test_communication_initialization(self, communication_service):
        """Test communication service initialization"""
        # Mock socket connection
        with patch('socket.socket') as mock_socket:
            mock_socket.return_value.connect.return_value = None
            
            # Mock WebSocket server
            with patch('websockets.serve') as mock_websocket:
                mock_websocket.return_value = AsyncMock()
                
                result = await communication_service.initialize()
                assert result is True
                assert communication_service.initialized is True
    
    @pytest.mark.asyncio
    async def test_send_hardware_command(self, communication_service):
        """Test sending hardware command"""
        # Mock queue system
        with patch.object(communication_service, 'command_queue') as mock_queue:
            mock_queue.put = AsyncMock()
            with patch.object(communication_service, 'response_queue') as mock_response_queue:
                # Mock response queue to return immediately
                mock_response_queue.empty.return_value = False
                
                # Capture the command ID and return matching response
                def mock_get():
                    # Get the command that was put into the queue
                    command = mock_queue.put.call_args[0][0]
                    return {
                        "id": command["id"],
                        "type": "set_led",
                        "success": True,
                        "data": {"led_id": "power", "state": True}
                    }
                
                mock_response_queue.get = AsyncMock(side_effect=mock_get)
                
                result = await communication_service.send_hardware_command(
                    "set_led", {"led_id": "power", "state": True}
                )
                
                assert "error" not in result
                assert result["success"] is True
    
    @pytest.mark.asyncio
    async def test_websocket_message_handling(self, communication_service):
        """Test WebSocket message handling"""
        # Mock event handler
        mock_handler = AsyncMock()
        communication_service.register_event_handler("led_state_changed", mock_handler)
        
        # Test message
        message = json.dumps({
            "type": "led_state_changed",
            "led_id": "power",
            "state": True
        })
        
        await communication_service._handle_websocket_message(message)
        
        # Verify handler was called
        mock_handler.assert_called_once()
        call_args = mock_handler.call_args[0][0]
        assert call_args["type"] == "led_state_changed"
        assert call_args["led_id"] == "power"
        assert call_args["state"] is True


class TestHardwareService:
    """Test Hardware Service Integration"""
    
    @pytest.fixture
    def hardware_service(self):
        """Fixture cho HardwareService"""
        log_service = LogService()
        service = HardwareService(log_service)
        return service
    
    @pytest.mark.asyncio
    async def test_hardware_service_initialization(self, hardware_service):
        """Test hardware service initialization"""
        # Mock communication service
        with patch.object(hardware_service.communication_service, 'initialize') as mock_init:
            mock_init.return_value = True
            
            with patch.object(hardware_service.communication_service, 'get_hardware_status') as mock_status:
                mock_status.return_value = {
                    "leds": {
                        "power": False,
                        "system": False,
                        "comm": False,
                        "network": False,
                        "error": False
                    },
                    "estop": {
                        "triggered": False,
                        "last_triggered": None
                    },
                    "relays": {
                        "relay1": False,
                        "relay2": False
                    }
                }
                
                result = await hardware_service.initialize()
                assert result is True
                assert hardware_service.initialized is True
    
    @pytest.mark.asyncio
    async def test_set_led_state(self, hardware_service):
        """Test setting LED state"""
        # Mock communication service
        with patch.object(hardware_service.communication_service, 'set_led_state') as mock_set:
            mock_set.return_value = {
                "success": True,
                "led_id": "power",
                "state": True
            }
            
            result = await hardware_service.set_led_state("power", True)
            
            assert "error" not in result
            assert result["success"] is True
            assert hardware_service.led_status["power"] is True
    
    @pytest.mark.asyncio
    async def test_get_estop_status(self, hardware_service):
        """Test getting E-Stop status"""
        # Mock communication service
        with patch.object(hardware_service.communication_service, 'get_estop_status') as mock_get:
            mock_get.return_value = {
                "estop": {
                    "triggered": False,
                    "last_triggered": None
                }
            }
            
            result = await hardware_service.get_estop_status()
            
            assert "error" not in result
            assert "estop" in result
            assert result["estop"]["triggered"] is False
    
    @pytest.mark.asyncio
    async def test_set_relay_state(self, hardware_service):
        """Test setting relay state"""
        # Mock communication service
        with patch.object(hardware_service.communication_service, 'set_relay_state') as mock_set:
            mock_set.return_value = {
                "success": True,
                "relay_id": "relay1",
                "state": True
            }
            
            result = await hardware_service.set_relay_state("relay1", True)
            
            assert "error" not in result
            assert result["success"] is True
            assert hardware_service.relay_status["relay1"] is True
    
    @pytest.mark.asyncio
    async def test_hardware_health(self, hardware_service):
        """Test hardware health metrics"""
        # Mock communication service
        with patch.object(hardware_service.communication_service, 'get_connection_status') as mock_conn:
            mock_conn.return_value = {
                "firmware_connected": True,
                "websocket_connected": True,
                "initialized": True
            }
            
            result = await hardware_service.get_hardware_health()
            
            assert "error" not in result
            assert "health_score" in result
            assert result["health_score"] >= 80  # Should be healthy
            assert result["status"] == "healthy"
    
    @pytest.mark.asyncio
    async def test_hardware_events(self, hardware_service):
        """Test hardware events handling"""
        # Add test event
        hardware_service._add_hardware_event("test_event", {"test": "data"})
        
        events = await hardware_service.get_hardware_events()
        
        assert len(events) >= 1
        assert events[-1]["type"] == "test_event"
        assert events[-1]["data"]["test"] == "data"


class TestHardwareAPI:
    """Test Hardware API Endpoints"""
    
    @pytest.fixture
    def client(self):
        """Test client"""
        from fastapi.testclient import TestClient
        from app.main import app
        return TestClient(app)
    
    def test_get_hardware_status(self, client):
        """Test GET /api/v1/hardware/status endpoint"""
        response = client.get("/api/v1/hardware/status")
        assert response.status_code == 200
        
        data = response.json()
        assert "success" in data
        assert data["success"] is True
        assert "data" in data
    
    def test_set_led_state(self, client):
        """Test POST /api/v1/hardware/leds/{led_name} endpoint"""
        # Mock HardwareService
        with patch('app.services.hardware_service.HardwareService.set_led_state') as mock_set_led:
            mock_set_led.return_value = {
                "success": True,
                "led_id": "power",
                "state": True
            }
            
            response = client.post(
                "/api/v1/hardware/leds/power",
                params={"state": True}
            )
            assert response.status_code == 200
            
            data = response.json()
            assert "success" in data
            assert data["success"] is True
    
    def test_get_estop_status(self, client):
        """Test GET /api/v1/hardware/estop endpoint"""
        response = client.get("/api/v1/hardware/estop")
        assert response.status_code == 200
        
        data = response.json()
        assert "success" in data
        assert data["success"] is True
    
    def test_reset_estop(self, client):
        """Test POST /api/v1/hardware/estop/reset endpoint"""
        response = client.post("/api/v1/hardware/estop/reset")
        assert response.status_code == 200
        
        data = response.json()
        assert "success" in data
        assert data["success"] is True
    
    def test_set_relay_state(self, client):
        """Test POST /api/v1/hardware/relays/{relay_name} endpoint"""
        # Mock HardwareService
        with patch('app.services.hardware_service.HardwareService.set_relay_state') as mock_set_relay:
            mock_set_relay.return_value = {
                "success": True,
                "relay_id": "relay1",
                "state": True
            }
            
            response = client.post(
                "/api/v1/hardware/relays/relay1",
                params={"state": True}
            )
            assert response.status_code == 200
            
            data = response.json()
            assert "success" in data
            assert data["success"] is True
    
    def test_get_hardware_health(self, client):
        """Test GET /api/v1/hardware/health endpoint"""
        response = client.get("/api/v1/hardware/health")
        assert response.status_code == 200
        
        data = response.json()
        assert "success" in data
        assert data["success"] is True
        assert "data" in data
        assert "health_score" in data["data"]
    
    def test_get_hardware_events(self, client):
        """Test GET /api/v1/hardware/events endpoint"""
        response = client.get("/api/v1/hardware/events")
        assert response.status_code == 200
        
        data = response.json()
        assert "success" in data
        assert data["success"] is True
        assert "data" in data
        assert isinstance(data["data"], list)


class TestHardwareErrorHandling:
    """Test Hardware Error Handling"""
    
    @pytest.fixture
    def hardware_service(self):
        """Fixture cho HardwareService"""
        log_service = LogService()
        service = HardwareService(log_service)
        return service
    
    @pytest.mark.asyncio
    async def test_invalid_led_id(self, hardware_service):
        """Test invalid LED ID handling"""
        result = await hardware_service.set_led_state("invalid_led", True)
        
        assert "error" in result
        assert "Invalid LED ID" in result["error"]
    
    @pytest.mark.asyncio
    async def test_invalid_relay_id(self, hardware_service):
        """Test invalid relay ID handling"""
        result = await hardware_service.set_relay_state("invalid_relay", True)
        
        assert "error" in result
        assert "Invalid relay ID" in result["error"]
    
    @pytest.mark.asyncio
    async def test_communication_failure(self, hardware_service):
        """Test communication failure handling"""
        # Mock communication failure
        with patch.object(hardware_service.communication_service, 'set_led_state') as mock_set:
            mock_set.return_value = {"error": "Connection failed"}
            
            result = await hardware_service.set_led_state("power", True)
            
            assert "error" in result
            assert "Connection failed" in result["error"]


class TestHardwarePerformance:
    """Test Hardware Performance"""
    
    @pytest.fixture
    def hardware_service(self):
        """Fixture cho HardwareService"""
        log_service = LogService()
        service = HardwareService(log_service)
        return service
    
    @pytest.mark.asyncio
    async def test_command_response_time(self, hardware_service):
        """Test command response time"""
        # Mock fast response
        with patch.object(hardware_service.communication_service, 'set_led_state') as mock_set:
            mock_set.return_value = {"success": True}
            
            start_time = datetime.now()
            result = await hardware_service.set_led_state("power", True)
            end_time = datetime.now()
            
            response_time = (end_time - start_time).total_seconds()
            
            assert response_time < 1.0  # Should respond within 1 second
            assert "error" not in result
    
    @pytest.mark.asyncio
    async def test_concurrent_commands(self, hardware_service):
        """Test concurrent command handling"""
        # Mock communication service
        with patch.object(hardware_service.communication_service, 'set_led_state') as mock_set:
            mock_set.return_value = {"success": True}
            
            # Send multiple concurrent commands
            tasks = []
            for i in range(10):
                task = hardware_service.set_led_state("power", True)
                tasks.append(task)
            
            results = await asyncio.gather(*tasks)
            
            # All commands should succeed
            for result in results:
                assert "error" not in result
                assert result["success"] is True


if __name__ == "__main__":
    pytest.main([__file__, "-v"])
