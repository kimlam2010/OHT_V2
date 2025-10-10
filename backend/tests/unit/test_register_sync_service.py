"""
Unit tests for RegisterSyncService
"""

import pytest
from unittest.mock import AsyncMock, MagicMock, patch
from sqlalchemy.ext.asyncio import AsyncSession

from app.services.register_sync_service import register_sync_service, RegisterSyncService
from app.models.register import Register, RegisterMode, RegisterAccessLevel


@pytest.fixture
def mock_db():
    """Mock database session"""
    db = AsyncMock(spec=AsyncSession)
    db.commit = AsyncMock()
    db.rollback = AsyncMock()
    db.add = MagicMock()
    return db


@pytest.fixture
def mock_firmware_response_success():
    """Mock successful firmware response"""
    return {
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
                        "data_type": "UINT16",
                        "description": "Battery Pack Voltage",
                        "unit": "V"
                    },
                    {
                        "address": "0x0001",
                        "name": "Battery Current",
                        "mode": "READ",
                        "data_type": "INT16",
                        "description": "Battery Current",
                        "unit": "A"
                    },
                    {
                        "address": "0x0049",
                        "name": "12V Relay State",
                        "mode": "READ_WRITE",
                        "data_type": "UINT8",
                        "description": "12V Relay State"
                    }
                ]
            }
        },
        "error": None
    }


@pytest.fixture
def mock_firmware_response_error():
    """Mock failed firmware response"""
    return {
        "success": False,
        "data": None,
        "error": "Firmware communication failed"
    }


@pytest.fixture
def mock_online_modules():
    """Mock online modules response"""
    return {
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


@pytest.mark.asyncio
async def test_sync_module_registers_success(mock_db, mock_firmware_response_success):
    """Test syncing module registers successfully (all new)"""
    # Mock firmware service
    with patch('app.services.register_sync_service.get_firmware_service') as mock_fw:
        mock_fw_instance = AsyncMock()
        mock_fw_instance.get_module_registers = AsyncMock(
            return_value=type('obj', (object,), mock_firmware_response_success)
        )
        mock_fw.return_value = mock_fw_instance
        
        # Mock database query (no existing registers)
        mock_result = MagicMock()
        mock_result.scalar_one_or_none = MagicMock(return_value=None)
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        # Execute sync
        result = await register_sync_service.sync_module_registers(mock_db, 2)
        
        # Assertions
        assert result["success"] is True
        assert result["module_addr"] == 2
        assert result["synced"] == 3
        assert result["created"] == 3
        assert result["updated"] == 0
        assert result["failed"] == 0
        assert len(result["errors"]) == 0
        
        # Verify database operations
        assert mock_db.add.call_count == 3  # 3 new registers
        mock_db.commit.assert_called_once()


@pytest.mark.asyncio
async def test_sync_module_registers_with_updates(mock_db, mock_firmware_response_success):
    """Test syncing module registers with some existing (updates)"""
    # Mock firmware service
    with patch('app.services.register_sync_service.get_firmware_service') as mock_fw:
        mock_fw_instance = AsyncMock()
        mock_fw_instance.get_module_registers = AsyncMock(
            return_value=type('obj', (object,), mock_firmware_response_success)
        )
        mock_fw.return_value = mock_fw_instance
        
        # Mock database query - first register exists, others don't
        call_count = [0]
        def mock_execute_func(*args, **kwargs):
            result = MagicMock()
            if call_count[0] == 0:
                # First call - register exists
                existing_reg = Register(
                    id=1,
                    name="Old Name",
                    address="0x0000",
                    mode=RegisterMode.READ,
                    access_level=RegisterAccessLevel.USER
                )
                result.scalar_one_or_none = MagicMock(return_value=existing_reg)
            else:
                # Other calls - no register
                result.scalar_one_or_none = MagicMock(return_value=None)
            call_count[0] += 1
            return result
        
        mock_db.execute = AsyncMock(side_effect=mock_execute_func)
        
        # Execute sync
        result = await register_sync_service.sync_module_registers(mock_db, 2)
        
        # Assertions
        assert result["success"] is True
        assert result["synced"] == 3
        assert result["created"] == 2
        assert result["updated"] == 1
        assert result["failed"] == 0


@pytest.mark.asyncio
async def test_sync_module_registers_firmware_error(mock_db, mock_firmware_response_error):
    """Test handling firmware communication errors"""
    with patch('app.services.register_sync_service.get_firmware_service') as mock_fw:
        mock_fw_instance = AsyncMock()
        mock_fw_instance.get_module_registers = AsyncMock(
            return_value=type('obj', (object,), mock_firmware_response_error)
        )
        mock_fw.return_value = mock_fw_instance
        
        # Execute sync
        result = await register_sync_service.sync_module_registers(mock_db, 2)
        
        # Assertions
        assert result["success"] is False
        assert result["module_addr"] == 2
        assert "error" in result
        assert result["error"] == "Firmware communication failed"


@pytest.mark.asyncio
async def test_sync_module_registers_database_commit_error(mock_db, mock_firmware_response_success):
    """Test handling database commit errors"""
    with patch('app.services.register_sync_service.get_firmware_service') as mock_fw:
        mock_fw_instance = AsyncMock()
        mock_fw_instance.get_module_registers = AsyncMock(
            return_value=type('obj', (object,), mock_firmware_response_success)
        )
        mock_fw.return_value = mock_fw_instance
        
        # Mock database query (no existing registers)
        mock_result = MagicMock()
        mock_result.scalar_one_or_none = MagicMock(return_value=None)
        mock_db.execute = AsyncMock(return_value=mock_result)
        
        # Mock commit to raise error
        mock_db.commit = AsyncMock(side_effect=Exception("Database error"))
        
        # Execute sync
        result = await register_sync_service.sync_module_registers(mock_db, 2)
        
        # Assertions
        assert result["success"] is False
        assert "Database commit failed" in result["error"]
        mock_db.rollback.assert_called_once()


@pytest.mark.asyncio
async def test_sync_all_modules_success(mock_db, mock_online_modules):
    """Test syncing all modules successfully"""
    with patch('app.services.register_sync_service.get_firmware_service') as mock_fw:
        mock_fw_instance = AsyncMock()
        mock_fw_instance.get_online_modules = AsyncMock(
            return_value=type('obj', (object,), mock_online_modules)
        )
        mock_fw.return_value = mock_fw_instance
        
        # Mock sync_module_registers
        with patch.object(register_sync_service, 'sync_module_registers') as mock_sync:
            mock_sync.side_effect = [
                {
                    "success": True,
                    "module_addr": 2,
                    "synced": 58,
                    "created": 50,
                    "updated": 8,
                    "failed": 0,
                    "errors": []
                },
                {
                    "success": True,
                    "module_addr": 3,
                    "synced": 30,
                    "created": 25,
                    "updated": 5,
                    "failed": 0,
                    "errors": []
                }
            ]
            
            # Execute sync
            result = await register_sync_service.sync_all_modules(mock_db)
            
            # Assertions
            assert result["success"] is True
            assert result["total_synced"] == 88
            assert result["total_created"] == 75
            assert result["total_updated"] == 13
            assert len(result["failed_modules"]) == 0
            assert len(result["results"]) == 2


@pytest.mark.asyncio
async def test_sync_all_modules_no_online_modules(mock_db):
    """Test sync all modules when no modules online"""
    with patch('app.services.register_sync_service.get_firmware_service') as mock_fw:
        mock_fw_instance = AsyncMock()
        mock_fw_instance.get_online_modules = AsyncMock(
            return_value=type('obj', (object,), {
                "success": True,
                "data": {"data": {"modules": [], "count": 0}},
                "error": None
            })
        )
        mock_fw.return_value = mock_fw_instance
        
        # Execute sync
        result = await register_sync_service.sync_all_modules(mock_db)
        
        # Assertions
        assert result["success"] is False
        assert "No online modules found" in result["error"]


@pytest.mark.asyncio
async def test_sync_all_modules_partial_failure(mock_db, mock_online_modules):
    """Test sync all modules with partial failures"""
    with patch('app.services.register_sync_service.get_firmware_service') as mock_fw:
        mock_fw_instance = AsyncMock()
        mock_fw_instance.get_online_modules = AsyncMock(
            return_value=type('obj', (object,), mock_online_modules)
        )
        mock_fw.return_value = mock_fw_instance
        
        # Mock sync_module_registers - one succeeds, one fails
        with patch.object(register_sync_service, 'sync_module_registers') as mock_sync:
            mock_sync.side_effect = [
                {
                    "success": True,
                    "module_addr": 2,
                    "synced": 58,
                    "created": 58,
                    "updated": 0,
                    "failed": 0,
                    "errors": []
                },
                {
                    "success": False,
                    "module_addr": 3,
                    "error": "Firmware timeout"
                }
            ]
            
            # Execute sync
            result = await register_sync_service.sync_all_modules(mock_db)
            
            # Assertions
            assert result["success"] is False  # Overall failure due to one failure
            assert result["total_synced"] == 58  # Only successful module counted
            assert len(result["failed_modules"]) == 1
            assert result["failed_modules"][0]["address"] == 3


def test_map_mode():
    """Test mode mapping from Firmware to Backend"""
    service = RegisterSyncService()
    
    assert service._map_mode("READ") == RegisterMode.READ
    assert service._map_mode("WRITE") == RegisterMode.WRITE
    assert service._map_mode("READ_WRITE") == RegisterMode.READ_WRITE
    assert service._map_mode("read") == RegisterMode.READ  # Case insensitive
    assert service._map_mode("unknown") == RegisterMode.READ  # Default

