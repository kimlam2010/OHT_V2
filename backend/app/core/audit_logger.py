"""
Audit Logger for Production Environment

This module provides comprehensive audit logging for security events.
It includes event tracking, security monitoring, and audit trail management.
"""

import logging
from typing import Dict, Any, Optional, List
from datetime import datetime, timedelta
from sqlalchemy.ext.asyncio import AsyncSession
from sqlalchemy import select, insert, delete
from fastapi import Request

from app.models.user import AuditLog

logger = logging.getLogger(__name__)


class AuditLogger:
    """
    Production-grade audit logging system
    
    Features:
    - Security event tracking
    - User action logging
    - System event monitoring
    - Audit trail management
    - Compliance reporting
    """
    
    def __init__(self, db: AsyncSession):
        self.db = db
        
    async def log_security_event(
        self, 
        event_type: str, 
        user_id: int, 
        resource: str, 
        action: str, 
        details: Optional[Dict[str, Any]] = None,
        ip_address: Optional[str] = None,
        user_agent: Optional[str] = None
    ) -> bool:
        """
        Log security event
        
        Args:
            event_type: Type of security event
            user_id: ID of user performing action
            resource: Resource being accessed
            action: Action performed
            details: Additional event details
            ip_address: User's IP address
            user_agent: User's browser/agent
            
        Returns:
            True if logged successfully, False otherwise
        """
        try:
            # Store event_type in details since it's not a field in AuditLog model
            audit_details = details or {}
            if event_type:
                audit_details["event_type"] = event_type
            if user_agent:
                audit_details["user_agent"] = user_agent
            
            audit_log = AuditLog(
                user_id=user_id,
                resource=resource,
                action=action,
                details=audit_details,
                ip_address=ip_address,
                success=True,  # Default to success for security events
                timestamp=datetime.utcnow()
            )
            
            self.db.add(audit_log)
            await self.db.commit()
            
            logger.info(f"🔍 Security event logged: {event_type} - {user_id} - {resource}:{action}")
            return True
            
        except Exception as e:
            logger.error(f"❌ Security event logging failed: {e}")
            await self.db.rollback()
            return False
            
    async def log_user_login(self, user_id: int, success: bool, ip_address: str, user_agent: str, details: Optional[Dict[str, Any]] = None) -> bool:
        """Log user login attempt"""
        event_type = "login_success" if success else "login_failure"
        
        login_details = {
            "success": success,
            "timestamp": datetime.utcnow().isoformat()
        }
        
        if details:
            login_details.update(details)
            
        return await self.log_security_event(
            event_type=event_type,
            user_id=user_id,
            resource="authentication",
            action="login",
            details=login_details,
            ip_address=ip_address,
            user_agent=user_agent
        )
        
    async def log_user_logout(self, user_id: int, ip_address: str, user_agent: str) -> bool:
        """Log user logout"""
        return await self.log_security_event(
            event_type="logout",
            user_id=user_id,
            resource="authentication",
            action="logout",
            ip_address=ip_address,
            user_agent=user_agent
        )
        
    async def log_permission_denied(self, user_id: int, resource: str, action: str, ip_address: str, user_agent: str) -> bool:
        """Log permission denied access attempt"""
        return await self.log_security_event(
            event_type="permission_denied",
            user_id=user_id,
            resource=resource,
            action=action,
            details={"reason": "Insufficient permissions"},
            ip_address=ip_address,
            user_agent=user_agent
        )
        
    async def log_resource_access(self, user_id: int, resource: str, action: str, success: bool, ip_address: str, user_agent: str, details: Optional[Dict[str, Any]] = None) -> bool:
        """Log resource access attempt"""
        event_type = f"resource_access_{'success' if success else 'failure'}"
        
        access_details = {
            "success": success,
            "timestamp": datetime.utcnow().isoformat()
        }
        
        if details:
            access_details.update(details)
            
        return await self.log_security_event(
            event_type=event_type,
            user_id=user_id,
            resource=resource,
            action=action,
            details=access_details,
            ip_address=ip_address,
            user_agent=user_agent
        )
        
    async def log_system_event(self, event_type: str, details: Dict[str, Any], user_id: Optional[int] = None) -> bool:
        """Log system-level events"""
        try:
            # Store event_type in details since it's not a field in AuditLog model
            system_details = details or {}
            system_details["event_type"] = event_type
            
            audit_log = AuditLog(
                user_id=user_id or 0,  # Use 0 for system events without user
                resource="system",
                action="system_event",
                details=system_details,
                success=True,  # Default to success for system events
                timestamp=datetime.utcnow()
            )
            
            self.db.add(audit_log)
            await self.db.commit()
            
            logger.info(f"🔍 System event logged: {event_type}")
            return True
            
        except Exception as e:
            logger.error(f"❌ System event logging failed: {e}")
            await self.db.rollback()
            return False
            
    async def log_configuration_change(self, user_id: int, resource: str, action: str, old_value: Any, new_value: Any, ip_address: str, user_agent: str) -> bool:
        """Log configuration changes"""
        details = {
            "change_type": "configuration",
            "old_value": str(old_value),
            "new_value": str(new_value),
            "timestamp": datetime.utcnow().isoformat()
        }
        
        return await self.log_security_event(
            event_type="configuration_change",
            user_id=user_id,
            resource=resource,
            action=action,
            details=details,
            ip_address=ip_address,
            user_agent=user_agent
        )
        
    async def get_audit_logs(
        self, 
        user_id: Optional[int] = None,
        event_type: Optional[str] = None,
        resource: Optional[str] = None,
        start_time: Optional[datetime] = None,
        end_time: Optional[datetime] = None,
        limit: int = 100,
        offset: int = 0
    ) -> List[AuditLog]:
        """Get audit logs with filtering"""
        try:
            query = select(AuditLog)
            
            if user_id:
                query = query.where(AuditLog.user_id == user_id)
                
            if event_type:
                query = query.where(AuditLog.event_type == event_type)
                
            if resource:
                query = query.where(AuditLog.resource == resource)
                
            if start_time:
                query = query.where(AuditLog.timestamp >= start_time)
                
            if end_time:
                query = query.where(AuditLog.timestamp <= end_time)
                
            query = query.order_by(AuditLog.timestamp.desc()).offset(offset).limit(limit)
            
            result = await self.db.execute(query)
            return result.scalars().all()
            
        except Exception as e:
            logger.error(f"❌ Get audit logs failed: {e}")
            return []
            
    async def get_user_audit_summary(self, user_id: int, days: int = 30) -> Dict[str, Any]:
        """Get audit summary for user"""
        try:
            start_time = datetime.utcnow() - timedelta(days=days)
            
            # Get all logs for user in time period
            logs = await self.get_audit_logs(
                user_id=user_id,
                start_time=start_time
            )
            
            # Analyze logs
            event_counts = {}
            resource_counts = {}
            action_counts = {}
            
            for log in logs:
                # Event type counts
                event_counts[log.event_type] = event_counts.get(log.event_type, 0) + 1
                
                # Resource counts
                resource_counts[log.resource] = resource_counts.get(log.resource, 0) + 1
                
                # Action counts
                action_counts[log.action] = action_counts.get(log.action, 0) + 1
                
            return {
                "user_id": user_id,
                "period_days": days,
                "total_events": len(logs),
                "event_type_counts": event_counts,
                "resource_counts": resource_counts,
                "action_counts": action_counts,
                "first_event": logs[-1].timestamp.isoformat() if logs else None,
                "last_event": logs[0].timestamp.isoformat() if logs else None
            }
            
        except Exception as e:
            logger.error(f"❌ Get user audit summary failed: {e}")
            return {"error": str(e)}
            
    async def get_security_alerts(self, hours: int = 24) -> List[Dict[str, Any]]:
        """Get security alerts from recent audit logs"""
        try:
            start_time = datetime.utcnow() - timedelta(hours=hours)
            
            # Get security-related events
            security_events = [
                "login_failure",
                "permission_denied",
                "resource_access_failure",
                "configuration_change",
                "system_event"
            ]
            
            alerts = []
            
            for event_type in security_events:
                logs = await self.get_audit_logs(
                    event_type=event_type,
                    start_time=start_time,
                    limit=10
                )
                
                if logs:
                    alerts.append({
                        "event_type": event_type,
                        "count": len(logs),
                        "recent_events": [
                            {
                                "timestamp": log.timestamp.isoformat(),
                                "user_id": log.user_id,
                                "resource": log.resource,
                                "action": log.action,
                                "ip_address": log.ip_address
                            }
                            for log in logs[:5]  # Last 5 events
                        ]
                    })
                    
            return alerts
            
        except Exception as e:
            logger.error(f"❌ Get security alerts failed: {e}")
            return []
            
    async def cleanup_old_logs(self, days_to_keep: int = 90) -> int:
        """Clean up old audit logs"""
        try:
            cutoff_date = datetime.utcnow() - timedelta(days=days_to_keep)
            
            # Count logs to be deleted
            count_query = select(AuditLog).where(AuditLog.timestamp < cutoff_date)
            count_result = await self.db.execute(count_query)
            logs_to_delete = len(count_result.scalars().all())
            
            if logs_to_delete > 0:
                # Delete old logs
                delete_query = delete(AuditLog).where(AuditLog.timestamp < cutoff_date)
                await self.db.execute(delete_query)
                await self.db.commit()
                
                logger.info(f"🧹 Cleaned up {logs_to_delete} old audit logs")
                return logs_to_delete
            else:
                logger.info("🧹 No old audit logs to clean up")
                return 0
                
        except Exception as e:
            logger.error(f"❌ Cleanup old logs failed: {e}")
            await self.db.rollback()
            return 0
            
    async def export_audit_logs(
        self, 
        start_time: datetime, 
        end_time: datetime, 
        format: str = "json"
    ) -> Optional[str]:
        """Export audit logs for compliance reporting"""
        try:
            logs = await self.get_audit_logs(
                start_time=start_time,
                end_time=end_time,
                limit=10000  # Large limit for export
            )
            
            if format.lower() == "json":
                import json
                export_data = []
                
                for log in logs:
                    export_data.append({
                        "id": log.id,
                        "event_type": log.event_type,
                        "user_id": log.user_id,
                        "resource": log.resource,
                        "action": log.action,
                        "details": log.details,
                        "ip_address": log.ip_address,
                        "user_agent": log.user_agent,
                        "timestamp": log.timestamp.isoformat()
                    })
                    
                return json.dumps(export_data, indent=2)
                
            elif format.lower() == "csv":
                import csv
                import io
                
                output = io.StringIO()
                writer = csv.writer(output)
                
                # Write header
                writer.writerow([
                    "ID", "Event Type", "User ID", "Resource", "Action", 
                    "Details", "IP Address", "User Agent", "Timestamp"
                ])
                
                # Write data
                for log in logs:
                    writer.writerow([
                        log.id, log.event_type, log.user_id, log.resource, log.action,
                        str(log.details), log.ip_address, log.user_agent, log.timestamp.isoformat()
                    ])
                    
                return output.getvalue()
                
            else:
                logger.error(f"❌ Unsupported export format: {format}")
                return None
                
        except Exception as e:
            logger.error(f"❌ Export audit logs failed: {e}")
            return None


# Convenience functions
async def log_security_event(
    db: AsyncSession,
    event_type: str,
    user_id: int,
    resource: str,
    action: str,
    details: Optional[Dict[str, Any]] = None,
    ip_address: Optional[str] = None,
    user_agent: Optional[str] = None
) -> bool:
    """Convenience function for logging security events"""
    audit_logger = AuditLogger(db)
    return await audit_logger.log_security_event(
        event_type, user_id, resource, action, details, ip_address, user_agent
    )


async def log_user_login(
    db: AsyncSession,
    user_id: int,
    success: bool,
    ip_address: str,
    user_agent: str,
    details: Optional[Dict[str, Any]] = None
) -> bool:
    """Convenience function for logging user login"""
    audit_logger = AuditLogger(db)
    return await audit_logger.log_user_login(user_id, success, ip_address, user_agent, details)
