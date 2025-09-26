# -*- coding: utf-8 -*-
"""
Database Service for Module Telemetry Validation
For Issue #144: Add Value Range Validation for Module Telemetry Data
"""

import json
import logging
from typing import Dict, Any, List, Optional
from datetime import datetime
import sqlite3
from pathlib import Path

from app.models.module_telemetry import ModuleTelemetry

logger = logging.getLogger(__name__)


class DatabaseService:
    """Database service for storing and retrieving validated telemetry data"""
    
    def __init__(self, db_path: str = "oht50.db"):
        """Initialize database service"""
        self.db_path = db_path
        self._initialize_database()
        logger.info(f"✅ DatabaseService initialized with database: {db_path}")
    
    def _initialize_database(self):
        """Initialize database tables for telemetry validation"""
        try:
            conn = sqlite3.connect(self.db_path)
            cursor = conn.cursor()
            
            # Create module_telemetry table if not exists
            cursor.execute("""
                CREATE TABLE IF NOT EXISTS module_telemetry (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    module_id INTEGER NOT NULL,
                    module_name TEXT NOT NULL,
                    telemetry_data TEXT NOT NULL,
                    validation_status TEXT NOT NULL,
                    validation_errors TEXT,
                    timestamp INTEGER NOT NULL,
                    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
                )
            """)
            
            # Create indexes separately
            cursor.execute("CREATE INDEX IF NOT EXISTS idx_module_id ON module_telemetry(module_id)")
            cursor.execute("CREATE INDEX IF NOT EXISTS idx_validation_status ON module_telemetry(validation_status)")
            cursor.execute("CREATE INDEX IF NOT EXISTS idx_timestamp ON module_telemetry(timestamp)")
            
            # Create telemetry_field_configs table if not exists
            cursor.execute("""
                CREATE TABLE IF NOT EXISTS telemetry_field_configs (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    field_name TEXT UNIQUE NOT NULL,
                    min_value REAL NOT NULL,
                    max_value REAL NOT NULL,
                    unit TEXT NOT NULL,
                    description TEXT NOT NULL,
                    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
                )
            """)
            
            # Create validation_status_summary table if not exists
            cursor.execute("""
                CREATE TABLE IF NOT EXISTS validation_status_summary (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    total_modules INTEGER NOT NULL,
                    valid_modules INTEGER NOT NULL,
                    invalid_modules INTEGER NOT NULL,
                    last_updated DATETIME DEFAULT CURRENT_TIMESTAMP
                )
            """)
            
            conn.commit()
            conn.close()
            
            logger.info("✅ Database tables initialized successfully")
            
        except Exception as e:
            logger.error(f"❌ Failed to initialize database: {e}")
            raise
    
    async def update_module_telemetry(self, module_id: int, telemetry_data: ModuleTelemetry) -> bool:
        """
        Update module telemetry data in database
        
        Args:
            module_id: Module ID
            telemetry_data: ModuleTelemetry object
            
        Returns:
            True if update successful, False otherwise
        """
        try:
            conn = sqlite3.connect(self.db_path)
            cursor = conn.cursor()
            
            # Prepare data for storage
            # Support both Pydantic v1 and v2
            try:
                telemetry_dict = telemetry_data.model_dump()
            except Exception:
                telemetry_dict = telemetry_data.dict()
            telemetry_json = json.dumps(telemetry_dict)
            validation_errors = None
            
            if telemetry_data.validation_status == "invalid":
                # Extract validation errors
                errors = []
                for field_name, field_data in telemetry_data.telemetry.items():
                    if not field_data.valid:
                        errors.append({
                            "field": field_name,
                            "value": field_data.value,
                            "range": {"min": field_data.min_value, "max": field_data.max_value},
                            "error": f"Value {field_data.value} is out of range [{field_data.min_value}, {field_data.max_value}]"
                        })
                validation_errors = json.dumps(errors)
            
            # Insert or update telemetry data
            cursor.execute("""
                INSERT OR REPLACE INTO module_telemetry 
                (module_id, module_name, telemetry_data, validation_status, validation_errors, timestamp)
                VALUES (?, ?, ?, ?, ?, ?)
            """, (
                module_id,
                telemetry_data.module_name,
                telemetry_json,
                telemetry_data.validation_status,
                validation_errors,
                telemetry_data.timestamp
            ))
            
            conn.commit()
            conn.close()
            
            logger.info(f"✅ Updated telemetry data for module {module_id}")
            return True
            
        except Exception as e:
            logger.error(f"❌ Failed to update telemetry data: {e}")
            return False
    
    async def get_module_telemetry(self, module_id: int) -> Optional[Dict[str, Any]]:
        """
        Get latest telemetry data for a module
        
        Args:
            module_id: Module ID
            
        Returns:
            Telemetry data dict or None if not found
        """
        try:
            conn = sqlite3.connect(self.db_path)
            cursor = conn.cursor()
            
            cursor.execute("""
                SELECT telemetry_data, validation_status, timestamp, created_at
                FROM module_telemetry 
                WHERE module_id = ? 
                ORDER BY timestamp DESC 
                LIMIT 1
            """, (module_id,))
            
            result = cursor.fetchone()
            conn.close()
            
            if result:
                telemetry_data = json.loads(result[0])
                return {
                    "telemetry_data": telemetry_data,
                    "validation_status": result[1],
                    "timestamp": result[2],
                    "created_at": result[3]
                }
            
            return None
            
        except Exception as e:
            logger.error(f"❌ Failed to get telemetry data: {e}")
            return None
    
    async def get_validation_status(self) -> Dict[str, Any]:
        """
        Get overall telemetry validation status
        
        Returns:
            Dictionary with validation status summary
        """
        try:
            conn = sqlite3.connect(self.db_path)
            cursor = conn.cursor()
            
            # Get total modules count
            cursor.execute("SELECT COUNT(DISTINCT module_id) FROM module_telemetry")
            total_modules = cursor.fetchone()[0] or 0
            
            # Get valid modules count
            cursor.execute("""
                SELECT COUNT(DISTINCT module_id) 
                FROM module_telemetry 
                WHERE validation_status = 'valid'
            """)
            valid_modules = cursor.fetchone()[0] or 0
            
            # Get invalid modules count
            cursor.execute("""
                SELECT COUNT(DISTINCT module_id) 
                FROM module_telemetry 
                WHERE validation_status = 'invalid'
            """)
            invalid_modules = cursor.fetchone()[0] or 0
            
            # Get recent validation errors
            cursor.execute("""
                SELECT module_id, module_name, validation_errors, timestamp
                FROM module_telemetry 
                WHERE validation_status = 'invalid' 
                AND timestamp > ? 
                ORDER BY timestamp DESC 
                LIMIT 10
            """, (int(datetime.utcnow().timestamp()) - 3600,))  # Last hour
            
            recent_errors = []
            for row in cursor.fetchall():
                if row[2]:  # validation_errors is not None
                    try:
                        errors = json.loads(row[2])
                        recent_errors.append({
                            "module_id": row[0],
                            "module_name": row[1],
                            "errors": errors,
                            "timestamp": row[3]
                        })
                    except json.JSONDecodeError:
                        continue
            
            conn.close()
            
            return {
                "total_modules": total_modules,
                "valid_modules": valid_modules,
                "invalid_modules": invalid_modules,
                "validation_rate": (valid_modules / total_modules * 100) if total_modules > 0 else 100,
                "recent_errors": recent_errors,
                "last_updated": datetime.utcnow().isoformat()
            }
            
        except Exception as e:
            logger.error(f"❌ Failed to get validation status: {e}")
            return {
                "total_modules": 0,
                "valid_modules": 0,
                "invalid_modules": 0,
                "validation_rate": 0,
                "recent_errors": [],
                "last_updated": datetime.utcnow().isoformat()
            }
    
    async def get_module_validation_history(self, module_id: int, limit: int = 100) -> List[Dict[str, Any]]:
        """
        Get validation history for a specific module
        
        Args:
            module_id: Module ID
            limit: Maximum number of records to return
            
        Returns:
            List of validation history records
        """
        try:
            conn = sqlite3.connect(self.db_path)
            cursor = conn.cursor()
            
            cursor.execute("""
                SELECT telemetry_data, validation_status, validation_errors, timestamp, created_at
                FROM module_telemetry 
                WHERE module_id = ? 
                ORDER BY timestamp DESC 
                LIMIT ?
            """, (module_id, limit))
            
            history = []
            for row in cursor.fetchall():
                try:
                    telemetry_data = json.loads(row[0])
                    validation_errors = json.loads(row[2]) if row[2] else []
                    
                    history.append({
                        "telemetry_data": telemetry_data,
                        "validation_status": row[1],
                        "validation_errors": validation_errors,
                        "timestamp": row[3],
                        "created_at": row[4]
                    })
                except json.JSONDecodeError:
                    continue
            
            conn.close()
            return history
            
        except Exception as e:
            logger.error(f"❌ Failed to get validation history: {e}")
            return []
    
    async def cleanup_old_telemetry_data(self, days_to_keep: int = 30) -> int:
        """
        Clean up old telemetry data
        
        Args:
            days_to_keep: Number of days to keep data
            
        Returns:
            Number of records deleted
        """
        try:
            conn = sqlite3.connect(self.db_path)
            cursor = conn.cursor()
            
            # Calculate cutoff timestamp
            cutoff_timestamp = int(datetime.utcnow().timestamp()) - (days_to_keep * 24 * 3600)
            
            # Delete old records
            cursor.execute("""
                DELETE FROM module_telemetry 
                WHERE timestamp < ?
            """, (cutoff_timestamp,))
            
            deleted_count = cursor.rowcount
            conn.commit()
            conn.close()
            
            logger.info(f"✅ Cleaned up {deleted_count} old telemetry records")
            return deleted_count
            
        except Exception as e:
            logger.error(f"❌ Failed to cleanup old data: {e}")
            return 0


# Global database service instance
db_service = DatabaseService()
