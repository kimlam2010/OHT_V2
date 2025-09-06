"""
Notification Service for OHT-50
Advanced notification delivery and management system
Target: < 100ms notification delivery time for critical alerts
"""

import asyncio
import logging
import time
import smtplib
import json
from typing import Dict, Any, List, Optional, Callable, Set
from datetime import datetime, timezone, timedelta
from dataclasses import dataclass, field
from enum import Enum
from email.mime.text import MIMEText
from email.mime.multipart import MIMEMultipart
import uuid

logger = logging.getLogger(__name__)


class NotificationChannel(Enum):
    """Notification channels"""
    EMAIL = "email"
    SMS = "sms"
    DASHBOARD = "dashboard"
    WEBSOCKET = "websocket"
    AUDIO = "audio"
    PUSH = "push"
    WEBHOOK = "webhook"
    LOG = "log"


class NotificationPriority(Enum):
    """Notification priority levels"""
    LOW = "low"
    NORMAL = "normal"
    HIGH = "high"
    URGENT = "urgent"
    CRITICAL = "critical"


class NotificationStatus(Enum):
    """Notification status"""
    PENDING = "pending"
    SENT = "sent"
    DELIVERED = "delivered"
    FAILED = "failed"
    CANCELLED = "cancelled"


class NotificationType(Enum):
    """Notification types"""
    ALERT = "alert"
    STATUS_UPDATE = "status_update"
    SYSTEM_MESSAGE = "system_message"
    MAINTENANCE = "maintenance"
    CUSTOM = "custom"


@dataclass
class NotificationTemplate:
    """Notification template"""
    template_id: str
    name: str
    channel: NotificationChannel
    subject_template: str
    body_template: str
    enabled: bool = True
    priority: NotificationPriority = NotificationPriority.NORMAL
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class NotificationRecipient:
    """Notification recipient"""
    recipient_id: str
    name: str
    email: Optional[str] = None
    phone: Optional[str] = None
    websocket_id: Optional[str] = None
    preferences: Dict[str, Any] = field(default_factory=dict)
    enabled: bool = True


@dataclass
class Notification:
    """Notification instance"""
    notification_id: str
    channel: NotificationChannel
    recipient: NotificationRecipient
    subject: str
    body: str
    priority: NotificationPriority
    notification_type: NotificationType
    status: NotificationStatus
    created_at: datetime
    sent_at: Optional[datetime] = None
    delivered_at: Optional[datetime] = None
    failed_at: Optional[datetime] = None
    retry_count: int = 0
    max_retries: int = 3
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class NotificationConfig:
    """Notification service configuration"""
    smtp_server: str = "localhost"
    smtp_port: int = 587
    smtp_username: str = ""
    smtp_password: str = ""
    smtp_use_tls: bool = True
    sms_gateway_url: str = ""
    sms_api_key: str = ""
    websocket_enabled: bool = True
    audio_enabled: bool = True
    max_retries: int = 3
    retry_delay_seconds: int = 60
    batch_size: int = 100
    processing_interval: float = 1.0


class NotificationService:
    """
    Advanced notification delivery service
    Handles multiple notification channels with retry logic and delivery tracking
    """

    def __init__(self, config: Optional[NotificationConfig] = None):
        self.config = config or NotificationConfig()
        self.templates: Dict[str, NotificationTemplate] = {}
        self.recipients: Dict[str, NotificationRecipient] = {}
        self.notification_queue: List[Notification] = []
        self.notification_history: List[Notification] = []
        self.active_websockets: Set[str] = set()
        
        # Performance tracking
        self.delivery_times: List[float] = []
        self.success_rates: Dict[NotificationChannel, List[bool]] = {}
        self.processing_times: List[float] = []
        
        # Statistics
        self.stats = {
            "total_sent": 0,
            "total_delivered": 0,
            "total_failed": 0,
            "by_channel": {channel.value: 0 for channel in NotificationChannel},
            "by_priority": {priority.value: 0 for priority in NotificationPriority},
            "by_type": {ntype.value: 0 for ntype in NotificationType}
        }
        
        # Initialize default templates and recipients
        self._initialize_defaults()
        
        # Start background processing (lazy initialization)
        self._processing_task: Optional[asyncio.Task] = None
        self._background_processing_started = False
        
        logger.info("NotificationService initialized with %d templates and %d recipients", 
                   len(self.templates), len(self.recipients))

    def _initialize_defaults(self):
        """Initialize default templates and recipients"""
        # Default templates
        self.add_template(NotificationTemplate(
            template_id="alert_email",
            name="Alert Email Template",
            channel=NotificationChannel.EMAIL,
            subject_template="OHT-50 Alert: {alert_title}",
            body_template="Alert: {alert_title}\n\nMessage: {alert_message}\n\nSeverity: {alert_severity}\nTime: {timestamp}",
            priority=NotificationPriority.HIGH
        ))
        
        self.add_template(NotificationTemplate(
            template_id="alert_dashboard",
            name="Alert Dashboard Template",
            channel=NotificationChannel.DASHBOARD,
            subject_template="{alert_title}",
            body_template="{alert_message}",
            priority=NotificationPriority.HIGH
        ))
        
        self.add_template(NotificationTemplate(
            template_id="system_status",
            name="System Status Template",
            channel=NotificationChannel.EMAIL,
            subject_template="OHT-50 System Status Update",
            body_template="System Status: {status}\n\nDetails: {details}\nTime: {timestamp}",
            priority=NotificationPriority.NORMAL
        ))
        
        # Default recipients
        self.add_recipient(NotificationRecipient(
            recipient_id="admin",
            name="System Administrator",
            email="admin@oht50.local",
            preferences={"channels": ["email", "dashboard", "log"], "priority_threshold": "warning"}
        ))
        
        self.add_recipient(NotificationRecipient(
            recipient_id="operator",
            name="System Operator",
            email="operator@oht50.local",
            preferences={"channels": ["email", "dashboard", "websocket", "log"], "priority_threshold": "normal"}
        ))

    def _start_background_processing(self):
        """Start background notification processing"""
        if not self._background_processing_started:
            try:
                if self._processing_task is None or self._processing_task.done():
                    self._processing_task = asyncio.create_task(self._process_notification_queue())
                    self._background_processing_started = True
            except RuntimeError:
                # No event loop running, will start later
                pass

    async def _process_notification_queue(self):
        """Background task to process notification queue"""
        while True:
            try:
                if self.notification_queue:
                    await self._process_batch()
                await asyncio.sleep(self.config.processing_interval)
            except Exception as e:
                logger.error(f"Error in notification processing: {e}")
                await asyncio.sleep(5.0)

    async def _process_batch(self):
        """Process a batch of notifications"""
        batch_size = min(self.config.batch_size, len(self.notification_queue))
        batch = self.notification_queue[:batch_size]
        self.notification_queue = self.notification_queue[batch_size:]
        
        for notification in batch:
            try:
                await self._send_notification(notification)
            except Exception as e:
                logger.error(f"Error sending notification {notification.notification_id}: {e}")
                await self._handle_notification_failure(notification, str(e))

    async def send_notification(self, channel: NotificationChannel, recipient_id: str,
                              subject: str, body: str, priority: NotificationPriority = NotificationPriority.NORMAL,
                              notification_type: NotificationType = NotificationType.SYSTEM_MESSAGE,
                              metadata: Optional[Dict[str, Any]] = None) -> str:
        """
        Send notification through specified channel
        """
        # Start background processing if not already started
        self._start_background_processing()
        
        start_time = time.perf_counter()
        
        try:
            # Get recipient
            recipient = self.recipients.get(recipient_id)
            if not recipient or not recipient.enabled:
                raise ValueError(f"Recipient {recipient_id} not found or disabled")
            
            # Check recipient preferences
            if not self._check_recipient_preferences(recipient, channel, priority):
                logger.info(f"Notification skipped due to recipient preferences: {recipient_id}")
                return ""
            
            # Create notification
            notification = Notification(
                notification_id=str(uuid.uuid4()),
                channel=channel,
                recipient=recipient,
                subject=subject,
                body=body,
                priority=priority,
                notification_type=notification_type,
                status=NotificationStatus.PENDING,
                created_at=datetime.now(timezone.utc),
                metadata=metadata or {}
            )
            
            # Add to queue
            self.notification_queue.append(notification)
            
            # Update statistics
            self.stats["total_sent"] += 1
            if hasattr(channel, 'value'):
                channel_key = channel.value
            else:
                channel_key = str(channel)
            
            # Initialize channel key if not exists
            if channel_key not in self.stats["by_channel"]:
                self.stats["by_channel"][channel_key] = 0
            self.stats["by_channel"][channel_key] += 1
            
            self.stats["by_priority"][priority.value] += 1
            self.stats["by_type"][notification_type.value] += 1
            
            processing_time = (time.perf_counter() - start_time) * 1000
            self.processing_times.append(processing_time)
            if len(self.processing_times) > 1000:
                self.processing_times.pop(0)
            
            if processing_time > 100.0:  # Target delivery time
                logger.warning(f"Notification queuing took {processing_time:.2f}ms, exceeding 100ms target")
            
            logger.info(f"Queued notification: {subject} to {recipient_id} via {channel.value}")
            return notification.notification_id
            
        except Exception as e:
            logger.error(f"Error queuing notification: {e}")
            raise

    async def _send_notification(self, notification: Notification):
        """Send individual notification"""
        start_time = time.perf_counter()
        
        try:
            # Send based on channel
            if notification.channel == NotificationChannel.EMAIL:
                await self._send_email(notification)
            elif notification.channel == NotificationChannel.SMS:
                await self._send_sms(notification)
            elif notification.channel == NotificationChannel.DASHBOARD:
                await self._send_dashboard(notification)
            elif notification.channel == NotificationChannel.WEBSOCKET:
                await self._send_websocket(notification)
            elif notification.channel == NotificationChannel.AUDIO:
                await self._send_audio(notification)
            elif notification.channel == NotificationChannel.WEBHOOK:
                await self._send_webhook(notification)
            elif notification.channel == NotificationChannel.LOG:
                await self._send_log(notification)
            else:
                raise ValueError(f"Unsupported notification channel: {notification.channel}")
            
            # Update notification status
            notification.status = NotificationStatus.SENT
            notification.sent_at = datetime.now(timezone.utc)
            
            # Track delivery time
            delivery_time = (time.perf_counter() - start_time) * 1000
            self.delivery_times.append(delivery_time)
            if len(self.delivery_times) > 1000:
                self.delivery_times.pop(0)
            
            # Track success rate
            if notification.channel not in self.success_rates:
                self.success_rates[notification.channel] = []
            self.success_rates[notification.channel].append(True)
            if len(self.success_rates[notification.channel]) > 100:
                self.success_rates[notification.channel].pop(0)
            
            # Add to history
            self.notification_history.append(notification)
            
            logger.info(f"Sent notification: {notification.notification_id} via {notification.channel.value}")
            
        except Exception as e:
            await self._handle_notification_failure(notification, str(e))
            raise

    async def _send_email(self, notification: Notification):
        """Send email notification"""
        if not notification.recipient.email:
            raise ValueError("Recipient email not configured")
        
        try:
            # Create message
            msg = MIMEMultipart()
            msg['From'] = self.config.smtp_username
            msg['To'] = notification.recipient.email
            msg['Subject'] = notification.subject
            
            msg.attach(MIMEText(notification.body, 'plain'))
            
            # Send email
            if self.config.smtp_use_tls:
                server = smtplib.SMTP(self.config.smtp_server, self.config.smtp_port)
                server.starttls()
                if self.config.smtp_username and self.config.smtp_password:
                    server.login(self.config.smtp_username, self.config.smtp_password)
                server.send_message(msg)
                server.quit()
            else:
                # For testing/development
                logger.info(f"Email notification (simulated): {notification.subject}")
            
        except Exception as e:
            logger.error(f"Failed to send email: {e}")
            raise

    async def _send_sms(self, notification: Notification):
        """Send SMS notification"""
        if not notification.recipient.phone:
            raise ValueError("Recipient phone not configured")
        
        # Simulate SMS sending (in real implementation, would use SMS gateway)
        logger.info(f"SMS notification (simulated): {notification.subject} to {notification.recipient.phone}")

    async def _send_dashboard(self, notification: Notification):
        """Send dashboard notification"""
        # In real implementation, would update dashboard UI
        logger.info(f"Dashboard notification: {notification.subject}")

    async def _send_websocket(self, notification: Notification):
        """Send websocket notification"""
        if notification.recipient.websocket_id and notification.recipient.websocket_id in self.active_websockets:
            # In real implementation, would send via websocket
            logger.info(f"WebSocket notification: {notification.subject} to {notification.recipient.websocket_id}")
        else:
            logger.warning(f"WebSocket recipient not connected: {notification.recipient.websocket_id}")

    async def _send_audio(self, notification: Notification):
        """Send audio notification"""
        if self.config.audio_enabled:
            # In real implementation, would play audio alert
            logger.info(f"Audio notification: {notification.subject}")

    async def _send_webhook(self, notification: Notification):
        """Send webhook notification"""
        # In real implementation, would send HTTP POST to webhook URL
        logger.info(f"Webhook notification: {notification.subject}")

    async def _send_log(self, notification: Notification):
        """Send log notification"""
        logger.info(f"Log notification: {notification.subject} - {notification.body}")

    async def _handle_notification_failure(self, notification: Notification, error: str):
        """Handle notification failure"""
        notification.retry_count += 1
        notification.failed_at = datetime.now(timezone.utc)
        
        if notification.retry_count < notification.max_retries:
            # Retry after delay
            await asyncio.sleep(self.config.retry_delay_seconds)
            self.notification_queue.append(notification)
            logger.info(f"Retrying notification {notification.notification_id} (attempt {notification.retry_count})")
        else:
            # Mark as failed
            notification.status = NotificationStatus.FAILED
            self.stats["total_failed"] += 1
            
            # Track failure
            if notification.channel not in self.success_rates:
                self.success_rates[notification.channel] = []
            self.success_rates[notification.channel].append(False)
            
            logger.error(f"Notification failed after {notification.max_retries} retries: {error}")

    def _check_recipient_preferences(self, recipient: NotificationRecipient, 
                                   channel: NotificationChannel, priority: NotificationPriority) -> bool:
        """Check if notification matches recipient preferences"""
        preferences = recipient.preferences
        
        # Check channel preferences
        allowed_channels = preferences.get("channels", [])
        if allowed_channels and hasattr(channel, 'value') and channel.value not in allowed_channels:
            return False
        
        # Check priority threshold
        priority_threshold = preferences.get("priority_threshold", "low")
        priority_levels = ["low", "normal", "high", "urgent", "critical"]
        
        # Handle case where threshold might be "warning" instead of "normal"
        if priority_threshold == "warning":
            priority_threshold = "normal"
        
        try:
            threshold_index = priority_levels.index(priority_threshold)
            priority_index = priority_levels.index(priority.value)
        except ValueError:
            # Default to allowing all if priority not found
            return True
        
        return priority_index >= threshold_index

    def add_template(self, template: NotificationTemplate):
        """Add notification template"""
        self.templates[template.template_id] = template
        logger.info(f"Added notification template: {template.name}")

    def add_recipient(self, recipient: NotificationRecipient):
        """Add notification recipient"""
        self.recipients[recipient.recipient_id] = recipient
        logger.info(f"Added notification recipient: {recipient.name}")

    def register_websocket(self, websocket_id: str):
        """Register active websocket connection"""
        self.active_websockets.add(websocket_id)
        logger.info(f"Registered websocket: {websocket_id}")

    def unregister_websocket(self, websocket_id: str):
        """Unregister websocket connection"""
        self.active_websockets.discard(websocket_id)
        logger.info(f"Unregistered websocket: {websocket_id}")

    def get_notification_statistics(self) -> Dict[str, Any]:
        """Get notification statistics"""
        return {
            "total_sent": self.stats["total_sent"],
            "total_delivered": self.stats["total_delivered"],
            "total_failed": self.stats["total_failed"],
            "by_channel": self.stats["by_channel"],
            "by_priority": self.stats["by_priority"],
            "by_type": self.stats["by_type"],
            "queue_size": len(self.notification_queue),
            "active_websockets": len(self.active_websockets),
            "avg_delivery_time_ms": sum(self.delivery_times) / len(self.delivery_times) if self.delivery_times else 0.0,
            "success_rates": {
                channel.value: sum(successes) / len(successes) if successes else 0.0
                for channel, successes in self.success_rates.items()
            },
            "templates_count": len(self.templates),
            "recipients_count": len(self.recipients)
        }

    def get_notification_history(self, limit: int = 100, 
                               channel: Optional[NotificationChannel] = None,
                               status: Optional[NotificationStatus] = None) -> List[Notification]:
        """Get notification history with optional filtering"""
        notifications = self.notification_history.copy()
        
        if channel:
            notifications = [n for n in notifications if n.channel == channel]
        
        if status:
            notifications = [n for n in notifications if n.status == status]
        
        return sorted(notifications, key=lambda x: x.created_at, reverse=True)[:limit]

    async def send_alert_notification(self, alert_data: Dict[str, Any], 
                                    recipients: Optional[List[str]] = None) -> List[str]:
        """Send alert notification to specified recipients"""
        if not recipients:
            recipients = ["admin", "operator"]
        
        notification_ids = []
        
        for recipient_id in recipients:
            try:
                # Use alert email template
                template = self.templates.get("alert_email")
                if template:
                    subject = template.subject_template.format(**alert_data)
                    body = template.body_template.format(**alert_data)
                else:
                    subject = f"Alert: {alert_data.get('title', 'Unknown Alert')}"
                    body = f"Alert: {alert_data.get('message', 'No message')}"
                
                notification_id = await self.send_notification(
                    channel=NotificationChannel.EMAIL,
                    recipient_id=recipient_id,
                    subject=subject,
                    body=body,
                    priority=NotificationPriority.HIGH,
                    notification_type=NotificationType.ALERT,
                    metadata={"alert_data": alert_data}
                )
                
                if notification_id:
                    notification_ids.append(notification_id)
                    
            except Exception as e:
                logger.error(f"Failed to send alert notification to {recipient_id}: {e}")
        
        return notification_ids

    def clear_notification_history(self, older_than_days: int = 30):
        """Clear old notification history"""
        cutoff_date = datetime.now(timezone.utc) - timedelta(days=older_than_days)
        original_count = len(self.notification_history)
        
        self.notification_history = [
            notification for notification in self.notification_history 
            if notification.created_at > cutoff_date
        ]
        
        removed_count = original_count - len(self.notification_history)
        logger.info(f"Cleared {removed_count} old notifications from history")

    async def shutdown(self):
        """Shutdown notification service"""
        if self._processing_task and not self._processing_task.done():
            self._processing_task.cancel()
            try:
                await self._processing_task
            except asyncio.CancelledError:
                pass
        
        logger.info("NotificationService shutdown complete")


# Global instance
notification_service = NotificationService()
