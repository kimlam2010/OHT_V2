# Developer Onboarding Guide - OHT-50 Backend

## 📋 Tổng quan
Hướng dẫn onboarding cho developers mới tham gia vào dự án OHT-50 Backend, bao gồm setup môi trường, cấu trúc dự án và quy trình phát triển.

## 🚀 Quick Start

### 1. Prerequisites
```bash
# System Requirements
- Python 3.9+ (recommended: 3.11)
- Git 2.30+
- Docker & Docker Compose
- Node.js 18+ (for frontend development)
- IDE: VS Code
# Python Dependencies
- pip 21.0+
- virtualenv hoặc conda
- poetry (optional, for dependency management)
```

### 2. Environment Setup
```bash
# Clone repository
git clone https://github.com/your-org/OHT-50.git
cd OHT-50

# Create virtual environment
python -m venv venv
source venv/bin/activate  # Linux/Mac
# hoặc
venv\Scripts\activate     # Windows

# Install dependencies
pip install -r requirements.txt

# Install development dependencies
pip install -r requirements-dev.txt

# Setup pre-commit hooks
pre-commit install
```

### 3. Database Setup
```bash
# Using SQLite (development)
python scripts/setup_database.py

# Using PostgreSQL (production-like)
docker-compose up -d postgres
python scripts/migrate_database.py
```

### 4. Run Development Server
```bash
# Start development server
uvicorn app.main:app --reload --host 0.0.0.0 --port 8000

# Or using make
make dev

# Check if everything is working
curl http://localhost:8000/health
```

## 🏗️ Project Structure

```
OHT-50/
├── app/                          # Main application code
│   ├── __init__.py
│   ├── main.py                   # FastAPI application entry point
│   ├── config.py                 # Configuration management
│   ├── dependencies.py           # Dependency injection
│   ├── api/                      # API routes
│   │   ├── __init__.py
│   │   ├── v1/                   # API version 1
│   │   │   ├── __init__.py
│   │   │   ├── auth.py           # Authentication endpoints
│   │   │   ├── config.py         # Configuration endpoints
│   │   │   ├── missions.py       # Mission management
│   │   │   ├── telemetry.py      # Telemetry endpoints
│   │   │   └── safety.py         # Safety endpoints
│   │   └── deps.py               # API dependencies
│   ├── core/                     # Core business logic
│   │   ├── __init__.py
│   │   ├── security.py           # Security utilities
│   │   ├── config.py             # Configuration models
│   │   └── exceptions.py         # Custom exceptions
│   ├── models/                   # Database models
│   │   ├── __init__.py
│   │   ├── user.py               # User model
│   │   ├── mission.py            # Mission model
│   │   ├── telemetry.py          # Telemetry model
│   │   └── safety.py             # Safety model
│   ├── schemas/                  # Pydantic schemas
│   │   ├── __init__.py
│   │   ├── user.py               # User schemas
│   │   ├── mission.py            # Mission schemas
│   │   ├── telemetry.py          # Telemetry schemas
│   │   └── safety.py             # Safety schemas
│   ├── services/                 # Business logic services
│   │   ├── __init__.py
│   │   ├── auth_service.py       # Authentication service
│   │   ├── config_service.py     # Configuration service
│   │   ├── mission_service.py    # Mission service
│   │   ├── telemetry_service.py  # Telemetry service
│   │   └── safety_service.py     # Safety service
│   ├── crud/                     # Database operations
│   │   ├── __init__.py
│   │   ├── base.py               # Base CRUD operations
│   │   ├── user.py               # User CRUD
│   │   ├── mission.py            # Mission CRUD
│   │   ├── telemetry.py          # Telemetry CRUD
│   │   └── safety.py             # Safety CRUD
│   └── utils/                    # Utility functions
│       ├── __init__.py
│       ├── logger.py             # Logging utilities
│       └── helpers.py            # Helper functions
├── tests/                        # Test files
│   ├── __init__.py
│   ├── conftest.py               # Test configuration
│   ├── test_api/                 # API tests
│   ├── test_services/            # Service tests
│   ├── test_models/              # Model tests
│   └── test_integration/         # Integration tests
├── docs/                         # Documentation
│   ├── backend/                  # Backend documentation
│   ├── api/                      # API documentation
│   └── deployment/               # Deployment guides
├── scripts/                      # Utility scripts
├── docker/                       # Docker configurations
├── requirements.txt              # Production dependencies
├── requirements-dev.txt          # Development dependencies
├── pyproject.toml               # Project configuration
├── Makefile                     # Development commands
└── README.md                    # Project overview
```

## 🔧 Development Workflow

### 1. Git Workflow
```bash
# Create feature branch
git checkout -b feature/your-feature-name

# Make changes and commit
git add .
git commit -m "feat: add new feature description"

# Push to remote
git push origin feature/your-feature-name

# Create Pull Request
# - Go to GitHub/GitLab
# - Create PR from your branch to main/develop
# - Add description, reviewers, labels
# - Wait for CI/CD checks to pass
# - Get approval and merge
```

### 2. Code Style & Quality
```bash
# Run linting
make lint

# Format code
make format

# Run type checking
make type-check

# Run security checks
make security-check

# Run all quality checks
make quality-check
```

### 3. Testing
```bash
# Run unit tests
make test

# Run tests with coverage
make test-coverage

# Run specific test file
pytest tests/test_api/test_missions.py

# Run tests with specific marker
pytest -m "integration"

# Run tests in parallel
pytest -n auto
```

### 4. Database Operations
```bash
# Create migration
alembic revision --autogenerate -m "Add new table"

# Apply migrations
alembic upgrade head

# Rollback migration
alembic downgrade -1

# Reset database (development only)
make db-reset
```

## 📚 Learning Resources

### 1. Core Technologies
- **FastAPI**: [Official Documentation](https://fastapi.tiangolo.com/)
- **SQLAlchemy**: [Official Documentation](https://docs.sqlalchemy.org/)
- **Pydantic**: [Official Documentation](https://pydantic-docs.helpmanual.io/)
- **Alembic**: [Official Documentation](https://alembic.sqlalchemy.org/)

### 2. Project-Specific Documentation
- **API Documentation**: `docs/backend/api-specs/`
- **Architecture**: `docs/backend/architecture/SYSTEM_ARCHITECTURE.md`
- **Database Schema**: `docs/backend/data/DATABASE_SCHEMA.md`
- **Security Guide**: `docs/backend/security/AUTH_SYSTEM.md`

### 3. Development Tools
- **Postman Collection**: `docs/backend/tools/postman_collection.json`
- **Insomnia Workspace**: `docs/backend/tools/INSOMNIA_WORKSPACE.json`
- **API Examples**: `docs/backend/guides/OPENAPI_EXAMPLES_GUIDE.md`

## 🛠️ Common Development Tasks

### 1. Adding New API Endpoint
```python
# 1. Create schema in app/schemas/
from pydantic import BaseModel

class NewFeatureCreate(BaseModel):
    name: str
    description: str

class NewFeatureResponse(BaseModel):
    id: int
    name: str
    description: str
    created_at: datetime

# 2. Create model in app/models/
from sqlalchemy import Column, Integer, String, DateTime
from app.db.base_class import Base

class NewFeature(Base):
    __tablename__ = "new_features"
    
    id = Column(Integer, primary_key=True, index=True)
    name = Column(String, unique=True, index=True)
    description = Column(String)
    created_at = Column(DateTime, default=datetime.utcnow)

# 3. Create CRUD operations in app/crud/
from app.crud.base import CRUDBase
from app.models.new_feature import NewFeature
from app.schemas.new_feature import NewFeatureCreate, NewFeatureUpdate

class CRUDNewFeature(CRUDBase[NewFeature, NewFeatureCreate, NewFeatureUpdate]):
    pass

new_feature = CRUDNewFeature(NewFeature)

# 4. Create service in app/services/
from app.crud.new_feature import new_feature
from app.schemas.new_feature import NewFeatureCreate

class NewFeatureService:
    @staticmethod
    async def create_feature(feature_data: NewFeatureCreate):
        return await new_feature.create(feature_data)

# 5. Create API endpoint in app/api/v1/
from fastapi import APIRouter, Depends
from app.schemas.new_feature import NewFeatureCreate, NewFeatureResponse
from app.services.new_feature import NewFeatureService

router = APIRouter()

@router.post("/new-features/", response_model=NewFeatureResponse)
async def create_new_feature(
    feature_data: NewFeatureCreate,
    current_user: User = Depends(get_current_user)
):
    return await NewFeatureService.create_feature(feature_data)

# 6. Add to main router in app/api/v1/__init__.py
from app.api.v1.new_feature import router as new_feature_router

api_router.include_router(new_feature_router, prefix="/new-features", tags=["new-features"])

# 7. Create tests
def test_create_new_feature(client, test_user):
    response = client.post(
        "/api/v1/new-features/",
        json={"name": "Test Feature", "description": "Test Description"},
        headers={"Authorization": f"Bearer {test_user.token}"}
    )
    assert response.status_code == 200
    assert response.json()["name"] == "Test Feature"
```

### 2. Adding New Database Model
```python
# 1. Create model file
# app/models/new_model.py

# 2. Add to base.py
from app.models.new_model import NewModel

# 3. Create migration
alembic revision --autogenerate -m "Add new model"

# 4. Apply migration
alembic upgrade head

# 5. Update tests
# tests/test_models/test_new_model.py
```

### 3. Adding New Service
```python
# 1. Create service file
# app/services/new_service.py

# 2. Add dependency injection
# app/dependencies.py

# 3. Use in API endpoints
# app/api/v1/new_endpoint.py

# 4. Add tests
# tests/test_services/test_new_service.py
```

## 🔍 Debugging & Troubleshooting

### 1. Common Issues
```bash
# Database connection issues
- Check database URL in .env
- Ensure database is running
- Check migration status

# Import errors
- Check virtual environment is activated
- Verify PYTHONPATH
- Check __init__.py files

# API errors
- Check request format
- Verify authentication
- Check database constraints
```

### 2. Debug Tools
```python
# Add debug logging
import logging
logger = logging.getLogger(__name__)

logger.debug("Debug message")
logger.info("Info message")
logger.warning("Warning message")
logger.error("Error message")

# Use Python debugger
import pdb; pdb.set_trace()

# Or use ipdb for better experience
import ipdb; ipdb.set_trace()
```

### 3. Performance Debugging
```python
# Profile code execution
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
```

## 📊 Monitoring & Observability

### 1. Logging
```python
# Structured logging
import structlog

logger = structlog.get_logger()

logger.info(
    "User action",
    user_id=user.id,
    action="login",
    ip_address=request.client.host
)
```

### 2. Metrics
```python
# Custom metrics
from prometheus_client import Counter, Histogram

request_counter = Counter('http_requests_total', 'Total HTTP requests')
request_duration = Histogram('http_request_duration_seconds', 'Request duration')

# Use in middleware or endpoints
request_counter.inc()
with request_duration.time():
    # Your code here
    pass
```

### 3. Health Checks
```python
# Add health check endpoint
@app.get("/health")
async def health_check():
    return {
        "status": "healthy",
        "timestamp": datetime.utcnow(),
        "version": "1.0.0"
    }
```

## 🚀 Deployment

### 1. Local Development
```bash
# Using Docker
docker-compose up -d

# Using local environment
make dev
```

### 2. Staging Environment
```bash
# Deploy to staging
make deploy-staging

# Run staging tests
make test-staging
```

### 3. Production Environment
```bash
# Deploy to production
make deploy-production

# Monitor deployment
make monitor-production
```

## 📋 Checklist for New Developers

### Week 1: Environment Setup
- [ ] Clone repository and setup development environment
- [ ] Run all tests and ensure they pass
- [ ] Read project documentation
- [ ] Understand project structure
- [ ] Setup IDE with recommended extensions

### Week 2: Code Familiarity
- [ ] Review existing codebase
- [ ] Understand API structure
- [ ] Learn database models and relationships
- [ ] Practice with development tools
- [ ] Make first small contribution

### Week 3: Feature Development
- [ ] Pick up first feature task
- [ ] Follow development workflow
- [ ] Write tests for new features
- [ ] Get code review and feedback
- [ ] Deploy to staging environment

### Week 4: Integration
- [ ] Work on integration tasks
- [ ] Participate in code reviews
- [ ] Help with documentation
- [ ] Understand monitoring and observability
- [ ] Contribute to team processes

## 🤝 Team Collaboration

### 1. Communication Channels
- **Slack**: #oht50-backend
- **Email**: backend-team@oht50.local
- **GitHub Issues**: For bug reports and feature requests
- **Pull Requests**: For code reviews and discussions

### 2. Code Review Guidelines
- Review all PRs thoroughly
- Provide constructive feedback
- Check for security issues
- Ensure tests are included
- Verify documentation updates

### 3. Team Meetings
- **Daily Standup**: 9:00 AM daily
- **Sprint Planning**: Every 2 weeks
- **Retrospective**: End of each sprint
- **Architecture Review**: Monthly

## 📚 Additional Resources

### 1. Internal Documentation
- [API Reference](../api-specs/README.md)
- [Database Schema](../data/DATABASE_SCHEMA.md)
- [Security Guide](../security/AUTH_SYSTEM.md)
- [Deployment Guide](../DEPLOYMENT.md)

### 2. External Resources
- [FastAPI Best Practices](https://fastapi.tiangolo.com/tutorial/best-practices/)
- [SQLAlchemy Best Practices](https://docs.sqlalchemy.org/en/14/orm/best_practices.html)
- [Python Type Hints](https://docs.python.org/3/library/typing.html)
- [Async Python](https://docs.python.org/3/library/asyncio.html)

### 3. Tools & Extensions
- **VS Code Extensions**:
  - Python
  - Pylance
  - FastAPI Snippets
  - SQLAlchemy Snippets
  - GitLens
  - Docker

---

**Changelog v1.0:**
- ✅ Created comprehensive onboarding guide
- ✅ Added environment setup instructions
- ✅ Included project structure overview
- ✅ Added development workflow guidelines
- ✅ Created common development tasks examples
- ✅ Added debugging and troubleshooting section
- ✅ Included monitoring and observability guide
- ✅ Added team collaboration guidelines
- ✅ Created onboarding checklist
- ✅ Added additional resources and tools
