"""
Integration tests for RS485 communication
"""

import pytest
import asyncio
from app.core.integration import RS485Service, FirmwareIntegrationService
from app.core.exceptions import RS485CommunicationException


class TestRS485Integration:
    """Test RS485 integration with mock data"""
    
    @pytest.fixture
    async def rs485_service(self):
        """Create RS485 service with mock data"""
        service = RS485Service(use_mock=True)
        yield service
        await service.disconnect()
    
    @pytest.fixture
    async def firmware_service(self):
        """Create firmware integration service with mock data"""
        service = FirmwareIntegrationService(use_mock=True)
        yield service
        await service.rs485_service.disconnect()
    
    @pytest.mark.asyncio
    async def test_rs485_connection(self, rs485_service):
        """Test RS485 connection"""
        connected = await rs485_service.connect()
        assert connected is True
        assert rs485_service.connected is True
    
    @pytest.mark.asyncio
    async def test_read_motor_register(self, rs485_service):
        """Test reading motor module registers"""
        await rs485_service.connect()
        
        # Read motor status
        status = await rs485_service.read_register(1, 30001)
        assert status == 1  # Running
        
        # Read motor speed
        speed = await rs485_service.read_register(1, 30002)
        assert speed == 1500  # 1500 RPM
        
        # Read motor temperature
        temp = await rs485_service.read_register(1, 30005)
        assert temp == 45  # 45°C
    
    @pytest.mark.asyncio
    async def test_read_dock_register(self, rs485_service):
        """Test reading dock module registers"""
        await rs485_service.connect()
        
        # Read dock status
        status = await rs485_service.read_register(2, 30001)
        assert status == 0  # Idle
        
        # Read dock position
        pos_x = await rs485_service.read_register(2, 30002)
        assert pos_x == 150  # 150mm
        
        pos_y = await rs485_service.read_register(2, 30003)
        assert pos_y == 200  # 200mm
        
        # Read NFC tag
        nfc_tag = await rs485_service.read_register(2, 30007)
        assert nfc_tag == 12345
    
    @pytest.mark.asyncio
    async def test_read_safety_register(self, rs485_service):
        """Test reading safety module registers"""
        await rs485_service.connect()
        
        # Read safety status
        status = await rs485_service.read_register(3, 30001)
        assert status == 0  # Safe
        
        # Read emergency stop status
        estop = await rs485_service.read_register(3, 30002)
        assert estop == 0  # Released
        
        # Read obstacle detection
        obstacles = await rs485_service.read_register(3, 30006)
        assert obstacles == 0  # No obstacles
    
    @pytest.mark.asyncio
    async def test_write_register(self, rs485_service):
        """Test writing to registers"""
        await rs485_service.connect()
        
        # Write to motor speed register
        success = await rs485_service.write_register(1, 40003, 75)
        assert success is True
        
        # Verify the write
        speed = await rs485_service.read_register(1, 40003)
        assert speed == 75
    
    @pytest.mark.asyncio
    async def test_read_multiple_registers(self, rs485_service):
        """Test reading multiple registers"""
        await rs485_service.connect()
        
        # Read motor status registers
        registers = await rs485_service.read_multiple_registers(1, 30001, 4)
        assert len(registers) == 4
        assert registers[0] == 1  # status
        assert registers[1] == 1500  # speed
        assert registers[2] == 2  # current
        assert registers[3] == 24  # voltage
    
    @pytest.mark.asyncio
    async def test_module_discovery(self, firmware_service):
        """Test module discovery"""
        modules = await firmware_service.discover_modules()
        
        # Should discover 3 modules (addresses 1, 2, 3)
        assert len(modules) == 3
        assert "module_1" in modules
        assert "module_2" in modules
        assert "module_3" in modules
        
        # Check module details
        module_1 = modules["module_1"]
        assert module_1["address"] == 1
        assert module_1["status"] == "online"
    
    @pytest.mark.asyncio
    async def test_motor_status(self, firmware_service):
        """Test motor module status"""
        await firmware_service.discover_modules()
        
        status = await firmware_service.get_module_status("module_1")
        assert status["type"] == "travel_motor"
        assert status["status"] == 1  # Running
        assert status["speed"] == 1500
        assert status["current"] == 2
        assert status["temperature"] == 45
        assert status["online"] is True
    
    @pytest.mark.asyncio
    async def test_dock_status(self, firmware_service):
        """Test dock module status"""
        await firmware_service.discover_modules()
        
        status = await firmware_service.get_module_status("module_2")
        assert status["type"] == "dock_location"
        assert status["status"] == 0  # Idle
        assert status["position_x"] == 150
        assert status["position_y"] == 200
        assert status["nfc_tag"] == 12345
        assert status["online"] is True
    
    @pytest.mark.asyncio
    async def test_safety_status(self, firmware_service):
        """Test safety module status"""
        await firmware_service.discover_modules()
        
        status = await firmware_service.get_module_status("module_3")
        assert status["type"] == "safety_module"
        assert status["status"] == 0  # Safe
        assert status["emergency_stop"] == 0  # Released
        assert status["obstacles_detected"] == 0  # No obstacles
        assert status["online"] is True
    
    @pytest.mark.asyncio
    async def test_motor_command(self, firmware_service):
        """Test motor command execution"""
        await firmware_service.discover_modules()
        
        # Test move command
        command = {
            "type": "move",
            "parameters": {
                "speed": 75,
                "direction": 0
            }
        }
        
        success = await firmware_service.send_command("module_1", command)
        assert success is True
    
    @pytest.mark.asyncio
    async def test_dock_command(self, firmware_service):
        """Test dock command execution"""
        await firmware_service.discover_modules()
        
        # Test dock command
        command = {
            "type": "dock"
        }
        
        success = await firmware_service.send_command("module_2", command)
        assert success is True
    
    @pytest.mark.asyncio
    async def test_safety_command(self, firmware_service):
        """Test safety command execution"""
        await firmware_service.discover_modules()
        
        # Test safety reset command
        command = {
            "type": "reset"
        }
        
        success = await firmware_service.send_command("module_3", command)
        assert success is True
    
    @pytest.mark.asyncio
    async def test_invalid_command(self, firmware_service):
        """Test invalid command handling"""
        await firmware_service.discover_modules()
        
        # Test invalid command type
        command = {
            "type": "invalid_command"
        }
        
        with pytest.raises(RS485CommunicationException, match="Invalid command type"):
            await firmware_service.send_command("module_1", command)
    
    @pytest.mark.asyncio
    async def test_nonexistent_module(self, firmware_service):
        """Test handling of nonexistent module"""
        with pytest.raises(RS485CommunicationException, match="Module module_99 not found"):
            await firmware_service.get_module_status("module_99")
    
    @pytest.mark.asyncio
    async def test_communication_performance(self, rs485_service):
        """Test communication performance"""
        await rs485_service.connect()
        
        start_time = asyncio.get_event_loop().time()
        
        # Perform multiple reads
        for _ in range(10):
            await rs485_service.read_register(1, 30001)
        
        end_time = asyncio.get_event_loop().time()
        duration = end_time - start_time
        
        # Should complete within reasonable time (10 reads * 10ms + overhead)
        assert duration < 0.2  # 200ms threshold
    
    @pytest.mark.asyncio
    async def test_error_handling(self, rs485_service):
        """Test error handling"""
        await rs485_service.connect()
        
        # Test reading non-existent register
        with pytest.raises(RS485CommunicationException, match="Mock register not found"):
            await rs485_service.read_register(1, 99999)


if __name__ == "__main__":
    # Run tests
    pytest.main([__file__, "-v"])
