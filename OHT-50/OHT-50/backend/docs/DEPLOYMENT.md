# Hướng dẫn Triển khai OHT-50 Backend

## Tổng quan

Tài liệu này hướng dẫn cách triển khai OHT-50 Backend service trong các môi trường khác nhau.

## Môi trường

### Development
- **URL**: http://localhost:8000
- **Database**: SQLite (local)
- **Log Level**: DEBUG
- **Auto-reload**: Enabled

### Staging
- **URL**: https://staging-backend.oht-50.com
- **Database**: PostgreSQL
- **Log Level**: INFO
- **Auto-reload**: Disabled

### Production
- **URL**: https://backend.oht-50.com
- **Database**: PostgreSQL (clustered)
- **Log Level**: WARNING
- **Auto-reload**: Disabled

## Yêu cầu Hệ thống

### Tối thiểu
- **CPU**: 1 core
- **RAM**: 512MB
- **Storage**: 1GB
- **OS**: Linux (Ubuntu 20.04+)

### Khuyến nghị
- **CPU**: 2 cores
- **RAM**: 2GB
- **Storage**: 10GB SSD
- **OS**: Linux (Ubuntu 22.04 LTS)

## Cài đặt

### 1. Sử dụng Docker (Khuyến nghị)

```bash
# Build image
docker build -t oht-50-backend:latest .

# Run container
docker run -d \
  --name oht-50-backend \
  -p 8000:8000 \
  -e DATABASE_URL=postgresql://user:pass@host:5432/oht50 \
  -e API_KEY=your-api-key \
  oht-50-backend:latest
```

### 2. Cài đặt trực tiếp

```bash
# Clone repository
git clone https://github.com/your-org/oht-50.git
cd oht-50/backend

# Tạo virtual environment
python -m venv venv
source venv/bin/activate  # Linux/Mac
# hoặc
venv\Scripts\activate  # Windows

# Cài đặt dependencies
pip install -r requirements.txt

# Chạy application
uvicorn app.main:app --host 0.0.0.0 --port 8000
```

## Cấu hình

### Environment Variables

Tạo file `.env` hoặc set environment variables:

```bash
# Server
SERVER_HOST=0.0.0.0
SERVER_PORT=8000
DEBUG=false
LOG_LEVEL=INFO

# Database
DATABASE_URL=postgresql://user:password@localhost:5432/oht50

# Security
API_KEY=your-secure-api-key
CORS_ORIGINS=["https://frontend.oht-50.com"]

# Telemetry
TELEMETRY_BUFFER_SIZE=1000
TELEMETRY_FLUSH_INTERVAL=5

# Center Communication
CENTER_URL=https://center.oht-50.com
CENTER_API_KEY=your-center-api-key

# Motor Configuration
MOTOR_MAX_SPEED=100.0
MOTOR_ACCELERATION=10.0

# Safety Zones
SAFETY_ZONE_X_MIN=-1000.0
SAFETY_ZONE_X_MAX=1000.0
SAFETY_ZONE_Y_MIN=-1000.0
SAFETY_ZONE_Y_MAX=1000.0
```

### Docker Compose

```yaml
version: '3.8'

services:
  backend:
    build: .
    ports:
      - "8000:8000"
    environment:
      - DATABASE_URL=postgresql://postgres:password@db:5432/oht50
      - API_KEY=${API_KEY}
      - DEBUG=false
    depends_on:
      - db
    restart: unless-stopped

  db:
    image: postgres:15
    environment:
      - POSTGRES_DB=oht50
      - POSTGRES_USER=postgres
      - POSTGRES_PASSWORD=password
    volumes:
      - postgres_data:/var/lib/postgresql/data
    restart: unless-stopped

volumes:
  postgres_data:
```

## CI/CD Pipeline

### GitHub Actions

Pipeline tự động chạy khi push code:

1. **Lint & Security**: Kiểm tra code style và bảo mật
2. **Test**: Chạy unit tests với coverage >80%
3. **Build**: Tạo Docker image
4. **Deploy Dev**: Tự động deploy lên staging
5. **Deploy Prod**: Manual approval cho production

### Manual Deployment

```bash
# Development
make deploy-dev

# Production
make deploy-prod
```

## Monitoring

### Health Checks

```bash
# Basic health
curl http://localhost:8000/health

# Detailed health
curl http://localhost:8000/health/detailed

# Readiness probe
curl http://localhost:8000/health/ready

# Liveness probe
curl http://localhost:8000/health/live
```

### Logs

```bash
# Docker logs
docker logs oht-50-backend

# Application logs
tail -f logs/app.log
```

### Metrics

- **CPU Usage**: `/health/detailed` endpoint
- **Memory Usage**: `/health/detailed` endpoint
- **Disk Usage**: `/health/detailed` endpoint
- **Uptime**: `/health/detailed` endpoint

## Troubleshooting

### Common Issues

1. **Port already in use**
   ```bash
   # Check port usage
   netstat -tulpn | grep :8000
   
   # Kill process
   sudo kill -9 <PID>
   ```

2. **Database connection failed**
   ```bash
   # Check database
   psql -h localhost -U postgres -d oht50
   
   # Check environment
   echo $DATABASE_URL
   ```

3. **Permission denied**
   ```bash
   # Fix permissions
   sudo chown -R $USER:$USER .
   chmod +x scripts/*.sh
   ```

### Debug Mode

```bash
# Enable debug
export DEBUG=true
export LOG_LEVEL=DEBUG

# Run with debug
uvicorn app.main:app --reload --log-level debug
```

## Backup & Recovery

### Database Backup

```bash
# PostgreSQL backup
pg_dump -h localhost -U postgres oht50 > backup.sql

# Restore
psql -h localhost -U postgres oht50 < backup.sql
```

### Configuration Backup

```bash
# Backup config
cp .env .env.backup

# Restore config
cp .env.backup .env
```

## Security

### API Key Management

```bash
# Generate secure API key
openssl rand -hex 32

# Rotate API key
export API_KEY=new-secure-key
```

### SSL/TLS

```bash
# Generate SSL certificate
openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -days 365

# Run with SSL
uvicorn app.main:app --ssl-keyfile key.pem --ssl-certfile cert.pem
```

## Performance Tuning

### Gunicorn (Production)

```bash
# Install gunicorn
pip install gunicorn

# Run with multiple workers
gunicorn app.main:app -w 4 -k uvicorn.workers.UvicornWorker --bind 0.0.0.0:8000
```

### Nginx Reverse Proxy

```nginx
server {
    listen 80;
    server_name backend.oht-50.com;

    location / {
        proxy_pass http://127.0.0.1:8000;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

## Support

- **Documentation**: https://github.com/your-org/oht-50/docs
- **Issues**: https://github.com/your-org/oht-50/issues
- **Email**: team@oht-50.com
