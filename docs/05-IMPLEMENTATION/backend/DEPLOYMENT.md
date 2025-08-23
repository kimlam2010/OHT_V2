# Backend Deployment Guide - OHT-50

## Overview
Hướng dẫn triển khai OHT-50 Backend service với FastAPI, Pydantic và structured logging.

## Prerequisites
- Python 3.11+
- Docker (optional)
- Systemd (for service management)

## Quick Start

### 1. Local Development
```bash
cd backend
pip install -r requirements.txt
uvicorn app.main:app --reload --host 0.0.0.0 --port 8000
```

### 2. Docker Deployment
```bash
docker build -t oht50-backend .
docker run -p 8000:8000 oht50-backend
```

### 3. Systemd Service
```bash
sudo cp deployment/oht50-backend.service /etc/systemd/system/
sudo systemctl enable oht50-backend
sudo systemctl start oht50-backend
```

## Configuration

### Environment Variables
```bash
# Required
SECRET_KEY=your-secret-key-here
DEBUG=false

# Optional
LOG_LEVEL=INFO
CORS_ORIGINS=http://localhost:3000,https://yourdomain.com
```

### Configuration Files
- `config/production.yaml` - Production settings
- `config/development.yaml` - Development settings

## API Documentation
- **OpenAPI UI**: `http://localhost:8000/docs`
- **ReDoc**: `http://localhost:8000/redoc`
- **Health Check**: `http://localhost:8000/health/`

## Monitoring

### Health Endpoints
- `GET /health/` - Basic health check
- `GET /health/detailed` - Detailed system status

### Logging
- Structured JSON logging
- Log rotation via systemd
- Centralized logging (ELK stack recommended)

## Security

### Authentication
- Bearer token authentication
- DEBUG mode bypass (development only)
- CORS configuration

### Network Security
- HTTPS termination at reverse proxy
- Rate limiting via Nginx
- Security headers

## Performance

### Optimization
- Async/await patterns
- Connection pooling
- Response caching
- Background tasks

### Scaling
- Horizontal scaling with load balancer
- Database connection pooling
- Redis for caching (optional)

## Troubleshooting

### Common Issues
1. **Port already in use**: Check `sudo netstat -tlnp | grep 8000`
2. **Permission denied**: Check file permissions and user
3. **Import errors**: Verify Python environment and dependencies

### Debug Mode
```bash
export DEBUG=true
uvicorn app.main:app --reload --log-level debug
```

## Support

### Documentation Structure
- **API Specifications**: `api-specs/` - CONFIG_API.md, TELEMETRY_API.md, WEBSOCKET_EVENTS.md, INTEGRATION_CENTER.md
- **Operational**: `operational/` - SECURITY_GUIDE.md, HARDENING_CHECKLIST.md, RUNBOOK.md, SLO_SLA.md, ERROR_MATRIX.md
- **Development Guides**: `guides/` - OPENAPI_EXAMPLES_GUIDE.md, OPENAPI_REVIEW.md, CI_DOCS_CHECK_GUIDE.md
- **Testing Tools**: `tools/` - postman_collection.json, INSOMNIA_WORKSPACE.json, curl_suite.sh
- **Core Docs**: DEPLOYMENT.md, RELEASE_NOTES.md, ERRORS_CONVENTION.md, BE-02_COMPLETION_SUMMARY.md

### Quick References
- **Testing**: Import Postman collection hoặc Insomnia workspace từ `tools/`
- **API Reference**: Xem `api-specs/` cho endpoint details
- **Security**: Review `operational/SECURITY_GUIDE.md` và `operational/HARDENING_CHECKLIST.md`
- **CI/CD**: Follow `guides/CI_DOCS_CHECK_GUIDE.md` cho automated validation
