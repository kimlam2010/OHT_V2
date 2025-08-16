#!/usr/bin/env python3
"""
Hardware Abstraction Layer (HAL) cho OHT-50 RS485
Cung cấp interface thống nhất cho RS485 communication
"""

import time
import serial
import struct
import threading
from enum import Enum
from typing import Optional, List, Dict, Any
from dataclasses import dataclass
from pathlib import Path

class RS485Mode(Enum):
    """RS485 operation modes"""
    RECEIVE = 0
    TRANSMIT = 1
    BIDIRECTIONAL = 2

class RS485Error(Exception):
    """RS485 specific exceptions"""
    pass

@dataclass
class RS485Config:
    """RS485 configuration parameters"""
    device: str = "/dev/ttyOHT485"  # Use udev symlink
    baudrate: int = 9600
    bytesize: int = serial.EIGHTBITS
    parity: str = serial.PARITY_NONE
    stopbits: int = serial.STOPBITS_ONE
    timeout: float = 1.0
    write_timeout: float = 1.0
    
    # GPIO control (CORRECTED)
    tx_pin: int = 46  # GPIO46 (wPi 1) - TX Output
    rx_pin: int = 47  # GPIO47 (wPi 0) - RX Input
    
    # RS485 specific
    rtscts: bool = False
    dsrdtr: bool = False
    xonxoff: bool = False
    
    # Timing
    de_delay: float = 0.001  # 1ms delay after DE assertion
    re_delay: float = 0.001  # 1ms delay after RE assertion

@dataclass
class RS485Stats:
    """RS485 communication statistics"""
    bytes_sent: int = 0
    bytes_received: int = 0
    packets_sent: int = 0
    packets_received: int = 0
    errors: int = 0
    timeouts: int = 0
    crc_errors: int = 0
    framing_errors: int = 0
    overrun_errors: int = 0

class HAL_RS485:
    """
    Hardware Abstraction Layer cho RS485 communication
    """
    
    def __init__(self, config: RS485Config):
        self.config = config
        self.serial_port: Optional[serial.Serial] = None
        self.stats = RS485Stats()
        self.lock = threading.Lock()
        self.is_open = False
        
        # GPIO control
        self._setup_gpio()
    
    def _setup_gpio(self):
        """Setup GPIO cho TX/RX control (CORRECTED)"""
        try:
            # Export GPIO pins
            self._export_gpio(self.config.tx_pin)
            self._export_gpio(self.config.rx_pin)
            
            # Set direction
            self._set_gpio_direction(self.config.tx_pin, "out")  # GPIO46 as output
            self._set_gpio_direction(self.config.rx_pin, "in")   # GPIO47 as input
            
            # Set initial state
            self._set_gpio_value(self.config.tx_pin, 0)  # TX low initially
            
        except Exception as e:
            raise RS485Error(f"GPIO setup failed: {e}")
    
    def _export_gpio(self, pin: int):
        """Export GPIO pin"""
        try:
            with open(f"/sys/class/gpio/export", "w") as f:
                f.write(str(pin))
        except FileExistsError:
            pass  # Already exported
        except Exception as e:
            raise RS485Error(f"Failed to export GPIO{pin}: {e}")
    
    def _set_gpio_direction(self, pin: int, direction: str):
        """Set GPIO direction"""
        try:
            with open(f"/sys/class/gpio/gpio{pin}/direction", "w") as f:
                f.write(direction)
        except Exception as e:
            raise RS485Error(f"Failed to set GPIO{pin} direction: {e}")
    
    def _set_gpio_value(self, pin: int, value: int):
        """Set GPIO value"""
        try:
            with open(f"/sys/class/gpio/gpio{pin}/value", "w") as f:
                f.write(str(value))
        except Exception as e:
            raise RS485Error(f"Failed to set GPIO{pin} value: {e}")
    
    def _get_gpio_value(self, pin: int) -> int:
        """Get GPIO value"""
        try:
            with open(f"/sys/class/gpio/gpio{pin}/value", "r") as f:
                return int(f.read().strip())
        except Exception as e:
            raise RS485Error(f"Failed to get GPIO{pin} value: {e}")
    
    def _cleanup_gpio(self):
        """Cleanup GPIO pins"""
        try:
            # Unexport GPIO pins
            with open(f"/sys/class/gpio/unexport", "w") as f:
                f.write(str(self.config.tx_pin))
        except Exception:
            pass  # Ignore errors during cleanup
        
        try:
            with open(f"/sys/class/gpio/unexport", "w") as f:
                f.write(str(self.config.rx_pin))
        except Exception:
            pass  # Ignore errors during cleanup
    
    def open(self) -> bool:
        """Open RS485 connection"""
        try:
            with self.lock:
                if self.is_open:
                    return True
                
                # Open serial port
                self.serial_port = serial.Serial(
                    port=self.config.device,
                    baudrate=self.config.baudrate,
                    bytesize=self.config.bytesize,
                    parity=self.config.parity,
                    stopbits=self.config.stopbits,
                    timeout=self.config.timeout,
                    write_timeout=self.config.write_timeout,
                    rtscts=self.config.rtscts,
                    dsrdtr=self.config.dsrdtr,
                    xonxoff=self.config.xonxoff
                )
                
                # Set receive mode
                self.set_mode(RS485Mode.RECEIVE)
                
                self.is_open = True
                return True
                
        except Exception as e:
            raise RS485Error(f"Failed to open RS485: {e}")
    
    def close(self):
        """Close RS485 connection"""
        try:
            with self.lock:
                if self.serial_port:
                    self.serial_port.close()
                    self.serial_port = None
                
                # Set receive mode
                self.set_mode(RS485Mode.RECEIVE)
                
                self.is_open = False
                
        except Exception as e:
            raise RS485Error(f"Failed to close RS485: {e}")
    
    def set_mode(self, mode: RS485Mode):
        """Set RS485 mode (receive/transmit) - DISABLED for manual control"""
        # Manual GPIO control - no RS485 mode needed
        pass
    
    def send(self, data: bytes) -> int:
        """Send data over RS485"""
        try:
            with self.lock:
                if not self.is_open or not self.serial_port:
                    raise RS485Error("RS485 not open")
                
                # Set transmit mode
                self.set_mode(RS485Mode.TRANSMIT)
                time.sleep(self.config.de_delay)
                
                # Send data
                bytes_written = self.serial_port.write(data)
                
                # Wait for transmission to complete
                self.serial_port.flush()
                
                # Set receive mode
                self.set_mode(RS485Mode.RECEIVE)
                time.sleep(self.config.re_delay)
                
                # Update statistics
                self.stats.bytes_sent += bytes_written
                self.stats.packets_sent += 1
                
                return bytes_written
                
        except Exception as e:
            self.stats.errors += 1
            raise RS485Error(f"Send failed: {e}")
    
    def receive(self, size: int = 1) -> bytes:
        """Receive data from RS485"""
        try:
            with self.lock:
                if not self.is_open or not self.serial_port:
                    raise RS485Error("RS485 not open")
                
                # Ensure receive mode
                self.set_mode(RS485Mode.RECEIVE)
                
                # Read data
                data = self.serial_port.read(size)
                
                # Update statistics
                self.stats.bytes_received += len(data)
                if len(data) > 0:
                    self.stats.packets_received += 1
                
                return data
                
        except Exception as e:
            self.stats.errors += 1
            raise RS485Error(f"Receive failed: {e}")
    
    def send_receive(self, data: bytes, timeout: float = None) -> bytes:
        """Send data and wait for response"""
        try:
            # Send data
            self.send(data)
            
            # Wait for response
            if timeout is None:
                timeout = self.config.timeout
            
            start_time = time.time()
            response = b""
            
            while time.time() - start_time < timeout:
                chunk = self.receive(1)
                if chunk:
                    response += chunk
                else:
                    time.sleep(0.001)  # 1ms delay
            
            return response
            
        except Exception as e:
            raise RS485Error(f"Send/receive failed: {e}")
    
    def flush(self):
        """Flush input/output buffers"""
        try:
            with self.lock:
                if self.serial_port:
                    self.serial_port.flush()
                    self.serial_port.reset_input_buffer()
                    self.serial_port.reset_output_buffer()
        except Exception as e:
            raise RS485Error(f"Flush failed: {e}")
    
    def get_stats(self) -> RS485Stats:
        """Get communication statistics"""
        return self.stats
    
    def reset_stats(self):
        """Reset communication statistics"""
        self.stats = RS485Stats()
    
    def get_status(self) -> Dict[str, Any]:
        """Get RS485 status"""
        try:
            status = {
                'is_open': self.is_open,
                'device': self.config.device,
                'baudrate': self.config.baudrate,
                'de_pin': self.config.de_pin,
                're_pin': self.config.re_pin,
                'de_value': self._get_gpio_value(self.config.de_pin),
                're_value': self._get_gpio_value(self.config.re_pin),
                'stats': {
                    'bytes_sent': self.stats.bytes_sent,
                    'bytes_received': self.stats.bytes_received,
                    'packets_sent': self.stats.packets_sent,
                    'packets_received': self.stats.packets_received,
                    'errors': self.stats.errors,
                    'timeouts': self.stats.timeouts,
                    'crc_errors': self.stats.crc_errors,
                    'framing_errors': self.stats.framing_errors,
                    'overrun_errors': self.stats.overrun_errors
                }
            }
            
            if self.serial_port:
                status['serial_status'] = {
                    'in_waiting': self.serial_port.in_waiting,
                    'out_waiting': self.serial_port.out_waiting,
                    'cts': self.serial_port.cts,
                    'dsr': self.serial_port.dsr,
                    'ri': self.serial_port.ri,
                    'cd': self.serial_port.cd
                }
            
            return status
            
        except Exception as e:
            raise RS485Error(f"Failed to get status: {e}")
    
    def __enter__(self):
        """Context manager entry"""
        self.open()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit"""
        self.close()

# Example usage
if __name__ == "__main__":
    # Create configuration
    config = RS485Config(
        device="/dev/ttyS1",
        baudrate=9600,
        de_pin=47,  # GPIO1_D3
        re_pin=46   # GPIO1_D2
    )
    
    # Use HAL
    with HAL_RS485(config) as rs485:
        print("RS485 HAL Test")
        
        # Send test data
        test_data = b"OHT50_HAL_TEST\r\n"
        rs485.send(test_data)
        print(f"Sent: {test_data}")
        
        # Get status
        status = rs485.get_status()
        print(f"Status: {status}")
        
        # Get stats
        stats = rs485.get_stats()
        print(f"Stats: {stats}")
