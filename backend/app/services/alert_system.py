"""
Alert System Service for OHT-50
Advanced alert management and processing system
Target: < 100ms alert response time for critical alerts
"""

import asyncio
import logging
import time
from typing import Dict, Any, List, Optional, Callable, Set
from datetime import datetime, timezone, timedelta
from dataclasses import dataclass, field
from enum import Enum
import uuid

logger = logging.getLogger(__name__)


class AlertType(Enum):
    """Types of alerts"""
    SYSTEM = "system"
    PERFORMANCE = "performance"
    SAFETY = "safety"
    ANOMALY = "anomaly"
    COMMUNICATION = "communication"
    HARDWARE = "hardware"
    SOFTWARE = "software"
    CUSTOM = "custom"


class AlertSeverity(Enum):
    """Alert severity levels"""
    INFO = "info"
    WARNING = "warning"
    ERROR = "error"
    CRITICAL = "critical"
    EMERGENCY = "emergency"


class AlertStatus(Enum):
    """Alert status"""
    ACTIVE = "active"
    ACKNOWLEDGED = "acknowledged"
    RESOLVED = "resolved"
    SUPPRESSED = "suppressed"
    EXPIRED = "expired"


class AlertSource(Enum):
    """Alert sources"""
    TELEMETRY = "telemetry"
    PERFORMANCE_MONITOR = "performance_monitor"
    ANOMALY_DETECTOR = "anomaly_detector"
    SAFETY_SYSTEM = "safety_system"
    HARDWARE_MONITOR = "hardware_monitor"
    USER_INPUT = "user_input"
    SYSTEM = "system"


@dataclass
class AlertRule:
    """Alert rule configuration"""
    rule_id: str
    name: str
    description: str
    alert_type: AlertType
    severity: AlertSeverity
    source: AlertSource
    condition: Callable[[Dict[str, Any]], bool]
    enabled: bool = True
    cooldown_seconds: int = 60
    max_occurrences: int = 10
    suppression_rules: List[str] = field(default_factory=list)
    notification_channels: List[str] = field(default_factory=list)
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class Alert:
    """Alert instance"""
    alert_id: str
    rule_id: str
    alert_type: AlertType
    severity: AlertSeverity
    source: AlertSource
    title: str
    message: str
    status: AlertStatus
    created_at: datetime
    updated_at: datetime
    acknowledged_at: Optional[datetime] = None
    resolved_at: Optional[datetime] = None
    acknowledged_by: Optional[str] = None
    resolved_by: Optional[str] = None
    data: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class AlertStatistics:
    """Alert statistics"""
    total_alerts: int = 0
    active_alerts: int = 0
    acknowledged_alerts: int = 0
    resolved_alerts: int = 0
    alerts_by_severity: Dict[str, int] = field(default_factory=dict)
    alerts_by_type: Dict[str, int] = field(default_factory=dict)
    alerts_by_source: Dict[str, int] = field(default_factory=dict)
    avg_response_time_ms: float = 0.0
    last_updated: datetime = field(default_factory=lambda: datetime.now(timezone.utc))


class AlertSystem:
    """
    Advanced alert management system
    Handles alert generation, processing, and management
    """

    def __init__(self):
        self.alert_rules: Dict[str, AlertRule] = {}
        self.active_alerts: Dict[str, Alert] = {}
        self.alert_history: List[Alert] = []
        self.alert_statistics = AlertStatistics()
        self.alert_handlers: Dict[AlertType, List[Callable]] = {}
        self.suppression_rules: Dict[str, Set[str]] = {}
        self.cooldown_tracker: Dict[str, datetime] = {}
        self.occurrence_tracker: Dict[str, int] = {}
        
        # Performance tracking
        self.processing_times: List[float] = []
        self.alert_counters: Dict[str, int] = {}
        
        # Initialize default rules
        self._initialize_default_rules()
        
        logger.info("AlertSystem initialized with %d default rules", len(self.alert_rules))

    def _initialize_default_rules(self):
        """Initialize default alert rules"""
        # System performance alerts
        self.add_alert_rule(AlertRule(
            rule_id="cpu_high_usage",
            name="High CPU Usage",
            description="CPU usage exceeds threshold",
            alert_type=AlertType.PERFORMANCE,
            severity=AlertSeverity.WARNING,
            source=AlertSource.PERFORMANCE_MONITOR,
            condition=lambda data: data.get("cpu_usage", 0) > 80.0,
            cooldown_seconds=300,
            notification_channels=["email", "dashboard"]
        ))
        
        self.add_alert_rule(AlertRule(
            rule_id="memory_high_usage",
            name="High Memory Usage",
            description="Memory usage exceeds threshold",
            alert_type=AlertType.PERFORMANCE,
            severity=AlertSeverity.WARNING,
            source=AlertSource.PERFORMANCE_MONITOR,
            condition=lambda data: data.get("memory_usage", 0) > 85.0,
            cooldown_seconds=300,
            notification_channels=["email", "dashboard"]
        ))
        
        # Safety alerts
        self.add_alert_rule(AlertRule(
            rule_id="emergency_stop",
            name="Emergency Stop",
            description="Emergency stop activated",
            alert_type=AlertType.SAFETY,
            severity=AlertSeverity.EMERGENCY,
            source=AlertSource.SAFETY_SYSTEM,
            condition=lambda data: data.get("emergency_stop", False),
            cooldown_seconds=0,
            notification_channels=["email", "sms", "dashboard", "audio"]
        ))
        
        # Anomaly alerts
        self.add_alert_rule(AlertRule(
            rule_id="anomaly_detected",
            name="Anomaly Detected",
            description="Anomaly detected in system data",
            alert_type=AlertType.ANOMALY,
            severity=AlertSeverity.WARNING,
            source=AlertSource.ANOMALY_DETECTOR,
            condition=lambda data: data.get("is_anomaly", False) and data.get("severity") in ["high", "critical"],
            cooldown_seconds=60,
            notification_channels=["dashboard"]
        ))
        
        # Communication alerts
        self.add_alert_rule(AlertRule(
            rule_id="communication_lost",
            name="Communication Lost",
            description="Communication with device lost",
            alert_type=AlertType.COMMUNICATION,
            severity=AlertSeverity.ERROR,
            source=AlertSource.TELEMETRY,
            condition=lambda data: data.get("communication_status") == "lost",
            cooldown_seconds=120,
            notification_channels=["email", "dashboard"]
        ))

    def add_alert_rule(self, rule: AlertRule):
        """Add alert rule"""
        self.alert_rules[rule.rule_id] = rule
        self.alert_counters[rule.rule_id] = 0
        logger.info(f"Added alert rule: {rule.name} ({rule.rule_id})")

    def remove_alert_rule(self, rule_id: str):
        """Remove alert rule"""
        if rule_id in self.alert_rules:
            del self.alert_rules[rule_id]
            if rule_id in self.alert_counters:
                del self.alert_counters[rule_id]
            logger.info(f"Removed alert rule: {rule_id}")

    async def process_alert_data(self, source: AlertSource, data: Dict[str, Any]) -> List[Alert]:
        """
        Process incoming data and generate alerts based on rules
        """
        start_time = time.perf_counter()
        generated_alerts = []
        
        try:
            # Check all enabled rules for this source
            for rule_id, rule in self.alert_rules.items():
                if not rule.enabled or rule.source != source:
                    continue
                
                # Check cooldown
                if self._is_in_cooldown(rule_id):
                    continue
                
                # Check occurrence limit
                if self._exceeds_occurrence_limit(rule_id, rule):
                    continue
                
                # Check suppression rules
                if self._is_suppressed(rule_id, data):
                    continue
                
                # Evaluate condition
                try:
                    if rule.condition(data):
                        alert = await self._create_alert(rule, data)
                        if alert:
                            generated_alerts.append(alert)
                            self._update_cooldown(rule_id, rule.cooldown_seconds)
                            self._increment_occurrence(rule_id)
                except Exception as e:
                    logger.error(f"Error evaluating rule {rule_id}: {e}")
            
            # Update processing time
            processing_time = (time.perf_counter() - start_time) * 1000
            self.processing_times.append(processing_time)
            if len(self.processing_times) > 1000:
                self.processing_times.pop(0)
            
            if processing_time > 100.0:  # Target response time
                logger.warning(f"Alert processing took {processing_time:.2f}ms, exceeding 100ms target")
            
            return generated_alerts
            
        except Exception as e:
            logger.error(f"Error processing alert data: {e}")
            return []

    async def _create_alert(self, rule: AlertRule, data: Dict[str, Any]) -> Optional[Alert]:
        """Create new alert from rule and data"""
        try:
            alert_id = str(uuid.uuid4())
            current_time = datetime.now(timezone.utc)
            
            # Generate alert title and message
            title = f"{rule.name}"
            message = f"{rule.description}"
            
            # Add data-specific information
            if "value" in data:
                message += f" (Value: {data['value']})"
            if "threshold" in data:
                message += f" (Threshold: {data['threshold']})"
            
            alert = Alert(
                alert_id=alert_id,
                rule_id=rule.rule_id,
                alert_type=rule.alert_type,
                severity=rule.severity,
                source=rule.source,
                title=title,
                message=message,
                status=AlertStatus.ACTIVE,
                created_at=current_time,
                updated_at=current_time,
                data=data,
                metadata={
                    "rule_name": rule.name,
                    "notification_channels": rule.notification_channels,
                    "cooldown_seconds": rule.cooldown_seconds
                }
            )
            
            # Store alert
            self.active_alerts[alert_id] = alert
            self.alert_history.append(alert)
            
            # Update statistics
            self._update_statistics(alert)
            
            # Trigger alert handlers
            await self._trigger_alert_handlers(alert)
            
            logger.info(f"Created alert: {alert.title} (ID: {alert_id})")
            return alert
            
        except Exception as e:
            logger.error(f"Error creating alert: {e}")
            return None

    def _is_in_cooldown(self, rule_id: str) -> bool:
        """Check if rule is in cooldown period"""
        if rule_id not in self.cooldown_tracker:
            return False
        
        cooldown_end = self.cooldown_tracker[rule_id]
        return datetime.now(timezone.utc) < cooldown_end

    def _exceeds_occurrence_limit(self, rule_id: str, rule: AlertRule) -> bool:
        """Check if rule exceeds occurrence limit"""
        current_count = self.alert_counters.get(rule_id, 0)
        return current_count >= rule.max_occurrences

    def _is_suppressed(self, rule_id: str, data: Dict[str, Any]) -> bool:
        """Check if rule is suppressed"""
        if rule_id not in self.suppression_rules:
            return False
        
        suppression_conditions = self.suppression_rules[rule_id]
        for condition in suppression_conditions:
            # Simple suppression logic - can be enhanced
            if condition in data and data[condition]:
                return True
        
        return False

    def _update_cooldown(self, rule_id: str, cooldown_seconds: int):
        """Update cooldown tracker"""
        if cooldown_seconds > 0:
            cooldown_end = datetime.now(timezone.utc) + timedelta(seconds=cooldown_seconds)
            self.cooldown_tracker[rule_id] = cooldown_end

    def _increment_occurrence(self, rule_id: str):
        """Increment occurrence counter"""
        self.alert_counters[rule_id] = self.alert_counters.get(rule_id, 0) + 1

    async def _trigger_alert_handlers(self, alert: Alert):
        """Trigger alert handlers"""
        handlers = self.alert_handlers.get(alert.alert_type, [])
        for handler in handlers:
            try:
                await handler(alert)
            except Exception as e:
                logger.error(f"Error in alert handler: {e}")

    def _update_statistics(self, alert: Alert):
        """Update alert statistics"""
        self.alert_statistics.total_alerts += 1
        
        # Update by severity
        severity_key = alert.severity.value
        self.alert_statistics.alerts_by_severity[severity_key] = \
            self.alert_statistics.alerts_by_severity.get(severity_key, 0) + 1
        
        # Update by type
        type_key = alert.alert_type.value
        self.alert_statistics.alerts_by_type[type_key] = \
            self.alert_statistics.alerts_by_type.get(type_key, 0) + 1
        
        # Update by source
        source_key = alert.source.value
        self.alert_statistics.alerts_by_source[source_key] = \
            self.alert_statistics.alerts_by_source.get(source_key, 0) + 1
        
        # Update active alerts count
        if alert.status == AlertStatus.ACTIVE:
            self.alert_statistics.active_alerts += 1
        elif alert.status == AlertStatus.ACKNOWLEDGED:
            self.alert_statistics.acknowledged_alerts += 1
        elif alert.status == AlertStatus.RESOLVED:
            self.alert_statistics.resolved_alerts += 1
        
        self.alert_statistics.last_updated = datetime.now(timezone.utc)

    async def acknowledge_alert(self, alert_id: str, acknowledged_by: str) -> bool:
        """Acknowledge an alert"""
        if alert_id not in self.active_alerts:
            return False
        
        alert = self.active_alerts[alert_id]
        alert.status = AlertStatus.ACKNOWLEDGED
        alert.acknowledged_at = datetime.now(timezone.utc)
        alert.acknowledged_by = acknowledged_by
        alert.updated_at = datetime.now(timezone.utc)
        
        # Update statistics
        self.alert_statistics.active_alerts -= 1
        self.alert_statistics.acknowledged_alerts += 1
        
        logger.info(f"Alert acknowledged: {alert_id} by {acknowledged_by}")
        return True

    async def resolve_alert(self, alert_id: str, resolved_by: str) -> bool:
        """Resolve an alert"""
        if alert_id not in self.active_alerts:
            return False
        
        alert = self.active_alerts[alert_id]
        alert.status = AlertStatus.RESOLVED
        alert.resolved_at = datetime.now(timezone.utc)
        alert.resolved_by = resolved_by
        alert.updated_at = datetime.now(timezone.utc)
        
        # Move from active to history
        del self.active_alerts[alert_id]
        
        # Update statistics
        if alert.status == AlertStatus.ACTIVE:
            self.alert_statistics.active_alerts -= 1
        elif alert.status == AlertStatus.ACKNOWLEDGED:
            self.alert_statistics.acknowledged_alerts -= 1
        self.alert_statistics.resolved_alerts += 1
        
        logger.info(f"Alert resolved: {alert_id} by {resolved_by}")
        return True

    def get_active_alerts(self, severity: Optional[AlertSeverity] = None) -> List[Alert]:
        """Get active alerts, optionally filtered by severity"""
        alerts = list(self.active_alerts.values())
        
        if severity:
            alerts = [alert for alert in alerts if alert.severity == severity]
        
        return sorted(alerts, key=lambda x: x.created_at, reverse=True)

    def get_alert_history(self, limit: int = 100, 
                         alert_type: Optional[AlertType] = None,
                         severity: Optional[AlertSeverity] = None) -> List[Alert]:
        """Get alert history with optional filtering"""
        alerts = self.alert_history.copy()
        
        if alert_type:
            alerts = [alert for alert in alerts if alert.alert_type == alert_type]
        
        if severity:
            alerts = [alert for alert in alerts if alert.severity == severity]
        
        return sorted(alerts, key=lambda x: x.created_at, reverse=True)[:limit]

    def get_alert_statistics(self) -> Dict[str, Any]:
        """Get alert statistics"""
        return {
            "total_alerts": self.alert_statistics.total_alerts,
            "active_alerts": self.alert_statistics.active_alerts,
            "acknowledged_alerts": self.alert_statistics.acknowledged_alerts,
            "resolved_alerts": self.alert_statistics.resolved_alerts,
            "alerts_by_severity": self.alert_statistics.alerts_by_severity,
            "alerts_by_type": self.alert_statistics.alerts_by_type,
            "alerts_by_source": self.alert_statistics.alerts_by_source,
            "avg_processing_time_ms": sum(self.processing_times) / len(self.processing_times) if self.processing_times else 0.0,
            "total_rules": len(self.alert_rules),
            "enabled_rules": len([r for r in self.alert_rules.values() if r.enabled]),
            "last_updated": self.alert_statistics.last_updated.isoformat()
        }

    def add_alert_handler(self, alert_type: AlertType, handler: Callable):
        """Add alert handler for specific alert type"""
        if alert_type not in self.alert_handlers:
            self.alert_handlers[alert_type] = []
        self.alert_handlers[alert_type].append(handler)
        logger.info(f"Added alert handler for {alert_type.value}")

    def add_suppression_rule(self, rule_id: str, suppression_conditions: List[str]):
        """Add suppression rule for alert rule"""
        self.suppression_rules[rule_id] = set(suppression_conditions)
        logger.info(f"Added suppression rule for {rule_id}")

    def clear_alert_history(self, older_than_days: int = 30):
        """Clear old alert history"""
        cutoff_date = datetime.now(timezone.utc) - timedelta(days=older_than_days)
        original_count = len(self.alert_history)
        
        self.alert_history = [
            alert for alert in self.alert_history 
            if alert.created_at > cutoff_date
        ]
        
        removed_count = original_count - len(self.alert_history)
        logger.info(f"Cleared {removed_count} old alerts from history")

    def reset_alert_counters(self):
        """Reset alert occurrence counters"""
        self.alert_counters.clear()
        self.cooldown_tracker.clear()
        logger.info("Reset alert counters and cooldown tracker")


# Global instance
alert_system = AlertSystem()
