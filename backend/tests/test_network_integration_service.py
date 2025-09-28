"""
Unit tests for Network Integration Service - OHT-50 Backend
"""

import pytest
import asyncio
from unittest.mock import AsyncMock, MagicMock, patch
from datetime import datetime, timezone

from app.services.network_integration_service import (
    NetworkIntegrationService, 
    MockNetworkService,
    NetworkStatus,
    WiFiAPStatus
)


class TestNetworkIntegrationService:
    """Test cases for NetworkIntegrationService"""
    
    @pytest.fixture
    def network_service(self):
        """Create network service instance for testing"""
        return NetworkIntegrationService()
    
    @pytest.fixture
    def mock_fw_client(self):
        """Create mock firmware client"""
        mock_client = AsyncMock()
        mock_client.connect.return_value = True
        mock_client.disconnect.return_value = True
        mock_client.check_connection_health.return_value = True
        mock_client.get.return_value = {"success": True, "data": {}}
        mock_client.post.return_value = {"success": True, "data": {}}
        return mock_client
    
    @pytest.mark.asyncio
    async def test_initialization(self, network_service):
        """Test service initialization"""
        assert network_service.status == NetworkStatus.DISCONNECTED
        assert network_service.ap_status == WiFiAPStatus.STOPPED
        assert network_service.connection_errors == 0
        assert network_service.fallback_enabled is False
    
    @pytest.mark.asyncio
    async def test_initialize_success(self, network_service, mock_fw_client):
        """Test successful initialization"""
        with patch('app.services.network_integration_service.FWClient', return_value=mock_fw_client):
            result = await network_service.initialize()
            
            assert result is True
            assert network_service.status == NetworkStatus.CONNECTED
            assert network_service.fw_client is not None
    
    @pytest.mark.asyncio
    async def test_initialize_failure(self, network_service, mock_fw_client):
        """Test initialization failure"""
        mock_fw_client.connect.return_value = False
        
        with patch('app.services.network_integration_service.FWClient', return_value=mock_fw_client):
            result = await network_service.initialize()
            
            assert result is False
            assert network_service.status == NetworkStatus.ERROR
    
    @pytest.mark.asyncio
    async def test_shutdown(self, network_service, mock_fw_client):
        """Test service shutdown"""
        network_service.fw_client = mock_fw_client
        network_service._health_monitor_task = asyncio.create_task(asyncio.sleep(1))
        network_service._network_monitor_task = asyncio.create_task(asyncio.sleep(1))
        
        await network_service.shutdown()
        
        assert network_service.status == NetworkStatus.DISCONNECTED
        mock_fw_client.disconnect.assert_called_once()
    
    @pytest.mark.asyncio
    async def test_get_network_status_success(self, network_service, mock_fw_client):
        """Test successful network status retrieval"""
        network_service.fw_client = mock_fw_client
        network_service.status = NetworkStatus.CONNECTED
        network_service.ap_status = WiFiAPStatus.RUNNING
        network_service.fallback_enabled = True
        network_service.last_heartbeat = datetime.now(timezone.utc)
        
        mock_fw_client.get.return_value = {
            "success": True,
            "data": {
                "config": {"ssid": "test"},
                "fallback_enabled": True
            }
        }
        
        result = await network_service.get_network_status()
        
        assert result["success"] is True
        assert "data" in result
        assert result["data"]["status"] == "connected"
        assert result["data"]["ap_status"] == "running"
        assert result["data"]["fallback_enabled"] is True
    
    @pytest.mark.asyncio
    async def test_get_network_status_failure(self, network_service):
        """Test network status retrieval failure"""
        network_service.fw_client = None
        
        result = await network_service.get_network_status()
        
        assert result["success"] is False
        assert "error" in result
    
    @pytest.mark.asyncio
    async def test_start_wifi_ap_success(self, network_service, mock_fw_client):
        """Test successful WiFi AP start"""
        network_service.fw_client = mock_fw_client
        
        config = {
            "ssid": "test_ap",
            "password": "testpassword123",
            "channel": 6,
            "security": "WPA2",
            "hidden": False,
            "max_clients": 10
        }
        
        mock_fw_client.post.return_value = {
            "success": True,
            "data": {"ap_ip": "192.168.4.1"}
        }
        
        result = await network_service.start_wifi_ap(config)
        
        assert result["success"] is True
        assert network_service.ap_status == WiFiAPStatus.RUNNING
        assert network_service.status == NetworkStatus.AP_MODE
        mock_fw_client.post.assert_called_once()
    
    @pytest.mark.asyncio
    async def test_start_wifi_ap_invalid_config(self, network_service):
        """Test WiFi AP start with invalid config"""
        network_service.fw_client = None
        
        config = {
            "ssid": "test_ap"
            # Missing required fields
        }
        
        result = await network_service.start_wifi_ap(config)
        
        assert result["success"] is False
        assert "Missing required fields" in result["error"]
    
    @pytest.mark.asyncio
    async def test_stop_wifi_ap_success(self, network_service, mock_fw_client):
        """Test successful WiFi AP stop"""
        network_service.fw_client = mock_fw_client
        network_service.ap_status = WiFiAPStatus.RUNNING
        network_service.status = NetworkStatus.AP_MODE
        network_service.ap_clients = [{"mac": "AA:BB:CC:DD:EE:FF"}]
        
        mock_fw_client.post.return_value = {"success": True}
        
        result = await network_service.stop_wifi_ap()
        
        assert result["success"] is True
        assert network_service.ap_status == WiFiAPStatus.STOPPED
        assert network_service.status == NetworkStatus.CONNECTED
        assert len(network_service.ap_clients) == 0
    
    @pytest.mark.asyncio
    async def test_get_ap_clients_success(self, network_service, mock_fw_client):
        """Test successful AP clients retrieval"""
        network_service.fw_client = mock_fw_client
        
        mock_clients = [
            {
                "mac_address": "AA:BB:CC:DD:EE:FF",
                "ip_address": "192.168.4.2",
                "hostname": "test-device",
                "connected_at": "2025-01-28T10:00:00Z",
                "last_seen": "2025-01-28T10:30:00Z",
                "signal_strength": -45
            }
        ]
        
        mock_fw_client.get.return_value = {
            "success": True,
            "data": {"clients": mock_clients}
        }
        
        result = await network_service.get_ap_clients()
        
        assert result["success"] is True
        assert result["data"]["client_count"] == 1
        assert len(result["data"]["clients"]) == 1
        assert network_service.ap_clients == mock_clients
    
    @pytest.mark.asyncio
    async def test_configure_ap_success(self, network_service, mock_fw_client):
        """Test successful AP configuration"""
        network_service.fw_client = mock_fw_client
        network_service.network_config = {}
        
        config = {
            "ssid": "new_ssid",
            "channel": 11,
            "max_clients": 20
        }
        
        mock_fw_client.post.return_value = {
            "success": True,
            "data": {"config_applied": True}
        }
        
        result = await network_service.configure_ap(config)
        
        assert result["success"] is True
        assert network_service.network_config["ssid"] == "new_ssid"
        assert network_service.network_config["channel"] == 11
        assert network_service.network_config["max_clients"] == 20
    
    @pytest.mark.asyncio
    async def test_enable_fallback_success(self, network_service, mock_fw_client):
        """Test successful fallback enable"""
        network_service.fw_client = mock_fw_client
        network_service.fallback_enabled = False
        network_service.status = NetworkStatus.CONNECTED
        
        mock_fw_client.post.return_value = {
            "success": True,
            "data": {"fallback_active": True}
        }
        
        result = await network_service.enable_fallback()
        
        assert result["success"] is True
        assert network_service.fallback_enabled is True
        assert network_service.status == NetworkStatus.FALLBACK
    
    @pytest.mark.asyncio
    async def test_disable_fallback_success(self, network_service, mock_fw_client):
        """Test successful fallback disable"""
        network_service.fw_client = mock_fw_client
        network_service.fallback_enabled = True
        network_service.status = NetworkStatus.FALLBACK
        
        mock_fw_client.post.return_value = {"success": True}
        
        result = await network_service.disable_fallback()
        
        assert result["success"] is True
        assert network_service.fallback_enabled is False
        assert network_service.status == NetworkStatus.CONNECTED
    
    @pytest.mark.asyncio
    async def test_connection_status(self, network_service):
        """Test connection status retrieval"""
        network_service.status = NetworkStatus.CONNECTED
        network_service.ap_status = WiFiAPStatus.RUNNING
        network_service.firmware_url = "http://localhost:8080"
        network_service.last_heartbeat = datetime.now(timezone.utc)
        network_service.connection_errors = 5
        network_service.fallback_enabled = True
        network_service.ap_clients = [{"mac": "AA:BB:CC:DD:EE:FF"}]
        
        status = network_service.get_connection_status()
        
        assert status["status"] == "connected"
        assert status["ap_status"] == "running"
        assert status["firmware_url"] == "http://localhost:8080"
        assert status["connection_errors"] == 5
        assert status["fallback_enabled"] is True
        assert status["ap_clients_count"] == 1


class TestMockNetworkService:
    """Test cases for MockNetworkService"""
    
    @pytest.fixture
    def mock_service(self):
        """Create mock network service instance"""
        return MockNetworkService()
    
    @pytest.mark.asyncio
    async def test_mock_initialization(self, mock_service):
        """Test mock service initialization"""
        assert mock_service.mock_data is not None
        assert "network_status" in mock_service.mock_data
        assert "ap_clients" in mock_service.mock_data
    
    @pytest.mark.asyncio
    async def test_mock_get_network_status(self, mock_service):
        """Test mock network status retrieval"""
        result = await mock_service.get_network_status()
        
        assert result["success"] is True
        assert "data" in result
        assert result["data"]["status"] == "connected"
    
    @pytest.mark.asyncio
    async def test_mock_start_wifi_ap(self, mock_service):
        """Test mock WiFi AP start"""
        config = {
            "ssid": "test_ap",
            "password": "testpassword123",
            "channel": 6
        }
        
        result = await mock_service.start_wifi_ap(config)
        
        assert result["success"] is True
        assert "Mock: WiFi AP started" in result["message"]
        assert "ap_ip" in result["data"]
    
    @pytest.mark.asyncio
    async def test_mock_stop_wifi_ap(self, mock_service):
        """Test mock WiFi AP stop"""
        result = await mock_service.stop_wifi_ap()
        
        assert result["success"] is True
        assert "Mock: WiFi AP stopped" in result["message"]
    
    @pytest.mark.asyncio
    async def test_mock_get_ap_clients(self, mock_service):
        """Test mock AP clients retrieval"""
        result = await mock_service.get_ap_clients()
        
        assert result["success"] is True
        assert result["data"]["client_count"] == 1
        assert len(result["data"]["clients"]) == 1
    
    @pytest.mark.asyncio
    async def test_mock_configure_ap(self, mock_service):
        """Test mock AP configuration"""
        config = {
            "ssid": "new_ssid",
            "channel": 11
        }
        
        result = await mock_service.configure_ap(config)
        
        assert result["success"] is True
        assert "Mock: WiFi AP configured" in result["message"]
    
    @pytest.mark.asyncio
    async def test_mock_enable_fallback(self, mock_service):
        """Test mock fallback enable"""
        result = await mock_service.enable_fallback()
        
        assert result["success"] is True
        assert "Mock: Fallback connectivity enabled" in result["message"]
    
    @pytest.mark.asyncio
    async def test_mock_disable_fallback(self, mock_service):
        """Test mock fallback disable"""
        result = await mock_service.disable_fallback()
        
        assert result["success"] is True
        assert "Mock: Fallback connectivity disabled" in result["message"]


class TestNetworkServiceFactory:
    """Test cases for network service factory function"""
    
    @patch.dict('os.environ', {'ENVIRONMENT': 'production'})
    def test_factory_production_mode(self):
        """Test factory in production mode"""
        from app.services.network_integration_service import get_network_service
        
        service = get_network_service()
        
        assert isinstance(service, NetworkIntegrationService)
        assert not isinstance(service, MockNetworkService)
    
    @patch.dict('os.environ', {'TESTING': 'true'})
    def test_factory_testing_mode(self):
        """Test factory in testing mode"""
        from app.services.network_integration_service import get_network_service
        
        service = get_network_service()
        
        assert isinstance(service, MockNetworkService)
    
    @patch.dict('os.environ', {'USE_MOCK_FIRMWARE': 'true'})
    def test_factory_development_mock_mode(self):
        """Test factory in development with mock flag"""
        from app.services.network_integration_service import get_network_service
        
        service = get_network_service()
        
        assert isinstance(service, MockNetworkService)
    
    @patch.dict('os.environ', {'ENVIRONMENT': 'development', 'USE_MOCK_FIRMWARE': 'false'})
    def test_factory_development_real_mode(self):
        """Test factory in development with real firmware"""
        from app.services.network_integration_service import get_network_service
        
        service = get_network_service()
        
        assert isinstance(service, NetworkIntegrationService)
        assert not isinstance(service, MockNetworkService)
