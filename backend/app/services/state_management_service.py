"""
StateManagementService - Quản lý trạng thái hệ thống
"""

import asyncio
import logging
from datetime import datetime, timedelta
from typing import List, Optional, Dict
from uuid import UUID

from app.models.state import (
    SystemState, SubsystemState, StateTransition, SubsystemStatus,
    SystemStatus, StateHistory, StateValidation, StateConfig,
    StateEvent, StateTransitionRule, StateSnapshot
)
from app.services.log_service import LogService


logger = logging.getLogger(__name__)


class StateManagementService:
    """Service quản lý trạng thái hệ thống"""
    
    def __init__(self, log_service: LogService):
        self.log_service = log_service
        self.current_state = SystemState.INITIALIZING
        self.subsystems: Dict[str, SubsystemStatus] = {}
        self.state_history: List[StateHistory] = []
        self.state_events: List[StateEvent] = []
        self.transition_rules: List[StateTransitionRule] = []
        self.state_config = StateConfig()
        self._lock = asyncio.Lock()
        
        # Initialize default subsystems
        self._initialize_subsystems()
        
        # Initialize transition rules
        self._initialize_transition_rules()
        
        logger.info("StateManagementService initialized")
    
    async def get_system_state(self) -> SystemState:
        """Lấy trạng thái hệ thống hiện tại"""
        async with self._lock:
            return self.current_state
    
    async def transition_state(self, from_state: SystemState, to_state: SystemState) -> bool:
        """Chuyển đổi trạng thái"""
        async with self._lock:
            try:
                # Validate transition
                if not await self.validate_state_transition(from_state, to_state):
                    logger.warning(f"Invalid state transition: {from_state} -> {to_state}")
                    return False
                
                # Create transition record
                transition = StateTransition(
                    from_state=from_state,
                    to_state=to_state,
                    timestamp=datetime.utcnow(),
                    trigger="manual",
                    is_valid=True
                )
                
                # Update current state
                self.current_state = to_state
                
                # Add to history
                state_history = StateHistory(
                    system_state=to_state,
                    timestamp=datetime.utcnow(),
                    trigger="manual"
                )
                self.state_history.append(state_history)
                
                # Create state event
                event = StateEvent(
                    event_type="state_transition",
                    description=f"State transition: {from_state} -> {to_state}",
                    source="state_management",
                    from_state=from_state,
                    to_state=to_state,
                    severity="info"
                )
                self.state_events.append(event)
                
                logger.info(f"State transition: {from_state} -> {to_state}")
                return True
                
            except Exception as e:
                logger.error(f"Error transitioning state: {e}")
                return False
    
    async def persist_state(self, state: SystemStatus) -> bool:
        """Lưu trạng thái hệ thống"""
        async with self._lock:
            try:
                # Create snapshot
                snapshot = StateSnapshot(
                    system_status=state,
                    state_history=self.state_history[-100:],  # Last 100 entries
                    events=self.state_events[-50:],  # Last 50 events
                    metadata={"persisted_at": datetime.utcnow().isoformat()}
                )
                
                # In a real implementation, this would save to database/file
                logger.info("System state persisted")
                return True
                
            except Exception as e:
                logger.error(f"Error persisting state: {e}")
                return False
    
    async def load_state(self) -> SystemStatus:
        """Tải trạng thái hệ thống"""
        async with self._lock:
            try:
                # In a real implementation, this would load from database/file
                # For now, return current state
                return await self._build_system_status()
                
            except Exception as e:
                logger.error(f"Error loading state: {e}")
                raise
    
    async def validate_state_transition(self, from_state: SystemState, to_state: SystemState) -> bool:
        """Validate chuyển đổi trạng thái"""
        async with self._lock:
            try:
                # Check if transition is allowed by rules
                for rule in self.transition_rules:
                    if (rule.from_state == from_state and 
                        rule.to_state == to_state and 
                        rule.is_enabled):
                        return True
                
                # Default validation logic
                valid_transitions = {
                    SystemState.INITIALIZING: [SystemState.IDLE, SystemState.FAULT],
                    SystemState.IDLE: [SystemState.MOVING, SystemState.DOCKING, SystemState.FAULT, SystemState.EMERGENCY_STOP],
                    SystemState.MOVING: [SystemState.IDLE, SystemState.DOCKING, SystemState.FAULT, SystemState.EMERGENCY_STOP],
                    SystemState.DOCKING: [SystemState.DOCKED, SystemState.IDLE, SystemState.FAULT, SystemState.EMERGENCY_STOP],
                    SystemState.DOCKED: [SystemState.IDLE, SystemState.FAULT, SystemState.EMERGENCY_STOP],
                    SystemState.FAULT: [SystemState.IDLE, SystemState.EMERGENCY_STOP],
                    SystemState.EMERGENCY_STOP: [SystemState.IDLE, SystemState.FAULT],
                    SystemState.MAINTENANCE: [SystemState.IDLE, SystemState.FAULT],
                    SystemState.SHUTDOWN: []
                }
                
                allowed_transitions = valid_transitions.get(from_state, [])
                return to_state in allowed_transitions
                
            except Exception as e:
                logger.error(f"Error validating state transition: {e}")
                return False
    
    async def update_subsystem_status(self, subsystem_id: str, status: SubsystemStatus):
        """Cập nhật trạng thái subsystem"""
        async with self._lock:
            try:
                self.subsystems[subsystem_id] = status
                
                # Create state event if status changed
                if subsystem_id in self.subsystems:
                    old_status = self.subsystems[subsystem_id]
                    if old_status.state != status.state:
                        event = StateEvent(
                            event_type="subsystem_status_change",
                            description=f"Subsystem {subsystem_id} status: {old_status.state} -> {status.state}",
                            source="subsystem",
                            subsystem_id=subsystem_id,
                            severity="info"
                        )
                        self.state_events.append(event)
                
                logger.debug(f"Subsystem {subsystem_id} status updated: {status.state}")
                
            except Exception as e:
                logger.error(f"Error updating subsystem status: {e}")
    
    async def get_system_status(self) -> SystemStatus:
        """Lấy trạng thái tổng thể hệ thống"""
        async with self._lock:
            return await self._build_system_status()
    
    async def add_transition_rule(self, rule: StateTransitionRule):
        """Thêm quy tắc chuyển đổi trạng thái"""
        async with self._lock:
            try:
                self.transition_rules.append(rule)
                logger.info(f"Transition rule added: {rule.from_state} -> {rule.to_state}")
                
            except Exception as e:
                logger.error(f"Error adding transition rule: {e}")
    
    async def get_state_history(self, hours: int = 24) -> List[StateHistory]:
        """Lấy lịch sử trạng thái"""
        async with self._lock:
            cutoff_time = datetime.utcnow() - timedelta(hours=hours)
            return [h for h in self.state_history if h.timestamp >= cutoff_time]
    
    async def get_state_events(self, limit: int = 100) -> List[StateEvent]:
        """Lấy sự kiện trạng thái"""
        async with self._lock:
            return self.state_events[-limit:]
    
    async def update_state_config(self, config_data: dict) -> StateConfig:
        """Cập nhật cấu hình quản lý trạng thái"""
        async with self._lock:
            try:
                for key, value in config_data.items():
                    if hasattr(self.state_config, key):
                        setattr(self.state_config, key, value)
                
                logger.info("State configuration updated")
                return self.state_config
                
            except Exception as e:
                logger.error(f"Error updating state config: {e}")
                raise
    
    async def _build_system_status(self) -> SystemStatus:
        """Xây dựng trạng thái tổng thể hệ thống"""
        try:
            # Calculate overall health
            total_subsystems = len(self.subsystems)
            online_subsystems = len([s for s in self.subsystems.values() if s.state == SubsystemState.ONLINE])
            overall_health = online_subsystems / total_subsystems if total_subsystems > 0 else 1.0
            
            # Count active alerts and errors
            active_alerts = len([s for s in self.subsystems.values() if s.state == SubsystemState.WARNING])
            active_errors = len([s for s in self.subsystems.values() if s.state == SubsystemState.ERROR])
            
            # Calculate uptime (simplified)
            uptime = 0.0  # In real implementation, calculate from start time
            
            return SystemStatus(
                system_state=self.current_state,
                subsystems=self.subsystems.copy(),
                overall_health=overall_health,
                uptime=uptime,
                last_state_change=datetime.utcnow(),
                active_alerts=active_alerts,
                active_errors=active_errors,
                current_mission=None,
                battery_level=None,
                temperature=None,
                metadata={}
            )
            
        except Exception as e:
            logger.error(f"Error building system status: {e}")
            raise
    
    def _initialize_subsystems(self):
        """Khởi tạo các subsystem mặc định"""
        default_subsystems = [
            ("movement", "Movement Control"),
            ("safety", "Safety System"),
            ("mission", "Mission Management"),
            ("communication", "Communication"),
            ("power", "Power Management"),
            ("sensors", "Sensor System")
        ]
        
        for subsystem_id, name in default_subsystems:
            status = SubsystemStatus(
                subsystem_id=subsystem_id,
                name=name,
                state=SubsystemState.ONLINE,
                health=1.0,
                last_heartbeat=datetime.utcnow(),
                error_count=0,
                warning_count=0,
                uptime=0.0
            )
            self.subsystems[subsystem_id] = status
    
    def _initialize_transition_rules(self):
        """Khởi tạo quy tắc chuyển đổi trạng thái"""
        # Add some default transition rules
        rules = [
            StateTransitionRule(
                from_state=SystemState.INITIALIZING,
                to_state=SystemState.IDLE,
                trigger="initialization_complete",
                conditions=["all_subsystems_online"],
                actions=["enable_mission_control", "start_safety_monitoring"],
                timeout=30.0,
                priority=1
            ),
            StateTransitionRule(
                from_state=SystemState.IDLE,
                to_state=SystemState.MOVING,
                trigger="mission_started",
                conditions=["mission_valid", "safety_system_ready"],
                actions=["start_movement_control", "activate_safety_zones"],
                timeout=5.0,
                priority=1
            ),
            StateTransitionRule(
                from_state=SystemState.MOVING,
                to_state=SystemState.EMERGENCY_STOP,
                trigger="emergency_condition",
                conditions=["estop_activated", "collision_detected"],
                actions=["stop_all_movement", "activate_emergency_procedures"],
                timeout=0.05,  # 50ms for emergency
                priority=10
            )
        ]
        
        for rule in rules:
            self.transition_rules.append(rule)
