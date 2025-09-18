"""
Mock RS485 Service for OHT-50 Backend
Mock implementation that matches firmware modules.yaml format
"""

import logging
import asyncio
import random
from datetime import datetime, timedelta
from typing import Dict, Any, List, Optional
from dataclasses import dataclass
import time

from app.schemas.rs485 import (
    RS485ModuleInfo, RS485ModuleType, RS485ModuleStatus,
    RS485BusHealth, RS485BusStatus, RS485ModuleRealTime,
    RS485DiscoveryStatus, RS485DiscoveryResult
)

logger = logging.getLogger(__name__)


@dataclass
class MockModuleData:
    """Mock module data structure matching firmware modules.yaml"""
    address: int
    type: int  # Firmware type code
    name: str
    status: int  # Firmware status code (1 = active)
    last_seen_ms: int
    capabilities: int
    version: str


class MockRS485Service:
    """
    Mock RS485 Service that simulates firmware RS485 modules
    
    WARNING: This is MOCK data - NOT for production use!
    Matches firmware modules.yaml format for development/testing
    Activated when use_mock_firmware=true
    """
    
    def __init__(self):
        """Initialize mock RS485 service"""
        # WARNING: This is MOCK data - NOT for production use
        logger.warning("🧪 MOCK RS485 Service: Using simulated RS485 modules (use_mock_firmware=true)")
        logger.warning("⚠️  WARNING: This service MUST NOT be used in production!")
        
        # Mock modules data (matches firmware modules.yaml)
        self._mock_modules = self._initialize_mock_modules()
        self._discovery_status = RS485DiscoveryStatus(
            is_running=False,
            progress=0,
            status_message="Ready to scan",
            modules_found=len(self._mock_modules),
            conflicts=[],
            start_time=None,
            end_time=None
        )
        
        # Bus health simulation
        self._bus_health = self._initialize_bus_health()
        
        # Start background update task
        self._update_task = None
        self._running = False
        
    async def initialize(self) -> bool:
        """Initialize mock RS485 service"""
        logger.warning("🧪 Initializing Mock RS485 Service...")
        self._running = True
        
        # Start background updates
        self._update_task = asyncio.create_task(self._background_updates())
        
        logger.warning("✅ Mock RS485 Service initialized - 7 mock modules available")
        return True
        
    async def shutdown(self) -> None:
        """Shutdown mock RS485 service"""
        logger.info("🧪 Shutting down Mock RS485 Service...")
        self._running = False
        
        if self._update_task:
            self._update_task.cancel()
            try:
                await self._update_task
            except asyncio.CancelledError:
                pass
                
    def _initialize_mock_modules(self) -> Dict[int, MockModuleData]:
        """Initialize mock modules data matching firmware modules.yaml"""
        current_time_ms = int(time.time() * 1000)
        
        # Mock modules matching firmware modules.yaml format
        mock_modules = {
            # Power Module (0x02)
            2: MockModuleData(
                address=2, type=2, name="Power", status=1,
                last_seen_ms=current_time_ms - 2000,
                capabilities=0, version="v2.1.0"
            ),
            # Safety Module (0x03) 
            3: MockModuleData(
                address=3, type=3, name="Safety", status=1,
                last_seen_ms=current_time_ms - 5000,
                capabilities=0, version="v1.9.5"
            ),
            # Travel Module (0x04)
            4: MockModuleData(
                address=4, type=4, name="Travel", status=1,
                last_seen_ms=current_time_ms - 1000,
                capabilities=0, version="v2.0.1"
            ),
            # Dock & Location Module (0x05)
            5: MockModuleData(
                address=5, type=5, name="Dock & Location", status=1,
                last_seen_ms=current_time_ms - 3000,
                capabilities=0, version="v1.8.2"
            ),
            # Master Module (0x06)
            6: MockModuleData(
                address=6, type=0, name="Master", status=1,
                last_seen_ms=current_time_ms - 1000,
                capabilities=0, version="v2.2.0"
            ),
            # Lifter Module (0x07)
            7: MockModuleData(
                address=7, type=0, name="Lifter", status=1,
                last_seen_ms=current_time_ms - 2000,
                capabilities=0, version="v1.7.8"
            ),
            # Cargo Module (0x08)
            8: MockModuleData(
                address=8, type=0, name="Cargo", status=1,
                last_seen_ms=current_time_ms - 1000,
                capabilities=0, version="v1.9.0"
            )
        }
        
        return mock_modules
        
    def _initialize_bus_health(self) -> RS485BusHealth:
        """Initialize mock bus health"""
        return RS485BusHealth(
            status=RS485BusStatus.ONLINE,
            error_rate=random.uniform(0.01, 0.05),  # 1-5% error rate
            response_time_p95=random.randint(60, 120),  # 60-120ms
            throughput=random.randint(45, 65),  # 45-65 fps
            last_scan=datetime.now().strftime("%H:%M:%S"),
            total_modules=len(self._mock_modules),
            active_modules=len([m for m in self._mock_modules.values() if m.status == 1]),
            failed_modules=0
        )
        
    def _firmware_type_to_module_type(self, fw_type: int) -> RS485ModuleType:
        """Convert firmware type code to module type enum"""
        type_mapping = {
            0: RS485ModuleType.UNKNOWN,
            2: RS485ModuleType.POWER,
            3: RS485ModuleType.SAFETY, 
            4: RS485ModuleType.TRAVEL,
            5: RS485ModuleType.DOCK_LOCATION,
            6: RS485ModuleType.MASTER,
            7: RS485ModuleType.LIFTER,
            8: RS485ModuleType.CARGO
        }
        return type_mapping.get(fw_type, RS485ModuleType.UNKNOWN)
        
    def _get_module_status(self, module: MockModuleData) -> RS485ModuleStatus:
        """Determine module status based on mock data"""
        current_time_ms = int(time.time() * 1000)
        time_since_last_seen = current_time_ms - module.last_seen_ms
        
        # Simulate status based on last seen time
        if time_since_last_seen > 120000:  # > 2 minutes
            return RS485ModuleStatus.LOST
        elif time_since_last_seen > 60000:  # > 1 minute
            return RS485ModuleStatus.ERROR
        elif time_since_last_seen > 30000:  # > 30 seconds
            return RS485ModuleStatus.WARNING
        else:
            return RS485ModuleStatus.HEALTHY
            
    def _generate_real_time_data(self, module: MockModuleData) -> RS485ModuleRealTime:
        """Generate mock real-time telemetry data"""
        # Different base values for different module types
        base_values = {
            2: {"battery": 85, "voltage": 24.2, "current": 2.1, "temp": 42},  # Power
            3: {"battery": 92, "voltage": 24.0, "current": 1.8, "temp": 38},  # Safety
            4: {"battery": 78, "voltage": 23.8, "current": 3.2, "temp": 45},  # Travel
            5: {"battery": 88, "voltage": 24.1, "current": 1.5, "temp": 40},  # Dock
            6: {"battery": 95, "voltage": 24.3, "current": 0.8, "temp": 35},  # Master
            7: {"battery": 82, "voltage": 23.9, "current": 2.5, "temp": 43},  # Lifter
            8: {"battery": 90, "voltage": 24.0, "current": 1.2, "temp": 37}   # Cargo
        }
        
        base = base_values.get(module.address, {"battery": 80, "voltage": 24.0, "current": 2.0, "temp": 40})
        
        # Add small random variations
        return RS485ModuleRealTime(
            battery=max(10, min(100, base["battery"] + random.uniform(-5, 5))),
            voltage=max(20, min(28, base["voltage"] + random.uniform(-0.5, 0.5))),
            current=max(0.1, min(5, base["current"] + random.uniform(-0.2, 0.2))),
            temperature=max(20, min(60, base["temp"] + random.uniform(-3, 3)))
        )
        
    async def _background_updates(self) -> None:
        """Background task to update mock data"""
        while self._running:
            try:
                # Update last_seen_ms for modules
                current_time_ms = int(time.time() * 1000)
                for module in self._mock_modules.values():
                    # Randomly update last_seen to simulate communication
                    if random.random() < 0.3:  # 30% chance to update
                        module.last_seen_ms = current_time_ms - random.randint(1000, 5000)
                
                # Update bus health
                self._bus_health.error_rate = random.uniform(0.01, 0.05)
                self._bus_health.response_time_p95 = random.randint(60, 120)
                self._bus_health.throughput = random.randint(45, 65)
                self._bus_health.last_scan = datetime.now().strftime("%H:%M:%S")
                
                await asyncio.sleep(2)  # Update every 2 seconds
                
            except asyncio.CancelledError:
                break
            except Exception as e:
                logger.error(f"❌ Mock RS485 background update error: {e}")
                await asyncio.sleep(5)
                
    async def get_modules(self) -> List[RS485ModuleInfo]:
        """Get list of all RS485 modules"""
        logger.info("🧪 Mock: Getting RS485 modules list")
        
        modules = []
        current_time_ms = int(time.time() * 1000)
        
        for module in self._mock_modules.values():
            # Calculate time since last seen
            time_diff_ms = current_time_ms - module.last_seen_ms
            time_diff_seconds = time_diff_ms // 1000
            
            # Format last seen
            if time_diff_seconds < 60:
                last_seen = f"{time_diff_seconds}s ago"
            elif time_diff_seconds < 3600:
                last_seen = f"{time_diff_seconds // 60}m ago"
            else:
                last_seen = f"{time_diff_seconds // 3600}h ago"
            
            # Determine if mandatory (addresses 2-6 are mandatory)
            mandatory = module.address in [2, 3, 4, 5, 6]
            
            module_info = RS485ModuleInfo(
                address=module.address,
                addr=f"0x{module.address:02X}",
                name=module.name,
                type=self._firmware_type_to_module_type(module.type),
                status=self._get_module_status(module),
                last_seen=last_seen,
                last_seen_ms=module.last_seen_ms,
                error_rate=random.uniform(0.01, 5.0),  # 0.01-5% error rate
                response_time=random.randint(30, 100),  # 30-100ms response time
                fw_version=module.version,
                mandatory=mandatory,
                capabilities=module.capabilities,
                version=module.version,
                real_time=self._generate_real_time_data(module)
            )
            modules.append(module_info)
            
        logger.info(f"🧪 Mock: Found {len(modules)} RS485 modules")
        return modules
        
    async def get_module(self, address: int) -> Optional[RS485ModuleInfo]:
        """Get specific RS485 module by address"""
        logger.info(f"🧪 Mock: Getting RS485 module 0x{address:02X}")
        
        modules = await self.get_modules()
        for module in modules:
            if module.address == address:
                return module
                
        logger.warning(f"🧪 Mock: Module 0x{address:02X} not found")
        return None
        
    async def get_bus_health(self) -> RS485BusHealth:
        """Get RS485 bus health status"""
        logger.info("🧪 Mock: Getting RS485 bus health")
        
        # Update active/failed modules count
        modules = await self.get_modules()
        active_count = len([m for m in modules if m.status in [RS485ModuleStatus.HEALTHY, RS485ModuleStatus.WARNING]])
        failed_count = len([m for m in modules if m.status in [RS485ModuleStatus.ERROR, RS485ModuleStatus.LOST]])
        
        self._bus_health.active_modules = active_count
        self._bus_health.failed_modules = failed_count
        
        return self._bus_health
        
    async def start_discovery(self) -> RS485DiscoveryStatus:
        """Start RS485 module discovery"""
        logger.info("🧪 Mock: Starting RS485 module discovery")
        
        if self._discovery_status.is_running:
            logger.warning("🧪 Mock: Discovery already running")
            return self._discovery_status
            
        # Start discovery simulation
        self._discovery_status.is_running = True
        self._discovery_status.progress = 0
        self._discovery_status.status_message = "Scanning RS485 bus..."
        self._discovery_status.start_time = datetime.now()
        self._discovery_status.end_time = None
        
        # Simulate discovery progress
        asyncio.create_task(self._simulate_discovery())
        
        return self._discovery_status
        
    async def _simulate_discovery(self) -> None:
        """Simulate discovery process"""
        try:
            # Simulate scanning progress
            for progress in range(0, 101, 10):
                self._discovery_status.progress = progress
                self._discovery_status.status_message = f"Scanning... {progress}%"
                await asyncio.sleep(0.2)  # 200ms per step
                
            # Complete discovery
            self._discovery_status.is_running = False
            self._discovery_status.progress = 100
            self._discovery_status.status_message = f"Scan complete · {len(self._mock_modules)} modules found · 0 conflicts"
            self._discovery_status.modules_found = len(self._mock_modules)
            self._discovery_status.conflicts = []
            self._discovery_status.end_time = datetime.now()
            
            logger.info("🧪 Mock: Discovery completed")
            
        except Exception as e:
            logger.error(f"❌ Mock discovery simulation error: {e}")
            self._discovery_status.is_running = False
            self._discovery_status.status_message = f"Discovery failed: {e}"
            
    async def get_discovery_status(self) -> RS485DiscoveryStatus:
        """Get discovery status"""
        return self._discovery_status
        
    async def get_discovery_results(self) -> List[RS485DiscoveryResult]:
        """Get discovery results"""
        logger.info("🧪 Mock: Getting discovery results")
        
        results = []
        for module in self._mock_modules.values():
            result = RS485DiscoveryResult(
                address=module.address,
                addr=f"0x{module.address:02X}",
                name=module.name,
                type=self._firmware_type_to_module_type(module.type),
                found=True,
                conflict=False,
                response_time=random.randint(30, 100)
            )
            results.append(result)
            
        return results
        
    async def ping_module(self, address: int) -> Dict[str, Any]:
        """Ping RS485 module"""
        logger.info(f"🧪 Mock: Pinging module 0x{address:02X}")
        
        if address not in self._mock_modules:
            return {"success": False, "message": "Module not found"}
            
        # Simulate ping response
        await asyncio.sleep(0.1)  # Simulate network delay
        
        return {
            "success": True,
            "message": "Ping successful",
            "response_time": random.randint(30, 80),
            "timestamp": datetime.now().isoformat()
        }
        
    async def reset_module(self, address: int) -> Dict[str, Any]:
        """Reset RS485 module"""
        logger.info(f"🧪 Mock: Resetting module 0x{address:02X}")
        
        if address not in self._mock_modules:
            return {"success": False, "message": "Module not found"}
            
        # Simulate reset
        await asyncio.sleep(2.0)  # Simulate reset time
        
        # Update last_seen after reset
        self._mock_modules[address].last_seen_ms = int(time.time() * 1000)
        
        return {
            "success": True,
            "message": "Module reset successful",
            "timestamp": datetime.now().isoformat()
        }
        
    async def restart_bus(self) -> Dict[str, Any]:
        """Restart RS485 bus"""
        logger.info("🧪 Mock: Restarting RS485 bus")
        
        # Simulate bus restart
        await asyncio.sleep(3.0)  # Simulate restart time
        
        # Update all modules last_seen
        current_time_ms = int(time.time() * 1000)
        for module in self._mock_modules.values():
            module.last_seen_ms = current_time_ms - random.randint(1000, 3000)
            
        # Reset bus health
        self._bus_health = self._initialize_bus_health()
        
        return {
            "success": True,
            "message": "RS485 bus restart successful",
            "timestamp": datetime.now().isoformat()
        }


# Global mock RS485 service instance
mock_rs485_service = MockRS485Service()
