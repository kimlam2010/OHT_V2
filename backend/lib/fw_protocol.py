"""
FW Protocol Implementation for OHT-50 Master Module Backend Integration
Version: 1.0.0
Date: 2025-01-27
Team: Backend
Task: BE-INT-02 (Protocol Implementation)
"""

import json
import time
import hashlib
import hmac
from typing import Dict, List, Optional, Any, Union
from dataclasses import dataclass, asdict
from enum import Enum
import logging

logger = logging.getLogger(__name__)

class FWMessageType(Enum):
    """FW message types"""
    SYSTEM_STATUS = "system_status"
    MODULE_INFO = "module_info"
    MODULE_COMMAND = "module_command"
    SAFETY_STATUS = "safety_status"
    CONFIG_UPDATE = "config_update"
    DIAGNOSTICS = "diagnostics"
    TELEMETRY = "telemetry"
    EVENT = "event"
    HEARTBEAT = "heartbeat"
    ERROR = "error"

class FWCommandType(Enum):
    """FW command types"""
    MOVE = "move"
    STOP = "stop"
    DOCK = "dock"
    UNDOCK = "undock"
    RESET = "reset"
    CONFIGURE = "configure"
    DIAGNOSE = "diagnose"
    EMERGENCY_STOP = "emergency_stop"

class FWEventType(Enum):
    """FW event types"""
    MODULE_CONNECTED = "module_connected"
    MODULE_DISCONNECTED = "module_disconnected"
    ESTOP_ACTIVATED = "estop_activated"
    ESTOP_DEACTIVATED = "estop_deactivated"
    FAULT_DETECTED = "fault_detected"
    FAULT_CLEARED = "fault_cleared"
    CONFIG_CHANGED = "config_changed"
    SYSTEM_STARTUP = "system_startup"
    SYSTEM_SHUTDOWN = "system_shutdown"

@dataclass
class FWMessage:
    """Base FW message structure"""
    message_id: str
    message_type: FWMessageType
    timestamp: int
    version: str = "1.0"
    source: str = "backend"
    destination: str = "firmware"
    data: Dict = None
    
    def __post_init__(self):
        if self.data is None:
            self.data = {}
    
    def to_dict(self) -> Dict:
        """Convert message to dictionary"""
        return {
            'message_id': self.message_id,
            'message_type': self.message_type.value,
            'timestamp': self.timestamp,
            'version': self.version,
            'source': self.source,
            'destination': self.destination,
            'data': self.data
        }
    
    def to_json(self) -> str:
        """Convert message to JSON string"""
        return json.dumps(self.to_dict())
    
    @classmethod
    def from_dict(cls, data: Dict) -> 'FWMessage':
        """Create message from dictionary"""
        return cls(
            message_id=data['message_id'],
            message_type=FWMessageType(data['message_type']),
            timestamp=data['timestamp'],
            version=data.get('version', '1.0'),
            source=data.get('source', 'backend'),
            destination=data.get('destination', 'firmware'),
            data=data.get('data', {})
        )
    
    @classmethod
    def from_json(cls, json_str: str) -> 'FWMessage':
        """Create message from JSON string"""
        data = json.loads(json_str)
        return cls.from_dict(data)

@dataclass
class FWCommand:
    """FW command structure"""
    command_id: str
    command_type: FWCommandType
    module_id: Optional[int] = None
    parameters: Dict = None
    priority: int = 1  # 1=low, 2=normal, 3=high, 4=critical
    timeout_ms: int = 30000
    
    def __post_init__(self):
        if self.parameters is None:
            self.parameters = {}
    
    def to_dict(self) -> Dict:
        """Convert command to dictionary"""
        return {
            'command_id': self.command_id,
            'command_type': self.command_type.value,
            'module_id': self.module_id,
            'parameters': self.parameters,
            'priority': self.priority,
            'timeout_ms': self.timeout_ms
        }

@dataclass
class FWEvent:
    """FW event structure"""
    event_id: str
    event_type: FWEventType
    timestamp: int
    source: str = "firmware"
    severity: str = "info"  # info, warning, error, critical
    data: Dict = None
    
    def __post_init__(self):
        if self.data is None:
            self.data = {}
    
    def to_dict(self) -> Dict:
        """Convert event to dictionary"""
        return {
            'event_id': self.event_id,
            'event_type': self.event_type.value,
            'timestamp': self.timestamp,
            'source': self.source,
            'severity': self.severity,
            'data': self.data
        }

class FWProtocol:
    """FW Protocol implementation"""
    
    def __init__(self, secret_key: Optional[str] = None):
        """
        Initialize FW protocol
        
        Args:
            secret_key: Secret key for message signing (optional)
        """
        self.secret_key = secret_key
        self.version = "1.0"
        self.message_counter = 0
    
    def generate_message_id(self) -> str:
        """Generate unique message ID"""
        self.message_counter += 1
        timestamp = int(time.time() * 1000)
        return f"msg_{timestamp}_{self.message_counter}"
    
    def generate_command_id(self) -> str:
        """Generate unique command ID"""
        timestamp = int(time.time() * 1000)
        return f"cmd_{timestamp}"
    
    def generate_event_id(self) -> str:
        """Generate unique event ID"""
        timestamp = int(time.time() * 1000)
        return f"evt_{timestamp}"
    
    def create_system_status_message(self, status_data: Dict) -> FWMessage:
        """Create system status message"""
        return FWMessage(
            message_id=self.generate_message_id(),
            message_type=FWMessageType.SYSTEM_STATUS,
            timestamp=int(time.time() * 1000),
            data=status_data
        )
    
    def create_module_command_message(self, command: FWCommand) -> FWMessage:
        """Create module command message"""
        return FWMessage(
            message_id=self.generate_message_id(),
            message_type=FWMessageType.MODULE_COMMAND,
            timestamp=int(time.time() * 1000),
            data=command.to_dict()
        )
    
    def create_safety_status_message(self, safety_data: Dict) -> FWMessage:
        """Create safety status message"""
        return FWMessage(
            message_id=self.generate_message_id(),
            message_type=FWMessageType.SAFETY_STATUS,
            timestamp=int(time.time() * 1000),
            data=safety_data
        )
    
    def create_config_update_message(self, config_data: Dict) -> FWMessage:
        """Create config update message"""
        return FWMessage(
            message_id=self.generate_message_id(),
            message_type=FWMessageType.CONFIG_UPDATE,
            timestamp=int(time.time() * 1000),
            data=config_data
        )
    
    def create_telemetry_message(self, telemetry_data: Dict) -> FWMessage:
        """Create telemetry message"""
        return FWMessage(
            message_id=self.generate_message_id(),
            message_type=FWMessageType.TELEMETRY,
            timestamp=int(time.time() * 1000),
            data=telemetry_data
        )
    
    def create_event_message(self, event: FWEvent) -> FWMessage:
        """Create event message"""
        return FWMessage(
            message_id=self.generate_message_id(),
            message_type=FWMessageType.EVENT,
            timestamp=int(time.time() * 1000),
            data=event.to_dict()
        )
    
    def create_heartbeat_message(self) -> FWMessage:
        """Create heartbeat message"""
        return FWMessage(
            message_id=self.generate_message_id(),
            message_type=FWMessageType.HEARTBEAT,
            timestamp=int(time.time() * 1000),
            data={'timestamp': int(time.time() * 1000)}
        )
    
    def create_error_message(self, error_code: str, error_message: str, details: Dict = None) -> FWMessage:
        """Create error message"""
        error_data = {
            'error_code': error_code,
            'error_message': error_message
        }
        if details:
            error_data['details'] = details
        
        return FWMessage(
            message_id=self.generate_message_id(),
            message_type=FWMessageType.ERROR,
            timestamp=int(time.time() * 1000),
            data=error_data
        )
    
    def create_move_command(self, module_id: int, target_position: float, 
                          velocity: float = 1.0, acceleration: float = 1.0) -> FWCommand:
        """Create move command"""
        return FWCommand(
            command_id=self.generate_command_id(),
            command_type=FWCommandType.MOVE,
            module_id=module_id,
            parameters={
                'target_position': target_position,
                'velocity': velocity,
                'acceleration': acceleration
            },
            priority=2,
            timeout_ms=60000
        )
    
    def create_stop_command(self, module_id: int, emergency: bool = False) -> FWCommand:
        """Create stop command"""
        return FWCommand(
            command_id=self.generate_command_id(),
            command_type=FWCommandType.STOP,
            module_id=module_id,
            parameters={'emergency': emergency},
            priority=3 if emergency else 2,
            timeout_ms=5000
        )
    
    def create_dock_command(self, module_id: int, dock_id: int) -> FWCommand:
        """Create dock command"""
        return FWCommand(
            command_id=self.generate_command_id(),
            command_type=FWCommandType.DOCK,
            module_id=module_id,
            parameters={'dock_id': dock_id},
            priority=2,
            timeout_ms=30000
        )
    
    def create_emergency_stop_command(self) -> FWCommand:
        """Create emergency stop command"""
        return FWCommand(
            command_id=self.generate_command_id(),
            command_type=FWCommandType.EMERGENCY_STOP,
            parameters={},
            priority=4,
            timeout_ms=1000
        )
    
    def create_module_connected_event(self, module_id: int, module_type: str) -> FWEvent:
        """Create module connected event"""
        return FWEvent(
            event_id=self.generate_event_id(),
            event_type=FWEventType.MODULE_CONNECTED,
            timestamp=int(time.time() * 1000),
            data={
                'module_id': module_id,
                'module_type': module_type
            }
        )
    
    def create_estop_activated_event(self, reason: str = None) -> FWEvent:
        """Create E-Stop activated event"""
        event_data = {}
        if reason:
            event_data['reason'] = reason
        
        return FWEvent(
            event_id=self.generate_event_id(),
            event_type=FWEventType.ESTOP_ACTIVATED,
            timestamp=int(time.time() * 1000),
            severity='critical',
            data=event_data
        )
    
    def create_fault_detected_event(self, fault_code: int, fault_message: str) -> FWEvent:
        """Create fault detected event"""
        return FWEvent(
            event_id=self.generate_event_id(),
            event_type=FWEventType.FAULT_DETECTED,
            timestamp=int(time.time() * 1000),
            severity='error',
            data={
                'fault_code': fault_code,
                'fault_message': fault_message
            }
        )
    
    def sign_message(self, message: FWMessage) -> str:
        """Sign message with HMAC"""
        if not self.secret_key:
            return ""
        
        message_str = message.to_json()
        signature = hmac.new(
            self.secret_key.encode('utf-8'),
            message_str.encode('utf-8'),
            hashlib.sha256
        ).hexdigest()
        
        return signature
    
    def verify_message_signature(self, message: FWMessage, signature: str) -> bool:
        """Verify message signature"""
        if not self.secret_key:
            return True  # No signature required
        
        expected_signature = self.sign_message(message)
        return hmac.compare_digest(signature, expected_signature)
    
    def validate_message(self, message: FWMessage) -> bool:
        """Validate message structure"""
        try:
            # Check required fields
            if not message.message_id or not message.message_type:
                return False
            
            # Check timestamp (not too old)
            current_time = int(time.time() * 1000)
            if current_time - message.timestamp > 300000:  # 5 minutes
                return False
            
            # Check version compatibility
            if message.version != self.version:
                return False
            
            return True
            
        except Exception as e:
            logger.error(f"Message validation error: {e}")
            return False
    
    def parse_message(self, json_str: str) -> Optional[FWMessage]:
        """Parse message from JSON string"""
        try:
            data = json.loads(json_str)
            message = FWMessage.from_dict(data)
            
            if not self.validate_message(message):
                return None
            
            return message
            
        except Exception as e:
            logger.error(f"Message parsing error: {e}")
            return None
    
    def create_response_message(self, original_message: FWMessage, 
                              success: bool, data: Dict = None, 
                              error_code: str = None, error_message: str = None) -> FWMessage:
        """Create response message"""
        response_data = {
            'success': success,
            'original_message_id': original_message.message_id
        }
        
        if success and data:
            response_data.update(data)
        elif not success:
            response_data['error_code'] = error_code
            response_data['error_message'] = error_message
        
        return FWMessage(
            message_id=self.generate_message_id(),
            message_type=FWMessageType.SYSTEM_STATUS if success else FWMessageType.ERROR,
            timestamp=int(time.time() * 1000),
            source=original_message.destination,
            destination=original_message.source,
            data=response_data
        )

# Example usage
def main():
    """Example usage of FW Protocol"""
    
    # Create protocol instance
    protocol = FWProtocol(secret_key="your-secret-key")
    
    # Create system status message
    status_data = {
        'system_name': 'OHT-50 Master Module',
        'version': '1.0.0',
        'status': 'running',
        'uptime_ms': 1234567890,
        'active_modules': 3,
        'estop_active': False,
        'safety_ok': True
    }
    
    status_message = protocol.create_system_status_message(status_data)
    print(f"Status Message: {status_message.to_json()}")
    
    # Create move command
    move_command = protocol.create_move_command(
        module_id=1,
        target_position=100.0,
        velocity=2.0,
        acceleration=1.5
    )
    
    command_message = protocol.create_module_command_message(move_command)
    print(f"Command Message: {command_message.to_json()}")
    
    # Create event
    event = protocol.create_module_connected_event(module_id=2, module_type="motor")
    event_message = protocol.create_event_message(event)
    print(f"Event Message: {event_message.to_json()}")
    
    # Sign message
    signature = protocol.sign_message(status_message)
    print(f"Message Signature: {signature}")
    
    # Verify signature
    is_valid = protocol.verify_message_signature(status_message, signature)
    print(f"Signature Valid: {is_valid}")
    
    # Parse message
    parsed_message = protocol.parse_message(status_message.to_json())
    print(f"Parsed Message ID: {parsed_message.message_id if parsed_message else 'None'}")

if __name__ == "__main__":
    main()
