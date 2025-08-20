# Backend Service OHT-50
## Run locally (Windows PowerShell)

1. Navigate to backend and create venv
```
cd "/home/orangepi/OHT-50/backend"
python -m venv .venv
.\.venv\Scripts\activate
pip install -r requirements.txt
```

2. Local dev env vars
```
$env:DEBUG = "true"            # skip token on protected routes
$env:USE_RS485_MOCK = "true"   # safe RS485 fallback
```

3. Run server (port 8000)
```
python -m uvicorn app.main:app --host 0.0.0.0 --port 8000 --reload
```

4. Quick checks
```
Invoke-WebRequest -Uri http://127.0.0.1:8000/health | Select -Expand Content
Invoke-WebRequest -Uri http://127.0.0.1:8000/api/v1/telemetry/current | Select -Expand Content
```

5. Auth (when DEBUG=false)
```
POST /api/v1/auth/login {"username":"admin","password":"admin"} -> token
Add header: Authorization: Bearer <token>
```


## Tổng quan
Backend service cho hệ thống điều khiển OHT-50, cung cấp API cho cấu hình, telemetry, logging và giao tiếp với Center.

## Cấu trúc dự án
```
backend/
├── app/
│   ├── __init__.py
│   ├── main.py              # FastAPI app entry point
│   ├── config.py            # Cấu hình môi trường
│   ├── models/              # Pydantic models
│   │   ├── __init__.py
│   │   ├── telemetry.py     # Telemetry schema
│   │   ├── config.py        # Config schema
│   │   └── center_comm.py   # Center communication
│   ├── api/                 # API routes
│   │   ├── __init__.py
│   │   ├── v1/
│   │   │   ├── __init__.py
│   │   │   ├── config.py    # Config API
│   │   │   ├── telemetry.py # Telemetry API
│   │   │   ├── center.py    # Center communication
│   │   │   └── health.py    # Health check
│   ├── services/            # Business logic
│   │   ├── __init__.py
│   │   ├── config_service.py
│   │   ├── telemetry_service.py
│   │   └── center_service.py
│   ├── core/                # Core utilities
│   │   ├── __init__.py
│   │   ├── logging.py       # Structured logging
│   │   ├── security.py      # Security utilities
│   │   └── exceptions.py    # Custom exceptions
│   └── utils/               # Utility functions
│       ├── __init__.py
│       └── helpers.py
├── tests/                   # Unit tests
│   ├── __init__.py
│   ├── test_config.py
│   ├── test_telemetry.py
│   └── test_center.py
├── requirements.txt         # Python dependencies
├── Dockerfile              # Container build
├── docker-compose.yml      # Development environment
└── README.md              # This file
```

## Cài đặt và chạy

### Yêu cầu
- Python 3.9+
- pip

### Cài đặt dependencies
```bash
cd backend
pip install -r requirements.txt
```

### Chạy development server
```bash
# Chạy với auto-reload
uvicorn app.main:app --reload --host 0.0.0.0 --port 8000

# Hoặc với Python
python -m uvicorn app.main:app --reload --host 0.0.0.0 --port 8000
```

### Chạy với Docker
```bash
# Build image
docker build -t oht50-backend .

# Chạy container
docker run -p 8000:8000 oht50-backend

# Hoặc với docker-compose
docker-compose up
```

## API Endpoints

### Health Check
- `GET /health` - Kiểm tra trạng thái service
- `GET /health/detailed` - Thông tin chi tiết

### Configuration
- `GET /api/v1/config` - Lấy cấu hình hiện tại
- `POST /api/v1/config` - Cập nhật cấu hình
- `GET /api/v1/config/history` - Lịch sử cấu hình
- `POST /api/v1/config/rollback/{version}` - Rollback cấu hình

### Telemetry
- `GET /api/v1/telemetry/current` - Trạng thái hiện tại
- `GET /api/v1/telemetry/history` - Lịch sử telemetry
- `WebSocket /ws/telemetry` - Telemetry realtime

### Center Communication
- `POST /api/v1/center/mission` - Nhận mission từ Center
- `POST /api/v1/center/permission` - Yêu cầu permission
- `WebSocket /ws/center` - Giao tiếp realtime với Center

## Cấu hình môi trường

Tạo file `.env` trong thư mục `backend/`:

```env
# Server
HOST=0.0.0.0
PORT=8000
DEBUG=true

# Database (nếu cần)
DATABASE_URL=sqlite:///./oht50.db

# Logging
LOG_LEVEL=INFO
LOG_FORMAT=json

# Security
SECRET_KEY=your-secret-key-here
ACCESS_TOKEN_EXPIRE_MINUTES=30

# Telemetry
TELEMETRY_WS_RATE=10  # Hz
TELEMETRY_HISTORY_SIZE=1000

# Center Communication
CENTER_WS_URL=ws://localhost:8080/ws
CENTER_HEARTBEAT_INTERVAL=2  # seconds
```

## Testing

```bash
# Chạy tất cả tests
pytest

# Chạy với coverage
pytest --cov=app --cov-report=html

# Chạy specific test
pytest tests/test_config.py
```

## Logging

Service sử dụng structured logging với correlation ID:

```python
import logging
from app.core.logging import get_logger

logger = get_logger(__name__)
logger.info("Service started", extra={"correlation_id": "abc123"})
```

## Monitoring

- Health check endpoint: `/health`
- Metrics endpoint: `/metrics` (nếu tích hợp Prometheus)
- API documentation: `/docs` (Swagger UI)

## Development

### Code Style
- Sử dụng Black cho formatting
- Sử dụng isort cho import sorting
- Sử dụng flake8 cho linting

### Pre-commit hooks
```bash
# Cài đặt pre-commit
pip install pre-commit
pre-commit install
```

## Deployment

### Production
```bash
# Build production image
docker build -t oht50-backend:prod .

# Chạy với environment variables
docker run -d \
  -p 8000:8000 \
  -e DEBUG=false \
  -e LOG_LEVEL=INFO \
  oht50-backend:prod
```

### Kubernetes (nếu cần)
```yaml
apiVersion: apps/v1
kind: Deployment
metadata:
  name: oht50-backend
spec:
  replicas: 2
  selector:
    matchLabels:
      app: oht50-backend
  template:
    metadata:
      labels:
        app: oht50-backend
    spec:
      containers:
      - name: backend
        image: oht50-backend:prod
        ports:
        - containerPort: 8000
        env:
        - name: DEBUG
          value: "false"
```

## Troubleshooting

### Common Issues

1. **Port already in use**
   ```bash
   # Tìm process sử dụng port 8000
   lsof -i :8000
   # Kill process
   kill -9 <PID>
   ```

2. **Import errors**
   ```bash
   # Cài đặt lại dependencies
   pip install -r requirements.txt --force-reinstall
   ```

3. **Database connection issues**
   - Kiểm tra DATABASE_URL trong .env
   - Đảm bảo database server đang chạy

### Logs
```bash
# Xem logs realtime
docker logs -f <container_id>

# Hoặc với docker-compose
docker-compose logs -f backend
```
