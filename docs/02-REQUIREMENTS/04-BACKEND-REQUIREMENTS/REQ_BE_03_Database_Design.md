# REQ_BE_03 - Database Design

**Document ID:** REQ_BE_03  
**Version:** 1.0  
**Date:** 2025-01-28  
**Status:** Draft  
**Classification:** Internal  
**Author:** CTO Team  

---

## 1. DOCUMENT INFORMATION

### 1.1 Document Control
- **Document Title:** Database Design
- **Document Type:** Requirements Specification
- **Standard Reference:** ISO/IEC/IEEE 29148:2018, ISO/IEC 25012:2008
- **Project:** OHT-50 Industrial Robot Control System
- **Component:** Backend Database Architecture

### 1.2 Revision History
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-01-28 | CTO Team | Initial creation |

---

## 2. DATABASE OVERVIEW

### 2.1 Database Architecture
The OHT-50 Backend System uses a hybrid database architecture:
- **Primary Database:** SQLite (local storage)
- **Cache Database:** Redis (in-memory cache)
- **File Storage:** Local file system (logs, configurations)

### 2.2 Database Schema Overview
```
Database Schema:
├── Users & Authentication
│   ├── users
│   ├── user_sessions
│   └── user_permissions
├── Robot Control
│   ├── robot_status
│   ├── robot_commands
│   └── robot_configurations
├── Telemetry Data
│   ├── telemetry_current
│   ├── telemetry_history
│   └── telemetry_alerts
├── Safety System
│   ├── safety_status
│   ├── safety_alerts
│   └── safety_logs
├── Module Management
│   ├── modules
│   ├── module_status
│   └── module_configurations
├── LiDAR System
│   ├── lidar_scans
│   ├── lidar_obstacles
│   ├── lidar_maps
│   └── lidar_paths
└── System Management
    ├── system_logs
    ├── system_configurations
    └── system_backups
```

---

## 3. SQLITE DATABASE DESIGN

### 3.1 LiDAR System Tables

#### 3.1.1 lidar_scans
```sql
CREATE TABLE lidar_scans (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    timestamp DATETIME NOT NULL,
    scan_frequency REAL NOT NULL,
    point_count INTEGER NOT NULL,
    average_quality REAL,
    processing_time_ms INTEGER,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Indexes
CREATE INDEX idx_lidar_scans_timestamp ON lidar_scans(timestamp);
CREATE INDEX idx_lidar_scans_frequency ON lidar_scans(scan_frequency);
```

#### 3.1.2 lidar_obstacles
```sql
CREATE TABLE lidar_obstacles (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    scan_id INTEGER NOT NULL,
    obstacle_id INTEGER NOT NULL,
    centroid_x REAL NOT NULL,
    centroid_y REAL NOT NULL,
    width REAL NOT NULL,
    height REAL NOT NULL,
    zone_type TEXT NOT NULL CHECK (zone_type IN ('critical', 'warning', 'safe')),
    distance REAL NOT NULL,
    angle REAL NOT NULL,
    confidence REAL NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (scan_id) REFERENCES lidar_scans(id)
);

-- Indexes
CREATE INDEX idx_lidar_obstacles_scan_id ON lidar_obstacles(scan_id);
CREATE INDEX idx_lidar_obstacles_zone_type ON lidar_obstacles(zone_type);
CREATE INDEX idx_lidar_obstacles_distance ON lidar_obstacles(distance);
```

#### 3.1.3 lidar_maps
```sql
CREATE TABLE lidar_maps (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    map_name TEXT NOT NULL,
    resolution REAL NOT NULL,
    grid_width INTEGER NOT NULL,
    grid_height INTEGER NOT NULL,
    occupancy_grid BLOB NOT NULL,
    robot_position_x REAL,
    robot_position_y REAL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Indexes
CREATE INDEX idx_lidar_maps_name ON lidar_maps(map_name);
CREATE INDEX idx_lidar_maps_updated_at ON lidar_maps(updated_at);
```

#### 3.1.4 lidar_paths
```sql
CREATE TABLE lidar_paths (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    start_x REAL NOT NULL,
    start_y REAL NOT NULL,
    goal_x REAL NOT NULL,
    goal_y REAL NOT NULL,
    path_points TEXT NOT NULL, -- JSON array of waypoints
    total_distance REAL NOT NULL,
    estimated_time REAL NOT NULL,
    safety_score REAL NOT NULL,
    algorithm_used TEXT NOT NULL,
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- Indexes
CREATE INDEX idx_lidar_paths_start_goal ON lidar_paths(start_x, start_y, goal_x, goal_y);
CREATE INDEX idx_lidar_paths_created_at ON lidar_paths(created_at);
```

### 3.2 Users & Authentication Tables

#### 3.2.1 users
```sql
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    email VARCHAR(100) UNIQUE,
    role VARCHAR(20) NOT NULL DEFAULT 'operator',
    permissions TEXT, -- JSON array of permissions
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP,
    created_by INTEGER,
    FOREIGN KEY (created_by) REFERENCES users(id)
);

-- Indexes
CREATE INDEX idx_users_username ON users(username);
CREATE INDEX idx_users_role ON users(role);
CREATE INDEX idx_users_active ON users(is_active);
```

#### 3.1.2 user_sessions
```sql
CREATE TABLE user_sessions (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    session_token VARCHAR(255) UNIQUE NOT NULL,
    refresh_token VARCHAR(255) UNIQUE NOT NULL,
    expires_at TIMESTAMP NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_activity TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    ip_address VARCHAR(45),
    user_agent TEXT,
    is_active BOOLEAN DEFAULT TRUE,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

-- Indexes
CREATE INDEX idx_user_sessions_token ON user_sessions(session_token);
CREATE INDEX idx_user_sessions_user_id ON user_sessions(user_id);
CREATE INDEX idx_user_sessions_expires ON user_sessions(expires_at);
```

### 3.2 Robot Control Tables

#### 3.2.1 robot_status
```sql
CREATE TABLE robot_status (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    robot_id VARCHAR(50) NOT NULL DEFAULT 'OHT-50-001',
    status VARCHAR(20) NOT NULL, -- idle, moving, paused, error, estop
    mode VARCHAR(20) NOT NULL, -- auto, manual, semi
    position_x DECIMAL(10,3),
    position_y DECIMAL(10,3),
    position_z DECIMAL(10,3),
    velocity_x DECIMAL(10,3),
    velocity_y DECIMAL(10,3),
    velocity_z DECIMAL(10,3),
    battery_level INTEGER, -- percentage
    battery_voltage DECIMAL(5,2),
    temperature DECIMAL(5,2),
    speed DECIMAL(5,2),
    safety_status VARCHAR(20) DEFAULT 'safe',
    last_command_id VARCHAR(50),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Indexes
CREATE INDEX idx_robot_status_robot_id ON robot_status(robot_id);
CREATE INDEX idx_robot_status_status ON robot_status(status);
CREATE INDEX idx_robot_status_updated ON robot_status(updated_at);
```

#### 3.2.2 robot_commands
```sql
CREATE TABLE robot_commands (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    command_id VARCHAR(50) UNIQUE NOT NULL,
    user_id INTEGER NOT NULL,
    command_type VARCHAR(50) NOT NULL, -- move, stop, pause, resume, home, set_mode
    command_data TEXT, -- JSON parameters
    priority VARCHAR(20) DEFAULT 'normal', -- low, normal, high, emergency
    status VARCHAR(20) DEFAULT 'pending', -- pending, executing, completed, failed, cancelled
    result TEXT, -- JSON result data
    error_message TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    started_at TIMESTAMP,
    completed_at TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

-- Indexes
CREATE INDEX idx_robot_commands_command_id ON robot_commands(command_id);
CREATE INDEX idx_robot_commands_user_id ON robot_commands(user_id);
CREATE INDEX idx_robot_commands_status ON robot_commands(status);
CREATE INDEX idx_robot_commands_created ON robot_commands(created_at);
```

#### 3.2.3 robot_configurations
```sql
CREATE TABLE robot_configurations (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    config_key VARCHAR(100) UNIQUE NOT NULL,
    config_value TEXT NOT NULL, -- JSON configuration data
    config_type VARCHAR(20) NOT NULL, -- robot, network, sensor, safety
    description TEXT,
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_by INTEGER,
    FOREIGN KEY (updated_by) REFERENCES users(id)
);

-- Indexes
CREATE INDEX idx_robot_configs_key ON robot_configurations(config_key);
CREATE INDEX idx_robot_configs_type ON robot_configurations(config_type);
CREATE INDEX idx_robot_configs_active ON robot_configurations(is_active);
```

### 3.3 Telemetry Data Tables

#### 3.3.1 telemetry_current
```sql
CREATE TABLE telemetry_current (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    robot_id VARCHAR(50) NOT NULL DEFAULT 'OHT-50-001',
    telemetry_type VARCHAR(20) NOT NULL, -- robot, system, sensor
    telemetry_data TEXT NOT NULL, -- JSON telemetry data
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Indexes
CREATE INDEX idx_telemetry_current_robot ON telemetry_current(robot_id);
CREATE INDEX idx_telemetry_current_type ON telemetry_current(telemetry_type);
CREATE INDEX idx_telemetry_current_timestamp ON telemetry_current(timestamp);
```

#### 3.3.2 telemetry_history
```sql
CREATE TABLE telemetry_history (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    robot_id VARCHAR(50) NOT NULL DEFAULT 'OHT-50-001',
    telemetry_type VARCHAR(20) NOT NULL,
    telemetry_data TEXT NOT NULL, -- JSON telemetry data
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Indexes
CREATE INDEX idx_telemetry_history_robot ON telemetry_history(robot_id);
CREATE INDEX idx_telemetry_history_type ON telemetry_history(telemetry_type);
CREATE INDEX idx_telemetry_history_timestamp ON telemetry_history(timestamp);

-- Partitioning strategy for large datasets
-- This table will be partitioned by date for performance
```

#### 3.3.3 telemetry_alerts
```sql
CREATE TABLE telemetry_alerts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    alert_id VARCHAR(50) UNIQUE NOT NULL,
    robot_id VARCHAR(50) NOT NULL DEFAULT 'OHT-50-001',
    alert_type VARCHAR(50) NOT NULL, -- battery, temperature, safety, system
    severity VARCHAR(20) NOT NULL, -- info, warning, error, critical
    message TEXT NOT NULL,
    alert_data TEXT, -- JSON additional data
    status VARCHAR(20) DEFAULT 'active', -- active, acknowledged, resolved
    acknowledged_by INTEGER,
    acknowledged_at TIMESTAMP,
    resolved_at TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (acknowledged_by) REFERENCES users(id)
);

-- Indexes
CREATE INDEX idx_telemetry_alerts_alert_id ON telemetry_alerts(alert_id);
CREATE INDEX idx_telemetry_alerts_robot ON telemetry_alerts(robot_id);
CREATE INDEX idx_telemetry_alerts_severity ON telemetry_alerts(severity);
CREATE INDEX idx_telemetry_alerts_status ON telemetry_alerts(status);
CREATE INDEX idx_telemetry_alerts_created ON telemetry_alerts(created_at);
```

### 3.4 Safety System Tables

#### 3.4.1 safety_status
```sql
CREATE TABLE safety_status (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    robot_id VARCHAR(50) NOT NULL DEFAULT 'OHT-50-001',
    emergency_stop BOOLEAN DEFAULT FALSE,
    safety_zones TEXT, -- JSON safety zone status
    obstacles TEXT, -- JSON obstacle data
    safety_switches TEXT, -- JSON safety switch status
    safety_score INTEGER, -- 0-100 safety score
    last_check TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Indexes
CREATE INDEX idx_safety_status_robot ON safety_status(robot_id);
CREATE INDEX idx_safety_status_emergency ON safety_status(emergency_stop);
CREATE INDEX idx_safety_status_last_check ON safety_status(last_check);
```

#### 3.4.2 safety_logs
```sql
CREATE TABLE safety_logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    robot_id VARCHAR(50) NOT NULL DEFAULT 'OHT-50-001',
    event_type VARCHAR(50) NOT NULL, -- estop_activated, zone_violation, switch_triggered
    event_data TEXT, -- JSON event data
    severity VARCHAR(20) NOT NULL, -- info, warning, error, critical
    user_id INTEGER, -- user who triggered or acknowledged
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

-- Indexes
CREATE INDEX idx_safety_logs_robot ON safety_logs(robot_id);
CREATE INDEX idx_safety_logs_event_type ON safety_logs(event_type);
CREATE INDEX idx_safety_logs_severity ON safety_logs(severity);
CREATE INDEX idx_safety_logs_timestamp ON safety_logs(timestamp);
```

### 3.5 Module Management Tables

#### 3.5.1 modules
```sql
CREATE TABLE modules (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    module_id VARCHAR(50) UNIQUE NOT NULL,
    module_type VARCHAR(50) NOT NULL, -- travel_motor, dock_location, safety_module
    module_name VARCHAR(100) NOT NULL,
    rs485_address INTEGER NOT NULL,
    firmware_version VARCHAR(20),
    hardware_version VARCHAR(20),
    is_active BOOLEAN DEFAULT TRUE,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Indexes
CREATE INDEX idx_modules_module_id ON modules(module_id);
CREATE INDEX idx_modules_type ON modules(module_type);
CREATE INDEX idx_modules_address ON modules(rs485_address);
CREATE INDEX idx_modules_active ON modules(is_active);
```

#### 3.5.2 module_status
```sql
CREATE TABLE module_status (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    module_id VARCHAR(50) NOT NULL,
    status VARCHAR(20) NOT NULL, -- online, offline, error, maintenance
    last_seen TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    status_data TEXT, -- JSON status data
    error_count INTEGER DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (module_id) REFERENCES modules(module_id)
);

-- Indexes
CREATE INDEX idx_module_status_module ON module_status(module_id);
CREATE INDEX idx_module_status_status ON module_status(status);
CREATE INDEX idx_module_status_last_seen ON module_status(last_seen);
```

### 3.6 System Management Tables

#### 3.6.1 system_logs
```sql
CREATE TABLE system_logs (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    log_level VARCHAR(10) NOT NULL, -- DEBUG, INFO, WARNING, ERROR, CRITICAL
    component VARCHAR(50) NOT NULL, -- api, robot, telemetry, safety, module
    message TEXT NOT NULL,
    details TEXT, -- JSON additional details
    user_id INTEGER,
    ip_address VARCHAR(45),
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (user_id) REFERENCES users(id)
);

-- Indexes
CREATE INDEX idx_system_logs_level ON system_logs(log_level);
CREATE INDEX idx_system_logs_component ON system_logs(component);
CREATE INDEX idx_system_logs_timestamp ON system_logs(timestamp);
CREATE INDEX idx_system_logs_user ON system_logs(user_id);
```

#### 3.6.2 system_backups
```sql
CREATE TABLE system_backups (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    backup_id VARCHAR(50) UNIQUE NOT NULL,
    backup_type VARCHAR(20) NOT NULL, -- full, incremental, config
    file_path VARCHAR(255) NOT NULL,
    file_size BIGINT,
    checksum VARCHAR(64),
    status VARCHAR(20) DEFAULT 'pending', -- pending, in_progress, completed, failed
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    completed_at TIMESTAMP,
    created_by INTEGER,
    FOREIGN KEY (created_by) REFERENCES users(id)
);

-- Indexes
CREATE INDEX idx_system_backups_backup_id ON system_backups(backup_id);
CREATE INDEX idx_system_backups_type ON system_backups(backup_type);
CREATE INDEX idx_system_backups_status ON system_backups(status);
CREATE INDEX idx_system_backups_created ON system_backups(created_at);
```

---

## 4. REDIS CACHE DESIGN

### 4.1 Cache Structure
Redis is used for high-performance caching and real-time data storage:

#### 4.1.1 Session Management
```
Key Pattern: session:{session_token}
Value: JSON session data
TTL: 24 hours

Key Pattern: user_sessions:{user_id}
Value: Set of active session tokens
TTL: 24 hours
```

#### 4.1.2 Real-time Data
```
Key Pattern: robot:status:{robot_id}
Value: JSON current robot status
TTL: 1 hour

Key Pattern: telemetry:current:{robot_id}
Value: JSON current telemetry data
TTL: 5 minutes

Key Pattern: safety:status:{robot_id}
Value: JSON current safety status
TTL: 1 minute
```

#### 4.1.3 Command Queue
```
Key Pattern: commands:queue
Value: List of pending commands
TTL: 1 hour

Key Pattern: commands:processing:{command_id}
Value: JSON command processing status
TTL: 10 minutes
```

#### 4.1.4 Rate Limiting
```
Key Pattern: rate_limit:{user_id}:{endpoint}
Value: Current request count
TTL: 1 minute
```

### 4.2 Redis Configuration
```yaml
Redis Configuration:
  host: localhost
  port: 6379
  database: 0
  max_connections: 100
  connection_timeout: 5
  read_timeout: 3
  write_timeout: 3
  retry_on_timeout: true
  health_check_interval: 30
```

---

## 5. DATA RETENTION POLICY

### 5.1 Retention Periods
- **Telemetry History:** 30 days
- **System Logs:** 90 days
- **Safety Logs:** 1 year
- **User Sessions:** 24 hours (active), 7 days (inactive)
- **Command History:** 30 days
- **Alert History:** 90 days
- **Backup History:** 1 year

### 5.2 Data Archival
- **Archival Strategy:** Monthly archival to compressed files
- **Archival Location:** Local storage with optional cloud backup
- **Retrieval Process:** Automated restoration procedures

### 5.3 Data Cleanup
- **Automated Cleanup:** Daily cleanup of expired data
- **Manual Cleanup:** Monthly review and cleanup procedures
- **Backup Before Cleanup:** Automatic backup before major cleanup operations

---

## 6. PERFORMANCE OPTIMIZATION

### 6.1 Indexing Strategy
- **Primary Indexes:** All primary keys and foreign keys
- **Composite Indexes:** Frequently queried combinations
- **Partial Indexes:** For active/inactive status filtering
- **Covering Indexes:** For frequently accessed data

### 6.2 Query Optimization
- **Query Analysis:** Regular query performance analysis
- **Query Caching:** Redis-based query result caching
- **Connection Pooling:** Efficient database connection management
- **Batch Operations:** Bulk insert/update operations

### 6.3 Partitioning Strategy
- **Time-based Partitioning:** For telemetry_history table
- **Range Partitioning:** For large datasets
- **Automatic Partitioning:** Monthly partition creation

---

## 7. BACKUP AND RECOVERY

### 7.1 Backup Strategy
- **Full Backup:** Daily full database backup
- **Incremental Backup:** Hourly incremental backups
- **Configuration Backup:** Real-time configuration backup
- **Log Backup:** Daily log file backup

### 7.2 Recovery Procedures
- **Point-in-time Recovery:** Support for specific timestamp recovery
- **Disaster Recovery:** Complete system restoration procedures
- **Data Validation:** Post-recovery data integrity checks
- **Rollback Procedures:** Emergency rollback capabilities

---

## 8. SECURITY REQUIREMENTS

### 8.1 Data Encryption
- **At Rest:** Database file encryption
- **In Transit:** TLS encryption for all connections
- **Backup Encryption:** Encrypted backup files
- **Sensitive Data:** Additional encryption for sensitive fields

### 8.2 Access Control
- **Database Access:** Role-based database access
- **Connection Security:** Secure connection parameters
- **Audit Logging:** Complete database access audit trail
- **Privilege Management:** Principle of least privilege

---

## 9. MONITORING AND MAINTENANCE

### 9.1 Database Monitoring
- **Performance Metrics:** Query performance monitoring
- **Resource Usage:** CPU, memory, disk usage monitoring
- **Connection Monitoring:** Active connection tracking
- **Error Monitoring:** Database error logging and alerting

### 9.2 Maintenance Procedures
- **Regular Maintenance:** Weekly database maintenance
- **Index Rebuilding:** Monthly index optimization
- **Statistics Updates:** Regular statistics updates
- **Vacuum Operations:** Periodic database cleanup

---

## 10. REFERENCES

### 10.1 Related Documents
- REQ_BE_01: System Overview
- REQ_BE_02: API Specifications
- REQ_BE_04: Security Requirements
- REQ_BE_05: Integration Specifications

### 10.2 Standards References
- ISO/IEC/IEEE 29148:2018 - Requirements Engineering
- ISO/IEC 25012:2008 - Data Quality Model
- SQLite Documentation
- Redis Documentation

---

## 11. APPROVAL

| Role | Name | Date | Signature |
|------|------|------|-----------|
| CTO | [Name] | [Date] | [Signature] |
| Technical Lead | [Name] | [Date] | [Signature] |
| Quality Assurance | [Name] | [Date] | [Signature] |

---

**Document End**
