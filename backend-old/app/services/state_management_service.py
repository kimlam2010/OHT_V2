"""
State Management Service for OHT-50
Manages system states and transitions
"""

import asyncio
import logging
from datetime import datetime
from typing import Dict, Any, Optional
from enum import Enum

logger = logging.getLogger(__name__)

class SystemState(Enum):
    """System states for OHT-50"""
    IDLE = "IDLE"
    MOVING = "MOVING"
    DOCKING = "DOCKING"
    FAULT = "FAULT"
    ESTOP = "ESTOP"
    INITIALIZING = "INITIALIZING"
    SHUTDOWN = "SHUTDOWN"

class SafetyState(Enum):
    """Safety states for OHT-50"""
    NORMAL = "NORMAL"
    WARNING = "WARNING"
    CRITICAL = "CRITICAL"
    ESTOP = "ESTOP"

class StateManagementService:
    """Service for managing OHT-50 system states"""
    
    def __init__(self):
        self._current_state = SystemState.INITIALIZING
        self._safety_state = SafetyState.NORMAL
        self._state_lock = asyncio.Lock()
        self._state_history = []
        self._max_history_size = 100
        
        # State transition rules
        self._valid_transitions = {
            SystemState.INITIALIZING: [SystemState.IDLE, SystemState.FAULT],
            SystemState.IDLE: [SystemState.MOVING, SystemState.DOCKING, SystemState.FAULT, SystemState.ESTOP, SystemState.SHUTDOWN],
            SystemState.MOVING: [SystemState.IDLE, SystemState.DOCKING, SystemState.FAULT, SystemState.ESTOP],
            SystemState.DOCKING: [SystemState.IDLE, SystemState.FAULT, SystemState.ESTOP],
            SystemState.FAULT: [SystemState.IDLE, SystemState.ESTOP, SystemState.SHUTDOWN],
            SystemState.ESTOP: [SystemState.IDLE, SystemState.FAULT, SystemState.SHUTDOWN],
            SystemState.SHUTDOWN: []
        }
        
        # Safety state transitions
        self._safety_transitions = {
            SafetyState.NORMAL: [SafetyState.WARNING, SafetyState.CRITICAL, SafetyState.ESTOP],
            SafetyState.WARNING: [SafetyState.NORMAL, SafetyState.CRITICAL, SafetyState.ESTOP],
            SafetyState.CRITICAL: [SafetyState.WARNING, SafetyState.ESTOP],
            SafetyState.ESTOP: [SafetyState.NORMAL, SafetyState.WARNING, SafetyState.CRITICAL]
        }
    
    async def get_current_state(self) -> str:
        """Get current system state as string"""
        async with self._state_lock:
            return self._current_state.value
    
    async def get_safety_state(self) -> str:
        """Get current safety state as string"""
        async with self._state_lock:
            return self._safety_state.value
    
    async def get_full_state(self) -> Dict[str, Any]:
        """Get complete system state information"""
        async with self._state_lock:
            return {
                "system_state": self._current_state.value,
                "safety_state": self._safety_state.value,
                "timestamp": datetime.utcnow().isoformat(),
                "state_history": self._state_history[-10:]  # Last 10 transitions
            }
    
    async def transition_to(self, new_state: SystemState, reason: str = "") -> bool:
        """Transition to new system state"""
        async with self._state_lock:
            if new_state in self._valid_transitions.get(self._current_state, []):
                old_state = self._current_state
                self._current_state = new_state
                
                # Record transition
                transition = {
                    "from_state": old_state.value,
                    "to_state": new_state.value,
                    "reason": reason,
                    "timestamp": datetime.utcnow().isoformat()
                }
                self._state_history.append(transition)
                
                # Keep history size manageable
                if len(self._state_history) > self._max_history_size:
                    self._state_history = self._state_history[-self._max_history_size:]
                
                logger.info(f"State transition: {old_state.value} -> {new_state.value} ({reason})")
                return True
            else:
                logger.warning(f"Invalid state transition: {self._current_state.value} -> {new_state.value}")
                return False
    
    async def transition_safety_to(self, new_safety_state: SafetyState, reason: str = "") -> bool:
        """Transition to new safety state"""
        async with self._state_lock:
            if new_safety_state in self._safety_transitions.get(self._safety_state, []):
                old_safety_state = self._safety_state
                self._safety_state = new_safety_state
                
                # Record transition
                transition = {
                    "from_safety_state": old_safety_state.value,
                    "to_safety_state": new_safety_state.value,
                    "reason": reason,
                    "timestamp": datetime.utcnow().isoformat()
                }
                self._state_history.append(transition)
                
                # Keep history size manageable
                if len(self._state_history) > self._max_history_size:
                    self._state_history = self._state_history[-self._max_history_size:]
                
                logger.info(f"Safety state transition: {old_safety_state.value} -> {new_safety_state.value} ({reason})")
                return True
            else:
                logger.warning(f"Invalid safety state transition: {self._safety_state.value} -> {new_safety_state.value}")
                return False
    
    async def is_system_ready(self) -> bool:
        """Check if system is ready for operations"""
        async with self._state_lock:
            return (self._current_state == SystemState.IDLE and 
                   self._safety_state in [SafetyState.NORMAL, SafetyState.WARNING])
    
    async def is_safe_to_move(self) -> bool:
        """Check if system is safe to move"""
        async with self._state_lock:
            return (self._current_state in [SystemState.IDLE, SystemState.MOVING] and
                   self._safety_state != SafetyState.ESTOP)
    
    async def is_in_fault_state(self) -> bool:
        """Check if system is in fault state"""
        async with self._state_lock:
            return (self._current_state == SystemState.FAULT or 
                   self._safety_state == SafetyState.ESTOP)
    
    async def emergency_stop(self, reason: str = "Emergency stop activated") -> bool:
        """Activate emergency stop"""
        async with self._state_lock:
            # Set both system and safety states to ESTOP
            self._current_state = SystemState.ESTOP
            self._safety_state = SafetyState.ESTOP
            
            # Record emergency stop
            transition = {
                "emergency_stop": True,
                "reason": reason,
                "timestamp": datetime.utcnow().isoformat()
            }
            self._state_history.append(transition)
            
            logger.critical(f"EMERGENCY STOP ACTIVATED: {reason}")
            return True
    
    async def clear_fault(self, reason: str = "Fault cleared") -> bool:
        """Clear fault state"""
        async with self._state_lock:
            if self._current_state == SystemState.FAULT:
                return await self.transition_to(SystemState.IDLE, reason)
            elif self._safety_state == SafetyState.ESTOP:
                return await self.transition_safety_to(SafetyState.NORMAL, reason)
            else:
                logger.warning("No fault state to clear")
                return False
    
    async def get_state_history(self, limit: int = 50) -> list:
        """Get state transition history"""
        async with self._state_lock:
            return self._state_history[-limit:] if limit > 0 else self._state_history.copy()
    
    async def reset_state(self):
        """Reset state to initializing (for testing/debugging)"""
        async with self._state_lock:
            self._current_state = SystemState.INITIALIZING
            self._safety_state = SafetyState.NORMAL
            self._state_history = []
            logger.info("State management service reset")

# Global instance
state_management_service = StateManagementService()
