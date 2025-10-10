"""
Integration tests for Admin Register Sync API and Query Filters
"""

import pytest
from httpx import AsyncClient
from unittest.mock import patch, AsyncMock


@pytest.mark.asyncio
async def test_sync_endpoint_admin_single_module(client: AsyncClient, admin_auth_headers: dict):
    """Test admin sync endpoint with valid auth for single module"""
    # Mock firmware service responses
    mock_modules_response = {
        "success": True,
        "data": {
            "data": {
                "module_addr": 2,
                "module_name": "Power Module",
                "register_count": 3,
                "registers": [
                    {
                        "address": "0x0000",
                        "name": "Battery Pack Voltage",
                        "mode": "READ",
                        "description": "Battery voltage"
                    },
                    {
                        "address": "0x0001",
                        "name": "Battery Current",
                        "mode": "READ",
                        "description": "Battery current"
                    },
                    {
                        "address": "0x0049",
                        "name": "12V Relay",
                        "mode": "READ_WRITE",
                        "description": "12V relay control"
                    }
                ]
            }
        },
        "error": None
    }
    
    with patch('app.services.register_sync_service.get_firmware_service') as mock_fw:
        mock_fw_instance = AsyncMock()
        mock_fw_instance.get_module_registers = AsyncMock(
            return_value=type('obj', (object,), mock_modules_response)
        )
        mock_fw.return_value = mock_fw_instance
        
        # Make request
        response = await client.post(
            "/api/v1/admin/registers/sync-from-firmware",
            json={"module_addr": 2},
            headers=admin_auth_headers
        )
        
        # Assertions
        assert response.status_code == 200
        data = response.json()
        assert data["success"] is True
        assert data["total_synced"] == 3
        assert data["total_created"] >= 0
        assert "message" in data


@pytest.mark.asyncio
async def test_sync_endpoint_admin_all_modules(client: AsyncClient, admin_auth_headers: dict):
    """Test admin sync endpoint for all modules"""
    # Mock firmware service responses
    mock_online_modules = {
        "success": True,
        "data": {
            "data": {
                "modules": [
                    {"address": 2, "type": "POWER", "online": True},
                    {"address": 3, "type": "SAFETY", "online": True}
                ],
                "count": 2
            }
        },
        "error": None
    }
    
    mock_module_registers = {
        "success": True,
        "data": {
            "data": {
                "module_addr": 2,
                "module_name": "Power Module",
                "register_count": 2,
                "registers": [
                    {"address": "0x0000", "name": "Voltage", "mode": "READ"},
                    {"address": "0x0001", "name": "Current", "mode": "READ"}
                ]
            }
        },
        "error": None
    }
    
    with patch('app.services.register_sync_service.get_firmware_service') as mock_fw:
        mock_fw_instance = AsyncMock()
        mock_fw_instance.get_online_modules = AsyncMock(
            return_value=type('obj', (object,), mock_online_modules)
        )
        mock_fw_instance.get_module_registers = AsyncMock(
            return_value=type('obj', (object,), mock_module_registers)
        )
        mock_fw.return_value = mock_fw_instance
        
        # Make request (no module_addr = sync all)
        response = await client.post(
            "/api/v1/admin/registers/sync-from-firmware",
            json={},
            headers=admin_auth_headers
        )
        
        # Assertions
        assert response.status_code == 200
        data = response.json()
        assert data["success"] is True
        assert "total_synced" in data
        assert "message" in data


@pytest.mark.asyncio
async def test_sync_endpoint_invalid_module(client: AsyncClient, admin_auth_headers: dict):
    """Test sync endpoint with invalid module address"""
    response = await client.post(
        "/api/v1/admin/registers/sync-from-firmware",
        json={"module_addr": 99},  # Invalid module
        headers=admin_auth_headers
    )
    
    assert response.status_code == 400
    data = response.json()
    assert "Invalid module_addr" in data["detail"]


@pytest.mark.asyncio
async def test_sync_endpoint_permission_denied(client: AsyncClient):
    """Test sync requires admin permission"""
    # Try without auth
    response = await client.post(
        "/api/v1/admin/registers/sync-from-firmware",
        json={"module_addr": 2}
    )
    
    # Should be unauthorized or forbidden
    assert response.status_code in [401, 403]


@pytest.mark.asyncio
async def test_query_registers_no_filter(client: AsyncClient, auth_headers: dict):
    """Test querying all registers without filters"""
    response = await client.get(
        "/api/v1/registers",
        headers=auth_headers
    )
    
    assert response.status_code == 200
    data = response.json()
    assert isinstance(data, list)


@pytest.mark.asyncio
async def test_query_registers_mode_filter(client: AsyncClient, auth_headers: dict, admin_auth_headers: dict):
    """Test querying registers with mode filter"""
    # First, populate some registers
    with patch('app.services.register_sync_service.get_firmware_service') as mock_fw:
        mock_fw_instance = AsyncMock()
        mock_fw_instance.get_module_registers = AsyncMock(
            return_value=type('obj', (object,), {
                "success": True,
                "data": {
                    "data": {
                        "registers": [
                            {"address": "0x0000", "name": "Read Register", "mode": "READ"},
                            {"address": "0x0001", "name": "Write Register", "mode": "WRITE"}
                        ]
                    }
                },
                "error": None
            })
        )
        mock_fw.return_value = mock_fw_instance
        
        # Sync first
        await client.post(
            "/api/v1/admin/registers/sync-from-firmware",
            json={"module_addr": 2},
            headers=admin_auth_headers
        )
    
    # Query with READ filter
    response = await client.get(
        "/api/v1/registers?mode=READ",
        headers=auth_headers
    )
    
    assert response.status_code == 200
    data = response.json()
    assert isinstance(data, list)
    # All returned registers should be READ mode
    for reg in data:
        assert reg["mode"] == "READ"


@pytest.mark.asyncio
async def test_query_registers_access_level_filter(client: AsyncClient, auth_headers: dict):
    """Test querying registers with access_level filter"""
    response = await client.get(
        "/api/v1/registers?access_level=USER",
        headers=auth_headers
    )
    
    assert response.status_code == 200
    data = response.json()
    assert isinstance(data, list)
    # All returned registers should be USER access level
    for reg in data:
        assert reg["access_level"] == "USER"


@pytest.mark.asyncio
async def test_query_registers_combined_filters(client: AsyncClient, auth_headers: dict):
    """Test querying registers with combined filters"""
    response = await client.get(
        "/api/v1/registers?mode=READ&access_level=USER",
        headers=auth_headers
    )
    
    assert response.status_code == 200
    data = response.json()
    assert isinstance(data, list)
    # All returned registers should match both filters
    for reg in data:
        assert reg["mode"] == "READ"
        assert reg["access_level"] == "USER"


@pytest.mark.asyncio
async def test_get_register_by_address(client: AsyncClient, auth_headers: dict, admin_auth_headers: dict):
    """Test getting register by address"""
    # First, populate a register
    with patch('app.services.register_sync_service.get_firmware_service') as mock_fw:
        mock_fw_instance = AsyncMock()
        mock_fw_instance.get_module_registers = AsyncMock(
            return_value=type('obj', (object,), {
                "success": True,
                "data": {
                    "data": {
                        "registers": [
                            {"address": "0x0000", "name": "Test Register", "mode": "READ"}
                        ]
                    }
                },
                "error": None
            })
        )
        mock_fw.return_value = mock_fw_instance
        
        # Sync first
        await client.post(
            "/api/v1/admin/registers/sync-from-firmware",
            json={"module_addr": 2},
            headers=admin_auth_headers
        )
    
    # Query by address
    response = await client.get(
        "/api/v1/registers/by-address/0x0000",
        headers=auth_headers
    )
    
    assert response.status_code == 200
    data = response.json()
    assert data["address"] == "0x0000"
    assert data["name"] == "Test Register"


@pytest.mark.asyncio
async def test_get_register_by_address_not_found(client: AsyncClient, auth_headers: dict):
    """Test getting register by non-existent address"""
    response = await client.get(
        "/api/v1/registers/by-address/0xFFFF",
        headers=auth_headers
    )
    
    assert response.status_code == 404
    data = response.json()
    assert "not found" in data["detail"].lower()


@pytest.mark.asyncio
async def test_sync_endpoint_firmware_connection_error(client: AsyncClient, admin_auth_headers: dict):
    """Test sync endpoint when firmware is unreachable"""
    with patch('app.services.register_sync_service.get_firmware_service') as mock_fw:
        mock_fw_instance = AsyncMock()
        mock_fw_instance.get_module_registers = AsyncMock(
            return_value=type('obj', (object,), {
                "success": False,
                "data": None,
                "error": "Connection refused"
            })
        )
        mock_fw.return_value = mock_fw_instance
        
        # Make request
        response = await client.post(
            "/api/v1/admin/registers/sync-from-firmware",
            json={"module_addr": 2},
            headers=admin_auth_headers
        )
        
        # Should return error
        assert response.status_code == 500
        data = response.json()
        assert "error" in data["detail"].lower() or "failed" in data["detail"].lower()

