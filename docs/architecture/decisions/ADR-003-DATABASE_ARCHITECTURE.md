# ADR-003: Database Architecture Decision

## Status
**APPROVED** - 2024-08-21

## Context
OHT-50 Master Module cần hệ thống lưu trữ dữ liệu cho:
- Configuration management
- Mission history và tracking
- Telemetry data storage
- Safety event logging
- User management
- Audit trails
- Real-time data access

## Decision
**Chọn PostgreSQL với Redis caching và SQLite cho development**

### Database Architecture:
```
┌─────────────────────────────────────┐
│         Application Layer           │
└─────────────────┬───────────────────┘
                  │
            ┌─────▼─────┐
            │   ORM     │
            │(SQLAlchemy)│
            └─────┬─────┘
                  │
            ┌─────▼─────┐
            │ PostgreSQL│
            │  Primary  │
            │ Database  │
            └─────┬─────┘
                  │
            ┌─────▼─────┐
            │   Redis   │
            │   Cache   │
            └───────────┘
```

### Database Schema Design:
```sql
-- Core Tables
CREATE TABLE users (
    id SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    role VARCHAR(20) NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE configurations (
    id SERIAL PRIMARY KEY,
    config_key VARCHAR(100) UNIQUE NOT NULL,
    config_value TEXT NOT NULL,
    config_type VARCHAR(50) NOT NULL,
    version VARCHAR(20) NOT NULL,
    created_by INTEGER REFERENCES users(id),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE missions (
    id SERIAL PRIMARY KEY,
    mission_id VARCHAR(50) UNIQUE NOT NULL,
    mission_type VARCHAR(50) NOT NULL,
    parameters JSONB NOT NULL,
    status VARCHAR(20) NOT NULL,
    created_by INTEGER REFERENCES users(id),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    started_at TIMESTAMP,
    completed_at TIMESTAMP,
    error_message TEXT
);

CREATE TABLE telemetry_data (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    module_id VARCHAR(50) NOT NULL,
    sensor_data JSONB,
    position_data JSONB,
    status_data JSONB,
    data_type VARCHAR(50) NOT NULL
);

CREATE TABLE safety_events (
    id SERIAL PRIMARY KEY,
    timestamp TIMESTAMP NOT NULL,
    event_type VARCHAR(50) NOT NULL,
    severity VARCHAR(20) NOT NULL,
    description TEXT,
    event_data JSONB,
    module_id VARCHAR(50),
    resolved BOOLEAN DEFAULT FALSE,
    resolved_at TIMESTAMP
);
```

## Consequences

### Positive:
- **ACID Compliance**: Full transaction support
- **JSON Support**: Native JSONB cho flexible data
- **Performance**: Optimized cho read/write operations
- **Scalability**: Horizontal scaling capabilities
- **Reliability**: Mature và battle-tested
- **Tooling**: Rich ecosystem và monitoring

### Negative:
- **Complexity**: Setup và maintenance overhead
- **Resource Usage**: Higher memory và CPU requirements
- **Learning Curve**: SQL expertise required
- **Cost**: Licensing cho enterprise features

### Risks:
- **Performance Bottleneck**: Single database instance
- **Data Loss**: Backup và recovery complexity
- **Scaling Issues**: Vertical scaling limits
- **Connection Limits**: Connection pool management

## Mitigation Strategies:
1. **Connection Pooling**: SQLAlchemy connection management
2. **Indexing Strategy**: Optimized query performance
3. **Partitioning**: Time-based data partitioning
4. **Backup Strategy**: Automated backup procedures
5. **Monitoring**: Database performance monitoring

## Performance Impact Assessment:
- **Query Performance**: < 10ms cho simple queries
- **Connection Pool**: 10-20 connections
- **Storage Growth**: ~1GB/month cho telemetry
- **Backup Time**: < 5 minutes
- **Recovery Time**: < 10 minutes

## Data Retention Policy:
```yaml
data_retention:
  telemetry_data: "90 days"
  safety_events: "1 year"
  mission_history: "2 years"
  audit_logs: "3 years"
  configurations: "indefinite"
```

## Caching Strategy:
```python
# Redis Cache Configuration
CACHE_CONFIG = {
    "telemetry": {
        "ttl": 300,  # 5 minutes
        "max_size": "100MB"
    },
    "configurations": {
        "ttl": 3600,  # 1 hour
        "max_size": "50MB"
    },
    "user_sessions": {
        "ttl": 1800,  # 30 minutes
        "max_size": "20MB"
    }
}
```

## Migration Strategy:
1. **Development**: SQLite cho local development
2. **Staging**: PostgreSQL với test data
3. **Production**: PostgreSQL với production data
4. **Backup**: Automated daily backups
5. **Monitoring**: Performance metrics collection

## Alternatives Considered:
1. **MongoDB**: NoSQL cho flexible schema
2. **MySQL**: Traditional relational database
3. **SQLite**: Lightweight cho embedded systems
4. **TimescaleDB**: Time-series database

## Compliance Requirements:
- **Data Integrity**: ACID transactions
- **Audit Trail**: Complete data history
- **Backup**: Automated backup procedures
- **Security**: Encrypted data storage
- **Performance**: SLA compliance

## Review Schedule:
- **Weekly**: Performance monitoring
- **Monthly**: Backup verification
- **Quarterly**: Schema optimization
- **Annually**: Database architecture review

---

**Approved by:** CTO  
**Date:** 2024-08-21  
**Next Review:** 2024-09-21
