"""
Unit tests for Network API endpoints - OHT-50 Backend
"""

import pytest
from unittest.mock import AsyncMock, patch
from datetime import datetime, timezone
from fastapi.testclient import TestClient
from fastapi import status

from app.main import app
from app.schemas.network import APStartRequest, APConfigRequest


class TestNetworkAPI:
    """Test cases for Network API endpoints"""
    
    @pytest.fixture
    def client(self):
        """Create test client"""
        return TestClient(app)
    
    @pytest.fixture
    def mock_network_service(self):
        """Create mock network service"""
        mock_service = AsyncMock()
        mock_service.get_network_status.return_value = {
            "success": True,
            "data": {
                "status": "connected",
                "ap_status": "stopped",
                "fallback_enabled": False,
                "config": {"ssid": "test"},
                "last_heartbeat": datetime.now(timezone.utc).isoformat(),
                "connection_errors": 0
            }
        }
        mock_service.start_wifi_ap.return_value = {
            "success": True,
            "message": "WiFi AP started successfully",
            "data": {"ap_ip": "192.168.4.1"}
        }
        mock_service.stop_wifi_ap.return_value = {
            "success": True,
            "message": "WiFi AP stopped successfully"
        }
        mock_service.get_ap_clients.return_value = {
            "success": True,
            "data": {
                "clients": [
                    {
                        "mac_address": "AA:BB:CC:DD:EE:FF",
                        "ip_address": "192.168.4.2",
                        "hostname": "test-device",
                        "connected_at": "2025-01-28T10:00:00Z",
                        "last_seen": "2025-01-28T10:30:00Z",
                        "signal_strength": -45,
                        "bytes_sent": 1024,
                        "bytes_received": 2048,
                        "session_duration": 1800,
                        "is_connected": True,
                        "is_blocked": False
                    }
                ],
                "client_count": 1
            }
        }
        mock_service.configure_ap.return_value = {
            "success": True,
            "message": "WiFi AP configured successfully",
            "data": {"config_applied": True}
        }
        mock_service.enable_fallback.return_value = {
            "success": True,
            "message": "Fallback connectivity enabled",
            "data": {"fallback_active": True}
        }
        mock_service.disable_fallback.return_value = {
            "success": True,
            "message": "Fallback connectivity disabled"
        }
        mock_service.get_connection_status.return_value = {
            "status": "connected",
            "ap_status": "stopped",
            "firmware_url": "http://localhost:8080",
            "last_heartbeat": datetime.now(timezone.utc).isoformat(),
            "connection_errors": 0,
            "fallback_enabled": False,
            "ap_clients_count": 0
        }
        return mock_service
    
    @pytest.fixture
    def admin_token(self):
        """Create admin token for testing"""
        return "admin_token_123"
    
    def test_get_network_status_success(self, client, mock_network_service):
        """Test successful network status retrieval"""
        with patch('app.api.v1.network.network_service', mock_network_service):
            response = client.get("/api/v1/network/status")
            
            assert response.status_code == status.HTTP_200_OK
            data = response.json()
            assert data["success"] is True
            assert "data" in data
            assert data["data"]["status"] == "connected"
            assert data["data"]["ap_status"] == "stopped"
            assert data["data"]["fallback_enabled"] is False
    
    def test_get_network_status_failure(self, client, mock_network_service):
        """Test network status retrieval failure"""
        mock_network_service.get_network_status.return_value = {
            "success": False,
            "error": "Service unavailable"
        }
        
        with patch('app.api.v1.network.network_service', mock_network_service):
            response = client.get("/api/v1/network/status")
            
            assert response.status_code == status.HTTP_500_INTERNAL_SERVER_ERROR
            data = response.json()
            assert "detail" in data
    
    def test_start_wifi_ap_success(self, client, mock_network_service, admin_token):
        """Test successful WiFi AP start"""
        with patch('app.api.v1.network.network_service', mock_network_service):
            with patch('app.api.v1.network.verify_admin_token', return_value={"is_admin": True}):
                response = client.post(
                    "/api/v1/network/ap/start",
                    json={
                        "ssid": "test_ap",
                        "password": "testpassword123",
                        "channel": 6,
                        "security": "WPA2",
                        "hidden": False,
                        "max_clients": 10
                    },
                    headers={"Authorization": f"Bearer {admin_token}"}
                )
                
                assert response.status_code == status.HTTP_200_OK
                data = response.json()
                assert data["success"] is True
                assert "WiFi AP started successfully" in data["message"]
                assert "ap_ip" in data["data"]
    
    def test_start_wifi_ap_unauthorized(self, client, mock_network_service):
        """Test WiFi AP start without admin token"""
        with patch('app.api.v1.network.network_service', mock_network_service):
            response = client.post(
                "/api/v1/network/ap/start",
                json={
                    "ssid": "test_ap",
                    "password": "testpassword123",
                    "channel": 6
                }
            )
            
            assert response.status_code == status.HTTP_403_FORBIDDEN
    
    def test_start_wifi_ap_invalid_config(self, client, mock_network_service, admin_token):
        """Test WiFi AP start with invalid configuration"""
        with patch('app.api.v1.network.network_service', mock_network_service):
            with patch('app.api.v1.network.verify_admin_token', return_value={"is_admin": True}):
                response = client.post(
                    "/api/v1/network/ap/start",
                    json={
                        "ssid": "test_ap",
                        "password": "short",  # Too short
                        "channel": 6
                    },
                    headers={"Authorization": f"Bearer {admin_token}"}
                )
                
                assert response.status_code == status.HTTP_422_UNPROCESSABLE_ENTITY
    
    def test_stop_wifi_ap_success(self, client, mock_network_service, admin_token):
        """Test successful WiFi AP stop"""
        with patch('app.api.v1.network.network_service', mock_network_service):
            with patch('app.api.v1.network.verify_admin_token', return_value={"is_admin": True}):
                response = client.post(
                    "/api/v1/network/ap/stop",
                    headers={"Authorization": f"Bearer {admin_token}"}
                )
                
                assert response.status_code == status.HTTP_200_OK
                data = response.json()
                assert data["success"] is True
                assert "WiFi AP stopped successfully" in data["message"]
    
    def test_get_ap_clients_success(self, client, mock_network_service):
        """Test successful AP clients retrieval"""
        with patch('app.api.v1.network.network_service', mock_network_service):
            response = client.get("/api/v1/network/ap/clients")
            
            assert response.status_code == status.HTTP_200_OK
            data = response.json()
            assert data["success"] is True
            assert data["data"]["client_count"] == 1
            assert len(data["data"]["clients"]) == 1
            assert data["data"]["clients"][0]["mac_address"] == "AA:BB:CC:DD:EE:FF"
    
    def test_configure_ap_success(self, client, mock_network_service, admin_token):
        """Test successful AP configuration"""
        with patch('app.api.v1.network.network_service', mock_network_service):
            with patch('app.api.v1.network.verify_admin_token', return_value={"is_admin": True}):
                response = client.post(
                    "/api/v1/network/ap/config",
                    json={
                        "ssid": "new_ssid",
                        "channel": 11,
                        "max_clients": 20
                    },
                    headers={"Authorization": f"Bearer {admin_token}"}
                )
                
                assert response.status_code == status.HTTP_200_OK
                data = response.json()
                assert data["success"] is True
                assert "WiFi AP configured successfully" in data["message"]
    
    def test_enable_fallback_success(self, client, mock_network_service, admin_token):
        """Test successful fallback enable"""
        with patch('app.api.v1.network.network_service', mock_network_service):
            with patch('app.api.v1.network.verify_admin_token', return_value={"is_admin": True}):
                response = client.post(
                    "/api/v1/network/fallback/enable",
                    headers={"Authorization": f"Bearer {admin_token}"}
                )
                
                assert response.status_code == status.HTTP_200_OK
                data = response.json()
                assert data["success"] is True
                assert "Fallback connectivity enabled" in data["message"]
    
    def test_disable_fallback_success(self, client, mock_network_service, admin_token):
        """Test successful fallback disable"""
        with patch('app.api.v1.network.network_service', mock_network_service):
            with patch('app.api.v1.network.verify_admin_token', return_value={"is_admin": True}):
                response = client.post(
                    "/api/v1/network/fallback/disable",
                    headers={"Authorization": f"Bearer {admin_token}"}
                )
                
                assert response.status_code == status.HTTP_200_OK
                data = response.json()
                assert data["success"] is True
                assert "Fallback connectivity disabled" in data["message"]
    
    def test_network_health_check(self, client, mock_network_service):
        """Test network health check endpoint"""
        with patch('app.api.v1.network.network_service', mock_network_service):
            response = client.get("/api/v1/network/health")
            
            assert response.status_code == status.HTTP_200_OK
            data = response.json()
            assert data["status"] == "healthy"
            assert data["service"] == "network"
            assert data["firmware_connected"] is True
            assert data["connection_errors"] == 0
    
    def test_network_health_check_unhealthy(self, client, mock_network_service):
        """Test network health check when unhealthy"""
        mock_network_service.get_connection_status.return_value = {
            "status": "error",
            "ap_status": "stopped",
            "firmware_url": "http://localhost:8080",
            "last_heartbeat": None,
            "connection_errors": 10,
            "fallback_enabled": False,
            "ap_clients_count": 0
        }
        
        with patch('app.api.v1.network.network_service', mock_network_service):
            response = client.get("/api/v1/network/health")
            
            assert response.status_code == status.HTTP_200_OK
            data = response.json()
            assert data["status"] == "unhealthy"
            assert data["firmware_connected"] is False
            assert data["connection_errors"] == 10


class TestNetworkAPISchemas:
    """Test cases for Network API schemas"""
    
    def test_ap_start_request_valid(self):
        """Test valid AP start request"""
        request = APStartRequest(
            ssid="test_ap",
            password="testpassword123",
            channel=6,
            security="WPA2",
            hidden=False,
            max_clients=10
        )
        
        assert request.ssid == "test_ap"
        assert request.password == "testpassword123"
        assert request.channel == 6
        assert request.security == "WPA2"
        assert request.hidden is False
        assert request.max_clients == 10
    
    def test_ap_start_request_invalid_ssid(self):
        """Test invalid AP start request - empty SSID"""
        with pytest.raises(ValueError, match="SSID cannot be empty"):
            APStartRequest(
                ssid="",
                password="testpassword123",
                channel=6
            )
    
    def test_ap_start_request_invalid_password(self):
        """Test invalid AP start request - short password"""
        with pytest.raises(ValueError, match="Password must be at least 8 characters"):
            APStartRequest(
                ssid="test_ap",
                password="short",
                channel=6
            )
    
    def test_ap_start_request_invalid_channel(self):
        """Test invalid AP start request - invalid channel"""
        with pytest.raises(ValueError):
            APStartRequest(
                ssid="test_ap",
                password="testpassword123",
                channel=15  # Invalid channel
            )
    
    def test_ap_config_request_partial(self):
        """Test partial AP config request"""
        request = APConfigRequest(
            ssid="new_ssid",
            channel=11
        )
        
        assert request.ssid == "new_ssid"
        assert request.channel == 11
        assert request.password is None
        assert request.security is None
        assert request.hidden is None
        assert request.max_clients is None
        assert request.bandwidth_limit is None
