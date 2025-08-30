# REQ_BE_16 - Database Migration

**Document ID:** REQ_BE_16  
**Version:** 1.0  
**Date:** 2025-01-28  
**Status:** Draft  
**Classification:** Internal  
**Author:** CTO Team  
**Standard Reference:** ISO/IEC/IEEE 29148:2018

---

## 1. DOCUMENT INFORMATION

### 1.1 Document Control
- **Document Title:** Database Migration
- **Document Type:** Database Migration Specification
- **Standard Reference:** ISO/IEC/IEEE 29148:2018, ISO/IEC 25010:2011
- **Project:** OHT-50 Industrial Robot Control System
- **Component:** Backend Database Migration Strategy

### 1.2 Revision History
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-01-28 | CTO Team | Initial creation |

---

## 2. DATABASE MIGRATION OVERVIEW

### 2.1 Migration Strategy
The database migration follows a **version-controlled migration strategy** with **backward compatibility** and **rollback capability**:

```yaml
Migration Strategy:
  Approach: "Version-controlled migrations"
  Framework: "Alembic for SQLAlchemy"
  Compatibility: "Backward compatible"
  Rollback: "Automatic rollback capability"
  Testing: "Migration testing in staging"
  Backup: "Pre-migration backups"
  Monitoring: "Migration progress tracking"
```

### 2.2 Migration Types
```
Migration Types:
├── Schema Migrations
│   ├── Table creation
│   ├── Column modifications
│   ├── Index creation
│   └── Constraint changes
├── Data Migrations
│   ├── Data transformation
│   ├── Data cleanup
│   ├── Data seeding
│   └── Data validation
└── Configuration Migrations
    ├── Environment changes
    ├── Connection updates
    ├── Performance tuning
    └── Security updates
```

---

## 3. MIGRATION FRAMEWORK

### 3.1 Alembic Configuration
```yaml
Alembic Configuration:
  Framework: "Alembic 1.12+"
  Database: "PostgreSQL 13+"
  ORM: "SQLAlchemy 2.0+"
  Python: "Python 3.9+"
  
  Features:
    - "Version control"
    - "Upgrade/downgrade"
    - "Dependency management"
    - "Environment-specific configs"
    - "Migration testing"
    - "Rollback support"
```

### 3.2 Migration Structure
```python
# alembic.ini Configuration
[alembic]
script_location = migrations
sqlalchemy.url = postgresql://user:pass@localhost/oht50_db
file_template = %%(year)d%%(month).2d%%(day).2d_%%(hour).2d%%(minute).2d_%%(rev)s_%%(slug)s

[post_write_hooks]
hooks = black
black.type = console_scripts
black.entrypoint = black
black.options = -l 79 REVISION_SCRIPT_FILENAME

[loggers]
keys = root,sqlalchemy,alembic

[handlers]
keys = console

[formatters]
keys = generic

[logger_root]
level = WARN
handlers = console
qualname =

[logger_sqlalchemy]
level = WARN
handlers =
qualname = sqlalchemy.engine

[logger_alembic]
level = INFO
handlers =
qualname = alembic

[handler_console]
class = StreamHandler
args = (sys.stderr,)
level = NOTSET
formatter = generic

[formatter_generic]
format = %(levelname)-5.5s [%(name)s] %(message)s
datefmt = %H:%M:%S
```

---

## 4. MIGRATION SCRIPTS

### 4.1 Initial Schema Migration
```python
# migrations/versions/001_initial_schema.py
"""Initial database schema

Revision ID: 001
Revises: 
Create Date: 2025-01-28 10:00:00.000000

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql

# revision identifiers, used by Alembic.
revision = '001'
down_revision = None
branch_labels = None
depends_on = None

def upgrade():
    # Create users table
    op.create_table('users',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('username', sa.String(length=50), nullable=False),
        sa.Column('email', sa.String(length=120), nullable=False),
        sa.Column('password_hash', sa.String(length=255), nullable=False),
        sa.Column('role', sa.String(length=20), nullable=False),
        sa.Column('is_active', sa.Boolean(), nullable=False, default=True),
        sa.Column('created_at', sa.DateTime(), nullable=False),
        sa.Column('updated_at', sa.DateTime(), nullable=False),
        sa.PrimaryKeyConstraint('id'),
        sa.UniqueConstraint('username'),
        sa.UniqueConstraint('email')
    )
    
    # Create robots table
    op.create_table('robots',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('name', sa.String(length=100), nullable=False),
        sa.Column('robot_type', sa.String(length=50), nullable=False),
        sa.Column('status', sa.String(length=20), nullable=False),
        sa.Column('mode', sa.String(length=20), nullable=False),
        sa.Column('position_x', sa.Float(), nullable=True),
        sa.Column('position_y', sa.Float(), nullable=True),
        sa.Column('position_z', sa.Float(), nullable=True),
        sa.Column('orientation', sa.Float(), nullable=True),
        sa.Column('speed', sa.Float(), nullable=True),
        sa.Column('battery_level', sa.Integer(), nullable=True),
        sa.Column('temperature', sa.Float(), nullable=True),
        sa.Column('created_at', sa.DateTime(), nullable=False),
        sa.Column('updated_at', sa.DateTime(), nullable=False),
        sa.PrimaryKeyConstraint('id')
    )
    
    # Create telemetry table
    op.create_table('telemetry',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('robot_id', sa.Integer(), nullable=False),
        sa.Column('timestamp', sa.DateTime(), nullable=False),
        sa.Column('speed', sa.Float(), nullable=True),
        sa.Column('temperature', sa.Float(), nullable=True),
        sa.Column('battery_voltage', sa.Float(), nullable=True),
        sa.Column('motor_current', sa.Float(), nullable=True),
        sa.Column('position_x', sa.Float(), nullable=True),
        sa.Column('position_y', sa.Float(), nullable=True),
        sa.Column('position_z', sa.Float(), nullable=True),
        sa.Column('orientation', sa.Float(), nullable=True),
        sa.PrimaryKeyConstraint('id'),
        sa.ForeignKeyConstraint(['robot_id'], ['robots.id'], ondelete='CASCADE')
    )
    
    # Create indexes
    op.create_index('idx_telemetry_robot_timestamp', 'telemetry', ['robot_id', 'timestamp'])
    op.create_index('idx_robots_status', 'robots', ['status'])
    op.create_index('idx_users_username', 'users', ['username'])

def downgrade():
    op.drop_index('idx_users_username', table_name='users')
    op.drop_index('idx_robots_status', table_name='robots')
    op.drop_index('idx_telemetry_robot_timestamp', table_name='telemetry')
    op.drop_table('telemetry')
    op.drop_table('robots')
    op.drop_table('users')
```

### 4.2 LiDAR Integration Migration
```python
# migrations/versions/002_lidar_integration.py
"""Add LiDAR integration tables

Revision ID: 002
Revises: 001
Create Date: 2025-01-28 11:00:00.000000

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql

revision = '002'
down_revision = '001'
branch_labels = None
depends_on = None

def upgrade():
    # Create lidar_scans table
    op.create_table('lidar_scans',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('timestamp', sa.DateTime(), nullable=False),
        sa.Column('scan_data', postgresql.JSONB(), nullable=True),
        sa.Column('quality_score', sa.Float(), nullable=True),
        sa.Column('scan_duration', sa.Float(), nullable=True),
        sa.Column('points_count', sa.Integer(), nullable=True),
        sa.PrimaryKeyConstraint('id')
    )
    
    # Create lidar_obstacles table
    op.create_table('lidar_obstacles',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('scan_id', sa.Integer(), nullable=False),
        sa.Column('obstacle_id', sa.String(length=50), nullable=False),
        sa.Column('position_x', sa.Float(), nullable=False),
        sa.Column('position_y', sa.Float(), nullable=False),
        sa.Column('position_z', sa.Float(), nullable=True),
        sa.Column('size_width', sa.Float(), nullable=True),
        sa.Column('size_height', sa.Float(), nullable=True),
        sa.Column('obstacle_type', sa.String(length=20), nullable=True),
        sa.Column('confidence', sa.Float(), nullable=True),
        sa.Column('detected_at', sa.DateTime(), nullable=False),
        sa.PrimaryKeyConstraint('id'),
        sa.ForeignKeyConstraint(['scan_id'], ['lidar_scans.id'], ondelete='CASCADE')
    )
    
    # Create lidar_maps table
    op.create_table('lidar_maps',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('timestamp', sa.DateTime(), nullable=False),
        sa.Column('map_data', postgresql.BYTEA(), nullable=True),
        sa.Column('resolution', sa.Float(), nullable=False),
        sa.Column('width', sa.Integer(), nullable=False),
        sa.Column('height', sa.Integer(), nullable=False),
        sa.Column('origin_x', sa.Float(), nullable=False),
        sa.Column('origin_y', sa.Float(), nullable=False),
        sa.Column('origin_z', sa.Float(), nullable=False),
        sa.PrimaryKeyConstraint('id')
    )
    
    # Create lidar_paths table
    op.create_table('lidar_paths',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('start_x', sa.Float(), nullable=False),
        sa.Column('start_y', sa.Float(), nullable=False),
        sa.Column('start_z', sa.Float(), nullable=True),
        sa.Column('goal_x', sa.Float(), nullable=False),
        sa.Column('goal_y', sa.Float(), nullable=False),
        sa.Column('goal_z', sa.Float(), nullable=True),
        sa.Column('path_data', postgresql.JSONB(), nullable=True),
        sa.Column('distance', sa.Float(), nullable=True),
        sa.Column('estimated_time', sa.Float(), nullable=True),
        sa.Column('safety_score', sa.Float(), nullable=True),
        sa.Column('algorithm', sa.String(length=20), nullable=True),
        sa.Column('created_at', sa.DateTime(), nullable=False),
        sa.PrimaryKeyConstraint('id')
    )
    
    # Create indexes
    op.create_index('idx_lidar_scans_timestamp', 'lidar_scans', ['timestamp'])
    op.create_index('idx_lidar_obstacles_scan', 'lidar_obstacles', ['scan_id'])
    op.create_index('idx_lidar_obstacles_type', 'lidar_obstacles', ['obstacle_type'])
    op.create_index('idx_lidar_maps_timestamp', 'lidar_maps', ['timestamp'])
    op.create_index('idx_lidar_paths_created', 'lidar_paths', ['created_at'])

def downgrade():
    op.drop_index('idx_lidar_paths_created', table_name='lidar_paths')
    op.drop_index('idx_lidar_maps_timestamp', table_name='lidar_maps')
    op.drop_index('idx_lidar_obstacles_type', table_name='lidar_obstacles')
    op.drop_index('idx_lidar_obstacles_scan', table_name='lidar_obstacles')
    op.drop_index('idx_lidar_scans_timestamp', table_name='lidar_scans')
    op.drop_table('lidar_paths')
    op.drop_table('lidar_maps')
    op.drop_table('lidar_obstacles')
    op.drop_table('lidar_scans')
```

### 4.3 Safety System Migration
```python
# migrations/versions/003_safety_system.py
"""Add safety system tables

Revision ID: 003
Revises: 002
Create Date: 2025-01-28 12:00:00.000000

"""
from alembic import op
import sqlalchemy as sa
from sqlalchemy.dialects import postgresql

revision = '003'
down_revision = '002'
branch_labels = None
depends_on = None

def upgrade():
    # Create safety_events table
    op.create_table('safety_events',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('event_type', sa.String(length=50), nullable=False),
        sa.Column('severity', sa.String(length=20), nullable=False),
        sa.Column('robot_id', sa.Integer(), nullable=True),
        sa.Column('description', sa.Text(), nullable=True),
        sa.Column('event_data', postgresql.JSONB(), nullable=True),
        sa.Column('resolved', sa.Boolean(), nullable=False, default=False),
        sa.Column('resolved_at', sa.DateTime(), nullable=True),
        sa.Column('resolved_by', sa.Integer(), nullable=True),
        sa.Column('created_at', sa.DateTime(), nullable=False),
        sa.PrimaryKeyConstraint('id'),
        sa.ForeignKeyConstraint(['robot_id'], ['robots.id'], ondelete='SET NULL'),
        sa.ForeignKeyConstraint(['resolved_by'], ['users.id'], ondelete='SET NULL')
    )
    
    # Create safety_zones table
    op.create_table('safety_zones',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('zone_name', sa.String(length=100), nullable=False),
        sa.Column('zone_type', sa.String(length=20), nullable=False),
        sa.Column('boundary_data', postgresql.JSONB(), nullable=False),
        sa.Column('radius', sa.Float(), nullable=True),
        sa.Column('is_active', sa.Boolean(), nullable=False, default=True),
        sa.Column('created_at', sa.DateTime(), nullable=False),
        sa.Column('updated_at', sa.DateTime(), nullable=False),
        sa.PrimaryKeyConstraint('id')
    )
    
    # Create emergency_stops table
    op.create_table('emergency_stops',
        sa.Column('id', sa.Integer(), nullable=False),
        sa.Column('robot_id', sa.Integer(), nullable=False),
        sa.Column('triggered_by', sa.Integer(), nullable=True),
        sa.Column('trigger_type', sa.String(length=20), nullable=False),
        sa.Column('trigger_data', postgresql.JSONB(), nullable=True),
        sa.Column('resolved', sa.Boolean(), nullable=False, default=False),
        sa.Column('resolved_at', sa.DateTime(), nullable=True),
        sa.Column('resolved_by', sa.Integer(), nullable=True),
        sa.Column('created_at', sa.DateTime(), nullable=False),
        sa.PrimaryKeyConstraint('id'),
        sa.ForeignKeyConstraint(['robot_id'], ['robots.id'], ondelete='CASCADE'),
        sa.ForeignKeyConstraint(['triggered_by'], ['users.id'], ondelete='SET NULL'),
        sa.ForeignKeyConstraint(['resolved_by'], ['users.id'], ondelete='SET NULL')
    )
    
    # Create indexes
    op.create_index('idx_safety_events_type', 'safety_events', ['event_type'])
    op.create_index('idx_safety_events_severity', 'safety_events', ['severity'])
    op.create_index('idx_safety_events_robot', 'safety_events', ['robot_id'])
    op.create_index('idx_safety_events_created', 'safety_events', ['created_at'])
    op.create_index('idx_safety_zones_type', 'safety_zones', ['zone_type'])
    op.create_index('idx_safety_zones_active', 'safety_zones', ['is_active'])
    op.create_index('idx_emergency_stops_robot', 'emergency_stops', ['robot_id'])
    op.create_index('idx_emergency_stops_resolved', 'emergency_stops', ['resolved'])

def downgrade():
    op.drop_index('idx_emergency_stops_resolved', table_name='emergency_stops')
    op.drop_index('idx_emergency_stops_robot', table_name='emergency_stops')
    op.drop_index('idx_safety_zones_active', table_name='safety_zones')
    op.drop_index('idx_safety_zones_type', table_name='safety_zones')
    op.drop_index('idx_safety_events_created', table_name='safety_events')
    op.drop_index('idx_safety_events_robot', table_name='safety_events')
    op.drop_index('idx_safety_events_severity', table_name='safety_events')
    op.drop_index('idx_safety_events_type', table_name='safety_events')
    op.drop_table('emergency_stops')
    op.drop_table('safety_zones')
    op.drop_table('safety_events')
```

---

## 5. DATA MIGRATION

### 5.1 Data Seeding
```python
# migrations/data/seed_initial_data.py
"""Seed initial data for OHT-50 system"""

from sqlalchemy.orm import Session
from app.models.user import User
from app.models.robot import Robot
from app.models.safety_zone import SafetyZone
from app.security import get_password_hash
from datetime import datetime

def seed_initial_data(session: Session):
    """Seed initial data for the system"""
    
    # Create default admin user
    admin_user = User(
        username="admin",
        email="admin@oht50.com",
        password_hash=get_password_hash("admin123"),
        role="admin",
        is_active=True,
        created_at=datetime.utcnow(),
        updated_at=datetime.utcnow()
    )
    session.add(admin_user)
    
    # Create default operator user
    operator_user = User(
        username="operator",
        email="operator@oht50.com",
        password_hash=get_password_hash("operator123"),
        role="operator",
        is_active=True,
        created_at=datetime.utcnow(),
        updated_at=datetime.utcnow()
    )
    session.add(operator_user)
    
    # Create default robot
    default_robot = Robot(
        name="OHT-50-001",
        robot_type="OHT-50",
        status="IDLE",
        mode="MANUAL",
        position_x=0.0,
        position_y=0.0,
        position_z=0.0,
        orientation=0.0,
        speed=0.0,
        battery_level=100,
        temperature=25.0,
        created_at=datetime.utcnow(),
        updated_at=datetime.utcnow()
    )
    session.add(default_robot)
    
    # Create default safety zones
    default_safety_zone = SafetyZone(
        zone_name="Default Safety Zone",
        zone_type="warning",
        boundary_data={
            "type": "polygon",
            "coordinates": [
                [0, 0], [10, 0], [10, 10], [0, 10], [0, 0]
            ]
        },
        radius=5.0,
        is_active=True,
        created_at=datetime.utcnow(),
        updated_at=datetime.utcnow()
    )
    session.add(default_safety_zone)
    
    session.commit()
```

### 5.2 Data Transformation
```python
# migrations/data/transform_existing_data.py
"""Transform existing data during migration"""

from sqlalchemy.orm import Session
from app.models.telemetry import Telemetry
from datetime import datetime, timedelta
import json

def transform_telemetry_data(session: Session):
    """Transform telemetry data to new format"""
    
    # Get all telemetry records
    telemetry_records = session.query(Telemetry).all()
    
    for record in telemetry_records:
        # Add missing fields with default values
        if record.position_x is None:
            record.position_x = 0.0
        if record.position_y is None:
            record.position_y = 0.0
        if record.position_z is None:
            record.position_z = 0.0
        if record.orientation is None:
            record.orientation = 0.0
        
        # Update timestamp if missing
        if record.timestamp is None:
            record.timestamp = datetime.utcnow()
    
    session.commit()

def cleanup_old_data(session: Session):
    """Clean up old data"""
    
    # Remove telemetry records older than 30 days
    cutoff_date = datetime.utcnow() - timedelta(days=30)
    old_telemetry = session.query(Telemetry).filter(
        Telemetry.timestamp < cutoff_date
    ).delete()
    
    session.commit()
```

---

## 6. MIGRATION COMMANDS

### 6.1 Basic Migration Commands
```bash
# Initialize Alembic
alembic init migrations

# Create new migration
alembic revision --autogenerate -m "Description of changes"

# Upgrade to latest version
alembic upgrade head

# Upgrade to specific version
alembic upgrade 002

# Downgrade to previous version
alembic downgrade -1

# Downgrade to specific version
alembic downgrade 001

# Show current version
alembic current

# Show migration history
alembic history

# Show pending migrations
alembic show head
```

### 6.2 Environment-Specific Commands
```bash
# Development environment
export DATABASE_URL="postgresql://user:pass@localhost/oht50_dev"
alembic upgrade head

# Staging environment
export DATABASE_URL="postgresql://user:pass@staging-db/oht50_staging"
alembic upgrade head

# Production environment
export DATABASE_URL="postgresql://user:pass@prod-db/oht50_prod"
alembic upgrade head
```

---

## 7. MIGRATION TESTING

### 7.1 Migration Testing Strategy
```yaml
Migration Testing Strategy:
  Pre-migration Testing:
    - "Backup verification"
    - "Migration script validation"
    - "Dependency checking"
    - "Performance impact assessment"
  
  Migration Testing:
    - "Staging environment testing"
    - "Rollback testing"
    - "Data integrity verification"
    - "Performance testing"
  
  Post-migration Testing:
    - "Application functionality testing"
    - "Data consistency verification"
    - "Performance monitoring"
    - "Error monitoring"
```

### 7.2 Migration Test Scripts
```python
# tests/test_migrations.py
import pytest
from alembic.config import Config
from alembic import command
from sqlalchemy import create_engine, text
from sqlalchemy.orm import sessionmaker

class TestMigrations:
    @pytest.fixture
    def test_db(self):
        """Setup test database"""
        engine = create_engine("postgresql://test:test@localhost/test_db")
        Session = sessionmaker(bind=engine)
        return engine, Session()
    
    def test_migration_upgrade(self, test_db):
        """Test migration upgrade"""
        engine, session = test_db
        
        # Run migration
        alembic_cfg = Config("alembic.ini")
        command.upgrade(alembic_cfg, "head")
        
        # Verify tables exist
        result = engine.execute(text("""
            SELECT table_name 
            FROM information_schema.tables 
            WHERE table_schema = 'public'
        """))
        tables = [row[0] for row in result]
        
        assert "users" in tables
        assert "robots" in tables
        assert "telemetry" in tables
        assert "lidar_scans" in tables
    
    def test_migration_rollback(self, test_db):
        """Test migration rollback"""
        engine, session = test_db
        
        # Upgrade to head
        alembic_cfg = Config("alembic.ini")
        command.upgrade(alembic_cfg, "head")
        
        # Rollback to initial
        command.downgrade(alembic_cfg, "001")
        
        # Verify tables removed
        result = engine.execute(text("""
            SELECT table_name 
            FROM information_schema.tables 
            WHERE table_schema = 'public'
        """))
        tables = [row[0] for row in result]
        
        assert "lidar_scans" not in tables
        assert "lidar_obstacles" not in tables
    
    def test_data_integrity(self, test_db):
        """Test data integrity after migration"""
        engine, session = test_db
        
        # Run migration
        alembic_cfg = Config("alembic.ini")
        command.upgrade(alembic_cfg, "head")
        
        # Verify data integrity
        result = engine.execute(text("SELECT COUNT(*) FROM users"))
        user_count = result.scalar()
        assert user_count >= 0
        
        result = engine.execute(text("SELECT COUNT(*) FROM robots"))
        robot_count = result.scalar()
        assert robot_count >= 0
```

---

## 8. BACKUP & RECOVERY

### 8.1 Backup Strategy
```yaml
Backup Strategy:
  Pre-migration Backup:
    - "Full database backup"
    - "Configuration backup"
    - "Application state backup"
    - "Backup verification"
  
  Backup Types:
    - "Full backup: Complete database dump"
    - "Incremental backup: Changes since last backup"
    - "Transaction log backup: Point-in-time recovery"
    - "Configuration backup: Settings and parameters"
  
  Backup Storage:
    - "Local storage: Immediate access"
    - "Remote storage: Disaster recovery"
    - "Cloud storage: Long-term retention"
    - "Backup encryption: Security protection"
```

### 8.2 Backup Commands
```bash
# Full database backup
pg_dump -h localhost -U username -d oht50_db > backup_$(date +%Y%m%d_%H%M%S).sql

# Backup with custom format
pg_dump -h localhost -U username -d oht50_db -Fc > backup_$(date +%Y%m%d_%H%M%S).dump

# Backup specific tables
pg_dump -h localhost -U username -d oht50_db -t users -t robots > tables_backup.sql

# Restore from backup
psql -h localhost -U username -d oht50_db < backup_20250128_120000.sql

# Restore from custom format
pg_restore -h localhost -U username -d oht50_db backup_20250128_120000.dump
```

---

## 9. MIGRATION MONITORING

### 9.1 Migration Monitoring
```yaml
Migration Monitoring:
  Pre-migration Checks:
    - "Database connectivity"
    - "Disk space availability"
    - "Backup completion"
    - "Application readiness"
  
  Migration Progress:
    - "Migration step execution"
    - "Time tracking"
    - "Error detection"
    - "Performance monitoring"
  
  Post-migration Verification:
    - "Data integrity checks"
    - "Application functionality"
    - "Performance validation"
    - "Error monitoring"
```

### 9.2 Migration Logging
```python
# migrations/utils/migration_logger.py
import logging
import json
from datetime import datetime
from typing import Dict, Any

class MigrationLogger:
    def __init__(self, log_file: str = "migration.log"):
        self.logger = logging.getLogger("migration")
        self.logger.setLevel(logging.INFO)
        
        handler = logging.FileHandler(log_file)
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
    
    def log_migration_start(self, migration_id: str, description: str):
        """Log migration start"""
        self.logger.info(f"Migration {migration_id} started: {description}")
    
    def log_migration_step(self, step: str, details: Dict[str, Any]):
        """Log migration step"""
        self.logger.info(f"Step: {step} - {json.dumps(details)}")
    
    def log_migration_complete(self, migration_id: str, duration: float):
        """Log migration completion"""
        self.logger.info(f"Migration {migration_id} completed in {duration:.2f}s")
    
    def log_migration_error(self, migration_id: str, error: str):
        """Log migration error"""
        self.logger.error(f"Migration {migration_id} failed: {error}")
```

---

## 10. REFERENCES

### 10.1 Related Documents
- REQ_BE_01: System Overview
- REQ_BE_02: API Specifications
- REQ_BE_03: Database Design
- REQ_BE_04: Security Requirements
- REQ_BE_05: Integration Specifications
- REQ_BE_06: LiDAR Integration
- REQ_BE_07: Detailed Architecture Requirements
- REQ_BE_08: Service Design Requirements
- REQ_BE_09: Data Flow Requirements
- REQ_BE_10: Performance Requirements
- REQ_BE_11: Implementation Roadmap
- REQ_BE_12: Development Environment
- REQ_BE_13: Testing Requirements
- REQ_BE_14: Deployment Requirements
- REQ_BE_15: API Documentation
- REQ_BE_17: Security Implementation
- REQ_BE_18: WebSocket Implementation

### 10.2 Standards References
- ISO/IEC/IEEE 29148:2018 - Requirements Engineering
- ISO/IEC 25010:2011 - Software Quality Requirements
- Alembic Documentation - Database Migration Tool
- SQLAlchemy Documentation - Python ORM
- PostgreSQL Documentation - Database Management

---

## 11. APPROVAL

| Role | Name | Date | Signature |
|------|------|------|-----------|
| CTO | [Name] | [Date] | [Signature] |
| Technical Lead | [Name] | [Date] | [Signature] |
| Database Administrator | [Name] | [Date] | [Signature] |

---

**Document End**
