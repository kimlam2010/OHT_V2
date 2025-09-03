"""
Firmware Fallback Manager for Production Environment

This module provides fallback strategies when firmware is unavailable.
It includes mock data generation, fallback configuration, and monitoring.
"""

import logging
from typing import Dict, Any, Optional
from datetime import datetime, timedelta

from app.core.integration import MockFirmwareService

logger = logging.getLogger(__name__)


class FirmwareFallbackManager:
    """
    Fallback management system for firmware unavailability
    
    Features:
    - Mock firmware service fallback
    - Configurable fallback strategies
    - Fallback monitoring và alerting
    - Graceful degradation
    """
    
    def __init__(self):
        self.mock_service = MockFirmwareService()
        self.fallback_enabled = True
        self.fallback_mode = False
        self.fallback_start_time = None
        self.fallback_duration = timedelta(minutes=30)  # 30 minutes max fallback
        
        # Fallback statistics
        self.fallback_count = 0
        self.total_fallback_time = timedelta(0)
        
        logger.info("🔄 FirmwareFallbackManager initialized")
        
    async def get_fallback_status(self) -> Dict[str, Any]:
        """Get fallback robot status"""
        if not self.fallback_enabled:
            raise Exception("Fallback mode is disabled")
            
        try:
            # Check if we should still be in fallback mode
            if self.fallback_mode and self.fallback_start_time:
                elapsed = datetime.utcnow() - self.fallback_start_time
                if elapsed > self.fallback_duration:
                    logger.warning("⚠️ Fallback duration exceeded, attempting to reconnect to firmware")
                    self.fallback_mode = False
                    self.fallback_start_time = None
                    
            # Get status from mock service
            status = await self.mock_service.get_robot_status()
            
            # Add fallback indicators
            status.update({
                "fallback_mode": self.fallback_mode,
                "fallback_start_time": self.fallback_start_time.isoformat() if self.fallback_start_time else None,
                "fallback_duration": str(self.fallback_duration),
                "fallback_count": self.fallback_count
            })
            
            if self.fallback_mode:
                logger.warning("🧪 Using fallback firmware service for robot status")
                
            return status
            
        except Exception as e:
            logger.error(f"❌ Fallback status failed: {e}")
            return {
                "status": "error",
                "error": str(e),
                "fallback_mode": self.fallback_mode,
                "fallback_enabled": self.fallback_enabled
            }
            
    async def get_fallback_telemetry(self) -> Dict[str, Any]:
        """Get fallback telemetry data"""
        if not self.fallback_enabled:
            raise Exception("Fallback mode is disabled")
            
        try:
            # Get telemetry from mock service
            telemetry = await self.mock_service.get_telemetry_data()
            
            # Add fallback indicators
            telemetry.update({
                "fallback_mode": self.fallback_mode,
                "fallback_start_time": self.fallback_start_time.isoformat() if self.fallback_start_time else None,
                "fallback_duration": str(self.fallback_duration),
                "fallback_count": self.fallback_count
            })
            
            if self.fallback_mode:
                logger.warning("🧪 Using fallback firmware service for telemetry")
                
            return telemetry
            
        except Exception as e:
            logger.error(f"❌ Fallback telemetry failed: {e}")
            return {
                "error": str(e),
                "fallback_mode": self.fallback_mode,
                "fallback_enabled": self.fallback_enabled
            }
            
    async def execute_fallback_command(self, command: Dict[str, Any]) -> bool:
        """Execute command in fallback mode"""
        if not self.fallback_enabled:
            raise Exception("Fallback mode is disabled")
            
        try:
            # Execute command via mock service
            success = await self.mock_service.send_robot_command(command)
            
            if success:
                logger.warning(f"🧪 Command executed in fallback mode: {command.get('command_type', 'unknown')}")
            else:
                logger.error(f"❌ Fallback command failed: {command.get('command_type', 'unknown')}")
                
            return success
            
        except Exception as e:
            logger.error(f"❌ Fallback command execution failed: {e}")
            return False
            
    def enable_fallback_mode(self):
        """Enable fallback mode"""
        if not self.fallback_enabled:
            logger.warning("⚠️ Cannot enable fallback mode - fallback is disabled")
            return
            
        self.fallback_mode = True
        self.fallback_start_time = datetime.utcnow()
        self.fallback_count += 1
        
        logger.warning("🚨 Fallback mode enabled - using mock firmware service")
        
    def disable_fallback_mode(self):
        """Disable fallback mode"""
        if self.fallback_mode:
            self.fallback_mode = False
            
            # Calculate total fallback time
            if self.fallback_start_time:
                elapsed = datetime.utcnow() - self.fallback_start_time
                self.total_fallback_time += elapsed
                self.fallback_start_time = None
                
            logger.info("✅ Fallback mode disabled - returning to normal operation")
            
    def set_fallback_enabled(self, enabled: bool):
        """Enable or disable fallback functionality"""
        self.fallback_enabled = enabled
        
        if not enabled and self.fallback_mode:
            self.disable_fallback_mode()
            
        status = "enabled" if enabled else "disabled"
        logger.info(f"🔄 Fallback functionality {status}")
        
    def set_fallback_duration(self, duration: timedelta):
        """Set maximum fallback duration"""
        self.fallback_duration = duration
        logger.info(f"🔄 Fallback duration set to {duration}")
        
    def get_fallback_status_info(self) -> Dict[str, Any]:
        """Get comprehensive fallback status information"""
        current_time = datetime.utcnow()
        
        # Calculate current fallback duration
        current_fallback_duration = None
        if self.fallback_mode and self.fallback_start_time:
            current_fallback_duration = current_time - self.fallback_start_time
            
        # Calculate remaining fallback time
        remaining_fallback_time = None
        if self.fallback_mode and self.fallback_start_time:
            elapsed = current_time - self.fallback_start_time
            remaining = self.fallback_duration - elapsed
            remaining_fallback_time = max(timedelta(0), remaining)
            
        return {
            "fallback_enabled": self.fallback_enabled,
            "fallback_mode": self.fallback_mode,
            "fallback_start_time": self.fallback_start_time.isoformat() if self.fallback_start_time else None,
            "fallback_duration": str(self.fallback_duration),
            "current_fallback_duration": str(current_fallback_duration) if current_fallback_duration else None,
            "remaining_fallback_time": str(remaining_fallback_time) if remaining_fallback_time else None,
            "fallback_count": self.fallback_count,
            "total_fallback_time": str(self.total_fallback_time),
            "max_fallback_duration": str(self.fallback_duration)
        }
        
    def get_fallback_statistics(self) -> Dict[str, Any]:
        """Get fallback usage statistics"""
        return {
            "total_fallbacks": self.fallback_count,
            "total_fallback_time": str(self.total_fallback_time),
            "avg_fallback_duration": str(self.total_fallback_time / max(1, self.fallback_count)),
            "max_fallback_duration": str(self.fallback_duration),
            "fallback_enabled": self.fallback_enabled,
            "current_fallback_mode": self.fallback_mode
        }
        
    def reset_fallback_statistics(self):
        """Reset fallback statistics"""
        self.fallback_count = 0
        self.total_fallback_time = timedelta(0)
        self.fallback_mode = False
        self.fallback_start_time = None
        
        logger.info("🔄 Fallback statistics reset")
        
    def is_fallback_mode_active(self) -> bool:
        """Check if fallback mode is currently active"""
        return self.fallback_mode and self.fallback_enabled
        
    def should_use_fallback(self) -> bool:
        """Determine if fallback should be used"""
        if not self.fallback_enabled:
            return False
            
        if not self.fallback_mode:
            return False
            
        # Check if fallback duration has expired
        if self.fallback_start_time:
            elapsed = datetime.utcnow() - self.fallback_start_time
            if elapsed > self.fallback_duration:
                logger.warning("⚠️ Fallback duration expired")
                self.disable_fallback_mode()
                return False
                
        return True
