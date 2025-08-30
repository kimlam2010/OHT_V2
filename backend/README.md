# OHT-50 Backend

Backend API server for OHT-50 robot control system.

## 🚀 Quick Start

### Prerequisites
- Python 3.11+
- Redis
- SQLite (or PostgreSQL for production)

### Installation

1. **Clone and setup:**
```bash
cd backend
python -m venv venv
source venv/bin/activate  # On Windows: venv\Scripts\activate
pip install -r requirements.txt
```

2. **Environment setup:**
```bash
cp .env.example .env
# Edit .env with your configuration
```

3. **Run development server:**
```bash
uvicorn app.main:app --reload --host 0.0.0.0 --port 8000
```

### Docker Deployment

```bash
# Build and run with Docker Compose
docker-compose -f docker/docker-compose.yml up --build
```

## 📁 Project Structure

```
backend/
├── app/
│   ├── api/           # API endpoints
│   ├── core/          # Core functionality (security, database, monitoring)
│   ├── models/        # Database models
│   ├── schemas/       # Pydantic schemas
│   ├── services/      # Business logic services
│   ├── config.py      # Configuration
│   └── main.py        # Application entry point
├── tests/             # Test suite
├── docker/            # Docker configuration
├── docs/              # Documentation
└── scripts/           # Utility scripts
```

## 🔧 Configuration

Key environment variables:

- `DATABASE_URL`: Database connection string
- `REDIS_URL`: Redis connection string
- `JWT_SECRET`: Secret key for JWT tokens
- `RS485_PORT`: RS485 device port
- `DEBUG`: Enable debug mode

## 🧪 Testing

```bash
# Run all tests
pytest

# Run with coverage
pytest --cov=app

# Run specific test category
pytest tests/unit/
pytest tests/integration/
pytest tests/performance/
```

## 📊 Monitoring

- **Health Check:** `GET /health`
- **Metrics:** `GET /metrics` (Prometheus format)
- **API Docs:** `GET /docs` (Swagger UI)

## 🔒 Security

- JWT-based authentication
- Rate limiting
- Input validation
- CORS protection
- Security headers

## 🚀 Performance Targets

- API Response Time: < 50ms
- WebSocket Latency: < 20ms
- Database Queries: < 10ms
- System Uptime: > 99.9%

## 📚 API Documentation

Visit `http://localhost:8000/docs` for interactive API documentation.

## 🤝 Contributing

1. Follow the coding standards (Black, isort, flake8)
2. Write tests for new features
3. Update documentation
4. Ensure performance requirements are met

## 📄 License

OHT-50 Backend - MIT License
