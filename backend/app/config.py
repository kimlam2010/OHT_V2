"""
Configuration settings for OHT-50 Backend
"""

from typing import List
from pydantic_settings import BaseSettings
from pydantic import ConfigDict


class Settings(BaseSettings):
    """Application settings"""
    
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
    jwt_secret: str = "your-secret-key-here"
    jwt_algorithm: str = "HS256"
    jwt_expiry_minutes: int = 30
    
    # API
    api_prefix: str = "/api/v1"
    cors_origins: List[str] = ["http://localhost:3000", "http://localhost:8080"]
    
    # Monitoring
    enable_metrics: bool = True
    prometheus_port: int = 9090
    log_level: str = "INFO"
    
    # Redis
    redis_url: str = "redis://localhost:6379"
    
    # JWT Settings
    jwt_expiry: str = "3600"
    
    # Rate Limiting
    rate_limit_requests: str = "1000"
    rate_limit_window: str = "60"
    
    # RS485 Settings
    rs485_port: str = "/dev/ttyOHT485"
    rs485_baud_rate: str = "115200"
    rs485_timeout: str = "1.0"
    
    # Metrics
    metrics_port: str = "9090"
    
    # Firmware Integration
    firmware_url: str = "http://localhost:8081"
    firmware_websocket_url: str = "ws://localhost:8081/ws"
    firmware_timeout: int = 10
    use_firmware_mock: bool = False  # Only allowed in non-production
    
    # Performance
    max_connections: int = 100
    request_timeout: int = 30
    
    model_config = ConfigDict(env_file=".env")


# Global settings instance
settings = Settings()
