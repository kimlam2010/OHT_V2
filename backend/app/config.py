"""
Configuration settings for OHT-50 Backend
"""

from typing import List
from pydantic_settings import BaseSettings
from pydantic import ConfigDict, field_validator
import os


class Settings(BaseSettings):
    """Application settings"""
    
    model_config = ConfigDict(extra='ignore')
    
    # Application
    app_name: str = "OHT-50 Backend"
    app_version: str = "1.0.0"
    debug: bool = False
    
    # Server
    host: str = "0.0.0.0"
    port: int = 8000
    max_workers: int = 1
    environment: str = "development"  # development | staging | production
    
    # Database
    database_url: str = "sqlite+aiosqlite:///./oht50.db"
    database_echo: bool = False
    
    # Security
    secret_key: str = "your-secret-key-here"
    jwt_secret: str = ""  # Must be set via environment variable
    jwt_algorithm: str = "HS256"
    jwt_expiry_minutes: int = 30
    
    # API
    api_prefix: str = "/api/v1"
    cors_origins: List[str] = ["http://localhost:3000", "http://localhost:8080", "http://localhost:5173", "http://localhost:8000"]
    
    # Monitoring
    enable_metrics: bool = True
    prometheus_port: int = 9090
    log_level: str = "INFO"
    
    # Redis
    redis_url: str = "redis://localhost:6379"
    
    # JWT Settings (consistent with jwt_expiry_minutes)
    jwt_expiry: int = 1800  # 30 minutes in seconds
    
    # Rate Limiting
    rate_limit_requests: int = 1000
    rate_limit_window: int = 60
    
    # RS485 Settings
    rs485_port: str = "/dev/ttyOHT485"
    rs485_baud_rate: str = "115200"
    rs485_timeout: str = "1.0"
    
    # Metrics
    metrics_port: str = "9090"
    
    # Firmware Integration
    firmware_url: str = "http://localhost:8080"  # Primary HTTP API server
    firmware_url_alt: str = "http://localhost:8081"  # Alternative WebSocket server
    firmware_websocket_url: str = "ws://localhost:8081/ws"
    firmware_timeout: float = 5.0
    firmware_retry_count: int = 3
    use_mock_firmware: bool = False  # Use mock firmware (includes RS485 mock) - Only allowed in non-production
    
    # Performance
    max_connections: int = 100
    request_timeout: int = 30
    
    model_config = ConfigDict(env_file=".env", extra='ignore')
    
    @field_validator('jwt_secret')
    @classmethod
    def validate_jwt_secret(cls, v: str) -> str:
        """Validate JWT secret is set for production"""
        # Allow empty JWT secret only in testing mode
        testing_mode = os.getenv("TESTING", "false").lower() == "true"
        if not v and not testing_mode:
            raise ValueError(
                "JWT_SECRET must be set via environment variable for production. "
                "Set JWT_SECRET in your .env file or environment variables. "
                "Generate with: openssl rand -hex 32"
            )
        # Provide strong default for testing
        if not v and testing_mode:
            return "test-jwt-secret-key-for-testing-only-not-for-production-use"
        return v


# Global settings instance
settings = Settings()
