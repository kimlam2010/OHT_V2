# REQ_BE_05 - Integration Specifications

**Document ID:** REQ_BE_05  
**Version:** 1.0  
**Date:** 2025-01-28  
**Status:** Draft  
**Classification:** Internal  
**Author:** CTO Team  

---

## 1. DOCUMENT INFORMATION

### 1.1 Document Control
- **Document Title:** Integration Specifications
- **Document Type:** Requirements Specification
- **Standard Reference:** ISO/IEC/IEEE 29148:2018, Modbus RTU Protocol
- **Project:** OHT-50 Industrial Robot Control System
- **Component:** Backend-Firmware Integration

### 1.2 Revision History
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-01-28 | CTO Team | Initial creation |

---

## 2. INTEGRATION OVERVIEW

### 2.1 Integration Architecture
The Backend System integrates with the existing firmware layer through:
- **RS485 Communication:** Modbus RTU protocol for module communication
- **Hardware Interface:** Direct hardware control through GPIO
- **Data Exchange:** Real-time telemetry and command exchange
- **Safety Integration:** Safety system coordination

### 2.2 Integration Components
```
Integration Architecture:
├── Backend Services
│   ├── Communication Service
│   ├── Hardware Interface Service
│   └── Safety Integration Service
├── Firmware Layer (firmware_new/)
│   ├── RS485 Communication
│   ├── Module Management
│   └── Hardware Control
└── Hardware Layer
    ├── Orange Pi 5B
    ├── RS485 Modules
    └── GPIO Interfaces
```

---

## 3. LIĐAR INTEGRATION

### 3.1 LiDAR Hardware Integration
```yaml
LiDAR Hardware:
  Sensor Model: "RPLIDAR A2M8"
  Interface: "USB 2.0 / UART"
  Communication:
    Protocol: "UART / USB"
    Baud Rate: "115200 bps"
    Data Format: "Binary / ASCII"
    Frame Rate: "8 FPS"
  
  Specifications:
    Range: "0.15m - 12m"
    Angular Resolution: "0.9°"
    Scan Frequency: "8Hz"
    Power: "5V DC, 500mA"
    Operating Temperature: "-10°C to +50°C"
    Protection: "IP54"
```

### 3.2 LiDAR Communication Protocol
```python
class LiDARCommunicationService:
    def __init__(self):
        self.lidar_port = "/dev/ttyUSB0"
        self.baud_rate = 115200
        self.timeout = 1.0
        self.retry_count = 3
        
    async def initialize_lidar(self):
        """Initialize LiDAR sensor connection"""
        # Implementation for LiDAR initialization
        
    async def read_scan_data(self):
        """Read LiDAR scan data"""
        # Implementation for reading scan data
        
    async def process_point_cloud(self, raw_data):
        """Process raw LiDAR data into structured format"""
        # Implementation for point cloud processing
```

### 3.3 LiDAR Safety Integration
```python
class LiDARSafetyIntegrationService:
    def __init__(self):
        self.safety_zones = {
            "critical": {"min": 0.0, "max": 0.5, "action": "emergency_stop"},
            "warning": {"min": 0.5, "max": 1.0, "action": "reduce_speed"},
            "safe": {"min": 1.0, "max": 2.0, "action": "monitor"}
        }
        
    async def check_safety_status(self, obstacles):
        """Check safety status and trigger appropriate actions"""
        # Implementation for safety checking
        
    async def trigger_emergency_stop(self):
        """Trigger emergency stop from LiDAR detection"""
        # Implementation for emergency stop
```

## 4. RS485 COMMUNICATION INTEGRATION

### 4.1 Modbus RTU Protocol
- **Protocol Version:** Modbus RTU
- **Baud Rate:** 115200 bps
- **Data Bits:** 8
- **Parity:** None
- **Stop Bits:** 1
- **Timeout:** 1000ms
- **Retry Count:** 3

### 4.2 Module Addressing
```yaml
Module Address Configuration:
  travel_motor: 1
  dock_location: 2
  safety_module: 3
  power_module: 4
  io_module: 5
  reserved_addresses: [6-10]
  broadcast_address: 0
```

### 4.3 Register Mapping
#### 4.3.1 Travel Motor Module (Address 1)
```yaml
Travel Motor Registers:
  # Control Registers (Write)
  40001: "motor_enable"           # 0=Disabled, 1=Enabled
  40002: "motor_direction"        # 0=Forward, 1=Reverse
  40003: "motor_speed"            # 0-100 (percentage)
  40004: "motor_acceleration"     # 0-100 (percentage)
  40005: "motor_deceleration"     # 0-100 (percentage)
  40006: "emergency_stop"         # 0=Normal, 1=Emergency Stop
  
  # Status Registers (Read)
  30001: "motor_status"           # 0=Stopped, 1=Running, 2=Error
  30002: "motor_speed_feedback"   # Current speed (RPM)
  30003: "motor_current"          # Current consumption (A)
  30004: "motor_voltage"          # Voltage (V)
  30005: "motor_temperature"      # Temperature (°C)
  30006: "motor_position"         # Current position (mm)
  30007: "motor_error_code"       # Error code
  30008: "motor_error_message"    # Error message
```

#### 4.3.2 Dock Location Module (Address 2)
```yaml
Dock Location Registers:
  # Control Registers (Write)
  40001: "dock_enable"            # 0=Disabled, 1=Enabled
  40002: "dock_mode"              # 0=Manual, 1=Auto
  40003: "dock_command"           # 0=Stop, 1=Dock, 2=Undock
  
  # Status Registers (Read)
  30001: "dock_status"            # 0=Idle, 1=Docking, 2=Docked, 3=Error
  30002: "dock_position_x"        # X position (mm)
  30003: "dock_position_y"        # Y position (mm)
  30004: "dock_orientation"       # Orientation (degrees)
  30005: "dock_accuracy"          # Docking accuracy (mm)
  30006: "dock_error_code"        # Error code
  30007: "nfc_tag_id"             # NFC tag ID
  30008: "dock_sensor_status"     # Sensor status
```

#### 4.3.3 Safety Module (Address 3)
```yaml
Safety Module Registers:
  # Control Registers (Write)
  40001: "safety_enable"          # 0=Disabled, 1=Enabled
  40002: "safety_mode"            # 0=Normal, 1=Maintenance
  40003: "safety_reset"           # 0=Normal, 1=Reset
  
  # Status Registers (Read)
  30001: "safety_status"          # 0=Safe, 1=Warning, 2=Error, 3=Emergency
  30002: "emergency_stop_status"  # 0=Released, 1=Pressed
  30003: "safety_zone_1"          # Zone 1 status
  30004: "safety_zone_2"          # Zone 2 status
  30005: "safety_zone_3"          # Zone 3 status
  30006: "obstacle_detected"      # Obstacle detection
  30007: "safety_error_code"      # Error code
  30008: "safety_error_message"   # Error message
```

### 4.4 Communication Service Implementation
```python
class RS485CommunicationService:
    def __init__(self):
        self.serial_port = "/dev/ttyOHT485"
        self.baud_rate = 115200
        self.timeout = 1.0
        self.retry_count = 3
        
    async def read_register(self, module_address: int, register_address: int) -> int:
        """Read Modbus register from module"""
        # Implementation for reading registers
        
    async def write_register(self, module_address: int, register_address: int, value: int) -> bool:
        """Write Modbus register to module"""
        # Implementation for writing registers
        
    async def read_multiple_registers(self, module_address: int, start_address: int, count: int) -> List[int]:
        """Read multiple registers from module"""
        # Implementation for reading multiple registers
        
    async def discover_modules(self) -> List[Dict]:
        """Discover connected modules"""
        # Implementation for module discovery
```

---

## 5. HARDWARE INTERFACE INTEGRATION

### 5.1 GPIO Control Integration
- **Platform:** Orange Pi 5B
- **GPIO Library:** libgpiod
- **Pin Configuration:** Device tree overlay

#### 5.1.1 GPIO Pin Mapping
```yaml
GPIO Pin Configuration:
  # Relay Control
  relay_1: "GPIO1_D3"    # Relay 1 control
  relay_2: "GPIO1_D2"    # Relay 2 control
  
  # LED Status
  led_power: "GPIO1_D4"      # Power LED
  led_system: "GPIO1_D5"     # System LED
  led_communication: "GPIO1_D6"  # Communication LED
  led_network: "GPIO1_D7"    # Network LED
  led_error: "GPIO1_D8"      # Error LED
  
  # Emergency Stop
  estop_input: "GPIO1_D9"    # Emergency stop input
  
  # Additional I/O
  digital_input_1: "GPIO1_D10"   # Digital input 1
  digital_input_2: "GPIO1_D11"   # Digital input 2
  digital_output_1: "GPIO1_D12"  # Digital output 1
  digital_output_2: "GPIO1_D13"  # Digital output 2
```

#### 5.1.2 Hardware Interface Service
```python
class HardwareInterfaceService:
    def __init__(self):
        self.gpio_chip = "gpiochip1"
        self.relay_lines = {}
        self.led_lines = {}
        self.estop_line = None
        
    async def initialize_hardware(self):
        """Initialize GPIO hardware interface"""
        # Implementation for hardware initialization
        
    async def set_relay(self, relay_id: int, state: bool) -> bool:
        """Set relay state"""
        # Implementation for relay control
        
    async def set_led(self, led_id: str, state: bool) -> bool:
        """Set LED state"""
        # Implementation for LED control
        
    async def read_estop(self) -> bool:
        """Read emergency stop status"""
        # Implementation for E-stop reading
        
    async def read_digital_input(self, input_id: int) -> bool:
        """Read digital input"""
        # Implementation for digital input reading
        
    async def set_digital_output(self, output_id: int, state: bool) -> bool:
        """Set digital output"""
        # Implementation for digital output control
```

---

## 6. FIRMWARE INTEGRATION SPECIFICATIONS

### 6.1 Module Communication Protocol
- **Protocol:** Modbus RTU over RS485
- **Frame Format:** Standard Modbus RTU frame
- **Error Handling:** CRC validation and retry mechanism
- **Timeout Management:** Configurable timeouts per module

### 6.2 Module Discovery and Management
```python
class ModuleManager:
    def __init__(self):
        self.modules = {}
        self.communication_service = RS485CommunicationService()
        
    async def discover_modules(self) -> Dict[str, Dict]:
        """Discover all connected modules"""
        discovered_modules = {}
        
        # Scan address range 1-10
        for address in range(1, 11):
            try:
                # Try to read status register
                status = await self.communication_service.read_register(address, 30001)
                if status is not None:
                    module_info = await self.get_module_info(address)
                    discovered_modules[f"module_{address}"] = module_info
            except Exception as e:
                continue
                
        return discovered_modules
        
    async def get_module_info(self, address: int) -> Dict:
        """Get module information"""
        # Implementation for getting module details
        
    async def monitor_modules(self):
        """Monitor module health and status"""
        # Implementation for module monitoring
```

### 6.3 Real-time Data Exchange
```python
class TelemetryService:
    def __init__(self):
        self.module_manager = ModuleManager()
        self.data_buffer = {}
        
    async def collect_telemetry(self) -> Dict:
        """Collect telemetry data from all modules"""
        telemetry_data = {
            "timestamp": datetime.utcnow().isoformat(),
            "robot_telemetry": {},
            "system_telemetry": {},
            "sensor_telemetry": {}
        }
        
        # Collect from travel motor
        motor_data = await self.get_motor_telemetry()
        telemetry_data["robot_telemetry"]["motor"] = motor_data
        
        # Collect from dock location
        dock_data = await self.get_dock_telemetry()
        telemetry_data["robot_telemetry"]["dock"] = dock_data
        
        # Collect from safety module
        safety_data = await self.get_safety_telemetry()
        telemetry_data["robot_telemetry"]["safety"] = safety_data
        
        return telemetry_data
        
    async def get_motor_telemetry(self) -> Dict:
        """Get motor telemetry data"""
        # Implementation for motor telemetry collection
        
    async def get_dock_telemetry(self) -> Dict:
        """Get dock telemetry data"""
        # Implementation for dock telemetry collection
        
    async def get_safety_telemetry(self) -> Dict:
        """Get safety telemetry data"""
        # Implementation for safety telemetry collection
```

---

## 7. COMMAND EXECUTION INTEGRATION

### 7.1 Command Processing Pipeline
```python
class CommandExecutionService:
    def __init__(self):
        self.communication_service = RS485CommunicationService()
        self.hardware_service = HardwareInterfaceService()
        self.command_queue = asyncio.Queue()
        
    async def execute_command(self, command: Dict) -> Dict:
        """Execute robot command"""
        command_id = command.get("command_id")
        command_type = command.get("command_type")
        
        try:
            if command_type == "move":
                result = await self.execute_move_command(command)
            elif command_type == "stop":
                result = await self.execute_stop_command(command)
            elif command_type == "emergency_stop":
                result = await self.execute_emergency_stop(command)
            elif command_type == "dock":
                result = await self.execute_dock_command(command)
            else:
                raise ValueError(f"Unknown command type: {command_type}")
                
            return {
                "command_id": command_id,
                "status": "completed",
                "result": result,
                "timestamp": datetime.utcnow().isoformat()
            }
            
        except Exception as e:
            return {
                "command_id": command_id,
                "status": "failed",
                "error": str(e),
                "timestamp": datetime.utcnow().isoformat()
            }
            
    async def execute_move_command(self, command: Dict) -> Dict:
        """Execute movement command"""
        # Implementation for movement command execution
        
    async def execute_stop_command(self, command: Dict) -> Dict:
        """Execute stop command"""
        # Implementation for stop command execution
        
    async def execute_emergency_stop(self, command: Dict) -> Dict:
        """Execute emergency stop"""
        # Implementation for emergency stop execution
```

### 7.2 Safety Integration
```python
class SafetyIntegrationService:
    def __init__(self):
        self.safety_status = "safe"
        self.safety_monitor_task = None
        
    async def start_safety_monitoring(self):
        """Start safety monitoring"""
        self.safety_monitor_task = asyncio.create_task(self.monitor_safety())
        
    async def monitor_safety(self):
        """Monitor safety conditions"""
        while True:
            try:
                # Read safety status from module
                safety_status = await self.read_safety_status()
                
                # Check for safety violations
                if safety_status != "safe":
                    await self.handle_safety_violation(safety_status)
                    
                # Update safety status
                self.safety_status = safety_status
                
                await asyncio.sleep(0.1)  # 100ms monitoring interval
                
            except Exception as e:
                logger.error(f"Safety monitoring error: {e}")
                await asyncio.sleep(1)
                
    async def handle_safety_violation(self, violation_type: str):
        """Handle safety violation"""
        # Implementation for safety violation handling
```

---

## 8. ERROR HANDLING AND RECOVERY

### 8.1 Communication Error Handling
- **Connection Loss:** Automatic reconnection with exponential backoff
- **Timeout Errors:** Configurable timeout and retry mechanisms
- **CRC Errors:** Automatic retry with error logging
- **Module Offline:** Module status monitoring and alerting

### 8.2 Hardware Error Handling
- **GPIO Errors:** Error logging and fallback mechanisms
- **Hardware Failures:** Automatic hardware reset procedures
- **Sensor Failures:** Sensor redundancy and error reporting
- **Power Failures:** Graceful shutdown and recovery procedures

### 8.3 Recovery Procedures
```python
class RecoveryService:
    def __init__(self):
        self.recovery_procedures = {
            "communication_error": self.recover_communication,
            "hardware_error": self.recover_hardware,
            "safety_violation": self.recover_safety,
            "module_offline": self.recover_module
        }
        
    async def handle_error(self, error_type: str, error_details: Dict):
        """Handle system errors and initiate recovery"""
        if error_type in self.recovery_procedures:
            await self.recovery_procedures[error_type](error_details)
        else:
            logger.error(f"Unknown error type: {error_type}")
            
    async def recover_communication(self, error_details: Dict):
        """Recover from communication errors"""
        # Implementation for communication recovery
        
    async def recover_hardware(self, error_details: Dict):
        """Recover from hardware errors"""
        # Implementation for hardware recovery
```

---

## 9. PERFORMANCE REQUIREMENTS

### 9.1 Communication Performance
- **Response Time:** < 50ms for register read/write operations
- **Throughput:** 1000+ register operations per second
- **Reliability:** 99.9% successful communication rate
- **Latency:** < 10ms for real-time data exchange

### 9.2 Hardware Performance
- **GPIO Response:** < 1ms for GPIO operations
- **LED Update:** < 5ms for LED state changes
- **Relay Control:** < 2ms for relay switching
- **E-Stop Response:** < 10ms for emergency stop detection

### 9.3 System Performance
- **Telemetry Collection:** 100Hz telemetry collection rate
- **Command Processing:** < 20ms command processing time
- **Safety Monitoring:** 10Hz safety monitoring rate
- **Module Discovery:** < 5 seconds for complete module discovery

---

## 10. TESTING AND VALIDATION

### 10.1 Integration Testing
- **Hardware-in-the-Loop Testing:** Real hardware integration testing
- **Module Communication Testing:** RS485 communication validation
- **Safety System Testing:** Safety integration validation
- **Performance Testing:** System performance validation

### 10.2 Test Procedures
```yaml
Integration Test Procedures:
  communication_test:
    - Test RS485 communication with each module
    - Validate register read/write operations
    - Test error handling and recovery
    
  hardware_test:
    - Test GPIO control operations
    - Validate LED and relay control
    - Test emergency stop functionality
    
  safety_test:
    - Test safety monitoring system
    - Validate safety violation handling
    - Test emergency procedures
    
  performance_test:
    - Test communication performance
    - Validate real-time data exchange
    - Test system response times
```

---

## 11. MONITORING AND DIAGNOSTICS

### 11.1 System Monitoring
- **Communication Health:** Monitor RS485 communication health
- **Module Status:** Monitor individual module status
- **Hardware Status:** Monitor GPIO and hardware status
- **Performance Metrics:** Monitor system performance metrics

### 11.2 Diagnostic Tools
```python
class DiagnosticService:
    def __init__(self):
        self.diagnostic_tools = {
            "communication": self.diagnose_communication,
            "hardware": self.diagnose_hardware,
            "modules": self.diagnose_modules,
            "performance": self.diagnose_performance
        }
        
    async def run_diagnostics(self, diagnostic_type: str) -> Dict:
        """Run system diagnostics"""
        if diagnostic_type in self.diagnostic_tools:
            return await self.diagnostic_tools[diagnostic_type]()
        else:
            raise ValueError(f"Unknown diagnostic type: {diagnostic_type}")
            
    async def diagnose_communication(self) -> Dict:
        """Diagnose communication issues"""
        # Implementation for communication diagnostics
        
    async def diagnose_hardware(self) -> Dict:
        """Diagnose hardware issues"""
        # Implementation for hardware diagnostics
```

---

## 12. REFERENCES

### 12.1 Related Documents
- REQ_BE_01: System Overview
- REQ_BE_02: API Specifications
- REQ_BE_03: Database Design
- REQ_BE_04: Security Requirements
- REQ_BE_06: LiDAR Integration
- Firmware Integration Guide
- Hardware Specifications

### 12.2 Standards References
- Modbus RTU Protocol Specification
- RS485 Communication Standards
- GPIO Interface Standards
- IEC 61508: Functional Safety

---

## 13. APPROVAL

| Role | Name | Date | Signature |
|------|------|------|-----------|
| CTO | [Name] | [Date] | [Signature] |
| Technical Lead | [Name] | [Date] | [Signature] |
| Quality Assurance | [Name] | [Date] | [Signature] |

---

**Document End**
