# Environment Configuration - OHT-50 Backend

## 📋 Tổng quan
Hướng dẫn cấu hình môi trường cho OHT-50 Backend, bao gồm biến môi trường, secrets management và deployment configs.

## 🔧 Environment Variables

### Core Configuration
```bash
# Application
APP_NAME=OHT-50 Backend
APP_VERSION=1.0.0
DEBUG=false
ENVIRONMENT=production

# Server
HOST=0.0.0.0
PORT=8000
WORKERS=4

# Database
DATABASE_URL=postgresql://user:password@localhost:5432/oht50_db
DATABASE_POOL_SIZE=10
DATABASE_MAX_OVERFLOW=20

# Security
SECRET_KEY=your-secret-key-here
ALGORITHM=HS256
ACCESS_TOKEN_EXPIRE_MINUTES=30
REFRESH_TOKEN_EXPIRE_DAYS=7

# Redis
REDIS_URL=redis://localhost:6379
REDIS_PASSWORD=

# External Services
CENTER_API_URL=https://center.oht50.local/api
CENTER_API_KEY=your-center-api-key

# Monitoring
PROMETHEUS_ENABLED=true
LOG_LEVEL=INFO

# Safety
SAFETY_CHECK_INTERVAL=100
EMERGENCY_STOP_TIMEOUT=100
```

### Environment-Specific Configs

#### Development
```bash
DEBUG=true
LOG_LEVEL=DEBUG
DATABASE_URL=sqlite:///./dev.db
SECRET_KEY=dev-secret-key
CENTER_API_URL=http://localhost:9000/api
PROMETHEUS_ENABLED=false
SAFETY_CHECK_INTERVAL=1000
```

#### Staging
```bash
DEBUG=false
LOG_LEVEL=INFO
DATABASE_URL=postgresql://staging_user:password@staging-db:5432/oht50_staging
CENTER_API_URL=https://staging-center.oht50.local/api
PROMETHEUS_ENABLED=true
```

#### Production
```bash
DEBUG=false
LOG_LEVEL=WARNING
DATABASE_URL=postgresql://prod_user:password@prod-db:5432/oht50_prod
CENTER_API_URL=https://center.oht50.local/api
PROMETHEUS_ENABLED=true
SAFETY_CHECK_INTERVAL=50
EMERGENCY_STOP_TIMEOUT=50
```

## 🔐 Secrets Management

### Configuration Class
```python
from pydantic import BaseSettings, Field
from typing import Optional

class EnvironmentConfig(BaseSettings):
    # Application Settings
    APP_NAME: str = "OHT-50 Backend"
    DEBUG: bool = Field(default=False, env="DEBUG")
    ENVIRONMENT: str = Field(default="development", env="ENVIRONMENT")
    
    # Database Settings
    DATABASE_URL: str = Field(..., env="DATABASE_URL")
    DATABASE_POOL_SIZE: int = Field(default=10, env="DATABASE_POOL_SIZE")
    
    # Security Settings
    SECRET_KEY: str = Field(..., env="SECRET_KEY")
    ACCESS_TOKEN_EXPIRE_MINUTES: int = Field(default=30, env="ACCESS_TOKEN_EXPIRE_MINUTES")
    
    # External Services
    CENTER_API_URL: str = Field(..., env="CENTER_API_URL")
    CENTER_API_KEY: str = Field(..., env="CENTER_API_KEY")
    
    # Monitoring Settings
    PROMETHEUS_ENABLED: bool = Field(default=True, env="PROMETHEUS_ENABLED")
    LOG_LEVEL: str = Field(default="INFO", env="LOG_LEVEL")
    
    # Safety Settings
    SAFETY_CHECK_INTERVAL: int = Field(default=100, env="SAFETY_CHECK_INTERVAL")
    EMERGENCY_STOP_TIMEOUT: int = Field(default=100, env="EMERGENCY_STOP_TIMEOUT")
    
    class Config:
        env_file = ".env"
        case_sensitive = False

# Load configuration
config = EnvironmentConfig()
```

### Secrets Manager
```python
import os
from cryptography.fernet import Fernet

class SecretsManager:
    def __init__(self, encryption_key: str = None):
        self.encryption_key = encryption_key or os.getenv("SECRETS_ENCRYPTION_KEY")
        self.fernet = Fernet(self.encryption_key.encode()) if self.encryption_key else None
    
    def get_secret(self, secret_name: str, default: str = None) -> str:
        """Get secret value from environment or secrets manager"""
        # Try environment variable first
        env_value = os.getenv(secret_name)
        if env_value:
            return self._decrypt_if_needed(env_value)
        
        # Try external secrets manager (AWS Secrets Manager, HashiCorp Vault, etc.)
        external_value = self._get_from_external_manager(secret_name)
        if external_value:
            return external_value
        
        return default
    
    def set_secret(self, secret_name: str, value: str, encrypt: bool = True):
        """Set secret value"""
        if encrypt and self.fernet:
            value = self.fernet.encrypt(value.encode()).decode()
        
        os.environ[secret_name] = value
    
    def _decrypt_if_needed(self, value: str) -> str:
        """Decrypt value if it's encrypted"""
        if self.fernet and value.startswith("encrypted:"):
            try:
                encrypted_value = value.replace("encrypted:", "")
                return self.fernet.decrypt(encrypted_value.encode()).decode()
            except Exception:
                return value
        return value
    
    def _get_from_external_manager(self, secret_name: str) -> str:
        """Get secret from external secrets manager"""
        # Implementation depends on external secrets manager
        return None

# Initialize secrets manager
secrets_manager = SecretsManager()
```

## 🚀 Deployment Configuration

### Docker Compose
```yaml
# docker-compose.yml
version: '3.8'

services:
  app:
    build: .
    ports:
      - "8000:8000"
    environment:
      - ENVIRONMENT=production
      - DATABASE_URL=postgresql://user:password@db:5432/oht50_db
      - REDIS_URL=redis://redis:6379
    depends_on:
      - db
      - redis
    volumes:
      - ./logs:/app/logs
    restart: unless-stopped

  db:
    image: postgres:15
    environment:
      - POSTGRES_DB=oht50_db
      - POSTGRES_USER=user
      - POSTGRES_PASSWORD=password
    volumes:
      - postgres_data:/var/lib/postgresql/data

  redis:
    image: redis:7-alpine
    volumes:
      - redis_data:/data

volumes:
  postgres_data:
  redis_data:
```

### Kubernetes ConfigMap
```yaml
# k8s/configmap.yaml
apiVersion: v1
kind: ConfigMap
metadata:
  name: oht50-backend-config
data:
  ENVIRONMENT: "production"
  HOST: "0.0.0.0"
  PORT: "8000"
  WORKERS: "4"
  DATABASE_POOL_SIZE: "10"
  ALGORITHM: "HS256"
  ACCESS_TOKEN_EXPIRE_MINUTES: "30"
  PROMETHEUS_ENABLED: "true"
  LOG_LEVEL: "INFO"
  SAFETY_CHECK_INTERVAL: "100"
```

### Kubernetes Secret
```yaml
# k8s/secret.yaml
apiVersion: v1
kind: Secret
metadata:
  name: oht50-backend-secrets
type: Opaque
data:
  SECRET_KEY: <base64-encoded-secret-key>
  DATABASE_URL: <base64-encoded-database-url>
  CENTER_API_KEY: <base64-encoded-center-api-key>
```

## 🔧 Configuration Management

### Configuration Loader
```python
import yaml
import os
from pathlib import Path

class ConfigurationLoader:
    def __init__(self, config_dir: str = "config"):
        self.config_dir = Path(config_dir)
    
    def load_config(self, environment: str = None) -> dict:
        """Load configuration for specified environment"""
        if not environment:
            environment = os.getenv("ENVIRONMENT", "development")
        
        # Load base configuration
        base_config = self._load_yaml_file("base.yaml")
        
        # Load environment-specific configuration
        env_config = self._load_yaml_file(f"{environment}.yaml")
        
        # Merge configurations
        config = self._merge_configs(base_config, env_config)
        
        # Override with environment variables
        config = self._override_with_env_vars(config)
        
        return config
    
    def _load_yaml_file(self, filename: str) -> dict:
        """Load YAML configuration file"""
        file_path = self.config_dir / filename
        if not file_path.exists():
            return {}
        
        with open(file_path, 'r', encoding='utf-8') as f:
            return yaml.safe_load(f) or {}
    
    def _merge_configs(self, base: dict, override: dict) -> dict:
        """Merge base configuration with override configuration"""
        result = base.copy()
        
        for key, value in override.items():
            if key in result and isinstance(result[key], dict) and isinstance(value, dict):
                result[key] = self._merge_configs(result[key], value)
            else:
                result[key] = value
        
        return result
    
    def _override_with_env_vars(self, config: dict) -> dict:
        """Override configuration with environment variables"""
        for key, value in os.environ.items():
            if key.startswith("OHT50_"):
                config_key = key.replace("OHT50_", "").lower()
                self._set_nested_value(config, config_key.split("_"), value)
        
        return config
    
    def _set_nested_value(self, config: dict, keys: list, value: str):
        """Set nested configuration value"""
        current = config
        for key in keys[:-1]:
            if key not in current:
                current[key] = {}
            current = current[key]
        
        # Convert value to appropriate type
        try:
            if value.lower() in ('true', 'false'):
                current[keys[-1]] = value.lower() == 'true'
            elif value.isdigit():
                current[keys[-1]] = int(value)
            else:
                current[keys[-1]] = value
        except ValueError:
            current[keys[-1]] = value

# Initialize configuration loader
config_loader = ConfigurationLoader()
```

### Configuration Validation
```python
from pydantic import BaseModel, validator

class DatabaseConfig(BaseModel):
    url: str
    pool_size: int = 10
    max_overflow: int = 20
    
    @validator('url')
    def validate_database_url(cls, v):
        if not v:
            raise ValueError('Database URL is required')
        return v

class SecurityConfig(BaseModel):
    secret_key: str
    algorithm: str = "HS256"
    access_token_expire_minutes: int = 30
    
    @validator('secret_key')
    def validate_secret_key(cls, v):
        if len(v) < 32:
            raise ValueError('Secret key must be at least 32 characters long')
        return v

class AppConfig(BaseModel):
    name: str
    version: str
    debug: bool = False
    environment: str
    
    database: DatabaseConfig
    security: SecurityConfig
    
    @validator('environment')
    def validate_environment(cls, v):
        valid_environments = ['development', 'staging', 'production', 'testing']
        if v not in valid_environments:
            raise ValueError(f'Environment must be one of: {valid_environments}')
        return v

def validate_config(config_dict: dict) -> AppConfig:
    """Validate configuration dictionary"""
    return AppConfig(**config_dict)
```

## 📋 Configuration Checklist

### Development Setup
- [ ] Create `.env` file with development variables
- [ ] Set `DEBUG=true`
- [ ] Configure local database URL
- [ ] Set development secret key
- [ ] Configure local external services

### Staging Setup
- [ ] Create staging environment variables
- [ ] Set `DEBUG=false`
- [ ] Configure staging database
- [ ] Set staging external service URLs
- [ ] Enable monitoring

### Production Setup
- [ ] Create production environment variables
- [ ] Set `DEBUG=false`
- [ ] Configure production database
- [ ] Set production external service URLs
- [ ] Enable all monitoring
- [ ] Configure secrets management
- [ ] Set up backup procedures

---

**Changelog v1.0:**
- ✅ Created environment configuration guide
- ✅ Added environment-specific configs
- ✅ Implemented secrets management
- ✅ Added deployment configurations
- ✅ Created configuration validation
- ✅ Added configuration checklist

