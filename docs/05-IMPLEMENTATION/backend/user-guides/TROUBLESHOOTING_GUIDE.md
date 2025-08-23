# Troubleshooting Guide - OHT-50 Backend

## 📋 Tổng quan
Hướng dẫn xử lý sự cố cho OHT-50 Backend, bao gồm các lỗi thường gặp, cách chẩn đoán và giải pháp.

## 🔍 Diagnostic Tools

### 1. Health Check Endpoints
```bash
# Basic health check
curl http://localhost:8000/health

# Detailed health check
curl http://localhost:8000/health/detailed

# Component-specific health checks
curl http://localhost:8000/health/database
curl http://localhost:8000/health/redis
curl http://localhost:8000/health/external-services
```

### 2. Log Analysis
```bash
# View application logs
tail -f logs/app.log

# View error logs
grep "ERROR" logs/app.log

# View specific component logs
grep "database" logs/app.log
grep "authentication" logs/app.log

# Search for specific error patterns
grep -i "timeout" logs/app.log
grep -i "connection refused" logs/app.log
```

### 3. Metrics & Monitoring
```bash
# View Prometheus metrics
curl http://localhost:8000/metrics

# Check specific metrics
curl http://localhost:8000/metrics | grep "http_requests_total"
curl http://localhost:8000/metrics | grep "database_connections"

# Grafana dashboard
# Access: http://localhost:3000
# Default credentials: admin/admin
```

## 🚨 Common Issues & Solutions

### 1. Database Connection Issues

#### Problem: Database Connection Failed
```bash
# Error message
ERROR: database connection failed: connection refused
```

#### Diagnosis:
```bash
# Check if database is running
sudo systemctl status postgresql

# Check database port
netstat -tlnp | grep 5432

# Test database connection
psql -h localhost -U oht50_user -d oht50_db
```

#### Solutions:
```bash
# Start database service
sudo systemctl start postgresql

# Check database configuration
cat /etc/postgresql/*/main/postgresql.conf | grep port

# Verify connection string
echo $DATABASE_URL

# Reset database connection pool
curl -X POST http://localhost:8000/admin/reset-connections
```

### 2. Authentication Issues

#### Problem: Invalid Token
```bash
# Error message
401 Unauthorized: Invalid authentication credentials
```

#### Diagnosis:
```bash
# Check token format
echo $AUTH_TOKEN | cut -d'.' -f2 | base64 -d

# Verify token expiration
jwt decode $AUTH_TOKEN

# Check authentication logs
grep "authentication" logs/app.log | tail -10
```

#### Solutions:
```bash
# Generate new token
curl -X POST http://localhost:8000/auth/login \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "password"}'

# Refresh token
curl -X POST http://localhost:8000/auth/refresh \
  -H "Authorization: Bearer $REFRESH_TOKEN"

# Clear token cache
curl -X POST http://localhost:8000/admin/clear-token-cache
```

### 3. API Performance Issues

#### Problem: Slow API Response
```bash
# Error message
Request timeout after 30 seconds
```

#### Diagnosis:
```bash
# Check response times
curl -w "@curl-format.txt" -o /dev/null -s http://localhost:8000/api/v1/missions

# Monitor system resources
htop
iostat -x 1
free -h

# Check database performance
SELECT query, mean_time, calls FROM pg_stat_statements ORDER BY mean_time DESC LIMIT 10;
```

#### Solutions:
```bash
# Optimize database queries
# Add database indexes
CREATE INDEX idx_missions_status ON missions(status);
CREATE INDEX idx_missions_created_at ON missions(created_at);

# Enable query caching
curl -X POST http://localhost:8000/admin/enable-cache

# Scale application
docker-compose scale app=3
```

### 4. Memory Issues

#### Problem: Out of Memory
```bash
# Error message
MemoryError: Unable to allocate memory
```

#### Diagnosis:
```bash
# Check memory usage
free -h
ps aux --sort=-%mem | head -10

# Check memory leaks
python -m memory_profiler app/main.py

# Monitor garbage collection
python -X dev -c "import gc; gc.set_debug(gc.DEBUG_STATS)"
```

#### Solutions:
```bash
# Increase memory limits
export PYTHONMALLOC=malloc
export PYTHONDEVMODE=1

# Optimize memory usage
# Reduce batch sizes
# Implement pagination
# Use generators for large datasets

# Restart application
docker-compose restart app
```

### 5. Network Issues

#### Problem: Connection Timeout
```bash
# Error message
Connection timeout: Unable to connect to external service
```

#### Diagnosis:
```bash
# Test network connectivity
ping external-service.com
telnet external-service.com 443

# Check DNS resolution
nslookup external-service.com
dig external-service.com

# Monitor network traffic
tcpdump -i any -w capture.pcap
```

#### Solutions:
```bash
# Update DNS settings
echo "nameserver 8.8.8.8" >> /etc/resolv.conf

# Configure proxy if needed
export HTTP_PROXY=http://proxy:8080
export HTTPS_PROXY=http://proxy:8080

# Implement retry logic
# Add circuit breaker pattern
# Use connection pooling
```

## 🔧 Advanced Troubleshooting

### 1. Performance Profiling
```python
# CPU profiling
import cProfile
import pstats

def profile_function():
    profiler = cProfile.Profile()
    profiler.enable()
    # Your code here
    profiler.disable()
    stats = pstats.Stats(profiler)
    stats.sort_stats('cumulative')
    stats.print_stats(10)

# Memory profiling
from memory_profiler import profile

@profile
def memory_intensive_function():
    # Your code here
    pass

# Line-by-line profiling
python -m line_profiler your_script.py
```

### 2. Debug Mode
```python
# Enable debug logging
import logging
logging.basicConfig(level=logging.DEBUG)

# Enable SQL query logging
import logging
logging.getLogger('sqlalchemy.engine').setLevel(logging.INFO)

# Enable async debug
import asyncio
asyncio.get_event_loop().set_debug(True)
```

### 3. Database Debugging
```sql
-- Check slow queries
SELECT query, mean_time, calls 
FROM pg_stat_statements 
ORDER BY mean_time DESC 
LIMIT 10;

-- Check table sizes
SELECT 
    schemaname,
    tablename,
    attname,
    n_distinct,
    correlation
FROM pg_stats 
WHERE tablename = 'missions';

-- Check index usage
SELECT 
    schemaname,
    tablename,
    indexname,
    idx_scan,
    idx_tup_read,
    idx_tup_fetch
FROM pg_stat_user_indexes 
ORDER BY idx_scan DESC;
```

## 📊 Monitoring & Alerting

### 1. Key Metrics to Monitor
```yaml
# System metrics
- cpu_usage_percent > 80%
- memory_usage_percent > 90%
- disk_usage_percent > 85%
- network_errors > 0

# Application metrics
- http_requests_total (rate)
- http_request_duration_seconds (p95 > 1s)
- database_connections_active > 80%
- error_rate > 5%

# Business metrics
- mission_success_rate < 95%
- safety_events_total (rate)
- telemetry_latency_seconds > 0.5
```

### 2. Alert Configuration
```yaml
# Prometheus alert rules
groups:
  - name: oht50_backend_alerts
    rules:
      - alert: HighErrorRate
        expr: rate(http_requests_total{status=~"5.."}[5m]) > 0.1
        for: 2m
        labels:
          severity: warning
        annotations:
          summary: "High error rate detected"
          description: "Error rate is {{ $value }} errors per second"

      - alert: HighResponseTime
        expr: histogram_quantile(0.95, rate(http_request_duration_seconds_bucket[5m])) > 1
        for: 5m
        labels:
          severity: warning
        annotations:
          summary: "High response time detected"
          description: "95th percentile response time is {{ $value }} seconds"
```

## 🛠️ Recovery Procedures

### 1. Service Recovery
```bash
# Restart specific service
docker-compose restart app

# Restart all services
docker-compose down && docker-compose up -d

# Rollback to previous version
docker-compose down
docker tag oht50_backend:previous oht50_backend:latest
docker-compose up -d
```

### 2. Database Recovery
```bash
# Backup database
pg_dump -h localhost -U oht50_user oht50_db > backup.sql

# Restore database
psql -h localhost -U oht50_user oht50_db < backup.sql

# Reset database (development only)
python scripts/reset_database.py
```

### 3. Configuration Recovery
```bash
# Restore configuration
cp config/backup/config.yaml config/config.yaml

# Reload configuration
curl -X POST http://localhost:8000/admin/reload-config

# Reset to defaults
python scripts/reset_config.py
```

## 📋 Troubleshooting Checklist

### Initial Assessment
- [ ] Check application status: `curl http://localhost:8000/health`
- [ ] Review recent logs: `tail -100 logs/app.log`
- [ ] Check system resources: `htop`, `free -h`, `df -h`
- [ ] Verify network connectivity: `ping google.com`
- [ ] Check database status: `sudo systemctl status postgresql`

### Common Issues
- [ ] Database connection issues
- [ ] Authentication problems
- [ ] API performance issues
- [ ] Memory leaks
- [ ] Network timeouts
- [ ] Configuration errors

### Resolution Steps
- [ ] Identify root cause
- [ ] Apply immediate fix
- [ ] Test solution
- [ ] Monitor for recurrence
- [ ] Document incident
- [ ] Update procedures

## 📞 Emergency Contacts

### Technical Support
- **Backend Team**: backend-team@oht50.local
- **DevOps Team**: devops@oht50.local
- **Database Admin**: dba@oht50.local

### Escalation Path
1. **Level 1**: On-call developer
2. **Level 2**: Backend team lead
3. **Level 3**: CTO
4. **Level 4**: Executive team

### Communication Channels
- **Slack**: #oht50-incidents
- **Email**: incidents@oht50.local
- **Phone**: +84-xxx-xxx-xxxx

---

**Changelog v1.0:**
- ✅ Created comprehensive troubleshooting guide
- ✅ Added diagnostic tools and commands
- ✅ Included common issues and solutions
- ✅ Added advanced troubleshooting techniques
- ✅ Created monitoring and alerting guidelines
- ✅ Added recovery procedures
- ✅ Included troubleshooting checklist
- ✅ Added emergency contacts and escalation path
