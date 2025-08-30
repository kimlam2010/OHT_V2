"""
SafetyService - Hệ thống an toàn
"""

import asyncio
import logging
import math
from datetime import datetime, timedelta
from typing import List, Optional, Dict, Any
from uuid import UUID

from app.models.safety import (
    SafetyZone, SafetyZoneType, CollisionAlert, EmergencyStatus, EmergencyLevel,
    ZoneStatus, SafetyStatus, SafetyConfig, EmergencyProcedure, SafetyEvent
)
from app.models.movement import Position
from app.services.log_service import LogService


logger = logging.getLogger(__name__)


class SafetyService:
    """Service quản lý hệ thống an toàn"""
    
    def __init__(self, log_service: LogService):
        self.log_service = log_service
        self.safety_zones: Dict[UUID, SafetyZone] = {}
        self.active_alerts: Dict[UUID, CollisionAlert] = {}
        self.emergency_status = EmergencyStatus()
        self.safety_config = SafetyConfig()
        self.emergency_procedures: Dict[UUID, EmergencyProcedure] = {}
        self.safety_events: List[SafetyEvent] = []
        self._lock = asyncio.Lock()
        
        # Monitoring state
        self.is_monitoring = False
        self.monitoring_task: Optional[asyncio.Task] = None
        self.current_position = Position(x=0, y=0, z=0)
        
        # Initialize default safety zones
        self._initialize_default_zones()
        
        logger.info("SafetyService initialized")
    
    async def handle_estop(self) -> EmergencyStatus:
        """Xử lý E-Stop"""
        async with self._lock:
            try:
                logger.warning("E-STOP ACTIVATED")
                
                # Update emergency status
                self.emergency_status.is_emergency = True
                self.emergency_status.emergency_level = EmergencyLevel.CRITICAL
                self.emergency_status.emergency_type = "emergency_stop"
                self.emergency_status.timestamp = datetime.utcnow()
                self.emergency_status.description = "Emergency stop activated"
                self.emergency_status.actions_taken = ["Stop all movement", "Disable motors", "Activate safety protocols"]
                
                # Create safety event
                event = SafetyEvent(
                    event_type="emergency_stop",
                    severity=EmergencyLevel.CRITICAL,
                    description="Emergency stop activated",
                    source="hardware_estop",
                    data={"position": self.current_position.dict()}
                )
                self.safety_events.append(event)
                
                # Execute emergency procedure
                await self.execute_emergency_procedure()
                
                logger.info("E-Stop handled successfully")
                return self.emergency_status
                
            except Exception as e:
                logger.error(f"Error handling E-Stop: {e}")
                raise
    
    async def monitor_safety_zones(self) -> List[ZoneStatus]:
        """Giám sát vùng an toàn"""
        async with self._lock:
            try:
                zone_statuses = []
                
                for zone_id, zone in self.safety_zones.items():
                    if not zone.is_active:
                        continue
                    
                    # Calculate distance to zone center
                    distance = math.sqrt(
                        (self.current_position.x - zone.center_x) ** 2 +
                        (self.current_position.y - zone.center_y) ** 2
                    )
                    
                    # Check if in zone
                    in_zone = distance <= zone.radius
                    occupancy = 1 if in_zone else 0
                    
                    # Get active alerts for this zone
                    zone_alerts = [
                        alert for alert in self.active_alerts.values()
                        if self._is_alert_in_zone(alert, zone)
                    ]
                    
                    zone_status = ZoneStatus(
                        zone_id=zone_id,
                        zone_name=zone.name,
                        zone_type=zone.zone_type,
                        is_active=zone.is_active,
                        current_occupancy=occupancy,
                        last_scan_time=datetime.utcnow(),
                        alerts=zone_alerts
                    )
                    
                    zone_statuses.append(zone_status)
                    
                    # Check for violations
                    if in_zone:
                        await self._handle_zone_violation(zone, distance)
                
                return zone_statuses
                
            except Exception as e:
                logger.error(f"Error monitoring safety zones: {e}")
                return []
    
    async def detect_collision(self) -> Optional[CollisionAlert]:
        """Phát hiện va chạm"""
        async with self._lock:
            try:
                # Simulate collision detection
                # In real implementation, this would integrate with sensors
                
                # Check for obstacles in current position
                obstacle_distance = self._simulate_obstacle_detection()
                
                if obstacle_distance < self.safety_config.min_safe_distance:
                    # Create collision alert
                    alert = CollisionAlert(
                        alert_type="obstacle_detected",
                        severity=EmergencyLevel.HIGH if obstacle_distance < 50 else EmergencyLevel.MEDIUM,
                        position_x=self.current_position.x,
                        position_y=self.current_position.y,
                        distance=obstacle_distance,
                        object_type="unknown_obstacle",
                        confidence=0.8
                    )
                    
                    self.active_alerts[alert.id] = alert
                    
                    # Create safety event
                    event = SafetyEvent(
                        event_type="collision_detected",
                        severity=alert.severity,
                        description=f"Collision detected at distance {obstacle_distance}mm",
                        source="collision_detection",
                        data={"alert_id": str(alert.id), "distance": obstacle_distance}
                    )
                    self.safety_events.append(event)
                    
                    logger.warning(f"Collision detected: distance={obstacle_distance}mm")
                    return alert
                
                return None
                
            except Exception as e:
                logger.error(f"Error detecting collision: {e}")
                return None
    
    async def execute_emergency_procedure(self) -> bool:
        """Thực hiện thủ tục khẩn cấp"""
        async with self._lock:
            try:
                logger.info("Executing emergency procedure")
                
                # Find appropriate emergency procedure
                procedure = self._find_emergency_procedure(self.emergency_status.emergency_level)
                
                if not procedure:
                    logger.warning("No emergency procedure found for current level")
                    return False
                
                # Execute procedure steps
                for i, step in enumerate(procedure.steps):
                    logger.info(f"Emergency procedure step {i+1}: {step}")
                    
                    # Simulate step execution
                    await asyncio.sleep(0.1)  # Simulate processing time
                    
                    # Add step to actions taken
                    self.emergency_status.actions_taken.append(f"Step {i+1}: {step}")
                
                # Mark emergency as resolved
                self.emergency_status.is_resolved = True
                self.emergency_status.resolution_time = datetime.utcnow()
                
                # Create safety event
                event = SafetyEvent(
                    event_type="emergency_procedure_completed",
                    severity=EmergencyLevel.MEDIUM,
                    description=f"Emergency procedure '{procedure.name}' completed",
                    source="safety_service",
                    data={"procedure_id": str(procedure.id), "steps_executed": len(procedure.steps)}
                )
                self.safety_events.append(event)
                
                logger.info("Emergency procedure completed successfully")
                return True
                
            except Exception as e:
                logger.error(f"Error executing emergency procedure: {e}")
                return False
    
    async def get_safety_status(self) -> SafetyStatus:
        """Lấy trạng thái an toàn tổng thể"""
        async with self._lock:
            try:
                # Get zone statuses
                zone_statuses = await self.monitor_safety_zones()
                
                # Calculate overall health
                total_zones = len(self.safety_zones)
                active_zones = len([z for z in zone_statuses if z.is_active])
                zones_with_alerts = len([z for z in zone_statuses if z.alerts])
                
                health = 1.0
                if total_zones > 0:
                    health = (active_zones - zones_with_alerts) / total_zones
                
                # Count active alerts and errors
                active_alerts_count = len(self.active_alerts)
                active_errors_count = len([a for a in self.active_alerts.values() if a.severity in [EmergencyLevel.HIGH, EmergencyLevel.CRITICAL]])
                
                safety_status = SafetyStatus(
                    system_status="normal" if not self.emergency_status.is_emergency else "emergency",
                    emergency_status=self.emergency_status,
                    active_zones=zone_statuses,
                    recent_alerts=list(self.active_alerts.values())[-10:],  # Last 10 alerts
                    estop_status=self.emergency_status.is_emergency and self.emergency_status.emergency_type == "emergency_stop",
                    safety_system_health=health,
                    last_update=datetime.utcnow(),
                    active_alerts=active_alerts_count,
                    active_errors=active_errors_count
                )
                
                return safety_status
                
            except Exception as e:
                logger.error(f"Error getting safety status: {e}")
                raise
    
    async def add_safety_zone(self, zone_data: dict) -> SafetyZone:
        """Thêm vùng an toàn"""
        async with self._lock:
            try:
                zone = SafetyZone(**zone_data)
                self.safety_zones[zone.id] = zone
                
                logger.info(f"Safety zone '{zone.name}' added")
                return zone
                
            except Exception as e:
                logger.error(f"Error adding safety zone: {e}")
                raise
    
    async def remove_safety_zone(self, zone_id: str) -> bool:
        """Xóa vùng an toàn"""
        async with self._lock:
            try:
                zone_uuid = UUID(zone_id)
                if zone_uuid in self.safety_zones:
                    zone_name = self.safety_zones[zone_uuid].name
                    del self.safety_zones[zone_uuid]
                    logger.info(f"Safety zone '{zone_name}' removed")
                    return True
                
                logger.warning(f"Safety zone {zone_id} not found")
                return False
                
            except Exception as e:
                logger.error(f"Error removing safety zone {zone_id}: {e}")
                return False
    
    async def update_safety_config(self, config_data: dict) -> SafetyConfig:
        """Cập nhật cấu hình an toàn"""
        async with self._lock:
            try:
                for key, value in config_data.items():
                    if hasattr(self.safety_config, key):
                        setattr(self.safety_config, key, value)
                
                logger.info("Safety configuration updated")
                return self.safety_config
                
            except Exception as e:
                logger.error(f"Error updating safety config: {e}")
                raise
    
    async def start_monitoring(self):
        """Bắt đầu giám sát an toàn"""
        if self.is_monitoring:
            logger.warning("Safety monitoring already started")
            return
        
        self.is_monitoring = True
        self.monitoring_task = asyncio.create_task(self._monitoring_loop())
        logger.info("Safety monitoring started")
    
    async def stop_monitoring(self):
        """Dừng giám sát an toàn"""
        if not self.is_monitoring:
            logger.warning("Safety monitoring not started")
            return
        
        self.is_monitoring = False
        if self.monitoring_task:
            self.monitoring_task.cancel()
            try:
                await self.monitoring_task
            except asyncio.CancelledError:
                pass
        
        logger.info("Safety monitoring stopped")
    
    async def update_position(self, position: Position):
        """Cập nhật vị trí hiện tại"""
        async with self._lock:
            self.current_position = position
    
    async def resolve_alert(self, alert_id: str) -> bool:
        """Giải quyết cảnh báo"""
        async with self._lock:
            try:
                alert_uuid = UUID(alert_id)
                if alert_uuid in self.active_alerts:
                    alert = self.active_alerts[alert_uuid]
                    alert.is_resolved = True
                    alert.resolution_time = datetime.utcnow()
                    
                    # Move to resolved alerts (could be stored separately)
                    del self.active_alerts[alert_uuid]
                    
                    logger.info(f"Alert {alert_id} resolved")
                    return True
                
                logger.warning(f"Alert {alert_id} not found")
                return False
                
            except Exception as e:
                logger.error(f"Error resolving alert {alert_id}: {e}")
                return False
    
    async def get_safety_events(self, hours: int = 24) -> List[SafetyEvent]:
        """Lấy sự kiện an toàn trong khoảng thời gian"""
        cutoff_time = datetime.utcnow() - timedelta(hours=hours)
        return [event for event in self.safety_events if event.timestamp >= cutoff_time]
    
    async def _monitoring_loop(self):
        """Vòng lặp giám sát an toàn"""
        try:
            while self.is_monitoring:
                # Monitor safety zones
                await self.monitor_safety_zones()
                
                # Detect collisions
                await self.detect_collision()
                
                # Check for emergency conditions
                await self._check_emergency_conditions()
                
                # Clean up old alerts
                await self._cleanup_old_alerts()
                
                # Sleep for monitoring interval
                await asyncio.sleep(0.1)  # 100ms monitoring interval
                
        except asyncio.CancelledError:
            logger.info("Safety monitoring loop cancelled")
        except Exception as e:
            logger.error(f"Error in safety monitoring loop: {e}")
    
    async def _check_emergency_conditions(self):
        """Kiểm tra điều kiện khẩn cấp"""
        # Check if too many high-severity alerts
        high_severity_alerts = [
            alert for alert in self.active_alerts.values()
            if alert.severity in [EmergencyLevel.HIGH, EmergencyLevel.CRITICAL]
        ]
        
        if len(high_severity_alerts) >= 3:
            # Trigger emergency
            self.emergency_status.is_emergency = True
            self.emergency_status.emergency_level = EmergencyLevel.HIGH
            self.emergency_status.emergency_type = "multiple_critical_alerts"
            self.emergency_status.description = f"Multiple critical alerts: {len(high_severity_alerts)}"
            
            logger.warning(f"Emergency triggered: {len(high_severity_alerts)} critical alerts")
    
    async def _cleanup_old_alerts(self):
        """Dọn dẹp cảnh báo cũ"""
        cutoff_time = datetime.utcnow() - timedelta(hours=self.safety_config.alert_retention_hours)
        
        old_alerts = [
            alert_id for alert_id, alert in self.active_alerts.items()
            if alert.timestamp < cutoff_time
        ]
        
        for alert_id in old_alerts:
            del self.active_alerts[alert_id]
        
        if old_alerts:
            logger.info(f"Cleaned up {len(old_alerts)} old alerts")
    
    def _initialize_default_zones(self):
        """Khởi tạo vùng an toàn mặc định"""
        # Add some default safety zones
        default_zones = [
            {
                "name": "Restricted Area 1",
                "zone_type": SafetyZoneType.RESTRICTED,
                "center_x": 1000,
                "center_y": 1000,
                "radius": 200
            },
            {
                "name": "Slow Zone 1",
                "zone_type": SafetyZoneType.SLOW,
                "center_x": 500,
                "center_y": 500,
                "radius": 150,
                "max_speed": 100
            }
        ]
        
        for zone_data in default_zones:
            zone = SafetyZone(**zone_data)
            self.safety_zones[zone.id] = zone
    
    def _find_emergency_procedure(self, level: EmergencyLevel) -> Optional[EmergencyProcedure]:
        """Tìm thủ tục khẩn cấp phù hợp"""
        for procedure in self.emergency_procedures.values():
            if procedure.emergency_level == level and procedure.is_active:
                return procedure
        return None
    
    def _is_alert_in_zone(self, alert: CollisionAlert, zone: SafetyZone) -> bool:
        """Kiểm tra cảnh báo có trong vùng không"""
        distance = math.sqrt(
            (alert.position_x - zone.center_x) ** 2 +
            (alert.position_y - zone.center_y) ** 2
        )
        return distance <= zone.radius
    
    def _handle_zone_violation(self, zone: SafetyZone, distance: float):
        """Xử lý vi phạm vùng an toàn"""
        if zone.zone_type == SafetyZoneType.RESTRICTED:
            # Trigger emergency stop
            asyncio.create_task(self.handle_estop())
        elif zone.zone_type == SafetyZoneType.WARNING:
            # Create warning alert
            alert = CollisionAlert(
                alert_type="zone_violation",
                severity=EmergencyLevel.MEDIUM,
                position_x=self.current_position.x,
                position_y=self.current_position.y,
                distance=distance,
                object_type="zone_violation",
                confidence=1.0
            )
            self.active_alerts[alert.id] = alert
    
    def _simulate_obstacle_detection(self) -> float:
        """Mô phỏng phát hiện vật cản"""
        # Simulate obstacle detection based on current position
        # In real implementation, this would integrate with sensors
        
        # Simple simulation: obstacles at certain positions
        obstacles = [
            (800, 800, 50),  # (x, y, distance)
            (1200, 1200, 30),
            (400, 600, 80)
        ]
        
        min_distance = float('inf')
        for ox, oy, base_distance in obstacles:
            distance = math.sqrt(
                (self.current_position.x - ox) ** 2 +
                (self.current_position.y - oy) ** 2
            )
            if distance < base_distance:
                min_distance = min(min_distance, distance)
        
        return min_distance if min_distance != float('inf') else 1000.0
