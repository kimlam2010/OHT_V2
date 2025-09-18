"""
WebSocket Log Service for Real-time System Log Streaming
Handles real-time log broadcasting via WebSocket connections
Issue #78 - WebSocket cho system logs
"""

import asyncio
import logging
import time
from typing import Dict, Any, List, Optional, Set
from datetime import datetime, timezone
from dataclasses import dataclass
from enum import Enum
import json

from app.core.websocket_service import websocket_service, WebSocketMessage

logger = logging.getLogger(__name__)


class LogLevel(Enum):
    """Log levels"""
    DEBUG = "debug"
    INFO = "info"
    WARNING = "warning"
    ERROR = "error"
    CRITICAL = "critical"


@dataclass
class SystemLogEntry:
    """System log entry data structure"""
    id: int
    level: str
    message: str
    source: str
    timestamp: datetime
    user_id: Optional[int] = None
    session_id: Optional[str] = None
    metadata: Dict[str, Any] = None
    
    def __post_init__(self):
        if self.metadata is None:
            self.metadata = {}


class LogCapture(logging.Handler):
    """Custom logging handler to capture logs for WebSocket streaming"""
    
    def __init__(self, log_service):
        super().__init__()
        self.log_service = log_service
        self.log_counter = 0
        
    def emit(self, record):
        """Handle log record emission"""
        try:
            self.log_counter += 1
            
            # Create log entry
            log_entry = SystemLogEntry(
                id=self.log_counter,
                level=record.levelname.lower(),
                message=record.getMessage(),
                source=record.name,
                timestamp=datetime.fromtimestamp(record.created, tz=timezone.utc),
                metadata={
                    "module": record.module,
                    "function": record.funcName,
                    "line": record.lineno,
                    "process": record.process,
                    "thread": record.thread
                }
            )
            
            # Queue for WebSocket broadcast
            asyncio.create_task(self.log_service.broadcast_log_entry(log_entry))
            
        except Exception as e:
            # Don't log errors from logging handler to avoid recursion
            print(f"LogCapture error: {e}")


class WebSocketLogService:
    """
    Service for broadcasting system logs via WebSocket
    Provides real-time log streaming to connected clients
    """
    
    def __init__(self):
        self.log_subscribers: Set[str] = set()
        self.log_buffer: List[SystemLogEntry] = []
        self.max_buffer_size = 1000
        self.log_broadcast_queue = asyncio.Queue()
        self.is_running = False
        self.log_capture_handler = None
        
        # Log level filters
        self.level_filters = {
            "debug": ["debug", "info", "warning", "error", "critical"],
            "info": ["info", "warning", "error", "critical"],
            "warning": ["warning", "error", "critical"],
            "error": ["error", "critical"],
            "critical": ["critical"]
        }
        
        logger.info("📝 WebSocketLogService initialized")
    
    async def start(self):
        """Start WebSocket log service"""
        if self.is_running:
            logger.warning("⚠️ WebSocket log service already running")
            return
        
        try:
            self.is_running = True
            
            # Start log capture
            self._start_log_capture()
            
            # Start broadcast worker
            asyncio.create_task(self._log_broadcast_worker())
            
            logger.info("🚀 WebSocket Log service started")
            
        except Exception as e:
            logger.error(f"❌ Failed to start WebSocket log service: {e}")
            self.is_running = False
            raise
    
    async def stop(self):
        """Stop WebSocket log service"""
        self.is_running = False
        
        # Stop log capture
        self._stop_log_capture()
        
        logger.info("🛑 WebSocket Log service stopped")
    
    def _start_log_capture(self):
        """Start capturing logs from Python logging system"""
        try:
            # Create log capture handler
            self.log_capture_handler = LogCapture(self)
            self.log_capture_handler.setLevel(logging.DEBUG)
            
            # Add to root logger
            root_logger = logging.getLogger()
            root_logger.addHandler(self.log_capture_handler)
            
            logger.info("📡 Log capture started")
            
        except Exception as e:
            logger.error(f"❌ Failed to start log capture: {e}")
    
    def _stop_log_capture(self):
        """Stop capturing logs"""
        try:
            if self.log_capture_handler:
                root_logger = logging.getLogger()
                root_logger.removeHandler(self.log_capture_handler)
                self.log_capture_handler = None
                
            logger.info("📡 Log capture stopped")
            
        except Exception as e:
            logger.error(f"❌ Failed to stop log capture: {e}")
    
    async def broadcast_log_entry(self, log_entry: SystemLogEntry):
        """Broadcast log entry to WebSocket clients"""
        try:
            # Add to buffer
            self.log_buffer.append(log_entry)
            
            # Limit buffer size
            if len(self.log_buffer) > self.max_buffer_size:
                self.log_buffer.pop(0)
            
            # Convert to message format (Issue #78 compliant)
            log_data = self._convert_log_to_response(log_entry)
            
            # Create WebSocket message
            message = WebSocketMessage(
                type="log_entry",
                data={
                    "log": log_data,
                    "action": "new"
                },
                timestamp=datetime.now(timezone.utc)
            )
            
            # Queue for broadcast
            await self.log_broadcast_queue.put(message)
            
        except Exception as e:
            logger.error(f"❌ Failed to broadcast log entry: {e}")
    
    async def send_recent_logs(self, websocket, level_filter: str = "info", limit: int = 50):
        """Send recent logs to newly connected client"""
        try:
            # Filter logs by level
            filtered_logs = []
            allowed_levels = self.level_filters.get(level_filter, ["info", "warning", "error", "critical"])
            
            for log_entry in self.log_buffer[-limit:]:
                if log_entry.level in allowed_levels:
                    filtered_logs.append(log_entry)
            
            # Convert to response format
            log_responses = [
                self._convert_log_to_response(log_entry) 
                for log_entry in filtered_logs
            ]
            
            # Send as WebSocket message
            message = WebSocketMessage(
                type="recent_logs",
                data={
                    "logs": log_responses,
                    "total_count": len(log_responses),
                    "level_filter": level_filter,
                    "limit": limit
                },
                timestamp=datetime.now(timezone.utc)
            )
            
            await websocket_service.send_to_client(websocket, message)
            
            logger.info(f"📨 Sent {len(log_responses)} recent logs to client")
            
        except Exception as e:
            logger.error(f"❌ Failed to send recent logs: {e}")
    
    def _convert_log_to_response(self, log_entry: SystemLogEntry) -> Dict[str, Any]:
        """Convert LogEntry to response format matching Issue #78 requirements"""
        return {
            "id": log_entry.id,
            "level": log_entry.level,
            "message": log_entry.message,
            "source": log_entry.source,
            "timestamp": log_entry.timestamp.isoformat(),
            "user_id": log_entry.user_id,
            "session_id": log_entry.session_id,
            "metadata": log_entry.metadata
        }
    
    async def _log_broadcast_worker(self):
        """Background worker for broadcasting logs"""
        try:
            while self.is_running:
                try:
                    # Wait for log message with timeout
                    message = await asyncio.wait_for(
                        self.log_broadcast_queue.get(), 
                        timeout=1.0
                    )
                    
                    # Broadcast to log subscribers
                    await websocket_service.broadcast_to_subscribers(message, "logs")
                    
                except asyncio.TimeoutError:
                    # No messages in queue, continue
                    continue
                except Exception as e:
                    logger.error(f"❌ Log broadcast worker error: {e}")
                    await asyncio.sleep(1)
                    
        except asyncio.CancelledError:
            logger.info("🛑 Log broadcast worker cancelled")
        except Exception as e:
            logger.error(f"❌ Log broadcast worker failed: {e}")
    
    async def get_log_service_stats(self) -> Dict[str, Any]:
        """Get WebSocket log service statistics"""
        try:
            return {
                "service_status": "running" if self.is_running else "stopped",
                "buffer_size": len(self.log_buffer),
                "max_buffer_size": self.max_buffer_size,
                "queue_size": self.log_broadcast_queue.qsize(),
                "subscriber_count": len(self.log_subscribers),
                "capture_active": self.log_capture_handler is not None,
                "last_updated": datetime.now(timezone.utc).isoformat()
            }
            
        except Exception as e:
            logger.error(f"❌ Failed to get log service stats: {e}")
            return {"error": str(e)}
    
    async def set_log_level_filter(self, websocket, level: str):
        """Set log level filter for specific client"""
        try:
            # Store filter in connection metadata
            if websocket in websocket_service.connection_metadata:
                websocket_service.connection_metadata[websocket]["log_level_filter"] = level
                
            logger.info(f"🔧 Set log level filter to {level} for client")
            
        except Exception as e:
            logger.error(f"❌ Failed to set log level filter: {e}")
    
    async def get_filtered_logs(self, level: str = "info", limit: int = 100) -> List[Dict[str, Any]]:
        """Get filtered logs from buffer"""
        try:
            allowed_levels = self.level_filters.get(level, ["info", "warning", "error", "critical"])
            
            filtered_logs = []
            for log_entry in reversed(self.log_buffer):  # Most recent first
                if log_entry.level in allowed_levels:
                    filtered_logs.append(self._convert_log_to_response(log_entry))
                    
                if len(filtered_logs) >= limit:
                    break
            
            return filtered_logs
            
        except Exception as e:
            logger.error(f"❌ Failed to get filtered logs: {e}")
            return []


# Global instance
websocket_log_service = WebSocketLogService()
