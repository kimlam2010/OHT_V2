# REQ_BE_12 - Development Environment

**Document ID:** REQ_BE_12  
**Version:** 1.0  
**Date:** 2025-01-28  
**Status:** Draft  
**Classification:** Internal  
**Author:** CTO Team  
**Standard Reference:** ISO/IEC/IEEE 29148:2018

---

## 1. DOCUMENT INFORMATION

### 1.1 Document Control
- **Document Title:** Development Environment
- **Document Type:** Environment Specification
- **Standard Reference:** ISO/IEC/IEEE 29148:2018, ISO/IEC 25010:2011
- **Project:** OHT-50 Industrial Robot Control System
- **Component:** Backend Development Environment

### 1.2 Revision History
| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 1.0 | 2025-01-28 | CTO Team | Initial creation |

---

## 2. DEVELOPMENT ENVIRONMENT OVERVIEW

### 2.1 Environment Strategy
The development environment follows a **containerized approach** with **local development** and **staging environments**:

```yaml
Environment Strategy:
  Approach: "Containerized development"
  Local Development: "Docker Compose"
  Staging Environment: "Docker Swarm/Kubernetes"
  Production Environment: "Cloud deployment"
  Version Control: "Git with GitHub"
  CI/CD: "GitHub Actions"
```

### 2.2 Environment Architecture
```
Development Environment Architecture:
├── Local Development
│   ├── Docker Compose Setup
│   ├── Database (SQLite/PostgreSQL)
│   ├── Redis Cache
│   ├── API Server (FastAPI)
│   └── Development Tools
├── Staging Environment
│   ├── Docker Swarm/Kubernetes
│   ├── Production-like Database
│   ├── Load Balancer
│   ├── Monitoring Stack
│   └── Testing Tools
└── Production Environment
    ├── Cloud Infrastructure
    ├── High Availability Setup
    ├── Backup Systems
    ├── Monitoring & Alerting
    └── Security Measures
```

---

## 3. LOCAL DEVELOPMENT SETUP

### 3.1 System Requirements
```yaml
System Requirements:
  Operating System:
    - "Linux (Ubuntu 20.04+ / CentOS 8+)"
    - "macOS (10.15+)"
    - "Windows 10/11 (WSL2 recommended)"
  
  Hardware Requirements:
    - "CPU: 4+ cores"
    - "RAM: 8GB+ (16GB recommended)"
    - "Storage: 50GB+ free space"
    - "Network: Stable internet connection"
  
  Software Requirements:
    - "Docker Desktop 4.0+"
    - "Docker Compose 2.0+"
    - "Git 2.30+"
    - "Python 3.9+"
    - "Node.js 16+ (for frontend integration)"
    - "VS Code / PyCharm / Vim"
```

### 3.2 Docker Configuration
```yaml
Docker Configuration:
  Base Images:
    - "Python: 3.9-slim"
    - "PostgreSQL: 13-alpine"
    - "Redis: 6-alpine"
    - "Nginx: 1.21-alpine"
  
  Container Setup:
    - "API Server: FastAPI with Uvicorn"
    - "Database: PostgreSQL with pgAdmin"
    - "Cache: Redis with Redis Commander"
    - "Reverse Proxy: Nginx"
    - "Monitoring: Prometheus + Grafana"
  
  Volume Mounts:
    - "Source code: /app"
    - "Database data: /var/lib/postgresql/data"
    - "Redis data: /data"
    - "Logs: /var/log"
    - "Configs: /etc/config"
```

### 3.3 Docker Compose Setup
```yaml
Docker Compose Configuration:
  Services:
    api-server:
      build: "./backend"
      ports: ["8000:8000"]
      environment:
        - DATABASE_URL=postgresql://user:pass@db:5432/oht50
        - REDIS_URL=redis://redis:6379
        - ENVIRONMENT=development
      volumes:
        - "./backend:/app"
        - "./logs:/var/log"
      depends_on: ["db", "redis"]
    
    db:
      image: "postgres:13-alpine"
      ports: ["5432:5432"]
      environment:
        - POSTGRES_DB=oht50
        - POSTGRES_USER=oht50_user
        - POSTGRES_PASSWORD=oht50_pass
      volumes:
        - "postgres_data:/var/lib/postgresql/data"
        - "./database/init:/docker-entrypoint-initdb.d"
    
    redis:
      image: "redis:6-alpine"
      ports: ["6379:6379"]
      volumes:
        - "redis_data:/data"
    
    pgadmin:
      image: "dpage/pgadmin4"
      ports: ["5050:80"]
      environment:
        - PGADMIN_DEFAULT_EMAIL=admin@oht50.local
        - PGADMIN_DEFAULT_PASSWORD=admin123
      depends_on: ["db"]
    
    redis-commander:
      image: "rediscommander/redis-commander"
      ports: ["8081:8081"]
      environment:
        - REDIS_HOSTS=local:redis:6379
      depends_on: ["redis"]
    
    nginx:
      image: "nginx:1.21-alpine"
      ports: ["80:80", "443:443"]
      volumes:
        - "./nginx/nginx.conf:/etc/nginx/nginx.conf"
        - "./nginx/ssl:/etc/nginx/ssl"
      depends_on: ["api-server"]
    
    prometheus:
      image: "prom/prometheus"
      ports: ["9090:9090"]
      volumes:
        - "./monitoring/prometheus.yml:/etc/prometheus/prometheus.yml"
        - "prometheus_data:/prometheus"
    
    grafana:
      image: "grafana/grafana"
      ports: ["3000:3000"]
      environment:
        - GF_SECURITY_ADMIN_PASSWORD=admin123
      volumes:
        - "grafana_data:/var/lib/grafana"
        - "./monitoring/grafana/dashboards:/etc/grafana/provisioning/dashboards"
        - "./monitoring/grafana/datasources:/etc/grafana/provisioning/datasources"
  
  Volumes:
    postgres_data:
    redis_data:
    prometheus_data:
    grafana_data:
```

---

## 4. DEVELOPMENT TOOLS

### 4.1 IDE Configuration
```yaml
IDE Configuration:
  VS Code Setup:
    Extensions:
      - "Python (Microsoft)"
      - "Pylance"
      - "Python Docstring Generator"
      - "Python Test Explorer"
      - "Docker"
      - "Remote - Containers"
      - "GitLens"
      - "Thunder Client"
      - "YAML"
      - "JSON Tools"
    
    Settings:
      - "Python interpreter: ./venv/bin/python"
      - "Format on save: enabled"
      - "Linting: enabled"
      - "Auto-save: enabled"
      - "Tab size: 4 spaces"
    
    Workspace Configuration:
      - "Multi-root workspace"
      - "Shared settings"
      - "Task configurations"
      - "Launch configurations"
```

### 4.2 Development Tools
```yaml
Development Tools:
  Code Quality:
    - "Black: Code formatting"
    - "Flake8: Linting"
    - "MyPy: Type checking"
    - "Pylint: Code analysis"
    - "Bandit: Security scanning"
  
  Testing Tools:
    - "Pytest: Unit testing"
    - "Pytest-cov: Coverage"
    - "Pytest-asyncio: Async testing"
    - "Pytest-mock: Mocking"
    - "Factory Boy: Test data"
  
  API Testing:
    - "Thunder Client: API testing"
    - "Postman: API collection"
    - "Newman: CLI testing"
    - "Insomnia: API client"
  
  Database Tools:
    - "pgAdmin: PostgreSQL admin"
    - "Redis Commander: Redis admin"
    - "DBeaver: Universal DB client"
    - "TablePlus: Database GUI"
  
  Monitoring Tools:
    - "Grafana: Metrics visualization"
    - "Prometheus: Metrics collection"
    - "Jaeger: Distributed tracing"
    - "ELK Stack: Logging"
```

### 4.3 Git Configuration
```yaml
Git Configuration:
  Repository Setup:
    - "Main branch: main"
    - "Development branch: develop"
    - "Feature branches: feature/*"
    - "Hotfix branches: hotfix/*"
    - "Release branches: release/*"
  
  Git Hooks:
    - "Pre-commit: Code formatting"
    - "Pre-commit: Linting"
    - "Pre-commit: Type checking"
    - "Pre-commit: Security scanning"
    - "Pre-push: Tests"
  
  Git Workflow:
    - "Feature branch workflow"
    - "Pull request reviews"
    - "Squash and merge"
    - "Conventional commits"
    - "Semantic versioning"
```

---

## 5. ENVIRONMENT VARIABLES

### 5.1 Environment Configuration
```yaml
Environment Variables:
  Development (.env.development):
    # Database Configuration
    DATABASE_URL: "postgresql://oht50_user:oht50_pass@localhost:5432/oht50"
    DATABASE_POOL_SIZE: "10"
    DATABASE_MAX_OVERFLOW: "20"
    
    # Redis Configuration
    REDIS_URL: "redis://localhost:6379"
    REDIS_POOL_SIZE: "10"
    
    # API Configuration
    API_HOST: "0.0.0.0"
    API_PORT: "8000"
    API_WORKERS: "4"
    API_RELOAD: "true"
    
    # Security Configuration
    SECRET_KEY: "dev-secret-key-change-in-production"
    JWT_SECRET: "dev-jwt-secret-change-in-production"
    JWT_ALGORITHM: "HS256"
    JWT_EXPIRATION: "3600"
    
    # Logging Configuration
    LOG_LEVEL: "DEBUG"
    LOG_FORMAT: "json"
    LOG_FILE: "/var/log/oht50-api.log"
    
    # Monitoring Configuration
    PROMETHEUS_ENABLED: "true"
    METRICS_PORT: "9090"
    
    # Hardware Integration
    HARDWARE_SIMULATION: "true"
    RS485_PORT: "/dev/ttyUSB0"
    RS485_BAUDRATE: "115200"
    
    # LiDAR Configuration
    LIDAR_SIMULATION: "true"
    LIDAR_PORT: "/dev/ttyUSB1"
    LIDAR_BAUDRATE: "115200"
    
    # Firmware Integration
    FIRMWARE_SIMULATION: "true"
    FIRMWARE_HOST: "localhost"
    FIRMWARE_PORT: "502"
    
    # Frontend Integration
    FRONTEND_URL: "http://localhost:3000"
    CORS_ORIGINS: "http://localhost:3000,http://localhost:8080"
    
    # External Services
    EMAIL_SERVICE: "smtp://localhost:1025"
    SMS_SERVICE: "http://localhost:8082"
    VOICE_SERVICE: "http://localhost:8083"
```

### 5.2 Environment Management
```yaml
Environment Management:
  Configuration Files:
    - ".env.development: Development environment"
    - ".env.staging: Staging environment"
    - ".env.production: Production environment"
    - ".env.local: Local overrides"
    - ".env.example: Template file"
  
  Security:
    - "Never commit .env files"
    - "Use .env.example as template"
    - "Rotate secrets regularly"
    - "Use environment-specific keys"
    - "Encrypt sensitive data"
  
  Validation:
    - "Environment variable validation"
    - "Required fields checking"
    - "Type validation"
    - "Format validation"
    - "Dependency checking"
```

---

## 6. DATABASE SETUP

### 6.1 Database Initialization
```yaml
Database Setup:
  Initialization Scripts:
    - "01_create_database.sql: Database creation"
    - "02_create_schemas.sql: Schema creation"
    - "03_create_tables.sql: Table creation"
    - "04_create_indexes.sql: Index creation"
    - "05_create_functions.sql: Function creation"
    - "06_create_triggers.sql: Trigger creation"
    - "07_insert_seed_data.sql: Seed data"
    - "08_create_views.sql: View creation"
  
  Migration System:
    - "Alembic: Database migrations"
    - "Version control: Migration files"
    - "Rollback support: Downgrade scripts"
    - "Data migration: Data transformation"
    - "Schema validation: Migration validation"
  
  Backup Strategy:
    - "Daily backups: Automated backups"
    - "Point-in-time recovery: WAL archiving"
    - "Backup verification: Restore testing"
    - "Backup retention: 30 days"
    - "Offsite storage: Cloud backup"
```

### 6.2 Database Configuration
```yaml
Database Configuration:
  PostgreSQL Settings:
    max_connections: "100"
    shared_buffers: "256MB"
    effective_cache_size: "1GB"
    maintenance_work_mem: "64MB"
    checkpoint_completion_target: "0.9"
    wal_buffers: "16MB"
    default_statistics_target: "100"
    random_page_cost: "1.1"
    effective_io_concurrency: "200"
    work_mem: "4MB"
    min_wal_size: "1GB"
    max_wal_size: "4GB"
    max_worker_processes: "8"
    max_parallel_workers_per_gather: "4"
    max_parallel_workers: "8"
    max_parallel_maintenance_workers: "4"
  
  Connection Pooling:
    pool_size: "10"
    max_overflow: "20"
    pool_timeout: "30"
    pool_recycle: "3600"
    pool_pre_ping: "true"
  
  Performance Tuning:
    query_timeout: "30"
    statement_timeout: "60"
    idle_in_transaction_session_timeout: "300"
    lock_timeout: "10"
    deadlock_timeout: "1000"
```

---

## 7. DEVELOPMENT WORKFLOW

### 7.1 Local Development Workflow
```yaml
Development Workflow:
  Setup Process:
    1. "Clone repository"
    2. "Copy .env.example to .env.local"
    3. "Configure environment variables"
    4. "Run docker-compose up -d"
    5. "Wait for services to start"
    6. "Run database migrations"
    7. "Seed initial data"
    8. "Start development server"
  
  Daily Workflow:
    1. "Pull latest changes"
    2. "Start development environment"
    3. "Run tests"
    4. "Make changes"
    5. "Run tests again"
    6. "Commit changes"
    7. "Push to feature branch"
  
  Testing Workflow:
    1. "Run unit tests"
    2. "Run integration tests"
    3. "Run API tests"
    4. "Check code coverage"
    5. "Run security scans"
    6. "Validate documentation"
  
  Deployment Workflow:
    1. "Create feature branch"
    2. "Implement feature"
    3. "Write tests"
    4. "Update documentation"
    5. "Create pull request"
    6. "Code review"
    7. "Merge to develop"
    8. "Deploy to staging"
    9. "Testing in staging"
    10. "Merge to main"
    11. "Deploy to production"
```

### 7.2 Code Quality Workflow
```yaml
Code Quality Workflow:
  Pre-commit Hooks:
    - "Code formatting (Black)"
    - "Linting (Flake8)"
    - "Type checking (MyPy)"
    - "Security scanning (Bandit)"
    - "Import sorting (isort)"
  
  Pre-push Hooks:
    - "Run all tests"
    - "Check code coverage"
    - "Validate migrations"
    - "Check documentation"
  
  CI/CD Pipeline:
    - "Build Docker images"
    - "Run unit tests"
    - "Run integration tests"
    - "Run security scans"
    - "Deploy to staging"
    - "Run end-to-end tests"
    - "Deploy to production"
```

---

## 8. MONITORING & DEBUGGING

### 8.1 Development Monitoring
```yaml
Development Monitoring:
  Application Monitoring:
    - "FastAPI built-in monitoring"
    - "Custom metrics collection"
    - "Performance profiling"
    - "Memory usage tracking"
    - "Database query monitoring"
  
  Infrastructure Monitoring:
    - "Docker container monitoring"
    - "System resource usage"
    - "Network connectivity"
    - "Disk space monitoring"
    - "Service health checks"
  
  Logging:
    - "Structured logging (JSON)"
    - "Log levels (DEBUG, INFO, WARNING, ERROR)"
    - "Log rotation"
    - "Log aggregation"
    - "Log search và analysis"
  
  Debugging Tools:
    - "Python debugger (pdb)"
    - "Remote debugging"
    - "Profiling tools"
    - "Memory profiling"
    - "Database query analysis"
```

### 8.2 Performance Monitoring
```yaml
Performance Monitoring:
  Metrics Collection:
    - "Request/response times"
    - "Throughput (requests/second)"
    - "Error rates"
    - "Database query performance"
    - "Memory usage"
    - "CPU usage"
    - "Network I/O"
    - "Disk I/O"
  
  Alerting:
    - "High error rates"
    - "Slow response times"
    - "High resource usage"
    - "Service unavailability"
    - "Database connection issues"
  
  Visualization:
    - "Grafana dashboards"
    - "Real-time metrics"
    - "Historical trends"
    - "Performance comparisons"
    - "Capacity planning"
```

---

## 9. SECURITY CONSIDERATIONS

### 9.1 Development Security
```yaml
Development Security:
  Code Security:
    - "Dependency vulnerability scanning"
    - "Code security analysis"
    - "Secret scanning"
    - "License compliance"
    - "Security best practices"
  
  Environment Security:
    - "Secure environment variables"
    - "Network isolation"
    - "Access control"
    - "Audit logging"
    - "Security monitoring"
  
  Data Security:
    - "Data encryption at rest"
    - "Data encryption in transit"
    - "Access control"
    - "Data backup security"
    - "Data retention policies"
  
  Infrastructure Security:
    - "Container security"
    - "Network security"
    - "Host security"
    - "Security updates"
    - "Security monitoring"
```

---

## 10. TROUBLESHOOTING

### 10.1 Common Issues
```yaml
Common Issues:
  Docker Issues:
    - "Container startup failures"
    - "Port conflicts"
    - "Volume mounting issues"
    - "Network connectivity"
    - "Resource constraints"
  
  Database Issues:
    - "Connection failures"
    - "Migration errors"
    - "Performance issues"
    - "Data corruption"
    - "Backup failures"
  
  API Issues:
    - "Service startup failures"
    - "Authentication issues"
    - "Performance problems"
    - "Integration errors"
    - "Configuration issues"
  
  Development Issues:
    - "Environment setup problems"
    - "Dependency conflicts"
    - "Build failures"
    - "Test failures"
    - "Deployment issues"
```

### 10.2 Troubleshooting Procedures
```yaml
Troubleshooting Procedures:
  Diagnostic Steps:
    1. "Check service status"
    2. "Review logs"
    3. "Check configuration"
    4. "Verify dependencies"
    5. "Test connectivity"
    6. "Check resources"
    7. "Validate data"
    8. "Review recent changes"
  
  Resolution Steps:
    1. "Identify root cause"
    2. "Apply fix"
    3. "Test solution"
    4. "Verify resolution"
    5. "Update documentation"
    6. "Prevent recurrence"
  
  Escalation:
    1. "Document issue"
    2. "Gather information"
    3. "Contact support"
    4. "Provide context"
    5. "Follow up"
```

---

## 11. REFERENCES

### 11.1 Related Documents
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
- REQ_BE_13: Testing Requirements
- REQ_BE_14: Deployment Requirements
- REQ_BE_15: API Documentation
- REQ_BE_16: Database Migration
- REQ_BE_17: Security Implementation
- REQ_BE_18: WebSocket Implementation

### 11.2 Standards References
- ISO/IEC/IEEE 29148:2018 - Requirements Engineering
- ISO/IEC 25010:2011 - Software Quality Requirements
- Docker Documentation - Container Platform
- PostgreSQL Documentation - Database System
- Redis Documentation - In-Memory Data Store

---

## 12. APPROVAL

| Role | Name | Date | Signature |
|------|------|------|-----------|
| CTO | [Name] | [Date] | [Signature] |
| Technical Lead | [Name] | [Date] | [Signature] |
| DevOps Engineer | [Name] | [Date] | [Signature] |

---

**Document End**
