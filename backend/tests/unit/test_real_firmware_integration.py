"""
Comprehensive tests for RealFirmwareIntegrationService

This test suite covers:
- HTTP API communication methods
- Retry mechanisms và fallback strategies
- Error handling và recovery
- Health monitoring
- Command validation
- Service lifecycle management
"""

import pytest
from unittest.mock import AsyncMock, patch
from datetime import datetime

from app.services.real_firmware_integration import (
    RealFirmwareIntegrationService,
)

from types import SimpleNamespace

class DummyResp:
    def __init__(self, status_code: int, json_data=None):
        self.status_code = status_code
        self._json_data = json_data
    def json(self):
        return self._json_data

    @property
    def text(self):
        return ""


class TestRealFirmwareIntegrationService:
    """Test suite for RealFirmwareIntegrationService"""
    
    @pytest.fixture
    async def service(self):
        """Create service instance for testing with all deps patched at usage site"""
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock_retry, \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager') as mock_fallback:
            # Ensure async close
            mock_client.return_value.aclose = AsyncMock()
            service = RealFirmwareIntegrationService("http://test-firmware:8081")
            yield service
            await service.close()
        
    @pytest.fixture
    def mock_http_client(self):
        """Mock HTTP client at usage site"""
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client:
            mock_client.return_value = AsyncMock()
            mock_client.return_value.aclose = AsyncMock()
            yield mock_client.return_value
            
    @pytest.fixture
    def mock_health_checker(self):
        """Mock health checker at usage site"""
        with patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock:
            instance = AsyncMock()
            mock.return_value = instance
            yield instance
            
    @pytest.fixture
    def mock_retry_manager(self):
        """Mock retry manager at usage site"""
        with patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock:
            instance = AsyncMock()
            mock.return_value = instance
            yield instance
            
    @pytest.fixture
    def mock_fallback_manager(self):
        """Mock fallback manager at usage site"""
        with patch('app.services.real_firmware_integration.FirmwareFallbackManager') as mock:
            instance = AsyncMock()
            mock.return_value = instance
            yield instance

    @pytest.mark.asyncio
    async def test_context_manager(self, mock_http_client):
        """Test async context manager"""
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker'), \
             patch('app.services.real_firmware_integration.FirmwareRetryManager'), \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager'):
            mock_client.return_value.aclose = AsyncMock()
            async with RealFirmwareIntegrationService("http://test:8081") as service:
                assert isinstance(service, RealFirmwareIntegrationService)
            mock_client.return_value.aclose.assert_called_once()

    @pytest.mark.asyncio
    async def test_close_method(self):
        """Test close method"""
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker'), \
             patch('app.services.real_firmware_integration.FirmwareRetryManager'), \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager'):
            mock_client.return_value.aclose = AsyncMock()
            service = RealFirmwareIntegrationService("http://test:8081")
            await service.close()
            mock_client.return_value.aclose.assert_called_once()

    @pytest.mark.asyncio
    async def test_check_health_success(self):
        """Test successful health check"""
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager'), \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager'):
            mock_client.return_value.aclose = AsyncMock()
            mock_health_instance = AsyncMock()
            mock_health_instance.check_health.return_value = True
            mock_health.return_value = mock_health_instance
            service = RealFirmwareIntegrationService("http://test:8081")
            result = await service.check_health()
            assert result is True
            assert service.health_status is True
            assert service.connection_errors == 0
            assert service.last_health_check is not None
            mock_health_instance.check_health.assert_called_once()

    @pytest.mark.asyncio
    async def test_check_health_failure(self):
        """Test failed health check"""
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager'), \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager'):
            mock_client.return_value.aclose = AsyncMock()
            mock_health_instance = AsyncMock()
            mock_health_instance.check_health.return_value = False
            mock_health.return_value = mock_health_instance
            service = RealFirmwareIntegrationService("http://test:8081")
            result = await service.check_health()
            assert result is False
            assert service.health_status is False
            mock_health_instance.check_health.assert_called_once()

    @pytest.mark.asyncio
    async def test_check_health_exception(self):
        """Test health check with exception"""
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager'), \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager'):
            mock_client.return_value.aclose = AsyncMock()
            mock_health_instance = AsyncMock()
            mock_health_instance.check_health.side_effect = Exception("Connection failed")
            mock_health.return_value = mock_health_instance
            service = RealFirmwareIntegrationService("http://test:8081")
            result = await service.check_health()
            assert result is False
            assert service.health_status is False

    @pytest.mark.asyncio
    async def test_get_service_health_flags(self, service):
        """Basic service health dict fields"""
        service.health_status = True
        service.last_health_check = datetime.utcnow()
        service.connection_errors = 2
        health = await service.get_service_health()
        assert health["service"] == "RealFirmwareIntegrationService"
        assert health["fallback_mode"] is False

    @pytest.mark.asyncio
    async def test_get_robot_status_healthy(self):
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock_retry, \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager') as mock_fallback:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock()
            mock_health.return_value.check_health.return_value = True
            mock_retry.return_value = AsyncMock()
            mock_retry.return_value.execute_with_retry = AsyncMock(return_value=DummyResp(200, {"status":"idle","battery":87}))
            service = RealFirmwareIntegrationService("http://fw:8081")
            data = await service.get_robot_status()
            assert data["status"] == "idle" and data["battery"] == 87

    @pytest.mark.asyncio
    async def test_get_robot_status_unhealthy_fallback(self):
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager') as mock_fallback:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock(); mock_health.return_value.check_health.return_value = False
            inst = AsyncMock(); inst.get_fallback_status = AsyncMock(return_value={"status":"fallback"})
            mock_fallback.return_value = inst
            service = RealFirmwareIntegrationService("http://fw:8081")
            data = await service.get_robot_status()
            assert data["status"] == "fallback"

    @pytest.mark.asyncio
    async def test_get_robot_status_http_error_fallback(self):
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock_retry, \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager') as mock_fallback:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock(); mock_health.return_value.check_health.return_value = True
            mock_retry.return_value = AsyncMock(); mock_retry.return_value.execute_with_retry = AsyncMock(return_value=DummyResp(500))
            inst = AsyncMock(); inst.get_fallback_status = AsyncMock(return_value={"status":"fallback"})
            mock_fallback.return_value = inst
            service = RealFirmwareIntegrationService("http://fw:8081")
            data = await service.get_robot_status()
            assert data["status"] == "fallback"

    @pytest.mark.asyncio
    async def test_get_robot_status_exception_increments_errors(self):
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock_retry, \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager') as mock_fallback:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock(); mock_health.return_value.check_health.return_value = True
            mock_retry.return_value = AsyncMock(); mock_retry.return_value.execute_with_retry = AsyncMock(side_effect=Exception("net"))
            inst = AsyncMock(); inst.get_fallback_status = AsyncMock(return_value={"status":"fallback"})
            mock_fallback.return_value = inst
            service = RealFirmwareIntegrationService("http://fw:8081")
            before = service.connection_errors
            data = await service.get_robot_status()
            assert data["status"] == "fallback" and service.connection_errors == before + 1

    @pytest.mark.asyncio
    async def test_get_telemetry_success_and_fallback(self):
        # success path
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock_retry:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock(); mock_health.return_value.check_health.return_value = True
            mock_retry.return_value = AsyncMock(); mock_retry.return_value.execute_with_retry = AsyncMock(return_value=DummyResp(200, {"temperature":41}))
            s = RealFirmwareIntegrationService("http://fw"); d = await s.get_telemetry_data()
            assert d["temperature"] == 41
        # fallback path when unhealthy
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager') as mock_fallback:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock(); mock_health.return_value.check_health.return_value = False
            inst = AsyncMock(); inst.get_fallback_telemetry = AsyncMock(return_value={"t":0})
            mock_fallback.return_value = inst
            s = RealFirmwareIntegrationService("http://fw"); d = await s.get_telemetry_data(); assert "t" in d

    @pytest.mark.asyncio
    async def test_send_robot_command_paths(self):
        # success
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock_retry:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock(); mock_health.return_value.check_health.return_value = True
            mock_retry.return_value = AsyncMock(); mock_retry.return_value.execute_with_retry = AsyncMock(return_value=DummyResp(200, {"success":True}))
            s = RealFirmwareIntegrationService("http://fw"); assert await s.send_robot_command({"command_type":"move","parameters":{}}) is True
        # failure response
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock_retry:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock(); mock_health.return_value.check_health.return_value = True
            mock_retry.return_value = AsyncMock(); mock_retry.return_value.execute_with_retry = AsyncMock(return_value=DummyResp(200, {"success":False}))
            s = RealFirmwareIntegrationService("http://fw"); assert await s.send_robot_command({"command_type":"move","parameters":{}}) is False
        # http error
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock_retry:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock(); mock_health.return_value.check_health.return_value = True
            mock_retry.return_value = AsyncMock(); mock_retry.return_value.execute_with_retry = AsyncMock(return_value=DummyResp(500))
            s = RealFirmwareIntegrationService("http://fw"); assert await s.send_robot_command({"command_type":"move","parameters":{}}) is False
        # exception increments errors
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock_retry:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock(); mock_health.return_value.check_health.return_value = True
            mock_retry.return_value = AsyncMock(); mock_retry.return_value.execute_with_retry = AsyncMock(side_effect=Exception("boom"))
            s = RealFirmwareIntegrationService("http://fw"); before = s.connection_errors
            ok = await s.send_robot_command({"command_type":"move","parameters":{}})
            assert ok is False and s.connection_errors == before + 1
        # invalid command
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client:
            mock_client.return_value.aclose = AsyncMock()
            s = RealFirmwareIntegrationService("http://fw")
            res = await s.send_robot_command({"invalid":"x"})
            assert res is False

    @pytest.mark.asyncio
    async def test_info_and_metrics_paths(self):
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client:
            cli = mock_client.return_value
            cli.aclose = AsyncMock()
            # firmware info ok
            cli.get = AsyncMock(return_value=DummyResp(200, {"version":"1.0.0","status":"active"}))
            s = RealFirmwareIntegrationService("http://fw"); info = await s.get_firmware_info(); assert info["version"] == "1.0.0"
            # firmware info http error
            cli.get = AsyncMock(return_value=DummyResp(500))
            s = RealFirmwareIntegrationService("http://fw"); info = await s.get_firmware_info(); assert info["status"] == "unavailable"
            # firmware info exception
            cli.get = AsyncMock(side_effect=Exception("down")); s = RealFirmwareIntegrationService("http://fw"); info = await s.get_firmware_info(); assert info["status"] == "error"
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client:
            cli = mock_client.return_value; cli.aclose = AsyncMock()
            # system metrics ok
            cli.get = AsyncMock(return_value=DummyResp(200, {"cpu":50}))
            s = RealFirmwareIntegrationService("http://fw"); m = await s.get_system_metrics(); assert m["cpu"] == 50
            # http error
            cli.get = AsyncMock(return_value=DummyResp(500)); s = RealFirmwareIntegrationService("http://fw"); m = await s.get_system_metrics(); assert m == {}
            # exception
            cli.get = AsyncMock(side_effect=Exception("err")); s = RealFirmwareIntegrationService("http://fw"); m = await s.get_system_metrics(); assert m == {}

    @pytest.mark.asyncio
    async def test_private_helpers(self):
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client:
            cli = mock_client.return_value; cli.aclose = AsyncMock()
            status_resp = DummyResp(200, {})
            cli.get = AsyncMock(return_value=status_resp)
            s = RealFirmwareIntegrationService("http://fw"); r = await s._make_status_request(); assert r == status_resp
            cmd_resp = DummyResp(200, {})
            cli.post = AsyncMock(return_value=cmd_resp)
            r = await s._make_command_request({}); assert r == cmd_resp
            tel_resp = DummyResp(200, {})
            cli.get = AsyncMock(return_value=tel_resp)
            r = await s._make_telemetry_request(); assert r == tel_resp

    @pytest.mark.asyncio
    async def test_fallback_helpers(self):
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager') as mock_fallback:
            mock_client.return_value.aclose = AsyncMock()
            inst = AsyncMock(); inst.get_fallback_status = AsyncMock(return_value={"status":"fallback"}); inst.get_fallback_telemetry = AsyncMock(return_value={"t":0})
            mock_fallback.return_value = inst
            s = RealFirmwareIntegrationService("http://fw")
            st = await s._get_fallback_status(); tl = await s._get_fallback_telemetry()
            assert st["status"] == "fallback" and tl["t"] == 0

    @pytest.mark.asyncio
    async def test_max_connection_errors_triggers_fallback_critical(self):
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock_retry, \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager') as mock_fallback:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock(); mock_health.return_value.check_health.return_value = True
            mock_retry.return_value = AsyncMock(); mock_retry.return_value.execute_with_retry = AsyncMock(side_effect=Exception("net"))
            inst = AsyncMock(); inst.get_fallback_status = AsyncMock(return_value={"status":"fallback"})
            mock_fallback.return_value = inst
            s = RealFirmwareIntegrationService("http://fw")
            s.connection_errors = s.max_connection_errors  # trigger critical branch
            data = await s.get_robot_status()
            assert data["status"] == "fallback"

    @pytest.mark.asyncio
    async def test_send_command_unhealthy_but_success(self):
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock_retry:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock(); mock_health.return_value.check_health.return_value = False
            mock_retry.return_value = AsyncMock(); mock_retry.return_value.execute_with_retry = AsyncMock(return_value=DummyResp(200, {"success": True}))
            s = RealFirmwareIntegrationService("http://fw")
            ok = await s.send_robot_command({"command_type":"move","parameters":{}})
            assert ok is True

    @pytest.mark.asyncio
    async def test_get_telemetry_exception_fallback(self):
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health, \
             patch('app.services.real_firmware_integration.FirmwareRetryManager') as mock_retry, \
             patch('app.services.real_firmware_integration.FirmwareFallbackManager') as mock_fallback:
            mock_client.return_value.aclose = AsyncMock()
            mock_health.return_value = AsyncMock(); mock_health.return_value.check_health.return_value = True
            mock_retry.return_value = AsyncMock(); mock_retry.return_value.execute_with_retry = AsyncMock(side_effect=Exception("boom"))
            inst = AsyncMock(); inst.get_fallback_telemetry = AsyncMock(return_value={"t":0})
            mock_fallback.return_value = inst
            s = RealFirmwareIntegrationService("http://fw")
            data = await s.get_telemetry_data()
            assert data["t"] == 0

    @pytest.mark.asyncio
    async def test_invalid_command_type_returns_false(self):
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client:
            mock_client.return_value.aclose = AsyncMock()
            s = RealFirmwareIntegrationService("http://fw")
            ok = await s.send_robot_command({"command_type":"INVALID","parameters":{}})
            assert ok is False

    @pytest.mark.asyncio
    async def test_force_health_check_calls_checker(self):
        with patch('app.services.real_firmware_integration.httpx.AsyncClient') as mock_client, \
             patch('app.services.real_firmware_integration.FirmwareHealthChecker') as mock_health:
            mock_client.return_value.aclose = AsyncMock()
            inst = AsyncMock(); inst.check_health.return_value = True
            mock_health.return_value = inst
            s = RealFirmwareIntegrationService("http://fw")
            result = await s.force_health_check()
            assert result is True
            inst.check_health.assert_awaited()
