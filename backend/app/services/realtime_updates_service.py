"""
Real-time Updates Service for OHT-50 Backend
Handles WebSocket communication and real-time data streaming
"""

import logging
import asyncio
import json
from typing import Dict, Any, List, Optional, Set
from datetime import datetime, timedelta
from dataclasses import dataclass, asdict
from enum import Enum

from app.core.websocket_service import websocket_service
from app.services.firmware_integration_service import SensorReading, SensorType
from app.services.sensor_data_processor import ProcessingResult

logger = logging.getLogger(__name__)


class UpdateType(Enum):
    """Types of real-time updates"""
    POSITION = "position"
    TELEMETRY = "telemetry"
    SENSOR_DATA = "sensor_data"
    MAP_UPDATE = "map_update"
    ALERT = "alert"
    STATUS = "status"


class UpdateFrequency(Enum):
    """Update frequencies"""
    HIGH = 20  # 20Hz - 50ms
    MEDIUM = 10  # 10Hz - 100ms
    LOW = 5  # 5Hz - 200ms
    VERY_LOW = 1  # 1Hz - 1000ms


@dataclass
class RealtimeUpdate:
    """Real-time update message"""
    update_type: UpdateType
    data: Dict[str, Any]
    timestamp: datetime
    frequency: UpdateFrequency
    priority: int = 1  # 1=high, 2=medium, 3=low


class RealtimeUpdatesService:
    """
    Real-time Updates Service
    
    Manages WebSocket connections and real-time data streaming
    with configurable update frequencies
    """
    
    def __init__(self):
        """Initialize real-time updates service"""
        self.update_queues: Dict[UpdateType, asyncio.Queue] = {}
        self.subscribers: Dict[UpdateType, Set[str]] = {}
        self.update_tasks: Dict[UpdateType, asyncio.Task] = {}
        self.is_running = False
        
        # Update frequencies
        self.frequencies = {
            UpdateType.POSITION: UpdateFrequency.HIGH,  # 20Hz
            UpdateType.TELEMETRY: UpdateFrequency.MEDIUM,  # 10Hz
            UpdateType.SENSOR_DATA: UpdateFrequency.MEDIUM,  # 10Hz
            UpdateType.MAP_UPDATE: UpdateFrequency.LOW,  # 5Hz
            UpdateType.ALERT: UpdateFrequency.VERY_LOW,  # 1Hz
            UpdateType.STATUS: UpdateFrequency.LOW  # 5Hz
        }
        
        # Initialize queues and subscribers
        for update_type in UpdateType:
            self.update_queues[update_type] = asyncio.Queue(maxsize=100)
            self.subscribers[update_type] = set()
        
        logger.info("🔄 Real-time Updates Service initialized")
    
    async def start(self) -> bool:
        """
        Start real-time updates service
        
        Returns:
            True if started successfully
        """
        try:
            if self.is_running:
                logger.warning("⚠️ Real-time updates service already running")
                return True
            
            logger.info("🔄 Starting real-time updates service...")
            
            # Start update tasks for each type
            for update_type in UpdateType:
                task = asyncio.create_task(
                    self._update_task(update_type)
                )
                self.update_tasks[update_type] = task
            
            self.is_running = True
            logger.info("✅ Real-time updates service started successfully")
            return True
            
        except Exception as e:
            logger.error("❌ Failed to start real-time updates service: %s", e)
            return False
    
    async def stop(self):
        """Stop real-time updates service"""
        try:
            if not self.is_running:
                return
            
            logger.info("🛑 Stopping real-time updates service...")
            
            # Cancel all update tasks
            for task in self.update_tasks.values():
                task.cancel()
            
            # Wait for tasks to complete
            await asyncio.gather(*self.update_tasks.values(), return_exceptions=True)
            
            self.is_running = False
            logger.info("✅ Real-time updates service stopped")
            
        except Exception as e:
            logger.error("❌ Error stopping real-time updates service: %s", e)
    
    async def subscribe(self, client_id: str, update_types: List[UpdateType]) -> bool:
        """
        Subscribe client to update types
        
        Args:
            client_id: Client identifier
            update_types: List of update types to subscribe to
            
        Returns:
            True if subscription successful
        """
        try:
            for update_type in update_types:
                self.subscribers[update_type].add(client_id)
            
            logger.info("📡 Client %s subscribed to %d update types", client_id, len(update_types))
            return True
            
        except Exception as e:
            logger.error("❌ Failed to subscribe client %s: %s", client_id, e)
            return False
    
    async def unsubscribe(self, client_id: str, update_types: List[UpdateType] = None) -> bool:
        """
        Unsubscribe client from update types
        
        Args:
            client_id: Client identifier
            update_types: List of update types to unsubscribe from (None = all)
            
        Returns:
            True if unsubscription successful
        """
        try:
            if update_types is None:
                update_types = list(UpdateType)
            
            for update_type in update_types:
                self.subscribers[update_type].discard(client_id)
            
            logger.info("📡 Client %s unsubscribed from %d update types", client_id, len(update_types))
            return True
            
        except Exception as e:
            logger.error("❌ Failed to unsubscribe client %s: %s", client_id, e)
            return False
    
    async def publish_update(self, update: RealtimeUpdate) -> bool:
        """
        Publish real-time update
        
        Args:
            update: Update to publish
            
        Returns:
            True if published successfully
        """
        try:
            # Add to appropriate queue
            queue = self.update_queues[update.update_type]
            
            # Non-blocking put
            try:
                queue.put_nowait(update)
                return True
            except asyncio.QueueFull:
                logger.warning("⚠️ Update queue full for %s, dropping update", update.update_type.value)
                return False
                
        except Exception as e:
            logger.error("❌ Failed to publish update: %s", e)
            return False
    
    async def publish_position_update(self, position_data: Dict[str, Any]) -> bool:
        """
        Publish position update
        
        Args:
            position_data: Position data
            
        Returns:
            True if published successfully
        """
        update = RealtimeUpdate(
            update_type=UpdateType.POSITION,
            data=position_data,
            timestamp=datetime.utcnow(),
            frequency=self.frequencies[UpdateType.POSITION],
            priority=1
        )
        
        return await self.publish_update(update)
    
    async def publish_telemetry_update(self, telemetry_data: Dict[str, Any]) -> bool:
        """
        Publish telemetry update
        
        Args:
            telemetry_data: Telemetry data
            
        Returns:
            True if published successfully
        """
        update = RealtimeUpdate(
            update_type=UpdateType.TELEMETRY,
            data=telemetry_data,
            timestamp=datetime.utcnow(),
            frequency=self.frequencies[UpdateType.TELEMETRY],
            priority=2
        )
        
        return await self.publish_update(update)
    
    async def publish_sensor_data_update(self, sensor_data: Dict[str, Any]) -> bool:
        """
        Publish sensor data update
        
        Args:
            sensor_data: Sensor data
            
        Returns:
            True if published successfully
        """
        update = RealtimeUpdate(
            update_type=UpdateType.SENSOR_DATA,
            data=sensor_data,
            timestamp=datetime.utcnow(),
            frequency=self.frequencies[UpdateType.SENSOR_DATA],
            priority=2
        )
        
        return await self.publish_update(update)
    
    async def publish_map_update(self, map_data: Dict[str, Any]) -> bool:
        """
        Publish map update
        
        Args:
            map_data: Map data
            
        Returns:
            True if published successfully
        """
        update = RealtimeUpdate(
            update_type=UpdateType.MAP_UPDATE,
            data=map_data,
            timestamp=datetime.utcnow(),
            frequency=self.frequencies[UpdateType.MAP_UPDATE],
            priority=3
        )
        
        return await self.publish_update(update)
    
    async def publish_alert(self, alert_data: Dict[str, Any]) -> bool:
        """
        Publish alert
        
        Args:
            alert_data: Alert data
            
        Returns:
            True if published successfully
        """
        update = RealtimeUpdate(
            update_type=UpdateType.ALERT,
            data=alert_data,
            timestamp=datetime.utcnow(),
            frequency=self.frequencies[UpdateType.ALERT],
            priority=1  # High priority for alerts
        )
        
        return await self.publish_update(update)
    
    async def publish_status_update(self, status_data: Dict[str, Any]) -> bool:
        """
        Publish status update
        
        Args:
            status_data: Status data
            
        Returns:
            True if published successfully
        """
        update = RealtimeUpdate(
            update_type=UpdateType.STATUS,
            data=status_data,
            timestamp=datetime.utcnow(),
            frequency=self.frequencies[UpdateType.STATUS],
            priority=3
        )
        
        return await self.publish_update(update)
    
    async def _update_task(self, update_type: UpdateType):
        """
        Update task for specific update type
        
        Args:
            update_type: Type of updates to process
        """
        try:
            queue = self.update_queues[update_type]
            frequency = self.frequencies[update_type]
            interval = 1.0 / frequency.value  # Convert Hz to seconds
            
            logger.info("🔄 Starting update task for %s at %dHz", update_type.value, frequency.value)
            
            while self.is_running:
                try:
                    # Wait for update with timeout
                    update = await asyncio.wait_for(queue.get(), timeout=interval)
                    
                    # Broadcast to subscribers
                    await self._broadcast_update(update)
                    
                    # Mark task as done
                    queue.task_done()
                    
                except asyncio.TimeoutError:
                    # No update received within interval, continue
                    continue
                except Exception as e:
                    logger.error("❌ Error in update task for %s: %s", update_type.value, e)
                    await asyncio.sleep(0.1)  # Brief pause before retry
            
        except asyncio.CancelledError:
            logger.info("🛑 Update task for %s cancelled", update_type.value)
        except Exception as e:
            logger.error("❌ Fatal error in update task for %s: %s", update_type.value, e)
    
    async def _broadcast_update(self, update: RealtimeUpdate):
        """
        Broadcast update to all subscribers
        
        Args:
            update: Update to broadcast
        """
        try:
            subscribers = self.subscribers[update.update_type]
            
            if not subscribers:
                return  # No subscribers
            
            # Prepare message
            message = {
                "type": update.update_type.value,
                "data": update.data,
                "timestamp": update.timestamp.isoformat(),
                "priority": update.priority
            }
            
            # Broadcast to all subscribers
            for client_id in list(subscribers):  # Copy list to avoid modification during iteration
                try:
                    await websocket_service.send_message(client_id, message)
                except Exception as e:
                    logger.warning("⚠️ Failed to send update to client %s: %s", client_id, e)
                    # Remove failed client from subscribers
                    subscribers.discard(client_id)
            
            logger.debug("📡 Broadcasted %s update to %d subscribers", 
                        update.update_type.value, len(subscribers))
            
        except Exception as e:
            logger.error("❌ Failed to broadcast update: %s", e)
    
    async def get_service_stats(self) -> Dict[str, Any]:
        """
        Get real-time updates service statistics
        
        Returns:
            Service statistics
        """
        try:
            stats = {
                "is_running": self.is_running,
                "update_types": len(UpdateType),
                "active_tasks": len([t for t in self.update_tasks.values() if not t.done()]),
                "subscribers": {
                    update_type.value: len(subscribers) 
                    for update_type, subscribers in self.subscribers.items()
                },
                "queue_sizes": {
                    update_type.value: queue.qsize() 
                    for update_type, queue in self.update_queues.items()
                },
                "frequencies": {
                    update_type.value: frequency.value 
                    for update_type, frequency in self.frequencies.items()
                }
            }
            
            return stats
            
        except Exception as e:
            logger.error("❌ Failed to get service stats: %s", e)
            return {}
    
    async def set_update_frequency(self, update_type: UpdateType, frequency: UpdateFrequency) -> bool:
        """
        Set update frequency for specific update type
        
        Args:
            update_type: Type of update
            frequency: New frequency
            
        Returns:
            True if frequency set successfully
        """
        try:
            self.frequencies[update_type] = frequency
            logger.info("🔄 Set %s update frequency to %dHz", update_type.value, frequency.value)
            return True
            
        except Exception as e:
            logger.error("❌ Failed to set update frequency: %s", e)
            return False
    
    async def get_subscriber_count(self, update_type: UpdateType) -> int:
        """
        Get number of subscribers for update type
        
        Args:
            update_type: Type of update
            
        Returns:
            Number of subscribers
        """
        return len(self.subscribers[update_type])
    
    async def clear_queues(self):
        """Clear all update queues"""
        try:
            for queue in self.update_queues.values():
                while not queue.empty():
                    try:
                        queue.get_nowait()
                        queue.task_done()
                    except asyncio.QueueEmpty:
                        break
            
            logger.info("🧹 Cleared all update queues")
            
        except Exception as e:
            logger.error("❌ Failed to clear queues: %s", e)


# Global real-time updates service instance
realtime_service = RealtimeUpdatesService()
