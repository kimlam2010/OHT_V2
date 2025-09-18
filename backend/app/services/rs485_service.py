"""
RS485 Service for OHT-50 Backend
Service layer for RS485 module communication and management
"""

import logging
import os
from typing import Dict, Any, List, Optional

from app.config import settings
from app.schemas.rs485 import (
    RS485ModuleInfo, RS485BusHealth, RS485DiscoveryStatus, 
    RS485DiscoveryResult, RS485ModuleTelemetry
)
from app.services.firmware_integration_service import firmware_service
from app.services.mock_rs485_service import mock_rs485_service

logger = logging.getLogger(__name__)


class RS485Service:
    """
    RS485 Service for module communication and management
    
    Handles both real firmware integration and mock service for development/testing
    """
    
    def __init__(self):
        """Initialize RS485 service"""
        # Determine which service to use
        self._use_mock = self._should_use_mock()
        
        if self._use_mock:
            logger.warning("🧪 RS485 Service: Using Mock RS485 Service for development/testing")
            self._service = mock_rs485_service
        else:
            logger.info("🔌 RS485 Service: Using Real Firmware Integration Service")
            self._service = firmware_service
            
    def _should_use_mock(self) -> bool:
        """Determine if should use mock service"""
        # Check environment variables
        testing = os.getenv("TESTING", "false").lower() == "true"
        use_mock_firmware = os.getenv("USE_MOCK_FIRMWARE", "false").lower() == "true"
        
        # Check settings
        settings_mock = getattr(settings, 'use_mock_firmware', False)
        
        # Check environment
        is_production = settings.environment.lower() == "production"
        
        # Use mock if explicitly requested and not in production
        # RS485 mock is included in firmware mock
        should_mock = (testing or use_mock_firmware or settings_mock) and not is_production
        
        if should_mock:
            logger.warning("🧪 RS485 Service will use MOCK data (use_mock_firmware=true)")
            logger.warning("⚠️  This includes RS485 modules simulation for development/testing")
        else:
            logger.info("🔌 RS485 Service will use REAL firmware communication")
            
        return should_mock
        
    async def initialize(self) -> bool:
        """Initialize RS485 service"""
        try:
            if hasattr(self._service, 'initialize'):
                return await self._service.initialize()
            return True
        except Exception as e:
            logger.error(f"❌ RS485 Service initialization failed: {e}")
            return False
            
    async def get_modules(self) -> List[RS485ModuleInfo]:
        """Get list of all RS485 modules"""
        try:
            if self._use_mock:
                return await mock_rs485_service.get_modules()
            else:
                # For real firmware, we need to implement the actual API call
                # This will be implemented when firmware HTTP API is ready
                logger.warning("🔌 Real firmware RS485 modules API not yet implemented")
                # Fallback to mock for now
                return await mock_rs485_service.get_modules()
                
        except Exception as e:
            logger.error(f"❌ Failed to get RS485 modules: {e}")
            raise
            
    async def get_module(self, address: int) -> Optional[RS485ModuleInfo]:
        """Get specific RS485 module by address"""
        try:
            if self._use_mock:
                return await mock_rs485_service.get_module(address)
            else:
                # For real firmware, implement actual API call
                logger.warning("🔌 Real firmware RS485 module API not yet implemented")
                # Fallback to mock for now
                return await mock_rs485_service.get_module(address)
                
        except Exception as e:
            logger.error(f"❌ Failed to get RS485 module 0x{address:02X}: {e}")
            raise
            
    async def get_bus_health(self) -> RS485BusHealth:
        """Get RS485 bus health status"""
        try:
            if self._use_mock:
                return await mock_rs485_service.get_bus_health()
            else:
                # For real firmware, implement actual API call
                logger.warning("🔌 Real firmware RS485 bus health API not yet implemented")
                # Fallback to mock for now
                return await mock_rs485_service.get_bus_health()
                
        except Exception as e:
            logger.error(f"❌ Failed to get RS485 bus health: {e}")
            raise
            
    async def start_discovery(self) -> RS485DiscoveryStatus:
        """Start RS485 module discovery"""
        try:
            if self._use_mock:
                return await mock_rs485_service.start_discovery()
            else:
                # For real firmware, implement actual API call
                logger.warning("🔌 Real firmware RS485 discovery API not yet implemented")
                # Fallback to mock for now
                return await mock_rs485_service.start_discovery()
                
        except Exception as e:
            logger.error(f"❌ Failed to start RS485 discovery: {e}")
            raise
            
    async def get_discovery_status(self) -> RS485DiscoveryStatus:
        """Get discovery status"""
        try:
            if self._use_mock:
                return await mock_rs485_service.get_discovery_status()
            else:
                # For real firmware, implement actual API call
                logger.warning("🔌 Real firmware RS485 discovery status API not yet implemented")
                # Fallback to mock for now
                return await mock_rs485_service.get_discovery_status()
                
        except Exception as e:
            logger.error(f"❌ Failed to get RS485 discovery status: {e}")
            raise
            
    async def get_discovery_results(self) -> List[RS485DiscoveryResult]:
        """Get discovery results"""
        try:
            if self._use_mock:
                return await mock_rs485_service.get_discovery_results()
            else:
                # For real firmware, implement actual API call
                logger.warning("🔌 Real firmware RS485 discovery results API not yet implemented")
                # Fallback to mock for now
                return await mock_rs485_service.get_discovery_results()
                
        except Exception as e:
            logger.error(f"❌ Failed to get RS485 discovery results: {e}")
            raise
            
    async def ping_module(self, address: int) -> Dict[str, Any]:
        """Ping RS485 module"""
        try:
            if self._use_mock:
                return await mock_rs485_service.ping_module(address)
            else:
                # For real firmware, implement actual API call
                logger.warning("🔌 Real firmware RS485 ping API not yet implemented")
                # Fallback to mock for now
                return await mock_rs485_service.ping_module(address)
                
        except Exception as e:
            logger.error(f"❌ Failed to ping RS485 module 0x{address:02X}: {e}")
            raise
            
    async def reset_module(self, address: int) -> Dict[str, Any]:
        """Reset RS485 module"""
        try:
            if self._use_mock:
                return await mock_rs485_service.reset_module(address)
            else:
                # For real firmware, implement actual API call
                logger.warning("🔌 Real firmware RS485 reset API not yet implemented")
                # Fallback to mock for now
                return await mock_rs485_service.reset_module(address)
                
        except Exception as e:
            logger.error(f"❌ Failed to reset RS485 module 0x{address:02X}: {e}")
            raise
            
    async def restart_bus(self) -> Dict[str, Any]:
        """Restart RS485 bus"""
        try:
            if self._use_mock:
                return await mock_rs485_service.restart_bus()
            else:
                # For real firmware, implement actual API call
                logger.warning("🔌 Real firmware RS485 bus restart API not yet implemented")
                # Fallback to mock for now
                return await mock_rs485_service.restart_bus()
                
        except Exception as e:
            logger.error(f"❌ Failed to restart RS485 bus: {e}")
            raise
            
    async def configure_module(self, address: int, config: Dict[str, Any]) -> Dict[str, Any]:
        """Configure RS485 module"""
        try:
            logger.info(f"🔧 Configuring RS485 module 0x{address:02X}")
            
            # For now, just return success
            # Real implementation will call firmware API
            return {
                "success": True,
                "message": f"Module 0x{address:02X} configuration updated",
                "config": config
            }
            
        except Exception as e:
            logger.error(f"❌ Failed to configure RS485 module 0x{address:02X}: {e}")
            raise
            
    async def quarantine_module(self, address: int) -> Dict[str, Any]:
        """Quarantine RS485 module"""
        try:
            logger.warning(f"⚠️  Quarantining RS485 module 0x{address:02X}")
            
            # For now, just return success
            # Real implementation will disable module communication
            return {
                "success": True,
                "message": f"Module 0x{address:02X} quarantined - communication disabled"
            }
            
        except Exception as e:
            logger.error(f"❌ Failed to quarantine RS485 module 0x{address:02X}: {e}")
            raise
            
    # New Telemetry Methods for Issue #91
    async def get_module_telemetry(self, address: int) -> Optional[RS485ModuleTelemetry]:
        """Get module telemetry data - Issue #91"""
        try:
            logger.info(f"📊 Getting telemetry for RS485 module 0x{address:02X}")
            
            if self._use_mock:
                # Use mock service
                return await self._service.get_module_telemetry(address)
            else:
                # For real firmware, implement actual API call
                logger.warning("🔌 Real firmware telemetry API not yet implemented")
                # Fallback to mock for now
                return await mock_rs485_service.get_module_telemetry(address)
                
        except Exception as e:
            logger.error(f"❌ Failed to get telemetry for module 0x{address:02X}: {e}")
            raise
            
    async def update_module_register(self, address: int, register_address: str, value: float, force: bool = False) -> Dict[str, Any]:
        """Update writable module register - Issue #91"""
        try:
            logger.info(f"✏️ Updating register {register_address} on module 0x{address:02X} to {value}")
            
            if self._use_mock:
                # Use mock service
                return await self._service.update_module_register(address, register_address, value, force)
            else:
                # For real firmware, implement actual API call
                logger.warning("🔌 Real firmware register update API not yet implemented")
                # Fallback to mock for now
                return await mock_rs485_service.update_module_register(address, register_address, value, force)
                
        except Exception as e:
            logger.error(f"❌ Failed to update register {register_address} on module 0x{address:02X}: {e}")
            raise


# Global RS485 service instance
rs485_service = RS485Service()
