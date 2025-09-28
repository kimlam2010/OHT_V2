"""
Unit tests for Network Monitor Service - OHT-50 Backend
"""

import pytest
import asyncio
from unittest.mock import AsyncMock, MagicMock, patch
from datetime import datetime, timezone

from app.services.network_monitor import (
    NetworkMonitorService,
    NetworkEventType,
    NetworkStatus,
    WiFiAPStatus
)


class TestNetworkMonitorService:
    """Test cases for NetworkMonitorService"""
    
    @pytest.fixture
    def network_monitor(self):
        """Create network monitor service instance"""
        return NetworkMonitorService()
    
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
                "latency_ms": 50.0,
                "packet_loss_percent": 1.0,
                "signal_strength": -60
            }
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
        return mock_service
    
    @pytest.fixture
    def mock_websocket_alert_service(self):
        """Create mock WebSocket alert service"""
        mock_service = AsyncMock()
        mock_service.broadcast_alert.return_value = True
        return mock_service
    
    @pytest.mark.asyncio
    async def test_initialization(self, network_monitor):
        """Test service initialization"""
        assert network_monitor.is_running is False
        assert network_monitor.last_status == NetworkStatus.DISCONNECTED
        assert network_monitor.last_ap_status == WiFiAPStatus.STOPPED
        assert network_monitor.last_client_count == 0
        assert network_monitor.last_fallback_enabled is False
        assert len(network_monitor.event_history) == 0
    
    @pytest.mark.asyncio
    async def test_start_monitoring_success(self, network_monitor):
        """Test successful monitoring start"""
        result = await network_monitor.start_monitoring()
        
        assert result is True
        assert network_monitor.is_running is True
        assert network_monitor.monitor_task is not None
        
        # Clean up
        await network_monitor.stop_monitoring()
    
    @pytest.mark.asyncio
    async def test_start_monitoring_already_running(self, network_monitor):
        """Test starting monitoring when already running"""
        # Start monitoring first time
        await network_monitor.start_monitoring()
        
        # Try to start again
        result = await network_monitor.start_monitoring()
        
        assert result is True  # Should return True without error
        
        # Clean up
        await network_monitor.stop_monitoring()
    
    @pytest.mark.asyncio
    async def test_stop_monitoring(self, network_monitor):
        """Test monitoring stop"""
        # Start monitoring first
        await network_monitor.start_monitoring()
        assert network_monitor.is_running is True
        
        # Stop monitoring
        await network_monitor.stop_monitoring()
        
        assert network_monitor.is_running is False
    
    @pytest.mark.asyncio
    async def test_stop_monitoring_not_running(self, network_monitor):
        """Test stopping monitoring when not running"""
        # Should not raise error
        await network_monitor.stop_monitoring()
        assert network_monitor.is_running is False
    
    @pytest.mark.asyncio
    async def test_check_network_status_success(self, network_monitor, mock_network_service):
        """Test successful network status check"""
        with patch('app.services.network_monitor.network_service', mock_network_service):
            # Set initial state
            network_monitor.last_status = NetworkStatus.DISCONNECTED
            network_monitor.last_ap_status = WiFiAPStatus.STOPPED
            network_monitor.last_client_count = 0
            network_monitor.last_fallback_enabled = False
            
            await network_monitor._check_network_status()
            
            # Should update status
            assert network_monitor.last_status == NetworkStatus.CONNECTED
            assert network_monitor.last_ap_status == WiFiAPStatus.STOPPED
            assert network_monitor.last_client_count == 1
            assert network_monitor.last_fallback_enabled is False
    
    @pytest.mark.asyncio
    async def test_check_network_status_failure(self, network_monitor, mock_network_service):
        """Test network status check failure"""
        mock_network_service.get_network_status.return_value = {
            "success": False,
            "error": "Service unavailable"
        }
        
        with patch('app.services.network_monitor.network_service', mock_network_service):
            # Should not raise error
            await network_monitor._check_network_status()
    
    @pytest.mark.asyncio
    async def test_handle_status_change(self, network_monitor, mock_websocket_alert_service):
        """Test handling network status change"""
        with patch('app.services.network_monitor.websocket_alert_service', mock_websocket_alert_service):
            old_status = NetworkStatus.DISCONNECTED
            new_status = NetworkStatus.CONNECTED
            
            await network_monitor._handle_status_change(old_status, new_status)
            
            # Should broadcast event
            mock_websocket_alert_service.broadcast_alert.assert_called_once()
            
            # Check event history
            assert len(network_monitor.event_history) == 1
            event = network_monitor.event_history[0]
            assert event["event_type"] == NetworkEventType.STATUS_CHANGED.value
            assert event["event_data"]["old_status"] == "disconnected"
            assert event["event_data"]["new_status"] == "connected"
    
    @pytest.mark.asyncio
    async def test_handle_ap_status_change_started(self, network_monitor, mock_websocket_alert_service):
        """Test handling AP status change - started"""
        with patch('app.services.network_monitor.websocket_alert_service', mock_websocket_alert_service):
            old_status = WiFiAPStatus.STOPPED
            new_status = WiFiAPStatus.RUNNING
            
            await network_monitor._handle_ap_status_change(old_status, new_status)
            
            # Should broadcast event
            mock_websocket_alert_service.broadcast_alert.assert_called_once()
            
            # Check event history
            assert len(network_monitor.event_history) == 1
            event = network_monitor.event_history[0]
            assert event["event_type"] == NetworkEventType.AP_STARTED.value
    
    @pytest.mark.asyncio
    async def test_handle_ap_status_change_stopped(self, network_monitor, mock_websocket_alert_service):
        """Test handling AP status change - stopped"""
        with patch('app.services.network_monitor.websocket_alert_service', mock_websocket_alert_service):
            old_status = WiFiAPStatus.RUNNING
            new_status = WiFiAPStatus.STOPPED
            
            await network_monitor._handle_ap_status_change(old_status, new_status)
            
            # Should broadcast event
            mock_websocket_alert_service.broadcast_alert.assert_called_once()
            
            # Check event history
            assert len(network_monitor.event_history) == 1
            event = network_monitor.event_history[0]
            assert event["event_type"] == NetworkEventType.AP_STOPPED.value
    
    @pytest.mark.asyncio
    async def test_handle_client_count_change_connected(self, network_monitor, mock_websocket_alert_service):
        """Test handling client count change - connected"""
        with patch('app.services.network_monitor.websocket_alert_service', mock_websocket_alert_service):
            old_count = 0
            new_count = 1
            
            await network_monitor._handle_client_count_change(old_count, new_count)
            
            # Should broadcast event
            mock_websocket_alert_service.broadcast_alert.assert_called_once()
            
            # Check event history
            assert len(network_monitor.event_history) == 1
            event = network_monitor.event_history[0]
            assert event["event_type"] == NetworkEventType.CLIENT_CONNECTED.value
            assert event["event_data"]["client_count"] == 1
            assert event["event_data"]["change"] == 1
    
    @pytest.mark.asyncio
    async def test_handle_client_count_change_disconnected(self, network_monitor, mock_websocket_alert_service):
        """Test handling client count change - disconnected"""
        with patch('app.services.network_monitor.websocket_alert_service', mock_websocket_alert_service):
            old_count = 2
            new_count = 1
            
            await network_monitor._handle_client_count_change(old_count, new_count)
            
            # Should broadcast event
            mock_websocket_alert_service.broadcast_alert.assert_called_once()
            
            # Check event history
            assert len(network_monitor.event_history) == 1
            event = network_monitor.event_history[0]
            assert event["event_type"] == NetworkEventType.CLIENT_DISCONNECTED.value
            assert event["event_data"]["client_count"] == 1
            assert event["event_data"]["change"] == 1
    
    @pytest.mark.asyncio
    async def test_handle_fallback_status_change_enabled(self, network_monitor, mock_websocket_alert_service):
        """Test handling fallback status change - enabled"""
        with patch('app.services.network_monitor.websocket_alert_service', mock_websocket_alert_service):
            old_enabled = False
            new_enabled = True
            
            await network_monitor._handle_fallback_status_change(old_enabled, new_enabled)
            
            # Should broadcast event
            mock_websocket_alert_service.broadcast_alert.assert_called_once()
            
            # Check event history
            assert len(network_monitor.event_history) == 1
            event = network_monitor.event_history[0]
            assert event["event_type"] == NetworkEventType.FALLBACK_ENABLED.value
            assert event["event_data"]["fallback_enabled"] is True
    
    @pytest.mark.asyncio
    async def test_handle_fallback_status_change_disabled(self, network_monitor, mock_websocket_alert_service):
        """Test handling fallback status change - disabled"""
        with patch('app.services.network_monitor.websocket_alert_service', mock_websocket_alert_service):
            old_enabled = True
            new_enabled = False
            
            await network_monitor._handle_fallback_status_change(old_enabled, new_enabled)
            
            # Should broadcast event
            mock_websocket_alert_service.broadcast_alert.assert_called_once()
            
            # Check event history
            assert len(network_monitor.event_history) == 1
            event = network_monitor.event_history[0]
            assert event["event_type"] == NetworkEventType.FALLBACK_DISABLED.value
            assert event["event_data"]["fallback_enabled"] is False
    
    @pytest.mark.asyncio
    async def test_handle_performance_alert(self, network_monitor, mock_websocket_alert_service):
        """Test handling performance alert"""
        with patch('app.services.network_monitor.websocket_alert_service', mock_websocket_alert_service):
            alert_type = "high_latency"
            data = {
                "latency_ms": 150.0,
                "threshold_ms": 100.0
            }
            
            await network_monitor._handle_performance_alert(alert_type, data)
            
            # Should broadcast event
            mock_websocket_alert_service.broadcast_alert.assert_called_once()
            
            # Check event history
            assert len(network_monitor.event_history) == 1
            event = network_monitor.event_history[0]
            assert event["event_type"] == NetworkEventType.PERFORMANCE_ALERT.value
            assert event["event_data"]["alert_type"] == alert_type
            assert event["event_data"]["data"] == data
    
    @pytest.mark.asyncio
    async def test_check_performance_metrics_high_latency(self, network_monitor, mock_network_service, mock_websocket_alert_service):
        """Test performance metrics check - high latency"""
        mock_network_service.get_network_status.return_value = {
            "success": True,
            "data": {
                "latency_ms": 150.0,  # Above threshold
                "packet_loss_percent": 1.0,
                "signal_strength": -60
            }
        }
        
        with patch('app.services.network_monitor.network_service', mock_network_service):
            with patch('app.services.network_monitor.websocket_alert_service', mock_websocket_alert_service):
                await network_monitor._check_performance_metrics()
                
                # Should trigger performance alert
                mock_websocket_alert_service.broadcast_alert.assert_called_once()
                
                # Check event history
                assert len(network_monitor.event_history) == 1
                event = network_monitor.event_history[0]
                assert event["event_type"] == NetworkEventType.PERFORMANCE_ALERT.value
                assert event["event_data"]["alert_type"] == "high_latency"
    
    @pytest.mark.asyncio
    async def test_check_performance_metrics_high_packet_loss(self, network_monitor, mock_network_service, mock_websocket_alert_service):
        """Test performance metrics check - high packet loss"""
        mock_network_service.get_network_status.return_value = {
            "success": True,
            "data": {
                "latency_ms": 50.0,
                "packet_loss_percent": 10.0,  # Above threshold
                "signal_strength": -60
            }
        }
        
        with patch('app.services.network_monitor.network_service', mock_network_service):
            with patch('app.services.network_monitor.websocket_alert_service', mock_websocket_alert_service):
                await network_monitor._check_performance_metrics()
                
                # Should trigger performance alert
                mock_websocket_alert_service.broadcast_alert.assert_called_once()
                
                # Check event history
                assert len(network_monitor.event_history) == 1
                event = network_monitor.event_history[0]
                assert event["event_type"] == NetworkEventType.PERFORMANCE_ALERT.value
                assert event["event_data"]["alert_type"] == "high_packet_loss"
    
    @pytest.mark.asyncio
    async def test_check_performance_metrics_weak_signal(self, network_monitor, mock_network_service, mock_websocket_alert_service):
        """Test performance metrics check - weak signal"""
        mock_network_service.get_network_status.return_value = {
            "success": True,
            "data": {
                "latency_ms": 50.0,
                "packet_loss_percent": 1.0,
                "signal_strength": -90  # Below threshold
            }
        }
        
        with patch('app.services.network_monitor.network_service', mock_network_service):
            with patch('app.services.network_monitor.websocket_alert_service', mock_websocket_alert_service):
                await network_monitor._check_performance_metrics()
                
                # Should trigger performance alert
                mock_websocket_alert_service.broadcast_alert.assert_called_once()
                
                # Check event history
                assert len(network_monitor.event_history) == 1
                event = network_monitor.event_history[0]
                assert event["event_type"] == NetworkEventType.PERFORMANCE_ALERT.value
                assert event["event_data"]["alert_type"] == "weak_signal"
    
    @pytest.mark.asyncio
    async def test_get_network_events(self, network_monitor):
        """Test getting network events"""
        # Add some events
        network_monitor.event_history = [
            {"event_type": "status_changed", "timestamp": "2025-01-28T10:00:00Z"},
            {"event_type": "ap_started", "timestamp": "2025-01-28T10:01:00Z"},
            {"event_type": "client_connected", "timestamp": "2025-01-28T10:02:00Z"}
        ]
        
        # Get all events
        events = await network_monitor.get_network_events()
        assert len(events) == 3
        
        # Get limited events
        events = await network_monitor.get_network_events(limit=2)
        assert len(events) == 2
        assert events[0]["event_type"] == "ap_started"
        assert events[1]["event_type"] == "client_connected"
    
    @pytest.mark.asyncio
    async def test_get_monitoring_status(self, network_monitor):
        """Test getting monitoring status"""
        network_monitor.is_running = True
        network_monitor.last_status = NetworkStatus.CONNECTED
        network_monitor.last_ap_status = WiFiAPStatus.RUNNING
        network_monitor.last_client_count = 2
        network_monitor.last_fallback_enabled = True
        network_monitor.event_history = [{"event": "test"}]
        
        status = await network_monitor.get_monitoring_status()
        
        assert status["is_running"] is True
        assert status["last_status"] == "connected"
        assert status["last_ap_status"] == "running"
        assert status["last_client_count"] == 2
        assert status["last_fallback_enabled"] is True
        assert status["event_count"] == 1
        assert "status_check_interval" in status
        assert "performance_check_interval" in status
        assert "latency_threshold_ms" in status
        assert "packet_loss_threshold_percent" in status
        assert "signal_strength_threshold_dbm" in status
    
    @pytest.mark.asyncio
    async def test_event_history_limit(self, network_monitor, mock_websocket_alert_service):
        """Test event history limit"""
        with patch('app.services.network_monitor.websocket_alert_service', mock_websocket_alert_service):
            # Set max history to 3
            network_monitor.max_event_history = 3
            
            # Add 5 events
            for i in range(5):
                await network_monitor._handle_performance_alert(f"alert_{i}", {"data": i})
            
            # Should only keep last 3 events
            assert len(network_monitor.event_history) == 3
            assert network_monitor.event_history[0]["event_data"]["data"] == 2
            assert network_monitor.event_history[1]["event_data"]["data"] == 3
            assert network_monitor.event_history[2]["event_data"]["data"] == 4
